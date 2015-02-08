// FTPserverDlg.h : header file
//

#if !defined(AFX_FTPSERVERDLG_H__AAFDE426_F2E2_4624_BEF3_7A38865C65E4__INCLUDED_)
#define AFX_FTPSERVERDLG_H__AAFDE426_F2E2_4624_BEF3_7A38865C65E4__INCLUDED_

#include "ListenSocket.h"	// Added by ClassView
#include "ConnectThread.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CFTPserverDlg dialog

class CFTPserverDlg : public CDialog
{
// Construction
public:
	// 保存线程指针
	CTypedPtrList<CObList, CConnectThread*> m_ThreadList;
	// 互斥对象
	CCriticalSection m_CriticalSection;
	// 在线人数
	UINT m_nConnectionCount;
	// 本机IP
	CString m_strHostIP;

private:
	// 超时
	int m_nTimeout;
	// 服务器是否正在运行
	BOOL m_bRunning;
	CListenSocket m_ListenSocket;
	void ReadRegistry();
	void WriteRegistry();

public:
	CFTPserverDlg(CWnd* pParent = NULL);
	void SetPort(int nValue);
	void SetMaxUsers(int nValue);
	void SetTimeout(int nValue);
	void SetGoodbyeMessage(LPCTSTR lpszText);
	void SetWelcomeMessage(LPCTSTR lpszText);
	int GetConnectionCount();
	int GetMaxUsers();
	int GetPort();
	int GetTimeout();
	CString GetGoodbyeMessage();
	CString GetWelcomeMessage();
	BOOL CheckMaxUsers();
	BOOL IsActive();
	BOOL Start();
	void Stop();
	void ItemSwitch(BOOL x);

// Dialog Data
	//{{AFX_DATA(CFTPserverDlg)
	enum { IDD = IDD_FTPSERVER_DIALOG };
	UINT	m_nMaxUsers;
	UINT	m_nPort;
	CString	m_strName;
	CString	m_strPassword;
	CString	m_strWelcomeMessage;
	CString	m_strGoodbyeMessage;
	CString	m_strHomeDirectory;
	BOOL	m_bAllowDownload;
	BOOL	m_bAllowUpload;
	BOOL	m_bAllowRename;
	BOOL	m_bAllowDelete;
	BOOL	m_bAllowCreateDirectory;
	CString	m_strState;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFTPserverDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CFTPserverDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnBrowse();
	afx_msg void OnBtnStart();
	afx_msg void OnBtnStop();
	afx_msg void OnBtnQuit();
	//}}AFX_MSG
	LRESULT OnThreadClose(WPARAM wParam, LPARAM lParam);
	LRESULT OnThreadStart(WPARAM wParam, LPARAM);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FTPSERVERDLG_H__AAFDE426_F2E2_4624_BEF3_7A38865C65E4__INCLUDED_)
