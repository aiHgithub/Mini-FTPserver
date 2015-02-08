// FTPserver.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "FTPserver.h"
#include "FTPserverDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFTPserverApp

BEGIN_MESSAGE_MAP(CFTPserverApp, CWinApp)
	//{{AFX_MSG_MAP(CFTPserverApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
		ON_COMMAND(ID_APP_SHOW, OnAppShow)
		ON_COMMAND(ID_APP_EXIT, OnAppExit)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFTPserverApp construction

CFTPserverApp::CFTPserverApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CFTPserverApp object

CFTPserverApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CFTPserverApp initialization

BOOL CFTPserverApp::InitInstance()
{
	SetRegistryKey(_T("SlyarFTPserver"));

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// ´´½¨ÍÐÅÌ
	if (!m_TrayIcon.Create(WM_ICON_NOTIFY,_T("Slyar FTP Server"),LoadIcon(IDR_MAINFRAME),IDR_POPUP))
	{
		return -1;
	}

	CFTPserverDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

void CFTPserverApp::OnAppShow() 
{
	if (m_pMainWnd)
		m_pMainWnd->ShowWindow(SW_SHOW);	
}

void CFTPserverApp::OnAppExit() 
{
	m_pMainWnd->DestroyWindow();
}
