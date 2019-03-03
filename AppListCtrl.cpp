// AppListCtrl.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RemoteController.h"
#include "AppListCtrl.h"
#include "socket\SocketServer.h"
#include "RemoteControllerDlg.h"
#include <io.h>
#include "DlgInfomation.h"

extern CSocketServer *g_pSocket;

extern const int g_Width[COLUMNS];

// ��һ���������
int last_col = -1;

// �����������
int sort_col = 0;

// ����ʽ
bool method = true;

String W_2_A(const CString &wStr)
{
	int len = WideCharToMultiByte(CP_ACP, 0, wStr, wStr.GetLength(), NULL, 0, NULL, NULL);
	String str(len);
	WideCharToMultiByte(CP_ACP, 0, wStr, wStr.GetLength(), str, len, NULL, NULL);
	str[len] = '\0';
	return str;
}

#ifdef W2A
#undef W2A
#endif

#ifdef USES_CONVERSION
#undef USES_CONVERSION
#endif

#define W2A W_2_A
#define USES_CONVERSION

// ffplay������ʱʱ��
#define FFPLAY_TIMEOUT 10

// CAppListCtrl

IMPLEMENT_DYNAMIC(CAppListCtrl, CListCtrl)

// App List Control
CAppListCtrl *g_pList = NULL;

CAppListCtrl::CAppListCtrl()
{
	m_nIndex = -1;
	g_pList = this;
	m_MapStat.insert(std::make_pair(ID_OP_QUERY, TRUE));
	m_MapStat.insert(std::make_pair(ID_OP_QUERY, TRUE));
	m_MapStat.insert(std::make_pair(ID_OP_RESTART, TRUE));
	m_MapStat.insert(std::make_pair(ID_OP_STOP, TRUE));
	m_MapStat.insert(std::make_pair(ID_OP_START, TRUE));
	m_MapStat.insert(std::make_pair(ID_OP_REMOTE, TRUE));
	m_MapStat.insert(std::make_pair(ID_OP_UPDATE, TRUE));
	m_MapStat.insert(std::make_pair(ID_OP_SPY, TRUE));
	InitializeCriticalSection(&m_cs);
}


CAppListCtrl::~CAppListCtrl()
{
	DeleteCriticalSection(&m_cs);
}


void CAppListCtrl::AddColumns(const CString its[], int cols)
{
	for (int i = 0; i < cols; ++i)
	{
		InsertColumn(i, its[i], LVCFMT_LEFT, g_Width[i]);
	}
}


BEGIN_MESSAGE_MAP(CAppListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(NM_RCLICK, &CAppListCtrl::OnNMRClick)
	ON_COMMAND(ID_OP_RESTART, &CAppListCtrl::RestartApp)
	ON_COMMAND(ID_OP_QUERY, &CAppListCtrl::QueryAppInfo)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, &CAppListCtrl::OnLvnColumnclick)
	ON_COMMAND(ID_OP_STOP, &CAppListCtrl::StopApp)
	ON_COMMAND(ID_OP_START, &CAppListCtrl::StartApp)
	ON_COMMAND(ID_OP_UPDATE, &CAppListCtrl::UpdateApp)
	ON_MESSAGE(MSG_InsertApp, &CAppListCtrl::MessageInsertApp)
	ON_MESSAGE(MSG_UpdateApp, &CAppListCtrl::MessageUpdateApp)
	ON_MESSAGE(MSG_DeleteApp, &CAppListCtrl::MessageDeleteApp)
	ON_MESSAGE(MSG_ChangeColor, &CAppListCtrl::MessageChangeColor)
	ON_MESSAGE(MSG_Infomation, &CAppListCtrl::MessageInfomation)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CAppListCtrl::OnNMCustomdraw)
	ON_COMMAND(ID_OP_REMOTE, &CAppListCtrl::OnOpRemote)
	ON_UPDATE_COMMAND_UI(ID_OP_REMOTE, &CAppListCtrl::OnUpdateOpRemote)
	ON_COMMAND(ID_OP_SPY, &CAppListCtrl::OnOpSpy)
	ON_UPDATE_COMMAND_UI(ID_OP_SPY, &CAppListCtrl::OnUpdateOpSpy)
	ON_COMMAND(ID_RECOVERY, &CAppListCtrl::RecoveryApp)
