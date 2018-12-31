#pragma once
#include "afxwin.h"

/************************************************************************
* @brief CDlgInfomation �Ի���
* @details ����ʱ�رնԻ���
************************************************************************/
class CDlgInfomation : public CDialog
{
	DECLARE_DYNAMIC(CDlgInfomation)

public:
	CDlgInfomation(const char *info, const char *details, int tm, CWnd* pParent = NULL);
	virtual ~CDlgInfomation();

// �Ի�������
	enum { IDD = IDD_INFOMATION_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_nTime;			// ms
	CString m_strInfo;
	CString m_strTitle;
	CStatic m_StaticInfo;

	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
