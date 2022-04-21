// PointDLG.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EllipsoMeter.h"
#include "PointDLG.h"
#include "afxdialogex.h"


// CPointDLG 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPointDLG, CDialogEx)

CPointDLG::CPointDLG(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPointDLG::IDD, pParent)
{
	this->Create(IDD_MODEL_POINT);
	m_bShow = FALSE;
}

CPointDLG::~CPointDLG()
{
}

void CPointDLG::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_CBO_INCH_KIND, m_clInchKind);
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CBO_METHOD, m_clMethod);
}


BEGIN_MESSAGE_MAP(CPointDLG, CDialogEx)
	ON_CBN_SELCHANGE(IDC_CBO_INCH, &CPointDLG::OnCbnSelchangeCboInch)
	ON_BN_CLICKED(IDC_BTN_CW, &CPointDLG::OnBnClickedBtnCw)
	ON_CBN_SELCHANGE(IDC_CBO_INCH_KIND, &CPointDLG::OnCbnSelchangeCboInchKind)
	ON_BN_CLICKED(IDC_BTN_CCW, &CPointDLG::OnBnClickedBtnCcw)
	ON_BN_CLICKED(IDC_BTN_CW_ADD, &CPointDLG::OnBnClickedBtnCwAdd)
	ON_BN_CLICKED(IDC_BTN_CCW_ADD, &CPointDLG::OnBnClickedBtnCcwAdd)
	ON_BN_CLICKED(IDC_BTN_CURR, &CPointDLG::OnBnClickedBtnCurr)
	ON_BN_CLICKED(IDC_BTN_REPEAT, &CPointDLG::OnBnClickedBtnRepeat)
	ON_BN_CLICKED(IDC_BTN_HIDE, &CPointDLG::OnBnClickedBtnHide)
	ON_BN_CLICKED(IDC_BTN_SQUARE_ADD, &CPointDLG::OnBnClickedBtnSquareAdd)
	ON_BN_CLICKED(IDC_BTN_CURR_XY, &CPointDLG::OnBnClickedBtnCurrXy)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------------------------
// 외부 참조용 Function
//---------------------------------------------------------------------------------------------
BOOL CPointDLG::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (( nID   == 2 ) &&
		( nCode == -1 ))
	{
		m_fnSetShowFlag( FALSE );
	}
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CPointDLG::m_fnSetShowFlag( BOOL bShow )
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

BOOL CPointDLG::PreTranslateMessage(MSG* pMsg) 
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
void CPointDLG::m_fnSetInit( ) 
{
	CString s("");
	s.Format( "등간격 좌표등록창" );
	this->SetWindowText(s);

	m_pTaskHandle = new CTaskHandle( this, SHARED_NAME, 
										1, 
										sizeof( SHEET_RESISTANCE_Format ) + 1,
										0,
										FALSE );

	for( int i = 0; i < 4; i++ )
	{
		m_clMethod.AddString( strMethod[i]);
	}
	m_pTask					= ( SHEET_RESISTANCE_Format* )m_pTaskHandle->m_pFilePointer( );

}

//---------------------------------------------------------------------------------------------------------
// 종료에 필요한 작업을 한다.
//---------------------------------------------------------------------------------------------------------
void CPointDLG::m_fnSetEnd() 
{
	if ( m_pTaskHandle != NULL )
	{
		m_pTaskHandle->m_fnSetEnd();
		delete m_pTaskHandle;
		m_pTaskHandle = NULL;
	}
}

void CPointDLG::OnCbnSelchangeCboInch()
{
	theApp.m_fnGetViewPointer()->m_pStageMap->m_nWaperSize = theApp.m_fnGetViewPointer()->m_pModel->m_fnGetWaperSize();
}

void CPointDLG::OnBnClickedBtnCw()
{
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

	strData.Format( "정말로 시계방향으로 좌표를 생성하시겠습니까?" );
	int nResult = TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 2 );
	if ( nResult == 1 )
	{
		t_WAPER_INFO  stWaper;
		stWaper.bCw = TRUE;
		stWaper.dMM = dMM;
		stWaper.dR  = dR;
		theApp.m_fnGetViewPointer()->m_pModel->m_fnSetWaper( stWaper );
		if ( stWaper.bOK )
		{
			m_pTask->stModel.nSRData = 0;
			memset( m_pTask->stModel.stSRData, 0x00, sizeof(m_pTask->stModel.stSRData));
			theApp.m_fnGetViewPointer()->m_pModel->m_fnSRSheetClear();
			for( int i = 0; i < stWaper.nTotal; i++ )
			{
				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetRow( m_pTask->stModel.nSRData + 1 );
				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_NAME );
				strNAME.Format( "%01d", m_pTask->stModel.nSRData + 1 );
				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText(strNAME);
				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_X );
				strX.Format( "%7.3f", stWaper.dX[i] );
				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText(strX);
				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_Y );
				strY.Format( "%7.3f", stWaper.dY[i] );
				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText(strY);
				sprintf_s(m_pTask->stModel.stSRData[m_pTask->stModel.nSRData].szName,"%s", strNAME.GetString() );
				m_pTask->stModel.stSRData[m_pTask->stModel.nSRData].dwXPosition = stWaper.dX[i];
				m_pTask->stModel.stSRData[m_pTask->stModel.nSRData].dwYPosition = stWaper.dY[i];
				m_pTask->stModel.nSRData++;
			}
		}
	}
}

