#include "TcpServThr.h"
#include <stdio.h>
#include <vector.h>
#include <pthread.h>
#include "MySocket.h"
#include "MyThread.h"
using namespace std;

/* ʵ�ֹ��캯��������������TCP�׽�����ز����������̶߳��� */
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

/* ʵ���������������ȵȴ������߳��˳���Ȼ��ɨ���̶߳��У�ɾ�������̶߳������ɾ���̶߳��� */
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

/* ʵ��Init����������TCP�׽��ֵİ󶨺ͼ��� */
int TcpServThr::Init() {
	int opt = SO_REUSEADDR;
	setsockopt(Mysocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (Bind() == -1) return socket_error;
	if (Listen(max_connections) == -1) return socket_error;
	return 0;
}

/* ʵ��Run�������������տͻ����ӣ�һ�����ӽ�����������շ��̣߳��������Ǽ����̶߳��У����������� */
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

/* ʵ��CreateThr�����������շ��߳� */
int TcpServThr::CreateThr(MyThread** Rthread, MyThread** Wthread) {
	printf("Accept Conn\n");
	*Rthread = new Receiver(conn_socket, this);
	*Wthread = new Sender(conn_socket, this);
	return 0;
}

/* Ĭ�ϵĽ��մ��� */
void TcpServThr::DealRecv(MyThread* thread){
	printf("Receiver is running! \n");
}

/* Ĭ�ϵķ��ʹ��� */
void TcpServThr::DealSend(MyThread* thread) {
	printf("Sender is running! \n");
}

/* ���̶߳�������̶߳��� */
void TcpServThr::AddThread(MyThread* thread) {
	if (thread == NULL) return;
	ThrSet->push_back(thread);
}

/* ��ָ�����̴߳��̶߳�����ɾ�� */
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

/* �ȴ��̶߳����������߳��˳���ɨ���̶߳��У��ȴ�ÿ���߳��˳� */
int TcpServThr::WaitAllThr() {
	vector<MyThread*> ::iterator it = ThrSet->begin();
	while (it != ThrSet->end()) {
		MyThread* thr = (MyThread*)(*it);
		pthread_join(thr->getId(), NULL);
		it++;
	}
	return 0;
}