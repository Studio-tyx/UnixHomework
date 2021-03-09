#pragma once
const int MAX_USRS = 10; /* ����û��� */
const int QUE_LEN = 10; /* ��Ϣ���г��� */
const int MAX_PACKET_LEN = 1000; /* �������� */
const int MAX_NAME_LEN = 100; /* ������󳤶� */

/* �ṹ��Message */
struct Message {
	long sn; /* �����Ϣ���к� */
	int connection; /* ��������׽��� */
	char* message; /* ��ſͻ����͵��ַ��� */
};

/******************************************************
class name: MessageQue
function: A queue for message
******************************************************/
/* �������һ����Ϣ���У����Ҹö����ɻ���������б��� */
class MessageQue {
	long lastSN; /* ����������к� */
	int queLen; /* ��ŵ�ǰ���г��� */
	vector<Message*> *queue; /* ָ��Message���� */
	MyMutex* lock; /* ��Ż������ */
public:
	MessageQue(int len);
	~MessageQue();

	int Add(int conn, char* m); /* �Ӷ�����������Ϣ */
	int Get(int conn, long* maxsn, char* m); /* �Ӷ�����ȡ����Ϣ */
	long GetSN() { return lastSN; } /* ȡ����ǰ���к� */
	void SetSN(int sn) { lastSN = sn; } /* ���õ�ǰ���к� */
	int GetQueLen() { return queLen; } /* ��ö��г��� */
	void SetQueLen(int l) { queLen = l; } /* ���ö��г��� */
};

/******************************************************
class name: ChatServer
function: A server used to chat with multiple users
******************************************************/
/* ����TCP�Ķ��̷߳����������ڽ�����ͬ�ͻ��������Ϣ������ɶԻ����� */
class ChatServer :public TcpServThr {
	int max_usrs; /* ����û��� */
	int queLen; /* ���г��� */
	MessageQue *msg; /* ָ����е�ָ�� */
	MyCondition *con; /* ָ����������������ɶ��̼߳��ͬ�� */
public:
	/* ���캯�� */
	ChatServer();
	ChatServer(int port, char *hostname = NULL);
	ChatServer(int port, int max_conn, int maxusers, int len, char *hostname = NULL);

	virtual ~ChatServer(); /* �������� */
	void SetMaxUser(int num) { max_usrs = num; } /* ��������û��� */
	int GetMaxUser() { return max_usrs; } /* ��ȡ����û��� */
	void DealRecv(MyThread* thread); /* ����������� */
	void DealSend(MyThread* thread); /* ���������� */
	void SetQueLen(int l) { queLen = l; } /* ���ö��г��� */
	int GetQueLen() { return queLen; } /* ��ö��г��� */
};
