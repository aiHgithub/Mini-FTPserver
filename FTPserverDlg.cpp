// FTPserverDlg.cpp : implementation file
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
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFTPserverDlg dialog

/********************************************************************/
/*																	*/
/* 函数名称 : CFTPserverDlg::CFTPserverDlg							*/
/* 作用     : 构造函数初始化										*/
/*																	*/
/********************************************************************/
CFTPserverDlg::CFTPserverDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFTPserverDlg::IDD, pParent)
{
	// 获取本机IP
	GetLocalHostIP(m_strHostIP);

	CString strTempState;
	strTempState.Format("本机IP:%s 当前连接数:0",m_strHostIP);

	//{{AFX_DATA_INIT(CFTPserverDlg)
	m_bRunning = FALSE;
	m_strState = _T(strTempState);
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nConnectionCount = 0;
	m_nTimeout = 5;

	ReadRegistry();
}

void CFTPserverDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFTPserverDlg)
	DDX_Text(pDX, IDC_EDIT_MAXUSER, m_nMaxUsers);
	DDV_MinMaxUInt(pDX, m_nMaxUsers, 0, 500);
	DDX_Text(pDX, IDC_EDIT_PORT, m_nPort);
	DDV_MinMaxUInt(pDX, m_nPort, 0, 65535);
	DDX_Text(pDX, IDC_EDIT_USER, m_strName);
	DDV_MaxChars(pDX, m_strName, 20);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strPassword);
	DDV_MaxChars(pDX, m_strPassword, 20);
	DDX_Text(pDX, IDC_EDIT_WELCOME, m_strWelcomeMessage);
	DDX_Text(pDX, IDC_EDIT_QUIT, m_strGoodbyeMessage);
	DDX_Text(pDX, IDC_EDIT_FOLDER, m_strHomeDirectory);
	DDX_Check(pDX, IDC_CHECK_DOWNLOAD, m_bAllowDownload);
	DDX_Check(pDX, IDC_CHECK_UPLOAD, m_bAllowUpload);
	DDX_Check(pDX, IDC_CHECK_RENAME, m_bAllowRename);
	DDX_Check(pDX, IDC_CHECK_DELETE, m_bAllowDelete);
	DDX_Check(pDX, IDC_CHECK_CREATEDIR, m_bAllowCreateDirectory);
	DDX_Text(pDX, IDC_STATE, m_strState);
	DDV_MaxChars(pDX, m_strState, 50);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFTPserverDlg, CDialog)
	//{{AFX_MSG_MAP(CFTPserverDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_BROWSE, OnBtnBrowse)
	ON_BN_CLICKED(IDC_BTN_START, OnBtnStart)
	ON_BN_CLICKED(IDC_BTN_STOP, OnBtnStop)
	ON_BN_CLICKED(IDC_BTN_QUIT, OnBtnQuit)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_THREADSTART, OnThreadStart)
	ON_MESSAGE(WM_THREADCLOSE, OnThreadClose)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFTPserverDlg message handlers

