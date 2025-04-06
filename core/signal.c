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

#include "signal.h"

#if defined(OS_WINDOWS)
#include <windows.h>
#endif

// Static variables for storing the child process ID and signal handler
static pid_t __CHILD_PID = -1;
static signal_handler_fn __SIGNAL_HANDLER_FN = NULL;

void register_signal_handler(signal_handler_fn handler) {
    __SIGNAL_HANDLER_FN = handler;

    // Set up standard signal handlers
    signal(SIGINT, __SIGNAL_HANDLER_FN);
    signal(SIGTERM, __SIGNAL_HANDLER_FN);

// SIGQUIT is not available on Windows
#if !defined(OS_WINDOWS)
    signal(SIGQUIT, __SIGNAL_HANDLER_FN);
#endif
}

void set_child_pid(pid_t pid) { __CHILD_PID = pid; }

pid_t get_child_pid(void) { return __CHILD_PID; }
