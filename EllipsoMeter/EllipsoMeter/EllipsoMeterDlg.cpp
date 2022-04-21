
// EllipsoMeterDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "EllipsoMeter.h"
#include "EllipsoMeterDlg.h"
#include "DlgProxy.h"
#include "afxdialogex.h"

#include "WorkRun.h"
#include "CamJAI.h"
#include "CommThread.h"
#include "MilVisionDlg.h"
#include "MilVision.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CEllipsoMeterDlg, CDialogEx);

static int nDnBtn [] = { IDC_BTN_MODEL,
	                     IDC_BTN_AUTO,
						 IDC_BTN_SEARCH,
						 IDC_BTN_ENV,
						 IDC_BTN_MAINT,
						 IDC_BTN_EQ_INIT,
						 IDC_BTN_REVIEW,
						 IDC_BTN_APP_EXIT
};

static CString strDnBtn[] = { "모델관리",
	                          "측정",
						      "검색",
						      "환경설정",
						      "카메라영상",
							  "축초기화",
							  "리뷰",
						      "프로그램 종료"
};

CEllipsoMeterDlg::CEllipsoMeterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEllipsoMeterDlg::IDD, pParent)
{
	m_hIcon				= AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pAutoProxy		= NULL;
	m_pTaskHandle		= NULL;
	m_pTask      		= NULL;
	m_pIO				= NULL;
	m_pCommon			= NULL;
	m_pJog				= NULL;
	m_pAuto             = NULL;
	m_pMap				= NULL;
	m_pStageMap         = NULL;
	m_pDel              = NULL;
	m_pContour			= NULL;
	m_bFirst            = FALSE;
	m_nPage             = SW_AUTO;
	m_pLogin			= NULL;
	m_pUserInfo			= NULL;
	m_pPwdChg			= NULL;
	m_pAutoParam        = NULL;
	m_pPoint			= NULL;

	m_pReviewMap		= NULL;
	m_pModel			= NULL;
	m_pManual			= NULL;
	m_pAjin				= NULL;

	m_hMutexOneInstance = NULL;
	m_strOldLogMsg      = _T("");

	m_hVacThread		= NULL;
	m_bVacEnd			= FALSE;
	m_nChart            = SW_MAP;
	m_pReview           = NULL;
	m_pSearch           = NULL;
	m_pUserAuto			= NULL;
	m_pUserReview		= NULL;

	m_bEmergencyStop    = FALSE;

	for (int i = 0; i < SR_CNT; i++)
		pCommTh[i] = nullptr;

	m_bLampOn = FALSE;
}

CEllipsoMeterDlg::~CEllipsoMeterDlg()
{
#if MIL_USE
  	if (pMILAlignDlg != nullptr)
	{
		delete pMILAlignDlg;
	}
#endif
	for (int i = SR_CNT - 1; i >= 0; i--)
	{
		if (pCommTh[i] != nullptr)
		{
			pCommTh[i]->ClosePort();
			delete pCommTh[i];
			pCommTh[i] = nullptr;
		}
	}

#if MIL_USE
	if (pWorkRun != nullptr)
		delete pWorkRun;
#endif
	if (m_pAutoProxy != NULL)
		m_pAutoProxy->m_pDialog = NULL;

	if ( m_pDel != NULL )
	{
		m_pDel->m_fnSetEnd();
		delete m_pDel;
		m_pDel = NULL;
	}

	if (m_pLogin != NULL)
	{
		m_pLogin->m_fnSetEnd();
		delete m_pLogin;
		m_pLogin = NULL;
	}
	if (m_pUserInfo != NULL)
	{
		m_pUserInfo->m_fnSetEnd();
		delete m_pUserInfo;
		m_pUserInfo = NULL;
	}
	if ( m_pPwdChg != NULL )
	{
		m_pPwdChg->m_fnSetEnd();
		delete m_pPwdChg;
		m_pPwdChg = NULL;
	}
	if ( m_pUserAuto != NULL )
	{
		m_pUserAuto->m_fnSetEnd();
		delete m_pUserAuto;
		m_pUserAuto = NULL;
	}

	if ( m_pUserAuto != NULL )
	{
		m_pUserAuto->m_fnSetEnd();
		delete m_pUserAuto;
		m_pUserAuto = NULL;
	}

	if ( m_pUserReview != NULL )
	{
		m_pUserReview->m_fnSetEnd();
		delete m_pUserReview;
		m_pUserReview = NULL;
	}

	if ( m_pAutoParam != NULL )
	{
		m_pAutoParam->m_fnSetEnd();
		delete m_pAutoParam;
		m_pAutoParam = NULL;
	}

	if ( m_pJog != NULL )
	{
		m_pJog->m_fnSetEnd();
		delete m_pJog;
		m_pJog = NULL;
	}

	if (m_pCommon != NULL)
	{
		m_pCommon->m_fnSetEnd();
		delete m_pCommon;
		m_pCommon = NULL;
	}
	m_fnLogWrite("<<<============프로그램 종료=============>>>");

	if ( m_hVacThread != NULL )
	{
		m_bVacEnd			= FALSE;
		SuspendThread( m_hVacThread );
		CloseHandle(m_hVacThread);
		m_hVacThread	    = NULL;
	}

	if ( m_pModel != NULL )
	{
		m_pModel->m_fnSetEnd();
		delete m_pModel;
		m_pModel = NULL;
	}

	if (m_pPoint != NULL)
	{
		m_pPoint->m_fnSetEnd();
		delete m_pPoint;
		m_pPoint = NULL;
	}


	if ( m_pAuto != NULL )
	{
		m_pAuto->m_fnSetEnd();
		delete m_pAuto;
		m_pAuto = NULL;
	}

	if ( m_pContour != NULL )
	{
		m_pContour->m_fnSetEnd();
		delete m_pContour;
		m_pContour = NULL;
	}

	if (m_pMap != NULL)
	{
		m_pMap->m_fnSetEnd();
		delete m_pMap;
		m_pMap = NULL;
	}
	if (m_pReviewMap != NULL)
	{
		m_pReviewMap->m_fnSetEnd();
		delete m_pReviewMap;
		m_pReviewMap = NULL;
	}
	

	if ( m_pStageMap != NULL )
	{
		m_pStageMap->m_fnSetEnd();
		delete m_pStageMap;
		m_pStageMap = NULL;
	}

#if JAI_CAMERA
	if (pCamJAI != nullptr)
		delete pCamJAI;
#endif

	if ( m_pReview != NULL )
	{
		m_pReview->m_fnSetEnd();
		delete m_pReview;
		m_pReview = NULL;
	}

	if ( m_pSearch != NULL )
	{
		m_pSearch->m_fnSetEnd();
		delete m_pSearch;
		m_pSearch = NULL;
	}

	if ( m_pIO != NULL )
	{
		m_pIO->m_fnSetEnd();
		delete m_pIO;
		m_pIO = NULL;
	}

	if ( m_pAjin != NULL )
	{
		delete m_pAjin;
		m_pAjin = NULL;
	}

	if ( m_pManual != NULL )
	{
		m_pManual->m_fnSetEnd();
		delete m_pManual;
		m_pManual = NULL;
	}

	if ( m_pTaskHandle != NULL )
	{
		m_pTaskHandle->m_fnSetEnd();
		delete m_pTaskHandle;
		m_pTaskHandle = NULL;
	}
}

//------------------------------------------------------------
// 각 버튼의 색상과 글자색을 변경한다.
//------------------------------------------------------------
void CEllipsoMeterDlg::m_fnSetBtnOperation( int nID, 
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

void CEllipsoMeterDlg::m_fnLogWrite( CString strMSG ) 
{
	CString strCnt("");
	int     nCnt		= 0;
	int     nTotal		= 0;

	if ( strMSG == m_strOldLogMsg )
	{
		return;
	}

	m_strOldLogMsg = strMSG;

	memset( m_szData, 0x00, sizeof( m_szData ));

	sprintf_s(m_szData, "%s", strMSG.GetString());
	if ( m_pTask == NULL ) return;
	if ( m_pTask->stCommon.bLogSave )
	{
		m_Log.DEBUG_LOG( m_szData );
	}

	if ( m_hWnd == NULL )
	{
		return;
	}
}

BOOL CEllipsoMeterDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (( nID   == 2 ) &&
		( nCode == -1 ))
	{
		return FALSE;
	}
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CEllipsoMeterDlg::PreTranslateMessage(MSG* pMsg) 
{
	// ENTER KEY or ESC KEY CANCEL
	if (( pMsg->wParam == 13 ) || 
		( pMsg->wParam == 27 ))
	{
		return FALSE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CEllipsoMeterDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_BTN_AUTO, m_clAuto);
	DDX_Control(pDX, IDC_BTN_SEARCH, m_clSearch);
	DDX_Control(pDX, IDC_BTN_ENV, m_clEnv);
	DDX_Control(pDX, IDC_BTN_APP_EXIT, m_clAppExit);
	DDX_Control(pDX, IDC_BTN_MAINT, m_clMaint);
	DDX_Control(pDX, IDC_BTN_MODEL, m_clModel);
	DDX_Control(pDX, IDC_BTN_REVIEW, m_clReview);
	DDX_Control(pDX, IDC_BTN_EQ_INIT, m_clEqInit);
	DDX_Control(pDX, IDC_BTN_BUZZER_OFF, m_clBuzzerOff);
	DDX_Control(pDX, IDC_BTN_VAC_OFF, m_clVacuumOff);
	DDX_Control(pDX, IDC_BTN_LOGIN, m_clLogin);
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_LAMP_OFF, m_ctrlBtnLampOff);
}

