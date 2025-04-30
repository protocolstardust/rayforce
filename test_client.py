import socket
import struct
import sys
import time
import threading
import random
from concurrent.futures import ThreadPoolExecutor
from statistics import mean, stdev
import queue

# Constants
HOST = 'localhost'
PORT = 5101
RAYFORCE_VERSION = 1  # This should match the server's version
SERDE_PREFIX = 0xcefadefa  # Magic number from server

# Test configuration
NUM_THREADS = 3  # Reduced number of concurrent threads for debugging
REQUESTS_PER_THREAD = 10  # Reduced requests per thread
TIMEOUT = 30.0  # Increased timeout
CONNECTION_RETRY_DELAY = 1.0  # Delay between connection retries

# Statistics
success_count = 0
failure_count = 0
response_times = []
stats_lock = threading.Lock()
result_queue = queue.Queue()

def create_header(msg_type, size):
    """Create a message header with type and size."""
    # Match server's ipc_header_t structure:
    # u32_t prefix (0xcefadefa)
    # u8_t version (1)
    # u8_t flags (0)
    # u8_t endian (0 for little)
    # u8_t msgtype (0=async, 1=sync, 2=response)
    # i64_t size (signed 64-bit integer)
    header = bytearray(16)  # Total header size is 16 bytes
    
    # Pack prefix (4 bytes, little-endian)
    header[0:4] = struct.pack('<I', SERDE_PREFIX)
    
    # Pack version (1 byte)
    header[4] = RAYFORCE_VERSION
    
    # Pack flags (1 byte)
    header[5] = 0
    
    # Pack endian (1 byte)
    header[6] = 0
    
    # Pack msgtype (1 byte)
    header[7] = msg_type
    
    # Pack size (8 bytes, signed 64-bit integer, little-endian)
    header[8:16] = struct.pack('<q', size)
    
    print(f"Header bytes: {header.hex()}")
    print(f"Size in bytes: {header[8:16].hex()}")
    return bytes(header)

def send_message(sock, msg_type, message):
    """Send a message with header."""
    try:
        # The server's serialization format for a list:
        # 1. Header (16 bytes)
        # 2. Message body:
        #    - Type byte (9 for list)
        #    - List length (8 bytes)
        #    - For each element:
        #      - Element length (8 bytes)
        #      - Element content
        msg_type_byte = struct.pack('B', 9)  # Type 9 for list
        
        # For a list with one string element
        list_length = struct.pack('Q', 1)  # List has 1 element
        element_length = struct.pack('Q', len(message))  # Length of the string element
        
        # Construct message body
        message_body = msg_type_byte + list_length + element_length + message.encode()
        total_size = len(message_body)
        
        print(f"Message body size: {total_size}")
        print(f"Message body bytes: {message_body.hex()}")
        
        # Create and send header
        header = create_header(msg_type, total_size)
        print(f"Sending header: {header.hex()}")
        sock.sendall(header)
        
        # Send message body
        print(f"Sending message: {message}")
        sock.sendall(message_body)
    except Exception as e:
        print(f"Error sending message: {e}")
        raise

def receive_response(sock):
    """Receive and process a response."""
    try:
        # Set timeout
        sock.settimeout(TIMEOUT)
        
        # First read the header (16 bytes)
        print("Waiting for header...")
        header_data = sock.recv(16)
        if len(header_data) != 16:
            print(f"Error: Invalid header received (length: {len(header_data)})")
            if len(header_data) > 0:
                print(f"Received data: {header_data.hex()}")
            return None
            
        # Unpack header
        prefix = struct.unpack('<I', header_data[0:4])[0]
        version = header_data[4]
        flags = header_data[5]
        endian = header_data[6]
        msg_type = header_data[7]
        size = struct.unpack('<q', header_data[8:16])[0]
        
        print(f"Received header: prefix=0x{prefix:08x}, version={version}, type={msg_type}, size={size}")
        
        if size == 0:
            return ""
            
        # Read the message body
        print(f"Reading message body ({size} bytes)...")
        data = b''
        while len(data) < size:
            chunk = sock.recv(size - len(data))
            if not chunk:
                print("Connection closed by server")
                break
            data += chunk
            print(f"Received chunk: {chunk.hex()}")
            
        # Parse the response
        if len(data) >= 9:  # type (1) + length (8)
            msg_type = data[0]
            msg_length = struct.unpack('Q', data[1:9])[0]
            if len(data) >= 9 + msg_length:
                return data[9:9+msg_length].decode()
            
        return None
    except socket.timeout:
        print("Timeout waiting for response")
        return None
    except Exception as e:
        print(f"Error in receive_response: {e}")
        return None

