#pragma once

#include "resource.h"

// CNoticeDlg �Ի���

/************************************************************************
* @class CNoticeDlg
* @brief ��ʾ��Ϣ����
/************************************************************************/
class CNoticeDlg : public CDialog
{
	DECLARE_DYNAMIC(CNoticeDlg)

public:
	CNoticeDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CNoticeDlg();

// �Ի�������
	enum { IDD = IDD_NOTICE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_strNotice;
	virtual BOOL OnInitDialog();
};
