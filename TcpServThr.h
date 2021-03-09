#pragma once
/******************************************************
class name：TcpServThr
Function：support TCP Server with multithread
******************************************************/
class TcpServThr : public MySocket
{
	int max_connections; /* 最大的连接数量 */
	vector<MyThread*> *ThrSet; /* 指向线程队列的指针，该队列存放当前所有的线程 */

public:
	/* 定义构造函数及析构函数 */
	TcpServThr();
	TcpServThr(int port, char *hostname = NULL);
	TcpServThr(int port, int maxconn, char *hostname = NULL);
	virtual ~TcpServThr();

	void SetMaxConn(int num) { max_connections = num; } /* 设置最大连接数 */
	int GetMaxConn() { return max_connections; } /* 获得最大的连接数量 */
	int Init(); /* 初始化TCP多线程服务器，产生监听套接字并绑定 */
	int Run(); /* 运行TCP，建立连接 */

	 /* 处理接收和发送过程 */
	virtual void DealRecv(MyThread* thread);
	virtual void DealSend(MyThread* thread);

protected:
	int CreateThr(MyThread** Rthtread, MyThread** Wthread); /* 产生收发线程 */
	void AddThread(MyThread* thread); /* 将线程加入线程队列 */
	void DelThread(MyThread* thread); /* 将线程从线程队列中删除 */
	int WaitAllThr(); /* 用于线程结束时的同步，即主线程退出前，应等待所有其他线程退出 */
};

/* 定义Receiver类，用于产生接收线程 */
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

/* 定义Sender类，用于产生接收线程 */
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