BOOL CFTPserverDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(FALSE);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFTPserverDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if ((nID & 0xFFF0) == SC_MINIMIZE)
	{
		ShowWindow(SW_HIDE);
	}
	else if ((nID & 0xFFF0) == SC_CLOSE)
	{
		CDlgQuit dlg;
		dlg.m_pWndServer = this;
		dlg.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFTPserverDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFTPserverDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

/********************************************************************/
/*																	*/
/* 函数名称 : CFTPserverDlg::SetPort								*/
/* 作用     : 设置连接端口											*/
/*																	*/
/********************************************************************/
void CFTPserverDlg::SetPort(int nValue)
{
	m_nPort = nValue;
}

/********************************************************************/
/*																	*/
/* 函数名称 : CFTPserverDlg::SetMaxUsers							*/
/* 作用     : 设置最大连接数										*/
/*																	*/
/********************************************************************/
void CFTPserverDlg::SetMaxUsers(int nValue)
{
	m_nMaxUsers = nValue;
}

/********************************************************************/
/*																	*/
/* 函数名称 : CFTPserverDlg::SetWelcomeMessage						*/
/* 作用     : 设置欢迎信息											*/
/*																	*/
/********************************************************************/
void CFTPserverDlg::SetWelcomeMessage(LPCTSTR lpszText)
{
	m_strWelcomeMessage = lpszText;
}

/********************************************************************/
/*																	*/
/* 函数名称 : CFTPserverDlg::SetGoodbyeMessage						*/
/* 作用     : 设置退出信息											*/
/*																	*/
/********************************************************************/
void CFTPserverDlg::SetGoodbyeMessage(LPCTSTR lpszText)
{
	m_strGoodbyeMessage = lpszText;
}

/********************************************************************/
/*																	*/
/* 函数名称 : CFTPserverDlg::SetTimeout								*/
/* 作用     : 设置超时												*/
/*																	*/
/********************************************************************/
void CFTPserverDlg::SetTimeout(int nValue)
{
	m_nTimeout = nValue;
}

/********************************************************************/
/*																	*/
/* 函数名称 : CFTPserverDlg::IsActive								*/
/* 作用     : 服务器是否正在运行									*/
/*																	*/
/********************************************************************/
BOOL CFTPserverDlg::IsActive()
{
	return m_bRunning;
}

/********************************************************************/
/*																	*/
/* 函数名称 : CFTPserverDlg::Stop									*/
/* 作用     : 停止服务												*/
/*																	*/
/********************************************************************/
void CFTPserverDlg::Stop()
{
	if (!m_bRunning)
	{
		return;
	}
	
	m_bRunning = FALSE;	
	m_ListenSocket.Close();
	
	CConnectThread* pThread = NULL;
	
	// 关闭所有运行的线程
	do
	{
		m_CriticalSection.Lock();
		
		POSITION pos = m_ThreadList.GetHeadPosition();
		if (pos != NULL)
		{
			pThread = (CConnectThread *)m_ThreadList.GetAt(pos);
			
			m_CriticalSection.Unlock();
			
			// 保存线程句柄
			HANDLE hThread = pThread->m_hThread;
			
			// 通知线程退出
			pThread->SetThreadPriority(THREAD_PRIORITY_HIGHEST);
			pThread->PostThreadMessage(WM_QUIT,0,0);
			
			// 等待退出，最多5秒
			if (WaitWithMessageLoop(hThread, 5000) == FALSE)
			{
				// 强制退出
				m_CriticalSection.Lock();
				POSITION rmPos = m_ThreadList.Find(pThread);
				if (rmPos != NULL)
					m_ThreadList.RemoveAt(rmPos);
				m_CriticalSection.Unlock();
			}
		}
		else
		{
			m_CriticalSection.Unlock();	
			pThread = NULL;
		}
	}
	while (pThread != NULL);
}

/********************************************************************/
/*																	*/
/* 函数名称 : CFTPserverDlg::Start									*/
/* 作用     : 启动服务												*/
/*																	*/
/********************************************************************/
BOOL CFTPserverDlg::Start()
{
	if (m_bRunning)
	{
		return FALSE;
	}

	// 创建监听套接字
	if (m_ListenSocket.Create(m_nPort))
	{
		// 开始监听
		if (m_ListenSocket.Listen())
		{
			m_ListenSocket.m_pWndServer = this;
			m_bRunning = TRUE;
			return TRUE;
		}
	}
	return FALSE;
}

/********************************************************************/
/*																	*/
/* 函数名称 : CFTPserverDlg::OnBtnBrowse							*/
/* 作用     : 目录浏览按钮响应										*/
/*																	*/
/********************************************************************/
void CFTPserverDlg::OnBtnBrowse() 
{
	UpdateData(TRUE);
	CString strDir = BrowseForFolder(m_hWnd, "选择访问目录:", BIF_RETURNONLYFSDIRS);
	if (!strDir.IsEmpty())
	{
		m_strHomeDirectory = strDir;
		UpdateData(FALSE);
	}	
}

/********************************************************************/
/*																	*/
/* 函数名称 : CFTPserverDlg::CheckMaxUsers							*/
/* 作用     : 检查在线人数											*/
/*																	*/
/********************************************************************/
BOOL CFTPserverDlg::CheckMaxUsers()
{
	if (m_nConnectionCount > m_nMaxUsers)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/********************************************************************/
/*																	*/
/* 函数名称 : CFTPserverDlg::GetPort								*/
/* 作用     : 返回端口												*/
/*																	*/
/********************************************************************/
int CFTPserverDlg::GetPort()
{
	return m_nPort;
}

/********************************************************************/
/*																	*/
/* 函数名称 : CFTPserverDlg::GetMaxUsers							*/
/* 作用     : 返回最大访问数										*/
/*																	*/
/********************************************************************/
int CFTPserverDlg::GetMaxUsers()
{
	return m_nMaxUsers;
}

/********************************************************************/
/*																	*/
/* 函数名称 : CFTPserverDlg::GetConnectionCount						*/
/* 作用     : 返回当前连接数										*/
/*																	*/
/********************************************************************/
int CFTPserverDlg::GetConnectionCount()
{
	return m_nConnectionCount;
}

/********************************************************************/
/*																	*/
/* 函数名称 : CFTPserverDlg::GetWelcomeMessage						*/
/* 作用     : 返回欢迎信息											*/
/*																	*/
/********************************************************************/
CString CFTPserverDlg::GetWelcomeMessage()
{
	return m_strWelcomeMessage;
}

/********************************************************************/
/*																	*/
/* 函数名称 : CFTPserverDlg::GetGoodbyeMessage						*/
/* 作用     : 返回退出信息											*/
/*																	*/
/********************************************************************/
CString CFTPserverDlg::GetGoodbyeMessage()
{
	return m_strGoodbyeMessage;
}

// 返回超时
/********************************************************************/
/*																	*/
/* 函数名称 : CFTPserverDlg::CFTPserverDlg							*/
/* 作用     : 构造函数初始化										*/
/*																	*/
/********************************************************************/
int CFTPserverDlg::GetTimeout()
{
	return m_nTimeout;
}

/********************************************************************/
/*																	*/
/* 函数名称 : CFTPserverDlg::ItemSwitch								*/
/* 作用     : 控件开关												*/
/*																	*/
/********************************************************************/
void CFTPserverDlg::ItemSwitch(BOOL x)
{
	GetDlgItem(IDC_EDIT_PORT)->EnableWindow(x);
	GetDlgItem(IDC_EDIT_MAXUSER)->EnableWindow(x);
	GetDlgItem(IDC_EDIT_USER)->EnableWindow(x);
	GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(x);
	GetDlgItem(IDC_EDIT_WELCOME)->EnableWindow(x);
	GetDlgItem(IDC_EDIT_QUIT)->EnableWindow(x);
	GetDlgItem(IDC_EDIT_USER)->EnableWindow(x);
	GetDlgItem(IDC_BTN_BROWSE)->EnableWindow(x);
	GetDlgItem(IDC_EDIT_FOLDER)->EnableWindow(x);
	GetDlgItem(IDC_CHECK_DOWNLOAD)->EnableWindow(x);
	GetDlgItem(IDC_CHECK_UPLOAD)->EnableWindow(x);
	GetDlgItem(IDC_CHECK_RENAME)->EnableWindow(x);
	GetDlgItem(IDC_CHECK_DELETE)->EnableWindow(x);
	GetDlgItem(IDC_CHECK_CREATEDIR)->EnableWindow(x);

	GetDlgItem(IDC_BTN_START)->EnableWindow(x);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(!x);
}

/********************************************************************/
/*																	*/
/* 函数名称 : CFTPserverDlg::OnBtnStart								*/
/* 作用     : 启动按钮响应											*/
/*																	*/
/********************************************************************/
void CFTPserverDlg::OnBtnStart() 
{
	ItemSwitch(FALSE);
	UpdateData(TRUE);
	if (m_strName == "")
	{
		m_strName = "anonymous";
		UpdateData(FALSE);
	}
	WriteRegistry();
	Start();
}

/********************************************************************/
/*																	*/
/* 函数名称 : CFTPserverDlg::OnBtnStop								*/
/* 作用     : 停止按钮响应											*/
/*																	*/
/********************************************************************/
void CFTPserverDlg::OnBtnStop() 
{
	ItemSwitch(TRUE);
	Stop();
}

/********************************************************************/
/*																	*/
/* 函数名称 : CFTPserverDlg::OnBtnQuit								*/
/* 作用     : 退出按钮响应											*/
/*																	*/
/********************************************************************/
void CFTPserverDlg::OnBtnQuit() 
{
	if (IsWindow(m_hWnd))
	{
		DestroyWindow();
	}
	m_hWnd = NULL;
}

/********************************************************************/
/*																	*/
/* 函数名称 : CFTPserverDlg::OnThreadStart							*/
/* 作用     : 线程启动响应											*/
/*																	*/
/********************************************************************/
LRESULT CFTPserverDlg::OnThreadStart(WPARAM wParam, LPARAM)
{
	m_nConnectionCount++;
	m_strState.Format("本机IP:%s 当前连接数:%d",m_strHostIP,m_nConnectionCount);
	UpdateData(FALSE);
	return TRUE;
}

/********************************************************************/
/*																	*/
/* 函数名称 : CFTPserverDlg::OnThreadClose							*/
/* 作用     : 线程关闭响应											*/
/*																	*/
/********************************************************************/
LRESULT CFTPserverDlg::OnThreadClose(WPARAM wParam, LPARAM lParam)
{
	m_nConnectionCount--;
	m_strState.Format("本机IP:%s 当前连接数:%d",m_strHostIP,m_nConnectionCount);
	UpdateData(FALSE);
	return TRUE;
}


/********************************************************************/
/*																	*/
/* 函数名称 : CFTPserverDlg::WriteRegistry							*/
/* 作用     : 将配置信息写入注册表									*/
/*																	*/
/********************************************************************/
void CFTPserverDlg::WriteRegistry()
{
	AfxGetApp()->WriteProfileString("设置","账户",m_strName);
	AfxGetApp()->WriteProfileString("设置","密码",m_strPassword);
	AfxGetApp()->WriteProfileString("设置","欢迎信息",m_strWelcomeMessage);
	AfxGetApp()->WriteProfileString("设置","离开信息",m_strGoodbyeMessage);
	AfxGetApp()->WriteProfileString("设置","访问目录",m_strHomeDirectory);

	AfxGetApp()->WriteProfileInt("设置","端口",m_nPort);
	AfxGetApp()->WriteProfileInt("设置","最大线程",m_nMaxUsers);
	AfxGetApp()->WriteProfileInt("设置","下载文件",m_bAllowDownload);
	AfxGetApp()->WriteProfileInt("设置","上传文件",m_bAllowUpload);
	AfxGetApp()->WriteProfileInt("设置","删除文件",m_bAllowDelete);
	AfxGetApp()->WriteProfileInt("设置","文件重命名",m_bAllowRename);
	AfxGetApp()->WriteProfileInt("设置","创建目录",m_bAllowCreateDirectory);
}


/********************************************************************/
/*																	*/
/* 函数名称 : CFTPserverDlg::ReadRegistry							*/
/* 作用     : 从注册表中读取配置信息								*/
/*																	*/
/********************************************************************/
void CFTPserverDlg::ReadRegistry()
{
	m_strName = AfxGetApp()->GetProfileString("设置","账户",_T("anonymous"));
	m_strPassword = AfxGetApp()->GetProfileString("设置","密码",_T(""));
	m_strWelcomeMessage = AfxGetApp()->GetProfileString("设置","欢迎信息",_T("欢迎访问 Slyar FTPserver!"));
	m_strGoodbyeMessage = AfxGetApp()->GetProfileString("设置","离开信息",_T("再见!"));
	m_strHomeDirectory = AfxGetApp()->GetProfileString("设置","访问目录",_T("C:\\"));

	m_nPort = AfxGetApp()->GetProfileInt("设置","端口",21);
	m_nMaxUsers = AfxGetApp()->GetProfileInt("设置","最大线程",100);
	m_bAllowDownload = AfxGetApp()->GetProfileInt("设置","下载文件",1);
	m_bAllowUpload = AfxGetApp()->GetProfileInt("设置","上传文件",0);
	m_bAllowDelete = AfxGetApp()->GetProfileInt("设置","删除文件",0);
	m_bAllowRename = AfxGetApp()->GetProfileInt("设置","文件重命名",0);
	m_bAllowCreateDirectory = AfxGetApp()->GetProfileInt("设置","创建目录",0);
}
