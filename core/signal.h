/*
 *   Copyright (c) 2025 Anton Kundenko <singaraiona@gmail.com>
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

#ifndef RAYFORCE_SIGNAL_H
#define RAYFORCE_SIGNAL_H

#include <signal.h>
#include <sys/types.h>

// Type definition for signal handler function pointer
typedef void (*signal_handler_fn)(int);

/**
 * Registers a signal handler for SIGINT, SIGTERM, and SIGQUIT
 * @param handler Function pointer to the signal handler
 */
void register_signal_handler(signal_handler_fn handler);

/**
 * Sets the child process ID
 * @param pid Process ID of the child process
 */
void set_child_pid(pid_t pid);

/**
 * Gets the current child process ID
 * @return Current child process ID, or -1 if no child process
 */
pid_t get_child_pid(void);

#endif /* RAYFORCE_SIGNAL_H */
