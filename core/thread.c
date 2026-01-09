/*
 *   Copyright (c) 2024 Anton Kundenko <singaraiona@gmail.com>
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

#include "thread.h"
#include "util.h"

#if defined(OS_WINDOWS)

// Mutex functions
mutex_t mutex_create() {
    mutex_t m;
    InitializeCriticalSection(&m.inner);
    return m;
}

nil_t mutex_destroy(mutex_t *mutex) { DeleteCriticalSection(&mutex->inner); }

nil_t mutex_lock(mutex_t *mutex) { EnterCriticalSection(&mutex->inner); }

nil_t mutex_unlock(mutex_t *mutex) { LeaveCriticalSection(&mutex->inner); }

// Condition variable functions
cond_t cond_create() {
    cond_t c;
    InitializeConditionVariable(&c.inner);
    return c;
}

nil_t cond_destroy(cond_t *cond) {
    // Windows does not require destruction of condition variables
    UNUSED(cond);
}

i32_t cond_wait(cond_t *cond, mutex_t *mutex) {
    return SleepConditionVariableCS(&cond->inner, &mutex->inner, INFINITE) ? 0 : -1;
}

i32_t cond_signal(cond_t *cond) {
    WakeConditionVariable(&cond->inner);
    return 0;
}

i32_t cond_broadcast(cond_t *cond) {
    WakeAllConditionVariable(&cond->inner);
    return 0;
}

// Thread functions
ray_thread_t ray_thread_create(raw_p (*fn)(raw_p), raw_p arg) {
    ray_thread_t t;
    raw_p (*orig_fn)(raw_p) = fn;
    DWORD(*cfn)
    (raw_p) = (DWORD(*)(raw_p))(raw_p)orig_fn;
    t.handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)cfn, arg, 0, NULL);
    return t;
}

i32_t thread_destroy(ray_thread_t *thread) { return CloseHandle(thread->handle) ? 0 : -1; }

i32_t thread_join(ray_thread_t thread) {
    return WaitForSingleObject(thread.handle, INFINITE) == WAIT_OBJECT_0 ? 0 : -1;
}

i32_t thread_detach(ray_thread_t thread) { return CloseHandle(thread.handle) ? 0 : -1; }

nil_t thread_exit(raw_p res) {
    i64_t code = (i64_t)res;
    ExitThread((DWORD)code);
}

ray_thread_t thread_self() {
    ray_thread_t t;
    t.handle = GetCurrentThread();
    return t;
}

i32_t thread_pin(ray_thread_t thread, i64_t core) {
    DWORD_PTR mask = 1ULL << core;
    if (SetThreadAffinityMask(thread.handle, mask) == 0) {
        return -1;
    }
    return 0;
}

#else

mutex_t mutex_create() {
    mutex_t mutex;
    pthread_mutex_init(&mutex.inner, NULL);
    return mutex;
}

nil_t mutex_destroy(mutex_t *mutex) { pthread_mutex_destroy(&mutex->inner); }

nil_t mutex_lock(mutex_t *mutex) { pthread_mutex_lock(&mutex->inner); }

nil_t mutex_unlock(mutex_t *mutex) { pthread_mutex_unlock(&mutex->inner); }

cond_t cond_create() {
    cond_t cond;
    pthread_cond_init(&cond.inner, NULL);
    return cond;
}

nil_t cond_destroy(cond_t *cond) { pthread_cond_destroy(&cond->inner); }

i32_t cond_wait(cond_t *cond, mutex_t *mutex) { return pthread_cond_wait(&cond->inner, &mutex->inner); }

i32_t cond_wait_timeout(cond_t *cond, mutex_t *mutex, i64_t timeout_ms) {
    struct timespec ts;

    // Get current time
    clock_gettime(CLOCK_REALTIME, &ts);

    // Add timeout to current time
    ts.tv_sec += timeout_ms / 1000;
    ts.tv_nsec += (timeout_ms % 1000) * 1000000;

    // Normalize the timespec
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec += 1;
        ts.tv_nsec -= 1000000000;
    }

    // Wait with timeout
    return pthread_cond_timedwait(&cond->inner, &mutex->inner, &ts);
}

i32_t cond_signal(cond_t *cond) { return pthread_cond_signal(&cond->inner); }

i32_t cond_broadcast(cond_t *cond) { return pthread_cond_broadcast(&cond->inner); }

ray_thread_t ray_thread_create(raw_p (*fn)(raw_p), raw_p arg) {
    ray_thread_t thread;
    pthread_create(&thread.handle, NULL, fn, arg);
    return thread;
}

i32_t thread_destroy(ray_thread_t *thread) { return pthread_cancel(thread->handle); }

i32_t thread_join(ray_thread_t thread) { return pthread_join(thread.handle, NULL); }

i32_t thread_detach(ray_thread_t thread) { return pthread_detach(thread.handle); }

nil_t thread_exit(raw_p res) { return pthread_exit(res); }

ray_thread_t thread_self() {
    ray_thread_t t;
    t.handle = pthread_self();
    return t;
}

#if defined(OS_LINUX)

i32_t thread_pin(ray_thread_t thread, i64_t core) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core, &cpuset);

    // Set affinity
    int result = pthread_setaffinity_np(thread.handle, sizeof(cpu_set_t), &cpuset);
    if (result != 0)
        return result;

    // Verify affinity
    result = pthread_getaffinity_np(thread.handle, sizeof(cpu_set_t), &cpuset);
    if (result != 0)
        return result;

    if (!CPU_ISSET(core, &cpuset))
        return -1;

    return 0;
}

#else

i32_t thread_pin(ray_thread_t thread, i64_t core) {
    UNUSED(thread);
    UNUSED(core);
    // thread_port_t mach_thread;
    // thread_affinity_policy_data_t policy;
    // kern_return_t kr;

    // mach_thread = pthread_mach_thread_np(thread.handle);
    // policy.affinity_tag = core;
    // kr = thread_policy_set(mach_thread, THREAD_AFFINITY_POLICY, (thread_policy_t)&policy,
    // THREAD_AFFINITY_POLICY_COUNT);

    // if (kr != KERN_SUCCESS)
    // {
    //     fprintf(stderr, "Error setting thread policy: %s\n", mach_error_C8(kr));
    //     return -1;
    // }

    return 0;
}

#endif

// Lightweight event signaling implementation
#if defined(OS_LINUX)
#include <sys/eventfd.h>
#include <unistd.h>
#include <fcntl.h>

i32_t event_create(void) {
    // EFD_SEMAPHORE makes each read decrement by 1 (good for counting signals)
    return eventfd(0, EFD_SEMAPHORE);
}

nil_t event_destroy(i32_t fd) {
    close(fd);
}

i32_t event_signal(i32_t fd) {
    u64_t val = 1;
    return write(fd, &val, sizeof(val)) == sizeof(val) ? 0 : -1;
}

i32_t event_wait(i32_t fd) {
    u64_t val;
    return read(fd, &val, sizeof(val)) == sizeof(val) ? 0 : -1;
}

i32_t event_clear(i32_t fd) {
    u64_t val;
    // Non-blocking read to clear any pending signals
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    while (read(fd, &val, sizeof(val)) > 0) {}
    fcntl(fd, F_SETFL, flags);
    return 0;
}

#else
// Fallback: use pipe for non-Linux systems
#include <unistd.h>
#include <fcntl.h>

// Store both read and write ends in a single int (read in low 16 bits, write in high 16 bits)
i32_t event_create(void) {
    int pipefd[2];
    if (pipe(pipefd) != 0)
        return -1;
    // Make read end non-blocking for clear operation
    fcntl(pipefd[0], F_SETFL, O_NONBLOCK);
    // Pack both fds - this limits fd values but works for most cases
    return (pipefd[1] << 16) | (pipefd[0] & 0xFFFF);
}

nil_t event_destroy(i32_t fd) {
    close(fd & 0xFFFF);        // read end
    close((fd >> 16) & 0xFFFF); // write end
}

i32_t event_signal(i32_t fd) {
    char c = 1;
    int write_fd = (fd >> 16) & 0xFFFF;
    return write(write_fd, &c, 1) == 1 ? 0 : -1;
}

i32_t event_wait(i32_t fd) {
    char c;
    int read_fd = fd & 0xFFFF;
    // Make blocking for wait
    int flags = fcntl(read_fd, F_GETFL, 0);
    fcntl(read_fd, F_SETFL, flags & ~O_NONBLOCK);
    int result = read(read_fd, &c, 1) == 1 ? 0 : -1;
    fcntl(read_fd, F_SETFL, flags);
    return result;
}

i32_t event_clear(i32_t fd) {
    char c;
    int read_fd = fd & 0xFFFF;
    while (read(read_fd, &c, 1) > 0) {}
    return 0;
}

#endif

#endif
