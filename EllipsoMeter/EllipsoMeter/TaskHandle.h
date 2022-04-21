#if !defined(AFX_TASKHANDLE_H_78AD778D_431D_451F_8DC1_3314CE025F9E__INCLUDED_)
#define AFX_TASKHANDLE_H_78AD778D_431D_451F_8DC1_3314CE025F9E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////////////
// TaskHandle.h : Header File
//
/////////////////////////////////////////////////////////////////////////////
// CTaskHandle Window

//---------------------------------------------------------
// Polling Á¤ÀÇ
//---------------------------------------------------------
#define WM_SET_COORDINATES   (WM_USER+2000)
class CTaskHandle : public CWnd
{
// Construction
public:

	CTaskHandle( CWnd*   pWnd, 
				 UINT    nPort, 
				 int     nSize,
				 int     nWait,
				 BOOL    bStart );

	CTaskHandle( CWnd*   pWnd, 
	 			 CString strFileName ,
				 UINT    nPort, 
				 int     nSize,
				 int     nWait, 
				 BOOL    bStart );

// Attributes
public:

// Operations
public:
	LPCH m_pFilePointer( );
	void m_fnSetEnd( ); 


// Overrides
	// ClassWizard generated virtual function overrides

	//{{AFX_VIRTUAL(CTaskHandle)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTaskHandle();

private:

protected:
	void        m_Share_Memory_Create( CString String, int nSize );
	CString     m_Share_FileName;
	HANDLE      m_hMapObject;
	LPCH        m_pMapView;
	CWinThread* m_pThread;

	BOOL        m_bStart;
	CString     m_strMsg;

	// Generated message map functions
	//{{AFX_MSG(CTaskHandle)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASKHANDLE_H_78AD778D_431D_451F_8DC1_3314CE025F9E__INCLUDED_)
