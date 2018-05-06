#include "SocketClient.h"
#include <process.h>
#include "..\AppListCtrl.h"
#include "SocketServer.h"

#ifndef X64
#ifdef _DEBUG
#pragma comment(lib, "Debug/tinyxml.lib")
#else
#pragma comment(lib, "Release/tinyxml.lib")
#endif
#else
#endif

extern CAppListCtrl *g_pList;

#ifndef BUFFER_LENGTH
	#define BUFFER_LENGTH 4096
#endif

// ����������߳�
UINT WINAPI CSocketClient::ParseThread(LPVOID param)
{
	CSocketClient *pThis = (CSocketClient*)param;
	pThis->m_bIsParsing = true;
	while (pThis->m_bAlive)
	{
		Sleep(10);
		pThis->ParseData();
	}
	pThis->m_bIsParsing = false;
	return 0xDead00A1;
}


// ��ȡAPP���ݵ��߳�
UINT WINAPI CSocketClient::ReceiveThread(LPVOID param)
{
	CSocketClient *pThis = (CSocketClient*)param;
	pThis->m_bIsReceiving = true;
	while (pThis->m_bAlive)
	{
		Sleep(10);
		char buffer[BUFFER_LENGTH];
		int nRet = pThis->recvData(buffer, BUFFER_LENGTH);
		(nRet < 0) ? pThis->Disconnect() : pThis->ReceiveData(buffer, nRet);
	}
	pThis->m_bIsReceiving = false;
	return 0xDead00A2;
}


/**
* @brief �µ�Socket�ͻ���
* @param[in] client ͨ��socket
* @param[in] *Ip �ͻ���Ip
* @param[in] port �ͻ��˶˿�
*/
CSocketClient::CSocketClient(SOCKET client, const char *Ip, int port)
{
	m_nType = 1;
	m_Socket = client;
	strcpy_s(m_chToIp, Ip);
	m_nToport = port;
	m_bExit = false;
	m_bAlive = true;
	m_bIsParsing = false;
	m_bIsReceiving = false;
	m_RingBuffer = new RingBuffer(2 * 1024 * 1024);
	m_RecvBuffer = new char[BUFFER_LENGTH];
	m_xmlParser = NULL;
	sprintf_s(m_strPort, "%d", port);
	memset(m_strName, 0, 64);
	_beginthreadex(NULL, 0, &ParseThread, this, 0, NULL);
	_beginthreadex(NULL, 0, &ReceiveThread, this, 0, NULL);
	char str[256];
	sprintf_s(str, "�����µ�Socket�ͻ���: [%d] %s:%d.\n", m_Socket, m_chToIp, m_nToport);
	OutputDebugStringA(str);
}


CSocketClient::~CSocketClient(void)
{
	delete m_RingBuffer;
	delete [] m_RecvBuffer;
	if (NULL != m_xmlParser)
		delete m_xmlParser;
}


void CSocketClient::unInit()
{
	m_bExit = true;
	m_bAlive = false;
	while (m_bIsParsing || m_bIsReceiving)
		Sleep(1);

	CSocketBase::unInit();
}


void CSocketClient::Disconnect()
{
	char str[256];
	sprintf(str, "Socket�ͻ���: [%d] %s:%d �ر�.\n", m_Socket, m_chToIp, m_nToport);
	g_pList->DeleteAppItem(m_strPort);
	OutputDebugStringA(str);
	m_bExit = true;
	m_bAlive = false;
}

// ��ȡ���ڵ����Ϊname���ӽڵ��ֵ
inline const char* GetValue(const TiXmlElement* pParent, const char* pName)
{
	const TiXmlElement* pChild = pParent ? pParent->FirstChildElement(pName) : NULL;
	const char *text = pChild ? pChild->GetText() : "";
	return text ? text : "";
}

/**
<?xml version="1.0" encoding="GB2312" standalone="yes"?>
<request command="KeepAlive">
<parameters>
	<szName>%s</szName>
	<szCpu>%s</szCpu>
	<szMem>%d</szMem>
	<szThreads>%s</szThreads>
	<szHandles>%d</szHandles>
	<szRunLog>%s</szRunLog>
	<szRunTimes>%d</szRunTimes>
	<szCreateTime>%s</szCreateTime>
	<szModTime>%s</szModTime>
	<szVersion>%s</szVersion>
	<szKeeperVer>%s</szKeeperVer>
	<szCmdLine>%s</szCmdLine>
</parameters>
</request>
*/
void CSocketClient::ReadSipXmlInfo(const char *buffer, int nLen)
{
	char seq[32] = { 0 }, cid[32] = { 0 };
	const char *xml = GetSeqAndCallId(buffer, seq, cid);
	if (0 == *xml)
		return;
	OutputDebugStringA(xml);
	if (NULL == m_xmlParser)
		m_xmlParser = new TiXmlDocument();
	m_xmlParser->Parse(xml, 0, TIXML_DEFAULT_ENCODING);
	TiXmlElement* rootElement = m_xmlParser->RootElement();
	if (NULL == rootElement)
	{
		m_xmlParser->Clear();
		return;
	}
	TiXmlAttribute* attri = rootElement->FirstAttribute();
	const char *cmdType = attri ? attri->Value() : NULL;// command����
	if (NULL == cmdType)
	{
		m_xmlParser->Clear();
		return;
	}
	TiXmlElement* parameters = rootElement->FirstChildElement("parameters");
	if (NULL == rootElement)
	{
		m_xmlParser->Clear();
		return;
	}
	if (0 == strcmp("KeepAlive", cmdType))
	{
		AppInfo item;
		strcpy_s(item.ip, m_chToIp);
		strcpy_s(item.name, GetValue(parameters, "szName"));
		strcpy_s(item.cpu, GetValue(parameters, "szCpu"));
		strcpy_s(item.mem, GetValue(parameters, "szMem"));
		strcpy_s(item.threads, GetValue(parameters, "szThreads"));
		strcpy_s(item.handles, GetValue(parameters, "szHandles"));
		strcpy_s(item.run_log, GetValue(parameters, "szRunLog"));
		strcpy_s(item.run_times, GetValue(parameters, "szRunTimes"));
		strcpy_s(item.create_time, GetValue(parameters, "szCreateTime"));
		strcpy_s(item.mod_time, GetValue(parameters, "szModTime"));
		strcpy_s(item.version, GetValue(parameters, "szVersion"));
		strcpy_s(item.keep_ver, GetValue(parameters, "szKeeperVer"));
		strcpy_s(item.cmd_line, GetValue(parameters, "szCmdLine"));
		g_pList->UpdateAppItem(m_strPort, item);
		sendData(KEEPALIVE, strlen(KEEPALIVE));
	}else if(0 == strcmp("Register", cmdType))
	{
		/**
		<?xml version="1.0" encoding="GB2312" standalone="yes"?>
		<request command="Register">
		  <parameters>
		    <szAppId>%s</szAppId>
		    <szPassword>%s</szPassword>
		  </parameters>
		</request>
		*/
		// �ظ�ע��ɹ�
		std::string cmd = MAKE_CMD(REGISTER, "success");
		sendData(cmd.c_str(), cmd.length());
	}

	m_xmlParser->Clear();
}
