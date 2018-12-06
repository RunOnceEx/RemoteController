
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

	// ����Ӳ���豸���ر�/����/ʱ��ͬ��
	void ControlDevice(const char *msg);

	// �����������ڣ���ˢ��Ƶ�ʣ�
	void SetAliveTime(int msg, const char *id = NULL);

	CRITICAL_SECTION m_cs;

	void Lock() { EnterCriticalSection(&m_cs); }

	void Unlock() { LeaveCriticalSection(&m_cs); }

private:
	bool m_bExit;		/**< �Ƿ��˳����� */
	bool m_bIsListen;	/**< �Ƿ��������߳� */

	/// ֻ���server�ˣ���������
	int CheckIO();
	// ��ȡ���õ�Client
	int GetAvailabeClient();
	/// ����Ƿ���������(����server)
	static UINT WINAPI CheckIOThread(LPVOID param);
	/// ��ǰĿ¼�����г���İ汾����
	std::map<std::string, std::string> m_mapVersion;
	void GetAllSoftwareVersion();

public:
	// ����Ƿ���Ҫ���°汾������Ҫ�򷵻��°汾��
	std::string CheckUpdate(const char *app) const;

public:
	CSocketClient *g_fd_ArrayC[MAX_LISTEN];/**< server�˴������еĴ������� */
};
