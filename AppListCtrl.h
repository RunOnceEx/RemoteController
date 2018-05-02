#pragma once
#include "afx.h"
#include "socket\SocketClient.h"
#include "AppInfo.h"

// CAppListCtrl

class CAppListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CAppListCtrl)

	int m_nIndex;		// ��ǰѡ����

	CRITICAL_SECTION cs;

	void Lock() { EnterCriticalSection(&cs); }

	void Unlock() { LeaveCriticalSection(&cs); }

public:
	CAppListCtrl();
	virtual ~CAppListCtrl();

	// ������
	void AddColumns(const CString its[], int cols);

	// ������
	void InsertAppItem(CSocketClient *client);

	// ɾ����
	void DeleteAppItem(CSocketClient *client);

	// ������
	void UpdateAppItem(CSocketClient *client, const AppInfo &it);

	// ƽ���ֲ�����
	void AvgColumnWidth(int cols);

	void Clear();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void RestartApp();
	afx_msg void QueryAppInfo();
	afx_msg void OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void StopApp();
	afx_msg void StartApp();
};
