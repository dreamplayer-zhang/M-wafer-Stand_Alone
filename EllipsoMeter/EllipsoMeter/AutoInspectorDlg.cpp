//
// AutoInspectorDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "EllipsoMeter.h"
#include "AutoInspectorDlg.h"
#include "afxdialogex.h"
#include "WorkRun.h"
#include "MilVision.h"
#include "Task.h"
#include "ModelData.h"

IMPLEMENT_DYNAMIC(CAutoInspectorDlg, CDialogEx);
static int nCmdBtn [] = { IDC_BTN_INSP_START,
	                      IDC_BTN_INSP_STOP,
						  IDC_BTN_RESTART,
						  IDC_BTN_PAUSE
};

static CString strCmdBtn[] = { "검사시작",
	                          "검사종료",
						      "재 시작",
						      "잠시멈춤"
};

void CAutoInspectorDlg::m_fnSetTargetSubCalcPoint()
{
#if MIL_USE
	CString strMM("");
	double dMM = 0.0;
	double dMM1 = 0.0;
	CString strData("");
	CString strTargetNo("");
	CString strSubNo("");
	CString strX("");
	CString strY("");
	CString strR("");

	pTask->iGroupNum = 0;

	m_fnSRSheetClear();

	int iArr;

	CString strCalPath;
	strCalPath.Format(_T("%sModel\\%s\\"), pModelData->curDirName, pWorkRun->m_strPatternName.GetString());
	pInsp[DEF_DISP_SET]->LoadCal(strCalPath);

	for (int i = 0; i < m_pTask->stTarget.nSRData; i++)
	{
		//stTarget.stSRData[i].dwXPosition = 행열값으로 Trans Calc
		//stTarget.stSRData[i].dwYPosition = 행열값으로 Trans Calc
// 		pTask->inX = (double)m_pTask->stTarget.stSRData[i].iColum;
// 		pTask->inY = (double)m_pTask->stTarget.stSRData[i].iRow;
// 		GetTransformCoordinate(pTask->iGroupNum, pTask->inX, pTask->inY, &pTask->OutX, &pTask->OutY);
// 		m_pTask->stTarget.stSRData[i].dwXPosition = pTask->OutX;
// 		m_pTask->stTarget.stSRData[i].dwYPosition = pTask->OutY;

		for (int j = 0; j < m_pTask->stSub.nSRData; j++)
		{
			iArr = (i * m_pTask->stSub.nSRData) + j;
			memcpy(&m_pTask->stResult.stSRData[iArr].szName, &m_pTask->stTarget.stSRData[i].szName, sizeof(m_pTask->stResult.stSRData[iArr].szName));
			m_pTask->stResult.stSRData[iArr].iColum = m_pTask->stTarget.stSRData[i].iColum;
			m_pTask->stResult.stSRData[iArr].iRow = m_pTask->stTarget.stSRData[i].iRow;
			m_pTask->stResult.stSRData[iArr].iSub = j+1;
// 			m_pTask->stResult.stSRData[iArr].dwXPosition = m_pTask->stTarget.stSRData[i].dwXPosition + m_pTask->stSub.stSRData[j].dwXPosition;
// 			m_pTask->stResult.stSRData[iArr].dwYPosition = m_pTask->stTarget.stSRData[i].dwYPosition + m_pTask->stSub.stSRData[j].dwYPosition;

			//위성좌표 보정
			pTask->inX = (double)m_pTask->stTarget.stSRData[i].iColum + m_pTask->stSub.stSRData[j].dColum;
			pTask->inY = (double)m_pTask->stTarget.stSRData[i].iRow + m_pTask->stSub.stSRData[j].dRow;
			GetTransformCoordinate(pTask->iGroupNum, pTask->inX, pTask->inY, &pTask->OutX, &pTask->OutY);
			m_pTask->stResult.stSRData[iArr].dwXPosition = pTask->OutX;
			m_pTask->stResult.stSRData[iArr].dwYPosition = pTask->OutY;
			//위성좌표 보정


		}
	}

	m_pTask->stResult.nSRData = m_pTask->stTarget.nSRData *  m_pTask->stSub.nSRData;
#endif
}

void CAutoInspectorDlg::m_fnSetBtnEnable(int iMode)
{
	BOOL bEnable = TRUE;
	int ArrExcepCnt = 0;
	int* ArrExcepID;

	//로그아웃
	//로그인
		//자동투입
		//수동투입
			//수동측정
			//자동측정
	switch (iMode)
	{
	case 0://로그인:AIM
		EnableDlgItem(this, TRUE);//모든버튼 활성
		return;

	case 1://자동투입/수동투입
		bEnable = TRUE;
		ArrExcepCnt = 5;
		ArrExcepID = new int[ArrExcepCnt];
		ArrExcepID[0] = IDC_BUTTON_DATA_INIT;
		ArrExcepID[1] = IDC_BUTTON_AUTO_FOCUS;
		ArrExcepID[2] = IDC_BUTTON_MANUAL_MEASURE;
		if(m_ctrlCheckAutoMode.GetCheck())
			ArrExcepID[3] = IDC_CHECK_MANUAL_MEASURE;
		else
			ArrExcepID[3] = IDC_BUTTON_MANUAL_MEASURE;//허수
		ArrExcepID[4] = IDC_BUTTON_AUTO_FOCUS_VISION;
		break;

	case 2://수동투입 - 수동측정/자동측정
		bEnable = FALSE;
		ArrExcepCnt = 15;
		ArrExcepID = new int[ArrExcepCnt];
		ArrExcepID[0] = IDC_BUTTON_DATA_INIT;
		ArrExcepID[1] = IDC_BUTTON_AUTO_FOCUS;
		ArrExcepID[2] = IDC_BUTTON_MANUAL_MEASURE;
		ArrExcepID[3] = IDC_BUTTON_STAGE_INIT;
		ArrExcepID[4] = IDC_BTN_REPORT;
		ArrExcepID[5] = IDC_BTN_RESULT;
		ArrExcepID[6] = IDC_MEASURE_ITEM;
		ArrExcepID[7] = IDC_BTN_USER_INPUT;
		ArrExcepID[8] = IDC_COMBO_RECEIPE;
		ArrExcepID[9] = IDC_CBO_INCH;
		ArrExcepID[10] = IDC_EDT_THICKNESS;
		ArrExcepID[11] = IDC_EDT_EDGE_EXCLUSION;
		ArrExcepID[12] = IDC_CHECK_MANUAL_MEASURE;
		ArrExcepID[13] = IDC_FPSPREAD_RESULT;
		ArrExcepID[14] = IDC_BUTTON_AUTO_FOCUS_VISION;
		break;

	case 3://자동측정중
		bEnable = FALSE;
		ArrExcepCnt = 8;
		ArrExcepID = new int[ArrExcepCnt];
		ArrExcepID[0] = IDC_BTN_INSP_STOP;
		ArrExcepID[1] = IDC_BTN_PAUSE;
		ArrExcepID[2] = IDC_BTN_INSP_START;
		ArrExcepID[3] = IDC_MEASURE_ITEM;
		ArrExcepID[4] = IDC_BTN_RESULT;
		ArrExcepID[5] = IDC_FPSPREAD_RESULT;
		ArrExcepID[6] = IDC_EDIT_WAFER_NUM;	
		ArrExcepID[7] = IDC_BUTTON_AUTO_FOCUS_VISION;
		break;

	default:
		EnableDlgItem(this, FALSE);//모든버튼 활성
		return;

	}

	EnableDlgItem(this, bEnable, ArrExcepID, ArrExcepCnt);
	delete[] ArrExcepID;

	return;
}

CAutoInspectorDlg::CAutoInspectorDlg(CWnd* pParent /*=NULL*/)
								: CDialogEx(CAutoInspectorDlg::IDD, pParent)
	, m_iRadioMeasureMode(-1)
{
	this->Create( IDD_AUTO_DIALOG );

	m_bShow             = FALSE;

	m_pTaskHandle       = NULL;
	m_pTask             = NULL;

	m_hExcelWriteThread  = NULL;
	m_bExcelWriteEnd     = FALSE;

	m_strOldLogMsg       = _T("");

	m_nStep              = -1;

	m_bOperation		 = FALSE;	
	m_bThTact            = FALSE;

	m_strTactTime        = _T("");
	m_strInspTime        = _T("");
	m_bInspProcess       = FALSE;

	m_nSwitchNo          = SW_INSP_END;
	m_dWaperThickness    = 1.0;
	m_dWaperExclusion    = 0.0;

	m_bPause             = FALSE;

	m_nSelLayer = 0;
	m_bPattAlignMode = FALSE;

	m_iSetPntMode = -1;
}

CAutoInspectorDlg::~CAutoInspectorDlg()
{
}

void CAutoInspectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CBO_MODEL_LIST, m_clModelList);
	DDX_Control(pDX, IDC_FPSPREAD_RESULT, m_clResult);
	DDX_Control(pDX, IDC_FPSPREAD_INFO, m_clInfo);
	DDX_Control(pDX, IDC_BTN_INSP_START, m_clInspStart);
	DDX_Control(pDX, IDC_BTN_INSP_STOP, m_clInspStop);
	DDX_Control(pDX, IDC_BTN_RESTART, m_clInspRestart);
	DDX_Control(pDX, IDC_BTN_PAUSE, m_clInspPause);
	DDX_Control(pDX, IDC_CBO_INCH, m_clWaperSize);
	DDX_Control(pDX, IDC_CBO_TYPE, m_clWaperType);
	DDX_Control(pDX, IDC_CBO_DANI, m_clDani);
	DDX_Control(pDX, IDC_MEASURE_ITEM, m_clMeasureItem);
	DDX_Control(pDX, IDC_CBO_METHOD, m_clMethod);
	DDX_Control(pDX, IDC_COMBO_RECEIPE, m_ctrlComboReceipe);
	DDX_Control(pDX, IDC_CHECK_AUTOMODE, m_ctrlCheckAutoMode);
	DDX_Control(pDX, IDC_EDIT_WAFER_NUM, m_ctrlEditWaferNum);
	DDX_Control(pDX, IDC_CHECK_PATT_MODE, m_ctrlPattAlignMode);
	DDX_Control(pDX, IDC_CBO_MODEL_LIST2, m_clModelList2);
	DDX_Control(pDX, IDC_CHECK_WAFER_ALIGN, m_ctrlCheckWaferAlign);
	DDX_Control(pDX, IDC_CHECK_AUTOMODE2, m_ctrlCheckAutoMode2);
	DDX_Control(pDX, IDC_CHECK_MANUAL_MEASURE, m_ctrlCheckManualMeasure);
	DDX_Control(pDX, IDC_CHECK_USE_AF, m_ctrlChkUseAF);
	DDX_Control(pDX, IDC_CBO_LAYER2, m_clLayer);
	DDX_Control(pDX, IDC_RADIO_MEASURE_MODE, m_ctrlRadioMeasureMode);
	DDX_Radio(pDX, IDC_RADIO_MEASURE_MODE, m_iRadioMeasureMode);
}

void CAutoInspectorDlg::m_fnSetShowFlag( BOOL bShow )
{
	m_bShow = bShow;
	if ( m_bShow )
	{
		this->ShowWindow( SW_SHOW );
	}
	else
	{
		this->ShowWindow( SW_HIDE );
	}
}

BOOL CAutoInspectorDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (( nID   == 2 ) &&
		( nCode == -1 ))
	{
		return FALSE;
	}
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CAutoInspectorDlg::PreTranslateMessage(MSG* pMsg) 
{
	// ENTER KEY or ESC KEY CANCEL
	if (( pMsg->wParam == 13 ) || 
		( pMsg->wParam == 27 ))
	{
		return FALSE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CAutoInspectorDlg::m_fnLogWrite( CString strMSG ) 
{
	CString strCnt("");
	int     nCnt		= 0;
	int     nTotal		= 0;

	if ( strMSG == m_strOldLogMsg )
	{
		return;
	}

	memset( m_szData, 0x00, sizeof( m_szData ));

	sprintf_s( m_szData, "%s", strMSG.GetString());
	if ( m_pTask == NULL ) return;
	if ( m_pTask->stCommon.bLogSave )
	{
		m_Log.DEBUG_LOG( m_szData );
	}
	m_strOldLogMsg = strMSG;

	if ( m_hWnd == NULL )
	{
		return;
	}
}

BEGIN_MESSAGE_MAP(CAutoInspectorDlg, CDialogEx)
	ON_MESSAGE(UM_CAMERA_CONNECT, m_fnSetCameraConnect)
	ON_MESSAGE(UM_START_BUTTON, m_fnSetStartButton)
	ON_MESSAGE(UM_REPORT, m_fnSetReport)//차트없고
	ON_MESSAGE(UM_REPORT_READY, m_fnSetReportReady)//차트만들고 UM_REPORT 콜
	ON_CBN_SELCHANGE(IDC_CBO_MODEL_LIST, &CAutoInspectorDlg::OnCbnSelchangeCboModelList)
	ON_BN_CLICKED(IDC_BTN_DEBUG, &CAutoInspectorDlg::OnBnClickedBtnDebug)
	ON_BN_CLICKED(IDC_BTN_INSP_START, &CAutoInspectorDlg::OnBnClickedBtnInspStart)
	ON_BN_CLICKED(IDC_BTN_INSP_STOP, &CAutoInspectorDlg::OnBnClickedBtnInspStop)
	ON_BN_CLICKED(IDC_BTN_RESTART, &CAutoInspectorDlg::OnBnClickedBtnRestart)
	ON_BN_CLICKED(IDC_BTN_PAUSE, &CAutoInspectorDlg::OnBnClickedBtnPause)
	ON_MESSAGE(UM_INSP_SWITCH, m_fnSetInspSwitch)
	ON_BN_CLICKED(IDC_BTN_PREVIEW, &CAutoInspectorDlg::OnBnClickedBtnPreview)
	ON_BN_CLICKED(IDC_BTN_USER_INFO, &CAutoInspectorDlg::OnBnClickedBtnUserInfo)
	ON_BN_CLICKED(IDC_BTN_REPORT, &CAutoInspectorDlg::OnBnClickedBtnReport)
	ON_BN_CLICKED(IDC_BTN_CW, &CAutoInspectorDlg::OnBnClickedBtnCw)
	ON_BN_CLICKED(IDC_BTN_CCW, &CAutoInspectorDlg::OnBnClickedBtnCcw)
	ON_BN_CLICKED(IDC_CHK_ALIGN, &CAutoInspectorDlg::OnBnClickedChkAlign)
	ON_CBN_SELCHANGE(IDC_CBO_INCH, &CAutoInspectorDlg::OnCbnSelchangeCboInch)
	ON_CBN_SELCHANGE(IDC_CBO_DANI, &CAutoInspectorDlg::OnCbnSelchangeCboDani)
	ON_CBN_SELCHANGE(IDC_CBO_TYPE, &CAutoInspectorDlg::OnCbnSelchangeCboType)
	ON_EN_CHANGE(IDC_EDT_THICKNESS, &CAutoInspectorDlg::OnEnChangeEdtThickness)
	ON_BN_CLICKED(IDC_BTN_RESET_DATA, &CAutoInspectorDlg::OnBnClickedBtnResetData)
	ON_BN_CLICKED(IDC_BTN_AUTO_MEASURE, &CAutoInspectorDlg::OnBnClickedBtnAutoMeasure)
	ON_BN_CLICKED(IDC_BTN_USER_INPUT, &CAutoInspectorDlg::OnBnClickedBtnUserInput)
	ON_BN_CLICKED(IDC_BTN_RESULT, &CAutoInspectorDlg::OnBnClickedBtnResult)
	ON_BN_CLICKED(IDC_BTN_POINT_4, &CAutoInspectorDlg::OnBnClickedBtnPoint4)
	ON_BN_CLICKED(IDC_BTN_POINT_12, &CAutoInspectorDlg::OnBnClickedBtnPoint12)
	ON_BN_CLICKED(IDC_BTN_POINT_28, &CAutoInspectorDlg::OnBnClickedBtnPoint28)
	ON_BN_CLICKED(IDC_BTN_POINT_60, &CAutoInspectorDlg::OnBnClickedBtnPoint60)
	ON_BN_CLICKED(IDC_BTN_POINT_124, &CAutoInspectorDlg::OnBnClickedBtnPoint124)
	ON_BN_CLICKED(IDC_BTN_POINT_252, &CAutoInspectorDlg::OnBnClickedBtnPoint252)
	ON_BN_CLICKED(IDC_BTN_POINT_508, &CAutoInspectorDlg::OnBnClickedBtnPoint508)
	ON_BN_CLICKED(IDC_BTN_CURR_XY, &CAutoInspectorDlg::OnBnClickedBtnCurrXy)
	ON_BN_CLICKED(IDC_BTN_SQUARE_ADD, &CAutoInspectorDlg::OnBnClickedBtnSquareAdd)
	ON_EN_CHANGE(IDC_EDT_EDGE_EXCLUSION, &CAutoInspectorDlg::OnEnChangeEdtEdgeExclusion)
	ON_BN_CLICKED(IDC_BTN_THICKNESS, &CAutoInspectorDlg::OnBnClickedBtnThickness)
	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(IDC_CBO_WAFER_NUM, &CAutoInspectorDlg::OnCbnSelchangeCboWaferNum)
	ON_CBN_SELCHANGE(IDC_MEASURE_ITEM, &CAutoInspectorDlg::OnCbnSelchangeMeasureItem)
	ON_BN_CLICKED(IDC_BTN_RECTANGLE_ADD, &CAutoInspectorDlg::OnBnClickedBtnRectangleAdd)
	ON_CBN_DROPDOWN(IDC_COMBO_RECEIPE, &CAutoInspectorDlg::OnCbnDropdownComboReceipe)
	ON_CBN_SELCHANGE(IDC_COMBO_RECEIPE, &CAutoInspectorDlg::OnCbnSelchangeComboReceipe)
	ON_BN_CLICKED(IDC_CHECK_AUTOMODE, &CAutoInspectorDlg::OnBnClickedCheckAutomode)
	ON_BN_CLICKED(IDC_CHECK_PATT_MODE, &CAutoInspectorDlg::OnBnClickedCheckPattMode)
	ON_CBN_SELCHANGE(IDC_CBO_MODEL_LIST2, &CAutoInspectorDlg::OnCbnSelchangeCboModelList2)
	ON_MESSAGE(UM_RUN_BTN_STATUS, m_fnSetChangeBtn)

	ON_BN_CLICKED(IDC_BTN_INIT_STAGE, &CAutoInspectorDlg::OnBnClickedBtnInitStage)
	ON_BN_CLICKED(IDC_BUTTON_STAGE_INIT, &CAutoInspectorDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BTN_POINT_5, &CAutoInspectorDlg::OnBnClickedBtnPoint5)
	ON_BN_CLICKED(IDC_CHECK_WAFER_ALIGN, &CAutoInspectorDlg::OnBnClickedCheckWaferAlign)
	ON_BN_CLICKED(IDC_CHECK_AUTOMODE2, &CAutoInspectorDlg::OnBnClickedCheckAutomode2)
	ON_BN_CLICKED(IDC_BUTTON_DATA_INIT, &CAutoInspectorDlg::OnBnClickedButtonDataInit)
	ON_BN_CLICKED(IDC_BUTTON_AUTO_FOCUS, &CAutoInspectorDlg::OnBnClickedButtonAutoFocus)
	ON_BN_CLICKED(IDC_BUTTON_MANUAL_MEASURE, &CAutoInspectorDlg::OnBnClickedButtonManualMeasure)
	ON_BN_CLICKED(IDC_CHECK_MANUAL_MEASURE, &CAutoInspectorDlg::OnBnClickedCheckManualMeasure)
	ON_CBN_SELCHANGE(IDC_CBO_LAYER2, &CAutoInspectorDlg::OnCbnSelchangeCboLayer2)
	ON_BN_CLICKED(IDC_CHECK_USE_AF, &CAutoInspectorDlg::OnBnClickedCheckUseAf)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_MEASURE_MODE, IDC_RADIO_MEASURE_MODE3, &CAutoInspectorDlg::OnBnClickedRadioMeasureMode)
	ON_BN_CLICKED(IDC_BUTTON_AUTO_FOCUS_VISION, &CAutoInspectorDlg::OnBnClickedButtonAutoFocusVision)
	ON_BN_CLICKED(IDC_BUTTON_RANDOM, &CAutoInspectorDlg::OnBnClickedButtonRandom)
END_MESSAGE_MAP()

void CAutoInspectorDlg::m_fnTECmsgOut( CString str, COLORREF color, int flag )
{
	TECmsgOut( m_Util.CStringTobstr(str), color, 1 );
}

LRESULT CAutoInspectorDlg::m_fnSetCameraConnect(  WPARAM nNo, LPARAM lParam )
{
	return 0;
}

LRESULT CAutoInspectorDlg::m_fnSetReportReady(WPARAM nNo, LPARAM lParam)
{
	if (m_hExcelWriteThread != NULL)
	{
		m_bExcelWriteEnd = FALSE;
		SuspendThread(m_hExcelWriteThread);
		CloseHandle(m_hExcelWriteThread);
		m_hExcelWriteThread = NULL;
	}

	unsigned long	ThreadID;
	m_bExcelWriteEnd = TRUE;
	m_hExcelWriteThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ExcelWriteThread, this, NULL, &ThreadID);
	m_nExcelWriteStep = 10;

	return 0;
}