END_MESSAGE_MAP()


// CAppListCtrl ��Ϣ�������


std::string CAppListCtrl::getCurSelNo()
{
	std::string n;
	Lock();
	POSITION pos = GetFirstSelectedItemPosition();
	if (pos)
	{
		USES_CONVERSION;
		int row = GetNextSelectedItem(pos);
		CString no = GetItemText(row, _no);
		n = W2A(no);
	}
	Unlock();
	return n;
}


void CAppListCtrl::OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem != -1)
	{
		DWORD dwPos = GetMessagePos();
		CPoint point(LOWORD(dwPos), HIWORD(dwPos));
		CMenu menu;
		// ����̲߳���
		VERIFY(menu.LoadMenu(IDR_LIST_MENU));
		CMenu* popup = menu.GetSubMenu(0);
		ASSERT(popup != NULL);
		popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
		Lock();
		// ��������д�����Ҫ��Ϊ�˺���Ĳ���Ϊ׼����
		// ��ȡ�б���ͼ�ؼ��е�һ����ѡ�����λ��
		POSITION m_pstion = GetFirstSelectedItemPosition();
		//�ú�����ȡ��posָ�����б����������Ȼ��pos����Ϊ��һ��λ�õ�POSITIONֵ
		m_nIndex = GetNextSelectedItem(m_pstion);
		Unlock();
	}
}


void CAppListCtrl::UpdateAppItem(const char* port, const AppInfo &it)
{
	USES_CONVERSION;
	Lock();
	int n = GetItemCount();
	for(int row = 0; row < n; ++row)
	{
		CString no = GetItemText(row, _no);
		if (0 == strcmp(port, W2A(no)))
		{
			SetItemText(row, _ip, CString(it.ip));
			SetItemText(row, _name, CString(it.name));
			SetItemText(row, _cpu, CString(it.cpu));
			SetItemText(row, _mem, CString(it.mem));
			SetItemText(row, _threads, CString(it.threads));
			SetItemText(row, _handles, CString(it.handles));
			SetItemText(row, _runlog, CString(it.run_log));
			SetItemText(row, _runtime, CString(it.run_times));
			SetItemText(row, _create_time, CString(it.create_time));
			SetItemText(row, _mod_time, CString(it.mod_time));
			SetItemText(row, _file_size, CString(it.file_size));
			SetItemText(row, _version, CString(it.version));
			SetItemText(row, _keep_ver, CString(it.keep_ver));
			SetItemText(row, _cmd_line, CString(it.cmd_line));
			SetItemText(row, _b32_64bits, CString(it.bits));
			SetItemText(row, _disk_info, CString(it.disk_info));
			break;
		}
	}
	Unlock();
}


void CAppListCtrl::AvgColumnWidth(int cols)
{
	Lock();
	for (int i = 0; i < cols; ++i)
	{
		SetColumnWidth(i, g_Width[i]);
	}
	Unlock();
}


void CAppListCtrl::Clear()
{
	Lock();
	int n = 0;
	while (n = GetItemCount())
	{
		DeleteItem(n-1);
	}
	Unlock();
}


void CAppListCtrl::InsertAppItem(const char* port)
{
	Lock();
	int n = GetItemCount();
	CString s;
	s.Format(_T("%d"), n+1);
	InsertItem(n, s);
	SetItemText(n, _no, CString(port));
	Unlock();
}


