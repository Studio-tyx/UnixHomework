#pragma once
/******************************************************
class name: TcpCliThr
Function: support TCP client with multithread
******************************************************/
class TcpCliThr : public MySocket
{
	vector<MyThread*>* ThrSet; /* ����̶߳��� */

public:
	/* ���幹�캯�� */
	TcpCliThr();
	TcpCliThr(int port, char *server);

	virtual ~TcpCliThr();/* ������������ */

	int ConnectServ(); /* �����������������*/

	/* ������պͷ��͹��� */
	virtual void DealRecv(MyThread* thread); 
	virtual void DealSend(MyThread* thread);

protected:
	int CreateThr(MyThread** Rthread, MyThread** Wthread); /* �����շ��߳� */
	void AddThread(MyThread* thread); /* ���̼߳����̶߳��� */
	void DelThread(MyThread* thread); /* ���̴߳��̶߳�����ɾ�� */
	int WaitAllThr(); /* �����߳̽���ʱ��ͬ���������߳��˳�ǰ��Ӧ�ȴ������߳��˳� */

};

/* ����Receiver�࣬���ڲ��������߳� */
class Receiver : public MyThread
{
public:
	int socket;
	TcpCliThr* server;
	Receiver(int connsocket, TcpCliThr* serv) {
		socket = connsocket;
		server = serv;
	}
	void run() { server->DealRecv(this); }
};

/* ����Sender�࣬���ڲ��������߳� */
class Sender : public MyThread
{
public:
	int socket;
	TcpCliThr* server;
	Sender(int connsocket, TcpCliThr* serv) {
		socket = connsocket;
		server = serv;
	}
	void run() { server->DealSend(this); }
};
