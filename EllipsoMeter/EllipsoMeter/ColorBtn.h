#if !defined(AFX_MYCLASS_H__8E7A4504_82E5_4C81_8A30_642AC65A4550__INCLUDED_)
#define AFX_MYCLASS_H__8E7A4504_82E5_4C81_8A30_642AC65A4550__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyClass.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorBtn window

class CColorBtn : public CButton
{
// Construction
public:
	void m_fnFlashBackground(BOOL bActivate);
	
	CColorBtn();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorBtn)
	//}}AFX_VIRTUAL

// Implementation
public:
	void m_fnSetParam(int nBtnColor = 0, int nTextColor = 0);
	virtual ~CColorBtn();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorBtn)
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	void m_fnSetBtnColor(int nBtnColor);
	void m_fnSetTextColor(int nTextColor);

	COLORREF m_BtnColorRef;
	COLORREF m_TextColorRef;
	
	int m_nTimer;
	int m_nBtnColor;
	int m_nTextColor;
	BOOL m_bDraw;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYCLASS_H__8E7A4504_82E5_4C81_8A30_642AC65A4550__INCLUDED_)