void CPointDLG::OnBnClickedBtnCcw()
{
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

	strData.Format( "정말로 시계반대방향으로 좌표를 생성하시겠습니까?" );
	int nResult = TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 2 );
	if ( nResult == 1 )
	{
		t_WAPER_INFO  stWaper;
		stWaper.bCw = FALSE;
		stWaper.dMM = dMM;
		stWaper.dR  = dR;
		theApp.m_fnGetViewPointer()->m_pModel->m_fnSetWaper( stWaper );
		if ( stWaper.bOK )
		{
			m_pTask->stModel.nSRData = 0;
			memset( m_pTask->stModel.stSRData, 0x00, sizeof(m_pTask->stModel.stSRData));
			theApp.m_fnGetViewPointer()->m_pModel->m_fnSRSheetClear();
			for( int i = 0; i < stWaper.nTotal; i++ )
			{
				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetRow( m_pTask->stModel.nSRData + 1 );
				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_NAME );
				strNAME.Format( "%01d", m_pTask->stModel.nSRData + 1 );
				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText(strNAME);
				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_X );
				strX.Format( "%7.3f", stWaper.dX[i] );
				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText(strX);
				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_Y );
				strY.Format( "%7.3f", stWaper.dY[i] );
				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText(strY);
				sprintf_s(m_pTask->stModel.stSRData[m_pTask->stModel.nSRData].szName, "%s", strNAME.GetString());
				m_pTask->stModel.stSRData[m_pTask->stModel.nSRData].dwXPosition = stWaper.dX[i];
				m_pTask->stModel.stSRData[m_pTask->stModel.nSRData].dwYPosition = stWaper.dY[i];
				m_pTask->stModel.nSRData++;
			}
		}
	}
}


void CPointDLG::OnBnClickedBtnCwAdd()
{
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
		if ( stWaper.bOK )
		{
			for( int i = 0; i < stWaper.nTotal; i++ )
			{
				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetRow( m_pTask->stModel.nSRData + 1 );
				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_NAME );
				strNAME.Format( "%01d", m_pTask->stModel.nSRData + 1 );
				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText(strNAME);
				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_X );
				strX.Format( "%7.3f", stWaper.dX[i] );
				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText(strX);
				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_Y );
				strY.Format( "%7.3f", stWaper.dY[i] );
				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText(strY);
				sprintf_s(m_pTask->stModel.stSRData[m_pTask->stModel.nSRData].szName, "%s", strNAME.GetString());
				m_pTask->stModel.stSRData[m_pTask->stModel.nSRData].dwXPosition = stWaper.dX[i];
				m_pTask->stModel.stSRData[m_pTask->stModel.nSRData].dwYPosition = stWaper.dY[i];
				m_pTask->stModel.nSRData++;
			}
		}
	}
}


void CPointDLG::OnBnClickedBtnCcwAdd()
{
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
		if ( stWaper.bOK )
		{
			for( int i = 0; i < stWaper.nTotal; i++ )
			{
				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetRow( m_pTask->stModel.nSRData + 1 );
				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_NAME );
				strNAME.Format( "%01d", m_pTask->stModel.nSRData + 1 );
				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText(strNAME);
				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_X );
				strX.Format( "%7.3f", stWaper.dX[i] );
				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText(strX);
				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_Y );
				strY.Format( "%7.3f", stWaper.dY[i] );
				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText(strY);
				sprintf_s(m_pTask->stModel.stSRData[m_pTask->stModel.nSRData].szName,"%s", strNAME.GetString());
				m_pTask->stModel.stSRData[m_pTask->stModel.nSRData].dwXPosition = stWaper.dX[i];
				m_pTask->stModel.stSRData[m_pTask->stModel.nSRData].dwYPosition = stWaper.dY[i];
				m_pTask->stModel.nSRData++;
			}
		}
	}
}