void CAppListCtrl::DeleteAppItem(const char* port)
{
	USES_CONVERSION;
	Lock();
	int n = GetItemCount();
	for(int row = 0; row < n; ++row)
	{
		CString no = GetItemText(row, _no);
		if (0 == strcmp(port, W2A(no)))
		{
			CString s = GetItemText(row, _id);
			String str = W2A(s);
			int id = atoi(str);// ��ɾ������
			DeleteItem(row);
			// ���е�����Ҫ���±��
			--n;
			for(int i = 0; i < n; ++i)
			{
				s = GetItemText(i, _id);
				String str = W2A(s);
				int temp = atoi(str);
				if (temp > id)
				{
					s.Format(_T("%d"), temp - 1);
					SetItemText(i, _id, s);
				}
			}
			break;
		}
	}
	Unlock();
}


void CAppListCtrl::RestartApp()
{
	if (-1 != m_nIndex && IDYES == MessageBox(_T("ȷ��\"����\"�˳�����?"), _T("����"), MB_ICONWARNING | MB_YESNO))
	{
		TRACE("======> RestartApp index = %d\n", m_nIndex);
		USES_CONVERSION;
		Lock();
		CString no = GetItemText(m_nIndex, _no);
		Unlock();
		g_pSocket->SendCommand(RESTART, W2A(no));
	}
}


void CAppListCtrl::QueryAppInfo()
{
	if (-1 != m_nIndex)
	{
		TRACE("======> QueryAppInfo index = %d\n", m_nIndex);
		USES_CONVERSION;
		Lock();
		CString no = GetItemText(m_nIndex, _no);
		Unlock();
		g_pSocket->SendCommand(REFRESH, W2A(no));
	}
}


// ����ص�
int CALLBACK comp(LPARAM p1, LPARAM p2, LPARAM s)
{
	int row1 = (int) p1;
	int row2 = (int) p2;
	const CAppListCtrl *lc = (CAppListCtrl*)s;
	CString lps1 = lc->GetItemText(row1, sort_col);
	CString lps2 = lc->GetItemText(row2, sort_col);
	if (sort_col < _ip || (_name < sort_col && sort_col < _create_time) || sort_col == _file_size)
	{
		USES_CONVERSION;
		double d1 = atof(W2A(lps1)), d2 = atof(W2A(lps2));
		return method ? d1 - d2 > 0 : d2 - d1 > 0;
	}
	else
	{
		return method ? lps1.CompareNoCase(lps2) : lps2.CompareNoCase(lps1);
	}
}


void CAppListCtrl::OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	Lock();
	int n = GetItemCount();
	if (n)
	{
		sort_col = pNMLV->iSubItem;
		method = (last_col==sort_col ? !method : true);
		for (int i = 0; i < n; ++i)
		{
			DWORD_PTR cur = GetItemData(i);
			SetItemData(i, (COLOR_DEFAULT < cur) ? cur : i);
		}
		SortItems(&comp, (DWORD_PTR)this);
		last_col = sort_col;
	}
	Unlock();

	*pResult = 0;
}


void CAppListCtrl::StopApp()
{
	if (-1 != m_nIndex && IDYES == MessageBox(_T("ȷ��\"ֹͣ\"�˳�����?"), _T("����"), MB_ICONWARNING | MB_YESNO))
	{
		TRACE("======> StopApp index = %d\n", m_nIndex);
		USES_CONVERSION;
		Lock();
		CString no = GetItemText(m_nIndex, _no);
		Unlock();
		g_pSocket->SendCommand(STOP, W2A(no));
	}
}


void CAppListCtrl::StartApp()
{
	if (-1 != m_nIndex)
	{
		TRACE("======> StartApp index = %d\n", m_nIndex);
		USES_CONVERSION;
		Lock();
		CString no = GetItemText(m_nIndex, _no);
		Unlock();
		g_pSocket->SendCommand(START, W2A(no));
	}
}


void CAppListCtrl::UpdateApp()
{
	if (-1 != m_nIndex && IDYES == MessageBox(_T("ȷ��\"����\"�˳�����?"), _T("����"), MB_ICONWARNING | MB_YESNO))
	{
		UpdateSelected();
	}
}