LRESULT CAutoInspectorDlg::m_fnSetReport(  WPARAM nNo, LPARAM lParam )
{
#if MIL_USE
	pWorkRun->m_bThrdFlag[TF_MAKE_REPORT] = TRUE;

	t_MODEL_INFO  stReport;
	t_RESULT_INFO stData;
	CString strModel("");
	if ( !m_bFirstInsp ) 
	{
		pWorkRun->m_bThrdFlag[TF_MAKE_REPORT] = FALSE;
		return 1;
	}
	if ( m_pTask->stResult.nSRData == 0 ) 
	{
		pWorkRun->m_bThrdFlag[TF_MAKE_REPORT] = FALSE;
		return 1;
	}
	strModel.Format( "%s", m_pTask->stLastModel.szLastModel );
	strModel.Trim();
	if (strModel.IsEmpty())
	{
		strModel = "Empty";
	}

	memcpy( &stReport, &m_pTask->stResult, sizeof( t_MODEL_INFO ));

	t_USER_INFO stUser;
	m_fnSetUserInput(&stUser);
	memcpy( &stReport.stUser, &m_stUser, sizeof( t_USER_INFO ));

	memcpy(&stData, &m_pTask->stData, sizeof(t_RESULT_INFO));

	//XML 생성
	m_fnSetXMLResult( strModel, stReport, stData, SW_AUTO );

	//레포트 생성
	memcpy( &theApp.m_fnGetViewPointer()->m_stModel, &stReport, sizeof(t_MODEL_INFO));
	theApp.m_fnGetViewPointer()->m_fnSetXLSResult(SW_AUTO);

	pWorkRun->m_bThrdFlag[TF_MAKE_REPORT] = FALSE;
#endif
	return 0;
}

LRESULT CAutoInspectorDlg::m_fnSetStartButton(  WPARAM nNo, LPARAM lParam )
{
	if ( theApp.m_fnGetViewPointer( )->m_pTask->stLogin.nLevel == UNKNOWN_USER ) return 0;

	if ( m_bInspProcess ) return 0;

	CString strData("");
	if ( theApp.m_fnGetViewPointer()->m_nPage != SW_AUTO )
	{
		strData.Format( "자동측정 페이지에서만 자동으로 측정이 가능합니다." );
		m_bInspProcess = TRUE;
		TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 1 );
		m_bInspProcess = FALSE;
		return 0;
	}
	if ( m_bOperation ) 
	{
		if ( m_pTask->stEqp.bIoStartButton ) 
		{
			if ( m_nStep == STEP_HALT ) return 0;
			m_pTask->stEqp.bIoStopButton = true;
			strData.Format( "검사를 중단하시겠습니까?" );
			m_bInspProcess = TRUE;
			int nResult = TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 2 );
			m_bInspProcess = FALSE;
			if(nResult == 1)
			{
				m_fnLogWrite( "검사 중지 진행중..." );
				m_bPause    = FALSE;
				m_fnSetOperationStop();
			}
			return 0;
		}
		else
		{
			m_pTask->stEqp.bIoStopButton = false;
		}
	}

	if ( m_pTask->stResult.nSRData == 0 ) 
	{
		strData.Format( "검사할 포인트가 입력되어 있지 않습니다." );
		TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 1 );
		return 0;
	}

#if	ELLIPSO	
	thread thrdStartAuto = thread(ThrdRunning);
	thrdStartAuto.detach();
#endif
	return 0;
}

void CAutoInspectorDlg::m_fnSetInit()
{
	this->MoveWindow( 20, 100, 1050, 860, TRUE );

	m_Log.m_fnSetLogPath( "AutoInspector", "AutoInspector" );

	m_pTaskHandle = new CTaskHandle( this, SHARED_NAME, 
											1, 
											sizeof( SHEET_RESISTANCE_Format ) + 1,
											0,
											FALSE );


	m_pTask					= ( SHEET_RESISTANCE_Format* )m_pTaskHandle->m_pFilePointer( );

	m_fnLogWrite( "<<<============프로그램 시작=============>>>" );
	m_Log.m_fnSetLogWriteFlag( m_pTask->stCommon.bLogSave );

	m_fnGetModelList();

	m_clWaperSize.SetCurSel( WAPER_8 );
	m_clWaperType.SetCurSel( FLAT );

//	m_clDani.SetCurSel( VAL_R );

	m_nWaperSize		= WAPER_8;
	m_bWaperType		= (BOOL)FLAT;
//	m_nDani				= VAL_R;
	m_bAlign            = FALSE;

	m_fnSetChangeBtn( SW_INSP_END );
	GetDlgItem( IDC_EDT_THICKNESS )->SetWindowText( "700" );
	GetDlgItem( IDC_EDT_EDGE_EXCLUSION )->SetWindowText( "5" );
	for (int i = 0; i < 7; i++)
	{
		m_clMeasureItem.AddString(strMeasureItem[i]);
	}
	m_clMeasureItem.SetCurSel(1);
	m_nMeasureItem = m_clMeasureItem.GetCurSel();
	for (int i = 0; i < 4; i++)
	{
		m_clMethod.AddString(strMethod[i]);
	}

	//m_clResult.SetMaxRows(MAX_DATA);

	int nFind = m_clModelList.FindString( -1, m_pTask->stLastModel.szLastModel );
	if ( nFind != -1 )
	{
		m_clModelList.SetCurSel(nFind);
		OnCbnSelchangeCboModelList();
	}
	m_fnSetChangeBtn( SW_INSP_END );

	m_fWaferNumFont.CreateFont(65, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, 0, 0, 0, 0, 0, _T("굴림체"));
 	m_ctrlEditWaferNum.SetFont(&m_fWaferNumFont);
	m_ctrlEditWaferNum.SetWindowTextA("0");

	m_fAMFont.CreateFont(30, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, 0, 0, 0, 0, 0, _T("굴림체"));
	m_ctrlCheckAutoMode.SetFont(&m_fAMFont);
	m_ctrlCheckAutoMode.SetWindowTextA("자동투입");
	m_ctrlCheckAutoMode.SetCheck(TRUE);

	m_ctrlCheckAutoMode2.SetFont(&m_fAMFont);
	m_ctrlCheckAutoMode2.SetWindowTextA("자동측정");
	m_ctrlCheckAutoMode2.SetCheck(TRUE);



	m_fModeGroupFont.CreateFont(15, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, 0, 0, 0, 0, 0,NULL);
	GetDlgItem(IDC_RADIO_MEASURE_MODE)->SetFont(&m_fModeGroupFont);
	GetDlgItem(IDC_RADIO_MEASURE_MODE2)->SetFont(&m_fModeGroupFont);
	GetDlgItem(IDC_RADIO_MEASURE_MODE3)->SetFont(&m_fModeGroupFont);

	GetDlgItem(IDC_BUTTON_MANUAL_MEASURE)->SetFont(&m_fAMFont);
	GetDlgItem(IDC_BUTTON_DATA_INIT)->SetFont(&m_fModeGroupFont);
	GetDlgItem(IDC_BUTTON_AUTO_FOCUS)->SetFont(&m_fModeGroupFont);

	m_fButtonFont.CreateFont(20, 0, 0, 0, 0, FALSE, FALSE, 0, 0, 0, 0, 0, 0, NULL);
	GetDlgItem(IDC_BTN_INSP_START)->SetFont(&m_fButtonFont);
	GetDlgItem(IDC_BTN_PAUSE)->SetFont(&m_fButtonFont);
	GetDlgItem(IDC_BTN_INSP_STOP)->SetFont(&m_fButtonFont);
	GetDlgItem(IDC_BUTTON_STAGE_INIT)->SetFont(&m_fButtonFont);

	h_wndWaferNum = ::FindWindowEx(m_ctrlComboReceipe, NULL, _T("Edit"), NULL);
	if (NULL != h_wndWaferNum)
		((CEdit *)CWnd::FromHandle(h_wndWaferNum))->SetReadOnly(TRUE);

#if	ELLIPSO	
	if(pWorkRun != nullptr)
	{
		m_ctrlCheckAutoMode.SetCheck(pWorkRun->m_bAutoMode);
// 		m_ctrlCheckAutoMode2.SetCheck(pWorkRun->m_bAutoMode2);
	}

	m_clModelList2.EnableWindow(FALSE);
	m_ctrlCheckWaferAlign.SetCheck(TRUE);

	m_clLayer.SetCurSel(0);

	m_fnSetBtnEnable(100);//모든버튼 비활성
#endif
}

void CAutoInspectorDlg::m_fnSetEnd()
{
	OnBnClickedBtnInspStop();

	if ( m_pTaskHandle != NULL )
	{
		m_pTaskHandle->m_fnSetEnd();
		delete m_pTaskHandle;
		m_pTaskHandle = NULL;
	}
}

void CAutoInspectorDlg::m_fnGetModelList()
{
	CStringArray strFile;
	CString strPath("");
	CString strDirve("");
	CString strData("");

	strDirve = m_Util.m_fnGetCurrentDrive();
	strPath.Format( "%s%s%s", strDirve,
								DEFAULT_PATH,
								DEFAULT_MODEL_PATH );
	m_clModelList.ResetContent();
	m_clModelList2.ResetContent();

	m_Util.m_fnGetFileList( strPath, "*.xml", strFile );
	for( int i = 0; i < strFile.GetSize(); i++ )
	{
		strData = strFile.GetAt(i);

		if (m_bPattAlignMode)
		{
			if (strData.GetLength() > 7 && strData.Right(7) == "_Target")
			{
				m_clModelList.AddString(strData.Mid(0, strData.GetLength() - 4));
			}
			else if (strData.GetLength() > 4 && strData.Right(4) == "_Sub")
			{
				m_clModelList2.AddString(strData.Mid(0, strData.GetLength() - 4));
			}
		} 
		else
		{
			if ((strData.GetLength() > 7 && strData.Right(7) == "_Target")
				|| (strData.GetLength() > 4 && strData.Right(4) == "_Sub"))
				continue;
			
			m_clModelList.AddString( strData.Mid( 0, strData.GetLength() - 4) );
		}
	}

	int nNo = m_clModelList.FindString( 0, m_pTask->stLastModel.szLastModel );
	if ( nNo != -1 )
	{
		m_clModelList.SetCurSel( nNo );
	}
}

void CAutoInspectorDlg::OnCbnSelchangeCboModelList()
{
#if !_DEBUG
	if ( m_pTask->stLogin.nLevel == UNKNOWN_USER )
	{
		return;
	}
#endif
// 	if ( m_pTask->stEqp.bIoStartButton )
#if MIL_USE
	if(pWorkRun != NULL && pWorkRun->m_bThrdFlag[TF_AUTORUN])
	{
		m_fnLogWrite("Start 버튼이 ON시에는 측정 관리 메뉴를 사용할수 없습니다.");
		return;
	}

	m_bFirstInsp = FALSE;

	CString strModel("");
	CString strData( "" );
	CString strMSG("");
	int nDataCnt = 0;
	int nRow     = 0;
	int nDataTotal = 0;
	int nNo = m_clModelList.GetCurSel();
	if (nNo == -1)
	{
		AfxMessageBox("모델을 선택해 주세요.");
		return;
	}

	m_ctrlPattAlignMode.SetCheck(FALSE);
	m_clModelList2.SetCurSel(-1);
	m_clMethod.SetCurSel(-1);

	m_iSetPntMode = PNT_MODEL;
// 	m_iRadioMeasureMode = 2;


	m_fnSRSheetClear();

	m_clModelList.GetLBText(nNo, strModel);
	strModel.Trim();
	if (!strModel.IsEmpty())
	{
		theApp.m_fnGetViewPointer()->m_pModel->m_fnGetCurrModel_Load(strModel);

		if (m_bPattAlignMode)
			memcpy(&m_pTask->stTarget, &m_pTask->stCurrModel, sizeof(t_MODEL_INFO));
		else
			memcpy(&m_pTask->stResult, &m_pTask->stCurrModel, sizeof(t_MODEL_INFO));

		memcpy(&m_pTask->stModel, &m_pTask->stCurrModel, sizeof(t_MODEL_INFO));

	}

	m_clWaperSize.SetCurSel(m_pTask->stModel.nWaperSize);
	if (theApp.m_fnGetViewPointer()->m_pStageMap != NULL)
	{
		theApp.m_fnGetViewPointer()->m_pMap->m_nWaperSize = m_pTask->stModel.nWaperSize;
	}
	// 		m_clDani.SetCurSel(m_pTask->stModel.nOutputDisp);

	m_dWaperThickness = m_pTask->stResult.dWaperThickness;
	strData.Format("%7.3f", m_pTask->stResult.dWaperThickness);
	GetDlgItem(IDC_EDT_THICKNESS)->SetWindowText(strData);

	m_dWaperExclusion = m_pTask->stResult.dEdgeExclusion;
	strData.Format("%7.3f", m_pTask->stResult.dEdgeExclusion);
	GetDlgItem(IDC_EDT_EDGE_EXCLUSION)->SetWindowText(strData);

	if(pWorkRun != NULL && !pWorkRun->m_bThrdFlag[TF_MODELSET])
	{
		if (m_bPattAlignMode)
		{
			m_fnSetTargetSubCalcPoint();
		}
	}

	nDataCnt = m_pTask->stResult.nSRData;
	if (nDataCnt > MAX_DATA)
	{
		strMSG.Format("최대 검사 사이즈보다 높습니다. MAX 값으로 재설정합니다. ( %01d )", nDataCnt);
		nDataCnt = MAX_DATA;
		m_fnLogWrite(strMSG);
	}

	InsertSheet(nDataCnt);
#endif
}

void CAutoInspectorDlg::InsertSheet(int nDataCnt)
{
	int nRow=0;
	CString strMSG;
	CString strData;
	CString strModel;

	for (int i = 0; i < nDataCnt; i++)
	{
		if (nRow > MAX_DATA)
		{
			strMSG.Format("데이터가 MAX 처리수를 초과하였습니다. MAX 수 이상은 무시됩니다. MAX = %01d", MAX_DATA);
			m_fnTECmsgOut(strMSG, RED_1, 1);
		}

		m_clResult.SetRow(nRow + 1);

		m_clResult.SetCol(RESULT_TARGET_NO);
		strData.Format("%s", m_pTask->stResult.stSRData[i].szName);
		m_clResult.SetText(strData);

		m_clResult.SetCol(RESULT_COL);
		strData.Format("%d", m_pTask->stResult.stSRData[i].iColum);
		m_clResult.SetText(strData);

		m_clResult.SetCol(RESULT_ROW);
		strData.Format("%d", m_pTask->stResult.stSRData[i].iRow);
		m_clResult.SetText(strData);

		m_clResult.SetCol(RESULT_SUB);
		strData.Format("%d", m_pTask->stResult.stSRData[i].iSub);
		m_clResult.SetText(strData);

		m_clResult.SetCol(RESULT_X1_X2);
		strData.Format("%7.3f", m_pTask->stResult.stSRData[i].dwXPosition);
		m_clResult.SetText(strData);

		m_clResult.SetCol(RESULT_Y1_Y2);
		strData.Format("%7.3f", m_pTask->stResult.stSRData[i].dwYPosition);
		m_clResult.SetText(strData);

		nRow++;
	}

	sprintf_s(m_pTask->stLastModel.szLastModel, "%s", strModel.GetString());
	theApp.m_fnGetViewPointer()->m_pCommon->m_fnSetLastModel_Save();


	memset(&m_pTask->stData, 0x00, sizeof(m_pTask->stData));
	memset(m_pTask->stData.m_dDataX, 0x00, sizeof(m_pTask->stData.m_dDataX));
	memset(m_pTask->stData.m_dDataY, 0x00, sizeof(m_pTask->stData.m_dDataY));
	memset(m_pTask->stData.m_dDataZ, 0x00, sizeof(m_pTask->stData.m_dDataZ));
	memset(m_pTask->stData.m_dDataH, 0x00, sizeof(m_pTask->stData.m_dDataH));
	memset(m_pTask->stData.m_dDataL, 0x00, sizeof(m_pTask->stData.m_dDataL));
	memset(m_pTask->stData.m_dMax, 0x00, sizeof(m_pTask->stData.m_dMax));
	memset(m_pTask->stData.m_dMin, 0x00, sizeof(m_pTask->stData.m_dMin));
	memset(m_pTask->stData.m_dStd, 0x00, sizeof(m_pTask->stData.m_dStd));
	memset(m_pTask->stData.m_dMax_Min, 0x00, sizeof(m_pTask->stData.m_dMax_Min));
	memset(m_pTask->stData.m_dUniformity, 0x00, sizeof(m_pTask->stData.m_dUniformity));
	memset(m_pTask->stData.m_dAvg, 0x00, sizeof(m_pTask->stData.m_dAvg));
	memset(m_pTask->stData.m_fTot, 0x00, sizeof(m_pTask->stData.m_fTot));
	m_pTask->stData.m_nCurrCnt = 0;
	theApp.m_fnGetViewPointer()->m_pContour->m_fnSetUpdateData();
	m_fnSetInspStatus(m_pTask->stData.m_dMax[m_nSelLayer][m_nMeasureItem],
		m_pTask->stData.m_dMin[m_nSelLayer][m_nMeasureItem],
		m_pTask->stData.m_dAvg[m_nSelLayer][m_nMeasureItem],
		m_pTask->stData.m_dStd[m_nSelLayer][m_nMeasureItem],
		m_pTask->stData.m_dMax_Min[m_nSelLayer][m_nMeasureItem],
		m_pTask->stData.m_dUniformity[m_nSelLayer][m_nMeasureItem]);

	return;
}

void CAutoInspectorDlg::m_fnSetOperationStop()
{
	m_bThTact	= FALSE;
	m_nStep		= INSP_STOP_POS;
	m_pTask->stEqp.bIoStartButton = FALSE;
	m_pTask->stEqp.bIoStopButton  = TRUE;

	this->PostMessageA( UM_CAMERA_CONNECT, 0, 0 );
	m_bOperation				  = FALSE;
}

void CAutoInspectorDlg::m_fnSetThTactTime()
{
	m_clInfo.SetRow(1);
	m_clInfo.SetCol(2);
	if (( m_bThTact ) &&
	    ( m_nStep > INSP_DATA_CLEAR ))
	{
		m_strTactTime = m_Util.m_fnGetTackTime( (double)CThTact.TimeCheck());
		m_clInfo.SetText( m_strTactTime );
	}
}

void CAutoInspectorDlg::m_fnSetDebugThTactTime()
{
	m_clInfo.SetRow(1);
	m_clInfo.SetCol(2);
	if ( m_bThTact )
	{
		m_strTactTime = m_Util.m_fnGetTackTime( (double)CThTact.TimeCheck());
		m_clInfo.SetText( m_strTactTime );
	}
}

