//
// AutoInspectorDlg.h : 헤더 파일
//
#pragma once
#include "EllipsoMeter.h"
#include "EllipsoMeterDlg.h"
#include "TaskHandle.h"
#include "fpspread_point.h"
#include "StopWatch.h"
#include "afxwin.h"

UINT ExcelWriteThread(LPVOID pFuncData);
void ThrdReadReceipeList(LPVOID pFuncData);
class CAutoInspectorDlg : public CDialogEx
{
public:
	DECLARE_DYNAMIC(CAutoInspectorDlg);

	BOOL								m_bShow;	
	void								m_fnSetShowFlag( BOOL bShow );
	CFont		m_fWaferNumFont;
	CFont		m_fAMFont;
	CFont		m_fModeGroupFont;
	CFont		m_fButtonFont;

	HWND		h_wndWaferNum;

	void								m_fnSetInit( void ); 
	void								m_fnSetEnd( void );

	BOOL								m_bOperation;
	void								m_fnGetModelList( void );

	BOOL                                m_bThTact;

	CTaskHandle*						m_pTaskHandle;
	SHEET_RESISTANCE_Format*			m_pTask;

	CLogProcess                         m_Log;
	CString                             m_strOldLogMsg;
	char                                m_szData[MAX_BUFF_SIZE+1];

	LRESULT								m_fnSetStartButton( WPARAM nNo, LPARAM lParam );
	LRESULT								m_fnSetCameraConnect( WPARAM nNo, LPARAM lParam );
	LRESULT								m_fnSetReport( WPARAM nNo, LPARAM lParam );
	CString								m_fnGetTempPath( void );
	LRESULT								m_fnSetReportReady(WPARAM nNo, LPARAM lParam);

	void								m_fnLogWrite( CString strMSG ); 
	void								m_fnTECmsgOut( CString str, COLORREF color, int flag );
	void								m_fnSetOperationStop( void );

// 	CString								m_fnGetResultFileFullPath( void );
	CString								m_fnGetStandardResultFileFullPath( void );
	CString								m_fnGetStandardExcelFileFullPath( void );
	void								m_fnSetXMLResult( CString strModel, t_MODEL_INFO &stReport,  t_RESULT_INFO stData, int nMode );
	CString								m_fnGetMXLPath( CString strDate, CString strTime, CString strLotID );

	void								m_fnSetInspStatus( double dMax, double dMin, double dAvg, double dStd, double dMax_Min, double dUniformity );
	void								m_fnSetDebugThTactTime( void );
	void								m_fnSetReCalculate( void );
	CString								m_fnGetSelectResultData(t_ELIPSO_RESULT_INFO stResult, int nMeasureItem, int nLayer);

	t_USER_INFO							m_stUser;
	CString								m_strInspDate;
	CString								m_strTactTime;
	CString								m_strInspTime;
	CString                             m_strLotStartTime;

	CString								m_strWaferTimeStart;
	CString								m_strWaferTimeEnd;
	CString								m_strWaferTimeSpend;
	CTimeInterval						m_tiAWafer;


// 	// INSP Thread
// 	HANDLE								m_hInspThread;
// 	BOOL								m_bInspEnd;
// 
// 	// Auto Thread
// 	HANDLE								m_hAutoMoveThread;
// 	BOOL								m_bAutoMoveEnd;
// 	int                                 m_nAutoMoveStep;
	double                              m_dPositionX;
	double                              m_dPositionY;
// 
// 	// Auto Thread
// 	HANDLE								m_hAutoMeasureThread;
// 	BOOL								m_bAutoMeasureEnd;
// 	int                                 m_nAutoMeasureStep;
// 	int                                 m_nRow;
// 	double                              m_dPositionZ;



	BOOL                                m_bFirstInsp;

	STOPWATCH							CThTact;
	void								m_fnSetThTactTime( void );

	int                                 m_nExcelWriteStep;
	HANDLE								m_hExcelWriteThread;
	BOOL								m_bExcelWriteEnd;

	int                                 m_nStep;
	BOOL                                m_bInspProcess;
	BOOL                                m_bPause;

	int									m_nWaperSize;
	int									m_nWaperSize_old;
	BOOL								m_bWaperType;
	int									m_nMeasureItem;
	BOOL                                m_bAlign;
	double                              m_dWaperThickness;
	double                              m_dWaperExclusion;
	void m_fnSetUserInput( t_USER_INFO* pstUser );
	int									m_iDivisionNum;
	int									m_iRadioMeasureMode;
	int									m_iRadioMeasureMode_old;

	LRESULT m_fnSetChangeBtn( WPARAM nSwitch, LPARAM lParam = 0 );
	int									m_nSwitchNo;
	LRESULT								m_fnSetInspSwitch( WPARAM nNo, LPARAM lParam );
	void								m_fnSetBtnOperation( int nID, 
															   int nBtnColor, 
															   int nTextColor,
															   CString strMsg );
	void m_fnSRSheetClear( BOOL bAllClear = TRUE );
	double								m_fnGetEdgeExclusion( void );
	void								m_fnSetView(void);

