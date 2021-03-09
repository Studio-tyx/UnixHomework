#include "MySync.h"
#include<iostream>
#include<pthread.h>
#include<unistd.h>
#include<stdio.h>
#include<errno.h>
using namespace std;

/* 声明MyMutex类构造函数，初始化一个互斥锁 */
MyMutex::MyMutex() {
	error = pthread_mutex_init(&a_mutex, NULL);
}

/* 声明MyMutex类析构函数，撤销互斥锁 */
MyMutex::~MyMutex() {
	error = pthread_mutex_destroy(&a_mutex);
}

/* 实现MyCondition的构造函数，初始化一个互斥锁及条件变量 */
MyCondition::MyCondition() {
	error = pthread_cond_init(&got_request, NULL);
	error += pthread_mutex_init(&a_mutex, NULL);
}

/* 实现MyCondition的析构函数，撤销互斥锁及条件变量，如果还有线程守候该条件变量，则等待直至该线程退出 */
MyCondition::~MyCondition() {
	error = pthread_mutex_destroy(&a_mutex);
	int rc = pthread_cond_destroy(&got_request);
	/*some thread is still waiting on this condition variable */
	while (rc == EBUSY) {
		sleep(1);
	}
	error = error + rc;
}

/* 实现MyCondition的wait()函数，它让线程守候在MyCondition对象的条件变量got_request上直至被唤醒或超时 */
int MyCondition::wait(int second = 0) {
	struct timeval now; /* time when we started waiting */
	struct timespec timeout; /* timeout value for the wait function */
	int done; /* are we done waiting ? */

	/* 加锁：first, lock the mutex */
	int rc = pthread_mutex_lock(&a_mutex);
	if (rc) {/*an error has occurred */
		perror("pthread_mutex_lock");
		pthread_exit(NULL);
	}

	/* 设置定时，并等候 */
	if (second) {
		/* get current time */
		gettimeofday(&now, NULL);
		/* prepare timeout value */
		timeout.tv_sec = now.tv_sec + second;
		timeout.tv_nsec = now.tv_usec * 1000; /* timeval uses microseconds. */
		/* timespec uses nanoseconds. */
		/* 1 nanosecond = 1000 micro seconds. */
		error = pthread_cond_timedwait(&got_request, &a_mutex, &timeout);
	}
	else error = pthread_cond_wait(&got_request, &a_mutex);
	
	/* 解锁：finally, unlock the mutex */
	pthread_mutex_unlock(&a_mutex);

	/* 错误处理 */
	switch (error) {
		/* we were awakened due to the cond.variable being signaled */
	case 0:
		/* the mutex was now locked again by pthread_cond_timedwait. */
		/* do your stuff here... */
		return 0;

	case ETIMEDOUT: /* our time is up */
		return 1;

	default: /* some error occurred(e.g.we got a Unix signal) */
		return 2; /* break this switch, but re—do the while loop. */
	}
}

/* 唤醒一个线程 */
int MyCondition::wake(){
	error = pthread_cond_signal(&got_request);
	return error;
}

/* 唤醒所有线程 */
int MyCondition::wakeAll() {
	error = pthread_cond_broadcast(&got_request);
	return error;
}

