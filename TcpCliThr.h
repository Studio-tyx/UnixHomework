#pragma once
/******************************************************
class name: TcpCliThr
Function: support TCP client with multithread
******************************************************/
class TcpCliThr : public MySocket
{
	vector<MyThread*>* ThrSet; /* 存放线程队列 */

public:
	/* 定义构造函数 */
	TcpCliThr();
	TcpCliThr(int port, char *server);

	virtual ~TcpCliThr();/* 定义析构函数 */

	int ConnectServ(); /* 用于与服务器相连接*/

	/* 处理接收和发送过程 */
	virtual void DealRecv(MyThread* thread); 
	virtual void DealSend(MyThread* thread);

protected:
	int CreateThr(MyThread** Rthread, MyThread** Wthread); /* 产生收发线程 */
	void AddThread(MyThread* thread); /* 将线程加入线程队列 */
	void DelThread(MyThread* thread); /* 将线程从线程队列中删除 */
	int WaitAllThr(); /* 用于线程结束时的同步。即主线程退出前，应等待其他线程退出 */

};

/* 定义Receiver类，用于产生接收线程 */
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

/* 定义Sender类，用于产生发送线程 */
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
