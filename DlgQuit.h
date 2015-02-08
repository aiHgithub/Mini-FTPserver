#if !defined(AFX_DLGQUIT_H__8FC05833_9F68_4092_9FF1_1BB3C1ACEEEE__INCLUDED_)
#define AFX_DLGQUIT_H__8FC05833_9F68_4092_9FF1_1BB3C1ACEEEE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgQuit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgQuit dialog

class CDlgQuit : public CDialog
{
// Construction
public:
	CDlgQuit(CWnd* pParent = NULL);   // standard constructor
	CWnd* m_pWndServer;
// Dialog Data
	//{{AFX_DATA(CDlgQuit)
	enum { IDD = IDD_QUIT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgQuit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgQuit)
	afx_msg void OnBtnCancel();
	afx_msg void OnBtnExit();
	afx_msg void OnBtnMinsize();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGQUIT_H__8FC05833_9F68_4092_9FF1_1BB3C1ACEEEE__INCLUDED_)
