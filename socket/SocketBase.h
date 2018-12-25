#pragma once

#include "../stdafx.h"
#include <assert.h>

#ifdef MAX_LISTEN
#undef MAX_LISTEN
#endif

#ifdef _DEBUG
/// �������ͻ�����
#define MAX_LISTEN 64
#else
#define MAX_LISTEN 256
#endif

/// Ĭ�ϻ�������С
#define DEFAULT_BUFFER 256

// �������Ӧ�Ĵ�����Ϣ
extern const char *pErrorCode[];

enum 
{
	SocketType_Server = 0, 
	SocketType_Client, 
};

/************************************************************************/
/* socket �����붨��                                                    */
/************************************************************************/
enum 
{
	_NO__ERROR = 0,		/// no error
	ERROR_WSASTARTUP,	/// WSAStartup() error
	ERROR_SOCKET,		/// socket() error
	ERROR_CONNECT,		/// connect() error
	ERROR_BIND,			/// bind() error
	ERROR_LISTEN,		/// listen() error
	ERROR_IOCTLSOCKET,	/// ioctlsocket() error
	ERROR_SELECT,		/// select() error
	ERROR_ACCEPT,		/// accept() error
	ERROR_CLIENTFULL,	/// ��������
	ERROR_TYPEERROR,	/// init() type error
	ERROR_PORTERROR,	/// port�ѱ�ռ��
	ERROR_MAX
};

/**
* @class CSocketBase
* @brief Socket���࣬ʵ�ֻ������շ����ݹ��ܣ��μ�BcecrSocket.
*/
class CSocketBase
{
public:
	CSocketBase(void);
	~CSocketBase(void);

	/// ��ʼ��socket[�麯��]
	virtual int init(const char *pIp, int nPort, int nType); //0:server, 1:client
	/// socket�˳�ʱ����������[�麯��]
	virtual void unInit();
	/// ��������
	int recvData(char *pBuf, int nReadLen, int nTimeOut = 1000); //nTimeOut��λ����
	/// ��������
	int sendData(const char *pData, int nSendLen);
	/// ͨ���������ȡ������Ϣ
	static const char* GetErrorMsg(int nRet) { assert(nRet >= 0 && nRet < ERROR_MAX); return pErrorCode[nRet]; }

protected:
	int m_nType;			/**< socket���ͣ�0:server, 1:client */

	SOCKET m_Socket;		/**< ��Ϊ�ͻ������ӵ�socket */
	SOCKET m_SocketListen;	/**< ��Ϊ���������ӵ�socket */
	fd_set fdRead;			/**< �������˽������� */

public:
	char m_chToIp[32];				/**< �Է���IP */
	int  m_nToport;					/**< �Է��Ķ˿� */
	char m_chLocalIp[32];			/**< ����IP */
	int  m_nLocalPort;				/**< ���ض˿� */
};