void CAppListCtrl::UpdateSelected()
{
	if (-1 != m_nIndex)
	{
		TRACE("======> UpdateApp index = %d\n", m_nIndex);
		USES_CONVERSION;
		std::vector<CString> v_str;
		Lock();
		CString no = GetItemText(m_nIndex, _no);
		// ����Ƿ�Ӧ�ó��������˶��
		CString ip = GetItemText(m_nIndex, _ip), cmd_line = GetItemText(m_nIndex, _cmd_line);
		// ����ʱ���Ŀ�����汾�Ƿ���ڱ����ļ�
		CString name = GetItemText(m_nIndex, _name);
		std::string ver = g_pSocket->getVersion(std::string(W2A(name)));
		String src_ver = W2A(GetItemText(m_nIndex, _version));
		const char *pVer = strcmp(src_ver, "��") ? src_ver.c_str() : "";
		if (!g_MainDlg->m_bAllowDebug && strcmp(pVer, ver.c_str()) >= 0)
		{
			Unlock();
			if (!name.IsEmpty() && !g_MainDlg->m_bAllowDebug)
				MessageBox(_T("Ŀ������������£���������!\r\n���鱾�س���汾�Ƿ���¡�"), 
				_T("��ʾ"), MB_ICONINFORMATION | MB_OK);
			return;
		}
		for (int i = 0; i < GetItemCount(); ++i)
		{
			if(m_nIndex != i && ip == GetItemText(i, _ip) && cmd_line == GetItemText(i, _cmd_line))
			{
				v_str.push_back(GetItemText(i, _no));
			}
		}
		Unlock();
		// �Ȱ��������ֹͣ
		for (int i = 0; i < v_str.size(); ++i)
		{
			g_pSocket->SendCommand(PAUSE, W2A(v_str.at(i)));
			Sleep(50);
			g_pSocket->SendCommand(STOP, W2A(v_str.at(i)));
		}
		char arg[64];
		sprintf_s(arg, "a,%s", g_MainDlg->m_strUp);
		std::string cmd = MAKE_CMD(UPDATE, arg);
		String c_no = W2A(no);
		if(g_MainDlg->m_bAllowDebug)
		{
			g_pSocket->SendCommand(ALLOW_DEBUG, c_no);
			g_MainDlg->m_bAllowDebug = false;
			Sleep(50);
		}
		g_pSocket->SendCommand(cmd.c_str(), c_no);
	}
}


LRESULT CAppListCtrl::MessageInsertApp(WPARAM wParam, LPARAM lParam)
{
	TRACE("======> MessageInsertApp\n");
	char port[32];
	sprintf_s(port, "%d", wParam);
	InsertAppItem(port);

	return 0;
}


LRESULT CAppListCtrl::MessageUpdateApp(WPARAM wParam, LPARAM lParam)
{
	TRACE("======> MessageUpdateApp\n");
	char port[32];
	sprintf_s(port, "%d", wParam);
	const AppInfo* item = (AppInfo*)lParam;
	AppInfo temp(*item);
	UpdateAppItem(port, temp);

	return 0;
}


LRESULT CAppListCtrl::MessageDeleteApp(WPARAM wParam, LPARAM lParam)
{
	TRACE("======> MessageDeleteApp\n");
	char port[32];
	sprintf_s(port, "%d", wParam);
	DeleteAppItem(port);

	return 0;
}


LRESULT CAppListCtrl::MessageChangeColor(WPARAM wParam, LPARAM lParam)
{
	TRACE("======> MessageChangeColor\n");
	char port[32];
	sprintf_s(port, "%d", wParam);

	USES_CONVERSION;
	Lock();
	int n = GetItemCount();
	for(int row = 0; row < n; ++row)
	{
		CString no = GetItemText(row, _no);
		if (0 == strcmp(port, W2A(no)))
		{
			SetItemData(row, lParam);
			break;
		}
	}
	Unlock();

	return 0;
}

