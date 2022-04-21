#pragma once
#include "AJIN_CtlClass.h"
#include "EllipsoMeterDlg.h"
#include "EllipsoMeter.h"
#include "TaskHandle.h"
#include "afxwin.h"
#include "fpspread_point.h"
#include "ModelMapDLG.h"

UINT RcpMeasureThread(LPVOID pFuncData);
UINT RcpMoveThread(LPVOID pFuncData);
class CModel : public CDialogEx
{
	DECLARE_DYNAMIC(CModel)
	void							m_fnSetShowFlag( BOOL bShow );

	void							m_fnSetParam( CString strUserID, CString strPwdID ); 
	void							m_fnSetInit( ); 
	void							m_fnSetEnd( void ); 

	BOOL							m_bShow;	

	BOOL                            m_bBaseSearch;

	CLogProcess						m_Log;
	CModelMapDLG*                   m_pModelMap;

	CTaskHandle*					m_pTaskHandle;
	SHEET_RESISTANCE_Format*		m_pTask;

	HANDLE							m_hRcpMeasureThread;
	BOOL                            m_bRcpMeasureEnd;
	int                             m_nRcpMeasureStep;
	BOOL                            m_bProcess;

	HANDLE							m_hRcpMoveThread;
	BOOL                            m_bRcpMoveEnd;
	int                             m_nRcpMoveStep;
	double                          m_dMoveX;
	double                          m_dMoveY;

	int                             m_nData;
    double							m_dDataX[2];
	double                          m_dDataY[2];

	BOOL                            m_bDisplay;
	BOOL                            m_bLive;

	char							m_szData[MAX_BUFF_SIZE+1];
	void							m_fnLogWrite( CString strMsg );

	BOOL							m_fnSetMakeFolder( void );
	CString							m_fnGetModelFileFullPath( CString strModelName );
	BOOL							m_fnSetModel_Save( CString strModelName );
	BOOL							m_fnGetModel_Load( CString strModelName );
	void							m_fnSetModel( void );
	CString							m_fnGetStandardModelFileFullPath( void );
	BOOL							m_fnGetCurrModel_Load( CString strModelName );
	CString                         m_strLastModel;
	void							m_fnSetResultDisplay( );
	int                             m_nResultKind;

	double							m_fnGetAlignSafetyZ( double dThickness );
	double							m_fnGetMeasureSafetyZ( double dThickness );
	double							m_fnGetAlignZ( double dThickness );
	double							m_fnGetMeasureZ( double dThickness );
	void							m_fnSRSheetClear( void );
	int  							m_fnGetWaperSize( void );
	void							m_fnSetWaper( t_WAPER_INFO &stWaper );
	void							m_fnSetWaperDivision(t_WAPER_INFO &stWaper);
	void							m_fnGetInchToMM(int nInch, double &dMM, double dExclusion);
	BOOL							m_fnGetCheckAxisData(double dStartX, double dStartY, double dStepX, double dStepY, int nCountX, int nCountY );

	double                          m_dWaperThickness;
	double                          m_dWaperExclusion;

	CListBox m_ctrlListMeasureResult;
	void m_fnCheckAlignUse(BOOL bCheck);
	BOOL m_bSetAlignModel;
	BOOL m_bSetInsertSubPosFinish;
	BOOL m_bPatternFinded;
	void m_fnPosInfoInputSheet(int iStep, int iCol, int iRaw);
	void m_fnModule_ModelLoad(CString &strModel);
	void m_fnModule_CalPattInfo();
	void m_fnModule_GetCurItemVal();

public:
	CModel(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CModel();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_MODEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:
	CFpspread_point m_clSR;
	DECLARE_EVENTSINK_MAP()
	void ButtonClickedFpspreadSRInfo(long Col, long Row, short ButtonDown);
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnLbnDblclkLstModel();


	CListBox m_clModel;
	afx_msg void OnBnClickedBtnAdd();
	afx_msg void OnBnClickedBtnDel();
	afx_msg void OnBnClickedBtnCopy();
	afx_msg void OnBnClickedBtnRename();
	afx_msg void OnBnClickedBtnModelName();
	void ClickFpspreadAlign(long Col, long Row);
	CComboBox m_clWaperSize;
	afx_msg void OnBnClickedBtnAddPoint();
	afx_msg void OnBnClickedBtnRcpInsp();
	CFpspread_point m_clInsp;
	afx_msg void OnBnClickedBtnDelAll();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedChkPreview();
	CComboBox m_clDisp;
	afx_msg void OnBnClickedChkUseSel();
	CComboBox m_clThickness;
	afx_msg void OnBnClickedBtnMapAdd();
	afx_msg void OnCbnSelchangeCboInch();
	afx_msg void OnBnClickedBtnAf();
	afx_msg void OnBnClickedBtnSetPatt();

public:
// 	void m_fnTranferCamBuff2PattBuff(BOOL bAutoPattAlignMode = FALSE);

	afx_msg void OnBnClickedBtnFindPatt();

	void m_fnModule_FindPatt();

	afx_msg void OnBnClickedBtnPoint();
	afx_msg void OnEnChangeEdtThickness();
	afx_msg void OnEnChangeEdtExclusion();

	afx_msg void OnBnClickedBtnAlignMapping();
	CEdit m_ctrlEditPattSizeX;
	CEdit m_ctrlEditPattSizeY;
	CEdit m_ctrlEditPattGabX;
	CEdit m_ctrlEditPattGabY;
	afx_msg void OnBnClickedButtonGetPos();
	CEdit m_ctrlEditPattPosX;
	CEdit m_ctrlEditPattPosY;
	afx_msg void OnBnClickedBtnAlignPosAutoSet();


	afx_msg void AlignPosAutoSet_Test4Point();

	afx_msg void OnBnClickedBtnCalcPos();
	afx_msg void OnLbnSelchangeListMeasureResult();
	CButton m_ctrlCheckSetSubPoint;
	CButton m_ctrlCheckAlignUse;
	afx_msg void OnBnClickedChkAlignUse();
	afx_msg void OnBnClickedCheckSubpoint();
	afx_msg void OnBnClickedBtnAf2();
	afx_msg void OnBnClickedButton3();

//	afx_msg void OnLbnSelchangeLstModel();
	afx_msg void OnBnClickedBtnInputRowcolAutoCalc();
	CButton m_ctrlRadioModelType;
	BOOL m_iRadioModelType;
	afx_msg void OnBnClickedRadioModelType(UINT wParam);
	void m_fnSetRadioModelType(int iCheckNum);
	void ChangeFpspreadSrInfo(long Col, long Row);
	afx_msg void OnLbnSelchangeLstModel();
};