def generate_test_expression():
    """Generate a random arithmetic expression for testing."""
    ops = ['+', '-', '*', '/']
    op = random.choice(ops)
    a = random.randint(1, 100)
    b = random.randint(1, 100)
    return f"({op} {a} {b})"

def worker(thread_id):
    """Worker function for each thread."""
    sock = None
    retry_count = 0
    max_retries = 3
    
    while retry_count < max_retries:
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.setsockopt(socket.SOL_SOCKET, socket.SO_KEEPALIVE, 1)
            sock.settimeout(TIMEOUT)
            
            print(f"Thread {thread_id}: Connecting to server (attempt {retry_count + 1})...")
            sock.connect((HOST, PORT))
            
            # Perform handshake
            handshake = bytes([RAYFORCE_VERSION, 0x00])
            print(f"Thread {thread_id}: Sending handshake...")
            sock.sendall(handshake)
            
            response = sock.recv(2)
            if len(response) != 2:
                print(f"Thread {thread_id}: Error: Invalid handshake response")
                raise Exception("Invalid handshake response")
            
            print(f"Thread {thread_id}: Handshake successful, starting requests...")
            
            for i in range(REQUESTS_PER_THREAD):
                # Generate random expression
                expr = generate_test_expression()
                
                # Send request and measure time
                start_time = time.time()
                print(f"Thread {thread_id}: Sending request {i+1}/{REQUESTS_PER_THREAD}: {expr}")
                send_message(sock, 1, expr)
                
                response = receive_response(sock)
                end_time = time.time()
                
                response_time = end_time - start_time
                
                with stats_lock:
                    if response is not None:  # Changed from if response to handle empty responses
                        success_count += 1
                        response_times.append(response_time)
                        print(f"Thread {thread_id}: Request {i+1} successful: {expr} -> {response} (time: {response_time:.3f}s)")
                    else:
                        failure_count += 1
                        print(f"Thread {thread_id}: Request {i+1} failed: {expr} (timeout)")
                
                # Small delay between requests
                time.sleep(0.1)
            
            # If we get here, all requests were processed
            break
            
        except socket.timeout:
            print(f"Thread {thread_id}: Socket timeout, retrying...")
            retry_count += 1
            time.sleep(CONNECTION_RETRY_DELAY)
        except Exception as e:
            print(f"Thread {thread_id}: Error: {str(e)}")
            retry_count += 1
            time.sleep(CONNECTION_RETRY_DELAY)
        finally:
            if sock:
                try:
                    sock.close()
                except:
                    pass
    
    if retry_count >= max_retries:
        print(f"Thread {thread_id}: Failed after {max_retries} retries")

def print_stats():
    """Print test statistics."""
    print("\n=== Test Statistics ===")
    print(f"Total Requests: {success_count + failure_count}")
    print(f"Successful Requests: {success_count}")
    print(f"Failed Requests: {failure_count}")
    print(f"Success Rate: {(success_count / (success_count + failure_count) * 100):.2f}%")
    
    if response_times:
        print(f"Average Response Time: {mean(response_times):.3f}s")
        print(f"Min Response Time: {min(response_times):.3f}s")
        print(f"Max Response Time: {max(response_times):.3f}s")
        print(f"Response Time StdDev: {stdev(response_times):.3f}s")

def main():
    print(f"Starting stress test with {NUM_THREADS} threads, {REQUESTS_PER_THREAD} requests per thread")
    print(f"Total requests: {NUM_THREADS * REQUESTS_PER_THREAD}")
    
    start_time = time.time()
    
    # Start worker threads
    with ThreadPoolExecutor(max_workers=NUM_THREADS) as executor:
        futures = [executor.submit(worker, i) for i in range(NUM_THREADS)]
        
        # Print results as they come in
        results_received = 0
        while results_received < NUM_THREADS * REQUESTS_PER_THREAD:
            try:
                thread_id, result = result_queue.get(timeout=1.0)
                print(f"Thread {thread_id}: {result}")
                results_received += 1
            except queue.Empty:
                continue
    
    end_time = time.time()
    total_time = end_time - start_time
    
    print_stats()
    print(f"\nTotal Test Time: {total_time:.2f}s")
    print(f"Requests per Second: {(NUM_THREADS * REQUESTS_PER_THREAD) / total_time:.2f}")

if __name__ == "__main__":
    main() 