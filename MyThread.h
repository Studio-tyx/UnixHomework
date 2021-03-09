#pragma once

/******************************************************
class name: Thread_interface
function: It is base class of all threads. Used by class Mythread.
******************************************************/

/* MyThread的基类Thread_interface */
class Thread_interface
{
public:
	virtual void run(){}
};

/******************************************************
class name: MyThread
function: Create thread and run it.
******************************************************/
class MyThread:public Thread_interface
{
	Thread_interface* worker; /* 指向Thread_interface的指针，所指的对象是线程将要执行的实体 */
	int error;
	pthread_t id; /* 定义id，用于存放线程ID */
	static void* run(void*); /* 声明静态函数run()，用于产生线程 */
public:
	/* 声明MyThread类构造函数 */
	MyThread(Thread_interface &w);
	MyThread() { worker = this; }

	/* 声明成员函数 */
	int Start();
	void Exit(void *value_ptr) { ::pthread_exit(value_ptr); }
	int Join(pthread_t thread, void **value_ptr) { 
		error = ::pthread_join(thread, value_ptr);
		return error;
	}
	int Cancel(pthread_t target_thread) {
		error = ::pthread_cancel(target_thread);
		return error;
	}
	int Detach() {
		error = ::pthread_detach(id);
		return error;
	}
	pthread_t getId() { return id; }
	int Error() { return error; }
};

