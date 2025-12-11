# :material-remote: IPC

RayforceDB can communicate with other RayforceDB instances via IPC (Inter-Process Communication). This provides a fast and efficient way to send data between processes, enabling distributed computing and client-server architectures.

!!! warning ""
    IPC functionality is currently unstable and may be subject to changes in the future.

## Listening to a Port

To start a RayforceDB process that listens on a port, use the `-p` flag:

```bash
rayforce -p 5110
```

The process will listen for incoming connections on the specified port.

## Connecting to a Remote Process

### Hopen

Opens a handle to a file or IPC connection.

```clj
↪ (set h (hopen "127.0.0.1:5100"))
↪ (set h (hopen "localhost:5110"))
↪ (set h (hopen "/tmp/log"))
↪ (set h (hopen "/tmp/log" 5000))
```

For IPC connections, provide a hostname:port string (supports both hostnames and IP addresses). For files, provide a file path. Optionally accepts a timeout value (in milliseconds) for IPC connections.

Once connected, you can send data to the remote process using `write`:

```clj
↪ (write h "(+ 1 2)")
3
↪ (write h (list (+ 1 2)))
3
```

### Hclose

Closes a handle opened with `hopen`.

```clj
↪ (hclose h)
```

!!! note ""
    Always close handles when done to free resources.

## Accessing Server Variables

When referring to variables that exist only on the server runtime, ensure they are not evaluated on the client side.

For example, if a table exists only on the server and is named `employees`:

```clj
;; WRONG - employees is resolved on the client
(write h employees)  ;; Error!
```

Instead, pass the variable name as a quoted symbol so it can be resolved on the server:

```clj
;; CORRECT - symbol is sent as-is and evaluated server-side
(write h 'employees)
```

## Message Format

There are two ways of sending IPC messages:

1. **String**: The string is parsed and evaluated on the remote side
2. **List**: The list is evaluated as-is on the remote side

```clj
↪ (write h "(+ 1 2)")        ;; String message
3
↪ (write h (list + 1 2))     ;; List message
3
```

## Message Types

### Sync Messages

Sync messages are used to send a request and get a response. Sync messages are blocking - the sender waits for the response.

```clj
↪ (write h "(+ 1 2)")
3
```

### Async Messages

Async messages are sent without waiting for a response. To send an async message, negate the connection handle:

```clj
↪ (write (neg h) (list (+ 1 2)))
```

Async messages are non-blocking, allowing the sender to continue immediately.

## Protocol

The IPC protocol uses serialization to send and receive messages. The protocol includes a handshake phase to negotiate version and credentials.

### Handshake

After a client opens a connection to a server, the first message sent by the client is a handshake message. It consists of a null-terminated ASCII string followed by a version number byte in the format: `[username:password]v`

- `username:password` is optional and can be empty `[]`
- `v` is a single byte version number: `(RAYFORCE_MAJOR_VERSION << 3 | RAYFORCE_MINOR_VERSION)`

The server validates the handshake and responds with:
- Success: `\x01` (1 byte)
- Failure: `\x00` (1 byte) followed by an error message

Example handshake messages:
```
[]v                    ;; No credentials, just version
[admin:secret123]v     ;; With credentials
```

### Message Header

Each IPC message consists of a header followed by serialized data. The header format is:

```
prefix(4) version(1) flags(1) endian(1) msgtype(1) size(8)
```

- `prefix`: Magic number `0xcefadefa`
- `version`: Protocol version
- `flags`: Message flags (0 = no flags)
- `endian`: Endianness indicator (0 = little, 1 = big)
- `msgtype`: Message type (0 = sync, 1 = response, 2 = async)
- `size`: Total message size in bytes

### Data Serialization

Messages use the same serialization format as the `ser` function. All numeric values are in network byte order (big-endian). String and symbol data is UTF-8 encoded.
