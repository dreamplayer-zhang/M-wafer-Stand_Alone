//  EllipsoMeterDlg.h : 헤더 파일
//

#pragma once
#include "AJIN_CtlClass.h"
#include "IO.h"
#include "JogMove.h"
#include "StopWatch.h"
#include "Common.h"
#include "Init.h"
#include "Model.h"
#include "AutoInspectorDlg.h"
#include "StageMapViewDLG.h"
#include "AutoFileDelDLG.h"
#include "ChartDLG.h"
#include "MapViewDLG.h"
#include "ManualDLG.h"
#include "ReviewDLG.h"
#include "USerInputDLG.h"
#include "PasswordDlg.h"
#include "PasswordManagerDLG.h"
#include "PasswordChangeDlg.h"
#include "SearchDlg.h"
#include "ExcelEx.h"
#include "AutoMeasureParamDLG.h"
#include "PointDLG.h"
#include "afxwin.h"

class CEllipsoMeterDlgAutoProxy;


// CEllipsoMeterDlg 대화 상자
UINT VacThread(LPVOID pFuncData);
void ThrdMsgReport();


class CEllipsoMeterDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEllipsoMeterDlg);
	friend class CEllipsoMeterDlgAutoProxy;

	CAJIN_CtlClass*						m_pAjin;
	CElipsoMeter_IO*					m_pIO;
	CJogMove*							m_pJog;
	CCommon*							m_pCommon;
	STOPWATCH							CThTact;
	SHELLEXECUTEINFO					m_siEllipsoSW;
	CModel*								m_pModel;
	CAutoInspectorDlg*					m_pAuto;
	CStageMapViewDLG*					m_pStageMap;
	CMapViewDLG*						m_pMap;
	CMapViewDLG*						m_pReviewMap;
	CAutoFileDelDLG*                    m_pDel;
	CChartDLG*							m_pContour;
	CManualDLG*							m_pManual;
	CReviewDLG*							m_pReview;
	CUSerInputDLG*						m_pUserAuto;
	CUSerInputDLG*						m_pUserReview;
	CSearchDLG*							m_pSearch;
	CPasswordDlg*						m_pLogin;
	CPasswordManagerDLG*				m_pUserInfo;
	CPasswordChangeDLG*					m_pPwdChg;
	CAutoMeasureParamDLG*               m_pAutoParam;
	CPointDLG*							m_pPoint;

	t_MODEL_INFO						m_stModel;
	CExcelEx							m_Excel;
	CString								m_fnGetResultFileFullPath( void );
	CString								m_fnGetStandardExcelFileFullPath( void );
	CString								m_fnGetTempPath( void );

	void								m_fnSetXLSResult( int nMode = SW_AUTO );
	void								m_fnSetViewCommon( void );
	void                                m_fnSetViewJog( void );
	void                                m_fnSetViewModel( void );
	void                                m_fnSetEquipInitialize( void );
	void								m_fnSetViewAuto( void );
	void								m_fnSetViewMap( void );
	void								m_fnSetViewStageMap( void );
	void								m_fnSetViewAutoDel( void );
	void								m_fnSetViewCamera( void );

	void								m_fnSetUpBanner( void );
	void								m_fnSetLogo( void ); 
	void								m_fnSetQrCode( void ); 
	void								m_fnSetTitle( void ); 
	void								m_fnSetDateTime( void ); 
	CString								m_fnGetToDay( void );
	CString								m_fnGetTime( void );

	HANDLE								m_hMutexOneInstance;
	CTaskHandle*						m_pTaskHandle;
	SHEET_RESISTANCE_Format*			m_pTask;

	char								m_szData[MAX_BUFF_SIZE+1];
	CString								m_strOldLogMsg;
	CLogProcess							m_Log;

	BOOL								m_bFirst;
	int									m_nPage;
	int                                 m_nChart;

	void								m_fnLogWrite( CString strMSG );
	void								m_fnSetSwitch( int nNo );

	HANDLE								m_hVacThread;
	BOOL								m_bVacEnd;

	LRESULT								m_fnSetVacOnOff( WPARAM nNo, LPARAM lParam );
	void								m_fnSetChangeBtn( int nSwitch ); 
	int									m_nSwitchNo;
	void								m_fnSetBtnOperation( int nID, 
															 int nBtnColor, 
															 int nTextColor,
															 CString strMsg );

	void								m_fnSetChartView( int nView );

	BOOL								m_bEmergencyStop;

	CFont								m_fMainButtonFont;
	LRESULT m_fnUserMsgSetChartView(WPARAM nNo, LPARAM lParam);

	// 생성입니다.
public:
	CEllipsoMeterDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
	virtual ~CEllipsoMeterDlg();
	BOOL								m_bLampOn;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SHEETRESISTANCEINSPECTOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);


// 구현입니다.
protected:
	CEllipsoMeterDlgAutoProxy* m_pAutoProxy;
	afx_msg LRESULT OnSetCoordinates(WPARAM nPort, LPARAM lParam);
	HICON m_hIcon;

	BOOL CanExit();

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();

	void InitObject();

	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CColorBtn m_clSearch;
	CColorBtn m_clAuto;
	CColorBtn m_clEnv;
	CColorBtn m_clAppExit;
	CColorBtn m_clMaint;
	CColorBtn m_clModel;
	CColorBtn m_clEqInit;
	CColorBtn m_clReview;
	CColorBtn m_clBuzzerOff;
	CColorBtn m_clVacuumOff;
	CColorBtn m_clLogin;
	CColorBtn m_ctrlBtnLampOff;

	afx_msg LRESULT m_fnSetEquipInit( WPARAM nNo, LPARAM lParam );
	afx_msg void OnBnClickedBtnAppExit();
	afx_msg void OnBnClickedBtnModel();
	afx_msg void OnBnClickedBtnSearch();
	afx_msg void OnBnClickedBtnAuto();
	afx_msg void OnBnClickedBtnEnv();
	afx_msg void OnBnClickedBtnMaint();
	afx_msg void OnBnClickedBtnEqInit();
	afx_msg void OnBnClickedBtnBuzzerOff();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedBtnVacOff();
	afx_msg void OnBnClickedBtnReview();
	afx_msg void OnBnClickedBtnLogin();
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	void Initial_Interface();

	afx_msg void OnClose();
	afx_msg void OnBnClickedBtnLampOff();
};

