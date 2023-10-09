/*
 *   Copyright (c) 2023 Anton Kundenko <singaraiona@gmail.com>
 *   All rights reserved.

 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:

 *   The above copyright notice and this permission notice shall be included in all
 *   copies or substantial portions of the Software.

 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *   SOFTWARE.
 */

#if defined(_WIN32) || defined(__CYGWIN__)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <MSWSock.h>
#include <windows.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "poll.h"
#include "string.h"
#include "hash.h"
#include "format.h"
#include "util.h"
#include "heap.h"

// Definitions and globals
#define STDIN_WAKER_ID ~0ull
#define STDIN_BUFFER_SIZE 256
#define MAX_IOCP_RESULTS 64

typedef struct listener_t
{
    u8_t buf[(sizeof(SOCKADDR_IN) + 16) * 2];
    OVERLAPPED overlapped;
    DWORD dwBytes;
    SOCKET hAccepted;
} *listener_t;

char_t __STDIN_BUF[STDIN_BUFFER_SIZE];
listener_t __LISTENER;

DWORD WINAPI StdinThread(LPVOID lpParam)
{
    HANDLE hCompletionPort = (HANDLE)lpParam;
    DWORD bytesRead;
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);

    while (true)
    {
        // Blocking read from stdin
        if (ReadFile(hStdin, __STDIN_BUF, STDIN_BUFFER_SIZE, &bytesRead, NULL))
        {
            __STDIN_BUF[bytesRead] = '\0';
            // Check for CTRL+C (ASCII code 3)
            if (bytesRead == 1 && __STDIN_BUF[0] == 0x03)
                PostQueuedCompletionStatus(hCompletionPort, 0, STDIN_WAKER_ID, NULL);
            else
                PostQueuedCompletionStatus(hCompletionPort, bytesRead, STDIN_WAKER_ID, NULL);
        }
    }

    return 0;
}

nil_t exit_werror()
{
    obj_t err;
    str_t fmt;

    err = sys_error(TYPE_WSAGETLASTERROR, "poll_init");
    fmt = obj_fmt(err);
    printf("%s\n", fmt);
    heap_free(fmt);
    drop(err);
    fflush(stdout);
    exit(1);
}

i64_t poll_accept(poll_t poll)
{
    i32_t code;
    LPFN_ACCEPTEX lpfnAcceptEx = NULL;
    GUID GuidAcceptEx = WSAID_ACCEPTEX;
    DWORD dwBytes;
    SOCKET sock_fd = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    OVERLAPPED overlapped = {0};

    if (sock_fd == INVALID_SOCKET)
        return -1;

    // Ensure the accept socket is associated with the IOCP.
    CreateIoCompletionPort((HANDLE)sock_fd, (HANDLE)poll->poll_fd, NULL, 0);

    // Load AcceptEx function
    if (WSAIoctl(poll->ipc_fd, SIO_GET_EXTENSION_FUNCTION_POINTER,
                 &GuidAcceptEx, sizeof(GuidAcceptEx),
                 &lpfnAcceptEx, sizeof(lpfnAcceptEx),
                 &dwBytes, NULL, NULL) == SOCKET_ERROR)
    {
        code = WSAGetLastError();
        closesocket(sock_fd);
        WSASetLastError(code);
        return -1;
    }

    BOOL success = lpfnAcceptEx(poll->ipc_fd, sock_fd,
                                __LISTENER->buf, 0,
                                sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
                                &__LISTENER->dwBytes, &__LISTENER->overlapped);
    if (!success)
    {
        code = WSAGetLastError();
        if (code != ERROR_IO_PENDING)
        {
            code = WSAGetLastError();
            closesocket(sock_fd);
            WSASetLastError(code);
            return -1;
        }
    }

    __LISTENER->hAccepted = sock_fd;

    return (i64_t)sock_fd;
}

poll_t poll_init(i64_t port)
{
    WSADATA wsaData;
    i64_t ipc_fd = -1, poll_fd = -1;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        exit_werror();

    poll_fd = (i64_t)CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

    if (!poll_fd)
        exit_werror();

    // Create a thread to read from stdin
    CreateThread(NULL, 0, StdinThread, (HANDLE)poll_fd, 0, NULL);

    poll_t poll = (poll_t)heap_alloc(sizeof(struct poll_t));

    poll->poll_fd = poll_fd;

    if (port)
    {
        ipc_fd = sock_listen(port);
        if (ipc_fd == -1)
        {
            heap_free(poll);
            exit_werror();
        }

        poll->ipc_fd = ipc_fd;
        CreateIoCompletionPort((HANDLE)ipc_fd, (HANDLE)poll_fd, ipc_fd, 0);

        __LISTENER = (listener_t)heap_alloc(sizeof(struct listener_t));
        memset(__LISTENER, 0, sizeof(struct listener_t));

        if (poll_accept(poll) == -1)
        {
            heap_free(poll);
            exit_werror();
        }
    }

    poll->selectors = freelist_new(128);

    return poll;
}

