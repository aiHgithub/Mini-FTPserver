// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__3897CF3B_5F98_4DB1_9373_EA4697D284A2__INCLUDED_)
#define AFX_STDAFX_H__3897CF3B_5F98_4DB1_9373_EA4697D284A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxsock.h>		// MFC socket extensions

// 使用CTypedPtrList
#include <afxtempl.h>

// 使用CCriticalSection
#include <afxmt.h>

#define FTPSTAT_DOWNLOADSUCCEEDED	2
#define FTPSTAT_UPLOADSUCCEEDED		3
#define FTPSTAT_DOWNLOADFAILED		4
#define FTPSTAT_UPLOADFAILED		5

// 线程消息
#define WM_THREADSTART				WM_USER+200
#define WM_THREADCLOSE				WM_USER+201

// 托盘消息
#define WM_DESTROYDATACONNECTION	WM_USER+204

extern void DoEvents();
extern CString BrowseForFolder(HWND hWnd, LPCSTR lpszTitle, UINT nFlags);
extern BOOL WaitWithMessageLoop(HANDLE hEvent, int nTimeout);

BOOL MakeSureDirectoryPathExists(LPCTSTR lpszDirPath);
BOOL FileExists(LPCTSTR lpszFileName, BOOL bIsDirCheck);
CString GetFileDate(CFileFind &find);
int GetLocalHostIP(CString &strIPAddress);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__3897CF3B_5F98_4DB1_9373_EA4697D284A2__INCLUDED_)

