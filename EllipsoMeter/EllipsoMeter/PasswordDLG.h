#include "afxwin.h"
#include "resource.h"
#if !defined(AFX_PASSWORDDLG_H__94F41C99_C775_4571_9232_C0B513832DC5__INCLUDED_)
#define AFX_PASSWORDDLG_H__94F41C99_C775_4571_9232_C0B513832DC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PasswordDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg dialog
class CPasswordDlg : public CDialog
{
// Construction
public:
	CPasswordDlg(CWnd* pParent = NULL);   // standard constructor
	CString m_fnGetCurrentTime();
	BOOL							m_bShow;	
	void							m_fnSetShowFlag( BOOL bShow );

	void							m_fnSetInit( void ); 
	void							m_fnSetEnd( void ); 

	CTaskHandle*					m_pTaskHandle;
	SHEET_RESISTANCE_Format*		m_pTask;

	void							m_fnGetUserInfo( void );
	void							m_fnSetUserInfo( void );
	CString							m_fnGetUserFileFullPath( void );
	CString							m_fnGetStandardUserFileFullPath( void );
	void							m_fnDefaultUserAdd( void ); 

// Dialog Data
	//{{AFX_DATA(CPasswordDlg)
	enum { IDD = IDD_PASSWORD };
	CString	m_strPassword;
	CString	m_strUserID;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPasswordDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPasswordDlg)
	afx_msg void OnBtnCancel();
	afx_msg void OnBtnOk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_clUserID;
	CComboBox m_clPassword;
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PASSWORDDLG_H__94F41C99_C775_4571_9232_C0B513832DC5__INCLUDED_)
