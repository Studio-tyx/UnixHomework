#include "TcpCliThr.h"
#include <stdio.h>
#include <vector>
#include <pthread.h>
#include "Mysocket.h"
#include "MyThread.h"

/* 实现构造函数，设置与TCP套接字相关参数并产生线程队列 */
TcpCliThr::TcpCliThr()
{
	Thrset = new vector<MyThread*>();
}

TcpCliThr::TcpCliThr(int port, char *server) :
	MySocket(AF_INET, SOCK_STREAM, 0, port, server) {
	Thrset = new vector<MyThread*>();
}

/* 实现析构函数，首先等待所有线程退出，然后扫描线程，删除所有线程对象，最后删除线程队列 */
TcpCliThr :: ~TcpCliThr() {
	/* Wait until all threads exits */
	WaitAllThr();
	/* free all memory of threads */
	vector<MyThread*>::iterator it = Thrset->begin();
	while (it != ThrSet->end()) {
		MyThread* thr = (MyThread*)(*it);
		delete thr;
		it++;
	}

	delete ThrSet;
}

/* 一旦连接建立，则产生收发线程，将它们加入线程队列并运行 */
int TcpCliThr::ConnectServ() {
	if (Connect() == -1) return -1;
	/* Create new thread */
	MyThread *Rthread, *Wthread;
	if (CreateThr(&Rthread, &Wthread) == -1) return -1;
	AddThread(Rthread);
	AddThread(Wthread);
	if (Rthread->Start()) return -1;
	if (Wthread->Start()) return -1;
	return 0;
}

/* 产生收发线程 */
int TcpCliThr::CreateThr(MyThread** Rthread, MyThread** Wthread)
{
	printf("Accept Conn\n");
	*Rthread = new Receiver(Mysocket, this);
	*Wthread = new Sender(Mysocket, this);
	return 0;
}

/* 默认的接收处理 */
void TcpCliThr::DealRecv(MyThread* thread)
{
	printf("Receiver is running!\n");
}

/* 默认的发送处理 */
void TcpCliThr::DealSend(MyThread* thread)
{
	printf("Sender is running!\n");
}

/* 将线程对象加入线程队列 */
void TcpCliThr::AddThread(MyThread* thread)
{
	if (thread == NULL) return;
	ThrSet->push_back(thread);
}

/* 将指定的线程对象从线程队列中删除 */
void TcpCliThr::DelThread(MyThread* thread) 
{
	vector<MyThread*>::iterator it = Thrset->begin();
	while (it != ThrSet->end()) {
		if (((MyThread*)*it) == thread) {
			ThrSet->erase(it);
			break;
		}
		it++;
	}
}

/* 等待线程队列中所有线程退出：扫描线程队列，等待每个线程退出 */
int TcpCliThr::WaitAllThr()
{
	vector<MyThread*>::iterator it = Thrset->begin();
	while (it != ThrSet->end()) {
		MyThread* thr = (MyThread*)(*it);
		pthread_join(thr_getId(), NULL);
		it++;
	}
	return 0;
}