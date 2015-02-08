#if !defined(AFX_SYSTEMTRAY_H__3DA5B184_0B5F_4505_8C23_E51EED4003F5__INCLUDED_)
#define AFX_SYSTEMTRAY_H__3DA5B184_0B5F_4505_8C23_E51EED4003F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SystemTray.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSystemTray dialog

class CSystemTray : public CDialog
{
// Construction
public:
	CSystemTray();
    virtual ~CSystemTray();
	
    DECLARE_DYNAMIC(CSystemTray)

// Operations
public:
	BOOL Create(UINT uCallbackMessage, LPCTSTR lpszTip, HICON hIcon, UINT uID);
    virtual LRESULT OnTrayNotification(WPARAM uID, LPARAM lEvent);

	// Generated message map functions
	// ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CSystemTray)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

		
// Implementation
protected:
	NOTIFYICONDATA  m_NotifyIconData;
	NOTIFYICONDATA  m_nid;

	//{{AFX_MSG(CSystemTray)
	// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYSTEMTRAY_H__3DA5B184_0B5F_4505_8C23_E51EED4003F5__INCLUDED_)
