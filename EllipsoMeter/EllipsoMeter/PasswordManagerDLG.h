#pragma once
#include "afxwin.h"


// CPasswordManagerDLG ��ȭ �����Դϴ�.

class CPasswordManagerDLG : public CDialogEx
{
	DECLARE_DYNAMIC(CPasswordManagerDLG)
	BOOL							m_bShow;	
	void							m_fnSetShowFlag( BOOL bShow );

	void							m_fnSetInit( void ); 
	void							m_fnSetEnd( void );

	CString                         m_strPassword;

	CTaskHandle*					m_pTaskHandle;
	SHEET_RESISTANCE_Format*		m_pTask;


public:
	CPasswordManagerDLG(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CPasswordManagerDLG();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_PASSWORD_MANAGER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnHide();
	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnBnClickedBtnDel();
	afx_msg void OnBnClickedBtnUpdate();
	afx_msg void OnBnClickedBtnChange();
	CComboBox m_clLevel;
	CListBox m_clUserList;
	afx_msg void OnLbnSelchangeLstUserList();
};