BEGIN_MESSAGE_MAP(CEllipsoMeterDlg, CDialogEx)
	ON_MESSAGE(WM_SET_COORDINATES, OnSetCoordinates) 
	ON_MESSAGE(WM_SET_EQUIP_INIT, m_fnSetEquipInit ) 
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_APP_EXIT, &CEllipsoMeterDlg::OnBnClickedBtnAppExit)
	ON_BN_CLICKED(IDC_BTN_MODEL, &CEllipsoMeterDlg::OnBnClickedBtnModel)
	ON_BN_CLICKED(IDC_BTN_AUTO, &CEllipsoMeterDlg::OnBnClickedBtnAuto)
	ON_BN_CLICKED(IDC_BTN_MAINT, &CEllipsoMeterDlg::OnBnClickedBtnMaint)
	ON_BN_CLICKED(IDC_BTN_ENV, &CEllipsoMeterDlg::OnBnClickedBtnEnv)
	ON_BN_CLICKED(IDC_BTN_SEARCH, &CEllipsoMeterDlg::OnBnClickedBtnSearch)
	ON_BN_CLICKED(IDC_BTN_EQ_INIT, &CEllipsoMeterDlg::OnBnClickedBtnEqInit)
	ON_BN_CLICKED(IDC_BTN_BUZZER_OFF, &CEllipsoMeterDlg::OnBnClickedBtnBuzzerOff)
	ON_BN_CLICKED(IDC_BTN_VAC_OFF, &CEllipsoMeterDlg::OnBnClickedBtnVacOff)
	ON_MESSAGE( UM_VAC_STATUS, m_fnSetVacOnOff)
	ON_BN_CLICKED(IDC_BTN_REVIEW, &CEllipsoMeterDlg::OnBnClickedBtnReview)
	ON_BN_CLICKED(IDC_BTN_LOGIN, &CEllipsoMeterDlg::OnBnClickedBtnLogin)
	ON_WM_SETFOCUS()
	ON_MESSAGE(UM_CHART_VIEW, &CEllipsoMeterDlg::m_fnUserMsgSetChartView)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_LAMP_OFF, &CEllipsoMeterDlg::OnBnClickedBtnLampOff)
END_MESSAGE_MAP()

/*-----------------------------------------------------------------------
Func Name : OnSetCoordinates
Reference : OnSetCoordinates 주기적으로 스레드로부터 메세지를 받음
-----------------------------------------------------------------------*/
LRESULT CEllipsoMeterDlg::OnSetCoordinates(WPARAM nPort, LPARAM lParam)
{
	if ( m_pTask == NULL )
	{
		return 1;
	}
	m_fnSetUpBanner();
	return 0;
}

BOOL CEllipsoMeterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CString s("");
	s.Format( "엘립소메타 시스템");
	this->SetWindowText(s);

	theApp.m_fnSetViewPointer( this );
	this->MoveWindow( 0, 0, 1920,1080, TRUE );

	m_hMutexOneInstance = ::CreateMutex(NULL, TRUE, _T("EllipsoMeter"));
	if(::GetLastError() == ERROR_ALREADY_EXISTS)
	{
		TECmsgOut( L"엘립소메타 모듈이 이미 실행중입니다.", GREEN_1, 1 );
		if ( m_hMutexOneInstance != NULL )
		{
			::ReleaseMutex(m_hMutexOneInstance);
			::CloseHandle(m_hMutexOneInstance);
			m_hMutexOneInstance = NULL;
		}
		exit(1);
		return FALSE;
	}

	m_Log.m_fnSetLogPath( "EllipsoMeter", "EllipsoMeter" );
	m_fnLogWrite( "<<<============프로그램 시작=============>>>" );
	m_Log.m_fnSetLogWriteFlag( TRUE );

	m_pTaskHandle = new CTaskHandle( this, SHARED_NAME, 
											1, 
											sizeof( SHEET_RESISTANCE_Format ) + 1,
											0,
											true );


	m_pTask					= ( SHEET_RESISTANCE_Format* )m_pTaskHandle->m_pFilePointer( );

	InitObject();
	//OUT6 ON
	m_pIO->m_fnSetOutputSignal(MEASURE_READY, FALSE);
	//OUT7 OFF
	m_pIO->m_fnSetOutputSignal(MEASURE_BUSY, FALSE);


	m_fnSetBtnOperation( IDC_BTN_LOGIN,L_BIT_BTN_OFF,L_BIT_CHAR_OFF,"로그인");

	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	JoyInit(m_pJog->m_hWnd);
	Initial_Interface();

#if !_DEBUG
#if ELLIPSO
	CString strPath, strExe;
	strPath.Format(_T("C:\\CompleteEASE\\CompleteEASE.exe"));
	strPath.Trim();
	if (!strPath.IsEmpty())
	{
		int nLen = strPath.ReverseFind('\\');
		if (nLen != -1)
		{
			strExe = strPath.Right(strPath.GetLength() - nLen - 1);
		}

		if (m_Util.Existence(strPath))
		{
			ZeroMemory(&m_siEllipsoSW, sizeof(SHELLEXECUTEINFO));

			m_siEllipsoSW.cbSize = sizeof(SHELLEXECUTEINFO);
			m_siEllipsoSW.lpFile = __TEXT(strPath);
			m_siEllipsoSW.lpVerb = __TEXT("open");
			m_siEllipsoSW.nShow = SW_HIDE;
			m_siEllipsoSW.fMask = SEE_MASK_NOCLOSEPROCESS;

			ShellExecuteEx(&m_siEllipsoSW);

 			m_fnLogWrite("CompleteEASE를 실행합니다.");
		}
	}
