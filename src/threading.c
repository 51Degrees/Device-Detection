/* *********************************************************************
* This Source Code Form is copyright of 51Degrees Mobile Experts Limited.
* Copyright 2015 51Degrees Mobile Experts Limited, 5 Charlotte Close,
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

#ifndef FIFTYONEDEGREES_NO_THREADING
#ifndef _MSC_VER

#include <pthread.h>
#include "threading.h"
#ifdef __APPLE__
#include <sys/time.h>
#endif

/**
 * GCC / PTHREAD SIGNAL IMPLEMENTATION - NOT USED BY WINDOWS
 */

/**
 * Initialises the mutex passed to the method.
 * @param mutex to be initialised.
 */
void fiftyoneDegreesMutexCreate(const fiftyoneDegreesMutex *mutex) {
	((fiftyoneDegreesMutex*)mutex)->initValue = pthread_mutex_init((pthread_mutex_t*)&mutex->mutex, NULL);
}

/**
 * Closes the mutex passed to the method.
 * @param mutex to be closed.
 */
void fiftyoneDegreesMutexClose(const fiftyoneDegreesMutex *mutex) {
	pthread_mutex_destroy((pthread_mutex_t*)&mutex->mutex);
}

/**
 * Locks the mutex passed to the method.
 * @param mutex to be locked.
 */
void fiftyoneDegreesMutexLock(const fiftyoneDegreesMutex *mutex) {
	pthread_mutex_lock((pthread_mutex_t*)&mutex->mutex);
}

/**
 * Unlocks the mutex passed to the method.
 * @param mutex to be unlocked.
 */
void fiftyoneDegreesMutexUnlock(const fiftyoneDegreesMutex *mutex) {
	pthread_mutex_unlock((pthread_mutex_t*)&mutex->mutex);
}

/**
 * Tests the mutex to confirm it's valid.
 * @param mutex to be tested
 */
int fiftyoneDegreesMutexValid(const fiftyoneDegreesMutex *mutex) {
	return mutex->initValue == 0;
}

/**
 * Initialises the signal pointer by setting the condition first followed by
 * the mutex if the condition was set correctly. Destroyed is set to false to
 * indicate to the other methods that the signal is still valid. The memory
 * used by the signal should be part of another structure and will be released
 * when that structure is released. If there is a problem creating the mutex
 * the condition is also released.
 * @param signal to be initialised
 */
void fiftyoneDegreesSignalCreate(fiftyoneDegreesSignal *signal) {
	signal->initValue = pthread_cond_init((pthread_cond_t*)&signal->cond, 0);
	if (signal->initValue == 0) {
		signal->destroyed = 0;
		fiftyoneDegreesMutexCreate(&signal->mutex);
		if (signal->mutex.initValue != 0) {
			pthread_cond_destroy((pthread_cond_t*)&signal->cond);
			signal->destroyed = 1;
			signal->initValue = 1;
		}
	}
}

/**
 * Closes the signal ensuring there is a lock on the signal before destroying
 * the signal. This means that no other process can be waiting on the signal
 * before it is destroyed. The destroyed field of the signal structure is set
 * to true after the condition is destroyed. All methods that could
 * subsequently try and get a lock on the signal MUST check the destroyed
 * field before trying to get the lock.
 * @param signal to be closed.
 */
void fiftyoneDegreesSignalClose(fiftyoneDegreesSignal *signal) {
	if (signal->destroyed == 0) {
		pthread_mutex_lock((pthread_mutex_t *__restrict)&signal->mutex.mutex);
		pthread_cond_destroy((pthread_cond_t*)&signal->cond);
		signal->destroyed = 1;
		pthread_mutex_unlock((pthread_mutex_t *__restrict)&signal->mutex.mutex);
		fiftyoneDegreesMutexClose(&signal->mutex);
	}
}

/**
 * If the signal has not been destroyed then sends a signal to a waiting
 * thread that the signal has been set and one can continue. This possible
 * because the condition will auto reset only enabling a signal thread to
 * continue even if multi threads are waiting.
 * @param signal to be set.
 */
void fiftyoneDegreesSignalSet(fiftyoneDegreesSignal *signal) {
	if (signal->destroyed == 0) {
		pthread_cond_signal((pthread_cond_t *__restrict)&signal->cond);
	}
}

/**
 * Wait for a signal to be set. Only waits for the signal if the signal has not
 * been destroyed. Locks the mutex before the signal is waited for. This ensures
 * only one thread can be waiting on the signal at any one time.
 * @param signal pointer to the signal used to wait on.
 */
void fiftyoneDegreesSignalWait(fiftyoneDegreesSignal *signal) {
	struct timespec timeout;
	if (signal->destroyed == 0) {
		if (pthread_mutex_lock((pthread_mutex_t *__restrict)&signal->mutex.mutex) == 0) {
#ifdef __APPLE__
			struct timeval tv;
			gettimeofday(&tv, NULL);
			timeout.tv_sec = tv.tv_sec;
			timeout.tv_nsec = tv.tv_usec * 1000;
#else
			clock_gettime(CLOCK_REALTIME, &timeout);
#endif
			timeout.tv_nsec += (FIFTYONEDEGREES_SIGNAL_TIMEOUT_MS * 1000000);
			pthread_cond_timedwait((pthread_cond_t *__restrict)&signal->cond,
							   (pthread_mutex_t *__restrict)&signal->mutex.mutex,
							   (const struct timespec *__restrict)&timeout);
			pthread_mutex_unlock((pthread_mutex_t *__restrict)&signal->mutex.mutex);
		}
	}
}

/**
 * Tests a signal to confirm it's valid for use.
 * @param signal pointer to the signal being tested.
 */
int fiftyoneDegreesSignalValid(fiftyoneDegreesSignal *signal) {
 	return signal->initValue == 0 && signal->destroyed == 0 && fiftyoneDegreesMutexValid(&signal->mutex);
}

#endif
#endif
