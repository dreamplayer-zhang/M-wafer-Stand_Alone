// ModelMapDLG.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EllipsoMeter.h"
#include "ModelMapDLG.h"
#include "afxdialogex.h"


// CModelMapDLG 대화 상자입니다.

IMPLEMENT_DYNAMIC(CModelMapDLG, CDialogEx)

CModelMapDLG::CModelMapDLG(CWnd* pParent /*=NULL*/)
	: CDialogEx(CModelMapDLG::IDD, pParent)
{
	this->Create( IDD_MODEL_MAP );
	m_bShow = FALSE;
	m_pTaskHandle       = NULL;
	m_pTask             = NULL;
}

CModelMapDLG::~CModelMapDLG()
{
}

void CModelMapDLG::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_CBO_METHOD, m_clMethod);
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CModelMapDLG, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_MODEL_MAP_HIDE, &CModelMapDLG::OnBnClickedBtnModelMapHide)
	ON_BN_CLICKED(IDC_BTN_CURR_LOC, &CModelMapDLG::OnBnClickedBtnCurrLoc)
	ON_BN_CLICKED(IDC_BTN_SET, &CModelMapDLG::OnBnClickedBtnSet)
	ON_BN_CLICKED(IDC_BTN_REPEAT, &CModelMapDLG::OnBnClickedBtnRepeat)
	ON_BN_CLICKED(IDC_BTN_LIGHT_VALUE, &CModelMapDLG::OnBnClickedBtnLightValue)
END_MESSAGE_MAP()

BOOL CModelMapDLG::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (( nID   == 2 ) &&
		( nCode == -1 ))
	{
		m_fnSetShowFlag( FALSE );
	}
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CModelMapDLG::m_fnSetShowFlag( BOOL bShow )
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

BOOL CModelMapDLG::PreTranslateMessage(MSG* pMsg) 
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
void CModelMapDLG::m_fnSetInit() 
{
	CString s("");
	s.Format( "등간격, 조명, 등고선정보 메인창");
	this->SetWindowText(s);

	m_pTaskHandle = new CTaskHandle( this, SHARED_NAME, 
											1, 
											sizeof( SHEET_RESISTANCE_Format ) + 1,
											0,
											FALSE );

	m_pTask					= ( SHEET_RESISTANCE_Format* )m_pTaskHandle->m_pFilePointer( );
}

//---------------------------------------------------------------------------------------------------------
// 종료에 필요한 작업을 한다.
//---------------------------------------------------------------------------------------------------------
void CModelMapDLG::m_fnSetEnd( ) 
{
	if ( m_pTaskHandle != NULL )
	{
		m_pTaskHandle->m_fnSetEnd();
		delete m_pTaskHandle;
		m_pTaskHandle = NULL;
	}
}

void CModelMapDLG::OnBnClickedBtnModelMapHide()
{
	m_fnSetShowFlag( FALSE );
}

void CModelMapDLG::OnBnClickedBtnCurrLoc()
{
#if AJINTEX_BOARD
	double dwPositionX = 0L;
	double dwPositionY = 0L;
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
#endif
}


