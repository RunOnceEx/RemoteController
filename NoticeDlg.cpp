// NoticeDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RemoteController.h"
#include "NoticeDlg.h"
#include "afxdialogex.h"


// CNoticeDlg �Ի���

IMPLEMENT_DYNAMIC(CNoticeDlg, CDialog)

CNoticeDlg::CNoticeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNoticeDlg::IDD, pParent)
	, m_strNotice(_T(""))
{

}

CNoticeDlg::~CNoticeDlg()
{
}

void CNoticeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NOTICE, m_strNotice);
}


BEGIN_MESSAGE_MAP(CNoticeDlg, CDialog)
END_MESSAGE_MAP()


// CNoticeDlg ��Ϣ�������


BOOL CNoticeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDIT_NOTICE);
	// �������100����
	pEdit->SetLimitText(100);
	return TRUE;
}
