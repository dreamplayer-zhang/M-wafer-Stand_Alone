#pragma once
#include "afxwin.h"
#include "EllipsoMeterDlg.h"


// CModelManage ��ȭ �����Դϴ�.

class CModelManage : public CDialogEx
{
	DECLARE_DYNAMIC(CModelManage)
	BOOL                            m_bFirst;
	BOOL							m_nNO;
	void							m_fnInit( int nNO, CString strName );
	CString                         m_strName;
	CEllipsoMeterDlg*	m_pView;

public:
	CModelManage(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CModelManage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_MODEL_MANAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnApply();
	afx_msg void OnBnClickedBtnCancel();
	CString m_strMODEL;
	CString m_strNewMODEL;
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
};
