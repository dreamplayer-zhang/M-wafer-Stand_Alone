// IO.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EllipsoMeter.h"
#include "IO.h"
#include "afxdialogex.h"
#include "resource.h"
#include "ATXMotion.h"

// CIO 대화 상자입니다.
//--------------------------------------------------
// 내부 IO INPUT
//--------------------------------------------------
static int btnIO_IN[] = 
{
	IDC_BTN_1_IN_01,
	IDC_BTN_1_IN_02,
	IDC_BTN_1_IN_03,
	IDC_BTN_1_IN_04,
	IDC_BTN_1_IN_05,
	IDC_BTN_1_IN_06,
	IDC_BTN_1_IN_07,
	IDC_BTN_1_IN_08,
	IDC_BTN_1_IN_09,
	IDC_BTN_1_IN_10,
	IDC_BTN_1_IN_11,
	IDC_BTN_1_IN_12,
	IDC_BTN_1_IN_13,
	IDC_BTN_1_IN_14,
	IDC_BTN_1_IN_15,
	IDC_BTN_1_IN_16
};
//--------------------------------------------------
// 내부 IO OUTOUT
//--------------------------------------------------
static int btnIO_OUT[] = 
{
	IDC_BTN_1_OUT_01,
	IDC_BTN_1_OUT_02,
	IDC_BTN_1_OUT_03,
	IDC_BTN_1_OUT_04,
	IDC_BTN_1_OUT_05,
	IDC_BTN_1_OUT_06,
	IDC_BTN_1_OUT_07,
	IDC_BTN_1_OUT_08,
	IDC_BTN_1_OUT_09,
	IDC_BTN_1_OUT_10,
	IDC_BTN_1_OUT_11,
	IDC_BTN_1_OUT_12,
	IDC_BTN_1_OUT_13,
	IDC_BTN_1_OUT_14,
	IDC_BTN_1_OUT_15,
	IDC_BTN_1_OUT_16
};
//--------------------------------------------------
// IO INPUT
//--------------------------------------------------
static CString szIO_IN[] = 
{
	"Wafer 감지센서 6",
	"Wafer 감지센서 8",
	"Wafer 감지센서 12",
	"EFEM 투입완료",
	"EFEM 투입중",
	"",
	"",
	"",
	"시스템 알람",
	"EMS 1",
	"EMS 2",
	"EMS 3",
	"EMS 4",
	"",
	"",
	""
};

//--------------------------------------------------
// IO OUTPUT
//--------------------------------------------------

static CString szIO_OUTPUT[] = 
{
	"Vacuum 6",
	"Vacuum 8",
	"Vacuum 12",
	"",
	"",
	"Measure Ready",
	"Measure Busy",
	"",
	"시스템 알람",
	"",
	"",
	"",
	"",
	"Wafer 0b01",
	"Wafer 0b10",
	""
};
IMPLEMENT_DYNAMIC(CIO, CDialogEx)

CIO::CIO(CWnd* pParent /*=NULL*/)
	: CDialogEx(CIO::IDD, pParent)
{

	this->Create( IDD_IO );

	m_bShow = FALSE;

	m_hIoThread			= NULL;
	m_bIoEnd			= FALSE;
	m_bProcessing		= FALSE;

	m_hStageInOutThread	= NULL;
	m_bStageInOutEnd	= FALSE;

	m_nStageInOut       = -1;
	m_bOperation		= FALSE;
	m_nLoading          = 0;
	m_nStart            = 0;
	m_bLoadingStop		= false;
	m_bProcess          = FALSE;
}

CIO::~CIO()
{
}