nil_t poll_cleanup(poll_t poll)
{
    i64_t i, l;

    if (poll->ipc_fd != -1)
        CloseHandle((HANDLE)poll->ipc_fd);

    // Free all selectors
    l = poll->selectors->data_pos;
    for (i = 0; i < l; i++)
    {
        if (poll->selectors->data[i] != NULL_I64)
            poll_deregister(poll, i + SELECTOR_ID_OFFSET);
    }

    freelist_free(poll->selectors);

    CloseHandle((HANDLE)poll->poll_fd);
    heap_free(poll);

    WSACleanup();

    printf("\nBye.\n");
    fflush(stdout);
}

i64_t poll_register(poll_t poll, i64_t fd, u8_t version)
{
    i64_t id;
    selector_t selector;

    selector = heap_alloc(sizeof(struct selector_t));
    id = freelist_push(poll->selectors, (i64_t)selector) + SELECTOR_ID_OFFSET;
    selector->id = id;
    selector->version = version;
    selector->fd = fd;
    selector->rx.flags = 0;
    selector->rx.wsa_buf.buf = NULL;
    selector->rx.wsa_buf.len = 0;
    selector->rx.bytes_transfered = 0;
    selector->tx.flags = 0;
    selector->tx.wsa_buf.buf = NULL;
    selector->tx.wsa_buf.len = 0;
    selector->tx.bytes_transfered = 0;
    selector->tx.queue = queue_new(TX_QUEUE_SIZE);

    CreateIoCompletionPort((HANDLE)fd, (HANDLE)poll->poll_fd, (ULONG_PTR)selector, 0);

    return id;
}

nil_t poll_deregister(poll_t poll, i64_t id)
{
    i64_t idx;
    selector_t selector;

    idx = freelist_pop(poll->selectors, id - SELECTOR_ID_OFFSET);

    if (idx == NULL_I64)
        return;

    selector = (selector_t)idx;

    CloseHandle((HANDLE)selector->fd);

    heap_free(selector->rx.wsa_buf.buf);
    heap_free(selector->tx.wsa_buf.buf);
    queue_free(&selector->tx.queue);
    heap_free(selector);
}

// i64_t poll_recv_handshake(poll_t poll, ipc_data_t data)
// {
//     WSABUF wsaBuf;
//     DWORD dsize = 0, flags = 0;
//     i32_t error, result, size = 0;
//     u8_t handshake[2] = {0, RAYFORCE_VERSION};

//     if (data->rx.buf == NULL)
//     {
//         data->rx.size = 128;
//         data->rx.buf = heap_alloc(data->rx.size);
//     }

//     while (data->rx.read_size == 0 || data->rx.buf[data->rx.read_size - 1] != '\0')
//     {
//         size = sock_recv(data->fd, &data->rx.buf[data->rx.read_size], 1);

//         if (size == -1)
//             return POLL_ERROR;

//         data->rx.read_size += size;
//     }

//     // read version after creds
//     size = 0;
//     while (true)
//     {
//         size = sock_recv(data->fd, &data->rx.buf[data->rx.read_size], 1);
//         if (size == -1)
//             return POLL_ERROR;

//         if (size == 1)
//             break;
//     }

//     data->rx.version = data->rx.buf[data->rx.read_size];

//     // Queue up the read header
//     wsaBuf.buf = (char_t *)data->rx.buf;
//     wsaBuf.len = sizeof(struct header_t);
//     result = WSARecv(data->fd, &wsaBuf, 1, &dsize, &flags, &data->rx.overlapped, NULL);

//     if (result == SOCKET_ERROR)
//     {
//         error = WSAGetLastError();
//         if (error != ERROR_IO_PENDING)
//             return POLL_ERROR;
//     }

//     // send handshake back
//     size = 0;
//     while (true)
//     {
//         size += sock_send(data->fd, handshake, 2 - size);
//         if (size == 2)
//             break;
//         else if (size == -1)
//             return POLL_ERROR;
//     }

