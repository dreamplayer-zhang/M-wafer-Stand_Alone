#pragma once
#include "afxwin.h"


// CPointDLG ��ȭ �����Դϴ�.

class CPointDLG : public CDialogEx
{
	DECLARE_DYNAMIC(CPointDLG)
	BOOL							m_bShow;
	void							m_fnSetShowFlag( BOOL bShow );

	void							m_fnSetInit( void ); 
	void							m_fnSetEnd( void ); 

	CTaskHandle*					m_pTaskHandle;
	SHEET_RESISTANCE_Format*		m_pTask;

public:
	CPointDLG(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CPointDLG();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_MODEL_POINT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	afx_msg void OnCbnSelchangeCboInch();
	afx_msg void OnBnClickedBtnCw();
	CComboBox m_clInchKind;
	afx_msg void OnCbnSelchangeCboInchKind();
	afx_msg void OnBnClickedBtnCcw();
	afx_msg void OnBnClickedBtnCwAdd();
	afx_msg void OnBnClickedBtnCcwAdd();
	afx_msg void OnBnClickedBtnCurr();
	afx_msg void OnBnClickedBtnRepeat();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnHide();
	afx_msg void OnBnClickedBtnSquareAdd();
	CComboBox m_clMethod;
	afx_msg void OnBnClickedBtnCurrXy();
};