void CAutoInspectorDlg::m_fnSetInspStatus( double dMax, double dMin, double dAvg, double dStd, double dMax_Min, double dUniformity)
{
	CString strData("");


	m_clInfo.SetRow( 1 );

	m_clInfo.SetCol(2);
	m_clInfo.SetText(m_pTask->stResult.szInspTackTime);

	strData.Format( "%7.3f", dMax );
	m_clInfo.SetCol( 4 );
	m_clInfo.SetText( strData );

	strData.Format( "%7.3f", dMin );
	m_clInfo.SetCol( 6 );
	m_clInfo.SetText( strData );

	strData.Format( "%7.3f", dMax_Min );
	m_clInfo.SetCol( 8 );
	m_clInfo.SetText( strData );

	m_clInfo.SetRow( 2 );

	strData.Format( "%7.3f", dAvg );
	m_clInfo.SetCol( 2 );
	m_clInfo.SetText( strData );

	strData.Format( "%7.3f", dUniformity );
	m_clInfo.SetCol( 4 );
	m_clInfo.SetText( strData );

	strData.Format( "%7.3f", dStd );
	m_clInfo.SetCol( 6 );
	m_clInfo.SetText( strData );
}

//------------------------------------------------------------------------
// EFU 파일의 경로를 읽어온다.
//------------------------------------------------------------------------
CString CAutoInspectorDlg::m_fnGetStandardResultFileFullPath( )
{
	char szFullPath[MAX_PATH + 1];
	memset( szFullPath, 0x00, sizeof( szFullPath ));

	sprintf_s(szFullPath, "%s%s%s%s", m_Util.m_fnGetCurrentDrive().GetString(),
									DEFAULT_PATH,
									DEFAULT_ENV_PATH,
									DEFAULT_MODEL_FILE );
	szFullPath[MAX_PATH] = NULL;

	return szFullPath;
}
//------------------------------------------------------------------------
// EFU 파일의 경로를 읽어온다.
//------------------------------------------------------------------------
CString CAutoInspectorDlg::m_fnGetStandardExcelFileFullPath( )
{
	char szFullPath[MAX_PATH + 1];
	memset( szFullPath, 0x00, sizeof( szFullPath ));

	sprintf_s( szFullPath, "%s%s%s%s", m_Util.m_fnGetCurrentDrive().GetString(),
										DEFAULT_PATH,
										DEFAULT_ENV_PATH,
										DEFAULT_EXCEL_FILENAME );
	szFullPath[MAX_PATH] = NULL;

	return szFullPath;
}
//------------------------------------------------------------------------
// 파일의 경로를 읽어온다.
//------------------------------------------------------------------------
CString CAutoInspectorDlg::m_fnGetTempPath( )
{
	CString strMemo("");
	char szFullPath[MAX_PATH + 1];
	memset( szFullPath, 0x00, sizeof( szFullPath ));

	sprintf_s( szFullPath, "%s%s%s", m_Util.m_fnGetCurrentDrive().GetString(), DEFAULT_PATH, DEFAULT_TEMP_PATH);
	szFullPath[MAX_PATH] = NULL;
	m_Util.m_fnSetCreateDir( szFullPath );

	szFullPath[MAX_PATH] = NULL;
	return szFullPath;
}

//------------------------------------------------------------------------
// 파일의 경로를 읽어온다.
//------------------------------------------------------------------------
CString CAutoInspectorDlg::m_fnGetMXLPath( CString strDate, CString strTime, CString strLotID )
{
	CString strMemo("");
	char szFullPath[MAX_PATH + 1];
	memset(szFullPath, 0x00, sizeof(szFullPath));

	sprintf_s(szFullPath, "%s%s\\%s_%s\\", m_pTask->stCommon.szXmlResultPath, strDate.GetString(), strTime.GetString(), strLotID.GetString());
	szFullPath[MAX_PATH] = NULL;
	m_Util.m_fnSetCreateDir(szFullPath);

	szFullPath[MAX_PATH] = NULL;
	return szFullPath;
}

void CAutoInspectorDlg::m_fnSetXMLResult( CString strModel, t_MODEL_INFO &stReport, t_RESULT_INFO stData, int nMode  )
{
	CString strWork("");
	CString strData("");
	CString strFullPath("");
	CString strFileName("");
	CString strBackup("");
	CString strLotID("");
	CString strLastModel("");
	CString strContect("");
	CString strDateTime("");
	CString strDate("");
	CString strTactTime("");
	CString strLayer("");
	CString strValue("");
	CString strSlotNo("");
	CString strLotStartTime("");
	CString strWaperNo("");


	int     nChart       = theApp.m_fnGetViewPointer()->m_nChart;
	int     nMeasureItem = m_nMeasureItem;
	int     nSelLayer = m_nSelLayer;
	
	switch (nMode)
	{
		case SW_AUTO:
		{
			strDateTime		= m_strInspTime;
			strDate			= m_strInspDate;
			strTactTime		= m_strTactTime;
			strLotID		= m_stUser.szLotID;
			strLotStartTime = m_strLotStartTime;
			strLastModel.Format( "%s", strModel );
			m_ctrlEditWaferNum.GetWindowTextA(strWaperNo);
			strSlotNo.Format("%02d", atoi(strWaperNo));

			stReport.stUser.iMeasureMode = m_iSetPntMode;
			sprintf_s(stReport.stUser.szRecipeFileName, "%s", pWorkRun->m_strRecipeName.GetString());
			sprintf_s(stReport.stUser.szDataFile, "%s", m_stUser.szDataFile);
		}
		break;
		case SW_REVIEW:
		{
			strDateTime.Format( "%s", stReport.szInspDateTime );
			strDate.Format("%s", stReport.stUser.szDate );
			strTactTime.Format("%s", stReport.szInspTackTime );
			strLotID.Format( "%s", stReport.stUser.szLotID );
			strLotStartTime.Format( "%s", stReport.szLotStartTime );
			strLastModel.Format( "%s", stReport.szInspModel );
			strSlotNo.Format("%02d", stReport.nSlotNo);
		}
		break;
		case 444://패턴매칭실패
			strDateTime = m_strInspTime;
			strDate = m_strInspDate;
			strTactTime = m_strTactTime;
			strLotID = m_stUser.szLotID;
			strLotStartTime = m_strLotStartTime;
			strLastModel.Format("PatternMapping-Failed");
			m_ctrlEditWaferNum.GetWindowTextA(strWaperNo);
			strSlotNo.Format("%02d", atoi(strWaperNo));

			stReport.stUser.iMeasureMode = PNT_MODEL;
			sprintf_s(stReport.stUser.szRecipeFileName, "%s", pWorkRun->m_strRecipeName.GetString());
			sprintf_s(stReport.stUser.szDataFile, "%s", m_stUser.szDataFile);
			break;
		default:
		{
			TECmsgOut(L"XML 생성화면에서만 작업이 가능합니다.", RED_1, 1);
			return;
		}
		break;
	}

	if ((strLotStartTime.IsEmpty()) ||
		(strDate.IsEmpty()))
	{	
		strWork.Format("LotStartTime(%s) 또는 Date(%s)에 데이터가 없어서 XML 생성불가입니다.", strLotStartTime, strDate);
		TECmsgOut(m_Util.CStringTobstr(strWork), RED_1, 1);
		return;
	}

	stReport.stUser.iSlotNo = stReport.nSlotNo = atoi(strSlotNo);
	sprintf_s(stReport.szLotStartTime, "%s", strLotStartTime.GetString());

	CStringArray strLst;
	CXmlElement *pElement     = NULL;
	CXmlElement *pRootElement = NULL;

	strWork = "Result Data XML 저장 시작";
	m_fnLogWrite( strWork ); 
	
	strFullPath           =  m_fnGetMXLPath( strDate, strLotStartTime, strLotID );
	m_fnLogWrite(strFullPath);

	strFileName           = strSlotNo;
	strFileName           +=  "_";
	switch (stReport.stUser.iMeasureMode)
	{
	case PNT_CIRCLE:
		strFileName += "Circle";
		break;
	case PNT_RECT:
		strFileName += "Rectangle";
		break;
	case PNT_MODEL:
		strFileName += strLastModel;
		break;
	case PNT_PATT:
		strFileName += "Pattern";
		break;
	default:
		strFileName += "Manual";
		break;
	}
	strFileName += "_";
	strFileName += pWorkRun->m_strRecipeName;
	strFileName += ".XML";

	TECmsgOut(L"XML 생성을 시작합니다.", GREEN_1, 1000);
	strWork.Format( "RESULT XML DATA : %s", strFullPath );
	m_fnLogWrite( strWork ); 

	CXmlDocument* pXML = new CXmlDocument();
	pXML->Load( m_fnGetStandardResultFileFullPath() );

	pXML->GetTagToString( MODEL , strData );
	pXML->Parse( strData );
	pRootElement = pXML->GetRootElement();
	if ( pRootElement == NULL )
	{
		delete pXML;
		pXML = NULL;
		m_fnLogWrite( "pRootElement is Null" );
		return;
	}

	strLst.RemoveAll();
	strLst.Add(ELEMENT);
	pElement = pXML->FindElement( pRootElement, strLst );
	if ( pElement != NULL )
	{
		if ( m_pTask->stCommon.bExcelSave )
		{
			pXML->DataSet( pElement, EXCEL_REPORT, STR_YES );
		}
		else
		{
			pXML->DataSet( pElement, EXCEL_REPORT, STR_NO );
		}
		if ( stReport.bUseAlign )
		{
			pXML->DataSet( pElement, ALIGN_USE, STR_YES );
		}
		else
		{
			pXML->DataSet( pElement, ALIGN_USE, STR_NO );
		}
		if ( stReport.bFlat )
		{
			pXML->DataSet( pElement, WAPER_TYPE, STR_YES );
		}
		else
		{
			pXML->DataSet( pElement, WAPER_TYPE, STR_NO );
		}
		if ( m_pTask->stCommon.bChartReport )
		{
			pXML->DataSet( pElement, CHART_INCLUDE, STR_YES );
		}
		else
		{
			pXML->DataSet( pElement, CHART_INCLUDE, STR_NO );
		}
		stReport.bChartReport = m_pTask->stCommon.bChartReport;

		strWork.Format( "%7.3f", m_dWaperThickness );
		pXML->DataSet( pElement, WAPER_THICKNESS, strWork );
		stReport.dWaperThickness = m_dWaperThickness;

		strWork.Format( "%01d", m_nMeasureItem);
		pXML->DataSet( pElement, OUTPUT_DISP, strWork );
		stReport.nOutputDisp = m_nMeasureItem;

		strWork.Format( "%7.3f", stReport.dAlignSafetyZ );
		pXML->DataSet( pElement, ALIGN_Z_SAFETY_AXIS, strWork );

		strWork.Format( "%7.3f", stReport.dSRSafetyZ );
		pXML->DataSet( pElement, SR_Z_SAFETY_AXIS, strWork );

		strWork.Format( "%7.3f", stReport.dSRSpeed );
		pXML->DataSet( pElement, SR_SPEED, strWork );

		strWork.Format( "%7.3f", stReport.dAlignZ );
		pXML->DataSet( pElement, ALIGN_Z, strWork );
		strWork.Format( "%7.3f", stReport.dSRZ );
		pXML->DataSet( pElement, SR_Z, strWork );

		strWork.Format( "%7.3f", stReport.dAlignX );
		pXML->DataSet( pElement, ALIGN_X, strWork );
		strWork.Format( "%7.3f", stReport.dAlignY );
		pXML->DataSet( pElement, ALIGN_Y, strWork );
		if (( stReport.nWaperSize >= WAPER_4 ) &&
			( stReport.nWaperSize <= WAPER_12 ))
		{
			strWork.Format( "%01d", m_nWaperSize );
			stReport.nWaperSize = m_nWaperSize;
		}
		else
		{
			strWork.Format( "%01d", WAPER_4 );
			stReport.nWaperSize = WAPER_4;
		}
		pXML->DataSet( pElement, WAPER_SIZE, strWork );

		strWork.Format( "%01d", m_pTask->stCommon.nInspLight );
		pXML->DataSet( pElement, LIGHT_VALUE, strWork );
		stReport.nLight = m_pTask->stCommon.nInspLight;

		for (int i = 0; i < stReport.nLayer; i++)
		{
			for (int j = 0; j < MAX_ITEM; j++)
			{
				stReport.dContourMax[i][j] = stData.m_dContourMax[i][j];
				stReport.dContourMin[i][j] = stData.m_dContourMin[i][j];
				stReport.dContourStep[i][j] = stData.m_dContourStep[i][j];
			}
		}
	}

	strLst.RemoveAll();
	strLst.Add(USER_INPUT);
	pElement = pXML->FindElement( pRootElement, strLst );
	if ( pElement != NULL )
	{
		strData.Format("%s", stReport.stUser.szLotID);
		pXML->DataSet(pElement, LOTID, strData);
		strData.Format("%7.3f", stReport.stUser.dSampleSize);
		pXML->DataSet(pElement, SAMPLE_SIZE, strData);
		strData.Format("%7.3f", stReport.stUser.dThickness);
		pXML->DataSet(pElement, THICKNESS, strData);
		strData.Format("%7.3f", stReport.stUser.dExclusion);
		pXML->DataSet(pElement, EXCLUSION, strData);
		strData.Format("%s", stReport.stUser.szRecipeFileName);
		pXML->DataSet(pElement, RECIPE, strData);
		strData.Format("%s", stReport.stUser.szInsertMode);
		pXML->DataSet(pElement, INSERTMODE, strData);
		strData.Format("%s", stReport.stUser.szWaferAlignMode);
		pXML->DataSet(pElement, WAFERALIGNMODE, strData);
		strData.Format("%s", stReport.stUser.szVisionAFMode);
		pXML->DataSet(pElement, VISIONAFMODE, strData);
		strData.Format("%s", stReport.stUser.szDate);
		pXML->DataSet(pElement, USER_DATE, strData);
		strData.Format("%d", stReport.stUser.iSlotNo);
		pXML->DataSet(pElement, SLOT_NO, strData);
		strData.Format("%s", stReport.stUser.szOperatorID);
		pXML->DataSet(pElement, OPID, strData);
		strData.Format("%s", stReport.stUser.szTimeStart);
		pXML->DataSet(pElement, TIMESTART, strData);
		strData.Format("%s", stReport.stUser.szTimeEnd);
		pXML->DataSet(pElement, TIMEEND, strData);
		strData.Format("%s", stReport.stUser.szTimeSpend);
		pXML->DataSet(pElement, TIMESPEND, strData);
		strData.Format("%s", stReport.stUser.szDataFile);
		pXML->DataSet(pElement, DATA_FILE, strData);
		strData.Format("%s", stReport.stUser.szDescription);
		pXML->DataSet(pElement, DESCRIPTION, strData);
		strData.Format("%d", stReport.stUser.iMeasureMode);
		pXML->DataSet(pElement, MEASUREAREATYPE, strData);

// 		strWork.Format( "%s", stReport.stUser.szLotID );
// 		pXML->DataSet( pElement, LOTID, strWork );
// 		strWork.Format( "%7.3f", stReport.stUser.dSampleSize );
// 		pXML->DataSet( pElement, SAMPLE_SIZE, strWork );
// 		strWork.Format( "%7.3f", stReport.stUser.dThickness );
// 		pXML->DataSet( pElement, THICKNESS, strWork );
// 		strWork.Format( "%s", stReport.stUser.szOperatorID );
// 		pXML->DataSet( pElement, OPID, strWork );
// 		strWork.Format( "%s", stReport.stUser.szDescription );
// 		pXML->DataSet( pElement, DESCRIPTION, strWork );
// 		strWork.Format( "%7.3f", stReport.stUser.dFlat );
// 		pXML->DataSet( pElement, USER_FLAT, strWork );
// 		strWork.Format( "%s", stReport.stUser.szMaterial );
// 		pXML->DataSet( pElement, METERIAL, strWork );
// 		strWork.Format( "%7.3f", stReport.stUser.dExclusion );
// 		pXML->DataSet( pElement, EXCLUSION, strWork );
// 		strWork.Format( "%s", stReport.stUser.szThinfilmmaterial );
// 		pXML->DataSet( pElement, THIN_METERIAL, strWork );
// 		strWork.Format( "%7.3f", stReport.stUser.dThinfilmthickness );
// 		pXML->DataSet( pElement, THIN_THICKNESS, strWork );
// 		strWork.Format( "%s", stReport.stUser.szDate );
// 		pXML->DataSet( pElement, USER_DATE, strWork );
// 		strWork.Format( "%s", stReport.stUser.szTime );
// 		pXML->DataSet( pElement, USER_TIME, strWork );
	}
	strLst.RemoveAll();
	strLst.Add(INSPECT_RESULT);
	pElement = pXML->FindElement( pRootElement, strLst );
	if ( pElement != NULL )
	{
		for (int i = 0; i < stReport.nLayer; i++)
		{
			for (int j = 0; j < MAX_ITEM; j++)
			{
				stReport.dInspMin[i][j] = stData.m_dMin[i][j];
				stReport.dInspMax[i][j] = stData.m_dMax[i][j];
				stReport.dInspAvg[i][j] = stData.m_dAvg[i][j];
				stReport.dInspStd[i][j] = stData.m_dStd[i][j];
				stReport.dInspMax_Min[i][j] = stData.m_dMax_Min[i][j];
				stReport.dInspUniformity[i][j] = stData.m_dUniformity[i][j];
			}
		}
		strModel.Format("%s", stReport.szInspModel);
		pXML->DataSet( pElement, INSP_MODEL, strModel );

		strDateTime.Format("%s", stReport.szInspDateTime);
		pXML->DataSet( pElement, INSP_DATE_TIME, strDateTime );

		strDate.Format("%s", stReport.szInspDate);
		pXML->DataSet( pElement, INSP_DATE, strDate );

		strTactTime.Format("%s", stReport.szInspTackTime);
		pXML->DataSet( pElement, TACT_TIME, strTactTime );

		strLotStartTime.Format("%s", stReport.szLotStartTime);
		pXML->DataSet(pElement, LOT_START_TIME, strLotStartTime);
	}

	strLst.RemoveAll();
	strLst.Add(BODY);
	strLst.Add(SR_LIST);
	pElement = pXML->FindElement( pRootElement, strLst );
	if ( pElement != NULL )
	{
		for ( int i = 0; i < stReport.nSRData; i++ )
		{
			strWork.Format( "%s", stReport.stSRData[i].szName );
			strWork.Trim();
			if ( !strWork.IsEmpty() )
			{
				CXmlElement* pElement1 = pXML->AddElement( pElement, ITEM );
				pXML->AddElement( pElement1, SR_NAME, stReport.stSRData[i].szName );
				strWork.Format("%01d", stReport.stSRData[i].iRow);
				pXML->AddElement(pElement1, SR_ROW, strWork);
				strWork.Format("%01d", stReport.stSRData[i].iColum);
				pXML->AddElement(pElement1, SR_COL, strWork);
				strWork.Format("%01d", stReport.stSRData[i].iSub);
				pXML->AddElement(pElement1, SR_SUB, strWork);
				strWork.Format( "%7.3f", stReport.stSRData[i].dwXPosition );
				pXML->AddElement( pElement1, SR_X, strWork );
				strWork.Format( "%7.3f", stReport.stSRData[i].dwYPosition );
				pXML->AddElement( pElement1, SR_Y, strWork );
				strWork.Format( "%7.3f", stReport.stSRData[i].dwZPosition );
				pXML->AddElement( pElement1, SR_Z, strWork );
			}
		}
	}
	strLst.RemoveAll();
	strLst.Add(BODY);
	strLst.Add(SR_DATA);
	pElement = pXML->FindElement(pRootElement, strLst);
	if (pElement != NULL)
	{
		for (int i = 0; i < stReport.nSRData; i++)
		{
			CXmlElement* pElement1 = pXML->AddElement(pElement, ITEM);
			for (int j = 0; j < stReport.nLayer; j++)
			{
				for (int k = 0; k < MAX_ITEM; k++)
				{
					strValue.Format(SR_V, j + 1, k + 1);
					strWork.Format("%7.3f", stReport.stSRData[i].dwResult[j][k]);
					pXML->AddElement(pElement1, strValue, strWork);
				}
			}
		}
	}

	strLst.RemoveAll();
	strLst.Add(BODY);
	strLst.Add(SR_TOTAL_CNT);
	pElement = pXML->FindElement(pRootElement, strLst);
	if (pElement != NULL)
	{
		strWork.Format( "%01d", stReport.nSRData );
		pXML->DataSet(pElement, DATA_CNT, strWork);
		strWork.Format("%01d", stReport.nLayer);
		pXML->DataSet(pElement, LAYER_CNT, strWork);
		strWork.Format("%01d", stReport.nSlotNo);
		pXML->DataSet(pElement, SLOT_NO, strWork);
	}

	if ( nMode == SW_AUTO )
	{
		pXML->Store( strFullPath + strFileName );
		m_fnLogWrite( strFullPath + strFileName );
		if (m_Util.Existence(strFullPath + strFileName) != FM_FILE)
		{
			strWork.Format("( %s%s ) XML 파일 생성 실패입니다.", strFullPath, strFileName);
			m_fnLogWrite(strWork);
			TECmsgOut(m_Util.CStringTobstr(strWork), RED_1, 1);
		}
	}

	delete pXML;
	pXML = NULL;

	TECmsgOut(L"XML 생성을 종료합니다.", GREEN_1, 1000);
	strWork = "Result XML Data 저장 완료";
	m_fnLogWrite( strWork );
}

