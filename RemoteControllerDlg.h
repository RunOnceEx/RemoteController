
// RemoteControllerDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "AppListCtrl.h"
#include "socket\SocketServer.h"
#include "Resource.h"
#include "UpdateServer.h"


extern CSocketServer *g_pSocket;

extern const int g_Width[COLUMNS];

// CRemoteControllerDlg �Ի���
class CRemoteControllerDlg : public CDialog
{
// ����
public:
	CRemoteControllerDlg(CWnd* pParent = NULL);	// ��׼���캯��

	~CRemoteControllerDlg();

	// �Ի�������
	enum { IDD = IDD_REMOTECONTROLLER_DIALOG };

	bool m_bExit;				// �˳���־
	char m_strIp[64];			// ����IP
	int m_nPort;				// Ip�˿�
	int m_nGhost;				// ghost����˶˿ڣ�<_BASE_PORT��
	char m_strUp[52];			// ������ַ
	char m_strConf[_MAX_PATH];	// �����ļ�
	bool m_bAdvanced;			// ���ø߼�����
	bool m_bDetectTime;			// ���ϵͳʱ��
	bool m_bAllowDebug;			// ������򽵼�
	CString m_sPicPath;			// ���ͼƬ��Ŀ¼
	CSocketServer *m_pServer;	// socket �����
	UpdateServer *m_pUpServer;	// ��������������

	void StartUpServer();

	CRITICAL_SECTION m_cs;		// δ�õ�
	bool IsDetectTimeError() const { return m_bDetectTime; }
	void Lock() { EnterCriticalSection(&m_cs); }
	void Unlock() { LeaveCriticalSection(&m_cs); }

	// ��ָ���˿ڵ�udp����ȡ�䴰��
	std::string udp(int port=_BASE_PORT) const 
	{
		char u[100];
		sprintf_s(u, "udp://%s:%d", m_strIp, port);
		return u;
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
	HICON m_hIcon;
	HACCEL m_hAcc;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CRect m_rect;
	CAppListCtrl m_ListApps;
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnQuitApp();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnRefreshAll();
	afx_msg void OnPoweroffAll();
	afx_msg void OnIpconfig();
	afx_msg void OnAppAbout();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnUpdatePoweroffAll(CCmdUI *pCmdUI);
	afx_msg void OnRebootSystem();
	afx_msg void OnUpdateRebootSystem(CCmdUI *pCmdUI);
	afx_msg void OnUpdateRefreshAll(CCmdUI *pCmdUI);
	afx_msg void OnUpdate();
	afx_msg void OnSettime();
	afx_msg void OnStopall();
	afx_msg void OnUpdateStopall(CCmdUI *pCmdUI);
	afx_msg void OnStartall();
	afx_msg void OnUpdateStartall(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSingle();
	afx_msg void OnUpdateSettime(CCmdUI *pCmdUI);
	afx_msg void OnUpdateUpdate(CCmdUI *pCmdUI);
	afx_msg void OnUpdateUpdateSingle(CCmdUI *pCmdUI);
	afx_msg void OnSetAlivetime();
	afx_msg void Screenshot(); // ��ͼ�������
	afx_msg void OnUpdateScreenshot(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSetAlivetime(CCmdUI *pCmdUI);
	afx_msg void OnSelectSettime();
	afx_msg void OnUpdateSelectSettime(CCmdUI *pCmdUI);
	afx_msg void OnSetUpserver();
	afx_msg void OnDetectTimeError();
	afx_msg void OnUpdateDetectTimeError(CCmdUI *pCmdUI);
	afx_msg void OnNotice();
	afx_msg void OnUpdateNotice(CCmdUI *pCmdUI);
	afx_msg void OnAllowDebug();
	afx_msg void OnUpdateAllowDebug(CCmdUI *pCmdUI);
	afx_msg void OnSetRemoteip();
	afx_msg void OnUpdateSetRemoteip(CCmdUI *pCmdUI);
	afx_msg void OnSetRemoteport();
	afx_msg void OnUpdateSetRemoteport(CCmdUI *pCmdUI);
	afx_msg void OnSpy();
	afx_msg void OnUpdateSpy(CCmdUI *pCmdUI);
	afx_msg void OnStartGhost();
	afx_msg void OnUpdateStartGhost(CCmdUI *pCmdUI);
	afx_msg void OnStopGhost();
	afx_msg void OnUpdateStopGhost(CCmdUI *pCmdUI);
	afx_msg void OnAccelRefresh();
	afx_msg void OnUpdateAccelRefresh(CCmdUI *pCmdUI);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnGhostPort();
	afx_msg void OnUpdateGhostPort(CCmdUI *pCmdUI);
	afx_msg void OnShowYama();
	afx_msg void OnUpdateShowYama(CCmdUI *pCmdUI);
	afx_msg void OnAccelStop();
	afx_msg void OnAccelDebug();
	afx_msg void OnAccelYama();
	afx_msg void OnAccelWatch();
	afx_msg void OnAccelNotice();
};

extern CRemoteControllerDlg *g_MainDlg;
