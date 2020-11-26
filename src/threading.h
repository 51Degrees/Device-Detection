/* *********************************************************************
 * This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
 * Copyright 2017 51Degrees Mobile Experts Limited, 5 Charlotte Close,
 * Caversham, Reading, Berkshire, United Kingdom RG4 7BY
 *
 * This Source Code Form is the subject of the following patents and patent
 * applications, owned by 51Degrees Mobile Experts Limited of 5 Charlotte
 * Close, Caversham, Reading, Berkshire, United Kingdom RG4 7BY:
 * European Patent No. 2871816;
 * European Patent Application No. 17184134.9;
 * United States Patent Nos. 9,332,086 and 9,350,823; and
 * United States Patent Application No. 15/686,066.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0.
 *
 * If a copy of the MPL was not distributed with this file, You can obtain
 * one at http://mozilla.org/MPL/2.0/.
 *
 * This Source Code Form is "Incompatible With Secondary Licenses", as
 * defined by the Mozilla Public License, v. 2.0.
 ********************************************************************** */


/**
 * Define macros if compiler directive is not explicitly requesting single
 * threaded operation.
 */

#ifndef FIFTYONEDEGREES_THREADING_INCLUDED
#define FIFTYONEDEGREES_THREADING_INCLUDED

#ifndef FIFTYONEDEGREES_NO_THREADING

/**
* MUTEX AND THREADING MACROS
*/

#ifdef _MSC_VER
#include <windows.h>
#include <intrin.h>  
#pragma intrinsic (_InterlockedIncrement) 
#pragma intrinsic (_InterlockedDecrement) 
#else
#include <pthread.h>
#endif

#define FIFTYONEDEGREES_SIGNAL_TIMEOUT_MS 1

/**
* Mutex used to synchronise access to data structures that could be used
* in parallel in a multi threaded environment.
*/
#ifdef _MSC_VER
#define FIFTYONEDEGREES_MUTEX HANDLE
#else
typedef struct fiftyoneDegrees_mutex_t {
    int initValue;
    pthread_mutex_t mutex;
} fiftyoneDegreesMutex;
void fiftyoneDegreesMutexCreate(const fiftyoneDegreesMutex *mutex);
void fiftyoneDegreesMutexClose(const fiftyoneDegreesMutex *mutex);
void fiftyoneDegreesMutexLock(const fiftyoneDegreesMutex *mutex);
void fiftyoneDegreesMutexUnlock(const fiftyoneDegreesMutex *mutex);
int fiftyoneDegreesMutexValid(const fiftyoneDegreesMutex *mutex);
#define FIFTYONEDEGREES_MUTEX fiftyoneDegreesMutex
#endif

/**
* A signal used to limit the number of worksets that can be created by
* the pool.
*/
#ifdef _MSC_VER
#define FIFTYONEDEGREES_SIGNAL HANDLE
#else
typedef struct fiftyoneDegrees_signal_t {
    int initValue;
    pthread_cond_t cond;
    fiftyoneDegreesMutex mutex;
    int destroyed; // Indicates if the signal has been destroyed
} fiftyoneDegreesSignal;
void fiftyoneDegreesSignalCreate(fiftyoneDegreesSignal *signal);
void fiftyoneDegreesSignalClose(fiftyoneDegreesSignal *signal);
void fiftyoneDegreesSignalSet(fiftyoneDegreesSignal *signal);
void fiftyoneDegreesSignalWait(fiftyoneDegreesSignal *signal);
int fiftyoneDegreesSignalValid(fiftyoneDegreesSignal *signal);
#define FIFTYONEDEGREES_SIGNAL fiftyoneDegreesSignal
#endif

/**
 * A thread created with the CREATE_THREAD macro.
 */
#ifdef _MSC_VER
#define FIFTYONEDEGREES_THREAD HANDLE
#else
#define FIFTYONEDEGREES_THREAD pthread_t
#endif

/**
* Creates a new signal that can be used to wait for
* other operations to complete before continuing.
*/
#ifdef _MSC_VER
#define FIFTYONEDEGREES_SIGNAL_CREATE(s) s = (FIFTYONEDEGREES_SIGNAL)CreateEvent(NULL, FALSE, TRUE, NULL)
#else
#define FIFTYONEDEGREES_SIGNAL_CREATE(s) fiftyoneDegreesSignalCreate((fiftyoneDegreesSignal*)&s)
#endif

