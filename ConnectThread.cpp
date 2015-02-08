// ConnectThread.cpp : implementation file
//

#include "stdafx.h"
#include "FTPserver.h"
#include "FTPserverDlg.h"
#include "ConnectThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConnectThread

IMPLEMENT_DYNCREATE(CConnectThread, CWinThread)

CConnectThread::CConnectThread()
{
	m_nTimerID = 0;
	m_LastDataTransferTime = CTime::GetCurrentTime();
}

CConnectThread::~CConnectThread()
{
}

/********************************************************************/
/*																	*/
/* 函数名称 : CConnectThread::InitInstance							*/
/* 作用     : 线程启动时执行的操作									*/
/*																	*/
/********************************************************************/
BOOL CConnectThread::InitInstance()
{
	try
	{
		// 关联SOCKET
		m_ControlSocket.Attach(m_hSocket);
		m_ControlSocket.m_pThread = this;
		
		m_ControlSocket.pThread = (CConnectThread*)this;
		m_ControlSocket.pWnd = (CFTPserverDlg *)(this->m_pWndServer);

		// 通知窗口启动了一个新线程
		m_pWndServer->SendMessage(WM_THREADSTART, (WPARAM)this, 0);
		
		// 检查在线人数
		if (((CFTPserverDlg *)m_pWndServer)->CheckMaxUsers())
		{
			m_ControlSocket.SendResponse("421 Too many users are connected, please try again later.");
			PostThreadMessage(WM_QUIT,0,0);
		}
		else
		{
			// 发送欢迎信息
			m_ControlSocket.SendResponse("220 %s", ((CFTPserverDlg *)m_pWndServer)->GetWelcomeMessage());
			m_nTimerID = ::SetTimer(NULL, 0, 1000, TimerProc);
		}
	}
	catch (CException* e)
	{
		e->Delete();
	}
	return TRUE;
}

/********************************************************************/
/*																	*/
/* 函数名称 : CConnectThread::ExitInstance							*/
/* 作用     : 线程退出时执行的操作									*/
/*																	*/
/********************************************************************/
int CConnectThread::ExitInstance()
{
	CFTPserverDlg *pWnd = (CFTPserverDlg *)m_pWndServer;
	
	try
	{
		pWnd->m_CriticalSection.Lock();
		
		// 从线程链表中删除线程
		POSITION pos = pWnd->m_ThreadList.Find(this);
		if(pos != NULL)
		{
			pWnd->m_ThreadList.RemoveAt(pos);
		}

		pWnd->m_CriticalSection.Unlock();

		// 关闭连接的套接字
		m_ControlSocket.Close();

		// 通知窗口关闭了一个线程
		pWnd->SendMessage(WM_THREADCLOSE, (WPARAM)this, 0);

	}
	catch(CException *e)
	{
		pWnd->m_CriticalSection.Unlock();
		e->Delete();
	}
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CConnectThread, CWinThread)
	//{{AFX_MSG_MAP(CConnectThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_THREAD_MESSAGE(WM_DESTROYDATACONNECTION, OnDestroyDataConnection)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConnectThread message handlers

/********************************************************************/
/*																	*/
/* 函数名称 : CConnectThread::OnDestroyDataConnection				*/
/* 作用     : 关闭数据连接消息响应									*/
/*																	*/
/********************************************************************/
void CConnectThread::OnDestroyDataConnection(WPARAM wParam, LPARAM lParam)
{
	// 关闭数据连接
	m_ControlSocket.DestroyDataConnection();
}

/********************************************************************/
/*																	*/
/* 函数名称 : TimerProc												*/	
/* 作用     : 定时器响应函数										*/
/*																	*/
/********************************************************************/
VOID CALLBACK CConnectThread::TimerProc(HWND hwnd, UINT uMsg, UINT uIDEvent, DWORD dwTime)
{
	CConnectThread *pThread = (CConnectThread *)AfxGetThread();
	
	if (uIDEvent == pThread->m_nTimerID)
	{
		int nConnectionTimeout = ((CFTPserverDlg *)pThread->m_pWndServer)->GetTimeout();
		// 检查超时
		CTime time = pThread->m_LastDataTransferTime;
		time += CTimeSpan(0, 0, nConnectionTimeout, 0);
		if (time < CTime::GetCurrentTime())
		{
			pThread->m_ControlSocket.SendResponse("426 Connection timed out, aborting transfer");
			pThread->PostThreadMessage(WM_QUIT,0,0);
		}
	}
} 

/********************************************************************/
/*																	*/
/* 函数名称 : UpdateTime											*/	
/* 作用     : 更新活动时间											*/
/*																	*/
/********************************************************************/
void CConnectThread::UpdateTime()
{
	m_LastDataTransferTime = CTime::GetCurrentTime();
}
