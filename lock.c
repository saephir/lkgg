#include "lock.h"
#include <pthread.h>

static pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t stdiomutex = PTHREAD_MUTEX_INITIALIZER;

void locks_init(void) {

	pthread_mutex_init(&mymutex, 0);
	pthread_mutex_init(&stdiomutex, 0);

}

void locks_cleanup(void) {

	pthread_mutex_destroy(&mymutex);
	pthread_mutex_destroy(&stdiomutex);

}

void socket_lock(void) {

	pthread_mutex_lock(&mymutex);

}

void socket_unlock(void) {

	pthread_mutex_unlock(&mymutex);

}

void stdio_lock(void) {

	pthread_mutex_lock(&stdiomutex);

}

void stdio_unlock(void) {

	pthread_mutex_unlock(&stdiomutex);

}