BEGIN_EVENTSINK_MAP(CAutoInspectorDlg, CDialogEx)
	ON_EVENT(CAutoInspectorDlg, IDC_FPSPREAD_RESULT, 5, CAutoInspectorDlg::ClickFpspreadResult, VTS_I4 VTS_I4)
END_EVENTSINK_MAP()

void CAutoInspectorDlg::ClickFpspreadResult(long Col, long Row)
{
#if MIL_USE
	if ( pWorkRun->m_bThrdFlag[TF_AUTORUN])
	{
		m_fnLogWrite("자동측정시에는 사용할수 없습니다.");
		return;
	}

	m_pTask->stData.m_nCurrCnt = Row-1;

	if (( Col != RESULT_MEASURE ) &&
		( Col != RESULT_MOVE ))
	{
		return;
	}

	if ( !theApp.m_fnGetViewPointer( )->m_pAjin->m_bInitEnd )
	{
		CString strMsg = "장비를 초기화후 진행해 주세요";
		m_fnTECmsgOut( strMsg, GREEN_1, 1 );
		m_fnLogWrite( strMsg );
		return;
	}
#endif
#if AJINTEX_BOARD
	CString strResult("");
	BOOL bFnRes = TRUE;
	CDPoint dPntTargetPos;
	t_ELIPSO_RESULT_INFO stcResultAlign;

	if ( Col == RESULT_MOVE )
	{
		if (pWorkRun->m_bAutoMode)
			return;

		m_clResult.SetRow( Row );
		m_clResult.SetCol(RESULT_TARGET_NO);
		strResult = m_clResult.GetText( );
		strResult.Trim();
		if ( strResult.IsEmpty() ) 
		{
			// TECmsgOut( L"측정범위를 벋어났습니다.", GREEN_1, 1 );
			m_fnLogWrite( "측정범위를 벋어났습니다." ); 
			return;
		}

#if AJINTEX_BOARD
		double dwPosition = 0.0;
		if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( Z_AXIS, dwPosition ) == RC_NORMAL )
		{
			if ( theApp.m_fnGetViewPointer()->m_pModel->m_fnGetMeasureSafetyZ( theApp.m_fnGetViewPointer()->m_pAuto->m_dWaperThickness ) > theApp.m_fnGetViewPointer()->m_pTask->stCommon.dZLimit )
			{
				TECmsgOut( L"Z축 Limit 위치보다 Z측정 안정위치가 낮아서 Stage와 충돌할수 있어서 이동불가 합니다.", RED_1, 1 );
				return;
			}
		}
#endif
		CString strDataX("");
		CString strDataY("");

		m_clResult.SetRow( Row );
		m_clResult.SetCol( RESULT_X1_X2 );
		strDataX = m_clResult.GetText();
		m_clResult.SetCol( RESULT_Y1_Y2 );
		strDataY = m_clResult.GetText();
		m_dPositionX = atof( strDataX );
		m_dPositionY = atof( strDataY );

#if MIL_USE
		dPntTargetPos.x = m_dPositionX;
		dPntTargetPos.y = m_dPositionY;
		pWorkRun->m_bThrdFlag[TF_AUTORUN] = TRUE;
		bFnRes = pWorkRun->SqncTargetMove(dPntTargetPos);
		if (!bFnRes)
		{
			TECmsgOut(L"측정위치""이동""을 완료하지 못했습니다. 3초 후 다음위치로 진행합니다", LIGHTMAGENTA, 3000);
		}
// 		else
// 			ThrdAF();
		pWorkRun->m_bThrdFlag[TF_AUTORUN] = FALSE;
#endif
	}
#endif

#if AJINTEX_BOARD
	if ( Col == RESULT_MEASURE )
	{
		if (pWorkRun->m_bAutoMode)
			return;
		if (m_ctrlCheckManualMeasure.GetCheck())
			return;


// 		if ( !m_bFirstInsp )
// 		{
// 			TECmsgOut( L"자동측정후에 사용가능합니다.", GREEN_1, 1 );
// 			return;
// 		}
		m_clResult.SetRow( Row );
		m_clResult.SetCol(RESULT_TARGET_NO);
		strResult = m_clResult.GetText( );
		strResult.Trim();
		if ( strResult.IsEmpty() ) 
		{
			// TECmsgOut( L"측정범위를 벋어났습니다.", GREEN_1, 1 );
			m_fnLogWrite( "측정범위를 벋어났습니다." );
			return;
		}
 
#if MIL_USE
		thread thrdMeasure = thread(ThrdReMeasure, Row, &bFnRes);
		thrdMeasure.detach();
#endif
	}
#endif
}


CString CAutoInspectorDlg::m_fnGetSelectResultData( t_ELIPSO_RESULT_INFO stResult, int nMeasureItem, int nLayer )
{
	CString strWork("");
	switch ( nMeasureItem )
	{
		case 0:	// THICKNESS
		{
			strWork.Format("%.3f", stResult.stLayer[nLayer - 1].dThickness);
		}
		break;
		case 1:	// N_550
		{
			strWork.Format("%.3f", stResult.stLayer[nLayer - 1].dN_550);
		}
		break;
		case 2:	// K_550
		{
			strWork.Format("%.3f", stResult.stLayer[nLayer - 1].dK_550);
		}
		break;
		case 3:	// N_632
		{
			strWork.Format("%.3f", stResult.stLayer[nLayer - 1].dN_632);
		}
		break;
		case 4:	// K632
		{
			strWork.Format("%.3f", stResult.stLayer[nLayer - 1].dK_632);
		}
		break;
	}
	return strWork;
}

void CAutoInspectorDlg::m_fnSetReCalculate()
{
	float  fStd[MAX_LAYER][MAX_ITEM];
	memset( fStd, 0x00, sizeof(fStd));
	memset(m_pTask->stData.m_dMax, 0x00, sizeof(m_pTask->stData.m_dMax));
	memset(m_pTask->stData.m_dMin, 0x00, sizeof(m_pTask->stData.m_dMin));
	memset(m_pTask->stData.m_dAvg, 0x00, sizeof(m_pTask->stData.m_dAvg));
	memset(m_pTask->stData.m_dStd, 0x00, sizeof(m_pTask->stData.m_dStd));
	memset(m_pTask->stData.m_dMax_Min, 0x00, sizeof(m_pTask->stData.m_dMax_Min));
	memset(m_pTask->stData.m_dUniformity, 0x00, sizeof(m_pTask->stData.m_dUniformity));
	memset(m_pTask->stData.m_fTot, 0x00, sizeof(m_pTask->stData.m_fTot));
	for( int i = 0; i < m_pTask->stResult.nSRData; i++ )
	{
		for (int j = 0; j < MAX_LAYER; j++)
		{
			for (int k = 0; k < MAX_ITEM; k++)
			{
				if (i == 0)
				{
					m_pTask->stData.m_dMax[j][k] = m_pTask->stResult.stSRData[i].dwResult[j][k];
					m_pTask->stData.m_dMin[j][k] = m_pTask->stResult.stSRData[i].dwResult[j][k];
					m_pTask->stData.m_dAvg[j][k] = m_pTask->stResult.stSRData[i].dwResult[j][k];
					m_pTask->stData.m_dStd[j][k] = 0.0;
					m_pTask->stData.m_dMax_Min[j][k] = 0.0;
					m_pTask->stData.m_dUniformity[j][k] = 0.0;
					m_pTask->stData.m_fTot[j][k] = 0.0f;
					m_pTask->stData.m_iZeroCnt[j][k] = 0;
				}

				if(m_pTask->stResult.stSRData[i].dwResult[j][k] > 0)
				{
					m_pTask->stData.m_fTot[j][k] += (float)m_pTask->stResult.stSRData[i].dwResult[j][k];
					m_pTask->stData.m_dAvg[j][k] = (double)(m_pTask->stData.m_fTot[j][k] / (double)(i + 1 - m_pTask->stData.m_iZeroCnt[j][k]));

					if (m_pTask->stData.m_dMax[j][k] < m_pTask->stResult.stSRData[i].dwResult[j][k])
					{
						m_pTask->stData.m_dMax[j][k] = m_pTask->stResult.stSRData[i].dwResult[j][k];
					}
					if (m_pTask->stData.m_dMin[j][k] > m_pTask->stResult.stSRData[i].dwResult[j][k])
					{
						m_pTask->stData.m_dMin[j][k] = m_pTask->stResult.stSRData[i].dwResult[j][k];
					}
					m_pTask->stData.m_dMax_Min[j][k] = abs(m_pTask->stData.m_dMax[j][k] - m_pTask->stData.m_dMin[j][k]);
				}
				else
				{
					m_pTask->stData.m_iZeroCnt[j][k]++;
				}
			}
		}
	}

	for (int j = 0; j < MAX_LAYER; j++)
	{
		for (int k = 0; k < MAX_ITEM; k++)
		{
			for (int i = 0; i < m_pTask->stResult.nSRData; i++)
			{
				if (m_pTask->stResult.stSRData[i].dwResult[j][k] > 0)
					fStd[j][k] += (float)pow(m_pTask->stResult.stSRData[i].dwResult[j][k] - m_pTask->stData.m_dAvg[j][k], 2);
			}

			m_pTask->stData.m_dStd[j][k] = sqrt((double)(fStd[j][k] / (double)(m_pTask->stResult.nSRData - m_pTask->stData.m_iZeroCnt[j][k])));
			m_pTask->stData.m_dUniformity[j][k] = (m_pTask->stData.m_dStd[j][k] / m_pTask->stData.m_dAvg[j][k]);
			m_pTask->stData.m_dContourMax[j][k] = m_pTask->stData.m_dMax[j][k];
			m_pTask->stData.m_dContourMin[j][k] = m_pTask->stData.m_dMin[j][k];
			m_pTask->stData.m_dContourStep[j][k] = m_pTask->stData.m_dMax_Min[j][k] / 10.;
		}
	}
	m_fnSetInspStatus( m_pTask->stData.m_dMax[m_nSelLayer][m_nMeasureItem],
		               m_pTask->stData.m_dMin[m_nSelLayer][m_nMeasureItem],
					   m_pTask->stData.m_dAvg[m_nSelLayer][m_nMeasureItem],
					   m_pTask->stData.m_dStd[m_nSelLayer][m_nMeasureItem],
					   m_pTask->stData.m_dMax_Min[m_nSelLayer][m_nMeasureItem],
					   m_pTask->stData.m_dUniformity[m_nSelLayer][m_nMeasureItem]);

}

void CAutoInspectorDlg::OnBnClickedBtnDebug()
{
}

void CAutoInspectorDlg::m_fnSRSheetClear(BOOL bAllClear /*= TRUE */)
{
	for( int i = 0; i < MAX_DATA; i++ )
	{
		m_clResult.SetRow(i + 1);
		if(bAllClear)
		{
			m_clResult.SetCol(RESULT_TARGET_NO);
			m_clResult.SetText("");
			m_clResult.SetCol(RESULT_COL);
			m_clResult.SetText("");
			m_clResult.SetCol(RESULT_ROW);
			m_clResult.SetText("");
			m_clResult.SetCol(RESULT_SUB);
			m_clResult.SetText("");
			m_clResult.SetCol(RESULT_X1_X2);
			m_clResult.SetText("");
			m_clResult.SetCol(RESULT_Y1_Y2);
			m_clResult.SetText("");
		}
		m_clResult.SetCol(RESULT_MSE);
		m_clResult.SetText("");
		m_clResult.SetCol(RESULT_TOTAL_THICKNESS);
		m_clResult.SetText("");
		m_clResult.SetCol(RESULT_LAYER_1);
		m_clResult.SetText("");
		m_clResult.SetCol(RESULT_LAYER_2);
		m_clResult.SetText("");
		m_clResult.SetCol(RESULT_LAYER_3);
		m_clResult.SetText("");
		m_clResult.SetCol(RESULT_LAYER_4);
		m_clResult.SetText("");
		m_clResult.SetCol(RESULT_LAYER_5);
		m_clResult.SetText("");
		m_clResult.SetCol(RESULT_LAYER_6);
		m_clResult.SetText("");
		m_clResult.SetCol(RESULT_LAYER_7);
		m_clResult.SetText("");
		m_clResult.SetCol(RESULT_LAYER_8);
		m_clResult.SetText("");
		m_clResult.SetCol(RESULT_LAYER_9);
		m_clResult.SetText("");
		m_clResult.SetCol(RESULT_LAYER_10);
		m_clResult.SetText("");
		}
	m_pTask->stResult.nSRData = 0;
	memset( m_pTask->stResult.stSRData, 0x00, sizeof(m_pTask->stResult.stSRData));
	memset( &m_pTask->stData, 0x00, sizeof(t_RESULT_INFO ));

	m_fnSetView();//초기화
}


// LRESULT CAutoInspectorDlg::m_fnSetRunBtn(WPARAM nNo, LPARAM lParam)
// {
// 	if (nNo == 1)
// 	{
// 		m_fnSetBtnOperation(IDC_BTN_VAC_OFF, L_BIT_BTN_ON, L_BIT_CHAR_ON, "VAC. ON");
// 	}
// 	else
// 	{
// 		m_fnSetBtnOperation(IDC_BTN_VAC_OFF, L_BIT_BTN_OFF, L_BIT_CHAR_OFF, "VAC. OFF");
// 	}
// 	return 0;
// }

LRESULT CAutoInspectorDlg::m_fnSetChangeBtn( WPARAM nSwitch, LPARAM lParam /*= 0 */ ) 
{
	m_nSwitchNo = nSwitch;
	for( int i = 0; i < 4; i++ )
	{
		if ( nSwitch == i )
		{
			m_fnSetBtnOperation( nCmdBtn[i],L_BIT_BTN_ON,L_BIT_CHAR_ON,strCmdBtn[i]);
		}
		else
		{
			m_fnSetBtnOperation( nCmdBtn[i],L_BIT_BTN_OFF,L_BIT_CHAR_OFF,strCmdBtn[i]);
		}
	}
	return 0;
}

//------------------------------------------------------------
// 각 버튼의 색상과 글자색을 변경한다.
//------------------------------------------------------------
void CAutoInspectorDlg::m_fnSetBtnOperation( int nID, 
										   int nBtnColor, 
										   int nTextColor,
										   CString strMsg )
{
	if ( this->m_hWnd == NULL ) return;

	CColorBtn* pColorBtn = NULL;
	CRect rect;
	pColorBtn = (CColorBtn*) GetDlgItem( nID );
	if ( pColorBtn == NULL ) return;
	pColorBtn->m_fnSetParam( nBtnColor, nTextColor );
	GetDlgItem( nID )->GetWindowRect(rect);
	if ( !strMsg.IsEmpty() )
	{
		GetDlgItem( nID )->SetWindowText( strMsg );
	}
	ScreenToClient( rect );
	InvalidateRect( rect, FALSE );
}

void CAutoInspectorDlg::OnBnClickedBtnInspStart()
{
#if !_DEBUG
	if ( m_pTask->stLogin.nLevel == UNKNOWN_USER )
	{
		return;
	}
#endif

	int nNo = m_ctrlComboReceipe.GetCurSel();
	if (nNo < 0)
	{
		TECmsgOut(L"측정 Receipe 선택을 해주세요.", LIGHTMAGENTA, 1);
		return;
	}

	if (m_iRadioMeasureMode < 0)
	{
		TECmsgOut(L"측정 MODE 선택을 해주세요.", LIGHTMAGENTA, 1);
		return;
	}

#if	AJINTEX_BOARD
#if MIL_USE
	if ( !pWorkRun->m_bThrdFlag[TF_AUTORUN])//theApp.m_fnGetViewPointer()->m_pTask->stEqp.bIoStartButton )
	{
		theApp.m_fnGetViewPointer()->m_pAuto->m_bPause = FALSE;
		m_fnSetChangeBtn( SW_INSP_START );

// 		int ArrExcepID_1[7] = {
// 			IDC_BTN_INSP_STOP,
// 			IDC_BTN_PAUSE,
// 			IDC_BTN_INSP_START,
// 			IDC_MEASURE_ITEM,
// 			IDC_BTN_RESULT,
// 			IDC_FPSPREAD_RESULT,
// 			IDC_EDIT_WAFER_NUM 
// 		};
// 		EnableDlgItem(this, FALSE, ArrExcepID_1, 7);
		m_fnSetBtnEnable(3);

		thread thrdStartAuto = thread(ThrdRunning);
		thrdStartAuto.detach();
// 		theApp.m_fnGetViewPointer()->m_pAuto->PostMessageA( UM_START_BUTTON, 0, 0 );
	}
#endif
#endif
}

void CAutoInspectorDlg::OnBnClickedBtnInspStop()
{
#if !_DEBUG
	if ( m_pTask->stLogin.nLevel == UNKNOWN_USER )
	{
		return;
	}
#endif

#if	AJINTEX_BOARD
#if MIL_USE
	if(pWorkRun != NULL)
	{
		if (pWorkRun->m_bThrdFlag[TF_AUTORUN])// theApp.m_fnGetViewPointer()->m_pTask->stEqp.bIoStartButton )
		{
			theApp.m_fnGetViewPointer()->m_pAuto->m_bPause = FALSE;
			m_fnSetChangeBtn(SW_INSP_END);

			pWorkRun->m_bThrdFlag[TF_AUTORUN] = FALSE;

			// 		theApp.m_fnGetViewPointer()->m_pAuto->PostMessageA( UM_START_BUTTON, 0, 0 );
		}
	}
#endif
#endif
}

void CAutoInspectorDlg::OnBnClickedBtnRestart()
{
#if !_DEBUG
	if ( m_pTask->stLogin.nLevel == UNKNOWN_USER )
	{
		return;
	}
#endif

#if	AJINTEX_BOARD
	if (( theApp.m_fnGetViewPointer()->m_pTask->stEqp.bIoStartButton ) &&
		( theApp.m_fnGetViewPointer()->m_pAuto->m_bPause))
	{
		theApp.m_fnGetViewPointer()->m_pAuto->m_bPause = FALSE;
		m_fnSetChangeBtn( SW_REINSP );
	}
#endif
}

void CAutoInspectorDlg::OnBnClickedBtnPause()
{
#if !_DEBUG
	if ( m_pTask->stLogin.nLevel == UNKNOWN_USER )
	{
		return;
	}
#endif
#if	AJINTEX_BOARD
#if MIL_USE
	if (pWorkRun->m_bThrdFlag[TF_AUTORUN])//theApp.m_fnGetViewPointer()->m_pTask->stEqp.bIoStartButton )
	{
		theApp.m_fnGetViewPointer()->m_pAuto->m_bPause = !theApp.m_fnGetViewPointer()->m_pAuto->m_bPause;
		if(theApp.m_fnGetViewPointer()->m_pAuto->m_bPause)
		{
#if _DEBUG
			AfxMessageBox(_T("||Pause AutoRun||"), MB_ICONHAND);
#endif
// 			EnableDlgItem(this, TRUE);
			m_fnSetBtnEnable(1);

			m_fnSetChangeBtn(SW_INSP_PAUSE);
		}
		else
		{
#if _DEBUG
			AfxMessageBox(_T("--Resume AutoRun--"), MB_ICONEXCLAMATION);
#endif
// 			int ArrExcepID_1[7] = {
// 				IDC_BTN_INSP_STOP,
// 				IDC_BTN_PAUSE,
// 				IDC_BTN_INSP_START,
// 				IDC_MEASURE_ITEM,
// 				IDC_BTN_RESULT,
// 				IDC_FPSPREAD_RESULT,
// 				IDC_EDIT_WAFER_NUM
// 			};
// 			EnableDlgItem(this, FALSE, ArrExcepID_1, 7);
			m_fnSetBtnEnable(3);

			m_fnSetChangeBtn(SW_INSP_START);
		}
	}
#endif
#endif
}