#endif
#endif

	m_fMainButtonFont.CreateFont(30, 0, 0, 0, 0/*FW_BOLD*/, FALSE, FALSE, 0, 0, 0, 0, 0, 0, NULL/*_T("굴림")*/);
	GetDlgItem(IDC_BTN_AUTO)->SetFont(&m_fMainButtonFont);
	GetDlgItem(IDC_BTN_REVIEW)->SetFont(&m_fMainButtonFont);
	GetDlgItem(IDC_BTN_MODEL)->SetFont(&m_fMainButtonFont);
	GetDlgItem(IDC_BTN_ENV)->SetFont(&m_fMainButtonFont);
	GetDlgItem(IDC_BTN_MAINT)->SetFont(&m_fMainButtonFont);
	GetDlgItem(IDC_BTN_EQ_INIT)->SetFont(&m_fMainButtonFont);
	GetDlgItem(IDC_BTN_VAC_OFF)->SetFont(&m_fMainButtonFont);
	GetDlgItem(IDC_BTN_LAMP_OFF)->SetFont(&m_fMainButtonFont);
	GetDlgItem(IDC_BTN_APP_EXIT)->SetFont(&m_fMainButtonFont);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CEllipsoMeterDlg::InitObject()
{
	m_pDel = new CAutoFileDelDLG();
	m_pDel->m_fnSetInit();
	m_pDel->m_fnSetShowFlag(FALSE);

	m_pCommon = new CCommon();
	m_pCommon->m_fnSetInit();
	m_pCommon->m_fnSetShowFlag(FALSE);

	m_pIO = new CElipsoMeter_IO();
	m_pIO->m_fnSetInit();
	m_pIO->m_fnSetShowFlag(FALSE);

	// 	m_pIO = new CIO();
	// 	m_pIO->m_fnSetInit( );
	// 	m_pIO->m_fnSetShowFlag( FALSE );

	m_pAjin = new CAJIN_CtlClass();
	char szMotFile[FILE_LEN + 1];
	memset(szMotFile, 0x00, sizeof(szMotFile));
	sprintf_s(szMotFile, "%s%s%s%s", m_Util.m_fnGetCurrentDrive().GetString(), DEFAULT_PATH, DEFAULT_ENV_PATH, DEFAULT_MOT_FILE);
	m_pAjin->LoadMotFile(szMotFile);

	m_pJog = new CJogMove();
	m_pJog->m_fnSetInit();
	m_pJog->m_fnSetShowFlag(FALSE);

	m_pStageMap = new CStageMapViewDLG();
	m_pStageMap->m_fnSetInit();
	m_pStageMap->m_fnSetShowFlag(false);

	m_pModel = new CModel();
	m_pModel->m_fnSetInit();
	m_pModel->m_fnSetShowFlag(false);

	m_pPoint = new CPointDLG();
	m_pPoint->m_fnSetInit();
	m_pPoint->m_fnSetShowFlag( false );

	m_pMap = new CMapViewDLG();
	m_pMap->m_fnSetInit( );
	m_pMap->m_fnSetShowFlag( false );

	m_pReviewMap = new CMapViewDLG();
	m_pReviewMap->m_fnSetInit( );
	m_pReviewMap->m_fnSetShowFlag( false );

	m_pAuto = new CAutoInspectorDlg();
	m_pAuto->m_fnSetInit();
	m_pAuto->m_fnSetShowFlag( false );

	m_pContour = new CChartDLG();
	m_pContour->m_fnSetInit();
	m_pContour->m_fnSetShowFlag(false);

	m_pReview = new CReviewDLG();
	m_pReview->m_fnSetInit();
	m_pReview->m_fnSetShowFlag(false);

	m_pSearch = new CSearchDLG();
	m_pSearch->m_fnSetInit();
	m_pSearch->m_fnSetShowFlag(false);

	m_pManual = new CManualDLG();
	m_pManual->m_fnSetInit();
	m_pManual->m_fnSetShowFlag(false);

	m_pUserAuto = new CUSerInputDLG();
	m_pUserAuto->m_fnSetInit();
	m_pUserAuto->m_fnSetShowFlag(false);

	m_pUserReview = new CUSerInputDLG();
	m_pUserReview->m_fnSetInit();
	m_pUserReview->m_fnSetShowFlag(false);

	m_fnSetChangeBtn(SW_AUTO);
	theApp.m_fnGetViewPointer()->m_fnSetSwitch(SW_AUTO);
	this->PostMessageA(UM_VAC_STATUS, 0, 0);
	m_fnSetBtnOperation(IDC_BTN_LAMP_OFF, L_BIT_BTN_OFF, L_BIT_CHAR_OFF, "Lamp OFF");

	m_pLogin = new CPasswordDlg();
	m_pLogin->m_fnSetInit();
	m_pLogin->m_fnSetShowFlag(false);

	m_pUserInfo = new CPasswordManagerDLG();
	m_pUserInfo->m_fnSetInit();
	m_pUserInfo->m_fnSetShowFlag(false);

	m_pPwdChg = new CPasswordChangeDLG();
	m_pPwdChg->m_fnSetInit();
	m_pPwdChg->m_fnSetShowFlag(false);

	m_pAutoParam = new CAutoMeasureParamDLG();
	m_pAutoParam->m_fnSetInit();
	m_pAutoParam->m_fnSetShowFlag(false);

#if MIL_USE
	pWorkRun = new CWorkRun();
#endif
#if JAI_CAMERA
	pCamJAI = new CCamJAI();
	pCamJAI->m_fnSetShowFlag(false);
#endif
	for (int i = 0; i < SR_CNT; i++)
		pCommTh[i] = (CCommThread *)new CCommThread(i);

#if MIL_USE
	pMILAlignDlg = new CMilVisionDlg();
#endif
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CEllipsoMeterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		m_fnSetUpBanner();
#if AJINTEX_BOARD
		if ( !m_bFirst )
		{
			m_bFirst				   = TRUE;
			m_fnSetEquipInitialize();
		}
#endif
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CEllipsoMeterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CEllipsoMeterDlg::m_fnSetViewCommon()
{
	if ( m_pCommon == NULL )
	{
		m_pCommon = new CCommon();
		m_pCommon->m_fnSetInit();
	}
	m_pCommon->m_fnSetShowFlag(!m_pCommon->m_bShow);
}

void CEllipsoMeterDlg::m_fnSetViewJog()
{
	if ( m_pJog == NULL )
	{
		m_pJog = new CJogMove();
		m_pJog->m_fnSetInit();
	}
	m_pJog->m_fnSetShowFlag(!m_pJog->m_bShow);
}

void CEllipsoMeterDlg::m_fnSetViewAutoDel()
{
	if ( m_pDel == NULL )
	{
		m_pDel = new CAutoFileDelDLG();
		m_pDel->m_fnSetInit();
	}
	m_pDel->m_fnSetShowFlag(!m_pDel->m_bShow);
}

void CEllipsoMeterDlg::m_fnSetViewCamera()
{
#if JAI_CAMERA
	if (pCamJAI == NULL)
	{
		pCamJAI = new CCamJAI();
	}
	pCamJAI->m_fnSetShowFlag(!pCamJAI->m_bShow);
#endif
}

void CEllipsoMeterDlg::m_fnSetViewModel()
{
	if ( m_pModel == NULL )
	{
		m_pModel = new CModel();
		m_pModel->m_fnSetInit();
	}
	m_pModel->m_fnSetShowFlag(!m_pModel->m_bShow);
}

void CEllipsoMeterDlg::m_fnSetViewAuto()
{
	if ( m_pAuto == NULL )
	{
		m_pAuto = new CAutoInspectorDlg();
		m_pAuto->m_fnSetInit();
	}
	m_pAuto->m_fnSetShowFlag(!m_pAuto->m_bShow);
}

void CEllipsoMeterDlg::m_fnSetEquipInitialize()
{
	m_pTask->stEqp.bEqpInit    = FALSE;
	this->SendMessageA( WM_SET_EQUIP_INIT, 0, 0 );
}

LRESULT CEllipsoMeterDlg::m_fnSetEquipInit(  WPARAM nNo, LPARAM lParam )
{
	CInit dlg;
	dlg.DoModal();

	return 0;
}

void CEllipsoMeterDlg::m_fnSetSwitch( int nNo )
{
	m_nPage = nNo;

	if (nNo != SW_MODEL)
	{
		if(pMILAlignDlg != NULL)
			pMILAlignDlg->SendMessageA(WM_COMMAND, IDOK, 0);
	}

	switch( nNo )
	{
		case SW_AUTO:
			{
				m_pSearch->m_fnSetShowFlag( FALSE );
				m_pReview->m_fnSetShowFlag( FALSE );
				m_pAuto->m_fnSetShowFlag( TRUE );
				m_pModel->m_fnSetShowFlag( FALSE );
				m_pCommon->m_fnSetShowFlag( FALSE );
				m_pMap->m_fnSetShowFlag( TRUE );
				m_pReviewMap->m_fnSetShowFlag( FALSE );
				m_pJog->m_fnSetShowFlag( FALSE );
				m_pStageMap->m_fnSetShowFlag( FALSE );
				m_pDel->m_fnSetShowFlag( FALSE );
				m_pContour->m_fnSetShowFlag( FALSE );
				m_pManual->m_fnSetShowFlag( FALSE );
#if MIL_USE
				if(pWorkRun != NULL && !pWorkRun->m_bThrdFlag[TF_AUTORUN])
					m_fnSetChartView( m_nChart );
#endif
			}
			break;
		case SW_MODEL:
			{
				m_pSearch->m_fnSetShowFlag( FALSE );
				m_pReview->m_fnSetShowFlag( FALSE );
				m_pAuto->m_fnSetShowFlag( FALSE );
				m_pModel->m_fnSetShowFlag( TRUE );
				m_pCommon->m_fnSetShowFlag( FALSE );
				m_pMap->m_fnSetShowFlag( FALSE );
				m_pReviewMap->m_fnSetShowFlag( FALSE );
				m_pJog->m_fnSetShowFlag( TRUE );
				m_pStageMap->m_fnSetShowFlag( TRUE );
				m_pDel->m_fnSetShowFlag( FALSE );
				m_pContour->m_fnSetShowFlag( FALSE );
				m_pManual->m_fnSetShowFlag( FALSE );
			}
			break;
		case SW_ENV:
			{
				m_pSearch->m_fnSetShowFlag( FALSE );
				m_pReview->m_fnSetShowFlag( FALSE );
				m_pAuto->m_fnSetShowFlag( FALSE );
				m_pModel->m_fnSetShowFlag( FALSE );
				m_pCommon->m_fnSetShowFlag( TRUE );
				m_pMap->m_fnSetShowFlag( FALSE );
				m_pReviewMap->m_fnSetShowFlag( FALSE );
				m_pJog->m_fnSetShowFlag( TRUE );
				m_pStageMap->m_fnSetShowFlag( TRUE );
				m_pDel->m_fnSetShowFlag( FALSE );
				m_pContour->m_fnSetShowFlag( FALSE );
				m_pManual->m_fnSetShowFlag( FALSE );
			}
			break;
		case SW_MAINT:
			{
				m_pSearch->m_fnSetShowFlag( FALSE );
				m_pReview->m_fnSetShowFlag( FALSE );
				m_pAuto->m_fnSetShowFlag( FALSE );
				m_pModel->m_fnSetShowFlag( FALSE );
				m_pCommon->m_fnSetShowFlag( FALSE );
				m_pMap->m_fnSetShowFlag( FALSE );
				m_pReviewMap->m_fnSetShowFlag( FALSE );
				m_pJog->m_fnSetShowFlag( FALSE );
				m_pStageMap->m_fnSetShowFlag( FALSE );
				m_pDel->m_fnSetShowFlag( FALSE );
				m_pContour->m_fnSetShowFlag( FALSE );
				m_pManual->m_fnSetShowFlag( TRUE );
			}
			break;
		case SW_REVIEW:
			{
				m_pSearch->m_fnSetShowFlag( FALSE );
				m_pReview->m_fnSetShowFlag( TRUE );
				m_pAuto->m_fnSetShowFlag( FALSE );
				m_pModel->m_fnSetShowFlag( FALSE );
				m_pCommon->m_fnSetShowFlag( FALSE );
				m_pMap->m_fnSetShowFlag( FALSE );
				m_pReviewMap->m_fnSetShowFlag( TRUE );
				m_pJog->m_fnSetShowFlag( FALSE );
				m_pStageMap->m_fnSetShowFlag( FALSE );
				m_pDel->m_fnSetShowFlag( FALSE );
				m_pContour->m_fnSetShowFlag( FALSE );
				m_pManual->m_fnSetShowFlag( FALSE );
#if MIL_USE
				if (pWorkRun != NULL && !pWorkRun->m_bThrdFlag[TF_AUTORUN])
					m_fnSetChartView(m_nChart);
#endif
			}
			break;
		case SW_SEARCH:
			{
				m_pSearch->m_fnSetShowFlag( TRUE );
				m_pReview->m_fnSetShowFlag( FALSE );
				m_pAuto->m_fnSetShowFlag( FALSE );
				m_pModel->m_fnSetShowFlag( FALSE );
				m_pCommon->m_fnSetShowFlag( FALSE );
				m_pMap->m_fnSetShowFlag( FALSE );
				m_pReviewMap->m_fnSetShowFlag( FALSE );
				m_pJog->m_fnSetShowFlag( FALSE );
				m_pStageMap->m_fnSetShowFlag( FALSE );
				m_pDel->m_fnSetShowFlag( FALSE );
				m_pContour->m_fnSetShowFlag( FALSE );
				m_pManual->m_fnSetShowFlag( FALSE );
			}
			break;
		case SW_EQ_INIT:
		case SW_APP_EXT:
			{
			}
			break;
	}
}


void CEllipsoMeterDlg::m_fnSetViewStageMap()
{
	if ( m_pStageMap == NULL )
	{
		m_pStageMap = new CStageMapViewDLG();
		m_pStageMap->m_fnSetInit();
	}
	m_pStageMap->m_fnSetShowFlag(!m_pMap->m_bShow);
}

void CEllipsoMeterDlg::m_fnSetViewMap()
{
	if ( m_pMap == NULL )
	{
		m_pMap = new CMapViewDLG();
		m_pMap->m_fnSetInit();
	}
	m_pMap->m_fnSetShowFlag(!m_pMap->m_bShow);
}

UINT VacThread(LPVOID pFuncData)
{
	BOOL bVac = false;
	CEllipsoMeterDlg		*pQI = (CEllipsoMeterDlg *)pFuncData;

	while ( pQI->m_bVacEnd )
	{
		if ( pQI->m_hWnd != NULL )
		{
// 			if ( bVac != theApp.m_fnGetViewPointer()->m_pTask->stElipsoIO.stIn.Vacuum_on )
// 			{
// 				bVac = theApp.m_fnGetViewPointer()->m_pTask->stElipsoIO.stOut.Vacuum_On;
// 				pQI->PostMessageA( UM_VAC_STATUS, theApp.m_fnGetViewPointer()->m_pTask->stElipsoIO.stIn.Vacuum_on, 0 );
// 			}
// 			Sleep( IO_READ_TIME );
		}
	}

	if ( pQI->m_hWnd == NULL ) return 0;

	if ( pQI->m_hVacThread != NULL )
	{
		SuspendThread(pQI->m_hVacThread);
		CloseHandle(pQI->m_hVacThread);
		pQI->m_hVacThread	= NULL;
	}
	return 0;
}

void ThrdMsgReport()
{
#if MIL_USE
	while (pWorkRun->m_bThrdFlag[TF_MAKE_REPORT])
	{
		Sleep(1000);
		TECmsgOut(L"Excel 레포트 작성 중....", YELLOW, 1000, 17, BLACK);
	}
	return;
#endif
}

LRESULT CEllipsoMeterDlg::m_fnSetVacOnOff(  WPARAM nNo, LPARAM lParam )
{
	if ( nNo == 1 )
	{
		m_fnSetBtnOperation( IDC_BTN_VAC_OFF, L_BIT_BTN_ON, L_BIT_CHAR_ON, "VAC. ON");
	}
	else
	{
		m_fnSetBtnOperation( IDC_BTN_VAC_OFF, L_BIT_BTN_OFF, L_BIT_CHAR_OFF, "VAC. OFF");
	}
	return 0;
}

void CEllipsoMeterDlg::m_fnSetChangeBtn( int nSwitch ) 
{
	m_nSwitchNo = nSwitch;
	for( int i = 0; i < 8; i++ )
	{
		if ( nSwitch == i )
		{
			m_fnSetBtnOperation( nDnBtn[i],L_BIT_BTN_ON,L_BIT_CHAR_ON,strDnBtn[i]);
		}
		else
		{
			m_fnSetBtnOperation( nDnBtn[i],L_BIT_BTN_OFF,L_BIT_CHAR_OFF,strDnBtn[i]);
		}
	}
}

void CEllipsoMeterDlg::OnBnClickedBtnSearch()
{
#if !_DEBUG
	if ( m_pTask->stLogin.nLevel == UNKNOWN_USER ) return;
#endif

	if ( !theApp.m_fnGetViewPointer()->m_pTask->stEqp.bIoStartButton )
	{
		theApp.m_fnGetViewPointer()->m_fnSetSwitch( SW_SEARCH );
		m_fnSetChangeBtn( SW_SEARCH );
	}
	else
	{
		CString strMsg("");
		strMsg.Format( "자동측정중에는 검색 기능이 제한됩니다." );
		//TECmsgOut( strMsg , GREEN_1, 1 );
		m_fnLogWrite( strMsg ); 
	}
}

void CEllipsoMeterDlg::OnBnClickedBtnAuto()
{
	Invalidate(TRUE);

// 	if ( m_pTask->stLogin.nLevel == UNKNOWN_USER ) return;

	if ( !theApp.m_fnGetViewPointer()->m_pTask->stEqp.bIoStartButton )
	{
		theApp.m_fnGetViewPointer()->m_fnSetSwitch( SW_AUTO );
		m_fnSetChangeBtn( SW_AUTO );
	}
}

void CEllipsoMeterDlg::OnBnClickedBtnEnv()
{
#if !_DEBUG
	if (( m_pTask->stLogin.nLevel == UNKNOWN_USER ) ||
	    ( m_pTask->stLogin.nLevel == OPERATOR ))
	{
		return;
	}
#endif

// 	if ( !theApp.m_fnGetViewPointer()->m_pTask->stEqp.bIoStartButton )
#if MIL_USE
	if (!pWorkRun->m_bThrdFlag[TF_AUTORUN])
	{
		theApp.m_fnGetViewPointer()->m_fnSetSwitch( SW_ENV );
		m_fnSetChangeBtn( SW_ENV );
	}
	else
	{
		CString strMsg("");
		strMsg.Format( "자동측정중에는 환경설정 화면전환이 불가합니다." );
		//TECmsgOut( strMsg , GREEN_1, 1 );
		m_fnLogWrite( strMsg ); 
	}
#endif
}

void CEllipsoMeterDlg::OnBnClickedBtnAppExit()
{
// 	if ( !theApp.m_fnGetViewPointer()->m_pTask->stEqp.bIoStartButton )
#if MIL_USE
	if (!pWorkRun->m_bThrdFlag[TF_AUTORUN])
	{
		theApp.m_fnGetViewPointer()->m_fnSetSwitch( SW_APP_EXT );
		m_fnSetChangeBtn( SW_APP_EXT );
		int nResult = TECmsgOut( L"프로그램을 종료하시겠습니까?", GREEN_1, 2 );
		if(nResult == 1)
		{
			theApp.m_fnGetViewPointer()->m_pManual->m_pElipso->m_fnSetSystemLampOff();

			theApp.m_fnGetViewPointer()->SendMessage( WM_CLOSE, 0, 0 );
		}
	}
	else
	{
		CString strMsg("");
		strMsg.Format( "자동측정중에는 프로그램 종료를 할수 없습다." );
		//TECmsgOut( strMsg , GREEN_1, 1 );
		m_fnLogWrite( strMsg ); 
	}
#else
	theApp.m_fnGetViewPointer()->m_fnSetSwitch(SW_APP_EXT);
	m_fnSetChangeBtn(SW_APP_EXT);
	int nResult = TECmsgOut(L"프로그램을 종료하시겠습니까?", GREEN_1, 2);
	if (nResult == 1)
	{
		theApp.m_fnGetViewPointer()->SendMessage(WM_CLOSE, 0, 0);
	}
#endif
}

void CEllipsoMeterDlg::OnBnClickedBtnMaint()
{
#if !_DEBUG
	if (( m_pTask->stLogin.nLevel == UNKNOWN_USER ) ||
	    ( m_pTask->stLogin.nLevel == OPERATOR ))
	{
		return;
	}
#endif

// 	if ( !theApp.m_fnGetViewPointer()->m_pTask->stEqp.bIoStartButton )
#if MIL_USE
// 	if (!pWorkRun->m_bThrdFlag[TF_AUTORUN])
	{
		if (m_pTask->stLogin.nLevel == MAKER_ENGINEER)
		{
			theApp.m_fnGetViewPointer()->m_fnSetSwitch(SW_MAINT);
			m_fnSetChangeBtn(SW_MAINT);
		}
		else
		{
			LightControl(0, 1, m_pTask->stCommon.nInspLight);
			theApp.m_fnGetViewPointer()->m_fnSetViewCamera();
		}

	}
// 	else
// 	{
// 		CString strMsg("");
// 		strMsg.Format( "자동측정중에는 메인트관리 화면전환 기능이 제한됩니다" );
// 		//TECmsgOut( strMsg , GREEN_1, 1 );
// 		m_fnLogWrite( strMsg ); 
// 	}
#endif
}

void CEllipsoMeterDlg::OnBnClickedBtnModel()
{
#if !_DEBUG
	if (( m_pTask->stLogin.nLevel == UNKNOWN_USER ) ||
	    ( m_pTask->stLogin.nLevel == OPERATOR ))
	{
		return;
	}
#endif

// 	if ( !theApp.m_fnGetViewPointer()->m_pTask->stEqp.bIoStartButton )
#if MIL_USE
	if (!pWorkRun->m_bThrdFlag[TF_AUTORUN])
	{
		theApp.m_fnGetViewPointer()->m_fnSetSwitch( SW_MODEL );
		m_fnSetChangeBtn( SW_MODEL );
	}
	else
	{
		CString strMsg("");
		strMsg.Format( "자동측정중에는 모델관리 화면전환 기능이 제한됩니다" );
		//TECmsgOut( strMsg , GREEN_1, 1 );
		m_fnLogWrite( strMsg ); 
	}
#endif
}


void CEllipsoMeterDlg::OnBnClickedBtnEqInit()
{
	if (!TECmsgOut(m_Util.CStringTobstr(_T("정말 초기화를 실시하시겠습니까?")), LIGHTMAGENTA, 2))
		return;

#if	AJINTEX_BOARD
// 	if ( !theApp.m_fnGetViewPointer()->m_pTask->stEqp.bIoStartButton )
	if (!pWorkRun->m_bThrdFlag[TF_AUTORUN])
	{
// 		theApp.m_fnGetViewPointer()->m_fnSetSwitch( SW_EQ_INIT );
// 		m_fnSetChangeBtn( SW_EQ_INIT );
		theApp.m_fnGetViewPointer()->m_pTask->stEqp.bEqpInit    = FALSE;
		theApp.m_fnGetViewPointer()->SendMessageA( WM_SET_EQUIP_INIT, 0, 0 );
	}
	else
	{
		TECmsgOut( L"자동측정중에는 장비초기화 기능이 제한됩니다.", GREEN_1, 1 );
	}
#endif
}


void CEllipsoMeterDlg::OnBnClickedBtnBuzzerOff()
{
	theApp.m_fnGetViewPointer()->m_pIO->PostMessageA( UM_BUZZER, BUZZER_ONOFF, L_BIT_OFF );
}


HBRUSH CEllipsoMeterDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}

