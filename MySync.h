#pragma once

/******************************************************
class name: MyMutex
function: support mutex
******************************************************/
class MyMutex {
	pthread_mutex_t a_mutex; /* 存放互斥锁 */
	int error; /* 存放错误码 */
public:

	/* 声明构造函数及析构函数 */
	MyMutex();
	~MyMutex();

	/* 声明并实现成员函数，包括加锁解锁操作 */
	int Lock() {
		error = pthread_mutex_lock(&a_mutex);
		return error;
	}
	int Trylock() {
		error = pthread_mutex_trylock(&a_mutex);
		return error;
	}
	int Unlock() {
		error = pthread_mutex_unlock(&a_mutex);
		return error;
	}
	int Error() { return error; }
};

/******************************************************
class name: MyCondition
function: support MyCondition variable
******************************************************/
class MyCondition {
	pthread_mutex_t a_mutex; /* 互斥锁 */
	pthread_cond_t got_request; /* 条件变量 */
	int error;
public:
	MyCondition();
	~MyCondition();

	/* 声明成员函数 */
	int wait(int second = 0);
	int wake();
	int wakeAll();
	int Error() { return error; }
};