LRESULT CAutoInspectorDlg::m_fnSetInspSwitch(  WPARAM nNo, LPARAM lParam )
{
	m_fnSetChangeBtn( nNo );

	return 0;
}
void CAutoInspectorDlg::OnBnClickedBtnPreview()
{
	memcpy( &m_pTask->stResult, &m_pTask->stCurrModel, sizeof( t_MODEL_INFO ));
}


void CAutoInspectorDlg::OnBnClickedBtnUserInfo()
{
// 	t_USER_INFO stUser;
// 	sprintf_s( stUser.szOperatorID, "%s", m_pTask->stLogin.szUserID );
// 	sprintf_s( stUser.szDate, "%s", m_strInspDate.GetString());
// 	sprintf_s( stUser.szTime, "%s", m_strInspTime.GetString());
// 
// 	theApp.m_fnGetViewPointer()->m_pUserAuto->m_fnSetInputData( stUser );
// 	theApp.m_fnGetViewPointer()->m_pUserAuto->m_fnSetShowFlag( !theApp.m_fnGetViewPointer()->m_pUserAuto->m_bShow );
}


void CAutoInspectorDlg::OnBnClickedBtnAutoMeasure()
{
	theApp.m_fnGetViewPointer()->m_pAutoParam->m_fnSetShowFlag( TRUE );
}

void CAutoInspectorDlg::OnBnClickedBtnReport()
{
	m_bFirstInsp = TRUE;
// 	memcpy(&m_pTask->stResult.stUser, &m_stUser, sizeof(t_USER_INFO));
// 	m_fnSetXMLResult(m_pTask->stLastModel.szLastModel, m_pTask->stResult, m_pTask->stData, SW_AUTO);

	this->PostMessageA( UM_REPORT_READY, 0, 0 );
}

void CAutoInspectorDlg::OnBnClickedBtnCw()
{
	if ( m_pTask->stLogin.nLevel == UNKNOWN_USER )
	{
		return;
	}
	CString strData("");
	CString strNAME("");
	CString strX("");
	CString strY("");
	CString strMM("");
	CString strR("");
	double  dMM   = 0.0;
	double  dR    = 0;
	GetDlgItem( IDC_EDT_MM )->GetWindowText( strMM );
	GetDlgItem( IDC_EDT_R )->GetWindowText( strR );
	strMM.Trim();
	strR.Trim();
	if ( strMM.IsEmpty() )
	{
		strData.Format( "반지름을 입력해 주세요." );
		TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 1 );
		return;
	}
	if ( strR.IsEmpty() )
	{
		strData.Format( "각도를 입력해 주세요." );
		TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 1 );
		return;
	}
	dMM = atof( strMM );
	dR = atof( strR );

	strData.Format( "정말로 시계방향으로 좌표를 추가하시겠습니까?" );
	int nResult = TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 2 );
	if ( nResult == 1 )
	{
		t_WAPER_INFO  stWaper;
		stWaper.bCw = TRUE;
		stWaper.dMM = dMM;
		stWaper.dR  = dR;
		if ( m_pTask->stModel.nSRData > 0 )
		{
			stWaper.bZeroInclude = FALSE;
		}
		theApp.m_fnGetViewPointer()->m_pModel->m_fnSetWaper( stWaper );
		m_fnSRSheetClear();
		if ( stWaper.bOK )
		{
			for( int i = 0; i < stWaper.nTotal; i++ )
			{
				m_clResult.SetRow( m_pTask->stResult.nSRData + 1 );
				m_clResult.SetCol( MEASURE_NAME );
				strNAME.Format( "%01d", m_pTask->stResult.nSRData + 1 );
				m_clResult.SetText(strNAME);
				m_clResult.SetCol( MEASURE_X );
				strX.Format( "%7.3f", stWaper.dX[i] );
				m_clResult.SetText(strX);
				m_clResult.SetCol( MEASURE_Y );
				strY.Format( "%7.3f", stWaper.dY[i] );
				m_clResult.SetText(strY);
				sprintf_s(m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].szName, "%s", strNAME.GetString());
				m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].dwXPosition = stWaper.dX[i];
				m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].dwYPosition = stWaper.dY[i];
				m_pTask->stResult.nSRData++;
			}
		}
	}
}

void CAutoInspectorDlg::OnBnClickedBtnCcw()
{
	if ( m_pTask->stLogin.nLevel == UNKNOWN_USER )
	{
		return;
	}
	CString strData("");
	CString strNAME("");
	CString strX("");
	CString strY("");
	CString strMM("");
	CString strR("");
	double  dMM   = 0.0;
	double  dR    = 0;
	GetDlgItem( IDC_EDT_MM )->GetWindowText( strMM );
	GetDlgItem( IDC_EDT_R )->GetWindowText( strR );
	strMM.Trim();
	strR.Trim();
	if ( strMM.IsEmpty() )
	{
		strData.Format( "반지름을 입력해 주세요." );
		TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 1 );
		return;
	}
	if ( strR.IsEmpty() )
	{
		strData.Format( "각도를 입력해 주세요." );
		TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 1 );
		return;
	}
	dMM = atof( strMM );
	dR = atof( strR );

	strData.Format( "정말로 시계반대방향으로 좌표를 추가하시겠습니까?" );
	int nResult = TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 2 );
	if ( nResult == 1 )
	{
		t_WAPER_INFO  stWaper;
		stWaper.bCw = FALSE;
		stWaper.dMM = dMM;
		stWaper.dR  = dR;
		if ( m_pTask->stModel.nSRData > 0 )
		{
			stWaper.bZeroInclude = FALSE;
		}
		theApp.m_fnGetViewPointer()->m_pModel->m_fnSetWaper( stWaper );
		m_fnSRSheetClear();
		if ( stWaper.bOK )
		{
			for( int i = 0; i < stWaper.nTotal; i++ )
			{
				m_clResult.SetRow( m_pTask->stResult.nSRData + 1 );
				m_clResult.SetCol( MEASURE_NAME );
				strNAME.Format( "%01d", m_pTask->stResult.nSRData + 1 );
				m_clResult.SetText(strNAME);
				m_clResult.SetCol( MEASURE_X );
				strX.Format( "%7.3f", stWaper.dX[i] );
				m_clResult.SetText(strX);
				m_clResult.SetCol( MEASURE_Y );
				strY.Format( "%7.3f", stWaper.dY[i] );
				m_clResult.SetText(strY);
				sprintf_s(m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].szName, "%s", strNAME.GetString());
				m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].dwXPosition = stWaper.dX[i];
				m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].dwYPosition = stWaper.dY[i];
				m_pTask->stResult.nSRData++;
			}
		}
	}
}


void CAutoInspectorDlg::OnBnClickedBtnPoint4()
{
	DivisionCirclePos(1);
	SendMessage(WM_COMMAND, IDC_RADIO_MEASURE_MODE, IDC_RADIO_MEASURE_MODE);
}


void CAutoInspectorDlg::OnBnClickedBtnPoint12()
{
	DivisionCirclePos(2);
	SendMessage(WM_COMMAND, IDC_RADIO_MEASURE_MODE, IDC_RADIO_MEASURE_MODE);
}


void CAutoInspectorDlg::OnBnClickedBtnPoint28()
{
	DivisionCirclePos(3);
	SendMessage(WM_COMMAND, IDC_RADIO_MEASURE_MODE, IDC_RADIO_MEASURE_MODE);
}


void CAutoInspectorDlg::OnBnClickedBtnPoint60()
{
	DivisionCirclePos(4);
	SendMessage(WM_COMMAND, IDC_RADIO_MEASURE_MODE, IDC_RADIO_MEASURE_MODE);
}


void CAutoInspectorDlg::DivisionCirclePos(int iDivisionNum)
{
	CString strMM("");
	double dMM = 0.0;
	double dMM1 = 0.0;
	CString strData("");
	CString strTargetNo("");
	CString strX("");
	CString strY("");
	CString strR("");

	BOOL bOnlyZeroPoint = FALSE;

	if (iDivisionNum < 0)
		return;
	else if (iDivisionNum == 0)
	{
		iDivisionNum = 1;
		bOnlyZeroPoint = TRUE;
	}
	m_iDivisionNum = iDivisionNum;

	strData.Format("기존 데이터 삭제 후, 검사위치가 재작성됩니다. 등록할까요?");
	if (!TECmsgOut(m_Util.CStringTobstr(strData), GREEN_1, 2))
		return;


	if (m_iSetPntMode != PNT_CIRCLE)
	{
		m_iSetPntMode = PNT_CIRCLE;
// 		m_iRadioMeasureMode = 0;

		m_ctrlPattAlignMode.SetCheck(FALSE);
		m_clModelList.SetCurSel(-1);
		m_clModelList2.SetCurSel(-1);
		m_clMethod.SetCurSel(-1);

		m_dWaperThickness = 700.0;
		m_dWaperExclusion = 5.0;
	}

	int arrWaferInch[4] = { 4,6,8,12 };
	theApp.m_fnGetViewPointer()->m_pModel->m_fnGetInchToMM(arrWaferInch[m_clWaperSize.GetCurSel()], dMM, m_dWaperExclusion);
	dMM1 = (dMM-5.0) / (double)iDivisionNum;

	t_WAPER_INFO  stWaperCircle;
	stWaperCircle.bCw = !((CButton*)GetDlgItem(IDC_CHK_ARRAW))->GetCheck();
	stWaperCircle.dR = 90.;
	double dR1 = stWaperCircle.dR;

	stWaperCircle.bOnlyZero = bOnlyZeroPoint;

	m_fnSRSheetClear();

	m_pTask->stResult.dWaperThickness = m_dWaperThickness;
	strData.Format("%7.3f", m_pTask->stResult.dWaperThickness);
	GetDlgItem(IDC_EDT_THICKNESS)->SetWindowText(strData);

	m_pTask->stResult.dEdgeExclusion = m_dWaperExclusion;
	strData.Format("%7.3f", m_pTask->stResult.dEdgeExclusion);
	GetDlgItem(IDC_EDT_EDGE_EXCLUSION)->SetWindowText(strData);

	for (int i = 0; i < iDivisionNum; i++)
	{
		if (m_pTask->stResult.nSRData > 0)
		{
			stWaperCircle.bZeroInclude = FALSE;
		}

		stWaperCircle.dMM += dMM1;
		stWaperCircle.dR = dR1 / (i + 1);
		theApp.m_fnGetViewPointer()->m_pModel->m_fnSetWaperDivision(stWaperCircle);

		if (stWaperCircle.bOK)
		{
			for (int j = 0; j < stWaperCircle.nTotal; j++)
			{
				m_clResult.SetRow(m_pTask->stResult.nSRData + 1);
				m_clResult.SetCol(RESULT_TARGET_NO);
				strTargetNo.Format("%01d", m_pTask->stResult.nSRData + 1);
				m_clResult.SetText(strTargetNo);
				strData.Format( "%01d", 0);
				m_clResult.SetCol(RESULT_COL);
				m_clResult.SetText(strData);
				m_clResult.SetCol(RESULT_ROW);
				m_clResult.SetText(strData);
				m_clResult.SetCol(RESULT_SUB);
				m_clResult.SetText(strData);
				m_clResult.SetCol(RESULT_MSE);
				m_clResult.SetText("");
				strX.Format("%7.3f", stWaperCircle.dX[j]);
				m_clResult.SetCol(RESULT_X1_X2);
				m_clResult.SetText(strX);
				m_clResult.SetCol(RESULT_Y1_Y2);
				strY.Format("%7.3f", stWaperCircle.dY[j]);
				m_clResult.SetText(strY);
				m_clResult.SetCol(RESULT_TOTAL_THICKNESS);
				m_clResult.SetText("");
				m_clResult.SetCol(RESULT_LAYER_1);
				m_clResult.SetText("");
				m_clResult.SetCol(RESULT_LAYER_2);
				m_clResult.SetText("");
				m_clResult.SetCol(RESULT_LAYER_3);
				m_clResult.SetText("");
				m_clResult.SetCol(RESULT_LAYER_4);
				m_clResult.SetText("");
				m_clResult.SetCol(RESULT_LAYER_5);
				m_clResult.SetText("");
				m_clResult.SetCol(RESULT_LAYER_6);
				m_clResult.SetText("");
				m_clResult.SetCol(RESULT_LAYER_7);
				m_clResult.SetText("");
				m_clResult.SetCol(RESULT_LAYER_8);
				m_clResult.SetText("");
				m_clResult.SetCol(RESULT_LAYER_9);
				m_clResult.SetText("");
				m_clResult.SetCol(RESULT_LAYER_10);
				m_clResult.SetText("");
				sprintf_s( m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].szName, "%01d", m_pTask->stResult.nSRData + 1);
				m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].dwXPosition = stWaperCircle.dX[j];
				m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].dwYPosition = stWaperCircle.dY[j];
				m_pTask->stResult.nSRData++;
			}
		}
	}
}

void CAutoInspectorDlg::OnBnClickedChkAlign()
{
	m_bAlign = ((CButton*)GetDlgItem( IDC_CHK_ALIGN ))->GetCheck( );
}


void CAutoInspectorDlg::OnCbnSelchangeCboInch()
{
	m_nWaperSize = m_clWaperSize.GetCurSel();
	theApp.m_fnGetViewPointer()->m_pMap->m_nWaperSize = m_nWaperSize;
	t_USER_INFO stUser;
	m_fnSetUserInput(&stUser);

	switch (m_nWaperSize)
	{
	case WAPER_4:
		theApp.m_fnGetViewPointer()->m_pIO->m_fnSetOutputSignal(WAFER_0B01, FALSE);
		theApp.m_fnGetViewPointer()->m_pIO->m_fnSetOutputSignal(WAFER_0B10, FALSE);
		break;
	case WAPER_6:
		theApp.m_fnGetViewPointer()->m_pIO->m_fnSetOutputSignal(WAFER_0B01, TRUE);
		theApp.m_fnGetViewPointer()->m_pIO->m_fnSetOutputSignal(WAFER_0B10, FALSE);
		break;
	case WAPER_8:
		theApp.m_fnGetViewPointer()->m_pIO->m_fnSetOutputSignal(WAFER_0B01, FALSE);
		theApp.m_fnGetViewPointer()->m_pIO->m_fnSetOutputSignal(WAFER_0B10, TRUE);
		break;
	case WAPER_12:
		theApp.m_fnGetViewPointer()->m_pIO->m_fnSetOutputSignal(WAFER_0B01, TRUE);
		theApp.m_fnGetViewPointer()->m_pIO->m_fnSetOutputSignal(WAFER_0B10, TRUE);
		break;
	default:
		theApp.m_fnGetViewPointer()->m_pIO->m_fnSetOutputSignal(WAFER_0B01, FALSE);
		theApp.m_fnGetViewPointer()->m_pIO->m_fnSetOutputSignal(WAFER_0B10, FALSE);
		break;
	}

	if(m_nWaperSize_old != m_nWaperSize && m_iRadioMeasureMode_old == m_iRadioMeasureMode)
	{
// 		m_fnSRSheetClear();
// 		memset(&theApp.m_fnGetViewPointer()->m_pTask->stData, 0x00, sizeof(t_RESULT_INFO));
// 		theApp.m_fnGetViewPointer()->m_pAuto->m_clResult.SetRow(1);
// 		theApp.m_fnGetViewPointer()->m_pAuto->m_clResult.SetCol(1);
// 
		switch (m_iRadioMeasureMode)
		{
		case 0:
			DivisionCirclePos(m_iDivisionNum);
			break;
		case 1:
			SendMessage(WM_COMMAND, IDC_BTN_RECTANGLE_ADD, 0);
			break;
		default:
			break;
		}

	}

	m_nWaperSize_old = m_nWaperSize;
	m_iRadioMeasureMode_old = m_iRadioMeasureMode;

	UpdateData(FALSE);
}


void CAutoInspectorDlg::OnCbnSelchangeCboDani()
{
}


void CAutoInspectorDlg::OnCbnSelchangeCboType()
{
	m_bWaperType = (BOOL) m_clWaperType.GetCurSel();
}

void CAutoInspectorDlg::OnEnChangeEdtThickness()
{
	CString strData("");
	double dThickness = 0.0;
	GetDlgItem( IDC_EDT_THICKNESS )->GetWindowText( strData );
	dThickness = atof( strData );
	if (( dThickness > 5000.0 ))
// 		||( dThickness < 0.0 ))
	{
		TECmsgOut( L"두께를 확인해 주세요.(~ 5000)", GREEN_1, 1 );
		GetDlgItem( IDC_EDT_THICKNESS )->SetWindowText( "1000" );
		return;
	}
	m_dWaperThickness = dThickness;// / 1000.;
	t_USER_INFO stUser;
	m_fnSetUserInput(&stUser);
}


void CAutoInspectorDlg::OnBnClickedBtnResetData()
{
	if ( m_pTask->stLogin.nLevel == UNKNOWN_USER )
	{
		return;
	}
	m_fnSRSheetClear();
	theApp.m_fnGetViewPointer()->m_pContour->m_fnSetUpdateData();
	m_fnSetInspStatus( m_pTask->stData.m_dMax[m_nSelLayer][m_nMeasureItem],
						m_pTask->stData.m_dMin[m_nSelLayer][m_nMeasureItem],
						m_pTask->stData.m_dAvg[m_nSelLayer][m_nMeasureItem],
						m_pTask->stData.m_dStd[m_nSelLayer][m_nMeasureItem],
						m_pTask->stData.m_dMax_Min[m_nSelLayer][m_nMeasureItem],
						m_pTask->stData.m_dUniformity[m_nSelLayer][m_nMeasureItem] );
	m_bFirstInsp = FALSE;

}

void CAutoInspectorDlg::m_fnSetUserInput( t_USER_INFO* pstUser )
{
	if (pWorkRun == NULL || m_pTask==NULL)
		return;

	t_USER_INFO stUser = *pstUser;

// 	theApp.m_fnGetViewPointer()->m_pUserAuto->m_fnGetInputData( stUser );
	memcpy(&stUser, &m_stUser, sizeof(t_USER_INFO));

	double dArrWaferSize[4] = { 100., 150., 200., 300. };
	stUser.dSampleSize = dArrWaferSize[m_nWaperSize];
	stUser.dThickness = m_dWaperThickness;// *1000.;
	stUser.dExclusion = m_dWaperExclusion;

	sprintf_s(stUser.szRecipeFileName, "%s", pWorkRun->m_strRecipeName.GetString());

	CString strArrMode[2];
	strArrMode[0] = "OFF";
	strArrMode[1] = "ON";
	sprintf_s(stUser.szInsertMode, "%s", strArrMode[m_ctrlCheckAutoMode.GetCheck()].GetString());
	sprintf_s(stUser.szWaferAlignMode, "%s", strArrMode[m_ctrlCheckWaferAlign.GetCheck()].GetString());
	sprintf_s(stUser.szVisionAFMode, "%s", strArrMode[m_ctrlChkUseAF.GetCheck()].GetString());

	CString strWaperNo;
	theApp.m_fnGetViewPointer()->m_pAuto->m_ctrlEditWaferNum.GetWindowTextA(strWaperNo);
	stUser.iSlotNo = atoi(strWaperNo);
	sprintf_s(stUser.szOperatorID, "%s", m_pTask->stLogin.szUserID);

	CString strFullPath;
	strFullPath.Format("D:\\%sSE data\\%s\\%s_%s\\%s\\",
		DEFAULT_RESULT_PATH, stUser.szDate, m_strLotStartTime, stUser.szLotID, strWaperNo);
	sprintf_s(stUser.szDataFile, "%s", strFullPath.GetString());

	stUser.iMeasureMode = m_iSetPntMode;

	memcpy( &m_stUser, &stUser, sizeof(t_USER_INFO));

	*pstUser = stUser;
}