// NoticeThread�̵߳Ĳ���
class NoticeParam
{
private:
	int t;
	const char *p1, *p2;
	~NoticeParam() { }
public:
	NoticeParam(const char *x1, const char *x2, int tm) : p1(x1), p2(x2), t(tm) { }
	const char *title() const { return p1; }
	const char *details() const { return p2; }
	int time() const { return t; }
	void destroy() { delete this; }
};

// �յ��ͻ��˵�"infomation"�������߳̽�����ʾ
void NoticeThread(void *param)
{
	OutputDebugStringA("======> BEGIN NoticeThread\n");

	NoticeParam *Para = (NoticeParam*)param;
	CDlgInfomation dlg(Para->title(), Para->details(), Para->time());
	Para->destroy();
	dlg.DoModal();

	OutputDebugStringA("======> END NoticeThread\n");
}

LRESULT CAppListCtrl::MessageInfomation(WPARAM wParam, LPARAM lParam)
{
	TRACE("======> MessageChangeColor\n");
	const char *info = (char *)wParam;
	const char *details = (char *)lParam;

	if (0 == strcmp(info, "ffmpeg"))
	{
		Uninit_ffplay(atoi(details+1));
	}

	_beginthread(&NoticeThread, 0, new NoticeParam(info, details, 5000));

	return 0;
}


void CAppListCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW *pNMCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);

	if (CDDS_ITEMPREPAINT == pNMCD->nmcd.dwDrawStage)
	{
		switch (pNMCD->nmcd.lItemlParam)
		{
		case COLOR_RED:// ��ɫ�����ػ�������ܳ��ּ���
			pNMCD->clrText = RGB(200, 0, 0);
			break;
		case COLOR_YELLOW:// ��ɫ��δ�ҵ����ػ�������
			pNMCD->clrText = RGB(255, 200, 0);
			break;
		case COLOR_BLUE1:// ��ɫ��ϵͳʱ��δУ׼��������ʱ��
			pNMCD->clrText = RGB(0, 200, 255);
			break;
		case COLOR_BLUE2:// ����
			pNMCD->clrText = RGB(0, 0, 255);
			break;
		case COLOR_DEFAULT:
			pNMCD->clrText = RGB(0, 0, 0);
			break;
		}
	}
	*pResult = 0;
	*pResult |= CDRF_NOTIFYPOSTPAINT;
	*pResult |= CDRF_NOTIFYITEMDRAW;
}


void CAppListCtrl::OnOpRemote()
{
	if (-1 != m_nIndex)
	{
		TRACE("======> Remote index = %d\n", m_nIndex);
		Lock();
		String ip = W2A(GetItemText(m_nIndex, _ip));
		Unlock();
		if (m_MapStat[ID_OP_REMOTE] == TRUE)
		{
			USES_CONVERSION;
			char mstsc[64];
			sprintf_s(mstsc, "mstsc /v %s", ip.c_str());
			system(mstsc);
		}else
		{
			MessageBox(CString(ip.c_str())+_T(" Զ�������Ѵ򿪡�"), _T("��ʾ"), MB_ICONINFORMATION | MB_OK);
		}
	}
}


void CAppListCtrl::OnUpdateOpRemote(CCmdUI *pCmdUI)
{
	if (-1 != m_nIndex)
	{
		Lock();
		CString wnd = GetItemText(m_nIndex, _ip);
		Unlock();
		wnd += _T(" - Զ����������");
		HWND hWnd = ::FindWindow(NULL, wnd);
		m_MapStat[ID_OP_REMOTE] = hWnd ? FALSE : TRUE;
	}
}

// �߳�ffplayThread�Ĳ���
struct ffplayThreadParam
{
	HANDLE handle;		// ffplay���̾��
	int Port;			// UDP�˿�
	std::string row;	// ��Ӧ������У�������س���
	ffplayThreadParam(HANDLE h, int port, const char *p) : handle(h), Port(port), row(p) { }
	void destroy() { delete this; }
};

