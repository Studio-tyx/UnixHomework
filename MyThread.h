#pragma once

/******************************************************
class name: Thread_interface
function: It is base class of all threads. Used by class Mythread.
******************************************************/

/* MyThread�Ļ���Thread_interface */
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
	Thread_interface* worker; /* ָ��Thread_interface��ָ�룬��ָ�Ķ������߳̽�Ҫִ�е�ʵ�� */
	int error;
	pthread_t id; /* ����id�����ڴ���߳�ID */
	static void* run(void*); /* ������̬����run()�����ڲ����߳� */
public:
	/* ����MyThread�๹�캯�� */
	MyThread(Thread_interface &w);
	MyThread() { worker = this; }

	/* ������Ա���� */
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

