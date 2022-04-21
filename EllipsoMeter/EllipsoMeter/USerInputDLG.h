#pragma once
#include "afxwin.h"
#include "fpspread_point.h"


// CUSerInputDLG 대화 상자입니다.

class CUSerInputDLG : public CDialogEx
{
	DECLARE_DYNAMIC(CUSerInputDLG)
	BOOL							m_bShow;	
	void							m_fnSetShowFlag( BOOL bShow );

	void							m_fnSetInit( void ); 
	void							m_fnSetEnd( void ); 

	CTaskHandle*					m_pTaskHandle;
	SHEET_RESISTANCE_Format*		m_pTask;
	void                            m_fnSetInputData( t_USER_INFO stUser ); 
	void                            m_fnGetInputData( t_USER_INFO &stUser ); 

public:
	CUSerInputDLG(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CUSerInputDLG();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_USER_INPUT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnHide();
	CEdit m_ctrlEditDate;
	CEdit m_ctrlEditTime;
	afx_msg void OnEnChangeEdtTime();
	CFpspread_point m_ctrlUserInfo;
};
