#pragma once
#include "afxwin.h"


// CUpdateServerDlg �Ի���

class CUpdateServerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CUpdateServerDlg)

public:
	CUpdateServerDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CUpdateServerDlg();

// �Ի�������
	enum { IDD = IDD_UPSERVER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_EditUpServer;
	CString m_strUpServer;
};