void CAutoInspectorDlg::OnBnClickedBtnUserInput()
{
	if (theApp.m_fnGetViewPointer()->m_pUserAuto == NULL)
		return;

	t_USER_INFO stUser;
	m_fnSetUserInput(&stUser);

	theApp.m_fnGetViewPointer()->m_pUserAuto->m_fnSetInputData(stUser);
	if (true)
	{
		theApp.m_fnGetViewPointer()->m_pUserAuto->m_fnSetShowFlag(TRUE);
	}
}


void CAutoInspectorDlg::OnBnClickedBtnResult()
{
#if !_DEBUG
	if ( m_pTask->stLogin.nLevel == UNKNOWN_USER )
	{
		return;
	}
#endif
	ShellExecute( NULL, _T("open"), _T("explorer.exe"), m_pTask->stCommon.szExcelResultPath, NULL, SW_SHOW );
}

double CAutoInspectorDlg::m_fnGetEdgeExclusion()
{
	CString strData("");
	GetDlgItem( IDC_EDT_EDGE_EXCLUSION )->GetWindowText( strData );
	return abs( atof( strData ));
}


void CAutoInspectorDlg::OnBnClickedBtnPoint124()
{
	DivisionCirclePos(5);
	SendMessage(WM_COMMAND, IDC_RADIO_MEASURE_MODE, IDC_RADIO_MEASURE_MODE);
}


void CAutoInspectorDlg::OnBnClickedBtnPoint252()
{
	DivisionCirclePos(6);
	SendMessage(WM_COMMAND, IDC_RADIO_MEASURE_MODE, IDC_RADIO_MEASURE_MODE);
}


void CAutoInspectorDlg::OnBnClickedBtnPoint508()
{
	DivisionCirclePos(7);
	SendMessage(WM_COMMAND, IDC_RADIO_MEASURE_MODE, IDC_RADIO_MEASURE_MODE);
}



void CAutoInspectorDlg::OnBnClickedBtnCurrXy()
{
	double dwPositionX = 0.0;
	double dwPositionY = 0.0;
#if AJINTEX_BOARD
	CString strData("");
	if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( X_AXIS, dwPositionX ) == RC_NORMAL )
	{
		strData.Format( "%7.3f", dwPositionX);
		GetDlgItem( IDC_EDT_CURR_X )->SetWindowText( strData );
	}
	if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( Y_AXIS, dwPositionY ) == RC_NORMAL )
	{
		strData.Format( "%7.3f", dwPositionY);
		GetDlgItem( IDC_EDT_CURR_Y )->SetWindowText( strData );
	}
#else
		GetDlgItem( IDC_EDT_CURR_X )->SetWindowText( "0.000" );
		GetDlgItem(IDC_EDT_CURR_Y)->SetWindowText("0.000");
#endif
}


void CAutoInspectorDlg::OnBnClickedBtnSquareAdd()
{
	if ( m_pTask->stLogin.nLevel == UNKNOWN_USER )
	{
		return;
	}
	CString strData("");
	int     nRow = 0;
	CString strZ("");

	CString strStartX("");
	CString strStartY("");
	CString strStepX("");
	CString strStepY("");
	CString strCountX("");
	CString strCountY("");
	CString strNAME("");

	double dPositionX	= 0.0L;
	double dPositionY	= 0.0L;
	double dPositionZ	= 0.0L;
	double dStartX		= 0.0L;
	double dStartY		= 0.0L;
	double dStepX		= 0.0L;
	double dStepY		= 0.0L;
	int    nCountX		= 0;
	int    nCountY		= 0;
	int    nMethod      = 0;

// 	nMethod = m_clMethod.GetCurSel();
	strData.Format( "기존 데이터는 삭제후 검사위치가 재 작성됩니다. 실행할까요?" );
	int nResult = TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 2 );
	if(nResult == 1)
	{
		GetDlgItem( IDC_EDT_CURR_X )->GetWindowText( strStartX );
		dStartX = atof( strStartX );
		GetDlgItem( IDC_EDT_CURR_Y )->GetWindowText( strStartY );
		dStartY = atof( strStartY );
		GetDlgItem( IDC_EDT_X_DISTANCE )->GetWindowText( strStepX );
		dStepX = atof( strStepX );
		GetDlgItem( IDC_EDT_Y_DISTANCE )->GetWindowText( strStepY );
		dStepY = atof( strStepY );
		GetDlgItem( IDC_EDT_X_COUNT )->GetWindowText( strCountX );
		nCountX = atoi( strCountX );
		GetDlgItem( IDC_EDT_Y_COUNT )->GetWindowText( strCountY );
		nCountY = atoi( strCountY );
		if	(( nCountX > 0 )		&&
			 ( nCountY > 0 ))
		{

			if (( nCountX * nCountY ) > MAX_DATA )
			{
				strData.Format("등록할 데이터가 MAX_DATA(%01d)를 초과합니다. MAX_DATA 이내로 설정해 주세요", MAX_DATA );
				m_fnLogWrite( strData );
				TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 1 );;
				return;
			}

			if ( !theApp.m_fnGetViewPointer()->m_pModel->m_fnGetCheckAxisData( dStartX, dStartY, dStepX, dStepY, nCountX, nCountY ) )
			{
				strData.Format("등록할 데이터 값이 축위치값(X:%7.3f,%7.3f),(Y:%7.3f,%7.3f)) 초과했습니다.", 
																		0.0, 
																		STAGE_SIZE_X,
																		0.0, 
																		STAGE_SIZE_Y);
				m_fnLogWrite( strData );
				TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 1 );;
				return;
			}

			m_pTask->stResult.nSRData = 0;
			memset( m_pTask->stResult.stSRData, 0x00, sizeof(m_pTask->stResult.stSRData));
			m_fnSRSheetClear();

			m_clResult.SetRow( 1 );
			m_clResult.SetCol( MEASURE_NAME );
			strNAME.Format( "%01d", 1);
			m_clResult.SetText( strNAME );
			strData.Format( "%7.3f", 0.0 );
			m_clResult.SetCol( MEASURE_X );
			m_clResult.SetText( strData );
			strData.Format( "%7.3f", 0.0 );
			m_clResult.SetCol( MEASURE_Y );
			m_clResult.SetText( strData );
			sprintf_s(m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].szName, "%s", strNAME.GetString());
			m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].dwXPosition = 0.0;
			m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].dwYPosition = 0.0;
			m_pTask->stResult.nSRData++;

			int nRow = 2;
			BOOL bToggle = FALSE;
			switch( nMethod )
			{ 
				case 0: // 0 : X축방향 Z
					{
						for( int j = 0; j <= nCountY; j++ )
						{
							dPositionY = dStartY + ( dStepY * j );
							for( int k = 0; k <= nCountX; k++ )
							{
								dPositionX = dStartX + ( dStepX * k );
								m_clResult.SetRow( nRow );
								m_clResult.SetCol( RESULT_TARGET_NO );
								strNAME.Format( "%01d", nRow);
								m_clResult.SetText( strNAME );
								strData.Format( "%7.3f", dPositionX );
								m_clResult.SetCol(RESULT_X1_X2);
								m_clResult.SetText( strData );
								strData.Format( "%7.3f", dPositionY );
								m_clResult.SetCol(RESULT_Y1_Y2);
								m_clResult.SetText( strData );
								sprintf_s(m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].szName, "%s", strNAME.GetString());
								m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].dwXPosition = dPositionX;
								m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].dwYPosition = dPositionY;
								m_pTask->stResult.nSRData++;
								nRow++;
							}
						}
					}
					break;
				case 1: // 1 : X축방향 ㄹ
					{
						for( int j = 0; j <= nCountY; j++ )
						{
							dPositionY = dStartY + ( dStepY * j );
							for( int k = 0; k <= nCountX; k++ )
							{
								if ( !bToggle ) 
								{
									dPositionX = dStartX + ( dStepX * k );
								}
								else
								{
									dPositionX = ( dStartX + ( dStepX * ( nCountX - 1)) ) - ( dStepX * k );
								}
								m_clResult.SetRow( nRow );
								m_clResult.SetCol(RESULT_TARGET_NO);
								strNAME.Format("%01d", nRow);
								m_clResult.SetText(strNAME);
								strData.Format("%7.3f", dPositionX);
								m_clResult.SetCol(RESULT_X1_X2);
								m_clResult.SetText(strData);
								strData.Format("%7.3f", dPositionY);
								m_clResult.SetCol(RESULT_Y1_Y2);
								m_clResult.SetText(strData);
								sprintf_s(m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].szName, "%s", strNAME.GetString());
								m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].dwXPosition = dPositionX;
								m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].dwYPosition = dPositionY;
								m_pTask->stResult.nSRData++;
								nRow++;
							}
							bToggle = !bToggle;
						}
					}
					break;
				case 2: // 2 : Y축방향 Z
					{
						for( int j = 0; j <= nCountX; j++ )
						{
							dPositionX = dStartX + ( dStepX * j );
							for( int k = 0; k <= nCountY; k++ )
							{
								dPositionY = dStartY + ( dStepY * k );
								m_clResult.SetRow( nRow );
								m_clResult.SetCol(RESULT_TARGET_NO);
								strNAME.Format("%01d", nRow);
								m_clResult.SetText(strNAME);
								strData.Format("%7.3f", dPositionX);
								m_clResult.SetCol(RESULT_X1_X2);
								m_clResult.SetText(strData);
								strData.Format("%7.3f", dPositionY);
								m_clResult.SetCol(RESULT_Y1_Y2);
								m_clResult.SetText(strData);
								sprintf_s(m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].szName, "%s", strNAME.GetString());
								m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].dwXPosition = dPositionX;
								m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].dwYPosition = dPositionY;
								m_pTask->stResult.nSRData++;
								nRow++;
							}
						}
					}
					break;
				case 3 : // 3 : Y축방향 ㄹ
					{
						for( int j = 0; j <= nCountX; j++ )
						{
							dPositionX = dStartX + ( dStepX * j );
							for( int k = 0; k <= nCountY; k++ )
							{
								if ( !bToggle ) 
								{
									dPositionY = dStartY + ( dStepY * k );
								}
								else
								{
									dPositionY = ( dStartY + ( dStepY * ( nCountY - 1)) ) - ( dStepY * k );
								}
								m_clResult.SetRow( nRow );
								m_clResult.SetCol(RESULT_TARGET_NO);
								strNAME.Format("%01d", nRow);
								m_clResult.SetText(strNAME);
								strData.Format("%7.3f", dPositionX);
								m_clResult.SetCol(RESULT_X1_X2);
								m_clResult.SetText(strData);
								strData.Format("%7.3f", dPositionY);
								m_clResult.SetCol(RESULT_Y1_Y2);
								m_clResult.SetText(strData);
								sprintf_s(m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].szName, "%s", strNAME.GetString());
								m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].dwXPosition = dPositionX;
								m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].dwYPosition = dPositionY;
								m_pTask->stResult.nSRData++;
								nRow++;
							}
							bToggle = !bToggle;
						}
					}
					break;
				default:
					{
						strData = "등간격설정 : 등간격 지정 방법을 설정해 주세요";
						m_fnLogWrite( strData );
						TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 1 );;

					}
			}
		}
		else
		{
			strData = "등간격설정 : 간격 또는 이동횟수에 값을 Zero 이상으로 설정해 주세요";
			m_fnLogWrite( strData );
			TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 1 );;
			return;
		}
	}
}


void CAutoInspectorDlg::OnEnChangeEdtEdgeExclusion()
{
	CString strData("");
	double dExclusion = 0.0;
	GetDlgItem( IDC_EDT_EDGE_EXCLUSION )->GetWindowText( strData );
	dExclusion = atof( strData );
	if (( dExclusion > 20.0 ) ||
		( dExclusion < 0.0 ))
	{
		m_dWaperExclusion = 0.0;
		TECmsgOut( L"Edge Exclusion을 확인해 주세요.(0.0 ~ 20.0)", GREEN_1, 1 );
		GetDlgItem( IDC_EDT_EDGE_EXCLUSION )->SetWindowText( "0.000" );
		theApp.m_fnGetViewPointer()->m_pMap->m_dExclusion = 0.; 
		return;
	}
	m_dWaperExclusion = dExclusion;
	if(theApp.m_fnGetViewPointer()->m_pMap != NULL)
		theApp.m_fnGetViewPointer()->m_pMap->m_dExclusion = dExclusion; 
	t_USER_INFO stUser;
	m_fnSetUserInput(&stUser);
}


void CAutoInspectorDlg::OnBnClickedBtnThickness()
{
/*	if ( m_pTask->stLogin.nLevel == UNKNOWN_USER )
	{
		return;
	}

	if ( m_pTask->stResult.nSRData == 0 ) return;
	if ( !m_bFirstInsp ) return;

	CString strThickness("");
	double  dThickness = 0.0;
	dThickness = m_pTask->stData.m_dAvg[VAL_V] / m_pTask->stData.m_dAvg[VAL_S];
	strThickness.Format( "%.3f", dThickness );
	GetDlgItem( IDC_EDT_REF_THICKNESS )->SetWindowText( strThickness );
*/
}


void CAutoInspectorDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if(bShow)
	{
#if	ELLIPSO	
		if(pWorkRun != nullptr)
			m_ctrlCheckAutoMode.SetCheck(pWorkRun->m_bAutoMode);
#endif
		t_USER_INFO stUser;
		memcpy(&stUser, &m_stUser, sizeof(t_USER_INFO));
// 		theApp.m_fnGetViewPointer()->m_pUserAuto->m_fnGetInputData(stUser);

		CString strData("");
		strData.Format("%s", stUser.szLotID);
		GetDlgItem(IDC_EDT_LOTID)->SetWindowText(strData);

		m_nSelLayer = m_clLayer.GetCurSel();
		theApp.m_fnGetViewPointer()->m_pContour->m_clLayer.SetCurSel(m_nSelLayer);
		m_fnSetView();

	}
}


void CAutoInspectorDlg::OnCbnSelchangeCboWaferNum()
{
	GetDlgItem(IDC_FPSPREAD_RESULT)->SetFocus();
}


void CAutoInspectorDlg::OnCbnSelchangeMeasureItem()
{
	int nNo = m_clMeasureItem.GetCurSel();
	if ( nNo == -1)
	{
		nNo = 1;
		m_clMeasureItem.SetCurSel(nNo);
	}
	CString strData("[ ㎚ ]");
	m_clInfo.SetRow(2);
	m_clInfo.SetCol(8);
	if (nNo == 0)
		m_clInfo.SetText("");
	else
		m_clInfo.SetText(strData);

	m_nMeasureItem = nNo;
	m_fnSetView();
}

void CAutoInspectorDlg::m_fnSetView()
{
	CString strResult("");
	int iMitem = m_nMeasureItem;
	if (iMitem < 2)
	{
		iMitem = 2;
	}

	for (int i = 0; i < m_pTask->stData.m_nDataCnt; i++)
	{
		m_clResult.SetRow(i + 1);
		m_clResult.SetCol(RESULT_MSE);
		strResult.Format("%.3f", m_pTask->stResult.stSRData[i].dwResult[0][0]);
		m_clResult.SetText(strResult);
		m_clResult.SetCol(RESULT_TOTAL_THICKNESS);
		strResult.Format("%.3f", m_pTask->stResult.stSRData[i].dwResult[0][1]);
		m_clResult.SetText(strResult);
		for (int j = 0; j < MAX_LAYER; j++)
		{
			m_clResult.SetCol(RESULT_LAYER_1 + j);
			strResult.Format("%.3f", m_pTask->stResult.stSRData[i].dwResult[j][iMitem]);
			m_clResult.SetText(strResult);
		}
	}

	m_fnSetInspStatus(m_pTask->stData.m_dMax[m_nSelLayer][m_nMeasureItem],
											m_pTask->stData.m_dMin[m_nSelLayer][m_nMeasureItem],
											m_pTask->stData.m_dAvg[m_nSelLayer][m_nMeasureItem],
											m_pTask->stData.m_dStd[m_nSelLayer][m_nMeasureItem],
											m_pTask->stData.m_dMax_Min[m_nSelLayer][m_nMeasureItem],
											m_pTask->stData.m_dUniformity[m_nSelLayer][m_nMeasureItem]);


	theApp.m_fnGetViewPointer()->PostMessage(UM_CHART_VIEW, theApp.m_fnGetViewPointer()->m_nChart, 0);
}