void CIO::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIO)
	for( int i = 0; i < IO_IN_CNT; i++ )
	{
		DDX_Control(pDX, btnIO_IN[i], m_btnIn[i]);
	}
	for( int i = 0; i < IO_OUT_CNT; i++ )
	{
		DDX_Control(pDX, btnIO_OUT[i], m_btnOutput[i]);
	}
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIO, CDialogEx)
// 	ON_MESSAGE(WM_SET_IO, m_fnIoRead ) 
// 	ON_BN_CLICKED(IDC_BTN_EXIT, &CIO::OnBnClickedBtnExit)
// 	ON_BN_CLICKED(IDC_BTN_SAVE, &CIO::OnBnClickedBtnSave)
// 	ON_BN_CLICKED(IDC_BTN_1_OUT_01, &CIO::OnBnClickedBtn1Out01)
// 	ON_BN_CLICKED(IDC_BTN_1_OUT_02, &CIO::OnBnClickedBtn1Out02)
// 	ON_BN_CLICKED(IDC_BTN_1_OUT_03, &CIO::OnBnClickedBtn1Out03)
// 	ON_BN_CLICKED(IDC_BTN_1_OUT_04, &CIO::OnBnClickedBtn1Out04)
// 	ON_BN_CLICKED(IDC_BTN_1_OUT_05, &CIO::OnBnClickedBtn1Out05)
// 	ON_BN_CLICKED(IDC_BTN_1_OUT_06, &CIO::OnBnClickedBtn1Out06)
// 	ON_BN_CLICKED(IDC_BTN_1_OUT_07, &CIO::OnBnClickedBtn1Out07)
// 	ON_MESSAGE(UM_LOADING_BUTTON, m_fnSetLoadingButton)
// 	ON_MESSAGE(UM_LOADING_SW_LIGHT, m_fnSetLoadingSwLight)
// 	ON_MESSAGE(UM_START_SW_LIGHT, m_fnSetStartSwLight)
// 	ON_MESSAGE(UM_SIGNAL_TOWER, m_fnSetSignalTower)
// 	ON_MESSAGE(UM_BUZZER, m_fnSetBuzzer)
// 	ON_MESSAGE(UM_VACUUM, m_fnSetVacuum)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------------------------
// 외부 참조용 Function
//---------------------------------------------------------------------------------------------
// BOOL CIO::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
// {
// 	if (( nID   == 2 ) &&
// 		( nCode == -1 ))
// 	{
// 		m_fnSetShowFlag( FALSE );
// 	}
// 	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
// }
// 
// void CIO::m_fnSetShowFlag( BOOL bShow )
// {
// 	m_bShow = bShow;
// 	if ( m_bShow )
// 	{
// 		this->ShowWindow( SW_SHOW );
// 	}
// 	else
// 	{
// 		this->ShowWindow( SW_HIDE );
// 	}
// }
// 
// BOOL CIO::PreTranslateMessage(MSG* pMsg) 
// {
// 	// ENTER KEY or ESC KEY CANCEL
// 	if (( pMsg->wParam == 13 ) || 
// 		( pMsg->wParam == 27 ))
// 	{
// 		return FALSE;
// 	}
// 
// 	return CDialog::PreTranslateMessage(pMsg);
// }
// 
// //---------------------------------------------------------------------------------------------------------
// // 시작에 필요한 작업을 한다.
// //---------------------------------------------------------------------------------------------------------
// void CIO::m_fnSetInit( ) 
// {
// 	CString s("");
// 	s.Format( "IO 메인창" );
// 	this->SetWindowText(s);
// 
// 	m_Log.m_fnSetLogPath( "IO", "IO" );
// 	m_Log.m_fnSetLogWriteFlag( TRUE );
// 
// 	m_HexaLog.m_fnSetLogPath( "IO", "IO_HEXA" );
// 	m_HexaLog.m_fnSetLogWriteFlag( TRUE );
// 
// 	m_pTaskHandle = new CTaskHandle( this, SHARED_NAME, 
// 									 1, 
// 									 sizeof( SHEET_RESISTANCE_Format ) + 1,
// 									 0,
// 									 FALSE );
// 
// 
// 	m_pTask					= ( SHEET_RESISTANCE_Format* )m_pTaskHandle->m_pFilePointer( );
// 	m_pTask->stElipsoIO.bConnect  = FALSE;
// 
// 	for ( int i = 0; i < IO_IN_CNT; i++ )
// 	{
// 		m_fnSetBtnOperation( btnIO_IN[i], L_BIT_BTN_OFF, L_BIT_CHAR_OFF, szIO_IN[i]);
// 	}
// 
// 	for ( int i = 0; i < IO_OUT_CNT; i++ )
// 	{
// 		m_fnSetBtnOperation( btnIO_OUT[i], L_BIT_BTN_OFF, L_BIT_CHAR_OFF, szIO_OUTPUT[i]);
// 		m_fnSetOutputSignal( i, FALSE );
// 	}
// 	this->PostMessage( UM_SIGNAL_TOWER, SIGNAL_TOWER_YELLOW, L_BIT_ON );
// 	this->PostMessage( UM_BUZZER, BUZZER_ONOFF, L_BIT_OFF );
// 	this->PostMessage( UM_VACUUM, VACUUM_ON_OFF, L_BIT_OFF );
// 
// 	unsigned long	ThreadID;
// 	m_bIoEnd    = TRUE;
// 	m_hIoThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)IoThread, this, NULL, &ThreadID);
// }
// 
// //---------------------------------------------------------------------------------------------------------
// // 종료에 필요한 작업을 한다.
// //---------------------------------------------------------------------------------------------------------
// void CIO::m_fnSetEnd() 
// {
// 	m_bProcessing = TRUE;
// 
// 	if ( m_hStageInOutThread != NULL )
// 	{
// 		m_bStageInOutEnd = FALSE;
// 		SuspendThread( m_hStageInOutThread );
// 		CloseHandle( m_hStageInOutThread );
// 		m_hStageInOutThread = NULL;
// 	}
// 
// 	if ( m_hIoThread != NULL )
// 	{
// 		m_bIoEnd			= FALSE;
// 		SuspendThread( m_hIoThread );
// 		CloseHandle(m_hIoThread);
// 		m_hIoThread	    = NULL;
// 	}
// 
// 	for( int i = 0; i < IO_OUT_CNT; i++ )
// 	{
// 		m_fnSetOutputSignal( i, FALSE );
// 	}
// 
// 	Sleep( 100 );
// 
// 	if ( m_pTaskHandle != NULL )
// 	{
// 		m_pTask->stElipsoIO.bConnect = FALSE;
// 		m_pTaskHandle->m_fnSetEnd();
// 		delete m_pTaskHandle;
// 		m_pTaskHandle = NULL;
// 	}
// }
// 
// void CIO::OnBnClickedBtnExit()
// {
// 	m_fnSetShowFlag( FALSE );
// }
// 
// void CIO::OnBnClickedBtnSave()
// {
// }
// 
// //---------------------------------------------------------------------------------------------------------
// // Log 파일을 저장한다.
// //---------------------------------------------------------------------------------------------------------
// void CIO::m_fnLogWrite( CString strMsg )
// {
// 	CString strCnt("");
// 	int     nCnt		= 0;
// 	int     nTotal		= 0;
// 
// 	memset( m_szData, 0x00, sizeof( m_szData ));
// 
// 	sprintf( m_szData, "%s", strMsg );
// 	if ( m_pTask == NULL ) return;
// 	if ( m_pTask->stCommon.bLogSave )
// 	{
// 		m_Log.DEBUG_LOG( m_szData );
// 	}
// 
// 	if ( m_hWnd == NULL )
// 	{
// 		return;
// 	}
// }
// 
// //------------------------------------------------------------
// // 각 버튼의 색상과 글자색을 변경한다.
// //------------------------------------------------------------
// void CIO::m_fnSetBtnOperation( int nID, 
// 							   int nBtnColor, 
// 							   int nTextColor,
// 							   CString strMsg )
// {
// 	if ( this->m_hWnd == NULL ) return;
// 
// 	CColorBtn* pColorBtn = NULL;
// 	CRect rect;
// 	pColorBtn = (CColorBtn*) GetDlgItem( nID );
// 	if ( pColorBtn == NULL ) return;
// 	pColorBtn->m_fnSetParam( nBtnColor, nTextColor );
// 	GetDlgItem( nID )->GetWindowRect(rect);
// 	if ( !strMsg.IsEmpty() )
// 	{
// 		GetDlgItem( nID )->SetWindowText( strMsg );
// 	}
// 	ScreenToClient( rect );
// 	InvalidateRect( rect, FALSE );
// }

UINT IoThread(LPVOID pFuncData)
{
	CIO		*pQI = (CIO *)pFuncData;

	while ( pQI->m_bIoEnd )
	{
		Sleep( IO_READ_TIME );
		if ( pQI->m_hWnd != NULL )
		{
#if AJINTEX_BOARD
			pQI->PostMessage( WM_SET_IO, 0,0 );
#endif
		}

		if (!pQI->m_pTask->stElipsoIO.stIn.EMS_1
			|| !pQI->m_pTask->stElipsoIO.stIn.EMS_2
			|| !pQI->m_pTask->stElipsoIO.stIn.EMS_3
			|| !pQI->m_pTask->stElipsoIO.stIn.EMS_4)
		{
			pMotion->m_bEmergency = TRUE;
			pMotion->StopAxisAll(true);
		}
		else if(pQI->m_pTask->stElipsoIO.stIn.EMS_1
			&& pQI->m_pTask->stElipsoIO.stIn.EMS_2
			&& pQI->m_pTask->stElipsoIO.stIn.EMS_3
			&& pQI->m_pTask->stElipsoIO.stIn.EMS_4
			&& pMotion->m_bEmergency)
		{
			pMotion->m_bEmergency = FALSE;
		}
	}

	if ( pQI->m_hWnd == NULL ) return 0;

	if ( pQI->m_hIoThread != NULL )
	{
		SuspendThread(pQI->m_hIoThread);
		CloseHandle(pQI->m_hIoThread);
		pQI->m_hIoThread	= NULL;
	}
	return 0;
}

