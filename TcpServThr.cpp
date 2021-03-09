#include "TcpServThr.h"
#include <stdio.h>
#include <vector.h>
#include <pthread.h>
#include "MySocket.h"
#include "MyThread.h"
using namespace std;

/* 实现构造函数，用于设置与TCP套接字相关参数及产生线程队列 */
TcpServThr::TcpServThr() {
	max_connections = MAXCONN;
	ThrSet = new vector<MyThread*>();
}

TcpServThr::TcpServThr(int port, char* hostname) :
	MySocket(AF_INET, SOCK_STREAM, 0, port, hostname) {
	max_connections = MAXCONN;
	ThrSet = new vector<MyThread*>();
}

TcpServThr::TcpServThr(int port, int maxconn, char* hostname) : 
	MySocket(AF_INET, SOCK_STREAM, 0, port, hostname) {
	max_connections = maxconn;
	ThrSet = new vector<MyThread*>();
}

/* 实现析构函数，首先等待所有线程退出，然后扫描线程队列，删除所有线程对象，最后删除线程队列 */
TcpServThr::~TcpServThr() {
	WaitAllThr();
	vector<MyThread*>::iterator it = ThrSet->begin();
	while (it != ThrSet->end()) {
		MyThread* thr = (MyThread*)(*it);
		delete thr;
		it++;
	}
	delete ThrSet;
}

/* 实现Init函数，包括TCP套接字的绑定和监听 */
int TcpServThr::Init() {
	int opt = SO_REUSEADDR;
	setsockopt(Mysocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (Bind() == -1) return socket_error;
	if (Listen(max_connections) == -1) return socket_error;
	return 0;
}

/* 实现Run函数，反复接收客户连接，一旦连接建立，则产生收发线程，并将它们加入线程队列，再运行它们 */
int TcpServThr :: Run(){
	while (Accept() != -1) {
		/*Create new thread */
		MyThread *Rthread, *Wthread;
		if (CreateThr(&Rthread, &Wthread) == -1) return -1;
		AddThread(Rthread);
		AddThread(Wthread);
		if (Rthread->Start()) return -1;
		if (Wthread->Start()) return -1;
	}
	return -1;
}

/* 实现CreateThr函数，产生收发线程 */
int TcpServThr::CreateThr(MyThread** Rthread, MyThread** Wthread) {
	printf("Accept Conn\n");
	*Rthread = new Receiver(conn_socket, this);
	*Wthread = new Sender(conn_socket, this);
	return 0;
}

/* 默认的接收处理 */
void TcpServThr::DealRecv(MyThread* thread){
	printf("Receiver is running! \n");
}

/* 默认的发送处理 */
void TcpServThr::DealSend(MyThread* thread) {
	printf("Sender is running! \n");
}

/* 将线程对象加入线程队列 */
void TcpServThr::AddThread(MyThread* thread) {
	if (thread == NULL) return;
	ThrSet->push_back(thread);
}

/* 将指定的线程从线程队列中删除 */
void TcpServThr::DelThread(MyThread* thread) {
	vector<MyThread*> ::iterator it = ThrSet->begin();
	while (it != ThrSet->end()) {
		if (((MyThread *)*it) == thread) {
			ThrSet->erase(it);
			break;
		}
		it++;
	}
}

/* 等待线程队列中所有线程退出：扫描线程队列，等待每个线程退出 */
int TcpServThr::WaitAllThr() {
	vector<MyThread*> ::iterator it = ThrSet->begin();
	while (it != ThrSet->end()) {
		MyThread* thr = (MyThread*)(*it);
		pthread_join(thr->getId(), NULL);
		it++;
	}
	return 0;
}