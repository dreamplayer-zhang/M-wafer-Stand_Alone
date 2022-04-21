#pragma once
#include "afxwin.h"
#include "EllipsoMeterDlg.h"


// CModelManage 대화 상자입니다.

class CModelManage : public CDialogEx
{
	DECLARE_DYNAMIC(CModelManage)
	BOOL                            m_bFirst;
	BOOL							m_nNO;
	void							m_fnInit( int nNO, CString strName );
	CString                         m_strName;
	CEllipsoMeterDlg*	m_pView;

public:
	CModelManage(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CModelManage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_MODEL_MANAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
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