void CAutoInspectorDlg::OnBnClickedBtnRectangleAdd()
{
	CString strData("");
	int     nRow = 0;
	CString strZ("");

	CString strStartX("");
	CString strStartY("");
	CString strEndX("");
	CString strEndY("");
	CString strCountX("");
	CString strCountY("");
	CString strNAME("");

	double dPositionX = 0.0L;
	double dPositionY = 0.0L;
	double dPositionZ = 0.0L;
	double dStartX = 0.0L;
	double dStartY = 0.0L;
	double dEndX = 0.0L;
	double dEndY = 0.0L;
	double dTotalX = 0.0L;
	double dTotalY = 0.0L;
	double dStepX = 0.0L;
	double dStepY = 0.0L;
	int    nCountX = 0;
	int    nCountY = 0;
	int    nMethod = 0;

	int arrWaferInch[4] = { 4,6,8,12 };
	double dMM = 0.0;

	nMethod = m_clMethod.GetCurSel();
	strData.Format("기존 데이터 삭제 후, 검사위치가 재작성됩니다. 등록할까요?");
	int nResult = TECmsgOut(m_Util.CStringTobstr(strData), GREEN_1, 2);
	if (nResult == 1)
	{
		GetDlgItem(IDC_EDT_START_X)->GetWindowText(strStartX);
		dStartX = atof(strStartX);
		GetDlgItem(IDC_EDT_START_Y)->GetWindowText(strStartY);
		dStartY = atof(strStartY);
		GetDlgItem(IDC_EDT_DISTANCE_X)->GetWindowText(strEndX);
		dEndX = atof(strEndX);
		GetDlgItem(IDC_EDT_DISTANCE_Y)->GetWindowText(strEndY);
		dEndY = atof(strEndY);
		GetDlgItem(IDC_EDT_REPEAT_X)->GetWindowText(strCountX);
		nCountX = atoi(strCountX);
		GetDlgItem(IDC_EDT_REPEAT_Y)->GetWindowText(strCountY);
		nCountY = atoi(strCountY);
		if ((dStartX > dEndX) ||
			(dStartY > dEndY))
		{
			strData.Format("시작좌표보다 종료좌표가 작으면 처리불가합니다.");
			m_fnLogWrite(strData);
			TECmsgOut(m_Util.CStringTobstr(strData), GREEN_1, 1);;
			return;
		}

		if ((nCountX > 1) ||
			(nCountY > 1))
		{
// 			nCountX -= 2;
// 			nCountY -= 2;

			if (((nCountX + 2) * (nCountY + 2)) > MAX_DATA)
			{
				strData.Format("등록할 데이터가 MAX_DATA(%01d)를 초과합니다. MAX_DATA 이내로 설정해 주세요", MAX_DATA);
				m_fnLogWrite(strData);
				TECmsgOut(m_Util.CStringTobstr(strData), GREEN_1, 1);;
				return;
			}


			if (m_iSetPntMode != PNT_RECT)
			{
				m_iSetPntMode = PNT_RECT;
// 				m_iRadioMeasureMode = 1;

				m_ctrlPattAlignMode.SetCheck(FALSE);
				m_clModelList.SetCurSel(-1);
				m_clModelList2.SetCurSel(-1);

				m_dWaperThickness = 700.0;
				m_dWaperExclusion = 5.0;
			}

			m_pTask->stResult.nSRData = 0;
			memset(m_pTask->stResult.stSRData, 0x00, sizeof(m_pTask->stResult.stSRData));
			m_fnSRSheetClear();

			m_pTask->stResult.dWaperThickness = m_dWaperThickness;
			strData.Format("%7.3f", m_pTask->stResult.dWaperThickness);
			GetDlgItem(IDC_EDT_THICKNESS)->SetWindowText(strData);

			m_pTask->stResult.dEdgeExclusion = m_dWaperExclusion;
			strData.Format("%7.3f", m_pTask->stResult.dEdgeExclusion);
			GetDlgItem(IDC_EDT_EDGE_EXCLUSION)->SetWindowText(strData);


			dTotalX = (abs)(dEndX - dStartX);
			dTotalY = (abs)(dEndY - dStartY);
			dStepX = dTotalX <= 0 ? 0 : dTotalX / (double)(nCountX-1);
			dStepY = dTotalY <= 0 ? 0 : dTotalY / (double)(nCountY-1);
			int nRow = 1;
			BOOL bToggle = FALSE;
			switch (nMethod)
			{
			case 0: // 0 : X축방향 Z
			{
				for (int j = 0; j < nCountY; j++)
				{
					dPositionY = dStartY + (dStepY * j);
					for (int k = 0; k < nCountX; k++)
					{
						dPositionX = dStartX + (dStepX * k);

						//웨이퍼영역 안에 있는지 확인-rsh
						theApp.m_fnGetViewPointer()->m_pModel->m_fnGetInchToMM(arrWaferInch[m_nWaperSize], dMM, m_dWaperExclusion);
						if (!inCircle(dPositionX, dPositionY, 0.0, 0.0, dMM))
							continue;

						m_clResult.SetRow(nRow);
						m_clResult.SetCol(RESULT_TARGET_NO);
						strNAME.Format("%01d", nRow);
						m_clResult.SetText(strNAME);
						strData.Format("%7.3f", dPositionX);
						m_clResult.SetCol(RESULT_X1_X2);
						m_clResult.SetText(strData);
						strData.Format("%7.3f", dPositionY);
						m_clResult.SetCol(RESULT_Y1_Y2);
						m_clResult.SetText(strData);
						sprintf_s(m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].szName, "%s", strNAME.GetString());
						m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].dwXPosition = dPositionX;
						m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].dwYPosition = dPositionY;
						m_pTask->stResult.nSRData++;
						nRow++;
					}
				}
			}
			break;
			case 1: // 1 : X축방향 ㄹ
			{
				for (int j = 0; j < nCountY; j++)
				{
					dPositionY = dStartY + (dStepY * j);
					for (int k = 0; k < nCountX; k++)
					{
						if (!bToggle)
						{
							dPositionX = dStartX + (dStepX * k);
						}
						else
						{
							dPositionX = dEndX - (dStepX * k);
						}

						//웨이퍼영역 안에 있는지 확인-rsh
						theApp.m_fnGetViewPointer()->m_pModel->m_fnGetInchToMM(arrWaferInch[m_nWaperSize], dMM, m_dWaperExclusion);
						if (!inCircle(dPositionX, dPositionY, 0.0, 0.0, dMM))
							continue;

						m_clResult.SetRow(nRow);
						m_clResult.SetCol(RESULT_TARGET_NO);
						strNAME.Format("%01d", nRow);
						m_clResult.SetText(strNAME);
						strData.Format("%7.3f", dPositionX);
						m_clResult.SetCol(RESULT_X1_X2);
						m_clResult.SetText(strData);
						strData.Format("%7.3f", dPositionY);
						m_clResult.SetCol(RESULT_Y1_Y2);
						m_clResult.SetText(strData);
						sprintf_s(m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].szName, "%s", strNAME.GetString());
						m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].dwXPosition = dPositionX;
						m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].dwYPosition = dPositionY;
						m_pTask->stResult.nSRData++;
						nRow++;
					}
					bToggle = !bToggle;
				}
			}
			break;
			case 2: // 2 : Y축방향 Z
			{
				for (int j = 0; j < nCountX; j++)
				{
					dPositionX = dStartX + (dStepX * j);
					for (int k = 0; k < nCountY; k++)
					{
						dPositionY = dStartY + (dStepY * k);

						//웨이퍼영역 안에 있는지 확인-rsh
						theApp.m_fnGetViewPointer()->m_pModel->m_fnGetInchToMM(arrWaferInch[m_nWaperSize], dMM, m_dWaperExclusion);
						if (!inCircle(dPositionX, dPositionY, 0.0, 0.0, dMM))
							continue;

						m_clResult.SetRow(nRow);
						m_clResult.SetCol(RESULT_TARGET_NO);
						strNAME.Format("%01d", nRow);
						m_clResult.SetText(strNAME);
						strData.Format("%7.3f", dPositionX);
						m_clResult.SetCol(RESULT_X1_X2);
						m_clResult.SetText(strData);
						strData.Format("%7.3f", dPositionY);
						m_clResult.SetCol(RESULT_Y1_Y2);
						m_clResult.SetText(strData);
						sprintf_s(m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].szName, "%s", strNAME.GetString());
						m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].dwXPosition = dPositionX;
						m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].dwYPosition = dPositionY;
						m_pTask->stResult.nSRData++;
						nRow++;
					}
				}
			}
			break;
			case 3: // 3 : Y축방향 ㄹ
			{
				for (int j = 0; j < nCountX; j++)
				{
					dPositionX = dStartX + (dStepX * j);
					for (int k = 0; k < nCountY; k++)
					{
						if (!bToggle)
						{
							dPositionY = dStartY + (dStepY * k);
						}
						else
						{
							dPositionY = dEndY - (dStepY * k);
						}

						//웨이퍼영역 안에 있는지 확인-rsh
						theApp.m_fnGetViewPointer()->m_pModel->m_fnGetInchToMM(arrWaferInch[m_nWaperSize], dMM, m_dWaperExclusion);
						if (!inCircle(dPositionX, dPositionY, 0.0, 0.0, dMM))
							continue;

						m_clResult.SetRow(nRow);
						m_clResult.SetCol(RESULT_TARGET_NO);
						strNAME.Format("%01d", nRow);
						m_clResult.SetText(strNAME);
						strData.Format("%7.3f", dPositionX);
						m_clResult.SetCol(RESULT_X1_X2);
						m_clResult.SetText(strData);
						strData.Format("%7.3f", dPositionY);
						m_clResult.SetCol(RESULT_Y1_Y2);
						m_clResult.SetText(strData);
						sprintf_s(m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].szName, "%s", strNAME.GetString());
						m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].dwXPosition = dPositionX;
						m_pTask->stResult.stSRData[m_pTask->stResult.nSRData].dwYPosition = dPositionY;
						m_pTask->stResult.nSRData++;
						nRow++;
					}
					bToggle = !bToggle;
				}
			}
			break;
			default:
			{
				strData = "등간격설정 : 등간격 지정 방법을 설정해 주세요";
				m_fnLogWrite(strData);
				TECmsgOut(m_Util.CStringTobstr(strData), GREEN_1, 1);;

			}
			}
		}
		else
		{
			strData = "등간격설정 : 분할횟수를 '2' 이상으로 설정해 주세요";
			m_fnLogWrite(strData);
			TECmsgOut(m_Util.CStringTobstr(strData), GREEN_1, 1);;
			return;
		}
	}

	SendMessage(WM_COMMAND, IDC_RADIO_MEASURE_MODE2, 0);
}


void CAutoInspectorDlg::OnCbnDropdownComboReceipe()
{
	theApp.m_fnGetViewPointer()->m_pManual->m_pElipso->m_fnGetListRecipes();

	thread thrdReadReceipe = thread(ThrdReadReceipeList, this);
	thrdReadReceipe.detach();
}

void ThrdReadReceipeList(LPVOID pFuncData)
{
	CAutoInspectorDlg		*pQI = (CAutoInspectorDlg *)pFuncData;

	t_GET_RECIPE stTempRcpList;

	int tmpStep = 10;
	BOOL bTmpReadRecipe = TRUE;

	stTempRcpList.bComplete = stTempRcpList.bError = FALSE;

	while (bTmpReadRecipe)
	{
		Sleep(200);

		switch (tmpStep)
		{
		case 10://reciepe load
			theApp.m_fnGetViewPointer()->m_pManual->m_pElipso->m_fnGetRecipeList(stTempRcpList);
			if (!stTempRcpList.bComplete)
				break;

			tmpStep++;
			break;

		case 11:
			if (stTempRcpList.bError)
			{
				TECmsgOut(L"Receipe리스트를 읽어오지 못했습니다. 경로를 확인해 주세요", LIGHTMAGENTA, 1);
				bTmpReadRecipe = FALSE;
				break;
			}
			else
			{
				pQI->m_ctrlComboReceipe.ResetContent();
				for (int i = 0; i < stTempRcpList.nRecipeCnt; i++)
				{
					pQI->m_ctrlComboReceipe.AddString(stTempRcpList.szRecipeList[i]);
				}
			}

			tmpStep = 20;
			break;


		default:
			bTmpReadRecipe = FALSE;
			break;

		}
	}
}



void CAutoInspectorDlg::OnCbnSelchangeComboReceipe()
{
	int nNo = m_ctrlComboReceipe.GetCurSel();
#if	ELLIPSO	
	m_ctrlComboReceipe.GetLBText(nNo, pWorkRun->m_strRecipeName);

	sprintf_s(m_stUser.szRecipeFileName, "%s", pWorkRun->m_strRecipeName.GetString());


	thread thrdResult = thread(ThrdLoadRecipe);
	thrdResult.detach();

// 	theApp.m_fnGetViewPointer()->m_pManual->m_pElipso->m_fnSetReadyEllipso();
// 
// 	t_ELIPSO_RESULT_INFO stcResultReady;
// 	while (1)
// 	{
// 		Sleep(10);
// 		theApp.m_fnGetViewPointer()->m_pManual->m_pElipso->m_fnGetResult(stcResultReady);
// 		if (stcResultReady.bComplete)
// 		{
// 			if (stcResultReady.bError)
// 			{
// 				TECmsgOut(L"Recipe Load를 완료하지 못했습니다. 연결상태를 확인하세요.", LIGHTMAGENTA, 1);
// 			}
// 			break;
// 		}
// 	}			

#endif
}


void CAutoInspectorDlg::OnBnClickedCheckAutomode()
{
#if	ELLIPSO	
	CString strTemp;
	pWorkRun->m_bAutoMode = m_ctrlCheckAutoMode.GetCheck();
	if (pWorkRun->m_bAutoMode)
	{
		strTemp.Format("자동투입");
		m_ctrlCheckManualMeasure.EnableWindow(FALSE);
	}
	else
	{
		strTemp.Format("수동투입");
		m_ctrlEditWaferNum.SetWindowTextA("0");
		m_ctrlCheckManualMeasure.EnableWindow(TRUE);
	}

	m_ctrlCheckAutoMode.SetWindowText(strTemp);

#endif
}

UINT ExcelWriteThread(LPVOID pFuncData)
{
	CAutoInspectorDlg		*pQI = (CAutoInspectorDlg *)pFuncData;
	int                     nLayer			= 0;
	int                     nMeasureItem	= 0;
	int                     nLevel			= 0;
	int                     nChart          = 0;
	CString                 strAxisImage("");
	CString                 str2DImage("");
	CString                 str3DImage("");
	BOOL                    bProcess        = FALSE;
// 	if (!pQI->m_pTask->stCommon.bChartReport)
// 	{
// 		pQI->m_nExcelWriteStep = 200;
// 	}

	while (pQI->m_bExcelWriteEnd)
	{
		if (pQI->m_hWnd == NULL) continue;
		if (bProcess) continue;
		bProcess = TRUE;
		switch (pQI->m_nExcelWriteStep)
		{
			case 10:
				{
					TECmsgOut(L"Excel 레포트를 위한 차트생성 작업을 시작합니다.", GREEN_1, 100);
					nMeasureItem        = pQI->m_nMeasureItem;
					nLayer              = pQI->m_nSelLayer;
					nChart				= theApp.m_fnGetViewPointer()->m_nChart;
					pQI->m_nSelLayer    = 0;
					pQI->m_nMeasureItem = 0;
					pQI->m_nExcelWriteStep  = 20;
				}
				break;
			case 20:
				{
					if (pQI->m_nSelLayer == MAX_LAYER)
					{
						theApp.m_fnGetViewPointer()->m_nChart++;
						pQI->m_nSelLayer = 0;
						pQI->m_nMeasureItem = 0;
					}
					if (pQI->m_nMeasureItem == MAX_ITEM)
					{
						pQI->m_nSelLayer++;
						pQI->m_nMeasureItem = 0;
					}
					pQI->m_nExcelWriteStep     = 30;
				}
				break;
			case 30:
				{
					if (theApp.m_fnGetViewPointer()->m_nChart == 0)
					{
						theApp.m_fnGetViewPointer()->PostMessage(UM_CHART_VIEW, SW_MAP);
						pQI->m_nExcelWriteStep = 40;
					}

					if (pQI->m_pTask->stCommon.bChartReport)
					{
						if (theApp.m_fnGetViewPointer()->m_nChart == 1)
						{
							theApp.m_fnGetViewPointer()->PostMessage(UM_CHART_VIEW, SW_2D);
							pQI->m_nExcelWriteStep = 60;
						}
						if (theApp.m_fnGetViewPointer()->m_nChart == 2)
						{
							theApp.m_fnGetViewPointer()->PostMessage(UM_CHART_VIEW, SW_3D);
							pQI->m_nExcelWriteStep = 80;
						}
					}
					else
					{
						if (theApp.m_fnGetViewPointer()->m_nChart == 1)
							theApp.m_fnGetViewPointer()->m_nChart = 3;
					}


					if (theApp.m_fnGetViewPointer()->m_nChart == 3)
					{
						theApp.m_fnGetViewPointer()->m_pMap->m_bView = FALSE;
						pQI->m_nMeasureItem = nMeasureItem;
						pQI->m_nSelLayer    = nLayer;
						theApp.m_fnGetViewPointer()->PostMessage(UM_CHART_VIEW, nChart);
						pQI->m_nExcelWriteStep = 200;
					}
				}
				break;
			case 40:
				{
					theApp.m_fnGetViewPointer()->m_pMap->m_bView = TRUE;
					theApp.m_fnGetViewPointer()->m_pMap->m_bSave = TRUE;
					theApp.m_fnGetViewPointer()->m_pMap->SendMessage(UM_MAP_DRAW, 0, 0);
					pQI->m_nExcelWriteStep               = 50;
				}
				break;
			case 50:
				{
					if (!theApp.m_fnGetViewPointer()->m_pMap->m_bSave)
					{
						strAxisImage.Format("AXIS_%01d_%01d.bmp", pQI->m_nSelLayer + 1, pQI->m_nMeasureItem + 1);
						theApp.m_fnGetViewPointer()->m_pMap->m_fnSetContourChartToImage(pQI->m_fnGetTempPath() + strAxisImage);
						theApp.m_fnGetViewPointer()->m_pMap->m_strImageInfo = pQI->m_fnGetTempPath() + strAxisImage;
						pQI->m_nMeasureItem++;
					}
					pQI->m_nExcelWriteStep = 20;
				}
				break;
			case 60:
				{
					theApp.m_fnGetViewPointer()->m_pContour->m_bSave = TRUE;
					pQI->m_nExcelWriteStep = 70;
				}
				break;
			case 70:
				{
					if (!theApp.m_fnGetViewPointer()->m_pContour->m_bSave)
					{
						str2DImage.Format("2D_%01d_%01d.bmp", pQI->m_nSelLayer + 1, pQI->m_nMeasureItem + 1);
						theApp.m_fnGetViewPointer()->m_pContour->m_fnSetContourChartToImage(FALSE, pQI->m_fnGetTempPath() + str2DImage);
						pQI->m_nMeasureItem++;
						pQI->m_nExcelWriteStep = 20;
					}
				}
				break;
			case 80:
				{
					theApp.m_fnGetViewPointer()->m_pContour->m_bSave = TRUE;
					pQI->m_nExcelWriteStep = 90;
				}
				break;
			case 90:
				{
					if (!theApp.m_fnGetViewPointer()->m_pContour->m_bSave)
					{
						str3DImage.Format("3D_%01d_%01d.bmp", pQI->m_nSelLayer + 1, pQI->m_nMeasureItem + 1);
						theApp.m_fnGetViewPointer()->m_pContour->m_fnSetContourChartToImage(TRUE, pQI->m_fnGetTempPath() + str3DImage);
						pQI->m_nMeasureItem++;
						pQI->m_nExcelWriteStep = 20;
					}
				}
				break;
			case 200:
				{
					theApp.m_fnGetViewPointer()->m_pMap->m_bView = FALSE;
					pQI->PostMessageA(UM_REPORT, 0, 0);
					pQI->m_nExcelWriteStep = 300;
				}
				break;
			case 300:
				{
					pQI->m_nExcelWriteStep = -1;
					pQI->m_bExcelWriteEnd = FALSE;
					TECmsgOut(L"Excel 레포트를 위한 차트생성 작업을 종료합니다.", GREEN_1, 100);
				}
				break;
		}
		Sleep(5);
		bProcess = FALSE;
	}
	if (pQI->m_hWnd == NULL) return 0;

	if (pQI->m_hExcelWriteThread != NULL)
	{
		SuspendThread(pQI->m_hExcelWriteThread);
		CloseHandle(pQI->m_hExcelWriteThread);
		pQI->m_hExcelWriteThread = NULL;
	}
return 0;
}


void CAutoInspectorDlg::OnBnClickedCheckPattMode()
{
	m_bPattAlignMode = m_ctrlPattAlignMode.GetCheck();
	m_clModelList2.EnableWindow(m_bPattAlignMode);
// 	m_fnGetModelList();

	if(m_bPattAlignMode)
	{
		m_iSetPntMode = PNT_PATT;
// 		m_iRadioMeasureMode = 2;

		m_ctrlChkUseAF.SetCheck(TRUE);
		m_ctrlCheckWaferAlign.SetCheck(FALSE);
		pWorkRun->m_bWaferAlign = FALSE;
	}
	else
	{
		m_iSetPntMode = -1;
// 		m_iRadioMeasureMode = -1;

		m_clModelList.SetCurSel(-1);
		m_clModelList2.SetCurSel(-1);
		m_clMethod.SetCurSel(-1);
	}

	SendMessage(WM_COMMAND, IDC_RADIO_MEASURE_MODE3, 0);
}


void CAutoInspectorDlg::OnCbnSelchangeCboModelList2()
{
#if !_DEBUG
	if (m_pTask->stLogin.nLevel == UNKNOWN_USER)
	{
		return;
	}
#endif
	if (m_pTask->stEqp.bIoStartButton)
	{
		m_fnLogWrite("Start 버튼이 ON시에는 측정 관리 메뉴를 사용할수 없습니다.");
		return;
	}

	m_bFirstInsp = FALSE;

	CString strModel("");
	CString strData("");
	CString strMSG("");
	int nDataCnt = 0;
	int nRow = 0;
	int nDataTotal = 0;
	int nNo = m_clModelList2.GetCurSel();
	if (nNo == -1)
	{
		AfxMessageBox("모델을 선택해 주세요.");
		return;
	}
	m_clModelList2.GetLBText(nNo, strModel);
	strModel.Trim();
	if (!strModel.IsEmpty())
	{
		theApp.m_fnGetViewPointer()->m_pModel->m_fnGetCurrModel_Load(strModel);

		memcpy(&m_pTask->stSub, &m_pTask->stCurrModel, sizeof(t_MODEL_INFO));
	}
}



void CAutoInspectorDlg::OnBnClickedBtnInitStage()
{
}


