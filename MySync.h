#pragma once

/******************************************************
class name: MyMutex
function: support mutex
******************************************************/
class MyMutex {
	pthread_mutex_t a_mutex; /* ��Ż����� */
	int error; /* ��Ŵ����� */
public:

	/* �������캯������������ */
	MyMutex();
	~MyMutex();

	/* ������ʵ�ֳ�Ա���������������������� */
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
	pthread_mutex_t a_mutex; /* ������ */
	pthread_cond_t got_request; /* �������� */
	int error;
public:
	MyCondition();
	~MyCondition();

	/* ������Ա���� */
	int wait(int second = 0);
	int wake();
	int wakeAll();
	int Error() { return error; }
};
