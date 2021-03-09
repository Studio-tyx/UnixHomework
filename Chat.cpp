#include "Chat.h"
#include <stdio.h>
#include <vector>
#include <string.h>
#include <pthread.h>
#include "MySocket.h"
#include "MyThread.h"
#include "MySync.h"
#include "TcpServThr.h"

/* 构造函数：初始序列号为1，队列长度由参数len决定。初始化队列和互斥对象 */
MessageQue::MessageQue(int len) {
	lastSN = 1;
	queLen = len;
	queue = new vector<Message*>m;
	lock = new MyMutex;
}

/* 析构函数：首先扫描队列，删除所有字符串所占内存。然后删除队列和互斥对象 */
MessageQue::~MessageQue() {
	vector<Message*>::iterator it = queue->begin();
	while (it != queue->end()) {
		Message* m = (Message*)*it;
		delete m->message;
		delete m;
		it++;
	}
	delete queue;
	delete lock;
}

/* 首先产生一个消息结构，并填充相应信息（连接套接字、字符串） */
/* 然后加锁以修改关键的数据。 */
/* 将序列号+1，如果队列已满，则删除第一个信息。再将新的信息加入队列。*/
/* 最后，进行解锁 */
int MessageQue::Add(int conn, char* m) {
	if (m == NULL) return -1;
	Message *mes;
	mes = new Message;
	mes->connection = conn;
	mes->message = new char[(strlen(m)];
	strcpy(mes->message, (const char *)m);
	lock->Lock();
	mes->sn = lastSN;
	lastSN++;
	if (queue->size() == queLen) {
		vector<Message*>::iterator it = queue->begin();
		queue->erase(it);
	}
	queue->push_back(mes);
	lock->Unlock();
	return 0;
}

/* 首先加锁，扫描队列。 */
/* 如果消息的序列号大于当前客户所收到的最大序列号，并且该消息不是当前客户所发送的，则取出当前消息，并置错误码为0。 */
/* 最后，解锁并返回错误码 */
int MessageQue::Get(int conn, long* maxsn, char* m) {
	int err;
	err = 1;
	lock->Lock();
	vector<Message*>::iterator it = queue->begin();
	while (it != queue->end()) {
		Message* mes = (Message*)*it;
		if ((mes->sn > *maxsn) && (mes->connection != conn)) {
			strcpy(m, mes->message);
			*maxsn = mes->sn;
			m = m + strlen(mes->message) - 1;
			err = 0;
		}
		it++;
	}
	lock->Unlock();
	return err;
}

/* 构造函数：初始化最大连接数、服务器监听的端口号和地址、最大用户数、队列长度、队列及条件变量 */
/* 由于变量max_connections是私有的，必须通过公有的成员函数SetMaxConn()来修改 */
ChatServer::ChatServer() {
	SetMaxConn(MAXCONN);
	max_usrs = MAX_USRS;
	msg = new MessageQue(QUE_LEN);
	con = new MyCondition;
}

ChatServer::ChatServer(int port, char *hostname) :TcpServThr(port, hos ename) {
	SetMaxConn(MAXCONN);
	max_usrs = MAX_USRS;
	msg = new MessageQue(QUE_LEN);
	con = new MyCondition;
}

ChatServer::ChatServer(int port, int max_conn, int maxusr, int len, char *hostname) :
	TcpServThr(port, max_conn, hostname) {
	max_usrs = maxusr;
	queLen = len;
	msg = new MessageQue(queLen);
	con = new MyCondition;
}

/* 析构函数：首先，主线程等待其他所有线程终止，然后删除信息队列和条件变量 */
ChatServer::~ChatServer() {
	/*Wait until all threads exits */
	WaitAllThr();	// must execute it before —TcpServThr
	delete msg;
	delete con;
}

/* 首先接收客户名称，然后反复接收客户发来的字符串信息。 */
/* 一旦收到消息则加入到消息队列中，然后唤醒其他线程，以便将消息广播到其他客户。 */
/* 当用户终止连接后，调用DelThread将该线程从线程队列中删除 */
void ChatServer::DealRecv(MyThread* thread) {
	printf("Receiver is running! \n");
	char buf[MAX_PACKET_LEN];
	int socket = ((Receiver*)thread)->socket;
	int len = Recv(socket, buf, MAX_NAME_LEN, 0);
	buf[len - 1] = ':';
	buf[len] = '\0';
	printf("%s %d\n", buf, len);
	while (1) {
		int len1 = Recv(socket, buf + len, MAX_PACKET_LEN, 0);
		if (len1 < 1)break;
		buf[len1 + len] = '\0';
		printf("Recv:%s", buf);
		msg->Add(socket, buf);
		con->wakeAll();
	}
	DelThread(thread);
}

/* 首先，进入睡眠状态等待被唤醒。 */
/* 唤醒后从消息队列中取出需要发送的消息，然后发给客户。 */
/* 连接终止后，调用DelThread将该线程从消息队列中删除。 */
void ChatServer::DealSend(MyThread* thread) {
	char buf[MAX_PACKET_LEN];
	long maxsn;
	maxsn = msg->GetSN() - 1;
	int socket = ((Receiver*)thread)->socket;
	printf("Sender is running! \n");
	while (1) {
		con->wait();
		int err = msg->Get(socket, &maxsn, buf);
		if (err) continue;
		printf("Send:%s", buf);
		Send(socket, buf, strlen(buf), 0);
	}
	DelThread(thread);
}

/**************************main**************************/
/* 主程序：产生ChatServer对象chat，守候的端口号为1234，然后初始化并运行。 */
void main() {
	ChatServer chat(1234);
	chat.Init();
	chat.Run();
}
