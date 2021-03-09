#pragma once
const int MAX_USRS = 10; /* 最大用户数 */
const int QUE_LEN = 10; /* 信息队列长度 */
const int MAX_PACKET_LEN = 1000; /* 最大包长度 */
const int MAX_NAME_LEN = 100; /* 名字最大长度 */

/* 结构体Message */
struct Message {
	long sn; /* 存放信息序列号 */
	int connection; /* 存放连接套接字 */
	char* message; /* 存放客户发送的字符串 */
};

/******************************************************
class name: MessageQue
function: A queue for message
******************************************************/
/* 该类包括一个信息队列，并且该队列由互斥变量进行保护 */
class MessageQue {
	long lastSN; /* 存放最新序列号 */
	int queLen; /* 存放当前队列长度 */
	vector<Message*> *queue; /* 指向Message队列 */
	MyMutex* lock; /* 存放互斥变量 */
public:
	MessageQue(int len);
	~MessageQue();

	int Add(int conn, char* m); /* 从队列中增加信息 */
	int Get(int conn, long* maxsn, char* m); /* 从队列中取出信息 */
	long GetSN() { return lastSN; } /* 取出当前序列号 */
	void SetSN(int sn) { lastSN = sn; } /* 设置当前序列号 */
	int GetQueLen() { return queLen; } /* 获得队列长度 */
	void SetQueLen(int l) { queLen = l; } /* 设置队列长度 */
};

/******************************************************
class name: ChatServer
function: A server used to chat with multiple users
******************************************************/
/* 基于TCP的多线程服务器，用于交换不同客户输入的信息，以完成对话功能 */
class ChatServer :public TcpServThr {
	int max_usrs; /* 最大用户数 */
	int queLen; /* 队列长度 */
	MessageQue *msg; /* 指向队列的指针 */
	MyCondition *con; /* 指向条件变量，以完成多线程间的同步 */
public:
	/* 构造函数 */
	ChatServer();
	ChatServer(int port, char *hostname = NULL);
	ChatServer(int port, int max_conn, int maxusers, int len, char *hostname = NULL);

	virtual ~ChatServer(); /* 析构函数 */
	void SetMaxUser(int num) { max_usrs = num; } /* 设置最大用户数 */
	int GetMaxUser() { return max_usrs; } /* 获取最大用户数 */
	void DealRecv(MyThread* thread); /* 处理接收数据 */
	void DealSend(MyThread* thread); /* 处理发送数据 */
	void SetQueLen(int l) { queLen = l; } /* 设置队列长度 */
	int GetQueLen() { return queLen; } /* 获得队列长度 */
};