void CModelMapDLG::OnBnClickedBtnSet()
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

	strData.Format( "기존 데이터는 삭제후 검사위치가 재 작성됩니다. 실행할까요?" );
	int nResult = TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 2 );
	if(nResult == 1)
	{
		GetDlgItem( IDC_EDT_EQ_START_X )->GetWindowText( strStartX );
		dStartX = atof( strStartX );
		GetDlgItem( IDC_EDT_EQ_START_Y )->GetWindowText( strStartY );
		dStartY = atof( strStartY );
		GetDlgItem( IDC_EDT_EQ_STEP_X )->GetWindowText( strStepX );
		dStepX = atof( strStepX );
		GetDlgItem( IDC_EDT_EQ_STEP_Y )->GetWindowText( strStepY );
		dStepY = atof( strStepY );
		GetDlgItem( IDC_EDT_EQ_COUNT_X )->GetWindowText( strCountX );
		nCountX = atoi( strCountX );
		GetDlgItem( IDC_EDT_EQ_COUNT_Y )->GetWindowText( strCountY );
		nCountY = atoi( strCountY );
		if (( dStepX > 0 )		&&
			( dStepY > 0 )		&&
			( nCountX > 0 )		&&
			( nCountY > 0 ))
		{

			if (( nCountX * nCountY ) > MAX_DATA )
			{
				strData.Format("등록할 데이터가 MAX_DATA(%01d)를 초과합니다. MAX_DATA 이내로 설정해 주세요", MAX_DATA );
				theApp.m_fnGetViewPointer()->m_pModel->m_fnLogWrite( strData );
				TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 1 );
				return;
			}

			if ( !theApp.m_fnGetViewPointer()->m_pModel->m_fnGetCheckAxisData( dStartX, dStartY, dStepX, dStepY, nCountX, nCountY ) )
			{
				strData.Format("등록할 데이터 값이 축위치값(X:%7.3f,%7.3f),(Y:%7.3f,%7.3f)) 초과했습니다.", 
																	theApp.m_fnGetViewPointer()->m_pTask->stCommon.dXMLimit, 
																	theApp.m_fnGetViewPointer()->m_pTask->stCommon.dXPLimit,
																	theApp.m_fnGetViewPointer()->m_pTask->stCommon.dYMLimit, 
																	theApp.m_fnGetViewPointer()->m_pTask->stCommon.dYPLimit);
				theApp.m_fnGetViewPointer()->m_pModel->m_fnLogWrite( strData );
				TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 1 );
				return;
			}

			m_pTask->stModel.nSRData = 0;
			memset( m_pTask->stModel.stSRData, 0x00, sizeof(t_DATA));
			for( int i = 0; i < MAX_DATA; i++ )
			{
				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetRow( i + 1 );
// 				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_USE );
// 				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( "0" );
				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_NAME );
				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( "" );
				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_X );
				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( "" );
				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_Y );
				theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( "" );
			}
			int nRow = 1;
			BOOL bToggle = FALSE;
			switch( nMethod )
			{ 
			case 0: // 0 : X축방향 Z
				{
					for( int j = 0; j < nCountY; j++ )
					{
						dPositionY = dStartY + ( dStepY * j );
						for( int k = 0; k < nCountX; k++ )
						{
							dPositionX = dStartX + ( dStepX * k );
							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetRow( nRow );
// 							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_USE );
// 							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( "1" );
							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_NAME );
							strData.Format( "%01d", nRow);
							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( strData );
							strData.Format( "%7.3f", dPositionX );
							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_X );
							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( strData );
							strData.Format( "%7.3f", dPositionY );
							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_Y );
							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( strData );
							m_pTask->stModel.stSRData[nRow-1].dwXPosition = dPositionX;
							m_pTask->stModel.stSRData[nRow-1].dwYPosition = dPositionY;
							sprintf_s( m_pTask->stModel.stSRData[nRow-1].szName, "%01d", nRow );
							nRow++;
						}
					}
					m_pTask->stModel.nSRData = nRow -1;
				}
				break;
			case 1: // 1 : X축방향 ㄹ
				{
					for( int j = 0; j < nCountY; j++ )
					{
						dPositionY = dStartY + ( dStepY * j );
						for( int k = 0; k < nCountX; k++ )
						{
							if ( !bToggle ) 
							{
								dPositionX = dStartX + ( dStepX * k );
							}
							else
							{
								dPositionX = ( dStartX + ( dStepX * ( nCountX - 1)) ) - ( dStepX * k );
							}
							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetRow( nRow );
// 							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_USE );
// 							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( "1" );
							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_NAME );
							strData.Format( "%01d", nRow);
							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( strData );
							strData.Format( "%7.3f", dPositionX );
							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_X );
							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( strData );
							strData.Format( "%7.3f", dPositionY );
							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_Y );
							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( strData );
							m_pTask->stModel.stSRData[nRow-1].dwXPosition = dPositionX;
							m_pTask->stModel.stSRData[nRow-1].dwYPosition = dPositionY;
							sprintf_s( m_pTask->stModel.stSRData[nRow-1].szName, "%01d", nRow );
							nRow++;
						}
						bToggle = !bToggle;
					}
					m_pTask->stModel.nSRData = nRow -1;
				}
				break;
			case 2: // 2 : Y축방향 Z
				{
					for( int j = 0; j < nCountX; j++ )
					{
						dPositionX = dStartX + ( dStepX * j );
						for( int k = 0; k < nCountY; k++ )
						{
							dPositionY = dStartY + ( dStepY * k );
							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetRow( nRow );
// 							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_USE );
// 							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( "1" );
							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_NAME );
							strData.Format( "%01d", nRow);
							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( strData );
							strData.Format( "%7.3f", dPositionX );
							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_X );
							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( strData );
							strData.Format( "%7.3f", dPositionY );
							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_Y );
							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( strData );
							m_pTask->stModel.stSRData[nRow-1].dwXPosition = dPositionX;
							m_pTask->stModel.stSRData[nRow-1].dwYPosition = dPositionY;
							sprintf_s(m_pTask->stModel.stSRData[nRow - 1].szName, "%01d", nRow);
							nRow++;
						}
					}
					m_pTask->stModel.nSRData = nRow -1;
				}
				break;
			case 3 : // 3 : Y축방향 ㄹ
				{
					for( int j = 0; j < nCountX; j++ )
					{
						dPositionX = dStartX + ( dStepX * j );
						for( int k = 0; k < nCountY; k++ )
						{
							if ( !bToggle ) 
							{
								dPositionY = dStartY + ( dStepY * k );
							}
							else
							{
								dPositionY = ( dStartY + ( dStepY * ( nCountY - 1)) ) - ( dStepY * k );
							}
							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetRow( nRow );
// 							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_USE );
// 							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( "1" );
							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_NAME );
							strData.Format( "%01d", nRow);
							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( strData );
							strData.Format( "%7.3f", dPositionX );
							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_X );
							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( strData );
							strData.Format( "%7.3f", dPositionY );
							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_Y );
							theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( strData );
							m_pTask->stModel.stSRData[nRow-1].dwXPosition = dPositionX;
							m_pTask->stModel.stSRData[nRow-1].dwYPosition = dPositionY;
							sprintf_s( m_pTask->stModel.stSRData[nRow-1].szName, "%01d", nRow );
							nRow++;
						}
						bToggle = !bToggle;
					}
					m_pTask->stModel.nSRData = nRow -1;
				}
				break;
			default:
				{
					strData = "등간격설정 : 등간격 지정 방법을 설정해 주세요";
					theApp.m_fnGetViewPointer()->m_pModel->m_fnLogWrite( strData );
					TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 1 );
				}
			}
		}
		else
		{
			strData = "등간격설정 : 간격 또는 이동횟수에 값을 Zero 이상으로 설정해 주세요";
			theApp.m_fnGetViewPointer()->m_pModel->m_fnLogWrite( strData );
			TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 1 );
			return;
		}
	}
}


