#pragma once
/******************************************************
class name��TcpServThr
Function��support TCP Server with multithread
******************************************************/
class TcpServThr : public MySocket
{
	int max_connections; /* ������������ */
	vector<MyThread*> *ThrSet; /* ָ���̶߳��е�ָ�룬�ö��д�ŵ�ǰ���е��߳� */

public:
	/* ���幹�캯������������ */
	TcpServThr();
	TcpServThr(int port, char *hostname = NULL);
	TcpServThr(int port, int maxconn, char *hostname = NULL);
	virtual ~TcpServThr();

	void SetMaxConn(int num) { max_connections = num; } /* ������������� */
	int GetMaxConn() { return max_connections; } /* ��������������� */
	int Init(); /* ��ʼ��TCP���̷߳����������������׽��ֲ��� */
	int Run(); /* ����TCP���������� */

	 /* ������պͷ��͹��� */
	virtual void DealRecv(MyThread* thread);
	virtual void DealSend(MyThread* thread);

protected:
	int CreateThr(MyThread** Rthtread, MyThread** Wthread); /* �����շ��߳� */
	void AddThread(MyThread* thread); /* ���̼߳����̶߳��� */
	void DelThread(MyThread* thread); /* ���̴߳��̶߳�����ɾ�� */
	int WaitAllThr(); /* �����߳̽���ʱ��ͬ���������߳��˳�ǰ��Ӧ�ȴ����������߳��˳� */
};

/* ����Receiver�࣬���ڲ��������߳� */
class Receiver : public MyThread {
public:
	int socket;
	TcpServThr* server;
	Receiver(int connsocket, TcpServThr* serv) {
		socket = connsocket;
		server = serv;
	}
	void run() { server->DealRecv(this); }
};

/* ����Sender�࣬���ڲ��������߳� */
class Sender : public MyThread {
public:
	int socket;
	TcpServThr* server;
	Sender(int connsocket, TcpServThr* serv) {
		socket = connsocket;
		server = serv;
	}
	void run() {
		server->DealSend(this);
	}
};

