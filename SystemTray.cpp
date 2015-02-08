// SystemTray.cpp : implementation file
//

#include "stdafx.h"
#include "FTPserver.h"
#include "SystemTray.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CSystemTray, CDialog)

/////////////////////////////////////////////////////////////////////////////
// CSystemTray dialog

CSystemTray::CSystemTray()
{
    ZeroMemory(&m_NotifyIconData, sizeof(m_NotifyIconData));
}

CSystemTray::~CSystemTray()
{
	// 删除托盘
    m_NotifyIconData.uFlags = 0;
    Shell_NotifyIcon(NIM_DELETE, &m_NotifyIconData);
	
	// 关闭载体窗口
    DestroyWindow();
}


BEGIN_MESSAGE_MAP(CSystemTray, CDialog)
	//{{AFX_MSG_MAP(CSystemTray)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSystemTray message handlers

/********************************************************************/
/*																	*/
/* 函数名称 : CSystemTray::Create									*/
/* 作用     : 创建托盘												*/
/*																	*/
/********************************************************************/
BOOL CSystemTray::Create(UINT uCallbackMessage, LPCTSTR lpszToolTip, HICON hIcon, UINT uID)
{
	// 创建一个载体窗口
    if (!CWnd::CreateEx(0, AfxRegisterWndClass(0), "CSystemTray Dummy Window", WS_POPUP, 0,0,0,0, NULL, 0))
		return FALSE;
	
    // 设置托盘状态
    m_NotifyIconData.cbSize = sizeof(NOTIFYICONDATA);
    m_NotifyIconData.hWnd   = m_hWnd;
    m_NotifyIconData.uID    = uID;
    m_NotifyIconData.hIcon  = hIcon;
    m_NotifyIconData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    m_NotifyIconData.uCallbackMessage = uCallbackMessage;
    lstrcpy(m_NotifyIconData.szTip, lpszToolTip);
	
    // 添加托盘
    return Shell_NotifyIcon(NIM_ADD, &m_NotifyIconData);
}

/********************************************************************/
/*																	*/
/* 函数名称 : CSystemTray::OnTrayNotification						*/
/* 作用     : 托盘动作处理											*/
/*																	*/
/********************************************************************/
LRESULT CSystemTray::OnTrayNotification(UINT wParam, LONG lParam) 
{
    // 不是托盘消息直接返回
    if (wParam != m_NotifyIconData.uID)
        return 0L;
	
    CWnd *pMainWnd = AfxGetMainWnd();
	
    switch(LOWORD(lParam))
	{
		// 右键单击弹出托盘菜单
		case WM_RBUTTONUP:
			{    
				CMenu menu, *pSubMenu;
				if (!menu.LoadMenu(m_NotifyIconData.uID)) 
					return 0;
				
				pSubMenu = menu.GetSubMenu(0);
				
				if (pSubMenu == NULL)
					return 0;
				
				CPoint pos;
				GetCursorPos(&pos);
				
				// 显示弹出菜单
				pMainWnd->SetForegroundWindow();
				pSubMenu->TrackPopupMenu(0, pos.x, pos.y, pMainWnd, NULL);
				
				menu.DestroyMenu();
				break;
			} 
		// 左键双击显示主窗口
		case WM_LBUTTONDBLCLK:
			{
				pMainWnd->ShowWindow(SW_SHOW);
				pMainWnd->SetForegroundWindow();
				break;
			}
		default:
			break;
	}
    return 1;
}

/********************************************************************/
/*																	*/
/* 函数名称 : CSystemTray::WindowProc								*/
/* 作用     : 窗口消息响应											*/
/*																	*/
/********************************************************************/
LRESULT CSystemTray::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
    if (uMsg == m_NotifyIconData.uCallbackMessage)
	{
        return OnTrayNotification(wParam, lParam);
	}
	return CWnd::WindowProc(uMsg, wParam, lParam);
}
