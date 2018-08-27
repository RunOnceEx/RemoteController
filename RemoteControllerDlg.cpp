
// RemoteControllerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RemoteController.h"
#include "RemoteControllerDlg.h"
#include "afxdialogex.h"
#include "IPConfigDlg.h"
#include "AliveTimeDlg.h"
#include "UpdateServerDlg.h"

#include <DbgHelp.h>
#include <io.h>
#include <direct.h>
#pragma comment(lib, "Dbghelp.lib")

/** 
* @brief ��������δ֪BUG������ֹʱ���ô˺�����������
* ����ת��dump�ļ���dumpĿ¼.
*/
long WINAPI whenbuged(_EXCEPTION_POINTERS *excp)
{
	// ����ļ��в����ڣ��򴴽�֮
	if (_access("./dump", 0) == -1)
		_mkdir("./dump");

	char curTime[64];
	time_t TIME(time(0));
	strftime(curTime, 64, "./dump/%Y-%m-%d %H%M%S.dmp", localtime(&TIME));
	HANDLE hFile = ::CreateFileA(curTime, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, NULL);
	if(INVALID_HANDLE_VALUE != hFile)
	{
		MINIDUMP_EXCEPTION_INFORMATION einfo = {::GetCurrentThreadId(), excp, FALSE};
		::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), 
			hFile, MiniDumpWithFullMemory, &einfo, NULL, NULL);
		::CloseHandle(hFile);
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ��������
const CString items[COLUMNS] = { 
	_T("���"), 
	_T("�˿�"), 
	_T("IP"), 
	_T("����"), 
	_T("CPU"), 
	_T("�ڴ�"), 
	_T("�߳�"),
	_T("���"),
	_T("����ʱ��"), 
	_T("Ƶ��"), 
	_T("��������"),
	_T("�޸�����"),
	_T("��С"),
	_T("�汾"), 
	_T("Keeper"), 
	_T("����λ��")
};

// ÿ�п��
const int g_Width[COLUMNS] = {
	55, // ���
	70, // �˿�
	125, // IP
	125, // ����
	80, // CPU
	100, // �ڴ�
	75, // �߳�
	75, // ���
	100, // ����ʱ��
	55, // Ƶ��
	155, // ��������
	155, // �޸�����
	80, // �ļ���С
	80, // �汾
	80, // �ػ�����汾
	475,// λ��
};

// Socket�����
CSocketServer *g_pSocket = NULL;

CRemoteControllerDlg *g_MainDlg = NULL;

const char* GetLocalHost()
{
	static char localhost[128] = { "127.0.0.1" };
	char hostname[128] = { 0 };
	if (0 == gethostname(hostname, 128))
	{
		hostent *host = gethostbyname(hostname);
		// ��ipת��Ϊ�ַ���
		char *hostip = inet_ntoa(*(struct in_addr*)host->h_addr_list[0]);
		memcpy(localhost, hostip, strlen(hostip));
	}
	return localhost;
}

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CRemoteControllerDlg �Ի���


CRemoteControllerDlg::CRemoteControllerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRemoteControllerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	strcpy_s(m_strIp, "127.0.0.1");
	m_nPort = 9999;

	m_pServer = NULL;
	m_bAdvanced = false;
	m_bDetectTime = true;
	InitializeCriticalSection(&m_cs);
	g_MainDlg = this;
	memset(m_strUp, 0, sizeof(m_strUp));
}


CRemoteControllerDlg::~CRemoteControllerDlg()
{
	WSACleanup();
	DeleteCriticalSection(&m_cs);
}


void CRemoteControllerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_APPLICATION_LIST, m_ListApps);
}


