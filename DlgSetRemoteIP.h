#pragma once
#include "afxwin.h"


// CDlgSetRemoteIP �Ի���

/************************************************************************
* @class CDlgSetRemoteIP
* @brief �����ػ�����Զ��IP
/************************************************************************/
class CDlgSetRemoteIP : public CDialog
{
	DECLARE_DYNAMIC(CDlgSetRemoteIP)

public:
	CDlgSetRemoteIP(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgSetRemoteIP();

// �Ի�������
	enum { IDD = IDD_SET_REMOTEIP_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_EditIP;
	CString m_strIP;
};