void CAutoInspectorDlg::OnBnClickedButton3()
{
#if MIL_USE
	if (pWorkRun->m_bThrdFlag[TF_AUTORUN])
		return;

	pWorkRun->m_bThrdFlag[TF_AUTORUN] = TRUE;
	//OUT6 ON
	theApp.m_fnGetViewPointer()->m_pIO->m_fnSetOutputSignal(MEASURE_READY, FALSE);
	//OUT7 OFF
	theApp.m_fnGetViewPointer()->m_pIO->m_fnSetOutputSignal(MEASURE_BUSY, TRUE);

	TECmsgOut(L"스테이지 위치를 이동 합니다.", YELLOW, 1000, 17, BLACK);
	pWorkRun->SqncMoveSupplyPosition();

	if (pWorkRun->m_bAutoMode)//자동
	{
		double dWaferLineMotorPos[2];
		pAxis[X_AXIS]->StatusGetPos(&dWaferLineMotorPos[0]);
		pAxis[Y_AXIS]->StatusGetPos(&dWaferLineMotorPos[1]);
		if (fabs(dWaferLineMotorPos[0] + pAxis[X_AXIS]->m_dOffsetPos - theApp.m_fnGetViewPointer()->m_pTask->stCommon.dLoadingX) < 1.0
			&& fabs(dWaferLineMotorPos[1] + pAxis[Y_AXIS]->m_dOffsetPos - theApp.m_fnGetViewPointer()->m_pTask->stCommon.dLoadingY) < 1.0)
		{
			//OUT6 ON
			theApp.m_fnGetViewPointer()->m_pIO->m_fnSetOutputSignal(MEASURE_READY, TRUE);
			//OUT7 OFF
			theApp.m_fnGetViewPointer()->m_pIO->m_fnSetOutputSignal(MEASURE_BUSY, FALSE);
		}
		else
		{
			TECmsgOut(L"Stage가 지정된 위치로 이동하지 못했습니다. X,Y Stage 상태를 확인하고 다시 시작해 주세요.", LIGHTMAGENTA, 1);
		}
	}
	else
	{
		//OUT6 ON
		theApp.m_fnGetViewPointer()->m_pIO->m_fnSetOutputSignal(MEASURE_READY, FALSE);
		//OUT7 OFF
		theApp.m_fnGetViewPointer()->m_pIO->m_fnSetOutputSignal(MEASURE_BUSY, FALSE);
	}

	theApp.m_fnGetViewPointer()->m_pTask->stElipsoIO.stOut.Vacuum_On_06_Inch = FALSE;
	theApp.m_fnGetViewPointer()->m_pTask->stElipsoIO.stOut.Vacuum_On_08_Inch = FALSE;
	theApp.m_fnGetViewPointer()->m_pTask->stElipsoIO.stOut.Vacuum_On_12_Inch = FALSE;
	theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxdoWriteOutportBit(ELIPSO_IO_OUTPUT_1, VACUUM_ON_06_INCH, theApp.m_fnGetViewPointer()->m_pTask->stElipsoIO.stOut.Vacuum_On_06_Inch);
	theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxdoWriteOutportBit(ELIPSO_IO_OUTPUT_1, VACUUM_ON_08_INCH, theApp.m_fnGetViewPointer()->m_pTask->stElipsoIO.stOut.Vacuum_On_08_Inch);
	theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxdoWriteOutportBit(ELIPSO_IO_OUTPUT_1, VACUUM_ON_12_INCH, theApp.m_fnGetViewPointer()->m_pTask->stElipsoIO.stOut.Vacuum_On_12_Inch);
	theApp.m_fnGetViewPointer()->SendMessageA(UM_VAC_STATUS, 0, 0);


	TECmsgOut(L"스테이지 초기화 완료", LIGHTBLUE, 1);
	pWorkRun->m_bThrdFlag[TF_AUTORUN] = FALSE;
#endif
}


void CAutoInspectorDlg::OnBnClickedBtnPoint5()
{
	DivisionCirclePos(0);
	SendMessage(WM_COMMAND, IDC_RADIO_MEASURE_MODE, IDC_RADIO_MEASURE_MODE);
}


void CAutoInspectorDlg::OnBnClickedCheckWaferAlign()
{
#if MIL_USE
	pWorkRun->m_bWaferAlign = m_ctrlCheckWaferAlign.GetCheck();
#endif
}


void CAutoInspectorDlg::OnBnClickedCheckAutomode2()
{
	CString strTemp;
// 	pWorkRun->m_bAutoMode2 = m_ctrlCheckAutoMode2.GetCheck();
// 	if (pWorkRun->m_bAutoMode2)
// 		strTemp.Format("자동측정");
// 	else
// 	{
// 		strTemp.Format("수동측정");
// 	}

	m_ctrlCheckAutoMode2.SetWindowText(strTemp);
}


void CAutoInspectorDlg::OnBnClickedButtonDataInit()
{
	m_fnSRSheetClear();
	memset(&theApp.m_fnGetViewPointer()->m_pTask->stData, 0x00, sizeof(t_RESULT_INFO));
	theApp.m_fnGetViewPointer()->m_pAuto->m_clResult.SetRow(1);
	theApp.m_fnGetViewPointer()->m_pAuto->m_clResult.SetCol(1);

	m_strInspDate = m_Util.m_fnGetDate();
	sprintf_s(m_stUser.szDate, "%s", m_strInspDate.GetString());
	m_strLotStartTime = m_Util.m_fnGetTime24();

	m_strWaferTimeStart = m_tiAWafer.GetStrTime(m_tiAWafer.SetTimeStart());
	sprintf_s(m_stUser.szTimeStart, "%s", m_strWaferTimeStart.GetString());

	pWorkRun->m_bInitManualData = TRUE;
}


void CAutoInspectorDlg::OnBnClickedButtonAutoFocus()
{
#if MIL_USE
	if (pWorkRun->m_bThrdFlag[TF_AUTORUN])
		return;
	if (pWorkRun->m_bThrdFlag[TF_AUTOFOCUS])
		return;

	pWorkRun->m_bThrdFlag[TF_AUTORUN] = TRUE;
	//OUT6 ON
	theApp.m_fnGetViewPointer()->m_pIO->m_fnSetOutputSignal(MEASURE_READY, FALSE);
	//OUT7 OFF
	theApp.m_fnGetViewPointer()->m_pIO->m_fnSetOutputSignal(MEASURE_BUSY, TRUE);

	TECmsgOut(L"스테이지 위치를 이동 합니다.", YELLOW, 1000, 17, BLACK);

	pAxis[X_AXIS]->fVelocity = (float)theApp.m_fnGetViewPointer()->m_pTask->stCommon.dInspXYSpeed*0.1f;
	pAxis[Y_AXIS]->fVelocity = (float)theApp.m_fnGetViewPointer()->m_pTask->stCommon.dInspXYSpeed*0.1f;
	pAxis[Z_AXIS]->fVelocity = (float)theApp.m_fnGetViewPointer()->m_pTask->stCommon.dInspZSpeed;
	
	pAxis[X_AXIS]->MoveAxis(0, 0, TRUE, FALSE);
	pAxis[Y_AXIS]->MoveAxis(0, 0, TRUE, FALSE);

	double dMovePos = (float)theApp.m_fnGetViewPointer()->m_pModel->m_fnGetAlignZ(m_dWaperThickness);
	pAxis[Z_AXIS]->MoveAxis(dMovePos, 0, TRUE);


	while (pAxis[X_AXIS]->CheckInMotion() || pAxis[Y_AXIS]->CheckInMotion())
	{
		Sleep(100);
	}


// 	thread thrdStartAuto = thread(ThrdAF, m_ctrlChkUseAF.GetCheck());
// 	thrdStartAuto.detach();
	
	pWorkRun->m_bThrdFlag[TF_AUTORUN] = FALSE;
#endif
}


void CAutoInspectorDlg::OnBnClickedButtonManualMeasure()
{
#if MIL_USE
	thread thrdManualMeasure = thread(ThrdManualMeasure);
	thrdManualMeasure.detach();
#endif
}


void CAutoInspectorDlg::OnBnClickedCheckManualMeasure()
{
	if (m_ctrlCheckManualMeasure.GetCheck())
	{
		m_fnSetBtnEnable(2);

		m_iSetPntMode = -1;

		m_strInspDate = m_Util.m_fnGetDate();
		sprintf_s(m_stUser.szDate, "%s", m_strInspDate.GetString());
		m_strLotStartTime = m_Util.m_fnGetTime24();

		m_strWaferTimeStart = m_tiAWafer.GetStrTime(m_tiAWafer.SetTimeStart());
		sprintf_s(m_stUser.szTimeStart, "%s", m_strWaferTimeStart.GetString());

		pWorkRun->m_bInitManualData = TRUE;
	} 
	else
	{
		m_fnSetBtnEnable(1);

		m_fnSRSheetClear();
		memset(&theApp.m_fnGetViewPointer()->m_pTask->stData, 0x00, sizeof(t_RESULT_INFO));
		theApp.m_fnGetViewPointer()->m_pAuto->m_clResult.SetRow(1);
		theApp.m_fnGetViewPointer()->m_pAuto->m_clResult.SetCol(1);
	}
}


void CAutoInspectorDlg::OnCbnSelchangeCboLayer2()
{
// 	if (theApp.m_fnGetViewPointer()->m_nPage == SW_AUTO)
	{
		m_nSelLayer = m_clLayer.GetCurSel();
		theApp.m_fnGetViewPointer()->m_pContour->m_clLayer.SetCurSel(m_nSelLayer);
		m_fnSetView();
	}
}


void CAutoInspectorDlg::OnBnClickedCheckUseAf()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CAutoInspectorDlg::OnBnClickedRadioMeasureMode(UINT wParam)
{
	GetDlgItem(IDC_BTN_POINT_5)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_POINT_4)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_POINT_12)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_POINT_28)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_POINT_60)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_POINT_124)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_POINT_252)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_POINT_508)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHK_ARRAW)->EnableWindow(FALSE);

	GetDlgItem(IDC_EDT_START_X)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDT_START_Y)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDT_DISTANCE_X)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDT_DISTANCE_Y)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDT_REPEAT_X)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDT_REPEAT_Y)->EnableWindow(FALSE);
	GetDlgItem(IDC_CBO_METHOD)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_RECTANGLE_ADD)->EnableWindow(FALSE);

	GetDlgItem(IDC_CBO_MODEL_LIST)->EnableWindow(FALSE);
	GetDlgItem(IDC_CBO_MODEL_LIST2)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_PATT_MODE)->EnableWindow(FALSE);
	m_bPattAlignMode = FALSE;

	
	switch (wParam)
	{
	case IDC_RADIO_MEASURE_MODE:
		GetDlgItem(IDC_BTN_POINT_5)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_POINT_4)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_POINT_12)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_POINT_28)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_POINT_60)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_POINT_124)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_POINT_252)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_POINT_508)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHK_ARRAW)->EnableWindow(TRUE);
		m_iRadioMeasureMode = 0;
		break;
	case IDC_RADIO_MEASURE_MODE2:
		GetDlgItem(IDC_EDT_START_X)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDT_START_Y)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDT_DISTANCE_X)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDT_DISTANCE_Y)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDT_REPEAT_X)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDT_REPEAT_Y)->EnableWindow(TRUE);
		GetDlgItem(IDC_CBO_METHOD)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_RECTANGLE_ADD)->EnableWindow(TRUE);
		m_iRadioMeasureMode = 1;
		break;
	case IDC_RADIO_MEASURE_MODE3:
		m_bPattAlignMode = m_ctrlPattAlignMode.GetCheck();
		if (!m_bPattAlignMode)
			GetDlgItem(IDC_CBO_MODEL_LIST)->EnableWindow(TRUE);
// 		GetDlgItem(IDC_CBO_MODEL_LIST2)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_PATT_MODE)->EnableWindow(TRUE);
		m_iRadioMeasureMode = 2;
		break;
	default:
		break;
	}

	UpdateData(FALSE);

	m_iRadioMeasureMode_old = m_iRadioMeasureMode;

}


void CAutoInspectorDlg::OnBnClickedButtonAutoFocusVision()
{
	if (pWorkRun->m_bThrdFlag[TF_AUTORUN])
		return;

	if (!theApp.m_fnGetViewPointer()->m_pStageMap->m_bCamera)
	{
		((CButton*)theApp.m_fnGetViewPointer()->m_pStageMap->GetDlgItem(IDC_CHK_CAMERA))->SetCheck(TRUE);
		theApp.m_fnGetViewPointer()->m_pStageMap->OnBnClickedChkCamera();
	}
#if MIL_USE
	m_pTask->stCurrModel.dWaperThickness = m_dWaperThickness;
	m_pTask->stCurrModel.dEdgeExclusion = m_dWaperExclusion;

	thread thrdStartAuto = thread(ThrdAF, TRUE);
	thrdStartAuto.detach();
#endif
	return;
}


void CAutoInspectorDlg::OnBnClickedButtonRandom()
{
	if (m_pTask->stResult.nSRData == 0) return;

	int nLoopCnt = 0;
	CString strUSE("");
	CString strNAME("");
	CString strX("");
	CString strY("");
	double  dResult = 0.0;
	CString strResult("");
	float fStd[MAX_LAYER][MAX_ITEM];
	memset(fStd, 0x00, sizeof(fStd));

	m_pTask->stData.m_nDataCnt = 0;
	nLoopCnt = 0;
	m_strTactTime = "";
	m_strInspTime = "";

	m_strInspTime = m_Util.m_fnGetCurrentTime1();
	m_strInspDate = m_Util.m_fnGetCurrentTime6();
	m_strLotStartTime = m_Util.m_fnGetTime24();
	t_USER_INFO stUser;
	m_fnSetUserInput(&stUser);

	sprintf_s(m_stUser.szDate, "%s", m_Util.m_fnGetDate().GetString());
	sprintf_s(m_stUser.szTime, "%s", m_Util.m_fnGetTime().GetString());
	memset(m_pTask->stData.m_dDataX, 0x00, sizeof(m_pTask->stData.m_dDataX));
	memset(m_pTask->stData.m_dDataY, 0x00, sizeof(m_pTask->stData.m_dDataY));
	memset(m_pTask->stData.m_dDataZ, 0x00, sizeof(m_pTask->stData.m_dDataZ));
	memset(m_pTask->stData.m_dDataH, 0x00, sizeof(m_pTask->stData.m_dDataH));
	memset(m_pTask->stData.m_dDataL, 0x00, sizeof(m_pTask->stData.m_dDataL));
// 	m_bThTact = TRUE;
// 	CThTact.Start();
	for (int i = 0; i < MAX_DATA; i++)
	{
		m_clResult.SetRow(i + 1);

		m_clResult.SetCol(RESULT_TARGET_NO);
		strNAME = m_clResult.GetText();

		m_clResult.SetCol(RESULT_X1_X2);
		strX = m_clResult.GetText();

		m_clResult.SetCol(RESULT_Y1_Y2);
		strY = m_clResult.GetText();

		strNAME.Trim();
		strX.Trim();
		strY.Trim();
		strNAME.Trim();
		if ((!strX.IsEmpty()) &&
			(!strY.IsEmpty()) &&
			(!strNAME.IsEmpty()))
		{
			m_pTask->stData.m_dDataL[m_pTask->stData.m_nDataCnt] = m_pTask->stData.m_nDataCnt;
			sprintf_s(m_pTask->stResult.stSRData[m_pTask->stData.m_nDataCnt].szName, "%s", strNAME.GetString());

			m_pTask->stResult.stSRData[m_pTask->stData.m_nDataCnt].dwXPosition = atof(strX);
			m_pTask->stData.m_dDataX[m_pTask->stData.m_nDataCnt] = atof(strX);

			m_pTask->stResult.stSRData[m_pTask->stData.m_nDataCnt].dwYPosition = atof(strY);
			m_pTask->stData.m_dDataY[m_pTask->stData.m_nDataCnt] = atof(strY);

			m_pTask->stData.m_nDataCnt++;
		}
	}
	m_pTask->stResult.nSRData = m_pTask->stData.m_nDataCnt;
	m_pTask->stResult.dSRZ = theApp.m_fnGetViewPointer()->m_pModel->m_fnGetMeasureZ(m_dWaperThickness);
	m_pTask->stResult.dSRSafetyZ = theApp.m_fnGetViewPointer()->m_pModel->m_fnGetMeasureSafetyZ(m_dWaperThickness);
	m_pTask->stResult.dAlignZ = theApp.m_fnGetViewPointer()->m_pModel->m_fnGetAlignZ(m_dWaperThickness);
	m_pTask->stResult.dAlignSafetyZ = theApp.m_fnGetViewPointer()->m_pModel->m_fnGetAlignSafetyZ(m_dWaperThickness);
	m_pTask->stResult.dAlignX = m_pTask->stCommon.dAlignX[m_bWaperType][m_nWaperSize];
	m_pTask->stResult.dAlignY = m_pTask->stCommon.dAlignY[m_bWaperType][m_nWaperSize];
	m_pTask->stResult.dSRSpeed = m_pTask->stCommon.dInspXYSpeed;

	nLoopCnt = 0;
	for (int i = 0; i < m_pTask->stData.m_nDataCnt; i++)
	{
		for (int j = 0; j < MAX_LAYER; j++)
		{
			for (int k = 0; k < MAX_ITEM; k++)
			{
				dResult = m_Util.m_fnGetRangeRandom(0.1, 1.5);
				m_pTask->stData.m_dDataZ[nLoopCnt][j][k] = dResult;
				m_pTask->stResult.stSRData[nLoopCnt].dwResult[j][k] = dResult;

				if (nLoopCnt == 0)
				{
					m_pTask->stData.m_dMax[j][k] = dResult;
					m_pTask->stData.m_dMin[j][k] = dResult;
					m_pTask->stData.m_dAvg[j][k] = dResult;
					fStd[j][k] = 0;
					m_pTask->stData.m_fTot[j][k] = 0.0f;
				}
				if (m_pTask->stData.m_dMax[j][k] < dResult)
				{
					m_pTask->stData.m_dMax[j][k] = dResult;
				}
				if (m_pTask->stData.m_dMin[j][k] > dResult)
				{
					m_pTask->stData.m_dMin[j][k] = dResult;
				}

				m_pTask->stData.m_fTot[j][k] += (float)dResult;
				m_pTask->stData.m_dAvg[j][k] = (double)(m_pTask->stData.m_fTot[j][k] / (float)(nLoopCnt + 1));
				m_pTask->stData.m_dMax_Min[j][k] = abs(m_pTask->stData.m_dMax[j][k] - m_pTask->stData.m_dMin[j][k]);
				fStd[j][k] += (float)pow(dResult - m_pTask->stData.m_dAvg[j][k], 2);
				m_pTask->stData.m_dStd[j][k] = sqrt((double)(fStd[j][k] / nLoopCnt + 1));
				m_pTask->stData.m_dUniformity[j][k] = (m_pTask->stData.m_dStd[j][k] / m_pTask->stData.m_dAvg[j][k]);
				m_pTask->stData.m_dContourMax[j][k] = m_pTask->stData.m_dMax[j][k];
				m_pTask->stData.m_dContourMin[j][k] = m_pTask->stData.m_dMin[j][k];
				m_pTask->stData.m_dContourStep[j][k] = m_pTask->stData.m_dMax_Min[j][k] / 10.;
			}
		}
		nLoopCnt++;
	}

	int iMitem = m_nMeasureItem;
	if (iMitem < 2)
	{
		iMitem = 2;
	}
	for (int i = 0; i < m_pTask->stData.m_nDataCnt; i++)
	{
		m_clResult.SetRow(i + 1);
		m_clResult.SetCol(RESULT_MSE);
		strResult.Format("%.3f", m_pTask->stResult.stSRData[i].dwResult[0][0]);
		m_clResult.SetText(strResult);
		m_clResult.SetCol(RESULT_TOTAL_THICKNESS);
		strResult.Format("%.3f", m_pTask->stResult.stSRData[i].dwResult[0][1]);
		m_clResult.SetText(strResult);
		for (int j = 0; j < MAX_LAYER; j++)
		{
			m_clResult.SetCol(RESULT_LAYER_1 + j);
			strResult.Format("%.3f", m_pTask->stResult.stSRData[i].dwResult[j][iMitem]);
			m_clResult.SetText(strResult);
		}
	}



	m_pTask->stResult.nLayer = MAX_LAYER;

	if (m_pTask->stData.m_nDataCnt == 0) return;

	m_fnSetInspStatus(m_pTask->stData.m_dMax[m_nSelLayer][iMitem],
		m_pTask->stData.m_dMin[m_nSelLayer][iMitem],
		m_pTask->stData.m_dAvg[m_nSelLayer][iMitem],
		m_pTask->stData.m_dStd[m_nSelLayer][iMitem],
		m_pTask->stData.m_dMax_Min[m_nSelLayer][iMitem],
		m_pTask->stData.m_dUniformity[m_nSelLayer][iMitem]);

	theApp.m_fnGetViewPointer()->m_pContour->m_fnSetUpdateData();

// 	if (m_bThTact)
// 	{
// 		CThTact.End();
// 	}
// 	m_fnSetDebugThTactTime();
// 	m_bFirstInsp = TRUE;
// 	this->PostMessageA(UM_REPORT_READY, 0, 0);
}