BEGIN_MESSAGE_MAP(CRemoteControllerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(ID_QUIT_APP, &CRemoteControllerDlg::OnQuitApp)
	ON_COMMAND(ID_REFRESH_ALL, &CRemoteControllerDlg::OnRefreshAll)
	ON_COMMAND(ID_POWEROFF_ALL, &CRemoteControllerDlg::OnPoweroffAll)
	ON_COMMAND(ID_IPCONFIG, &CRemoteControllerDlg::OnIpconfig)
	ON_COMMAND(ID_APP_ABOUT, &CRemoteControllerDlg::OnAppAbout)
	ON_WM_INITMENUPOPUP()
	ON_UPDATE_COMMAND_UI(ID_POWEROFF_ALL, &CRemoteControllerDlg::OnUpdatePoweroffAll)
	ON_COMMAND(ID_REBOOT_SYSTEM, &CRemoteControllerDlg::OnRebootSystem)
	ON_UPDATE_COMMAND_UI(ID_REBOOT_SYSTEM, &CRemoteControllerDlg::OnUpdateRebootSystem)
	ON_UPDATE_COMMAND_UI(ID_REFRESH_ALL, &CRemoteControllerDlg::OnUpdateRefreshAll)
	ON_COMMAND(ID_UPDATE, &CRemoteControllerDlg::OnUpdate)
	ON_COMMAND(ID_SETTIME, &CRemoteControllerDlg::OnSettime)
	ON_COMMAND(ID_STOPALL, &CRemoteControllerDlg::OnStopall)
	ON_UPDATE_COMMAND_UI(ID_STOPALL, &CRemoteControllerDlg::OnUpdateStopall)
	ON_COMMAND(ID_STARTALL, &CRemoteControllerDlg::OnStartall)
	ON_UPDATE_COMMAND_UI(ID_STARTALL, &CRemoteControllerDlg::OnUpdateStartall)
	ON_COMMAND(ID_UPDATE_SINGLE, &CRemoteControllerDlg::OnUpdateSingle)
	ON_UPDATE_COMMAND_UI(ID_SETTIME, &CRemoteControllerDlg::OnUpdateSettime)
	ON_UPDATE_COMMAND_UI(ID_UPDATE, &CRemoteControllerDlg::OnUpdateUpdate)
	ON_UPDATE_COMMAND_UI(ID_UPDATE_SINGLE, &CRemoteControllerDlg::OnUpdateUpdateSingle)
	ON_COMMAND(ID_SET_ALIVETIME, &CRemoteControllerDlg::OnSetAlivetime)
	ON_COMMAND(ID_SCREENSHOT, &CRemoteControllerDlg::Screenshot)
	ON_UPDATE_COMMAND_UI(ID_SCREENSHOT, &CRemoteControllerDlg::OnUpdateScreenshot)
	ON_UPDATE_COMMAND_UI(ID_SET_ALIVETIME, &CRemoteControllerDlg::OnUpdateSetAlivetime)
	ON_COMMAND(ID_SELECT_SETTIME, &CRemoteControllerDlg::OnSelectSettime)
	ON_UPDATE_COMMAND_UI(ID_SELECT_SETTIME, &CRemoteControllerDlg::OnUpdateSelectSettime)
	ON_COMMAND(ID_SET_UPSERVER, &CRemoteControllerDlg::OnSetUpserver)
	ON_COMMAND(ID_DETECT_TIME_ERROR, &CRemoteControllerDlg::OnDetectTimeError)
	ON_UPDATE_COMMAND_UI(ID_DETECT_TIME_ERROR, &CRemoteControllerDlg::OnUpdateDetectTimeError)
END_MESSAGE_MAP()


// CRemoteControllerDlg ��Ϣ�������

BOOL CRemoteControllerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	ShowWindow(SW_SHOWMAXIMIZED);

	// Ϊ�б���ͼ�ؼ����ȫ��ѡ�к�դ����
	m_ListApps.SetExtendedStyle(m_ListApps.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// Ϊ�б���ͼ�ؼ������
	m_ListApps.AddColumns(items, COLUMNS);
	// ˫���������˸
	m_ListApps.SetExtendedStyle(LVS_EX_DOUBLEBUFFER | m_ListApps.GetExtendedStyle());
	m_ListApps.GetWindowRect(&m_rect);

	// socket
	WSADATA wsaData; // Socket
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	m_pServer = new CSocketServer;
	// settings.ini
	GetModuleFileNameA(NULL, m_strConf, _MAX_PATH);
	char *p = m_strConf;
	while('\0' != *p) ++p;
	while('\\' != *p) --p;
	strcpy(p, "\\ScreenShot\\");
	m_sPicPath = CString(m_strConf);
	CreateDirectory(m_sPicPath, NULL);
	strcpy(p, "\\settings.ini");
	GetPrivateProfileStringA("settings", "localIp", "", m_strIp, 64, m_strConf);
	if (m_strIp[0] == '\0')
		strcpy_s(m_strIp, GetLocalHost());
	GetPrivateProfileStringA("settings", "upServer", "", m_strUp, sizeof(m_strUp), m_strConf);
	if (0 == strcmp(m_strIp, m_strUp))
		memset(m_strUp, 0, sizeof(m_strUp));

	m_nPort = GetPrivateProfileIntA("settings", "port", 9999, m_strConf);
	TRACE("======> Start socket: Ip = %s, Port = %d\n", m_strIp, m_nPort);
	m_pServer->init(m_strIp, m_nPort, SocketType_Server);
	g_pSocket = m_pServer;

	m_bAdvanced = GetPrivateProfileIntA("settings", "advanced", 0, m_strConf);
	SetUnhandledExceptionFilter(&whenbuged);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}


void CRemoteControllerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CRemoteControllerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CRemoteControllerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CRemoteControllerDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	OutputDebugStringA("======> CRemoteControllerDlg begin OnDestroy()\n");
	if (NULL != m_pServer)
	{
		m_pServer->unInit();
		delete m_pServer;
		m_pServer = NULL;
	}
	OutputDebugStringA("======> CRemoteControllerDlg end OnDestroy()\n");
}


void CRemoteControllerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (m_ListApps.GetSafeHwnd())
	{
		CRect rt;
		GetClientRect(&rt);
		const int EDGE = 16;
		m_ListApps.MoveWindow(CRect(rt.left + EDGE, rt.top + EDGE, 
			rt.right - EDGE, rt.bottom - EDGE), TRUE);
		m_ListApps.AvgColumnWidth(COLUMNS);
	}
}


void CRemoteControllerDlg::OnQuitApp()
{
	SendMessage(WM_CLOSE, 0, 0);
}


BOOL CRemoteControllerDlg::PreTranslateMessage(MSG* pMsg)
{
	// ���� ESC/Enter �رմ���
	if( pMsg->message == WM_KEYDOWN && 
		(pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN) )
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CRemoteControllerDlg::OnRefreshAll()
{
	g_pSocket->SendCommand(REFRESH);
}


void CRemoteControllerDlg::OnPoweroffAll()
{
	int i = 0, nRet = 0;
	do 
	{
		nRet = MessageBox(_T("���\"��\"����ͼ�ر��������ӵ������������豸!")\
			_T("\r\n�벻Ҫ���׵��\"��\"��ť, ����˼������!"), 
			_T("����"), MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2);
	} while (IDYES == nRet && ++i < 3);
	if (3 == i)
	{
		BeginWaitCursor();
		g_pSocket->ControlDevice(SHUTDOWN);
		EndWaitCursor();
	}
}


void CRemoteControllerDlg::OnIpconfig()
{
	CIPConfigDlg dlg;
	dlg.m_strIpAddr = m_strIp;
	dlg.m_nPort = m_nPort;
	if (IDOK == dlg.DoModal())
	{
		if (m_nPort != dlg.m_nPort || CString(m_strIp) != dlg.m_strIpAddr)
		{
			USES_CONVERSION;
			strcpy_s(m_strIp, W2A(dlg.m_strIpAddr));
			m_nPort = dlg.m_nPort;
			WritePrivateProfileStringA("settings", "localIp", m_strIp, m_strConf);
			char buf[64];
			sprintf_s(buf, "%d", m_nPort);
			WritePrivateProfileStringA("settings", "port", buf, m_strConf);
			m_pServer->unInit();
			m_pServer->init(m_strIp, m_nPort, SocketType_Server);
			m_ListApps.Clear();
		}
	}
}


void CRemoteControllerDlg::OnAppAbout()
{
	CAboutDlg dlg;
	dlg.DoModal();
}


void CRemoteControllerDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CDialogEx::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	ASSERT(pPopupMenu != NULL);
	// Check the enabled state of various menu items.

	CCmdUI state;
	state.m_pMenu = pPopupMenu;
	ASSERT(state.m_pOther == NULL);
	ASSERT(state.m_pParentMenu == NULL);

	// Determine if menu is popup in top-level menu and set m_pOther to
	// it if so (m_pParentMenu == NULL indicates that it is secondary popup).
	HMENU hParentMenu;
	if (AfxGetThreadState()->m_hTrackingMenu == pPopupMenu->m_hMenu)
		state.m_pParentMenu = pPopupMenu;    // Parent == child for tracking popup.
	else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
	{
		CWnd* pParent = this;
		// Child windows don't have menus--need to go to the top!
		if (pParent != NULL &&
			(hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
		{
			int nIndexMax = ::GetMenuItemCount(hParentMenu);
			for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
			{
				if (::GetSubMenu(hParentMenu, nIndex) == pPopupMenu->m_hMenu)
				{
					// When popup is found, m_pParentMenu is containing menu.
					state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
					break;
				}
			}
		}
	}

	state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
		state.m_nIndex++)
	{
		state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0)
			continue; // Menu separator or invalid cmd - ignore it.

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (state.m_nID == (UINT)-1)
		{
			// Possibly a popup menu, route to first item of that popup.
			state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
			if (state.m_pSubMenu == NULL ||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				state.m_nID == (UINT)-1)
			{
				continue;       // First item of popup can't be routed to.
			}
			state.DoUpdate(this, TRUE);   // Popups are never auto disabled.
		}
		else
		{
			// Normal menu item.
			// Auto enable/disable if frame window has m_bAutoMenuEnable
			// set and command is _not_ a system command.
			state.m_pSubMenu = NULL;
			state.DoUpdate(this, FALSE);
		}

		// Adjust for menu deletions and additions.
		UINT nCount = pPopupMenu->GetMenuItemCount();
		if (nCount < state.m_nIndexMax)
		{
			state.m_nIndex -= (state.m_nIndexMax - nCount);
			while (state.m_nIndex < nCount &&
				pPopupMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
			{
				state.m_nIndex++;
			}
		}
		state.m_nIndexMax = nCount;
	}
}


void CRemoteControllerDlg::OnUpdatePoweroffAll(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_bAdvanced && m_ListApps.GetItemCount());
}


