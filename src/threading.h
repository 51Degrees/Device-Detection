/* *********************************************************************
* This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
* Copyright © 2014 51Degrees Mobile Experts Limited, 5 Charlotte Close,
* Caversham, Reading, Berkshire, United Kingdom RG4 7BY
*
* This Source Code Form is the subject of the following patent
* applications, owned by 51Degrees Mobile Experts Limited of 5 Charlotte
* Close, Caversham, Reading, Berkshire, United Kingdom RG4 7BY:
* European Patent Application No. 13192291.6; and
* United States Patent Application Nos. 14/085,223 and 14/085,301.
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
* MUTEX AND THREADING MACROS
*/

#ifdef _MSC_VER
#include <windows.h>
#else
#include <pthread.h>
#endif

/**
* Mutex used to synchronise access to data structures that could be used
* in parallel in a multi threaded environment.
*/
#ifdef _MSC_VER
#define FIFTYONEDEGREES_MUTEX HANDLE
#else
#define FIFTYONEDEGREES_MUTEX pthread_mutex_t
#endif

/**
* A signal used to limit the number of worksets that can be created by
* the pool.
*/
#ifdef _MSC_VER
#define FIFTYONEDEGREES_SIGNAL HANDLE
#else
#define FIFTYONEDEGREES_SIGNAL pthread_mutex_t
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
#endif

/**
* Frees the handle provided to the macro.
*/
#ifdef _MSC_VER
#define FIFTYONEDEGREES_SIGNAL_CLOSE(s) CloseHandle(s)
#else
#endif

/**
* Signals a thread waiting for the signal to proceed.
*/
#ifdef _MSC_VER
#define FIFTYONEDEGREES_SIGNAL_SET(s) SetEvent(s)
#else
#endif

/**
* Waits for the signal to become set by another thread.
*/
#ifdef _MSC_VER
#define FIFTYONEDEGREES_SIGNAL_WAIT(s) WaitForSingleObject(s, INFINITE)
#else
#endif

/**
* Creates a new mutex at the pointer provided.
*/
#ifdef _MSC_VER
#define FIFTYONEDEGREES_MUTEX_CREATE(m) m = (FIFTYONEDEGREES_MUTEX)CreateMutex(NULL,FALSE,NULL)
#else
#define FIFTYONEDEGREES_MUTEX_CREATE(m) pthread_mutex_init(m, NULL);
#endif

/**
* Frees the mutex at the pointer provided.
*/
#ifdef _MSC_VER
#define FIFTYONEDEGREES_MUTEX_CLOSE(m) CloseHandle(m)
#else
#define FIFTYONEDEGREES_MUTEX_CLOSE(m) pthread_mutex_destroy(m);
#endif

/**
* Locks the mutex at the pointer provided.
*/
#ifdef _MSC_VER
#define FIFTYONEDEGREES_MUTEX_LOCK(m) WaitForSingleObject(m, INFINITE)
#else
#define FIFTYONEDEGREES_MUTEX_LOCK(m) pthread_mutex_lock(m)
#endif

/**
* Unlocks the mutex at the pointer provided.
*/
#ifdef _MSC_VER
#define FIFTYONEDEGREES_MUTEX_UNLOCK(m) ReleaseMutex(m)
#else
#define FIFTYONEDEGREES_MUTEX_UNLOCK(m) pthread_mutex_unlock(m)
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
#define FIFTYONEDEGREES_THREAD_CREATE(t, m, s) pthread_create(t, NULL, m, s);
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
#define FIFTYONEDEGREES_THREAD_EXIT
#else
#define FIFTYONEDEGREES_THREAD_FREE(t) pthread_exit(NULL)
#endif