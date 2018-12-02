#pragma once
#pragma once
#include <WinSock2.h>
#include <process.h>

#define MAX_CONNECT 100

// ����ָ��Ĺ̶�����
#define SIZE_1 100

// req					| resp
// (1) size:file		| size:bytes,md5
// (2) down:file		| ......

// ƴ������ָ��
#define make_cmd(buf, cmd, arg) {memset(buf,0,SIZE_1); sprintf_s(buf,"%s:%s",cmd,arg);}

// ����ָ��
#define parse_cmd(buf, arg) {arg=buf; while(*arg && ':'!=*arg)++arg; if(*arg)*arg++=0;}

// Ŀ¼������[��Ա�ױ��]
typedef struct folder
{
	char buf[_MAX_PATH], *pos;
	folder()
	{
		// ��ȡ����ǰĿ¼
		GetModuleFileNameA(NULL, buf, sizeof(buf));
		pos = buf;
		while ('\0' != *pos) ++pos;
		while ('\\' != *pos) --pos;
		++pos;
	}
	// ��ȡ��ǰĿ¼�µ��ļ�
	inline const char* get(const char *file)
	{
		strcpy(pos, file);
		return buf;
	}
}folder;

struct SocketInfo
{
	SOCKET s;					// socket
	int total;					// �ļ����ֽ�
	bool flag;					// �߳�״̬
	int step[2];				// ֪ͨ
	char *buf;					// ���ļ�����
	char file[_MAX_PATH];		// �ļ�ȫ·��
	SocketInfo() : s(INVALID_SOCKET), total(0), flag(false), buf(NULL)
	{
		step[0] = step[1] = 0;
	}
	~SocketInfo() { if (buf) delete [] buf; }
	void callback(const char *data, int len); // �յ�����ʱ�ص�
	void processing();
	void exit() {
		closesocket(s);
		s = INVALID_SOCKET;
		while(flag)
			Sleep(10);
		step[0] = step[1] = 0;
		total = 0;
	}
	void start(){
		HANDLE h = NULL;
		flag = h = (HANDLE)_beginthreadex(0, 0, ParseDataThread, this, 0, 0);
		CloseHandle(h);
	}
	static UINT WINAPI ParseDataThread(void *param);
};

class UpdateServer
{
private:
	char m_strIp[64];
	int m_nPort;
	SOCKET m_Socket;
	bool m_bExit;					// ��������״̬
	bool m_AcceptThread;			// �߳�״̬
	bool m_RecvDataThread;			// �߳�״̬
	int nConnNum;					// ������
	SocketInfo g_queue[MAX_CONNECT];	/**< server�˴������еĴ������� */
	CRITICAL_SECTION m_cs;

	void CheckIO();

	bool add_client(SOCKET s);

	int ThisRecvDataProc();

public:
	UpdateServer(void);
	~UpdateServer(void);

	// ��ʼ��socket
	int init(const char *pIp, int nPort);

	// socket�˳�ʱ����������
	void unInit();

	static UINT WINAPI AcceptThread(void *param);

	static UINT WINAPI RecvDataThread(void *param);
};
