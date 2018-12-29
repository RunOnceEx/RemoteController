#include "SocketBase.h"
#include <process.h>


// �������Ӧ�Ĵ�����Ϣ
const char *pErrorCode[] =
{
	"no error",
	"WSAStartup() error",
	"socket() error",
	"connect() error",
	"bind() error",
	"listen() error",
	"ioctlsocket() error",
	"select() error",
	"accept() error",
	"��������",
	"init() type error", 
};

CSocketBase::CSocketBase(void)
{
	m_nType = 0;
	m_nToport = 0;
	m_nLocalPort = 0;

	m_Socket = INVALID_SOCKET;
	m_SocketListen = INVALID_SOCKET;

	memset(m_chLocalIp, 0, sizeof(m_chLocalIp));
	memset(m_chToIp, 0, sizeof(m_chToIp));
}


CSocketBase::~CSocketBase(void)
{
}


/** 
* @brief ��ʼ��һ�� socket
* @param[in] *pIp	�����IP
* @param[in] nPort	ͨ�Ŷ˿�
* @param[in] nType	socket���ͣ�0��server 1:client
* @retval	 int
* @return	 ������,��ͨ��GetErrorMsg()��ȡ������Ϣ  
*/
int CSocketBase::init(const char *pIp, int nPort, int nType)
{
	int nRet = _NO__ERROR;

	/// ����socket������������
	do 
	{
		/// ��ʼ��һ�������socket
		if (SocketType_Server == nType)
		{		
			/////////////////////////////////////////////////////////////////////////////////
			// �����׽ӿڵ�ѡ��
			// ��send()��ʱ�򣬷��ص���ʵ�ʷ��ͳ�ȥ���ֽ�(ͬ��)���͵�socket���������ֽ�
			// (�첽);ϵͳĬ�ϵ�״̬���ͺͽ���һ��Ϊ8688�ֽ�(ԼΪ8.5K)����ʵ�ʵĹ����з�������
			// �ͽ����������Ƚϴ󣬿�������socket����������������send(),recv()���ϵ�ѭ���շ�
			////////////////////////////////////////////////////////////////////////////////

			/// ���ͻ�����
			int nSendBuf = 1024 * 1024 * 2;
			::setsockopt(m_Socket, SOL_SOCKET, SO_SNDBUF, (char*)&nSendBuf, sizeof(int));
			/// ���ջ�����
			int nRecvBuf = 1024 * 1024 * 2;
			::setsockopt(m_Socket, SOL_SOCKET, SO_RCVBUF, (char*)&nRecvBuf, sizeof(int));
			bool bConditionalAccept = true;
			::setsockopt(m_Socket, SOL_SOCKET, SO_CONDITIONAL_ACCEPT, (const char *)&bConditionalAccept, sizeof(bool));

			/// ���������socket
			sockaddr_in addrServer;
			m_SocketListen = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
			if (m_SocketListen == INVALID_SOCKET)
			{
				nRet = ERROR_SOCKET;
				break;
			}
			/// �趨������IP �� ����PORT
			addrServer.sin_family = AF_INET;
			addrServer.sin_addr.S_un.S_addr = inet_addr(pIp);
			addrServer.sin_port = htons(nPort);
			/// ��SOCKET �� PORT
			if (SOCKET_ERROR == bind(m_SocketListen,(const sockaddr*)&addrServer, sizeof(addrServer)) )
			{
				nRet = ERROR_BIND;
				break;
			}
			/// �����˿�
			if (SOCKET_ERROR == listen(m_SocketListen, MAX_LISTEN))
			{
				nRet = ERROR_LISTEN;
				break;
			}
			/// ������ģʽ�趨
			ULONG ul = 1;
			if (SOCKET_ERROR == ioctlsocket(m_SocketListen, FIONBIO, &ul))
			{
				nRet = ERROR_IOCTLSOCKET;
				break;
			}
		}
		/// ��ʼ��һ���ͻ���socket
		else if (SocketType_Client == nType)
		{
			m_Socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
			if (INVALID_SOCKET == m_Socket)
			{
				nRet = ERROR_SOCKET;
				break;
			}
			/// ���ͻ�����
			int nSendBuf = 1024 * 1024 * 2;
			::setsockopt(m_Socket, SOL_SOCKET, SO_SNDBUF, (char*)&nSendBuf, sizeof(int));
			/// ���ջ�����
			int nRecvBuf = 1024 * 1024 * 2;
			::setsockopt(m_Socket, SOL_SOCKET, SO_RCVBUF, (char*)&nRecvBuf, sizeof(int));
			bool bConditionalAccept = true;
			::setsockopt(m_Socket, SOL_SOCKET, SO_CONDITIONAL_ACCEPT, (const char *)&bConditionalAccept, sizeof(bool));

			m_nToport = nPort;
			memcpy(m_chToIp, pIp, strlen(pIp));

			sockaddr_in addrAdpter;
			memset(&addrAdpter, 0, sizeof(addrAdpter));
			addrAdpter.sin_family = AF_INET;
			addrAdpter.sin_port = htons(nPort);
			addrAdpter.sin_addr.s_addr = inet_addr(pIp);

			/// �ͷ���˽�������
			nRet = ::connect(m_Socket, (sockaddr *)&addrAdpter, sizeof(addrAdpter));
			if (SOCKET_ERROR == nRet)
			{
				::closesocket(m_Socket);
				m_Socket = INVALID_SOCKET;

				nRet = ERROR_CONNECT;
				break;
			}

			//socketת��Ϊ������ģʽ,����ĳ����Ƶ�����������ж�ʱ,��ֹ��������
			ULONG ul = 1;   
			if (SOCKET_ERROR == ioctlsocket(m_Socket, FIONBIO, &ul))
			{
				nRet = ERROR_IOCTLSOCKET;
				break;
			}
		}
		else
			nRet = ERROR_TYPEERROR;
	} while (false);

	return nRet;
}


