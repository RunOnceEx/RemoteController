#pragma once
#include "afxwin.h"


// CAliveTimeDlg �Ի���

class CAliveTimeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAliveTimeDlg)

public:
	CAliveTimeDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAliveTimeDlg();

// �Ի�������
	enum { IDD = IDD_REFRESH_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_EditAliveTime;
	int m_nAliveTime;
};
