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

	// ������
	void AddColumns(const CString its[], int cols);

	// ������
	void InsertAppItem(const char* port);

	// ɾ����
	void DeleteAppItem(const char* port);

	// ������
	void UpdateAppItem(const char* port, const AppInfo &it);

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
	afx_msg void UpdateApp();
};
