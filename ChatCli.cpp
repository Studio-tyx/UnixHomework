/* File: chatcli.cpp */
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <pthread.h> 
#include "MySocket.h"
#include "MyThread.h"
#include "TcpCliThr.h"

const int MAX_MESSAGE_LEN = 1000; /* 最大的消息长度 */
const int DEFAULT_PORT = 1234; /* 定义默认端口号 */

/******************************************************
class name : ChatC1ient
Function : A client used to chat
******************************************************/
class ChatClient : public TcpCliThr
{
public:
	ChatClient(int port, char* server) : TcpCliThr(port, server){}
	/* Must wait, or Client 'object will be destoried before execute —'TcpC1iThr() */
	~ChatClient() { WaitAllThr(); }
	void DealRecv(MyThread* thread); 
	void DealSend(MyThread* thread);
protected:
	char* getMessage(char* buf, int len, FILE* fp);
};

/* 反复从服务器接收数据。一旦收到则显示，如果出错则退出并关闭套接字 */
void ChatClient::DealRecv(MyThread* thread)
{
	char buf[MAX_MESSAGE_LEN + 1];
	int socket = ((Receiver *)thread)->socket;
	while (1) {
		int len = recv(socket, buf, MAX_MESSAGE_LEN, 0);
		if (len < 1) break;
		buf[len] = '\0';
		printf("\n%s", buf);
	}
	close(socket);
}

/* 首先让用户输入名字并将其发送给服务器。 */
/* 然后反复接收用户输入并将其发送给服务器。 */
void ChatClient::DealSend(MyThread* thread)
{
	char buf[MAX_MESSAGE_LEN + 1];
	int socket = ((Sender *)thread)->socket;
	printf("Connected to server. \n");
	/* send name to server */
	printf("Input name : ");
	if (fgets(buf, MAX_MESSAGE_LEN, stdin) == NULL) {
		printf("\nExit.\n");
		close(socket);
		return;
	}
	Send(buf, strlen(buf), 0);

	/* send message to server */
	while (getMessage(buf, MAX_MESSAGE_LEN, stdin) != NULL) {
		Send(buf, strlen(buf), 0);
	}
	printf("\nExit.\n");
	close(socket);
}

/* 获取用户输入 */
char* ChatClient::getMessage(char* buf, int len, FILE* fp)
{
	printf("Talk:");
	fflush(stdout);
	return (fgets(buf, MAX_MESSAGE_LEN, fp));
}

/************************main***************************/
/* 根据命令行参数，生成ChatClient对象，然后执行与服务器的连接操作。 */
int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Usage: %s <IP Address> <port>\n", argv[0]);
		exit(1);
	}
	if (argc == 2) {
		ChatClient chat(DEFAULT_PORT, argv[1]);
		chat.ConnectServ();
	}
	if (argc == 3) {
		ChatClient chat(atoi(argv[2]), argv[1]);
		chat.ConnectServ();
	}
}