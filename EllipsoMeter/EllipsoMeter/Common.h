#pragma once

#include "EllipsoMeterDlg.h"
#include "EllipsoMeter.h"
#include "TaskHandle.h"
#include "afxwin.h"
#include "afxdtctl.h"

UINT CommonMoveThread(LPVOID pFuncData);
class CCommon : public CDialogEx
{
	DECLARE_DYNAMIC(CCommon)
	void							m_fnSetShowFlag( BOOL bShow );

	void							m_fnSetInit( ); 
	void							m_fnSetEnd( void ); 
	void							m_fnSetView(); 

	BOOL							m_bShow;

	CTaskHandle*					m_pTaskHandle;
	SHEET_RESISTANCE_Format*		m_pTask;

	BOOL							m_fnSetMakeFolder( void );
	CString							m_fnGetCommonFileFullPath( void );
	CString							m_fnGetLastModelFileFullPath( void );
	BOOL							m_fnGetCommon_Load( void );
	BOOL							m_fnSetCommon_Save( void );
	BOOL                            m_fnGetLastModel_Load( void );
	BOOL							m_fnSetLastModel_Save( void );
	CString							m_fnGetDefaultResultPath( void );

	HANDLE							m_hCommonMoveThread;
	BOOL                            m_bCommonMoveEnd;
	int                             m_nCommonMoveStep;
	double                          m_dMoveX;
	double                          m_dMoveY;
	void							m_fnSetMove( double dX, double dY );

public:
	CCommon(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CCommon();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_COMMON_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedBtnExit();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnCommonSave();
	afx_msg void OnBnClickedBtnXMLimit();
	afx_msg void OnBnClickedBtnXPLimit();
	afx_msg void OnBnClickedBtnYMLimit();
	afx_msg void OnBnClickedBtnYPLimit();
	afx_msg void OnEnChangeEdtHighSpeed();
	afx_msg void OnEnChangeEdtMiddleSpeed();
	afx_msg void OnEnChangeEdtSlowSpeed();
	afx_msg void OnEnChangeEdtHighSpeedZ();
	afx_msg void OnEnChangeEdtMiddleSpeedZ();
	afx_msg void OnEnChangeEdtSlowSpeedZ();
	afx_msg void OnBnClickedChkLogSave();
	afx_msg void OnBnClickedBtnLoadingRead();
	afx_msg void OnBnClickedBtnLoadingMove();
	afx_msg void OnBnClickedBtnZLimitRead();
	afx_msg void OnBnClickedBtnZLimitMove();
	afx_msg void OnBnClickedBtnZAlignRead();
	afx_msg void OnBnClickedBtnMeasureRead();
	afx_msg void OnBnClickedBtnZAlignMove();
	afx_msg void OnBnClickedBtnZMeasureMove();
	afx_msg void OnBnClickedBtnZAlignSafetyRead();
	afx_msg void OnBnClickedBtnZAlignSafetyMove();
	afx_msg void OnBnClickedBtnZMeasureSafetyRead();
	afx_msg void OnBnClickedBtnZMeasureSafetyMove();
	afx_msg void OnBnClickedBtnNorch4inchRead();
	afx_msg void OnBnClickedBtnNorch6inchRead();
	afx_msg void OnBnClickedBtnNorch8inchRead();
	afx_msg void OnBnClickedBtnNorch12inchRead();
	afx_msg void OnBnClickedBtnFlat4inchRead();
	afx_msg void OnBnClickedBtnFlat6inchRead();
	afx_msg void OnBnClickedBtnFlat8inchRead();
	afx_msg void OnBnClickedBtnFlat12inchRead();
	afx_msg void OnBnClickedBtnNorch4inchMove();
	afx_msg void OnBnClickedBtnNorch6inchMove();
	afx_msg void OnBnClickedBtnNorch8inchMove();
	afx_msg void OnBnClickedBtnNorch12inchMove();
	afx_msg void OnBnClickedBtnFlat4inchMove();
	afx_msg void OnBnClickedBtnFlat6inchMove();
	afx_msg void OnBnClickedBtnFlat8inchMove();
	afx_msg void OnBnClickedBtnFlat12inchMove();
	afx_msg void OnBnClickedBtnLoadingRead2();
	afx_msg void OnBnClickedBtnLoadingMove2();
	afx_msg void OnBnClickedBtnExcelPath();
	afx_msg void OnBnClickedBtnPathXml();
	afx_msg void OnBnClickedBtnTrendPath();

	afx_msg void OnBnClickedBtnLightValue();
	int m_iEditLightVal;
};


