#include "MyThread.h"
#include<iostream>
#include<pthread.h>
#include<unistd.h>
#include<stdio.h>
#include<errno.h>
using namespace std;

/* 实现构造函数 */
MyThread::MyThread(Thread_interface &w) {
	worker = &w;
	error = 0;
}

/* 执行worker对象的run()函数 */
void* MyThread::run(void* w) {
	((Thread_interface *)w)->run();
}

/* 通过系统调用pthread_create()产生线程 */
int MyThread::Start() {
	error = pthread_create(&id, NULL, run, (void*)worker);
	return error;
}