void ffplayThread(void *param)
{
	OutputDebugStringA("======> BEGIN ffplayThread\n");
	
	ffplayThreadParam *p = (ffplayThreadParam *)param;
	std::string udp = g_MainDlg->udp(p->Port);
	HWND hWnd = NULL;
	for (int i = FFPLAY_TIMEOUT; !g_MainDlg->m_bExit && --i; ) // �ȴ�����
	{
		Sleep(1000);
		if(hWnd = ::FindWindowA(NULL, udp.c_str()))
		{
			::ShowWindow(hWnd, SW_SHOW);
			::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
			break;
		}
	}
	while(hWnd = ::FindWindowA(NULL, udp.c_str()))
	{
		Sleep(1000);
	}
	// ����ֹͣ��������Ϣ
	std::string cmd = WATCH; cmd += ":0";
	if(!p->row.empty())
		g_pSocket->SendCommand(cmd.c_str(), p->row.c_str());
	// 3s��û�������˳�����ǿ�ƽ���"ffplay"����
	if (WAIT_TIMEOUT == WaitForSingleObject(p->handle, 3000))
		TerminateProcess(p->handle, -1);
	CloseHandle(p->handle);
	p->destroy();

	OutputDebugStringA("======> END ffplayThread\n");
}

// ����"ffplay"������������ȡ���ػ���������н���ͼ
void CAppListCtrl::SpyOnSelected(const char *no, int nPort)
{
	char ffplay[_MAX_PATH], *p = ffplay;
	GetModuleFileNameA(NULL, ffplay, _MAX_PATH);
	while (*p) ++p;
	while ('\\' != *p) --p;
	strcpy(p+1, "ffplay.exe");
	if (-1 == _access(ffplay, 0)) // ������"ffplay"
	{
		MessageBox(_T("\"ffplay.exe\"������, �޷�������Ļ�ش�!"));
		return;
	}
	Lock();
	std::map<std::string, ffplayInfo>::const_iterator iter = m_ffplayMap.find(no);
	bool NotExist =  iter == m_ffplayMap.end();
	if(!NotExist && iter->second.timeout(FFPLAY_TIMEOUT+5) 
		&& !::FindWindowA(NULL, g_MainDlg->udp(iter->second).c_str()))
	{
		m_ffplayMap.erase(iter);
		NotExist = true;
	}
	Unlock();
	if (NotExist)
	{
		char param[100];
		sprintf_s(param, "-analyzeduration 200000 -autoexit -f h264 %s", g_MainDlg->udp(nPort).c_str());
		CString lpFile = CString(ffplay);
		CString lpParameters = CString(param);
		SHELLEXECUTEINFO ShExecInfo = { 0 };
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.hwnd = NULL;
		ShExecInfo.lpVerb = NULL;
		ShExecInfo.lpFile = lpFile;
		ShExecInfo.lpParameters = lpParameters;
		ShExecInfo.lpDirectory = NULL;
		ShExecInfo.nShow = SW_HIDE;
		ShExecInfo.hInstApp = NULL;
		if (FALSE == ShellExecuteEx(&ShExecInfo))
			OutputDebugStringA("===> [ERROR] ���� \"ffplay\" ʧ�ܡ�\n");
		else {
			// ��ȡ���ػ��������н���
			char port[8];
			_itoa(nPort, port, 10);
			std::string cmd = WATCH; cmd += ":";cmd += port;
			Lock();
			m_ffplayMap.insert(std::make_pair(no, nPort));
			Unlock();
			g_pSocket->SendCommand(cmd.c_str(), no);
			_beginthread(&ffplayThread, 0, new ffplayThreadParam(ShExecInfo.hProcess, nPort, no));
			OutputDebugStringA("===> [SUCCESS] ���� \"ffplay\" �ɹ���\n");
		}
	}else
	{
		MessageBox(_T("���Ӵ������ڴ򿪻��Ѵ򿪣����Ժ����ԡ�"), _T("��ʾ"), MB_ICONINFORMATION | MB_OK);
	}
}