void CEllipsoMeterDlg::OnBnClickedBtnVacOff()
{

	if (m_pTask->stElipsoIO.stIn.Wafer_Vacuum_Sensor_06_Inch
		|| m_pTask->stElipsoIO.stIn.Wafer_Vacuum_Sensor_08_Inch
		|| m_pTask->stElipsoIO.stIn.Wafer_Vacuum_Sensor_12_Inch)
	{
		this->SendMessageA(UM_VAC_STATUS, 0, 0);

		m_pTask->stElipsoIO.stOut.Vacuum_On_06_Inch = FALSE;
		m_pTask->stElipsoIO.stOut.Vacuum_On_08_Inch = FALSE;
		m_pTask->stElipsoIO.stOut.Vacuum_On_12_Inch = FALSE;
		m_pAjin->m_fnSetAxdoWriteOutportBit(ELIPSO_IO_OUTPUT_1, VACUUM_ON_06_INCH, m_pTask->stElipsoIO.stOut.Vacuum_On_06_Inch);
		m_pAjin->m_fnSetAxdoWriteOutportBit(ELIPSO_IO_OUTPUT_1, VACUUM_ON_08_INCH, m_pTask->stElipsoIO.stOut.Vacuum_On_08_Inch);
		m_pAjin->m_fnSetAxdoWriteOutportBit(ELIPSO_IO_OUTPUT_1, VACUUM_ON_12_INCH, m_pTask->stElipsoIO.stOut.Vacuum_On_12_Inch);
	}
	else
	{
		this->SendMessageA(UM_VAC_STATUS, 1, 0);

		switch (m_pMap->m_nWaperSize)
		{
		case WAPER_4:
			this->SendMessageA(UM_VAC_STATUS, 0, 0);
			break;
		case WAPER_6:
			m_pIO->m_fnSetOUTPUT(VACUUM_ON_06_INCH);
			break;
		case WAPER_8:
			m_pIO->m_fnSetOUTPUT(VACUUM_ON_08_INCH);
			break;
		case WAPER_12:
			m_pIO->m_fnSetOUTPUT(VACUUM_ON_12_INCH);
			break;
		default:
			break;
		}
	}
}