void CPointDLG::OnCbnSelchangeCboInchKind()
{
	CString strMM("");
	double dMM = 0.0;
	int nNo = m_clInchKind.GetCurSel();
	switch( nNo )
	{
		case 0:
			{
				theApp.m_fnGetViewPointer()->m_pModel->m_fnGetInchToMM( 4, dMM, theApp.m_fnGetViewPointer()->m_pModel->m_dWaperExclusion );
				strMM.Format( "%.3f", dMM );
				GetDlgItem( IDC_EDT_MM )->SetWindowText( strMM );
			}
			break;
		case 1:
			{
				theApp.m_fnGetViewPointer()->m_pModel->m_fnGetInchToMM( 6, dMM, theApp.m_fnGetViewPointer()->m_pModel->m_dWaperExclusion );
				strMM.Format( "%.3f", dMM );
				GetDlgItem( IDC_EDT_MM )->SetWindowText( strMM );
			}
			break;
		case 2:
			{
				theApp.m_fnGetViewPointer()->m_pModel->m_fnGetInchToMM( 8, dMM, theApp.m_fnGetViewPointer()->m_pModel->m_dWaperExclusion );
				strMM.Format( "%.3f", dMM );
				GetDlgItem( IDC_EDT_MM )->SetWindowText( strMM );
			}
			break;
		case 3:
			{
				theApp.m_fnGetViewPointer()->m_pModel->m_fnGetInchToMM( 12, dMM, theApp.m_fnGetViewPointer()->m_pModel->m_dWaperExclusion );
				strMM.Format( "%.3f", dMM );
				GetDlgItem( IDC_EDT_MM )->SetWindowText( strMM );
			}
			break;
	}
}

void CPointDLG::OnBnClickedBtnCurr()
{
	double dwPositionX = 0.0;
	double dwPositionY = 0.0;
#if AJINTEX_BOARD
	CString strData("");
	if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( X_AXIS, dwPositionX ) == RC_NORMAL )
	{
		strData.Format( "%7.3f", dwPositionX);
		GetDlgItem( IDC_EDT_EQ_START_X )->SetWindowText( strData );
	}
	if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( Y_AXIS, dwPositionY ) == RC_NORMAL )
	{
		strData.Format( "%7.3f", dwPositionY);
		GetDlgItem( IDC_EDT_EQ_START_Y )->SetWindowText( strData );
	}
#else
		GetDlgItem( IDC_EDT_EQ_START_X )->SetWindowText( "0.000" );
		GetDlgItem( IDC_EDT_EQ_START_Y )->SetWindowText( "0.000" );
#endif
}

void CPointDLG::OnBnClickedBtnRepeat()
{
	CString strX("");
	CString strY("");
	CString strRepeat("");
	CString strData("");
	CString strNAME("");
	double  dX      = 0.0;
	double  dY      = 0.0;
	int     nRepeat = 0;
	GetDlgItem( IDC_EDT_EQ_START_X )->GetWindowText( strX );
	GetDlgItem( IDC_EDT_EQ_START_Y )->GetWindowText( strY );
	GetDlgItem( IDC_EDT_EQ_REPEAT )->GetWindowText( strRepeat );
	strX.Trim();
	strY.Trim();
	strRepeat.Trim();

	if ( strX.IsEmpty())
	{
		strData.Format( "X 좌표를 입력해 주세요." );
		TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 1 );
		return;
	}
	if ( strX.IsEmpty())
	{
		strData.Format( "Y 좌표를 입력해 주세요." );
		TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 1 );
		return;
	}
	if ( strRepeat.IsEmpty())
	{
		strData.Format( "반복횟수를 입력해 주세요." );
		TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 1 );
		return;
	}

	dX      = atof( strX );
	dY      = atof( strY );
	nRepeat = atoi( strRepeat );

	if ( nRepeat == 0 ) return;

	m_pTask->stModel.nSRData = 0;
	memset( m_pTask->stModel.stSRData, 0x00, sizeof(m_pTask->stModel.stSRData));
	theApp.m_fnGetViewPointer()->m_pModel->m_fnSRSheetClear();
	for( int i = 0; i < nRepeat; i++ )
	{
		theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetRow( m_pTask->stModel.nSRData + 1 );
		theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_NAME );
		strNAME.Format( "%01d", m_pTask->stModel.nSRData + 1 );
		theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText(strNAME);
		theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_X );
		strX.Format( "%7.3f", dX );
		theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText(strX);
		theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_Y );
		strY.Format( "%7.3f", dY );
		theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText(strY);
		sprintf_s(m_pTask->stModel.stSRData[m_pTask->stModel.nSRData].szName, "%s", strNAME.GetString());
		m_pTask->stModel.stSRData[m_pTask->stModel.nSRData].dwXPosition = dX;
		m_pTask->stModel.stSRData[m_pTask->stModel.nSRData].dwYPosition = dY;
		m_pTask->stModel.nSRData++;
	}
}