void CRemoteControllerDlg::OnRebootSystem()
{
	int i = 0, nRet = 0;
	do 
	{
		nRet = MessageBox(_T("���\"��\"����ͼ�����������ӵ������������豸!")\
			_T("\r\n�벻Ҫ���׵��\"��\"��ť, ����˼������!"), 
			_T("����"), MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2);
	} while (IDYES == nRet && ++i < 3);
	if (3 == i)
	{
		BeginWaitCursor();
		g_pSocket->ControlDevice(REBOOT);
		EndWaitCursor();
	}
}


void CRemoteControllerDlg::OnUpdateRebootSystem(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_bAdvanced && m_ListApps.GetItemCount());
}


void CRemoteControllerDlg::OnUpdateRefreshAll(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_ListApps.GetItemCount());
}


void CRemoteControllerDlg::OnUpdate()
{
	if (IDYES == MessageBox(_T("ȷ������ȫ�����ػ�������?"), _T("����"), MB_ICONWARNING | MB_YESNO))
	{
		std::string cmd = MAKE_CMD(UPDATE, m_strUp);
		g_pSocket->SendCommand(cmd.c_str());
	}
}


void CRemoteControllerDlg::OnSettime()
{
	if (IDYES == MessageBox(_T("ȷ����ȫ���������뱾������ʱ��У׼��?"), _T("����"), MB_ICONWARNING | MB_YESNO))
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		char buf[64];
		sprintf_s(buf, "%s:%d,%d,%d,%d,%d,%d,%d,%d", SETTIME, st.wYear, st.wMonth, st.wDayOfWeek, 
			st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
		g_pSocket->ControlDevice(buf);
	}
}


void CRemoteControllerDlg::OnStopall()
{
	int i = 0, nRet = 0;
	do 
	{
		nRet = MessageBox(_T("���\"��\"��ֹͣ���г���! ����˼������!"), 
			_T("����"), MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2);
	} while (IDYES == nRet && ++i < 3);
	if (3 == i)
		g_pSocket->SendCommand(STOP);
}


void CRemoteControllerDlg::OnUpdateStopall(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_bAdvanced && m_ListApps.GetItemCount());
}


void CRemoteControllerDlg::OnStartall()
{
	if(IDYES == MessageBox(_T("���\"��\"���������г���! ��ȷ��������?"), 
			_T("��ʾ"), MB_ICONINFORMATION | MB_YESNO | MB_DEFBUTTON2))
		g_pSocket->SendCommand(START);
}


void CRemoteControllerDlg::OnUpdateStartall(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_bAdvanced && m_ListApps.GetItemCount());
}