void CEllipsoMeterDlg::m_fnSetUpBanner( ) 
{
	m_fnSetLogo();
	m_fnSetTitle();
	m_fnSetQrCode();
	m_fnSetDateTime();
	this->Invalidate(false);
}

void CEllipsoMeterDlg::m_fnSetLogo( ) 
{
#if OPENCV
	CvvImage							cImg;
	IplImage*							pImg;

	CString strImgFile("");
	strImgFile.Format( "%s", theApp.m_fnGetViewPointer()->m_pTask->stCommon.szLogoPath );
	strImgFile.Trim();

	RECT rectImg;
	CDC* pImgDC = GetDlgItem( IDC_STC_COMPANY )->GetDC();
	GetDlgItem( IDC_STC_COMPANY )->GetClientRect(&rectImg);
	if ( !strImgFile.IsEmpty() )
	{
		pImg = cvLoadImage( theApp.m_fnGetViewPointer()->m_pTask->stCommon.szLogoPath );
	}
	else
	{
		pImg = cvCreateImage(cvSize( rectImg.right, rectImg.bottom ), IPL_DEPTH_8U, 3);
		cvSet( pImg, cvScalar(CV_RGB(255, 255, 255)));
	}

	cImg.CopyOf( pImg, 1 );
	if (pImgDC!=NULL)
	{
		cImg.DrawToHDC( pImgDC->m_hDC, &rectImg );
	
		ReleaseDC( pImgDC );
	}
	cvReleaseImage( &pImg );
	pImg     = NULL;
#endif
}

void CEllipsoMeterDlg::m_fnSetQrCode( ) 
{
#if OPENCV
	CvvImage							cImg;
	IplImage*							pImg;

	CString strImgFile("");
	strImgFile.Format( "%s", theApp.m_fnGetViewPointer()->m_pTask->stCommon.szQrCodePath );
	strImgFile.Trim();

	RECT rectImg;
	CDC* pImgDC = GetDlgItem( IDC_STC_QRCODE )->GetDC();
	GetDlgItem( IDC_STC_QRCODE )->GetClientRect(&rectImg);
	if ( !strImgFile.IsEmpty() )
	{
		pImg = cvLoadImage( strImgFile );
	}
	else
	{
		pImg = cvCreateImage(cvSize( rectImg.right, rectImg.bottom ), IPL_DEPTH_8U, 3);
		cvSet( pImg, cvScalar(CV_RGB(255, 255, 255)));
	}

	cImg.CopyOf( pImg, 1 );
	if(pImgDC!=NULL)
	{
		cImg.DrawToHDC(pImgDC->m_hDC, &rectImg);

		ReleaseDC(pImgDC);
	}
	cvReleaseImage( &pImg );
	pImg     = NULL;
#endif
}