//     data->rx.read_size = 0;
//     data->rx.size = 0;

//     return POLL_PENDING;
// }

i64_t poll_run(poll_t poll)
{
    DWORD i, num = 5, size;
    OVERLAPPED *overlapped;
    HANDLE hPollFd = (HANDLE)poll->poll_fd, hAccepted;
    OVERLAPPED_ENTRY events[MAX_IOCP_RESULTS];
    BOOL success;
    i64_t key, poll_result, idx;
    obj_t res, v;
    str_t fmt;
    bool_t running = true;
    selector_t selector;

    prompt();

    while (running)
    {
        success = GetQueuedCompletionStatusEx(
            hPollFd,
            events,
            MAX_IOCP_RESULTS,
            &num,
            INFINITE,
            TRUE // set this to TRUE if you want to return on alertable wait
        );

        // Handle IOCP events
        if (success)
        {
            for (i = 0; i < num; ++i)
            {
                key = events[i].lpCompletionKey;
                size = events[i].dwNumberOfBytesTransferred;
                overlapped = events[i].lpOverlapped;

                switch (key)
                {
                case STDIN_WAKER_ID:
                    if (size == 0)
                    {
                        running = false;
                        break;
                    }
                    res = eval_str(0, "stdin", __STDIN_BUF);
                    if (res)
                    {
                        fmt = obj_fmt(res);
                        printf("%s\n", fmt);
                        heap_free(fmt);
                        drop(res);
                    }

                    prompt();
                    break;

                default:
                    // Accept new connection
                    if (key == poll->ipc_fd)
                    {
                        hAccepted = __LISTENER->hAccepted;

                        if (hAccepted != INVALID_SOCKET)
                        {
                            idx = poll_register(poll, hAccepted, 0);
                            selector = (selector_t)freelist_get(poll->selectors, idx - SELECTOR_ID_OFFSET);
                            selector->rx.wsa_buf.buf = heap_alloc(128);
                            selector->rx.wsa_buf.len = 128;
                            WSARecv(hAccepted, &selector->rx.wsa_buf, 1, &selector->rx.bytes_transfered,
                                    &selector->rx.flags, &selector->rx.overlapped, NULL);
                        }

                        poll_accept(poll);
                        break;
                    }

                    selector = (selector_t)key;

                    // Connection closed
                    if (size == 0)
                    {
                        poll_deregister(poll, selector->id);
                        break;
                    }

                    // recv
                    if (overlapped == &(selector->rx.overlapped))
                    {
                        debug("READ!!!");
                        // data->rx.read_size += size;
                        // // completed
                        // if (data->rx.read_size == data->rx.size && data->rx.size > 0)
                        // {
                        //     res = de_raw(data->rx.buf, data->rx.size);
                        //     data->rx.read_size = 0;
                        //     data->rx.size = 0;
                        //     heap_free(data->rx.buf);
                        //     data->rx.buf = NULL;

                        //     if (res->type == TYPE_CHAR)
                        //     {
                        //         v = eval_str(0, "ipc", as_string(res));
                        //         drop(res);
                        //     }
                        //     else
                        //         v = eval_obj(0, "ipc", res);

                        //     // sync request
                        //     if (data->msgtype == MSG_TYPE_SYNC)
                        //     {
                        //         ipc_enqueue_msg(data, v, MSG_TYPE_RESP);
                        //         poll_result = poll_send(poll, data, false);

                        //         if (poll_result == POLL_ERROR)
                        //             perror("send reply");
                        //     }
                        //     else
                        //         drop(v);
                        // }
                        // // setup next recv
                        // poll_result = poll_recv(poll, data, false);
                        // if (poll_result == POLL_ERROR)
                        // {
                        //     poll_del(poll, key);
                        //     res = sys_error(TYPE_WSAGETLASTERROR, "ipc: recv request");
                        //     fmt = obj_fmt(res);
                        //     printf("%s\n", fmt);
                        //     heap_free(fmt);
                        //     drop(res);
                        // }
                    }
                    // send
                    else if (overlapped == &(selector->tx.overlapped))
                    {
                        debug("WRITE!!");
                        // data->tx.write_size += size;

                        // // setup next send
                        // poll_result = poll_send(poll, data, false);

                        // if (poll_result == POLL_ERROR)
                        //     perror("send reply");
                    }

                    break;
                }
            }
        }
        else
        {
            res = sys_error(TYPE_WSAGETLASTERROR, "poll_init");
            fmt = obj_fmt(res);
            printf("%s\n", fmt);
            heap_free(fmt);
            drop(res);
        }
    }

    return 0;
}