	int									m_nSelLayer;
	CComboBox m_ctrlComboReceipe;
	CEdit m_ctrlEditWaferNum;

	BOOL	m_bPattAlignMode;
	CButton m_ctrlPattAlignMode;

	int m_iSetPntMode;

	CComboBox m_clInchKind;
	CComboBox m_clWaperSize;
	CComboBox m_clWaperType;
	CComboBox m_clDani;
	CComboBox m_clModelList;
	CComboBox m_clModelList2;
	CButton m_ctrlCheckManualMeasure;
	CComboBox m_clLayer;

	void								m_fnSetTargetSubCalcPoint();

	//case 0://로그인:AIM
	//case 1://자동투입/수동투입
	//case 2://수동투입 - 수동측정/자동측정
	//case 3://자동측정중
	//default://로그아웃
	void								m_fnSetBtnEnable(int iMode);

// 생성입니다.
public:
	CAutoInspectorDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
	virtual ~CAutoInspectorDlg();

	CButton m_ctrlChkUseAF;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_AUTO_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

// 구현입니다.
protected:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
public:
	CColorBtn m_clInspStart;
	CColorBtn m_clInspStop;
	CColorBtn m_clInspRestart;
	CColorBtn m_clInspPause;
	afx_msg void OnCbnSelchangeCboModelList();

	void InsertSheet(int nDataCnt);

	CFpspread_point m_clResult;
	CFpspread_point m_clInfo;
	DECLARE_EVENTSINK_MAP()
	void ClickFpspreadResult(long Col, long Row);
	afx_msg void OnBnClickedBtnDebug();
	afx_msg void OnBnClickedBtnInspStart();
	afx_msg void OnBnClickedBtnInspStop();
	afx_msg void OnBnClickedBtnRestart();
	afx_msg void OnBnClickedBtnPause();
	afx_msg void OnBnClickedBtnPreview();
	afx_msg void OnBnClickedBtnUserInfo();
	afx_msg void OnBnClickedBtnReport();
	afx_msg void OnBnClickedBtnCw();
	afx_msg void OnBnClickedBtnCcw();
	afx_msg void OnBnClickedChkAlign();
	afx_msg void OnCbnSelchangeCboInch();
	afx_msg void OnCbnSelchangeCboDani();
	afx_msg void OnCbnSelchangeCboType();
	afx_msg void OnEnChangeEdtThickness();
	afx_msg void OnBnClickedBtnResetData();
	afx_msg void OnBnClickedBtnUserInput();
	afx_msg void OnBnClickedBtnResult();
	afx_msg void OnBnClickedBtnAutoMeasure();
	afx_msg void OnBnClickedBtnPoint4();
	afx_msg void OnBnClickedBtnPoint12();
	afx_msg void OnBnClickedBtnPoint28();
	afx_msg void OnBnClickedBtnPoint60();
	void DivisionCirclePos( int iDivisionNum);
	afx_msg void OnBnClickedBtnPoint124();
	afx_msg void OnBnClickedBtnPoint252();
	afx_msg void OnBnClickedBtnPoint508();
// 	CComboBox m_clMethod;
	afx_msg void OnBnClickedBtnCurrXy();
	afx_msg void OnBnClickedBtnSquareAdd();
	afx_msg void OnEnChangeEdtEdgeExclusion();
	afx_msg void OnBnClickedBtnThickness();
// 	CComboBox m_ctrlComboWaferNum;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnCbnSelchangeCboWaferNum();
	CComboBox m_clMeasureItem;
	afx_msg void OnCbnSelchangeMeasureItem();
	afx_msg void OnBnClickedBtnRectangleAdd();
	CComboBox m_clMethod;
	afx_msg void OnCbnDropdownComboReceipe();
	afx_msg void OnCbnSelchangeComboReceipe();
	CButton m_ctrlCheckAutoMode;
	afx_msg void OnBnClickedCheckAutomode();
	afx_msg void OnBnClickedCheckPattMode();
	afx_msg void OnCbnSelchangeCboModelList2();
	afx_msg void OnBnClickedBtnInitStage();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedBtnPoint5();
	CButton m_ctrlCheckWaferAlign;
	afx_msg void OnBnClickedCheckWaferAlign();
	afx_msg void OnBnClickedCheckAutomode2();
	CButton m_ctrlCheckAutoMode2;
	afx_msg void OnBnClickedButtonDataInit();
	afx_msg void OnBnClickedButtonAutoFocus();
	afx_msg void OnBnClickedButtonManualMeasure();
	afx_msg void OnBnClickedCheckManualMeasure();
	afx_msg void OnCbnSelchangeCboLayer2();
	afx_msg void OnBnClickedCheckUseAf();
	CButton m_ctrlRadioMeasureMode;
	afx_msg void OnBnClickedRadioMeasureMode(UINT wParam);
	afx_msg void OnBnClickedButtonAutoFocusVision();
	afx_msg void OnBnClickedButtonRandom();
};