void CEllipsoMeterDlg::m_fnSetTitle( ) 
{
#if OPENCV
	CvvImage							cImg;
	IplImage*							pImg;

	CString strText("");
	sprintf_s( theApp.m_fnGetViewPointer()->m_pTask->stCommon.szTitle, "Thin Film Measurement System");
	strText.Format( "%s", theApp.m_fnGetViewPointer()->m_pTask->stCommon.szTitle );
	strText.Trim();

	RECT rectImg;
	CDC* pImgDC = GetDlgItem( IDC_STC_MAIN_TITLE )->GetDC();
	GetDlgItem( IDC_STC_MAIN_TITLE )->GetClientRect(&rectImg);
	pImg = cvCreateImage(cvSize( rectImg.right, rectImg.bottom ), IPL_DEPTH_8U, 3);
	cvSet( pImg, cvScalar(CV_RGB(15, 18, 43)));

	CvFont fontTitle;
	cvInitFont(&fontTitle, CV_FONT_HERSHEY_TRIPLEX, 1.5, 1.5);
	cvPutText(pImg, strText, cvPoint(240, 50), &fontTitle, cvScalar(CV_RGB(255, 255, 255)));

	strText = "  Ver. 1.83F.09"; // YYMM hex . DD hex
// 	strText += m_Util.m_fnGetVersionInfo();
	CvFont fontVer;
	cvInitFont(&fontVer, CV_FONT_HERSHEY_COMPLEX, 0.6,0.6 );
	cvPutText( pImg, strText , cvPoint(1150,50),&fontVer, cvScalar(CV_RGB( 255,255,255 )));

	cImg.CopyOf( pImg, 1 );
	if (pImgDC!=NULL)
	{
		cImg.DrawToHDC( pImgDC->m_hDC, &rectImg );
		ReleaseDC( pImgDC );
	}
	cvReleaseImage( &pImg );
	pImg     = NULL;
#endif
}

void CEllipsoMeterDlg::m_fnSetDateTime( ) 
{
#if OPENCV
	CvvImage							cImg;
	IplImage*							pImg;

	CString strText("");
	strText.Format( "%s", theApp.m_fnGetViewPointer()->m_pTask->stCommon.szTitle );
	strText.Trim();

	RECT rectImg;
	CDC* pImgDC = GetDlgItem( IDC_STC_DATETIME )->GetDC();
	GetDlgItem( IDC_STC_DATETIME )->GetClientRect(&rectImg);
	pImg = cvCreateImage(cvSize( rectImg.right, rectImg.bottom ), IPL_DEPTH_8U, 3);
	cvSet( pImg, cvScalar(CV_RGB(26, 30, 72)));

	CvFont fontDay;
	cvInitFont(&fontDay, CV_FONT_HERSHEY_SIMPLEX, 0.5,0.5 );
	cvPutText( pImg, m_fnGetToDay() , cvPoint(10,20),&fontDay, cvScalar(CV_RGB( 255,255,255 )));
	CvFont fontTime;
	cvInitFont(&fontTime, CV_FONT_HERSHEY_SIMPLEX, 0.8, 0.8);
	cvPutText( pImg, m_fnGetTime() , cvPoint( 30,50),&fontTime, cvScalar(CV_RGB( 255,255,255 )));

	cImg.CopyOf( pImg, 1 );
	if (pImgDC != NULL)
	{
		cImg.DrawToHDC(pImgDC->m_hDC, &rectImg);

		ReleaseDC(pImgDC);
	}
	cvReleaseImage( &pImg );
	pImg     = NULL;
#endif
}

CString CEllipsoMeterDlg::m_fnGetToDay()
{
	CString     strTime("");
	SYSTEMTIME mtime;
	GetLocalTime(&mtime);

	strTime.Format( "%04d.%02d.%02d", mtime.wYear, 
								 			 mtime.wMonth, 
											 mtime.wDay);
	return strTime;
}

CString CEllipsoMeterDlg::m_fnGetTime()
{
	CString     strTime("");
	SYSTEMTIME mtime;
	GetLocalTime(&mtime);

	strTime.Format( "%01d:%02d:%02d", mtime.wHour, 
									  mtime.wMinute, 
									  mtime.wSecond);
	return strTime;
}

void CEllipsoMeterDlg::OnBnClickedBtnReview()
{
	Invalidate(TRUE);

#if !_DEBUG
	if ( m_pTask->stLogin.nLevel == UNKNOWN_USER ) return;
#endif

// 	if (!theApp.m_fnGetViewPointer()->m_pTask->stEqp.bIoStartButton)
#if MIL_USE
//	if (!pWorkRun->m_bThrdFlag[TF_AUTORUN])
//	{
		theApp.m_fnGetViewPointer()->m_fnSetSwitch( SW_REVIEW );
		m_fnSetChangeBtn( SW_REVIEW );
//	}
//	else
//	{
//		TECmsgOut( L"자동측정중에는 메인트관리 화면전환 기능이 제한됩니다.", GREEN_1, 1 );
//	}
#endif
}

LRESULT CEllipsoMeterDlg::m_fnUserMsgSetChartView(WPARAM nNo, LPARAM lParam)
{
	m_fnSetChartView(nNo);
	return 0;
}

void CEllipsoMeterDlg::m_fnSetChartView( int nView )
{

#if !_DEBUG
	if ( m_pTask->stLogin.nLevel == UNKNOWN_USER ) return;
#endif

	m_pStageMap->m_fnSetShowFlag(FALSE);

	switch (m_nPage)
	{
	case SW_AUTO:
		m_pReviewMap->m_fnSetShowFlag(FALSE);
		switch (nView)
		{
		case SW_MAP:
			m_pContour->m_fnSetShowFlag(FALSE);
			m_pMap->m_fnSetShowFlag(TRUE);
			m_pMap->m_fnSetChangeBtn(SW_MAP);
			m_nChart = SW_MAP;
			break;
		case SW_2D:
			m_pContour->m_fnSetShowFlag(TRUE);
			m_pContour->m_fnSetChangeBtn(SW_2D);
			m_pContour->m_bKind = FALSE;
			m_nChart = SW_2D;
			m_pContour->m_fnSetUpdateData();
			break;
		case SW_3D:
			m_pContour->m_fnSetShowFlag(TRUE);
			m_pContour->m_fnSetChangeBtn(SW_3D);
			m_pContour->m_bKind = TRUE;
			m_nChart = SW_3D;
			m_pContour->m_fnSetUpdateData();
			break;
		}
		break;

	case SW_REVIEW:
		m_pMap->m_fnSetShowFlag(FALSE);
		switch (nView)
		{
		case SW_MAP:
			m_pContour->m_fnSetShowFlag(FALSE);
			m_pReviewMap->m_fnSetShowFlag(TRUE);
			m_pReviewMap->m_fnSetChangeBtn(SW_MAP);
			m_nChart = SW_MAP;
			break;
		case SW_2D:
			m_pContour->m_fnSetShowFlag(TRUE);
			m_pContour->m_fnSetChangeBtn(SW_2D);
			m_pContour->m_bKind = FALSE;
			m_nChart = SW_2D;
			m_pContour->m_fnSetUpdateData();
			break;
		case SW_3D:
			m_pContour->m_fnSetShowFlag(TRUE);
			m_pContour->m_fnSetChangeBtn(SW_3D);
			m_pContour->m_bKind = TRUE;
			m_nChart = SW_3D;
			m_pContour->m_fnSetUpdateData();
			break;
		}
		break;
	}

}



void CEllipsoMeterDlg::OnBnClickedBtnLogin()
{
	if ( m_pTask->stLogin.bLogin )
	{
		m_pTask->stLogin.bLogin = FALSE;
		m_pTask->stLogin.nLevel = UNKNOWN_USER;
		memset( m_pTask->stLogin.szUserID, 0x00, sizeof(m_pTask->stLogin.szUserID)); 
		memset( m_pTask->stLogin.szPasswd, 0x00, sizeof(m_pTask->stLogin.szPasswd)); 
		m_fnSetBtnOperation( IDC_BTN_LOGIN,L_BIT_BTN_OFF,L_BIT_CHAR_OFF,"로그인");
		m_pAuto->m_fnSetBtnEnable(100);
		m_pAuto->m_iRadioMeasureMode = -1;

	}
	else
	{
		m_pLogin->m_fnSetShowFlag( TRUE );
	}
}

void CEllipsoMeterDlg::OnSetFocus(CWnd* pOldWnd)
{
	CDialogEx::OnSetFocus(pOldWnd);

}

void CEllipsoMeterDlg::Initial_Interface()
{
	int i = 0;
	int j = 0;
	int iPort = 0;//Com (iPort+1)
	int iBaud = 6;//CBR_57600
	int iDataBit = 3;//8bit
	int iStopBit = 0;//one bit
	int iParity = 0;//none

	int chNum;

	for (i = 0; i < SR_CNT; i++)
	{
		if (pCommTh[i]->m_bConnected == FALSE)//포트가 닫혀 있을 경우에만 포트를 열기 위해
		{
			if (pCommTh[i]->OpenPort
			(
				pCommTh[i]->byIndexComPort(iPort),
				pCommTh[i]->byIndexBaud(iBaud),
				pCommTh[i]->byIndexData(iDataBit),
				pCommTh[i]->byIndexStop(iStopBit),
				pCommTh[i]->byIndexParity(iParity))
				== TRUE)
			{
				//컨트롤러 2개일때
				//Ch : 각각 0~~~>
				//변수배열 : 0~ch통합순차적으로..
				//컨트롤러 1개일때
				//Ch : 통합순차적으로....
				//변수배열 : 0~ch통합순차적으로..
				for (j = 0; j < 1; j++)
				{
					chNum = (j + 1) - (i*8);
					LightControl(i, chNum, 0);
				}
			}
		}
		else
		{
			AfxMessageBox("Already Port Open");
		}
	}
}


