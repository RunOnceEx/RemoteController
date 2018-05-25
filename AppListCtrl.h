#pragma once
#include "afx.h"
#include "socket\SocketClient.h"
#include "AppInfo.h"
#include "cmdList.h"

// CAppListCtrl

class CAppListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CAppListCtrl)

	int m_nIndex;		// ��ǰѡ����

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

	void Lock() { EnterCriticalSection(&m_cs); }

	void Unlock() { LeaveCriticalSection(&m_cs); }

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
};
