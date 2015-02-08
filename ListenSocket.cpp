// ListenSocket.cpp : implementation file
//

#include "stdafx.h"
#include "FTPserver.h"
#include "FTPserverDlg.h"
#include "ListenSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListenSocket

CListenSocket::CListenSocket()
:m_pWndServer(NULL)
{
}

CListenSocket::~CListenSocket()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CListenSocket, CAsyncSocket)
	//{{AFX_MSG_MAP(CListenSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CListenSocket member functions

/********************************************************************/
/*																	*/
/* 函数名称 : CListenSocket::OnAccept								*/
/* 作用     : 接收到新连接时触发									*/
/*																	*/
/********************************************************************/
void CListenSocket::OnAccept(int nErrorCode) 
{
	// 创建临时的SOCKET
	CSocket tmpSocket;

	// 接受连接
	Accept(tmpSocket);

	// 使用SUSPENDED模式创建新的线程，这样可以在执行线程前设置线程信息
	CConnectThread* pThread = (CConnectThread*)AfxBeginThread(RUNTIME_CLASS(CConnectThread), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	if (!pThread)
	{
		tmpSocket.Close();
		TRACE("Could not create thread\n");
		return;
	}
	
	// 添加线程指针到线程链表
	CFTPserverDlg *pWnd = (CFTPserverDlg *)m_pWndServer;
	pWnd->m_CriticalSection.Lock();
    pWnd->m_ThreadList.AddTail(pThread);

	pWnd->m_CriticalSection.Unlock();

	// 传递窗口指针
	pThread->m_pWndServer = m_pWndServer;
	
	// 传递SOCKET
	pThread->m_hSocket = tmpSocket.Detach();
	
	// 重新启动SUSPENDED模式下的线程
	pThread->ResumeThread();

	CAsyncSocket::OnAccept(nErrorCode);
}