void CPointDLG::OnBnClickedBtnHide()
{
	m_fnSetShowFlag( FALSE );
}

void CPointDLG::OnBnClickedBtnSquareAdd()
{
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

	nMethod = m_clMethod.GetCurSel();
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
				theApp.m_fnGetViewPointer()->m_fnLogWrite( strData );
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
				theApp.m_fnGetViewPointer()->m_fnLogWrite( strData );
				TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 1 );;
				return;
			}

			int nRow = 1;
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
								theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetRow( nRow );
								theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_NAME );
								strNAME.Format( "%01d", nRow);
								theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( strNAME );
								strData.Format( "%7.3f", dPositionX );
								theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_X );
								theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( strData );
								strData.Format( "%7.3f", dPositionY );
								theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_Y );
								theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( strData );
								sprintf_s(m_pTask->stModel.stSRData[m_pTask->stModel.nSRData].szName, "%s", strNAME.GetString());
								m_pTask->stModel.stSRData[m_pTask->stModel.nSRData].dwXPosition = dPositionX;
								m_pTask->stModel.stSRData[m_pTask->stModel.nSRData].dwYPosition = dPositionY;
								m_pTask->stModel.nSRData++;
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
									dPositionX = ( dStartX + ( dStepX * nCountX) ) - ( dStepX * k );
								}
								theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetRow( nRow );
								theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_NAME );
								strNAME.Format( "%01d", nRow);
								theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( strNAME );
								strData.Format( "%7.3f", dPositionX );
								theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_X );
								theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( strData );
								strData.Format( "%7.3f", dPositionY );
								theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_Y );
								theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( strData );
								sprintf_s(m_pTask->stModel.stSRData[m_pTask->stModel.nSRData].szName, "%s", strNAME.GetString());
								m_pTask->stModel.stSRData[m_pTask->stModel.nSRData].dwXPosition = dPositionX;
								m_pTask->stModel.stSRData[m_pTask->stModel.nSRData].dwYPosition = dPositionY;
								m_pTask->stModel.nSRData++;
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
								theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetRow( nRow );
								theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_NAME );
								strNAME.Format( "%01d", nRow);
								theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( strNAME );
								strData.Format( "%7.3f", dPositionX );
								theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_X );
								theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( strData );
								strData.Format( "%7.3f", dPositionY );
								theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_Y );
								theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( strData );
								sprintf_s(m_pTask->stModel.stSRData[m_pTask->stModel.nSRData].szName,"%s", strNAME.GetString());
								m_pTask->stModel.stSRData[m_pTask->stModel.nSRData].dwXPosition = dPositionX;
								m_pTask->stModel.stSRData[m_pTask->stModel.nSRData].dwYPosition = dPositionY;
								m_pTask->stModel.nSRData++;
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
									dPositionY = ( dStartY + ( dStepY * nCountY ) ) - ( dStepY * k );
								}
								theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetRow( nRow );
								theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_NAME );
								strNAME.Format( "%01d", nRow);
								theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( strNAME );
								strData.Format( "%7.3f", dPositionX );
								theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_X );
								theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( strData );
								strData.Format( "%7.3f", dPositionY );
								theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_Y );
								theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( strData );
								sprintf_s(m_pTask->stModel.stSRData[m_pTask->stModel.nSRData].szName, "%s", strNAME.GetString());
								m_pTask->stModel.stSRData[m_pTask->stModel.nSRData].dwXPosition = dPositionX;
								m_pTask->stModel.stSRData[m_pTask->stModel.nSRData].dwYPosition = dPositionY;
								m_pTask->stModel.nSRData++;
								nRow++;
							}
							bToggle = !bToggle;
						}
					}
					break;
				default:
					{
						strData = "등간격설정 : 등간격 지정 방법을 설정해 주세요";
						theApp.m_fnGetViewPointer()->m_fnLogWrite( strData );
						TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 1 );;

					}
			}
		}
		else
		{
			strData = "등간격설정 : 간격 또는 이동횟수에 값을 Zero 이상으로 설정해 주세요";
			theApp.m_fnGetViewPointer()->m_fnLogWrite( strData );
			TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 1 );;
			return;
		}
	}
}


void CPointDLG::OnBnClickedBtnCurrXy()
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
		GetDlgItem( IDC_EDT_CURR_Y )->SetWindowText( "0.000" );
#endif
}