// i64_t poll_recv_block(poll_t poll, ipc_data_t data)
// {
//     WSABUF wsaBuf;
//     DWORD dsize = 0, numberOfBytes, flags = 0;
//     ULONG_PTR completionKey;
//     LPOVERLAPPED lpOverlapped;
//     i32_t error, result;

//     // read header
//     while (true)
//     {
//         wsaBuf.buf = (char_t *)&data->rx.buf[data->rx.read_size];
//         wsaBuf.len = sizeof(struct header_t) - data->rx.read_size;
//         result = WSARecv(data->fd, &wsaBuf, 1, &dsize, &flags, &data->rx.overlapped, NULL);

//         if (result == SOCKET_ERROR)
//         {
//             error = WSAGetLastError();
//             if (error != ERROR_IO_PENDING)
//                 return POLL_ERROR;
//         }

//         if (!GetQueuedCompletionStatus((HANDLE)poll->poll_fd, &numberOfBytes, &completionKey, &lpOverlapped, INFINITE))
//             return POLL_ERROR;

//         if (lpOverlapped == &data->rx.overlapped)
//         {
//             data->rx.read_size += numberOfBytes;
//             if (data->rx.read_size == sizeof(struct header_t))
//                 break;
//         }
//     }

//     data->msgtype = ((header_t *)data->rx.buf)->msgtype;
//     data->rx.size = sizeof(struct header_t) + ((header_t *)data->rx.buf)->size;
//     data->rx.buf = heap_realloc(data->rx.buf, data->rx.size);

//     while (true)
//     {
//         wsaBuf.buf = (char_t *)&data->rx.buf[data->rx.read_size];
//         wsaBuf.len = data->rx.size - data->rx.read_size;
//         result = WSARecv(data->fd, &wsaBuf, 1, &dsize, &flags, &data->rx.overlapped, NULL);

//         if (result == SOCKET_ERROR)
//         {
//             error = WSAGetLastError();
//             if (error != ERROR_IO_PENDING)
//                 return POLL_ERROR;
//         }

//         if (!GetQueuedCompletionStatus((HANDLE)poll->poll_fd, &numberOfBytes, &completionKey, &lpOverlapped, INFINITE))
//             return POLL_ERROR;

//         if (lpOverlapped == &data->rx.overlapped)
//         {
//             data->rx.read_size += numberOfBytes;
//             if (data->rx.read_size == data->rx.size)
//                 break;
//         }
//     }

//     return POLL_DONE;
// }

// i64_t poll_recv_nonblock(poll_t poll, ipc_data_t data)
// {
//     WSABUF wsaBuf;
//     DWORD dsize = 0, flags = 0;
//     i32_t error, result;

//     // read header
//     if (data->rx.size == 0)
//     {
//         if (data->rx.read_size == (i64_t)sizeof(struct header_t))
//         {
//             data->msgtype = ((header_t *)data->rx.buf)->msgtype;
//             data->rx.size = sizeof(struct header_t) + ((header_t *)data->rx.buf)->size;
//             data->rx.buf = heap_realloc(data->rx.buf, data->rx.size);
//         }
//         else
//         {
//             wsaBuf.buf = (char_t *)&data->rx.buf[data->rx.read_size];
//             wsaBuf.len = sizeof(struct header_t) - data->rx.read_size;
//             result = WSARecv(data->fd, &wsaBuf, 1, &dsize, &flags, &data->rx.overlapped, NULL);

//             if (result == SOCKET_ERROR)
//             {
//                 error = WSAGetLastError();
//                 if (error == ERROR_IO_PENDING)
//                     return POLL_PENDING;

//                 return POLL_ERROR;
//             }

//             return POLL_PENDING;
//         }
//     }

//     // read body
//     if (data->rx.read_size < data->rx.size)
//     {
//         wsaBuf.buf = (char_t *)&data->rx.buf[data->rx.read_size];
//         wsaBuf.len = data->rx.size - data->rx.read_size;
//         result = WSARecv(data->fd, &wsaBuf, 1, &dsize, &flags, &data->rx.overlapped, NULL);

//         if (result == SOCKET_ERROR)
//         {
//             error = WSAGetLastError();
//             if (error == ERROR_IO_PENDING)
//                 return POLL_PENDING;

