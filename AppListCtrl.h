#pragma once
#include "afx.h"
#include "socket\SocketClient.h"
#include "AppInfo.h"
#include "cmdList.h"
#include <map>

// �û��Զ�����Ϣ
enum UserMsg
{
	MSG_InsertApp = (WM_USER + 100), 
	MSG_UpdateApp, 
	MSG_DeleteApp, 
	MSG_ChangeColor, 
	MSG_Infomation,			// �յ���Ϣ
};

#define COLOR_DEFAULT 2047	// Ĭ��������ɫ
#define COLOR_RED 2048		// ��ɫ����:�����쳣������ʱ�ӽϴ�
#define COLOR_YELLOW 2049	// ��ɫ����:�����쳣
#define COLOR_BLUE1 2050	// ��ɫ����:�����Уʱ�쳣
#define COLOR_BLUE2 2051	// ����:ʱ����ϴ�

// �ַ���
class String
{
private:
	int *ref;
	char *buf;
	String& operator = (const String &o) { return *this; }

public:
	explicit String(const int size) : ref(new int(1)), buf(new char[size+1])
	{
	}
	String(const String &o)
	{
		ref = o.ref;
		buf = o.buf;
		++(*ref);
	}
	~String()
	{
		if (0 == --(*ref))
		{
			delete ref;
			delete [] buf;
		}
	}
	operator char*() const { return buf; }
	operator const char*() const { return buf; }
	char& operator [] (int i) { return buf[i]; }
	const char* tolower() { return _strlwr(buf); }
	const char* c_str() const { return buf; }
};

// ���W2A����
String W_2_A(const CString &wStr);

#define MENU_COUNT 7

// CAppListCtrl

class CAppListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CAppListCtrl)

	int m_nIndex;		// ��ǰѡ����

	std::map<int, int> m_MapStat; // �����˵�״̬

public:
	CAppListCtrl();
	virtual ~CAppListCtrl();

	// ��ʼ��������
	void AddColumns(const CString its[], int cols);

	// ƽ���ֲ�����
	void AvgColumnWidth(int cols);

	//////////////////////////////////////////////////////////////////////////
	// ����4�������漰���߳�

	// ������[m]
	void InsertAppItem(const char* port);

	// ɾ����[m]
	void DeleteAppItem(const char* port);

	// ������[m]
	void UpdateAppItem(const char* port, const AppInfo &it);

	// ���������[m]
	void Clear();

	CRITICAL_SECTION m_cs;

	inline void Lock() { EnterCriticalSection(&m_cs); }

	inline void Unlock() { LeaveCriticalSection(&m_cs); }

	std::string getCurSelNo();

	// ������ѡ����
	void SpyOnSelected(const char *no, int nPort);

	// ������Դ���˳��߳�
	void Uninit_ffplay(int nPort);

	int GetUdpPort(int base = 5555);

	struct ffplayInfo
	{
		int udp_port;
		time_t t;
		ffplayInfo(int port) : udp_port(port), t(time(NULL)) { }
		bool timeout(int timeouttime) const { return time(NULL) - t > timeouttime; }
		operator int() const { return udp_port; }
	};

	std::map<std::string, ffplayInfo> m_ffplayMap;// ÿ�����Ӷ�Ӧ��ffplay�˿�

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void RestartApp();
	afx_msg void QueryAppInfo();
	afx_msg void OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void StopApp();
	afx_msg void StartApp();
	afx_msg void UpdateApp();
	afx_msg LRESULT MessageInsertApp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT MessageUpdateApp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT MessageDeleteApp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT MessageChangeColor(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT MessageInfomation(WPARAM wParam, LPARAM lParam);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnOpRemote();
	afx_msg void OnUpdateOpRemote(CCmdUI *pCmdUI);
	afx_msg void OnOpSpy();
	afx_msg void OnUpdateOpSpy(CCmdUI *pCmdUI);
};
