
/** Copyright notice \n
* Copyright (c) 2018, BCECR
* All rights reserved.
* @file		SocketServer.h
* @brief	BCECR SOCKET SERVER
* @version	1.0
* @date		2018/3/20
* @update	
* @author	yuanyuanxiang
*/

#pragma once

#include "SocketBase.h"
#include <map>
#include "..\AppInfo.h"

class CSocketClient;

// �������Ӧ�Ĵ�����Ϣ
extern const char *pErrorCode[];

/** 
* @class	CSocketServer 
* @brief	socketͨ��������
* @details	ʵ�ֻ�������/�����ݵĹ���
*/
class CSocketServer : public CSocketBase
{
public:
	/// ���캯��
	CSocketServer();
	/// ����
	~CSocketServer();
	/// ��ʼ��socket
	int init(const char *pIp, int nPort, int nType); //0:server, 1:client
	/// socket�˳�ʱ����������
	void unInit();

	// ��APP������Ϣ[id==NULL��ʾȫ��]
	void SendCommand(const char *msg, const char *id = NULL);

	// ɾ������е�һ��
	void DeleteAppItem(CSocketClient *client);

	// ���±���е�һ��
	void UpdateAppItem(CSocketClient *client, const AppInfo &it);

private:
	bool m_bExit;		/**< �Ƿ��˳����� */
	bool m_bIsListen;	/**< �Ƿ��������߳� */

	CRITICAL_SECTION m_cs;

	void Lock() { EnterCriticalSection(&m_cs); }

	void Unlock() { LeaveCriticalSection(&m_cs); }

	/// ֻ���server�ˣ���������
	int CheckIO();
	/// ����Ƿ���������(����server)
	static UINT WINAPI CheckIOThread(LPVOID param);

public:
	CSocketClient *g_fd_ArrayC[MAX_LISTEN];/**< server�˴������еĴ������� */
};