void CModelMapDLG::OnBnClickedBtnRepeat()
{
	CString strCount("");
	CString strData("");
	CString strDataX("");
	CString strDataY("");
	CString strDataZ("0.000");
	double  dPosition  = 0.0;
	double  dPositionX = 0.0;
	double  dPositionY = 0.0;
	GetDlgItem( IDC_EDT_REPEAT )->GetWindowText( strCount );
	GetDlgItem( IDC_EDT_EQ_START_X )->GetWindowText( strDataX );
	GetDlgItem( IDC_EDT_EQ_START_Y )->GetWindowText( strDataY );
	dPositionX = atof( strDataX );
	dPositionY = atof( strDataY );

	int nRepeatCount = atoi( strCount );
	if (( nRepeatCount <= 0 ) ||
		( nRepeatCount > MAX_DATA ))
	{
		strData.Format( "지정 가능한 횟수는 %01d ~ %01d 입니다.", 1, MAX_DATA );
		TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 1 );
		return;
	}
	strData.Format( "기존 데이터는 삭제후 반복측정위치가 재 작성됩니다. 실행할까요?" );
	int nResult = TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 2 );
	if(nResult == 1)
	{
		m_pTask->stModel.nSRData = 0;
		memset( m_pTask->stModel.stSRData, 0x00, sizeof(t_DATA));
		for( int i = 0; i < MAX_DATA; i++ )
		{
			theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetRow( i + 1 );
// 			theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_USE );
// 			theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( "0" );
			theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_NAME );
			theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( "" );
			theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_X );
			theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( "" );
			theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_Y );
			theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( "" );
		}
		int nRow = 1;
		for( int i = 0; i < nRepeatCount; i++ )
		{
			theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetRow( nRow );
// 			theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_USE );
// 			theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( "1" );
			theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_NAME );
			strData.Format( "%01d", i + 1);
			theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( strData );
			theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_X );
			theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( strDataX );
			theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetCol( MEASURE_Y );
			theApp.m_fnGetViewPointer()->m_pModel->m_clSR.SetText( strDataX );
			m_pTask->stModel.stSRData[i].dwXPosition = dPositionX;
			m_pTask->stModel.stSRData[i].dwYPosition = dPositionY;
			sprintf_s( m_pTask->stModel.stSRData[i].szName, "%01d", nRow);
			nRow++;
		}
		m_pTask->stModel.nSRData = nRow-1;
	}
}


void CModelMapDLG::OnBnClickedBtnLightValue()
{
}
