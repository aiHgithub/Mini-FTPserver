#if !defined(AFX_CONTROLSOCKET_H__51DE5049_9A9F_41B0_A132_B2C70C6BEFAC__INCLUDED_)
#define AFX_CONTROLSOCKET_H__51DE5049_9A9F_41B0_A132_B2C70C6BEFAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ControlSocket.h : header file
//

class CFTPserverDlg;
class CConnectThread;
class CClientThread;
class CDataSocket;

// 操作标志
#define FTP_DOWNLOAD	1
#define FTP_UPLOAD		2
#define FTP_RENAME		3
#define FTP_DELETE		4
#define FTP_CREATE_DIR	5
#define FTP_LIST		6

// 服务器连接状态
enum
{
	STATUS_IDLE		= 0,
	STATUS_LOGIN	= 1,
	STATUS_LIST		= 2,
	STATUS_UPLOAD	= 3,
	STATUS_DOWNLOAD	= 4,
};

/////////////////////////////////////////////////////////////////////////////
// CControlSocket command target

class CControlSocket : public CSocket
{
// Attributes
public:
// Operations
public:
	CControlSocket();
	virtual ~CControlSocket();
	BOOL SendResponse(LPCTSTR pstrFormat, ...);

// Overrides
public:
	CWinThread* m_pThread;
	CConnectThread* pThread;
	CFTPserverDlg* pWnd;
	CString m_strUserName;

public:
	BOOL CreateDataConnection();
	void DestroyDataConnection();
	void ProcessCommand();
	void GetCommandLine();
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CControlSocket)
	public:
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CControlSocket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
	BOOL GetLocalPath(LPCTSTR lpszRelativePath, CString &strLocalPath);
	BOOL GetRelativePath(LPCTSTR lpszLocalPath, CString &strRelativePath);
	BOOL GetDirectoryList(LPCTSTR lpszDirectory, CString &strResult);
	BOOL CheckAccessRights(LPCTSTR lpszDirectory, int nOption);
	int CheckFileName(CString strFilename, int nOption, CString &strResult);
	int CheckDirectory(CString strDirectory, int nOption, CString &strResult);

protected:
	CString strReName;

	// TRUE:PASV模式 FALSE:PORT模式
	int m_bPassiveMode;
	
	// 对端信息
	int m_nRemotePort;
	CString m_strRemoteHost;
	
	CDataSocket *m_pDataSocket;

	// 操作状态
	int m_nStatus;
	
	// 命令信息
	CStringList m_strCommands;
	
	CString m_RxBuffer;
	CString m_strCurrentDir;
	
	friend CClientThread;
	friend CDataSocket;

	void DoChangeDirectory(LPCTSTR lpszDirectory);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTROLSOCKET_H__51DE5049_9A9F_41B0_A132_B2C70C6BEFAC__INCLUDED_)
