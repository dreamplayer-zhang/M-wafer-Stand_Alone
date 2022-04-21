#pragma once

#include "EllipsoMeter.h"
#include "EllipsoMeterDlg.h"
#include "TaskHandle.h"
#include "afxwin.h"
#include "ColorBtn.h"

// CElipso_IO 대화 상자입니다.

UINT ElipsoIoThread(LPVOID pFuncData);
UINT ElipsoStageInOutThread(LPVOID pFuncData);
class CElipsoMeter_IO : public CDialogEx
{
	DECLARE_DYNAMIC(CElipsoMeter_IO)
	BOOL							m_bShow;	
	void							m_fnSetShowFlag( BOOL bShow );

	void							m_fnSetInit( void ); 
	void							m_fnSetEnd( void ); 

	CTaskHandle*					m_pTaskHandle;
	SHEET_RESISTANCE_Format*		m_pTask;
	
	HANDLE							m_hIoThread;
	BOOL                            m_bIoEnd;
	unsigned int                    m_nLoading;
	unsigned int                    m_nStart;
	BOOL                            m_bLoadingStop;
	BOOL                            m_bProcess;

	// INSP Thread
	int                             m_nStageInOut;
	BOOL							m_bOperation;
	LRESULT							m_fnSetLoadingButton( WPARAM nNo, LPARAM lParam );
	LRESULT							m_fnSetLoadingSwLight( WPARAM nNo, LPARAM lParam );
	LRESULT							m_fnSetStartSwLight( WPARAM nNo, LPARAM lParam );
	LRESULT							m_fnSetSignalTower( WPARAM nNo, LPARAM lParam );
	LRESULT							m_fnSetBuzzer( WPARAM nNo, LPARAM lParam );
	LRESULT							m_fnSetVacuum( WPARAM nNo, LPARAM lParam );
	HANDLE							m_hStageInOutThread;
	BOOL							m_bStageInOutEnd;
	short							m_fnSetOUTPUT( int nNo );
	short							m_fnSetOutputSignal( int nSignal, BOOL bON );

protected:
	char							m_szData[MAX_BUFF_SIZE+1];
	BOOL							m_bProcessing;
	CLogProcess						m_Log;
	CLogProcess						m_HexaLog;
	void							m_fnSetBtnOperation( int nID, 
														 int nBtnColor, 
														 int nTextColor,
														 CString strMsg );
	void							m_fnLogWrite( CString strMsg );
	void							m_fnLogRvcDataWrite( CString strMsg );
	void							m_fnTECmsgOut( CString str, COLORREF color, int flag );

	DWORD                           m_dwINPUT;
	DWORD                           m_dwOUTPUT;

public:
	CElipsoMeter_IO(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CElipsoMeter_IO();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ELIPSO_IO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CListBox m_lstRcvComm;
	CListBox m_lstComm;
	afx_msg void OnBnClickedBtnExit();
	afx_msg void OnBnClickedBtnSave();
	CColorBtn	m_btnIn[ELIPSO_IO_IN_CNT];
	CColorBtn	m_btnOutput[ELIPSO_IO_OUT_CNT];

	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT m_fnIoRead( WPARAM nNo, LPARAM lParam );
	afx_msg void OnBnClickedBtn1Out01();
	afx_msg void OnBnClickedBtn1Out02();
	afx_msg void OnBnClickedBtn1Out03();
	afx_msg void OnBnClickedBtn1Out04();
	afx_msg void OnBnClickedBtn1Out05();
	afx_msg void OnBnClickedBtn1Out06();
	afx_msg void OnBnClickedBtn1Out07();
	afx_msg void OnBnClickedBtn1Out08();
	afx_msg void OnBnClickedBtn1Out09();
	afx_msg void OnBnClickedBtn1Out10();
	afx_msg void OnBnClickedBtn1Out11();
	afx_msg void OnBnClickedBtn1Out12();
	afx_msg void OnBnClickedBtn1Out13();
	afx_msg void OnBnClickedBtn1Out14();
	afx_msg void OnBnClickedBtn1Out15();
	afx_msg void OnBnClickedBtn1Out16();
};