//             return POLL_ERROR;
//         }

//         return POLL_PENDING;
//     }

//     return POLL_DONE;
// }

// i64_t poll_recv(poll_t poll, ipc_data_t data, bool_t block)
// {
//     return block ? poll_recv_block(poll, data) : poll_recv_nonblock(poll, data);
// }

// i64_t poll_send_block(poll_t poll, ipc_data_t data)
// {
//     i64_t size;
//     obj_t obj;
//     nil_t *v;
//     i8_t msg_type = MSG_TYPE_RESP;
//     WSABUF wsaBuf;
//     DWORD dsize = 0, numberOfBytes;
//     ULONG_PTR completionKey;
//     LPOVERLAPPED lpOverlapped;
//     i32_t result, error;

// send:
//     while (true)
//     {
//         wsaBuf.buf = (char_t *)&data->tx.buf[data->tx.write_size];
//         wsaBuf.len = data->tx.size - data->tx.write_size;
//         result = WSASend(data->fd, &wsaBuf, 1, &dsize, 0, &data->tx.overlapped, NULL);

//         if (result == SOCKET_ERROR)
//         {
//             error = WSAGetLastError();
//             if (error != ERROR_IO_PENDING)
//                 return POLL_ERROR;
//         }

//         if (!GetQueuedCompletionStatus((HANDLE)poll->poll_fd, &numberOfBytes, &completionKey, &lpOverlapped, INFINITE))
//             return POLL_ERROR;

//         if (lpOverlapped == &data->tx.overlapped)
//         {
//             data->tx.write_size += numberOfBytes;
//             if (data->tx.write_size == data->tx.size)
//                 break;
//         }
//     }

//     v = queue_pop(&data->tx.queue);

//     if (v)
//     {
//         obj = (obj_t)((i64_t)v & ~(3ll << 61));
//         msg_type = (((i64_t)v & (3ll << 61)) >> 61);
//         size = ser_raw(&data->tx.buf, obj);
//         drop(obj);
//         if (size == -1)
//             return POLL_ERROR;

//         data->tx.size = size;
//         data->tx.write_size = 0;
//         ((header_t *)data->tx.buf)->msgtype = msg_type;
//         goto send;
//     }

//     heap_free(data->tx.buf);
//     data->tx.buf = NULL;
//     data->tx.write_size = 0;
//     data->tx.size = 0;

//     return POLL_DONE;
// }

// i64_t poll_send_nonblock(poll_t poll, ipc_data_t data)
// {
//     i64_t size;
//     obj_t obj;
//     nil_t *v;
//     i8_t msg_type = MSG_TYPE_RESP;
//     WSABUF wsaBuf;
//     DWORD dsize = 0;
//     i32_t result, error;

// send:
//     if (data->tx.write_size < data->tx.size)
//     {
//         wsaBuf.buf = (char_t *)&data->tx.buf[data->tx.write_size];
//         wsaBuf.len = data->tx.size - data->tx.write_size;
//         result = WSASend(data->fd, &wsaBuf, 1, &dsize, 0, &data->tx.overlapped, NULL);

//         if (result == SOCKET_ERROR)
//         {
//             error = WSAGetLastError();
//             if (error == ERROR_IO_PENDING)
//                 return POLL_PENDING;

//             return POLL_ERROR;
//         }

//         return POLL_PENDING;
//     }

//     v = queue_pop(&data->tx.queue);

//     if (v)
//     {
//         obj = (obj_t)((i64_t)v & ~(3ll << 61));
//         msg_type = (((i64_t)v & (3ll << 61)) >> 61);
//         size = ser_raw(&data->tx.buf, obj);
//         drop(obj);
//         if (size == -1)
//             return POLL_ERROR;

//         data->tx.size = size;
//         data->tx.write_size = 0;
//         ((header_t *)data->tx.buf)->msgtype = msg_type;
//         goto send;
//     }

//     heap_free(data->tx.buf);
//     data->tx.buf = NULL;
//     data->tx.write_size = 0;
//     data->tx.size = 0;

//     return POLL_DONE;
// }

// i64_t poll_send(poll_t poll, ipc_data_t data, bool_t block)
// {
//     return block ? poll_send_block(poll, data) : poll_send_nonblock(poll, data);
// }

obj_t ipc_send_sync(poll_t poll, i64_t id, obj_t msg)
{
    return NULL;
}

obj_t ipc_send_async(poll_t poll, i64_t id, obj_t msg)
{
    return NULL;
}