/************************************************************************
* @brief �ر�ָ����ffplay
* @param[in] nPort ������ѡ���е�"�˿�"
* @note nPort��0���ر�ȫ��
************************************************************************/
void CAppListCtrl::Uninit_ffplay(int nPort)
{
	TRACE("======>[Uninit_ffplay] nPort= %d\n", nPort);
	Lock();
	if (nPort)
	{
		char port[8];
		_itoa(nPort, port, 10);
		std::map<std::string, ffplayInfo>::const_iterator iter = m_ffplayMap.find(port);
		if (iter != m_ffplayMap.end())
		{
			HWND hWnd = ::FindWindowA(NULL, g_MainDlg->udp(iter->second).c_str());
			if (hWnd)
			{
				::SendMessage(hWnd, WM_CLOSE, 0, 0);
				for (int k = 100; !iter->first.empty() && --k; Sleep(20));
			}
			// ����ֹͣ��������Ϣ
			std::string cmd = WATCH; cmd += ":0";
			g_pSocket->SendCommand(cmd.c_str(), iter->first.c_str());
		}
	}
	else
	for(std::map<std::string, ffplayInfo>::const_iterator iter = m_ffplayMap.begin(); 
		iter != m_ffplayMap.end(); ++iter)
	{
		HWND hWnd = ::FindWindowA(NULL, g_MainDlg->udp(iter->second).c_str());
		if (hWnd)
		{
			::SendMessage(hWnd, WM_CLOSE, 0, 0);
			for (int k = 100; !iter->first.empty() && --k; Sleep(20));
		}
		// ����ֹͣ��������Ϣ
		std::string cmd = WATCH; cmd += ":0";
		g_pSocket->SendCommand(cmd.c_str(), iter->first.c_str());
	}
	Unlock();
}

int CAppListCtrl::GetUdpPort(int base)
{
	static int n = base;
	n += 2;
	if (n > 60000)
	{
		n = base;
	}
	return n;
}

void CAppListCtrl::OnOpSpy()
{
	if (-1 != m_nIndex)
	{
		TRACE("======> SPY index = %d\n", m_nIndex);
		Lock();
		String no = W2A(GetItemText(m_nIndex, _no));
		Unlock();
		SpyOnSelected(no.c_str(), GetUdpPort());
	}
}


void CAppListCtrl::OnUpdateOpSpy(CCmdUI *pCmdUI)
{
	if (-1 != m_nIndex)
	{
		Lock();
		String no = W2A(GetItemText(m_nIndex, _no));
		std::map<std::string, ffplayInfo>::const_iterator iter = m_ffplayMap.find(no.c_str());
		bool NotExist = iter == m_ffplayMap.end();
		HWND hWnd = NotExist ? NULL : ::FindWindowA(NULL, g_MainDlg->udp(iter->second).c_str());
		if (hWnd)
		{
			::ShowWindow(hWnd, SW_SHOW);
			::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
		}else if (!NotExist && iter->second.timeout(FFPLAY_TIMEOUT + 5)){
			m_ffplayMap.erase(iter);
		}
		Unlock();
	}
}


void CAppListCtrl::RecoveryApp()
{
	if (-1 != m_nIndex && IDYES == MessageBox(_T("ȷ��\"��ԭ\"�˳�����? \r\nӦ��������������ʱ���д˲�����"), 
		_T("����"), MB_ICONWARNING | MB_YESNO))
	{
		TRACE("======> RecoveryApp index = %d\n", m_nIndex);
		USES_CONVERSION;
		Lock();
		String no = W2A(GetItemText(m_nIndex, _no));
		Unlock();
		std::string cmd = MAKE_CMD(UPDATE, "-");
		g_pSocket->SendCommand(cmd.c_str(), no.c_str());
	}
}