/**
* Frees the handle provided to the macro.
*/
#ifdef _MSC_VER
#define FIFTYONEDEGREES_SIGNAL_CLOSE(s) if (s != NULL) { CloseHandle(s); }
#else
#define FIFTYONEDEGREES_SIGNAL_CLOSE(s) fiftyoneDegreesSignalClose((fiftyoneDegreesSignal*)&s)
#endif

/**
* Signals a thread waiting for the signal to proceed.
*/
#ifdef _MSC_VER
#define FIFTYONEDEGREES_SIGNAL_SET(s) SetEvent(s)
#else
#define FIFTYONEDEGREES_SIGNAL_SET(s) fiftyoneDegreesSignalSet((fiftyoneDegreesSignal*)s)
#endif

/**
* Waits for the signal to become set by another thread.
*/
#ifdef _MSC_VER
#define FIFTYONEDEGREES_SIGNAL_WAIT(s) WaitForSingleObject(*s, FIFTYONEDEGREES_SIGNAL_TIMEOUT_MS)
#else
#define FIFTYONEDEGREES_SIGNAL_WAIT(s) fiftyoneDegreesSignalWait((fiftyoneDegreesSignal*)s)
#endif

/**
 * Returns true if the signal is valid.
 */
#ifdef _MSC_VER
#define FIFTYONEDEGREES_SIGNAL_VALID(s) (*s != NULL)
#else
#define FIFTYONEDEGREES_SIGNAL_VALID(s) fiftyoneDegreesSignalValid((fiftyoneDegreesSignal*)s)
#endif

/**
* Creates a new mutex at the pointer provided.
*/
#ifdef _MSC_VER
#define FIFTYONEDEGREES_MUTEX_CREATE(m) m = (FIFTYONEDEGREES_MUTEX)CreateMutex(NULL,FALSE,NULL)
#else
#define FIFTYONEDEGREES_MUTEX_CREATE(m) fiftyoneDegreesMutexCreate(&m)
#endif

/**
* Frees the mutex at the pointer provided.
*/
#ifdef _MSC_VER
#define FIFTYONEDEGREES_MUTEX_CLOSE(m) if (m != NULL) { CloseHandle(m); }
#else
#define FIFTYONEDEGREES_MUTEX_CLOSE(m) fiftyoneDegreesMutexClose(&m)
#endif

/**
* Locks the mutex at the pointer provided.
*/
#ifdef _MSC_VER
#define FIFTYONEDEGREES_MUTEX_LOCK(m) WaitForSingleObject(*m, INFINITE)
#else
#define FIFTYONEDEGREES_MUTEX_LOCK(m) fiftyoneDegreesMutexLock(m)
#endif

/**
* Unlocks the mutex at the pointer provided.
*/
#ifdef _MSC_VER
#define FIFTYONEDEGREES_MUTEX_UNLOCK(m) ReleaseMutex(*m)
#else
#define FIFTYONEDEGREES_MUTEX_UNLOCK(m) fiftyoneDegreesMutexUnlock(m)
#endif

/**
 * Returns true if the signal is valid.
 */
#ifdef _MSC_VER
#define FIFTYONEDEGREES_MUTEX_VALID(m) (*m != NULL)
#else
#define FIFTYONEDEGREES_MUTEX_VALID(m) fiftyoneDegreesMutexValid(m)
#endif

/**
 * Creates a new thread with the following parameters:
 * t - pointer to FIFTYONEDEGREES_THREAD memory
 * m - the method to call when the thread runs
 * s - pointer to the state data to pass to the method
 */
#ifdef _MSC_VER
#define FIFTYONEDEGREES_THREAD_CREATE(t, m, s) t = (FIFTYONEDEGREES_THREAD)CreateThread(NULL, 0, m, s, 0, NULL)
#else
#define FIFTYONEDEGREES_THREAD_CREATE(t, m, s) pthread_create(&t, NULL, m, s)
#endif

/**
 * Joins the thread provided to the current thread waiting
 * indefinitely for the operation to complete.
 * t - pointer to a previously created thread
 */
#ifdef _MSC_VER
#define FIFTYONEDEGREES_THREAD_JOIN(t) WaitForSingleObject(t, INFINITE)
#else
#define FIFTYONEDEGREES_THREAD_JOIN(t) pthread_join(t, NULL)
#endif

/**
 * Closes the thread passed to the macro.
 */
#ifdef _MSC_VER
#define FIFTYONEDEGREES_THREAD_CLOSE(t) CloseHandle(t)
#else
#define FIFTYONEDEGREES_THREAD_CLOSE(t) pthread_exit(NULL)
#endif

