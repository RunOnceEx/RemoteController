
// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// RemoteController.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"
#include "RemoteControllerDlg.h"

extern CRemoteControllerDlg *g_MainDlg;

void LOCK()
{
	if (g_MainDlg)
		g_MainDlg->g_Lock();
}

void UNLOCK()
{
	if (g_MainDlg)
		g_MainDlg->g_Unlock();
}
