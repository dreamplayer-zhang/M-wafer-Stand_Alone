#pragma once

#include "fpspread_point.h"
#include "EllipsoMeterDlg.h"
void ThrdLampOnCheck(BOOL bOn = TRUE);
// CInit 대화 상자입니다.
UINT EqInitThread(LPVOID pFuncData);

class CInit : public CDialogEx
{
	DECLARE_DYNAMIC(CInit)
	void							m_fnInit();
	BOOL                            m_bFirst;
	BOOL                            m_bInit;
	int                             m_nStep;
	int                             m_nCount;
	int                             m_nRtn[MAX_AXIS+1];
	int                             m_nInit;
	BOOL							m_bEqInitEnd;
	HANDLE							m_hEqInitThread;
	void							m_fnSetInitCtl( BOOL bEnable );
	void							m_fnSetEnd( void );

	CString                         m_strProcess[MAX_AXIS + 1];
	BOOL                            m_bProcess;

	CTaskHandle*					m_pTaskHandle;
	SHEET_RESISTANCE_Format*		m_pTask;

public:
	CInit(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CInit();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_INIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT m_fnSetInitEnd( WPARAM nNo, LPARAM lParam );
	afx_msg void OnBnClickedBtnInit();
	afx_msg void OnBnClickedBtnInitStop();
	afx_msg void OnBnClickedBtnAppExt();
	CFpspread_point m_clInit;
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnBnClickedBtnInitShow();
	afx_msg void OnPaint();
};
