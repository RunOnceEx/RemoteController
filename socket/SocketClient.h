#pragma once

/** Copyright notice \n
* Copyright (c) 2018, BCECR
* All rights reserved.
* @file		SocketClient.h
* @brief	BCECR SOCKET CLIENT
* @version	1.0
* @date		2018/3/20
* @update	
* @author	yuanyuanxiang
*/

#include "SocketBase.h"
#include "RingBuffer.h"
#include "tinyxml\tinyxml.h"
#include <vector>
#include "..\AppInfo.h"

/** 
* @class	CSocketServer 
* @brief	socketͨ����ͻ���
* @details	ʵ�ֻ�������/�����ݵĹ���
*/
class CSocketClient : public CSocketBase
{
private:
	char m_strSrcPort[64];	// Դ�˿�
	char m_strName[64];		// ����
	int m_nSrcPort;			// Դ�˿�
	bool m_bExit;			// �Ƿ��˳�
	bool m_bAlive;			// �Ƿ񱣳�������
	bool m_bIsParsing;		// �����߳��Ƿ���
	bool m_bIsReceiving;	// �������߳��Ƿ���
	int m_nAliveTime;		// �������ڣ��룩
	clock_t m_tick;			// ��Ϣһ�����صļ�ʱ��

	char *m_RecvBuffer;			// �����ݻ���
	RingBuffer *m_RingBuffer;	// ������
	TiXmlDocument *m_xmlParser;
	AppInfo item;				// AppInfo

	static UINT WINAPI ParseThread(LPVOID param);
	static UINT WINAPI ReceiveThread(LPVOID param);

	/// ��MU��Ϣ
	void ReceiveData(const char *buffer, int nLength)
	{
		if(*buffer)
			m_RingBuffer->PushData(buffer, nLength);
	}

	/// ����MU��Ϣ
	void ParseData()
	{
		if (m_RingBuffer->GetXml(m_RecvBuffer, &m_bExit))
			ReadSipXmlInfo(m_RecvBuffer, m_RingBuffer->GetSipXmlLength());
	}

	// ��ȡSipXml
	void ReadSipXmlInfo(const char *buffer, int nLen);

protected:
	~CSocketClient(void); // ֻ��ͨ��new�����������

public:
	CSocketClient(SOCKET client, const char *Ip, int port);

	void Destroy() { delete this; }

	/// socket�˳�ʱ����������
	void unInit();

	// ������������
	void SetAliveTime(int nAliveTime) { m_nAliveTime = nAliveTime; }

	// ��¼��ǰʱ��
	void StartClock() { m_tick = clock(); }

	// ���Ϊ���˳�
	void SetExit() { m_bExit = true; m_bAlive = false; }

	/// ��ȡ���
	const char* GetNo() const { return m_strSrcPort; }
	int GetSrcPort() const { return m_nSrcPort; }

	/// ��ȡ����
	const char* Name() const { return m_strName; }

	/// ��ȡIP
	const char* GetIp() const { return m_chToIp; }

	/// �Ƿ�ʹ��
	bool IsAlive() const { return m_bAlive; }

	// �Ͽ�����
	void Disconnect();
};