//------------------------------------------------------------------------
// 파일의 경로를 읽어온다.
//------------------------------------------------------------------------
CString CEllipsoMeterDlg::m_fnGetResultFileFullPath( )
{
	CString strMemo("");
	char szFullPath[MAX_PATH + 1];
	memset( szFullPath, 0x00, sizeof( szFullPath ));

	sprintf_s( szFullPath, "%s%s", m_Util.m_fnGetCurrentDrive().GetString(), DEFAULT_RESULT_PATH );
	szFullPath[MAX_PATH] = NULL;
	m_Util.m_fnSetCreateDir( szFullPath );

	szFullPath[MAX_PATH] = NULL;
	return szFullPath;
}

//------------------------------------------------------------------------
// EFU 파일의 경로를 읽어온다.
//------------------------------------------------------------------------
CString CEllipsoMeterDlg::m_fnGetStandardExcelFileFullPath( )
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
CString CEllipsoMeterDlg::m_fnGetTempPath( )
{
	CString strMemo("");
	char szFullPath[MAX_PATH + 1];
	memset( szFullPath, 0x00, sizeof( szFullPath ));

	sprintf_s( szFullPath, "%s%s%s", m_Util.m_fnGetCurrentDrive().GetString(), DEFAULT_PATH, DEFAULT_TEMP_PATH );
	szFullPath[MAX_PATH] = NULL;
	m_Util.m_fnSetCreateDir( szFullPath );

	szFullPath[MAX_PATH] = NULL;
	return szFullPath;
}