#ifdef _MSC_VER
#define FIFTYONEDEGREES_THREAD_EXIT ExitThread(0)
#else
#define FIFTYONEDEGREES_THREAD_EXIT pthread_exit(NULL)
#endif

#endif
#endif

#ifdef _MSC_VER
#define FIFTYONEDEGREES_INTERLOCK_INC(v) _InterlockedIncrement(v)
#else
#define FIFTYONEDEGREES_INTERLOCK_INC(v) __sync_add_and_fetch(v, 1)
#endif

#ifdef _MSC_VER
#define FIFTYONEDEGREES_INTERLOCK_DEC(v) _InterlockedDecrement(v)
#else
#define FIFTYONEDEGREES_INTERLOCK_DEC(v) __sync_add_and_fetch(v, -1)
#endif

#ifdef _MSC_VER
#ifdef _WIN64
#define FIFTYONEDEGREES_INTERLOCK_EXCHANGE(d,e,c) \
	InterlockedCompareExchange64((volatile __int64*)&d, (__int64)e, (__int64)c)
#else
#define FIFTYONEDEGREES_INTERLOCK_EXCHANGE(d,e,c) \
	InterlockedCompareExchange((volatile long*)&d, (long)e, (long)c))
#endif
#else
#define FIFTYONEDEGREES_INTERLOCK_EXCHANGE(d,e,c) \
	__sync_val_compare_and_swap(&d,c,e)
#endif

/**
 * Double width (64 or 128 depending on the architecture) compare and exchange.
 * Replaces the destination value with the exchange value, only if the
 * destination value matched the comparand. Returns true if the value was
 * exchanged.
 * @param d the destination to swap
 * @param e the exchange value
 * @param c the comparand
 */
#ifdef _MSC_VER
#ifdef _WIN64
#define FIFTYONEDEGREES_INTERLOCK_EXCHANGE_DW(d,e,c) \
    InterlockedCompareExchange128((__int64*)d, *((__int64*)&e + 1), *(((__int64*)&e)), (__int64*)&c)
#else
#define FIFTYONEDEGREES_INTERLOCK_EXCHANGE_DW(d,e,c) \
    InterlockedCompareExchange64((__int64*)d, *((__int64*)&e), *(__int64*)&c)
#endif
#else
/**
 * Implements the __sync_bool_compare_and_swap_16 function which is often not
 * implementned by the compiler. This uses the cmpxchg16b instruction from the
 * x86-64 instruction set, the same instruction as the
 * InterlockedCompareExchange128 implementation
 * (see https://docs.microsoft.com/en-us/cpp/intrinsics/interlockedcompareexchange128?view=vs-2019#remarks).
 * It is therefore supported by modern Intel and AMD CPUs. However, most ARM
 * chips will not support this.
 * For full details of the cmpxchg16b instruction, see the manual:
 * https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-instruction-set-reference-manual-325383.pdf
 * and other example implementations:
 * https://github.com/ivmai/libatomic_ops/blob/release-7_2/src/atomic_ops/sysdeps/gcc/x86_64.h#L148
 * https://github.com/haproxy/haproxy/blob/a7bf57352059277239794950f9aac33d05741f1a/include/common/hathreads.h#L1000
 * @param destination memory location to be replaced if the compare is true
 * @param exchange memory to copy to the destination if the compare is true
 * @param compare memory to compare to destination.
 * @return 1 if all 16 bytes of destination and compare were equal and
 * destination was replaced, otherwise 0
 */
static __inline int
__fod_sync_bool_compare_and_swap_16(
    void *destination,
    const void *exchange,
    void *compare)
{
    char result;
    __asm __volatile("lock cmpxchg16b %0; setz %3"
    : "+m" (*(void **)destination),
        "=a" (((void **)compare)[0]),
        "=d" (((void **)compare)[1]),
        "=q" (result)
        : "a" (((void **)compare)[0]),
        "d" (((void **)compare)[1]),
        "b" (((const void **)exchange)[0]),
        "c" (((const void **)exchange)[1])
        : "memory", "cc");
    return (result);
}
#define FIFTYONEDEGREES_INTERLOCK_EXCHANGE_DW(d,e,c) \
    (sizeof(void*) == 8 ? \
    __fod_sync_bool_compare_and_swap_16((void*)d, (void*)&e, (void*)&c) : \
    __sync_bool_compare_and_swap((long*)d, *((long*)&c), *((long*)&e)))
#endif
