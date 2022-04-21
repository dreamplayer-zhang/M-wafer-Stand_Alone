// IO.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EllipsoMeter.h"
#include "ElipsoMeter_IO.h"
#include "afxdialogex.h"
#include "resource.h"

// ElipsoMeter_IO 대화 상자입니다.
//--------------------------------------------------
// 내부 IO INPUT
//--------------------------------------------------
static int btnElipso_IO_IN[] = 
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
static int btnElipso_IO_OUT[] = 
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
static CString szElipso_IO_IN[] = 
{
	"01. Wafer Vacuum  6Inch",
	"02. Wafer Vacuum  8Inch",
	"03. Wafer Vacuum 12Inch",
	"04. EFEM Pause (대기)",
	"05. EFEM Busy  (구동중)",
	"06. ",
	"07. Wafer 감지센서",
	"08. ",
	"09. 시스템 알람 (EFEM)",
	"10. EMS #1",
	"11. EMS #2",
	"12. EMS #3",
	"13. EMS #4",
	"14. ",
	"15. ",
	"16. "
};

//--------------------------------------------------
// IO OUTPUT
//--------------------------------------------------

static CString szElipso_IO_OUTPUT[] = 
{
	"01. Vacuum On  6Inch",
	"02. Vacuum On  8Inch",
	"03. Vacuum On 12Inch",
	"04. ",
	"05. ",
	"06. Measure Ready (대기)",
	"07. Measure Busy (구동중)",
	"08. ",
	"09. 시스템 알람 (Ellipsometer)",
	"10. ",
	"11. ",
	"12. ",
	"13. ",
	"14. Wafer Bit 01",
	"15. Wafer Bit 10",
	"16. "
};
IMPLEMENT_DYNAMIC(CElipsoMeter_IO, CDialogEx)

CElipsoMeter_IO::CElipsoMeter_IO(CWnd* pParent /*=NULL*/)
	: CDialogEx(CElipsoMeter_IO::IDD, pParent)
{

	this->Create( IDD_ELIPSO_IO );

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

CElipsoMeter_IO::~CElipsoMeter_IO()
{
}

void CElipsoMeter_IO::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CElipsoMeter_IO)
	for( int i = 0; i < ELIPSO_IO_IN_CNT; i++ )
	{
		DDX_Control(pDX, btnElipso_IO_IN[i], m_btnIn[i]);
	}
	for( int i = 0; i < ELIPSO_IO_OUT_CNT; i++ )
	{
		DDX_Control(pDX, btnElipso_IO_OUT[i], m_btnOutput[i]);
	}
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CElipsoMeter_IO, CDialogEx)
	ON_MESSAGE(WM_SET_IO, m_fnIoRead)
	ON_BN_CLICKED(IDC_BTN_EXIT, &CElipsoMeter_IO::OnBnClickedBtnExit)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CElipsoMeter_IO::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_1_OUT_01, &CElipsoMeter_IO::OnBnClickedBtn1Out01)
	ON_BN_CLICKED(IDC_BTN_1_OUT_02, &CElipsoMeter_IO::OnBnClickedBtn1Out02)
	ON_BN_CLICKED(IDC_BTN_1_OUT_03, &CElipsoMeter_IO::OnBnClickedBtn1Out03)
	ON_BN_CLICKED(IDC_BTN_1_OUT_04, &CElipsoMeter_IO::OnBnClickedBtn1Out04)
	ON_BN_CLICKED(IDC_BTN_1_OUT_05, &CElipsoMeter_IO::OnBnClickedBtn1Out05)
	ON_BN_CLICKED(IDC_BTN_1_OUT_06, &CElipsoMeter_IO::OnBnClickedBtn1Out06)
	ON_BN_CLICKED(IDC_BTN_1_OUT_07, &CElipsoMeter_IO::OnBnClickedBtn1Out07)
	ON_BN_CLICKED(IDC_BTN_1_OUT_08, &CElipsoMeter_IO::OnBnClickedBtn1Out08)
	ON_BN_CLICKED(IDC_BTN_1_OUT_09, &CElipsoMeter_IO::OnBnClickedBtn1Out09)
	ON_BN_CLICKED(IDC_BTN_1_OUT_10, &CElipsoMeter_IO::OnBnClickedBtn1Out10)
	ON_BN_CLICKED(IDC_BTN_1_OUT_11, &CElipsoMeter_IO::OnBnClickedBtn1Out11)
	ON_BN_CLICKED(IDC_BTN_1_OUT_12, &CElipsoMeter_IO::OnBnClickedBtn1Out12)
	ON_BN_CLICKED(IDC_BTN_1_OUT_13, &CElipsoMeter_IO::OnBnClickedBtn1Out13)
	ON_BN_CLICKED(IDC_BTN_1_OUT_14, &CElipsoMeter_IO::OnBnClickedBtn1Out14)
	ON_BN_CLICKED(IDC_BTN_1_OUT_15, &CElipsoMeter_IO::OnBnClickedBtn1Out15)
	ON_BN_CLICKED(IDC_BTN_1_OUT_16, &CElipsoMeter_IO::OnBnClickedBtn1Out16)
	ON_MESSAGE(UM_LOADING_BUTTON, m_fnSetLoadingButton)
	ON_MESSAGE(UM_VACUUM, m_fnSetVacuum)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------------------------