// LRESULT CIO::m_fnIoRead(  WPARAM nNo, LPARAM lParam )
// {
// #if AJINTEX_BOARD
// #if !EQ_DEBUG
// 	if ( m_bProcessing ) return 0;
// 	if ( !m_bIoEnd ) return 0;
// 
// 	m_bProcessing	= TRUE;
// 	DWORD upOn      = 0;
// 	int   nCnt      = 0;
// 	//---------------------------------
// 	// INPUT
// 	//---------------------------------
// 	// Jog X+
// 	if ( theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmSignalReadInputBit( X_AXIS, BIT_2, upOn ) == RC_NORMAL )
// 	{
// 		if ( m_pTask->stElipsoIO.stIn.Jog_X_Plus != upOn )
// 		{
// 			if ( upOn  == L_BIT_ON )
// 			{
// 				m_fnSetBtnOperation( btnIO_IN[nCnt], L_BIT_BTN_OFF, L_BIT_CHAR_OFF, szIO_IN[nCnt]);
// 			}
// 			else
// 			{
// 				m_fnSetBtnOperation( btnIO_IN[nCnt], L_BIT_BTN_ON, L_BIT_CHAR_ON, szIO_IN[nCnt]);
// 			}
// 		}
// 		m_pTask->stElipsoIO.stIn.Jog_X_Plus = upOn;
// 	}
// 	nCnt++;
// 	// Jog X-
// 	if ( theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmSignalReadInputBit( X_AXIS, BIT_3, upOn ) == RC_NORMAL )
// 	{
// 		if ( m_pTask->stElipsoIO.stIn.Jog_X_Minus != upOn )
// 		{
// 			if ( upOn  == L_BIT_ON )
// 			{
// 				m_fnSetBtnOperation( btnIO_IN[nCnt], L_BIT_BTN_OFF, L_BIT_CHAR_OFF, szIO_IN[nCnt]);
// 			}
// 			else
// 			{
// 				m_fnSetBtnOperation( btnIO_IN[nCnt], L_BIT_BTN_ON, L_BIT_CHAR_ON, szIO_IN[nCnt]);
// 			}
// 		}
// 		m_pTask->stElipsoIO.stIn.Jog_X_Minus = upOn;
// 	}
// 	nCnt++;
// 	// Jog Y+
// 	if ( theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmSignalReadInputBit( X_AXIS, BIT_4, upOn ) == RC_NORMAL )
// 	{
// 		if ( m_pTask->stElipsoIO.stIn.Jog_Y_Plus != upOn )
// 		{
// 			if ( upOn  == L_BIT_ON )
// 			{
// 				m_fnSetBtnOperation( btnIO_IN[nCnt], L_BIT_BTN_OFF, L_BIT_CHAR_OFF, szIO_IN[nCnt]);
// 			}
// 			else
// 			{
// 				m_fnSetBtnOperation( btnIO_IN[nCnt], L_BIT_BTN_ON, L_BIT_CHAR_ON, szIO_IN[nCnt]);
// 			}
// 		}
// 		m_pTask->stElipsoIO.stIn.Jog_Y_Plus = upOn;
// 	}
// 	nCnt++;
// 	// Jog Y-
// 	if ( theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmSignalReadInputBit( Y_AXIS, BIT_2, upOn ) == RC_NORMAL )
// 	{
// 		if ( m_pTask->stElipsoIO.stIn.Jog_Y_Minus != upOn )
// 		{
// 			if ( upOn  == L_BIT_ON )
// 			{
// 				m_fnSetBtnOperation( btnIO_IN[nCnt], L_BIT_BTN_OFF, L_BIT_CHAR_OFF, szIO_IN[nCnt]);
// 			}
// 			else
// 			{
// 				m_fnSetBtnOperation( btnIO_IN[nCnt], L_BIT_BTN_ON, L_BIT_CHAR_ON, szIO_IN[nCnt]);
// 			}
// 		}
// 		m_pTask->stElipsoIO.stIn.Jog_Y_Minus = upOn;
// 	}
// 	nCnt++;
// 	// Jog Z+
// 	if ( theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmSignalReadInputBit( Y_AXIS, BIT_3, upOn ) == RC_NORMAL )
// 	{
// 		if ( m_pTask->stElipsoIO.stIn.Jog_Z_Plus != upOn )
// 		{
// 			if ( upOn  == L_BIT_ON )
// 			{
// 				m_fnSetBtnOperation( btnIO_IN[nCnt], L_BIT_BTN_OFF, L_BIT_CHAR_OFF, szIO_IN[nCnt]);
// 			}
// 			else
// 			{
// 				m_fnSetBtnOperation( btnIO_IN[nCnt], L_BIT_BTN_ON, L_BIT_CHAR_ON, szIO_IN[nCnt]);
// 			}
// 		}
// 		m_pTask->stElipsoIO.stIn.Jog_Z_Plus = upOn;
// 	}
// 	nCnt++;
// 	// Jog Z-
// 	if ( theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmSignalReadInputBit( Y_AXIS, BIT_4, upOn ) == RC_NORMAL )
// 	{
// 		if ( m_pTask->stElipsoIO.stIn.Jog_Z_Minus != upOn )
// 		{
// 			if ( upOn  == L_BIT_ON )
// 			{
// 				m_fnSetBtnOperation( btnIO_IN[nCnt], L_BIT_BTN_OFF, L_BIT_CHAR_OFF, szIO_IN[nCnt]);
// 			}
// 			else
// 			{
// 				m_fnSetBtnOperation( btnIO_IN[nCnt], L_BIT_BTN_ON, L_BIT_CHAR_ON, szIO_IN[nCnt]);
// 			}
// 		}
// 		m_pTask->stElipsoIO.stIn.Jog_Z_Minus = upOn;
// 	}
// 	nCnt++;
// 	// SPEED Low
// 	if ( theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmSignalReadInputBit( Z_AXIS, BIT_2, upOn ) == RC_NORMAL )
// 	{
// 		if ( m_pTask->stElipsoIO.stIn.Jog_Speed_Low != upOn )
// 		{
// 			if ( upOn  == L_BIT_ON )
// 			{
// 				m_fnSetBtnOperation( btnIO_IN[nCnt], L_BIT_BTN_OFF, L_BIT_CHAR_OFF, szIO_IN[nCnt]);
// 			}
// 			else
// 			{
// 				m_fnSetBtnOperation( btnIO_IN[nCnt], L_BIT_BTN_ON, L_BIT_CHAR_ON, szIO_IN[nCnt]);
// 			}
// 		}
// 		m_pTask->stElipsoIO.stIn.Jog_Speed_Low = upOn;
// 	}
// 	nCnt++;
// 	// SPEED High
// 	if ( theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmSignalReadInputBit( Z_AXIS, BIT_3, upOn ) == RC_NORMAL )
// 	{
// 		if ( m_pTask->stElipsoIO.stIn.Jog_Speed_High != upOn )
// 		{
// 			if ( upOn  == L_BIT_ON )
// 			{
// 				m_fnSetBtnOperation( btnIO_IN[nCnt], L_BIT_BTN_OFF, L_BIT_CHAR_OFF, szIO_IN[nCnt]);
// 			}
// 			else
// 			{
// 				m_fnSetBtnOperation( btnIO_IN[nCnt], L_BIT_BTN_ON, L_BIT_CHAR_ON, szIO_IN[nCnt]);
// 			}
// 		}
// 		m_pTask->stElipsoIO.stIn.Jog_Speed_High = upOn;
// 	}
// 	nCnt++;
// 	// Measure Switch
// 	if ( theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmSignalReadInputBit( Z_AXIS, BIT_4, upOn ) == RC_NORMAL )
// 	{
// 		if ( m_pTask->stElipsoIO.stIn.Measure_Switch != upOn )
// 		{
// 			if ( upOn  == L_BIT_ON )
// 			{
// 				m_fnSetBtnOperation( btnIO_IN[nCnt], L_BIT_BTN_OFF, L_BIT_CHAR_OFF, szIO_IN[nCnt]);
// 			}
// 			else
// 			{
// 				m_fnSetBtnOperation( btnIO_IN[nCnt], L_BIT_BTN_ON, L_BIT_CHAR_ON, szIO_IN[nCnt]);
// 			}
// 		}
// 		m_pTask->stElipsoIO.stIn.Measure_Switch = upOn;
// 	}
// // 	nCnt++;
// // 	// Loading Switch
// // 	if ( theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmSignalReadInputBit( R_AXIS, BIT_2, upOn ) == RC_NORMAL )
// // 	{
// // 		if ( m_pTask->stElipsoIO.stIn.Loading_Switcn != upOn )
// // 		{
// // 			if ( upOn  == L_BIT_ON )
// // 			{
// // 				m_fnSetBtnOperation( btnIO_IN[nCnt], L_BIT_BTN_OFF, L_BIT_CHAR_OFF, szIO_IN[nCnt]);
// // 			}
// // 			else
// // 			{
// // 				m_fnSetBtnOperation( btnIO_IN[nCnt], L_BIT_BTN_ON, L_BIT_CHAR_ON, szIO_IN[nCnt]);
// // 			}
// // 		}
// // 		m_pTask->stElipsoIO.stIn.Loading_Switcn = upOn;
// // 	}
// // 	nCnt++;
// // 	// Vacuum On/Off Status
// // 	if ( theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmSignalReadInputBit( R_AXIS, BIT_3, upOn ) == RC_NORMAL )
// // 	{
// // 		if ( m_pTask->stElipsoIO.stIn.Vacuum_on != upOn )
// // 		{
// // 			if ( upOn  == L_BIT_ON )
// // 			{
// // 				m_fnSetBtnOperation( btnIO_IN[nCnt], L_BIT_BTN_OFF, L_BIT_CHAR_OFF, szIO_IN[nCnt]);
// // 			}
// // 			else
// // 			{
// // 				m_fnSetBtnOperation( btnIO_IN[nCnt], L_BIT_BTN_ON, L_BIT_CHAR_ON, szIO_IN[nCnt]);
// // 			}
// // 		}
// // 		m_pTask->stElipsoIO.stIn.Vacuum_on = upOn;
// // 	}
// 
// 	//---------------------------------
// 	// OUTPUT
// 	//---------------------------------
// 	nCnt      = 0;
// 	// Measure Switch Lamp
// 	if ( theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmSignalReadOutputBit( X_AXIS, BIT_2, upOn ) == RC_NORMAL )
// 	{
// 		if ( m_pTask->stElipsoIO.stOut.Measure_Lamp_On != upOn )
// 		{
// 			if ( upOn  == L_BIT_ON )
// 			{
// 				m_fnSetBtnOperation( btnIO_OUT[nCnt], L_BIT_BTN_OFF, L_BIT_CHAR_OFF, szIO_OUTPUT[nCnt]);
// 			}
// 			else
// 			{
// 				m_fnSetBtnOperation( btnIO_OUT[nCnt], L_BIT_BTN_ON, L_BIT_CHAR_ON, szIO_OUTPUT[nCnt]);
// 			}
// 		}
// 		m_pTask->stElipsoIO.stOut.Measure_Lamp_On = upOn;
// 	}
// 	nCnt++;
// 	// Loading Switch Lamp
// 	if ( theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmSignalReadOutputBit( X_AXIS, BIT_3, upOn ) == RC_NORMAL )
// 	{
// 		if ( m_pTask->stElipsoIO.stOut.Loading_Switch_Lamp_On != upOn )
// 		{
// 			if ( upOn  == L_BIT_ON )
// 			{
// 				m_fnSetBtnOperation( btnIO_OUT[nCnt], L_BIT_BTN_OFF, L_BIT_CHAR_OFF, szIO_OUTPUT[nCnt]);
// 			}
// 			else
// 			{
// 				m_fnSetBtnOperation( btnIO_OUT[nCnt], L_BIT_BTN_ON, L_BIT_CHAR_ON, szIO_OUTPUT[nCnt]);
// 			}
// 		}
// 		m_pTask->stElipsoIO.stOut.Loading_Switch_Lamp_On = upOn;
// 	}
// 	nCnt++;
// 	// Vacuum On/Off
// 	if ( theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmSignalReadOutputBit( Y_AXIS, BIT_2, upOn ) == RC_NORMAL )
// 	{
// 		if ( m_pTask->stElipsoIO.stOut.Vacuum_On != upOn )
// 		{
// 			if ( upOn  == L_BIT_ON )
// 			{
// 				m_fnSetBtnOperation( btnIO_OUT[nCnt], L_BIT_BTN_OFF, L_BIT_CHAR_OFF, szIO_OUTPUT[nCnt]);
// 			}
// 			else
// 			{
// 				m_fnSetBtnOperation( btnIO_OUT[nCnt], L_BIT_BTN_ON, L_BIT_CHAR_ON, szIO_OUTPUT[nCnt]);
// 			}
// 		}
// 		m_pTask->stElipsoIO.stOut.Vacuum_On = upOn;
// 	}
// 	nCnt++;
// 	// Signal Tower RED
// 	if ( theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmSignalReadOutputBit( Z_AXIS, BIT_2, upOn ) == RC_NORMAL )
// 	{
// 		if ( m_pTask->stElipsoIO.stOut.Tower_Lamp_Red != upOn )
// 		{
// 			if ( upOn  == L_BIT_ON )
// 			{
// 				m_fnSetBtnOperation( btnIO_OUT[nCnt], L_BIT_BTN_OFF, L_BIT_CHAR_OFF, szIO_OUTPUT[nCnt]);
// 			}
// 			else
// 			{
// 				m_fnSetBtnOperation( btnIO_OUT[nCnt], L_BIT_BTN_ON, L_BIT_CHAR_ON, szIO_OUTPUT[nCnt]);
// 			}
// 		}
// 		m_pTask->stElipsoIO.stOut.Tower_Lamp_Red = upOn;
// 	}
// 	nCnt++;
// 	// Signal Tower YELLOW
// 	if ( theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmSignalReadOutputBit( Z_AXIS, BIT_3, upOn ) == RC_NORMAL )
// 	{
// 		if ( m_pTask->stElipsoIO.stOut.Tower_Lamp_Yellow != upOn )
// 		{
// 			if ( upOn  == L_BIT_ON )
// 			{
// 				m_fnSetBtnOperation( btnIO_OUT[nCnt], L_BIT_BTN_OFF, L_BIT_CHAR_OFF, szIO_OUTPUT[nCnt]);
// 			}
// 			else
// 			{
// 				m_fnSetBtnOperation( btnIO_OUT[nCnt], L_BIT_BTN_ON, L_BIT_CHAR_ON, szIO_OUTPUT[nCnt]);
// 			}
// 		}
// 		m_pTask->stElipsoIO.stOut.Tower_Lamp_Yellow = upOn;
// 	}
// // 	nCnt++;
// // 	// Signal Tower GREEN
// // 	if ( theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmSignalReadOutputBit( R_AXIS, BIT_2, upOn ) == RC_NORMAL )
// // 	{
// // 		if ( m_pTask->stElipsoIO.stOut.Tower_Lamp_Green != upOn )
// // 		{
// // 			if ( upOn  == L_BIT_ON )
// // 			{
// // 				m_fnSetBtnOperation( btnIO_OUT[nCnt], L_BIT_BTN_OFF, L_BIT_CHAR_OFF, szIO_OUTPUT[nCnt]);
// // 			}
// // 			else
// // 			{
// // 				m_fnSetBtnOperation( btnIO_OUT[nCnt], L_BIT_BTN_ON, L_BIT_CHAR_ON, szIO_OUTPUT[nCnt]);
// // 			}
// // 		}
// // 		m_pTask->stElipsoIO.stOut.Tower_Lamp_Green = upOn;
// // 	}
// // 	nCnt++;
// // 	// Signal Tower Buzzer
// // 	if ( theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmSignalReadOutputBit( R_AXIS, BIT_3, upOn ) == RC_NORMAL )
// // 	{
// // 		if ( m_pTask->stElipsoIO.stOut.Buzzer != upOn )
// // 		{
// // 			if ( upOn  == L_BIT_ON )
// // 			{
// // 				m_fnSetBtnOperation( btnIO_OUT[nCnt], L_BIT_BTN_OFF, L_BIT_CHAR_OFF, szIO_OUTPUT[nCnt]);
// // 			}
// // 			else
// // 			{
// // 				m_fnSetBtnOperation( btnIO_OUT[nCnt], L_BIT_BTN_ON, L_BIT_CHAR_ON, szIO_OUTPUT[nCnt]);
// // 			}
// // 		}
// // 		m_pTask->stElipsoIO.stOut.Buzzer = upOn;
// // 	}
// // 
// 	if (( m_pTask->stElipsoIO.stIn.Jog_Speed_Low  ) &&
// 	    ( m_pTask->stElipsoIO.stIn.Jog_Speed_High ))
// 	{
// 		m_pTask->stEqp.nJoyStickSpeed = JOG_HIGH;
// 	}
// 	else
// 	{
// 		if (( !m_pTask->stElipsoIO.stIn.Jog_Speed_Low  ) &&
// 			( !m_pTask->stElipsoIO.stIn.Jog_Speed_High ))
// 		{
// 			m_pTask->stEqp.nJoyStickSpeed = JOG_ZERO;
// 		}
// 		else
// 		{
// 			if ( m_pTask->stElipsoIO.stIn.Jog_Speed_High )
// 			{
// 				m_pTask->stEqp.nJoyStickSpeed = JOG_MIDDLE;
// 			}
// 			else
// 			{
// 				m_pTask->stEqp.nJoyStickSpeed = JOG_SLOW;
// 			}
// 		}
// 	}
// 
// 	if (( m_pTask->stElipsoIO.stIn.Jog_X_Plus  ) &&
// 	    ( m_pTask->stElipsoIO.stIn.Jog_Y_Plus ))
// 	{
// 		m_pTask->stEqp.nJoyStickArrowXY = JOG_DOWN_RIGHT;
// 	}
// 	else
// 	{
// 		if (( m_pTask->stElipsoIO.stIn.Jog_X_Plus  ) &&
// 			( m_pTask->stElipsoIO.stIn.Jog_Y_Minus ))
// 		{
// 			m_pTask->stEqp.nJoyStickArrowXY = JOG_UP_RIGHT;
// 		}
// 		else
// 		{
// 			if (( m_pTask->stElipsoIO.stIn.Jog_X_Minus  ) &&
// 				( m_pTask->stElipsoIO.stIn.Jog_Y_Plus ))
// 				{
// 					m_pTask->stEqp.nJoyStickArrowXY = JOG_DOWN_LEFT;
// 				}
// 				else
// 				{
// 					if (( m_pTask->stElipsoIO.stIn.Jog_X_Minus  ) &&
// 						( m_pTask->stElipsoIO.stIn.Jog_Y_Minus ))
// 					{
// 						m_pTask->stEqp.nJoyStickArrowXY = JOG_UP_LEFT;
// 					}
// 					else
// 					{
// 						if ( m_pTask->stElipsoIO.stIn.Jog_X_Plus  )
// 						{
// 							m_pTask->stEqp.nJoyStickArrowXY = JOG_RIGHT;
// 						}
// 						else
// 						{
// 							if ( m_pTask->stElipsoIO.stIn.Jog_X_Minus  )
// 							{
// 								m_pTask->stEqp.nJoyStickArrowXY = JOG_LEFT;
// 							}
// 							else
// 							{
// 								if ( m_pTask->stElipsoIO.stIn.Jog_Y_Plus  )
// 								{
// 									m_pTask->stEqp.nJoyStickArrowXY = JOG_DOWN;
// 								}
// 								else
// 								{
// 									if ( m_pTask->stElipsoIO.stIn.Jog_Y_Minus  )
// 									{
// 										m_pTask->stEqp.nJoyStickArrowXY = JOG_UP;
// 									}
// 									else
// 									{
// 										m_pTask->stEqp.nJoyStickArrowXY = JOG_STOP;
// 									}
// 								}
// 							}
// 						}
// 					}
// 				}
// 			}
// 	}
// 
// 	if (( m_pTask->stElipsoIO.stIn.Jog_Z_Plus  ) &&
// 	    ( m_pTask->stElipsoIO.stIn.Jog_Z_Minus ))
// 	{
// 		m_pTask->stEqp.nJoyStickArrowZ = JOG_STOP;
// 	}
// 	else
// 	{
// 		if (( !m_pTask->stElipsoIO.stIn.Jog_Z_Plus  ) &&
// 			( !m_pTask->stElipsoIO.stIn.Jog_Z_Minus ))
// 		{
// 			m_pTask->stEqp.nJoyStickArrowZ = JOG_STOP;
// 		}
// 		else
// 		{
// 			if ( m_pTask->stElipsoIO.stIn.Jog_Z_Plus  )
// 			{
// 				m_pTask->stEqp.nJoyStickArrowZ = JOG_DOWN;
// 			}
// 			else
// 			{
// 				m_pTask->stEqp.nJoyStickArrowZ = JOG_UP;
// 			}
// 		}
// 	}
// 
// 	if ( m_pTask->stElipsoIO.stIn.Loading_Switcn )
// 	{
// 		if ( m_nLoading != m_pTask->stElipsoIO.stIn.Loading_Switcn )
// 		{
// 			m_nLoading = m_pTask->stElipsoIO.stIn.Loading_Switcn;
// 			this->PostMessage( UM_LOADING_BUTTON, 0, 0 );
// 		}
// 	}
// 	else
// 	{
// 		m_nLoading = 0;
// 	}
// 
// 	if ( m_pTask->stElipsoIO.stIn.Measure_Switch )
// 	{
// 		if ( m_nStart != m_pTask->stElipsoIO.stIn.Measure_Switch )
// 		{
// 			m_nStart = m_pTask->stElipsoIO.stIn.Measure_Switch;
// 			theApp.m_fnGetViewPointer()->m_pAuto->PostMessage( UM_START_BUTTON, 0, 0 );
// 		}
// 	}
// 	else
// 	{
// 		m_nStart = 0;
// 	}
// 
// 	m_pTask->stEqp.bIoStart = TRUE;
// 	m_bProcessing	= FALSE;
// #endif
// #endif
// 	return 0;
// }
// 
// short CIO::m_fnSetOUTPUT( int nNo )
// {
// 	short nResult = 0;
// #if AJINTEX_BOARD
// #if !EQ_DEBUG
// 	switch( nNo )
// 	{
// 		case MEASURE_LAMP_ON:
// 			{
// 				theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmSignalWriteOutputBit( X_AXIS, BIT_2, !m_pTask->stElipsoIO.stOut.Measure_Lamp_On );
// 			}
// 			break;
// 		case LOADING_LAMP_ON:
// 			{
// 				theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmSignalWriteOutputBit( X_AXIS, BIT_3, !m_pTask->stElipsoIO.stOut.Loading_Switch_Lamp_On );
// 			}
// 			break;
// 		case VACUUM_ON_OFF:
// 			{
// 				theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmSignalWriteOutputBit( Y_AXIS, BIT_2, !m_pTask->stElipsoIO.stOut.Vacuum_On );
// 			}
// 			break;
// 		case SIGNAL_TOWER_RED:											
// 			{
// 				theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmSignalWriteOutputBit( Z_AXIS, BIT_2, !m_pTask->stElipsoIO.stOut.Tower_Lamp_Red );
// 			}
// 			break;
// 		case SIGNAL_TOWER_YELLOW:	
// 			{
// 				theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmSignalWriteOutputBit( Z_AXIS, BIT_3, !m_pTask->stElipsoIO.stOut.Tower_Lamp_Yellow );
// 			}
// 			break;
// 		case SIGNAL_TOWER_GREEN:
// 			{
// // 				theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmSignalWriteOutputBit( R_AXIS, BIT_2, !m_pTask->stElipsoIO.stOut.Tower_Lamp_Green );
// 			}
// 			break;
// 		case BUZZER_ONOFF:
// 			{
// // 				theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmSignalWriteOutputBit( R_AXIS, BIT_3, !m_pTask->stElipsoIO.stOut.Buzzer );
// 			}
// 			break;
// 	}
// #endif
// #endif
// 	return nResult;
// }
// 
// short CIO::m_fnSetOutputSignal( int nSignal, BOOL bON )
// {
// 	short nResult = 0;
// #if AJINTEX_BOARD
// #if !EQ_DEBUG
// 	switch( nSignal )
// 	{
// 		case MEASURE_LAMP_ON:
// 			{
// 				theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmSignalWriteOutputBit( X_AXIS, BIT_2, bON );
// 			}
// 			break;
// 		case LOADING_LAMP_ON:
// 			{
// 				theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmSignalWriteOutputBit( X_AXIS, BIT_3, bON );
// 			}
// 			break;
// 		case VACUUM_ON_OFF:
// 			{
// 				theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmSignalWriteOutputBit( Y_AXIS, BIT_2, bON );
// 			}
// 			break;
// 		case SIGNAL_TOWER_RED:											
// 			{
// 				theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmSignalWriteOutputBit( Z_AXIS, BIT_2, bON );
// 			}
// 			break;
// 		case SIGNAL_TOWER_YELLOW:	
// 			{
// 				theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmSignalWriteOutputBit( Z_AXIS, BIT_3, bON );
// 			}
// 			break;
// 		case SIGNAL_TOWER_GREEN:
// 			{
// // 				theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmSignalWriteOutputBit( R_AXIS, BIT_2, bON );
// 			}
// 			break;
// 		case BUZZER_ONOFF:
// 			{
// // 				theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmSignalWriteOutputBit( R_AXIS, BIT_3, bON );
// 			}
// 			break;
// 	}
// #endif
// #endif
// 	return nResult;
// }
// 
// void CIO::OnBnClickedBtn1Out01()
// {
// 	m_fnSetOUTPUT( MEASURE_LAMP_ON );
// }
// 
// void CIO::OnBnClickedBtn1Out02()
// {
// 	m_fnSetOUTPUT( LOADING_LAMP_ON );
// }
// 
// 
// void CIO::OnBnClickedBtn1Out03()
// {
// 	m_fnSetOUTPUT( VACUUM_ON_OFF );
// }
// 
// void CIO::OnBnClickedBtn1Out04()
// {
// 	m_fnSetOUTPUT( SIGNAL_TOWER_RED );
// }
// 
// 
// void CIO::OnBnClickedBtn1Out05()
// {
// 	m_fnSetOUTPUT( SIGNAL_TOWER_YELLOW );
// }
// 
// 
// void CIO::OnBnClickedBtn1Out06()
// {
// 	m_fnSetOUTPUT( SIGNAL_TOWER_GREEN );
// }
// 
// 
// void CIO::OnBnClickedBtn1Out07()
// {
// 	m_fnSetOUTPUT( BUZZER_ONOFF );
// }
// 
// void CIO::m_fnTECmsgOut( CString str, COLORREF color, int flag )
// {
// 	TECmsgOut( m_Util.CStringTobstr(str), color, 1 );
// }
// 
// LRESULT CIO::m_fnSetLoadingButton(  WPARAM nNo, LPARAM lParam )
// {
// 	if ( m_bProcess ) return 0;
// 
// 	if ( m_pTask->stEqp.bIoStartButton ) 
// 	{
// 		m_bOperation = FALSE;
// 		m_bProcess			  = TRUE;	
// 		m_fnTECmsgOut( "검사중입니다. 검사종료 또는 검사중지후 진행해 주세요.", GREEN_1, 1 );
// 		m_bProcess			  = FALSE;	
// 		return 0;
// 	}
// 
// 	if ( !m_pTask->stEqp.bEqpInit ) 
// 	{
// 		m_bOperation = FALSE;
// 		m_bProcess			  = TRUE;	
// 		m_fnTECmsgOut( "장비를 초기화후 진행해 주세요.", GREEN_1, 1 );
// 		m_bProcess			  = FALSE;	
// 		return 0;
// 	}
// 
// 	CString strData("");
// 	if ( m_bOperation )	
// 	{
// 		if ( m_nStageInOut == STEP_HALT ) return 0;
// 
// 		m_bLoadingStop   = true;
// 		int nStep        = m_nStageInOut;
// 		m_nStageInOut    = STEP_HALT;
// 		strData.Format( "Waper 로딩중입니다 중단하시겠습니까?" );
// 		m_bProcess			  = TRUE;	
// 		int nResult = TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 2 );
// 		m_bProcess			  = FALSE;	
// 		if(nResult == 1)
// 		{
// 			if ( nStep != STEP_HALT )
// 			{
// 				strData.Format( "사용자에 의해 웨이퍼 로딩이 중지되었습니다." );
// 				theApp.m_fnGetViewPointer()->m_fnLogWrite( strData );
// 				this->PostMessageA( UM_LOADING_SW_LIGHT, 0, 0 );
// 				m_nStageInOut    = STEP_HALT;
// 				m_bStageInOutEnd = FALSE;
// 				m_bOperation     = FALSE;
// 				m_bProcess		 = TRUE;	
// 				TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 1 );
// 				m_bProcess		 = FALSE;	
// 			}
// 		}
// 		else
// 		{
// 			m_bLoadingStop = false;
// 			m_nStageInOut  = nStep;
// 		}
// 		return 0;
// 	}
// 
// 	m_bOperation   = TRUE;
// 	m_bLoadingStop = false;
// 
// 	if ( m_hStageInOutThread != NULL )
// 	{
// 		m_bStageInOutEnd = FALSE;
// 		SuspendThread( m_hStageInOutThread );
// 		CloseHandle( m_hStageInOutThread );
// 		m_hStageInOutThread = NULL;
// 	}
// 
// 	theApp.m_fnGetViewPointer()->m_fnLogWrite( "웨이퍼 로딩위치로 이동합니다." );
// 
// 	m_nStageInOut = STEP_HALT;
// 	unsigned long	ThreadID1;
// 	m_bStageInOutEnd			= TRUE;
// 	m_hStageInOutThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StageInOutThread, this, NULL, &ThreadID1);
// 	m_nStageInOut = 10;
// 
// 	return 0;
// }
// 
// LRESULT CIO::m_fnSetSignalTower(  WPARAM nNo, LPARAM lParam )
// {
// 	if (( nNo != SIGNAL_TOWER_RED ) && 
// 	    ( nNo != SIGNAL_TOWER_YELLOW ) &&
// 	    ( nNo != SIGNAL_TOWER_GREEN ))
// 	{
// 		return 0;
// 	}
// 
// 	m_fnSetOutputSignal( SIGNAL_TOWER_RED, FALSE );
// 	m_fnSetOutputSignal( SIGNAL_TOWER_GREEN, FALSE );
// 	m_fnSetOutputSignal( SIGNAL_TOWER_YELLOW, FALSE );
// 	if ( lParam == L_BIT_OFF ) return 0;
// 
// 	if ( nNo == SIGNAL_TOWER_RED )
// 	{
// 		m_fnSetOutputSignal( SIGNAL_TOWER_RED, TRUE );
// 	}
// 	if ( nNo == SIGNAL_TOWER_YELLOW )
// 	{
// 		m_fnSetOutputSignal( SIGNAL_TOWER_YELLOW, TRUE );
// 	}
// 	if ( nNo == SIGNAL_TOWER_GREEN )
// 	{
// 		m_fnSetOutputSignal( SIGNAL_TOWER_YELLOW, TRUE );
// 	}
// 	return 0;
// }
// 
// LRESULT CIO::m_fnSetBuzzer(  WPARAM nNo, LPARAM lParam )
// {
// 	if ( nNo != BUZZER_ONOFF )
// 	{
// 		return 0;
// 	}
// 
// 	if ( lParam == L_BIT_ON )
// 	{
// 		if ( m_pTask->stCommon.bAlarmOn )
// 		{
// 			m_fnSetOutputSignal( BUZZER_ONOFF, TRUE );
// 		}
// 	}
// 	else
// 	{
// 		m_fnSetOutputSignal( BUZZER_ONOFF, FALSE );
// 	}
// 	return 0;
// }
// 
// LRESULT CIO::m_fnSetVacuum(  WPARAM nNo, LPARAM lParam )
// {
// 	if ( nNo != VACUUM_ON_OFF )
// 	{
// 		return 0;
// 	}
// 
// 	if ( lParam == L_BIT_ON )
// 	{
// 		m_fnSetOutputSignal( VACUUM_ON_OFF, TRUE );
// 	}
// 	else
// 	{
// 		m_fnSetOutputSignal( VACUUM_ON_OFF, FALSE );
// 	}
// 	return 0;
// }
// 
// LRESULT CIO::m_fnSetLoadingSwLight(  WPARAM nNo, LPARAM lParam )
// {
// 	if ( nNo == 1 )
// 	{
// 		m_fnSetOutputSignal( LOADING_LAMP_ON, TRUE );
// 	}
// 	else
// 	{
// 		m_fnSetOutputSignal( LOADING_LAMP_ON, FALSE );
// 	}
// 	return 0;
// }
// 
// LRESULT CIO::m_fnSetStartSwLight(  WPARAM nNo, LPARAM lParam )
// {
// 	if ( nNo == 1 )
// 	{
// 		m_fnSetOutputSignal( MEASURE_LAMP_ON, TRUE );
// 	}
// 	else
// 	{
// 		m_fnSetOutputSignal( MEASURE_LAMP_ON, FALSE );
// 	}
// 	return 0;
// }

