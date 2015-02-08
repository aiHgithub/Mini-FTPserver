// DlgQuit.cpp : implementation file
//

#include "stdafx.h"
#include "FTPserver.h"
#include "FTPserverDlg.h"
#include "DlgQuit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgQuit dialog


CDlgQuit::CDlgQuit(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgQuit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgQuit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgQuit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgQuit)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgQuit, CDialog)
	//{{AFX_MSG_MAP(CDlgQuit)
	ON_BN_CLICKED(IDC_BTN_CANCEL, OnBtnCancel)
	ON_BN_CLICKED(IDC_BTN_EXIT, OnBtnExit)
	ON_BN_CLICKED(IDC_BTN_MINSIZE, OnBtnMinsize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgQuit message handlers

/********************************************************************/
/*																	*/
/* 函数名称 : CDlgQuit::OnBtnMinsize								*/
/* 作用     : 最小化按钮响应										*/
/*																	*/
/********************************************************************/
void CDlgQuit::OnBtnMinsize() 
{
	((CFTPserverDlg*)m_pWndServer)->ShowWindow(SW_HIDE);
	EndDialog(-1);
}

/********************************************************************/
/*																	*/
/* 函数名称 : CDlgQuit::OnBtnExit									*/
/* 作用     : 退出按钮响应											*/
/*																	*/
/********************************************************************/
void CDlgQuit::OnBtnExit() 
{
	((CFTPserverDlg*)m_pWndServer)->DestroyWindow();
}

/********************************************************************/
/*																	*/
/* 函数名称 : CDlgQuit::OnBtnCancel									*/
/* 作用     : 取消按钮响应											*/
/*																	*/
/********************************************************************/
void CDlgQuit::OnBtnCancel() 
{
	EndDialog(-1);		
}