// 외부 참조용 Function
//---------------------------------------------------------------------------------------------
BOOL CElipsoMeter_IO::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (( nID   == 2 ) &&
		( nCode == -1 ))
	{
		m_fnSetShowFlag( FALSE );
	}
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CElipsoMeter_IO::m_fnSetShowFlag( BOOL bShow )
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

BOOL CElipsoMeter_IO::PreTranslateMessage(MSG* pMsg) 
{
	// ENTER KEY or ESC KEY CANCEL
	if (( pMsg->wParam == 13 ) || 
		( pMsg->wParam == 27 ))
	{
		return FALSE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

//---------------------------------------------------------------------------------------------------------
// 시작에 필요한 작업을 한다.
//---------------------------------------------------------------------------------------------------------
void CElipsoMeter_IO::m_fnSetInit( ) 
{
	CString s("");
	s.Format( "Elipso IO 메인창" );
	this->SetWindowText(s);

	m_Log.m_fnSetLogPath( "Elipso_IO", "Elipso_IO" );
	m_Log.m_fnSetLogWriteFlag( TRUE );

	m_HexaLog.m_fnSetLogPath( "Elipso_IO", "Elipso_IOIO_HEXA" );
	m_HexaLog.m_fnSetLogWriteFlag( TRUE );

	m_pTaskHandle = new CTaskHandle( this, SHARED_NAME, 
									 1, 
									 sizeof( SHEET_RESISTANCE_Format ) + 1,
									 0,
									 FALSE );


	m_pTask					= ( SHEET_RESISTANCE_Format* )m_pTaskHandle->m_pFilePointer( );
	m_pTask->stElipsoIO.bConnect  = FALSE;

	for ( int i = 0; i < ELIPSO_IO_IN_CNT; i++ )
	{
		m_fnSetBtnOperation( btnElipso_IO_IN[i], L_BIT_BTN_OFF, L_BIT_CHAR_OFF, szElipso_IO_IN[i]);
	}

	for ( int i = 0; i < ELIPSO_IO_OUT_CNT; i++ )
	{
		m_fnSetBtnOperation( btnElipso_IO_OUT[i], L_BIT_BTN_OFF, L_BIT_CHAR_OFF, szElipso_IO_OUTPUT[i]);
		m_fnSetOutputSignal( i, FALSE );
	}

	unsigned long	ThreadID;
	m_bIoEnd    = TRUE;
	m_hIoThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ElipsoIoThread, this, NULL, &ThreadID);
}

//---------------------------------------------------------------------------------------------------------
// 종료에 필요한 작업을 한다.
//---------------------------------------------------------------------------------------------------------
void CElipsoMeter_IO::m_fnSetEnd() 
{
	m_bProcessing = TRUE;

	if ( m_hStageInOutThread != NULL )
	{
		m_bStageInOutEnd = FALSE;
		SuspendThread( m_hStageInOutThread );
		CloseHandle( m_hStageInOutThread );
		m_hStageInOutThread = NULL;
	}

	if ( m_hIoThread != NULL )
	{
		m_bIoEnd			= FALSE;
		SuspendThread( m_hIoThread );
		CloseHandle(m_hIoThread);
		m_hIoThread	    = NULL;
	}

	for( int i = 0; i < ELIPSO_IO_OUT_CNT; i++ )
	{
		m_fnSetOutputSignal( i, FALSE );
	}

	Sleep( 100 );

	if ( m_pTaskHandle != NULL )
	{
		m_pTask->stElipsoIO.bConnect = FALSE;
		m_pTaskHandle->m_fnSetEnd();
		delete m_pTaskHandle;
		m_pTaskHandle = NULL;
	}
}

void CElipsoMeter_IO::OnBnClickedBtnExit()
{
	m_fnSetShowFlag( FALSE );
}

void CElipsoMeter_IO::OnBnClickedBtnSave()
{
}

//---------------------------------------------------------------------------------------------------------
// Log 파일을 저장한다.
//---------------------------------------------------------------------------------------------------------
void CElipsoMeter_IO::m_fnLogWrite( CString strMsg )
{
	CString strCnt("");
	int     nCnt		= 0;
	int     nTotal		= 0;

	memset( m_szData, 0x00, sizeof( m_szData ));

	sprintf_s(m_szData, "%s", strMsg.GetString());
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

//------------------------------------------------------------
// 각 버튼의 색상과 글자색을 변경한다.
//------------------------------------------------------------
void CElipsoMeter_IO::m_fnSetBtnOperation( int nID, 
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

UINT ElipsoIoThread(LPVOID pFuncData)
{
	CElipsoMeter_IO		*pQI = (CElipsoMeter_IO *)pFuncData;

	while ( pQI->m_bIoEnd )
	{
		if ( pQI->m_hWnd != NULL )
		{
#if AJINTEX_BOARD
			pQI->PostMessage( WM_SET_IO, 0,0 );
#endif
			Sleep( IO_READ_TIME );
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

LRESULT CElipsoMeter_IO::m_fnIoRead(  WPARAM nNo, LPARAM lParam )
{
	DWORD dwINPUT = 0;
	DWORD dwSTS   = 0;
	int   i       = 0;
#if AJINTEX_BOARD
	if ( m_bProcessing ) return 0;
	if ( !m_bIoEnd ) return 0;

	m_bProcessing	= TRUE;

	theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxdiReadInportDword( ELIPSO_IO_INPUT_1, 0, dwINPUT );
	theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxdoReadOutportDword( ELIPSO_IO_OUTPUT_1, 0, dwSTS );

	//--------------------------------------------------------------------------
	// IO INPUT 1
	//--------------------------------------------------------------------------
	if ( m_dwINPUT != dwINPUT )
	{
		m_dwINPUT = dwINPUT;
		int nCnt = 0;
		for (i = 0; i < 16; i++)
		{

			DWORD dwValue = 0x00000001 << nCnt;
			dwValue &= dwINPUT;
			dwValue = dwValue >> nCnt++;
			if (dwValue == L_BIT_ON)
			{
				m_fnSetBtnOperation(btnElipso_IO_IN[i], L_BIT_BTN_OFF, L_BIT_CHAR_OFF, szElipso_IO_IN[i]);
			}
			else
			{
				m_fnSetBtnOperation(btnElipso_IO_IN[i], L_BIT_BTN_ON, L_BIT_CHAR_ON, szElipso_IO_IN[i]);
			}
		}
	}
	if (m_dwOUTPUT != dwSTS)
	{
		m_dwOUTPUT = dwSTS;
		int nCnt = 0;
		for (i = 0; i < 16; i++)
		{

			DWORD dwValue = 0x00000001 << nCnt;
			dwValue &= dwSTS;
			dwValue = dwValue >> nCnt++;
			if (dwValue == L_BIT_ON)
			{
				m_fnSetBtnOperation(btnElipso_IO_OUT[i], L_BIT_BTN_OFF, L_BIT_CHAR_OFF, szElipso_IO_OUTPUT[i]);
			}
			else
			{
				m_fnSetBtnOperation(btnElipso_IO_OUT[i], L_BIT_BTN_ON, L_BIT_CHAR_ON, szElipso_IO_OUTPUT[i]);
			}
		}
	}
#endif
	int nSize = sizeof( t_OUTPUT_BIT_IO );
	memcpy( &m_pTask->stElipsoIO.stIn, &dwINPUT, sizeof( t_INPUT_BIT_IO ));
	memcpy( &m_pTask->stElipsoIO.stOut, &dwSTS, sizeof( t_OUTPUT_BIT_IO ));
	m_pTask->stEqp.bIoStart = TRUE;
	m_bProcessing	= FALSE;
	return 0;
}

short CElipsoMeter_IO::m_fnSetOUTPUT( int nNo )
{
	short nResult = 0;
#if AJINTEX_BOARD
#if !EQ_DEBUG
	switch( nNo )
	{
		case VACUUM_ON_06_INCH:
			{
				theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxdoWriteOutportBit( ELIPSO_IO_OUTPUT_1, nNo, !m_pTask->stElipsoIO.stOut.Vacuum_On_06_Inch );
			}
			break;
		case VACUUM_ON_08_INCH:
			{
				theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxdoWriteOutportBit( ELIPSO_IO_OUTPUT_1, nNo, !m_pTask->stElipsoIO.stOut.Vacuum_On_08_Inch );
			}
			break;
		case VACUUM_ON_12_INCH:
			{
				theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxdoWriteOutportBit( ELIPSO_IO_OUTPUT_1, nNo, !m_pTask->stElipsoIO.stOut.Vacuum_On_12_Inch );
			}
			break;
		case RESERVED3:											
			{
				theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxdoWriteOutportBit( ELIPSO_IO_OUTPUT_1, nNo, !m_pTask->stElipsoIO.stOut.Reserved03);
			}
			break;
		case RESERVED4:
			{
				theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxdoWriteOutportBit( ELIPSO_IO_OUTPUT_1, nNo, !m_pTask->stElipsoIO.stOut.Reserved04);
			}
			break;
		case MEASURE_READY:
			{
				theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxdoWriteOutportBit( ELIPSO_IO_OUTPUT_1, nNo, !m_pTask->stElipsoIO.stOut.Measure_Ready);
			}
			break;
		case MEASURE_BUSY:
			{
				theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxdoWriteOutportBit( ELIPSO_IO_OUTPUT_1, nNo, !m_pTask->stElipsoIO.stOut.Measure_Busy);
			}
			break;
		case RESERVED7:											
			{
				theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxdoWriteOutportBit( ELIPSO_IO_OUTPUT_1, nNo, !m_pTask->stElipsoIO.stOut.Reserved07);
			}
			break;
		case ELIPSOMETER_ALARM:											
			{
				theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxdoWriteOutportBit( ELIPSO_IO_OUTPUT_1, nNo, !m_pTask->stElipsoIO.stOut.Ellipsometer_Alarm );
			}
			break;
		case RESERVED9:
			{
				theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxdoWriteOutportBit( ELIPSO_IO_OUTPUT_1, nNo, !m_pTask->stElipsoIO.stOut.Reserved09);
			}
			break;
		case RESERVED10:											
			{
				theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxdoWriteOutportBit( ELIPSO_IO_OUTPUT_1, nNo, !m_pTask->stElipsoIO.stOut.Reserved10);
			}
			break;
		case RESERVED11:											
			{
				theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxdoWriteOutportBit( ELIPSO_IO_OUTPUT_1, nNo, !m_pTask->stElipsoIO.stOut.Reserved11);
			}
			break;
		case RESERVED12:											
			{
				theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxdoWriteOutportBit( ELIPSO_IO_OUTPUT_1, nNo, !m_pTask->stElipsoIO.stOut.Reserved12);
			}
			break;
		case WAFER_0B01:
			{
				theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxdoWriteOutportBit( ELIPSO_IO_OUTPUT_1, nNo, !m_pTask->stElipsoIO.stOut.Wafer_Bit_01);
			}
			break;
		case WAFER_0B10:
			{
				theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxdoWriteOutportBit( ELIPSO_IO_OUTPUT_1, nNo, !m_pTask->stElipsoIO.stOut.Wafer_Bit_10);
			}
			break;
		case RESERVED15:										
			{
				theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxdoWriteOutportBit( ELIPSO_IO_OUTPUT_1, nNo, !m_pTask->stElipsoIO.stOut.Reserved15);
			}
			break;
	}
#endif
#endif
	return nResult;
}

short CElipsoMeter_IO::m_fnSetOutputSignal( int nSignal, BOOL bON )
{
	short nResult = 0;
#if AJINTEX_BOARD
#if !EQ_DEBUG
	theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxdoWriteOutportBit( ELIPSO_IO_OUTPUT_1, nSignal, bON );
#endif
#endif
	return nResult;
}

void CElipsoMeter_IO::OnBnClickedBtn1Out01()
{
	m_fnSetOUTPUT( VACUUM_ON_06_INCH );
}

void CElipsoMeter_IO::OnBnClickedBtn1Out02()
{
	m_fnSetOUTPUT( VACUUM_ON_08_INCH );
}


void CElipsoMeter_IO::OnBnClickedBtn1Out03()
{
	m_fnSetOUTPUT( VACUUM_ON_12_INCH );
}

void CElipsoMeter_IO::OnBnClickedBtn1Out04()
{
	m_fnSetOUTPUT(RESERVED3);
}


void CElipsoMeter_IO::OnBnClickedBtn1Out05()
{
	m_fnSetOUTPUT(RESERVED4);
}


void CElipsoMeter_IO::OnBnClickedBtn1Out06()
{
	m_fnSetOUTPUT( MEASURE_READY );
}


void CElipsoMeter_IO::OnBnClickedBtn1Out07()
{
	m_fnSetOUTPUT( MEASURE_BUSY );
}

void CElipsoMeter_IO::OnBnClickedBtn1Out08()
{
	m_fnSetOUTPUT(RESERVED7);
}

void CElipsoMeter_IO::OnBnClickedBtn1Out09()
{
	m_fnSetOUTPUT( ELIPSOMETER_ALARM );
}

void CElipsoMeter_IO::OnBnClickedBtn1Out10()
{
	m_fnSetOUTPUT(RESERVED9);
}

void CElipsoMeter_IO::OnBnClickedBtn1Out11()
{
	m_fnSetOUTPUT(RESERVED10);
}

void CElipsoMeter_IO::OnBnClickedBtn1Out12()
{
	m_fnSetOUTPUT(RESERVED11);
}

void CElipsoMeter_IO::OnBnClickedBtn1Out13()
{
	m_fnSetOUTPUT(RESERVED12);
}

void CElipsoMeter_IO::OnBnClickedBtn1Out14()
{
	m_fnSetOUTPUT(WAFER_0B01);
}

void CElipsoMeter_IO::OnBnClickedBtn1Out15()
{
	m_fnSetOUTPUT(WAFER_0B10);
}

void CElipsoMeter_IO::OnBnClickedBtn1Out16()
{
	m_fnSetOUTPUT( RESERVED15 );
}

void CElipsoMeter_IO::m_fnTECmsgOut( CString str, COLORREF color, int flag )
{
	TECmsgOut( m_Util.CStringTobstr(str), color, 1 );
}

LRESULT CElipsoMeter_IO::m_fnSetLoadingButton(  WPARAM nNo, LPARAM lParam )
{
	if ( m_bProcess ) return 0;

	if ( m_pTask->stEqp.bIoStartButton ) 
	{
		m_bOperation = FALSE;
		m_bProcess			  = TRUE;	
		m_fnTECmsgOut( "검사중입니다. 검사종료 또는 검사중지후 진행해 주세요.", GREEN_1, 1 );
		m_bProcess			  = FALSE;	
		return 0;
	}

	if ( !m_pTask->stEqp.bEqpInit ) 
	{
		m_bOperation = FALSE;
		m_bProcess			  = TRUE;	
		m_fnTECmsgOut( "장비를 초기화후 진행해 주세요.", GREEN_1, 1 );
		m_bProcess			  = FALSE;	
		return 0;
	}

	CString strData("");
	if ( m_bOperation )	
	{
		if ( m_nStageInOut == STEP_HALT ) return 0;

		m_bLoadingStop   = true;
		int nStep        = m_nStageInOut;
		m_nStageInOut    = STEP_HALT;
		strData.Format( "Waper 로딩중입니다 중단하시겠습니까?" );
		m_bProcess			  = TRUE;	
		int nResult = TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 2 );
		m_bProcess			  = FALSE;	
		if(nResult == 1)
		{
			if ( nStep != STEP_HALT )
			{
				strData.Format( "사용자에 의해 웨이퍼 로딩이 중지되었습니다." );
				theApp.m_fnGetViewPointer()->m_fnLogWrite( strData );
				this->PostMessageA( UM_LOADING_SW_LIGHT, 0, 0 );
				m_nStageInOut    = STEP_HALT;
				m_bStageInOutEnd = FALSE;
				m_bOperation     = FALSE;
				m_bProcess		 = TRUE;	
				TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 1 );
				m_bProcess		 = FALSE;	
			}
		}
		else
		{
			m_bLoadingStop = false;
			m_nStageInOut  = nStep;
		}
		return 0;
	}

	m_bOperation   = TRUE;
	m_bLoadingStop = false;

	if ( m_hStageInOutThread != NULL )
	{
		m_bStageInOutEnd = FALSE;
		SuspendThread( m_hStageInOutThread );
		CloseHandle( m_hStageInOutThread );
		m_hStageInOutThread = NULL;
	}

	theApp.m_fnGetViewPointer()->m_fnLogWrite( "웨이퍼 로딩위치로 이동합니다." );

	m_nStageInOut = STEP_HALT;
	unsigned long	ThreadID1;
	m_bStageInOutEnd			= TRUE;
	m_hStageInOutThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ElipsoStageInOutThread, this, NULL, &ThreadID1);
	m_nStageInOut = 10;

	return 0;
}

LRESULT CElipsoMeter_IO::m_fnSetVacuum(  WPARAM nNo, LPARAM lParam )
{
	return 0;
}

UINT ElipsoStageInOutThread(LPVOID pFuncData)
{
	CElipsoMeter_IO		*pQI = (CElipsoMeter_IO *)pFuncData;

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
// 						&&( dwBusyStatus2 == 0 )
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
