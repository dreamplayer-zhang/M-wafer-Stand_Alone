#pragma once

#include "stdafx.h"
#include "afxwin.h"
// CReviewDLG 대화 상자입니다.

UINT ExcelWriteThreadReview(LPVOID pFuncData);
class CReviewDLG : public CDialogEx
{
	DECLARE_DYNAMIC(CReviewDLG)
	BOOL							m_bShow;	
	void							m_fnSetShowFlag( BOOL bShow );

	void							m_fnSetInit( void ); 
	void							m_fnSetEnd( void ); 

	CTaskHandle*					m_pTaskHandle;
	SHEET_RESISTANCE_Format*		m_pTask;

	t_USER_INFO						m_stUser;
	CString							m_strModel;
	BOOL							m_fnGetXMLResult( CString strXMLFile );
	CString							m_fnGetResultFileFullPath( void );
	void							m_fnSetInspStatus( double dMax, double dMin, double dAvg, double dStd, double dMax_Min, double dUniformity );
	int                             m_nMeasureItem;
	int                             m_nSelLayer;
	void							m_fnSRSheetClear(BOOL bAllClear = TRUE);
	void							m_fnSetReCalculate(void);
	LRESULT							m_fnSetReportReview(WPARAM nNo, LPARAM lParam);
	LRESULT							m_fnSetReportReadyReview(WPARAM nNo, LPARAM lParam);

	int                             m_nExcelWriteStep;
	HANDLE							m_hExcelWriteThread;
	BOOL							m_bExcelWriteEnd;

	CFont		m_fWaferNumFont;
	CEdit m_ctrlEditWaferNum;
	CComboBox m_clLayer;

public:
	CReviewDLG(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CReviewDLG();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void							m_fnSetView( void );

// 대화 상자 데이터입니다.
	enum { IDD = IDD_REVIEW_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnUserInfo();
	afx_msg void OnBnClickedBtnXmlPath();
	afx_msg void OnBnClickedBtnReport();
	CFpspread_point m_clReview;
	CFpspread_point m_clReviewResult;
	DECLARE_EVENTSINK_MAP()
	void ClickFpspreadResult(long Col, long Row);
	CComboBox m_clMeasureItem;
	afx_msg void OnCbnSelchangeMeasureItem();
	afx_msg void OnBnClickedBtnResult();
	afx_msg void OnCbnSelchangeCboLayer();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
