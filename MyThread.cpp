#include "MyThread.h"
#include<iostream>
#include<pthread.h>
#include<unistd.h>
#include<stdio.h>
#include<errno.h>
using namespace std;

/* ʵ�ֹ��캯�� */
MyThread::MyThread(Thread_interface &w) {
	worker = &w;
	error = 0;
}

/* ִ��worker�����run()���� */
void* MyThread::run(void* w) {
	((Thread_interface *)w)->run();
}

/* ͨ��ϵͳ����pthread_create()�����߳� */
int MyThread::Start() {
	error = pthread_create(&id, NULL, run, (void*)worker);
	return error;
}