#pragma once
#include "afxwin.h"


// CDlgSetRemotePort �Ի���

/************************************************************************
* @class CDlgSetRemotePort
* @brief �����ػ��������ӵĶ˿�
/************************************************************************/
class CDlgSetRemotePort : public CDialog
{
	DECLARE_DYNAMIC(CDlgSetRemotePort)

public:
	CDlgSetRemotePort(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgSetRemotePort();

// �Ի�������
	enum { IDD = IDD_SET_REMOTEPORT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_EditPort;
	int m_nPort;
};