UINT StageInOutThread(LPVOID pFuncData)
{
	CIO		*pQI = (CIO *)pFuncData;

	double dSpeed			 = pQI->m_pTask->stCommon.dSpeedHigh;
	double dX				 = pQI->m_pTask->stCommon.dLoadingX;
	double dY				 = pQI->m_pTask->stCommon.dLoadingY;
	DWORD  dwRtnCode         = 0;
	DWORD  dwRtnCode1        = 0;
	DWORD  dwRtnCode2        = 0;
	DWORD  dwBusyStatus  	 = 0;	
	DWORD  dwBusyStatus1	 = 0;	
	DWORD  dwBusyStatus2     = 0;

	while ( pQI->m_bStageInOutEnd )
	{
		if ( pQI->m_nStageInOut == STEP_HALT )
		{
			Sleep( AUTO_TIME );
			continue;
		}

		if (( theApp.m_fnGetViewPointer()->m_pIO == NULL ) ||
			( theApp.m_fnGetViewPointer()->m_pAjin == NULL ))
		{
			Sleep( AUTO_TIME );
			continue;
		}

		switch( pQI->m_nStageInOut )
		{
			case 10:
				{
#if AJINTEX_BOARD
					theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMotSetAbsRelMode( Z_AXIS, POS_ABS_MODE );
// 					theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMotSetAbsRelMode( R_AXIS, POS_ABS_MODE );
					if (( theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveStartPos( Z_AXIS, 0.0, dSpeed, dSpeed * 2, dSpeed * 2 ) == AXT_RT_SUCCESS ) 
// 						&& ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveStartPos( R_AXIS, 90.0, dSpeed, dSpeed * 2, dSpeed * 2 ) == AXT_RT_SUCCESS )
						)
					{
						if ( !pQI->m_bLoadingStop )
						{
							pQI->PostMessageA( UM_LOADING_SW_LIGHT, 1, 0 );
							pQI->m_nStageInOut = 20;
							theApp.m_fnGetViewPointer()->m_fnLogWrite( "스테이지 Z축을 원점( 0,0 ) 로 이동 시작" );
						}
					}
#endif
				}
				break;
			case 20:
				{
#if AJINTEX_BOARD
					dwRtnCode1 = theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmStatusReadInMotion( Z_AXIS, dwBusyStatus1 );
// 					dwRtnCode2 = theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmStatusReadInMotion( R_AXIS, dwBusyStatus2 );
					if (( dwBusyStatus1 == 0 ) 
// 						&& ( dwBusyStatus2 == 0 )
						)
					{
						if ( !pQI->m_bLoadingStop )
						{
							theApp.m_fnGetViewPointer()->m_fnLogWrite( "스테이지 Z축을 원점( 0,0 ) 로 이동 완료" );
							pQI->m_nStageInOut = 30;
						}
					}
#endif
				}
				break;
			case 30:
				{
#if AJINTEX_BOARD
					theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMotSetAbsRelMode( X_AXIS, POS_ABS_MODE );
					theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMotSetAbsRelMode( Y_AXIS, POS_ABS_MODE );
					if (( theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveStartPos( X_AXIS, dX, dSpeed, dSpeed * 2, dSpeed * 2 ) == AXT_RT_SUCCESS ) &&
					    ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveStartPos( Y_AXIS, dY, dSpeed, dSpeed * 2, dSpeed * 2 ) == AXT_RT_SUCCESS ))
					{
						if ( !pQI->m_bLoadingStop )
						{
							pQI->m_nStageInOut = 40;
							theApp.m_fnGetViewPointer()->m_fnLogWrite( "스테이지 X, Y축을 Loading위치 로 이동 시작" );
						}
					}
#endif
				}
				break;
			case 40:
				{
#if AJINTEX_BOARD
					dwRtnCode1 = theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmStatusReadInMotion( X_AXIS, dwBusyStatus1 );
					dwRtnCode2 = theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmStatusReadInMotion( Y_AXIS, dwBusyStatus2 );
					if (( dwBusyStatus1 == 0 ) &&
					    ( dwBusyStatus2 == 0 ))
					{
						if ( !pQI->m_bLoadingStop )
						{
							theApp.m_fnGetViewPointer()->m_fnLogWrite( "스테이지 X, Y축을 Loading위치 로 이동 완료" );
							pQI->PostMessageA( UM_LOADING_SW_LIGHT, 0, 0 );
							pQI->m_nStageInOut    = STEP_HALT;
							pQI->m_bStageInOutEnd = FALSE;
							pQI->m_bOperation     = FALSE;
							pQI->m_bProcess		  = TRUE;	
							// TECmsgOut( L"웨이퍼 로딩작업이 완료되었습니다.", GREEN_1, 1 ); 
							pQI->m_bProcess		  = FALSE;	
						}
					}
#endif
				}
				break;
		}
		Sleep( AUTO_TIME );
	}
	if ( pQI->m_hStageInOutThread != NULL )
	{
		pQI->m_bStageInOutEnd = FALSE;
		CloseHandle(pQI->m_hStageInOutThread);
		pQI->m_hStageInOutThread	= NULL;
	}
	return 0;
}