void CEllipsoMeterDlg::m_fnSetXLSResult( int nMode )
{
	CString strMsg("");
	CString strData("");
	CString strFullPath("");
	CString strOriginExcelFile("");
	CString strExcelFilePath("");
	CString strExcelFileName("");
	CString strXMLFilePath("");
	CString strXMLFileName("");
	CString strExcelTempPath("");
	CString strExcelTempName("");
	int     nFind = -1;
	char    szFile[FILE_LEN*2] = { 0, };

	CString fileName;
	CStringArray strLst;
	CXmlElement *pElement     = NULL;
	CXmlElement *pRootElement = NULL;
	strMsg = "Result Data EXCEL 저장 시작";
	m_fnLogWrite( strMsg );

	CString strSEDataPath;
	strSEDataPath.Format("D:\\%sSE data\\%s\\%s_%s\\%d\\",
		DEFAULT_RESULT_PATH, m_stModel.stUser.szDate, m_stModel.szLotStartTime, m_stModel.stUser.szLotID, m_stModel.nSlotNo);
	sprintf_s(m_stModel.stUser.szDataFile, "%s", strSEDataPath.GetString());
	sprintf_s(m_stModel.stUser.szRecipeFileName, "%s", pWorkRun->m_strRecipeName.GetString());
	m_stModel.stUser.iSlotNo = m_stModel.nSlotNo;

	TECmsgOut(L"EXCEL Sheet 생성을 시작합니다.", GREEN_1, 1000);
	strExcelFilePath.Format( "%s%s\\%s_%s\\", m_pTask->stCommon.szExcelResultPath, m_stModel.stUser.szDate, m_stModel.szLotStartTime, m_stModel.stUser.szLotID);

	CString strMeasureMode;
	switch (m_stModel.stUser.iMeasureMode)
	{
	case PNT_CIRCLE:
		strMeasureMode += "Circle";
		break;
	case PNT_RECT:
		strMeasureMode += "Rectangle";
		break;
	case PNT_MODEL:
		strMeasureMode += m_stModel.szInspModel;
		break;
	case PNT_PATT:
		strMeasureMode += "Pattern";
		break;
	default:
		strMeasureMode += "Manual";
		break;
	}

	strExcelFileName.Format( "%02d_%s_%s.XLSX", m_stModel.nSlotNo, strMeasureMode, pWorkRun->m_strRecipeName);

	m_Util.m_fnSetCreateDir(strExcelFilePath);

	sprintf_s(szFile, "%s", strExcelFileName.GetString());
	CFileDialog fileDlg(FALSE, NULL, strExcelFilePath, OFN_EXPLORER | OFN_HIDEREADONLY, "파일 선택(*.XLSX) | *.XLSX");
	fileDlg.m_ofn.lpstrInitialDir = strExcelFilePath;
	// 파일 정보 버퍼 및 크기 지정
	fileDlg.m_ofn.lpstrFile = szFile;
	fileDlg.m_ofn.nMaxFile = FILE_LEN * 2;
	if (fileDlg.DoModal() == IDOK)
	{
		strExcelTempName = fileDlg.GetFileName();
		strExcelTempName.Trim();
		if (!strExcelTempName.IsEmpty())
		{
			strExcelTempPath = fileDlg.GetPathName();				// 경로 받기
			nFind = strExcelTempPath.ReverseFind('\\');
			strExcelTempPath = strExcelTempPath.Left(nFind + 1);
			strExcelTempName.Replace(".XLSX", "");
			strExcelTempName += ".XLSX";							// 파일명 받기
			strExcelFilePath = strExcelTempPath;
			strExcelFileName = strExcelTempName;
		}
	}
	else
	{
		return;
	}

	m_Util.m_fnSetCreateDir( strExcelFilePath );

	strFullPath.Format("%s%s", strExcelFilePath, strExcelFileName);
	if ( !::CopyFile(m_fnGetStandardExcelFileFullPath(), strFullPath,FALSE))
	{
		strMsg.Format( "결과파일.XLSX(%s) 파일을 결과파일(%s%s)로 복사하지 못했습니다.", m_fnGetStandardExcelFileFullPath(), strFullPath);
		m_fnLogWrite( strMsg );
		return;
	}
	

	strMsg.Format("결과파일.XLSX(%s) 파일을 결과파일(%s%s)로 복사", m_fnGetStandardExcelFileFullPath(),	strExcelFilePath, strExcelFileName);
	m_fnLogWrite(strMsg);

	m_Excel.StartExcel();
	if ( !m_Excel.m_bIsExcelStarted ) return;

	thread thrdMsg = thread(ThrdMsgReport);
	thrdMsg.detach();


	m_Excel.OpenWorkBook( strExcelFilePath + strExcelFileName );

	m_Excel.SelectSheet( "검사결과" );

	//LOT ID
	strData.Format("%s", m_stModel.stUser.szLotID);
	m_Excel.InsertString(3, 3, strData);
	//WaferSize
	strData.Format("%.3f", m_stModel.stUser.dSampleSize);
	m_Excel.InsertString(3, 4, strData);
	//Thickness
	strData.Format("%.3f", m_stModel.stUser.dThickness);
	m_Excel.InsertString(5, 4, strData);
	//Exclusion
	strData.Format("%.3f", m_stModel.stUser.dExclusion);
	m_Excel.InsertString(7, 4, strData);
	//측정Recipe
	strData.Format("%s", m_stModel.stUser.szRecipeFileName);
	m_Excel.InsertString(3, 5, strData);
	//자동/수동 투입
	strData.Format("%s", m_stModel.stUser.szInsertMode);
	m_Excel.InsertString(3, 6, strData);
	//웨이퍼얼라인
	strData.Format("%s", m_stModel.stUser.szWaferAlignMode);
	m_Excel.InsertString(5, 6, strData);
	//Vision AF
	strData.Format("%s", m_stModel.stUser.szVisionAFMode);
	m_Excel.InsertString(7, 6, strData);
	//검사일자
	strData.Format("%s", m_stModel.stUser.szDate);
	m_Excel.InsertString(3, 7, strData);
	//Slot No.
	strData.Format("%02d", m_stModel.stUser.iSlotNo);
	m_Excel.InsertString(5, 7, strData);
	//Op. ID
	strData.Format("%s", m_stModel.stUser.szOperatorID);
	m_Excel.InsertString(7, 7, strData);
	//검사시작시각
	strData.Format("%s", m_stModel.stUser.szTimeStart);
	m_Excel.InsertString(3, 8, strData);
	//검사종료시각
	strData.Format("%s", m_stModel.stUser.szTimeEnd);
	m_Excel.InsertString(5, 8, strData);
	//검사시간
	strData.Format("%s", m_stModel.stUser.szTimeSpend);
	m_Excel.InsertString(7, 8, strData);
	//파일경로
	strData.Format("%s", m_stModel.stUser.szDataFile);
	m_Excel.InsertString(3, 9, strData);
	//Description
	strData.Format("%s", m_stModel.stUser.szDescription);
	m_Excel.InsertString(3, 10, strData);

// 	// 모델명
// 	m_Excel.InsertString(3, 4, m_stModel.szInspModel);
// 	// 결과저장 경로
// 	m_Excel.InsertString(3, 5, strExcelFilePath);
// 	// 검사시각
// 	m_Excel.InsertString(3, 6, m_stModel.szInspDateTime);
// 	// 검사시간
// 	m_Excel.InsertString(3, 7, m_stModel.szInspTackTime);
// 	// SLOT NO
// 	strData.Format("%01d", m_stModel.nSlotNo);
// 	m_Excel.InsertString(3, 7, strData);
// 
// 	// LOTID
// 	m_Excel.InsertString(3, 8, m_stModel.stUser.szLotID );
// 	// Sample Size(mm) - Sample
// 	strData.Format( "%01d", (int)m_stModel.stUser.dSampleSize );
// 	m_Excel.InsertString(3, 9, strData );
// 	// Thickness
// 	strData.Format( "%01d", (int)m_stModel.stUser.dThickness );
// 	m_Excel.InsertString(3, 10, strData );
// 	// Op ID
// 	m_Excel.InsertString(3, 11, m_stModel.stUser.szOperatorID);
// 	// Description
// 	m_Excel.InsertString(3, 12, m_stModel.stUser.szDescription );
// 
// 	// DATE
// 	m_Excel.InsertString(5, 8, m_stModel.stUser.szDate );
// 	// Flat
// 	strData.Format( "%7.3f", m_stModel.stUser.dFlat );
// 	m_Excel.InsertString(5, 9, strData );
// 	// Sample Meterial
// 	m_Excel.InsertString(5, 10, m_stModel.stUser.szMaterial );
// 
// 	// Time
// 	m_Excel.InsertString(7, 8, m_stModel.stUser.szTime );
// 	// Exclusion
// 	strData.Format( "%7.3f", m_stModel.stUser.dExclusion );
// 	m_Excel.InsertString(7, 9, strData );
// 	// Thin film material
// 	m_Excel.InsertString(7, 10, m_stModel.stUser.szThinfilmmaterial );
// 	// Thin film thickness
// 	strData.Format( "%7.3f", m_stModel.stUser.dThinfilmthickness );
// 	m_Excel.InsertString(7, 11, strData );

	CString strWork("");
	CString strLayer("");
	CString strAxis("");
	CString str2D("");
	CString str3D("");
	CString strRow("");
	CString strCol("");
	int     nRow = 0;
	int     nCol = 0;
	for( int i = 0; i < m_stModel.nLayer; i++ )
	{
		strLayer.Format(EXCEL_LAYER, i + 1);
		m_Excel.SelectSheet(strLayer);
		strLayer.Format("%01d", i + 1);
		m_Excel.InsertString(14, 12, strLayer);
		int nBase = 15;
		for ( int j = 0; j < m_stModel.nSRData; j++ )
		{
			if ( j <= MAX_DATA )
			{
				strWork.Format( "%s", m_stModel.stSRData[j].szName );
				strWork.Trim();
				if ( !strWork.IsEmpty() )
				{
					// TARGET_NO
					m_Excel.InsertString(3, nBase + j, m_stModel.stSRData[j].szName );

					// SUB POINT
					strWork.Format( "%01d", m_stModel.stSRData[j].iSub );
					m_Excel.InsertString(4, nBase + j, strWork );

					// X축
					strWork.Format("%7.3f", m_stModel.stSRData[j].dwXPosition);
					m_Excel.InsertString(5, nBase + j, strWork);

					// Y축
					strWork.Format( "%7.3f", m_stModel.stSRData[j].dwYPosition );
					m_Excel.InsertString(6, nBase + j, strWork );

					// Z축
					strWork.Format( "%7.3f", m_stModel.stSRData[j].dwZPosition );
					m_Excel.InsertString(7, nBase + j, strWork );

					// MSE
					strWork.Format( "%7.3f", m_stModel.stSRData[j].dwResult[i][RES_MSE] );
					m_Excel.InsertString(8, nBase + j, strWork );

					// TOTAL THICKNESS
					strWork.Format("%7.3f", m_stModel.stSRData[j].dwResult[i][RES_TOTAL_THICKNESS]);
					m_Excel.InsertString(9, nBase + j, strWork);

					// THICKNESS
					strWork.Format("%7.3f", m_stModel.stSRData[j].dwResult[i][RES_THICKNESS]);
					m_Excel.InsertString(10, nBase + j, strWork);

					// N-550
					strWork.Format("%7.3f", m_stModel.stSRData[j].dwResult[i][RES_N_550]);
					m_Excel.InsertString(11, nBase + j, strWork);

					// K-550
					strWork.Format("%7.3f", m_stModel.stSRData[j].dwResult[i][RES_K_550]);
					m_Excel.InsertString(12, nBase + j, strWork);

					// N-632
					strWork.Format("%7.3f", m_stModel.stSRData[j].dwResult[i][RES_N_632]);
					m_Excel.InsertString(13, nBase + j, strWork);

					// K-632
					strWork.Format("%7.3f", m_stModel.stSRData[j].dwResult[i][RES_K_632]);
					m_Excel.InsertString(14, nBase + j, strWork);
				}
			}
		}
		
		nRow = 28;
		nCol = 5;
		for (int k = 0; k < MAX_ITEM; k++)
		{
			// MIN
			strWork.Format("%7.3f", m_stModel.dInspMin[i][k]);
			m_Excel.InsertString(9, 5 + k, strWork);

			// MAX
			strWork.Format("%7.3f", m_stModel.dInspMax[i][k]);
			m_Excel.InsertString(10, 5 + k, strWork);

			// MAX-MIN
			strWork.Format("%7.3f", m_stModel.dInspMax_Min[i][k]);
			m_Excel.InsertString(11, 5 + k, strWork);

			// AVG
			strWork.Format("%7.3f", m_stModel.dInspAvg[i][k]);
			m_Excel.InsertString(12, 5 + k, strWork);

			// UNI
			strWork.Format("%7.3f", m_stModel.dInspUniformity[i][k]);
			m_Excel.InsertString(13, 5 + k, strWork);

			// STD
			strWork.Format("%7.3f", m_stModel.dInspStd[i][k]);
			m_Excel.InsertString(14, 5 + k, strWork);

			strAxis.Format("AXIS_%01d_%01d.BMP", i + 1, k + 1);
			if (m_Util.Existence(m_fnGetTempPath() + strAxis) == FM_FILE)
			{
				strCol.Format("Q%01d", nCol + (k * 25));
				strRow.Format("X%01d", nRow + (k * 25));
				m_Excel.InsertPicture(strCol, strRow, m_fnGetTempPath() + strAxis);
			}

			if (m_stModel.bChartReport)
			{
				str2D.Format("2D_%01d_%01d.BMP", i + 1, k + 1);
				if (m_Util.Existence(m_fnGetTempPath() + str2D) == FM_FILE)
				{
					strCol.Format("Z%01d", nCol + (k * 25));
					strRow.Format("AG%01d", nRow + (k * 25));
					m_Excel.InsertPicture(strCol, strRow, m_fnGetTempPath() + str2D);
				}
				str3D.Format("3D_%01d_%01d.BMP", i + 1, k + 1);
				if (m_Util.Existence(m_fnGetTempPath() + str3D) == FM_FILE)
				{
					strCol.Format("AI%01d", nCol + (k * 25));
					strRow.Format("AP%01d", nRow + (k * 25));
					m_Excel.InsertPicture(strCol, strRow, m_fnGetTempPath() + str3D);
				}
			}
		}
	}

	m_Excel.SaveData();

	m_Excel.CloseWorkBook();
	m_Excel.QuitExcel(TRUE);
#if MIL_USE
	pWorkRun->m_bThrdFlag[TF_MAKE_REPORT] = FALSE;
#endif
	TECmsgOut(L"EXCEL Sheet 생성을 종료합니다.", GREEN_1, 1000);
	strMsg = "Result Data EXCEL 저장 완료";
	m_fnLogWrite( strMsg );
}


void CEllipsoMeterDlg::OnClose()
{
	CDialogEx::OnClose();
}


void CEllipsoMeterDlg::OnBnClickedBtnLampOff()
{
#if MIL_USE
	if (!pWorkRun->m_bThrdFlag[TF_AUTORUN])
	{
		thread thrdReadReceipe = thread(ThrdLampOnCheck, !m_bLampOn);
		thrdReadReceipe.detach();
	}
	else
	{
		if (!m_bLampOn)
		{
			theApp.m_fnGetViewPointer()->m_pManual->m_pElipso->m_fnSetSystemLampOn();
			TECmsgOut(L"Lamp On이 완료되었습니다.", LIGHTBLUE, 1000);
			theApp.m_fnGetViewPointer()->m_bLampOn = TRUE;
			theApp.m_fnGetViewPointer()->m_fnSetBtnOperation(IDC_BTN_LAMP_OFF, L_BIT_BTN_ON, L_BIT_CHAR_ON, "Lamp ON");
		}
	}
#endif
}