void CRemoteControllerDlg::OnUpdateSingle()
{
	POSITION pos = m_ListApps.GetFirstSelectedItemPosition();
	if (pos && IDYES == MessageBox(_T("ȷ��������ѡ���ػ�������?"), _T("����"), MB_ICONWARNING | MB_YESNO))
	{
		while (pos)
		{
			int nRow = m_ListApps.GetNextSelectedItem(pos);
			CString no = m_ListApps.GetItemText(nRow, _no);
			USES_CONVERSION;
			std::string cmd = MAKE_CMD(UPDATE, m_strUp);
			g_pSocket->SendCommand(cmd.c_str(), W2A(no));
		}
	}
}


void CRemoteControllerDlg::OnUpdateSettime(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_ListApps.GetItemCount());
}


void CRemoteControllerDlg::OnUpdateUpdate(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_ListApps.GetItemCount());
}


void CRemoteControllerDlg::OnUpdateUpdateSingle(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_ListApps.GetItemCount());
}


void CRemoteControllerDlg::OnSetAlivetime()
{
	CAliveTimeDlg dlg;
	dlg.m_nAliveTime = 0;
	if (IDOK == dlg.DoModal() && dlg.m_nAliveTime)
	{
		std::string cur = m_ListApps.getCurSelNo();
		g_pSocket->SetAliveTime(dlg.m_nAliveTime, cur.length() ? cur.c_str() : NULL);
	}
}


void CRemoteControllerDlg::Screenshot()
{
	CWindowDC winDC(this);

	CImage image;
	int nBPP = winDC.GetDeviceCaps(BITSPIXEL) * winDC.GetDeviceCaps(PLANES);
	if(image.Create(m_rect.Width(), m_rect.Height(), max(nBPP, 24), 0))
	{
		CImageDC imageDC(image);
		::BitBlt(imageDC, 0, 0, m_rect.Width(), m_rect.Height(), winDC, m_rect.left, m_rect.top, SRCCOPY);
		CTime t = CTime::GetCurrentTime();
		CString tt = t.Format("%Y-%m-%d_%H-%M-%S.png");
		CString strFull = m_sPicPath + tt;
		HRESULT hr = image.Save(strFull);
		MessageBox(CString("�������\"") + tt + (S_OK == hr ? CString("\"�ɹ�!") : CString("\"ʧ��!")));
	}
}


void CRemoteControllerDlg::OnUpdateScreenshot(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_ListApps.GetItemCount());
}


void CRemoteControllerDlg::OnUpdateSetAlivetime(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_ListApps.GetItemCount());
}


void CRemoteControllerDlg::OnSelectSettime()
{
	POSITION pos = m_ListApps.GetFirstSelectedItemPosition();
	if (pos && IDYES == MessageBox(_T("ȷ������ѡ�������뱾������ʱ��У׼��?"), _T("����"), MB_ICONWARNING | MB_YESNO))
	{
		int nRow = m_ListApps.GetNextSelectedItem(pos);
		CString no = m_ListApps.GetItemText(nRow, _no);
		SYSTEMTIME st;
		GetLocalTime(&st);
		char buf[64];
		sprintf_s(buf, "%s:%d,%d,%d,%d,%d,%d,%d,%d", SETTIME, st.wYear, st.wMonth, st.wDayOfWeek, 
			st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
		USES_CONVERSION;
		g_pSocket->SendCommand(buf, W2A(no));
	}
}


void CRemoteControllerDlg::OnUpdateSelectSettime(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_ListApps.GetItemCount());
}


void CRemoteControllerDlg::OnSetUpserver()
{
	CUpdateServerDlg dlg;
	CString old = CString(m_strUp[0] ? m_strUp : m_strIp);
	dlg.m_strUpServer = old;
	if (IDOK == dlg.DoModal() && old != dlg.m_strUpServer)
	{
		USES_CONVERSION;
		strcpy_s(m_strUp, W2A(dlg.m_strUpServer));
		if (0 == strcmp(m_strIp, m_strUp))
			memset(m_strUp, 0, sizeof(m_strUp));
		WritePrivateProfileStringA("settings", "upServer", m_strUp, m_strConf);
	}
}


void CRemoteControllerDlg::OnDetectTimeError()
{
	m_bDetectTime = !m_bDetectTime;
}


void CRemoteControllerDlg::OnUpdateDetectTimeError(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bDetectTime);
}