void CSocketBase::unInit()
{
	if (INVALID_SOCKET != m_Socket)
	{
		closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
	}

	if (INVALID_SOCKET != m_SocketListen)
	{
		closesocket(m_SocketListen);
		m_SocketListen = INVALID_SOCKET;
		OutputDebugStringA("\n======> CSocketBase Server Socket Closed.\n\n");
	}
}


/**
* @brief ��ȡ����
* @param[in] *pBuf			������
* @param[in] nReadLen		���ݳ���
* @param[in] nTimeOut		��ʱʱ��
* @return ���ز�������(���ڵ���0Ϊ�ɹ�)
* @retval int
*/
int CSocketBase::recvData(char *pBuf, int nReadLen, int nTimeOut)
{
	if ((INVALID_SOCKET == m_Socket) || (NULL == pBuf) || (0 == nReadLen))
		return -1;

	int ret = 0;
	struct timeval time;
	time.tv_sec = nTimeOut/1000;
	time.tv_usec = (nTimeOut%1000) * 1000;

	fd_set fd;
	FD_ZERO(&fd);
	FD_SET(m_Socket, &fd);

	pBuf[0] = '\0';
	ret = ::select(m_Socket+1, &fd, NULL, NULL, &time);
	if ( ret )
	{
		if ( FD_ISSET(m_Socket, &fd) )
		{
			ret = ::recv(m_Socket, pBuf, nReadLen, 0);
			ret = (ret <= 0) ? -1 : ret;
		}
	}
	else if(ret < 0)
	{
		ret = -1;
	}

	return ret;
}

/**
* @brief ��������
* @param[in] *pData			������
* @param[in] nSendLen		���ݳ���
* @return ���ز�������
* @retval int
*/
int CSocketBase::sendData(const char *pData, int nSendLen)
{
	if ((INVALID_SOCKET == m_Socket) || (NULL == pData) || (0 == nSendLen))
		return -1;

	int nRet = 0;
	int ret = 0;

	struct timeval time;
	time.tv_sec = 2;
	time.tv_usec =0;

	fd_set fdSend;
	int nLen = nSendLen;
	const char *pTmp = pData;

	while (nLen > 0)
	{
		FD_ZERO(&fdSend);
		FD_SET(m_Socket, &fdSend);

		ret = ::select(m_Socket+1, NULL, &fdSend, NULL, &time);
		if ( 1== ret )
		{
			if ( FD_ISSET(m_Socket, &fdSend) )
			{
				ret = ::send(m_Socket, pTmp, nLen, 0);
				if (ret <= 0)
				{
					nRet = -1;
					break;
				}

				nLen -= ret;
				pTmp += ret;

			}
		}
		else if ( ret < 0)
		{
			nRet = ret;
			break;
		}
		else if (0 == ret)
		{
			nRet = 0;
			break;
		}
	}

	return nRet;
}
