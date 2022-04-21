// JogMove.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EllipsoMeter.h"
#include "JogMove.h"
#include "afxdialogex.h"
#include "WorkRun.h"

// CJogMove 대화 상자입니다.

IMPLEMENT_DYNAMIC(CJogMove, CDialogEx)

CJogMove::CJogMove(CWnd* pParent /*=NULL*/)
	: CDialogEx(CJogMove::IDD, pParent)
	, m_dEditIncZ(0)
{
	m_bShow = FALSE;

	this->Create( IDD_JOGMOVE );

	m_hJogMoveThread	= NULL;
	m_bJogMoveEnd		= FALSE;

	m_dX				= 0.0;
	m_dY				= 0.0;
	m_dZ				= 0.0;
	m_dR				= 0.0;
	m_nXY				= JOG_STOP;
	m_nZ                = JOG_STOP;
	m_bJogDisp			= FALSE;
}

CJogMove::~CJogMove()
{
}

void CJogMove::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_BTN_UP, m_clUp);
	DDX_Control(pDX, IDC_BTN_DOWN, m_clDown);
	DDX_Control(pDX, IDC_BTN_FRONT, m_clFront);
	DDX_Control(pDX, IDC_BTN_BACK, m_clBack);
	DDX_Control(pDX, IDC_BTN_LEFT, m_clLeft);
	DDX_Control(pDX, IDC_BTN_RIGHT, m_clRight);
	DDX_Control(pDX, IDC_BTN_CW, m_clCw);
	DDX_Control(pDX, IDC_BTN_CCW, m_clCcw);
	DDX_Control(pDX, IDC_CBO_JOG_SPEED, m_clJogSpeed);
	DDX_Control(pDX, IDC_CBO_JOG_SPEED_Z, m_clJogSpeedZ);
	DDX_Control(pDX, IDC_CBO_JOG_SPEED_R, m_clJogSpeedR);
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_INC_Z, m_ctrlEditIncZ);
	DDX_Text(pDX, IDC_EDIT_INC_Z, m_dEditIncZ);
}


BEGIN_MESSAGE_MAP(CJogMove, CDialogEx)
	ON_MESSAGE(WM_SET_JOYSTICK_XY, m_fnSetJoyStickXY ) 
	ON_MESSAGE(WM_SET_JOYSTICK_Z, m_fnSetJoyStickZ ) 
	ON_MESSAGE(WM_SET_JOG, m_fnSetJog ) 
	ON_MESSAGE(WM_SET_JOG, m_fnSetJog ) 
	ON_MESSAGE(UM_LBUTTONUP, OnLButtonUp)
	ON_MESSAGE(UM_LBUTTONDOWN, OnLButtonDown)
	ON_MESSAGE(UM_MOUSEMOVE, OnMouseMove)
	ON_BN_CLICKED(IDC_BTN_UP, &CJogMove::OnBnClickedBtnUp)
	ON_BN_CLICKED(IDC_BUTTON_INC_Z_MOVE, &CJogMove::OnBnClickedButtonIncZMove)
END_MESSAGE_MAP()

BOOL CJogMove::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (( nID   == 2 ) &&
		( nCode == -1 ))
	{
		m_fnSetShowFlag( FALSE );
	}
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CJogMove::m_fnSetShowFlag( BOOL bShow )
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

BOOL CJogMove::PreTranslateMessage(MSG* pMsg) 
{
	// ENTER KEY or ESC KEY CANCEL
	if (( pMsg->wParam == 13 ) || 
		( pMsg->wParam == 27 ))
	{
		return FALSE;
	}

#if AJINTEX_BOARD
	int wCurInput = 0;
	int iCurSpeedSet;// = m_clJogSpeed.GetCurSel();// theApp.m_fnGetViewPointer()->m_pTask->stEqp.nJoyStickSpeed;

	double dMaxSpeed[3] = { 100.0, 10.0, 1.0 };
	double dMaxSpeedZ[3] = { 300.0, 3.0, 0.05 };
	double dMinSpeed = 0.001;
	double dSpeedRespon = 0.0;

	double dSpeed = 0.0;
	double dAccDcc = 150000;

	DWORD dwRetCode;
	CString strData;

	switch (pMsg->message)
	{
		//pMsg->wParam   //pMsg->lParam 하위:X / 상위:Y
	case MM_JOY1MOVE:
#if MIL_USE
// 		if(pWorkRun != NULL)
// 			if (pWorkRun->m_bThrdFlag[TF_AUTORUN])
// 				break;
#endif
		//pMsg->lParam;//좌상:0000 0000 우하:ffff ffff 우상:0000 ffff 좌하:ffff 0000
		iCurSpeedSet = m_clJogSpeed.GetCurSel();

		//X_axis
		wCurInput = (int)LOWORD(pMsg->lParam) - (int)0x7fef;
		strData.Format("%01d", wCurInput);
		theApp.m_fnGetViewPointer()->m_pStageMap->GetDlgItem(IDC_EDT_POINT_X)->SetWindowText(strData);
		if( abs(wCurInput) > 0x0700)
		{
			dSpeedRespon = (double)(dMaxSpeed[iCurSpeedSet] / 0x7fef);
			if (wCurInput >= 0)
				dSpeed = dSpeedRespon* (wCurInput - 0x700);
			else
				dSpeed = dSpeedRespon* (wCurInput + 0x700);
			// 			dwRetCode = theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveVel(X_AXIS, dSpeed, JOG_ACC_DEC, JOG_ACC_DEC);
			if (pAxis[X_AXIS]->CheckInMotion())
			{
				dwRetCode = AxmOverrideVel(X_AXIS, fabs(dSpeed)*1000);
			}
			else
			{
				AxmOverrideSetMaxVel(X_AXIS, 150000);
				if (dSpeed >= 0)
					dwRetCode = theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveStartPos(X_AXIS, theApp.m_fnGetViewPointer()->m_pTask->stCommon.dXPLimit, fabs(dSpeed), dAccDcc * 2, dAccDcc * 4);
				else																																 
					dwRetCode = theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveStartPos(X_AXIS, theApp.m_fnGetViewPointer()->m_pTask->stCommon.dXMLimit, fabs(dSpeed), dAccDcc * 2, dAccDcc * 4);
			}

			if (dwRetCode != TRUE)
				break;
		}
		else
		{
			dwRetCode = theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveSStop(X_AXIS);
			if (dwRetCode != TRUE)
				break;
		}


		//Y_axis
		wCurInput = (int)HIWORD(pMsg->lParam) - (int)0x7fef;
		strData.Format("%01d", wCurInput);
		theApp.m_fnGetViewPointer()->m_pStageMap->GetDlgItem(IDC_EDT_POINT_Y)->SetWindowText(strData);
		if (abs(wCurInput) > 0x0700)
		{
			dSpeedRespon = (double)(dMaxSpeed[iCurSpeedSet] / 0x7fef);
			if (wCurInput >= 0)
				dSpeed = dSpeedRespon* (wCurInput - 0x700);
			else
				dSpeed = dSpeedRespon* (wCurInput + 0x700);
			// 			dwRetCode = theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveVel(Y_AXIS, dSpeed, JOG_ACC_DEC, JOG_ACC_DEC);
			if (pAxis[Y_AXIS]->CheckInMotion())
			{
				dwRetCode = AxmOverrideVel(Y_AXIS, fabs(dSpeed) * 1000);
			}
			else
			{
				AxmOverrideSetMaxVel(Y_AXIS, 150000);
				if (dSpeed >= 0)
					dwRetCode = theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveStartPos(Y_AXIS, theApp.m_fnGetViewPointer()->m_pTask->stCommon.dYPLimit, fabs(dSpeed), dAccDcc * 2, dAccDcc * 4);
				else
					dwRetCode = theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveStartPos(Y_AXIS, theApp.m_fnGetViewPointer()->m_pTask->stCommon.dYMLimit, fabs(dSpeed), dAccDcc * 2, dAccDcc * 4);
			}

			if (dwRetCode != TRUE)
				break;
		}
		else
		{
			dwRetCode = theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveSStop(Y_AXIS);
			if (dwRetCode != TRUE)
				break;
		}


		break;
	case MM_JOY1ZMOVE:
// 		i = pMsg->lParam;//반시계:0000 시계:ffff
		iCurSpeedSet = m_clJogSpeedZ.GetCurSel();

		//Z_axis
		wCurInput = (int)LOWORD(pMsg->lParam) - (int)0x7fef;
		if (abs(wCurInput) > 0x0500)
		{
			dSpeedRespon = (double)(dMaxSpeedZ[iCurSpeedSet] / 0x7fef);
			if (wCurInput >= 0)
				dSpeed = dSpeedRespon* (wCurInput - 0x0500);
			else
				dSpeed = dSpeedRespon* (wCurInput + 0x0500);
			// 			dwRetCode = theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveVel(Z_AXIS, dSpeed, JOG_ACC_DEC, JOG_ACC_DEC);
			if (pAxis[Z_AXIS]->CheckInMotion())
			{
				dwRetCode = AxmOverrideVel(Z_AXIS, fabs(dSpeed) * 1000);
			}
			else
			{
				if		(iCurSpeedSet == 0)					AxmOverrideSetMaxVel(Z_AXIS, 15000);
				else if (iCurSpeedSet == 1)					AxmOverrideSetMaxVel(Z_AXIS, 500);
				else										AxmOverrideSetMaxVel(Z_AXIS, 5);

				if (dSpeed >= 0)
					dwRetCode = theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveStartPos(Z_AXIS, theApp.m_fnGetViewPointer()->m_pTask->stCommon.dZLimit, fabs(dSpeed), dAccDcc*2, dAccDcc *4);
				else
					dwRetCode = theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveStartPos(Z_AXIS, -1.0, fabs(dSpeed), dAccDcc, dAccDcc * 2);
			}

			if (dwRetCode != AXT_RT_SUCCESS)
				break;
		}
		else
		{
			dwRetCode = theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveSStop(Z_AXIS);
			if (dwRetCode != AXT_RT_SUCCESS)
				break;
		}


		break;
	case MM_JOY1BUTTONDOWN:
// 		i = pMsg->wParam;//1~4 0x0000 0101 0202 0404 0808
		dwRetCode = theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmMoveAllStop();
		break;
	case MM_JOY1BUTTONUP:
// 		i = pMsg->wParam;//1~4 0x0000 0100 0200 0400 0800
		break;
	default:
		break;
	}


	
#endif
	return CDialog::PreTranslateMessage(pMsg);
}

LRESULT CJogMove::m_fnSetJoyStickXY(  WPARAM nNo, LPARAM lParam )
{
	CString strData("");
	if ( !theApp.m_fnGetViewPointer( )->m_pAjin->m_bInitEnd )
	{
		CString strMsg = "장비를 초기화후 진행해 주세요";
		theApp.m_fnGetViewPointer()->m_fnLogWrite( strMsg );
		return 0;
	}

#if AJINTEX_BOARD
	theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveSStop( X_AXIS );	
	theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveSStop( Y_AXIS );	
#endif

	double dSpeed = 0.0;
	switch( theApp.m_fnGetViewPointer()->m_pTask->stEqp.nJoyStickSpeed )
	{
		case JOG_SLOW:
			{
				dSpeed = theApp.m_fnGetViewPointer()->m_pTask->stCommon.dSpeedSlow;
			}
			break;
		case JOG_MIDDLE:
			{
				dSpeed = theApp.m_fnGetViewPointer()->m_pTask->stCommon.dSpeedMiddle;
			}
			break;
		case JOG_HIGH:
			{
				dSpeed = theApp.m_fnGetViewPointer()->m_pTask->stCommon.dSpeedHigh;
			}
			break;
	}

#if AJINTEX_BOARD
	double dwPosition = 0.0;
	if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( Z_AXIS, dwPosition ) == RC_NORMAL )
	{
		if ( dwPosition > theApp.m_fnGetViewPointer()->m_pTask->stCommon.dZLimit )
		{
			if ( m_bJogDisp ) return FALSE;
			m_bJogDisp = TRUE;
			TECmsgOut( L"Z축이 낮아서 Stage와 충돌할수 있어서 이동불가 합니다.", RED_1, 1 );
			m_bJogDisp = FALSE;
			return false;
		}
		if ( theApp.m_fnGetViewPointer()->m_pModel->m_fnGetMeasureSafetyZ( theApp.m_fnGetViewPointer()->m_pTask->stModel.dWaperThickness ) > theApp.m_fnGetViewPointer()->m_pTask->stCommon.dZLimit )
		{
			if ( m_bJogDisp ) return FALSE;
			m_bJogDisp = TRUE;
			TECmsgOut( L"Z축 Limit 위치보다 Z측정 안정위치가 낮아서 Stage와 충돌할수 있어서 이동불가 합니다.", RED_1, 1 );
			m_bJogDisp = FALSE;
			return false;
		}
	}
#endif

	switch( nNo )
	{
		case JOG_UP:
			{
#if AJINTEX_BOARD
				DWORD dwRetCode = theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveVel(Y_AXIS, -dSpeed, JOG_ACC_DEC, JOG_ACC_DEC );
				if(dwRetCode != AXT_RT_SUCCESS)
				{
				}
#endif
			}
			break;
		case JOG_DOWN:
			{
#if AJINTEX_BOARD
				DWORD dwRetCode = theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveVel(Y_AXIS, dSpeed, JOG_ACC_DEC, JOG_ACC_DEC );
				if(dwRetCode != AXT_RT_SUCCESS)
				{
				}
#endif
			}
			break;
		case JOG_LEFT:
			{
#if AJINTEX_BOARD
				DWORD dwRetCode = theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveVel(X_AXIS, -dSpeed, JOG_ACC_DEC, JOG_ACC_DEC );
				if(dwRetCode != AXT_RT_SUCCESS)
				{
				}
#endif
			}
			break;
		case JOG_RIGHT:
			{
#if AJINTEX_BOARD
				DWORD dwRetCode = theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveVel(X_AXIS, dSpeed, JOG_ACC_DEC, JOG_ACC_DEC );
				if(dwRetCode != AXT_RT_SUCCESS)
				{
				}
#endif
			}
			break;
		case JOG_UP_RIGHT:
			{
#if AJINTEX_BOARD
				DWORD dwRetCode1 = theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveVel(X_AXIS, dSpeed, JOG_ACC_DEC, JOG_ACC_DEC );
				DWORD dwRetCode2 = theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveVel(Y_AXIS, -dSpeed, JOG_ACC_DEC, JOG_ACC_DEC );
				if ((dwRetCode1 != AXT_RT_SUCCESS) ||
				    (dwRetCode2 != AXT_RT_SUCCESS))
				{
				}
#endif
			}
			break;
		case JOG_UP_LEFT:
			{
#if AJINTEX_BOARD
				DWORD dwRetCode1 = theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveVel(X_AXIS, -dSpeed, JOG_ACC_DEC, JOG_ACC_DEC );
				DWORD dwRetCode2 = theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveVel(Y_AXIS, -dSpeed, JOG_ACC_DEC, JOG_ACC_DEC );
				if ((dwRetCode1 != AXT_RT_SUCCESS) ||
				    (dwRetCode2 != AXT_RT_SUCCESS))
				{
				}
#endif
			}
			break;
		case JOG_DOWN_RIGHT:
			{
#if AJINTEX_BOARD
				DWORD dwRetCode1 = theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveVel(X_AXIS, dSpeed, JOG_ACC_DEC, JOG_ACC_DEC );
				DWORD dwRetCode2 = theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveVel(Y_AXIS, dSpeed, JOG_ACC_DEC, JOG_ACC_DEC );
				if ((dwRetCode1 != AXT_RT_SUCCESS) ||
				    (dwRetCode2 != AXT_RT_SUCCESS))
				{
				}
#endif
			}
			break;
		case JOG_DOWN_LEFT:
			{
#if AJINTEX_BOARD
				DWORD dwRetCode1 = theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveVel(X_AXIS, -dSpeed, JOG_ACC_DEC, JOG_ACC_DEC );
				DWORD dwRetCode2 = theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveVel(Y_AXIS, dSpeed, JOG_ACC_DEC, JOG_ACC_DEC );
				if ((dwRetCode1 != AXT_RT_SUCCESS) ||
				    (dwRetCode2 != AXT_RT_SUCCESS))
				{
				}
#endif
			}
			break;
		case JOG_STOP:
			{
#if AJINTEX_BOARD
				theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveSStop( X_AXIS );	
				theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveSStop( Y_AXIS );	
#endif
			}
			break;
	}
	return 0;
}

LRESULT CJogMove::m_fnSetJoyStickZ(  WPARAM nNo, LPARAM lParam )
{
	CString strData("");
	if ( !theApp.m_fnGetViewPointer( )->m_pAjin->m_bInitEnd )
	{
		CString strMsg = "장비를 초기화후 진행해 주세요";
		theApp.m_fnGetViewPointer()->m_fnLogWrite( strMsg );
		return 0;
	}

#if AJINTEX_BOARD
	theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveSStop( Z_AXIS );	
#endif

	double dSpeed = 0.0;
	switch( theApp.m_fnGetViewPointer()->m_pTask->stEqp.nJoyStickSpeed )
	{
		case JOG_SLOW:
			{
				dSpeed = theApp.m_fnGetViewPointer()->m_pTask->stCommon.dSpeedSlowZ;
			}
			break;
		case JOG_MIDDLE:
			{
				dSpeed = theApp.m_fnGetViewPointer()->m_pTask->stCommon.dSpeedMiddleZ;
			}
			break;
		case JOG_HIGH:
			{
				dSpeed = theApp.m_fnGetViewPointer()->m_pTask->stCommon.dSpeedHighZ;
			}
			break;
	}

	switch( nNo )
	{
		case JOG_UP:
			{
#if AJINTEX_BOARD
				DWORD dwRetCode = theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveVel(Z_AXIS, -dSpeed, JOG_ACC_DEC, JOG_ACC_DEC );
				if(dwRetCode != AXT_RT_SUCCESS)
				{
				}
#endif
			}
			break;
		case JOG_DOWN:
			{
#if AJINTEX_BOARD
				DWORD dwRetCode = theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveVel(Z_AXIS, dSpeed, JOG_ACC_DEC, JOG_ACC_DEC );
				if(dwRetCode != AXT_RT_SUCCESS)
				{
				}
#endif
			}
			break;
		case JOG_STOP:
			{
#if AJINTEX_BOARD
				theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveSStop( Z_AXIS );	
#endif
			}
			break;
	}
	return 0;
}

LRESULT CJogMove::m_fnSetJog(  WPARAM nNo, LPARAM lParam )
{
	switch( nNo )
	{
		case X_AXIS:
			{
				m_fnSetEdit(IDC_EDT_JOG_X, m_dX );
			}
			break;
		case Y_AXIS:
			{
				m_fnSetEdit(IDC_EDT_JOG_Y, m_dY );
			}
			break;
		case Z_AXIS:
			{
				m_fnSetEdit(IDC_EDT_JOG_Z, m_dZ );
			}
			break;
	}
	return 0;
}
//---------------------------------------------------------------------------------------------------------
// 시작에 필요한 작업을 한다.
//---------------------------------------------------------------------------------------------------------
void CJogMove::m_fnSetInit( ) 
{
	CString s("");
	s.Format( "JOG 이동 관리화면" );
	this->SetWindowText(s);

	this->MoveWindow( 1200, 800, 700,160, TRUE );

	m_clUp.SetID( JOG_UP );
	m_clUp.SetWindowText( "" );
	m_clUp.SetBitmaps( IDB_BITMAP_UP_1, IDB_BITMAP_UP_2 );

	m_clDown.SetID( JOG_DOWN );
	m_clDown.SetWindowText( "" );
	m_clDown.SetBitmaps( IDB_BITMAP_DOWN_1, IDB_BITMAP_DOWN_2 );

	m_clFront.SetID( JOG_FRONT );
	m_clFront.SetWindowText( "" );
	m_clFront.SetBitmaps( IDB_BITMAP_DOWN_1, IDB_BITMAP_DOWN_2 );

	m_clBack.SetID( JOG_BACK );
	m_clBack.SetWindowText( "" );
	m_clBack.SetBitmaps( IDB_BITMAP_UP_1, IDB_BITMAP_UP_2 );

	m_clLeft.SetID( JOG_LEFT );
	m_clLeft.SetWindowText( "" );
	m_clLeft.SetBitmaps( IDB_BITMAP_RIGHT_1, IDB_BITMAP_RIGHT_2 );

	m_clRight.SetID( JOG_RIGHT );
	m_clRight.SetWindowText( "" );
	m_clRight.SetBitmaps( IDB_BITMAP_LEFT_1, IDB_BITMAP_LEFT_2 );

	m_clCcw.SetID( JOG_CW );
	m_clCcw.SetWindowText( "" );
	m_clCcw.SetBitmaps( IDB_BITMAP_LEFT_1, IDB_BITMAP_LEFT_2 );

	m_clCw.SetID( JOG_CCW );
	m_clCw.SetWindowText( "" );
	m_clCw.SetBitmaps( IDB_BITMAP_RIGHT_1, IDB_BITMAP_RIGHT_2 );

	m_clJogSpeed.SetCurSel( 1 );
	m_clJogSpeedZ.SetCurSel(1);
	m_clJogSpeedR.SetCurSel(1);

	unsigned long	ThreadID;
	m_bJogMoveEnd    = TRUE;
	m_hJogMoveThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)JogMoveThread, this, NULL, &ThreadID);
}

//---------------------------------------------------------------------------------------------------------
// 시작에 필요한 작업을 한다.
//---------------------------------------------------------------------------------------------------------
void CJogMove::m_fnSetEnd( ) 
{

	m_bJogMoveEnd			= FALSE;

	Sleep(100);

	if ( m_hJogMoveThread != NULL )
	{
		SuspendThread( m_hJogMoveThread );
		CloseHandle(m_hJogMoveThread);
		m_hJogMoveThread	    = NULL;
	}
}

void CJogMove::m_fnSetImageButton( BOOL bUp, short nNo )
{
	if ( theApp.m_fnGetViewPointer()->m_pTask->stEqp.bIoStartButton )
	{
		theApp.m_fnGetViewPointer()->m_fnLogWrite( "자동 Start 모드에서는 사용할수 없습니다." );
		return;
	}

	int nSpeed		   = 0;
	double dwAxisSpeed = 0.0;
	switch (nNo)
	{
		case JOG_RIGHT:
		case JOG_LEFT:
		case JOG_FRONT:
		case JOG_BACK:
			{
				nSpeed = m_clJogSpeed.GetCurSel();
				if (nSpeed == -1)
				{
					theApp.m_fnGetViewPointer()->m_fnLogWrite("Jog Speed 를 선택해 주세요......");
					return;
				}
				switch (nSpeed)
				{
					case 0:
						{
							dwAxisSpeed = theApp.m_fnGetViewPointer()->m_pTask->stCommon.dSpeedHigh;
						}
						break;
					case 1:
						{
							dwAxisSpeed = theApp.m_fnGetViewPointer()->m_pTask->stCommon.dSpeedMiddle;
						}
						break;
					case 2:
						{
							dwAxisSpeed = theApp.m_fnGetViewPointer()->m_pTask->stCommon.dSpeedSlow;
						}
						break;
				}
			}
			break;
		case JOG_UP:
		case JOG_DOWN:
			{
				nSpeed = m_clJogSpeedZ.GetCurSel();
				if (nSpeed == -1)
				{
					theApp.m_fnGetViewPointer()->m_fnLogWrite("Jog Speed 를 선택해 주세요......");
					return;
				}
				switch (nSpeed)
				{
					case 0:
						{
							dwAxisSpeed = theApp.m_fnGetViewPointer()->m_pTask->stCommon.dSpeedHighZ;
						}
						break;
					case 1:
						{
							dwAxisSpeed = theApp.m_fnGetViewPointer()->m_pTask->stCommon.dSpeedMiddleZ;
						}
						break;
					case 2:
						{
							dwAxisSpeed = theApp.m_fnGetViewPointer()->m_pTask->stCommon.dSpeedSlowZ;
						}
						break;
				}
			}
			break;
		case JOG_CW:
		case JOG_CCW:
			{
				nSpeed = m_clJogSpeedR.GetCurSel();
				if (nSpeed == -1)
				{
					theApp.m_fnGetViewPointer()->m_fnLogWrite("Jog Speed 를 선택해 주세요......");
					return;
				}
				switch (nSpeed)
				{
					case 0:
						{
							dwAxisSpeed = theApp.m_fnGetViewPointer()->m_pTask->stCommon.dSpeedHighZ;
						}
						break;
					case 1:
						{
							dwAxisSpeed = theApp.m_fnGetViewPointer()->m_pTask->stCommon.dSpeedMiddleZ;
						}
						break;
					case 2:
						{
							dwAxisSpeed = theApp.m_fnGetViewPointer()->m_pTask->stCommon.dSpeedSlowZ;
						}
						break;
				}
			}
			break;
	}

	int nAxisNo = -1;
	switch( nNo )
	{
		case JOG_RIGHT:
			{
				nAxisNo = X_AXIS;
				dwAxisSpeed *= 1;
			}
			break;
		case JOG_LEFT:
			{
				nAxisNo = X_AXIS;
				dwAxisSpeed *= -1;
			}
			break;
		case JOG_UP:
			{
				nAxisNo = Z_AXIS;
				dwAxisSpeed *= -1;
			}
			break;
		case JOG_DOWN:
			{
				nAxisNo = Z_AXIS;
				dwAxisSpeed *= 1;
			}
			break;
		case JOG_FRONT:
			{
				nAxisNo = Y_AXIS;
				dwAxisSpeed *= 1;
			}
			break;
		case JOG_BACK:
			{
				nAxisNo = Y_AXIS;
				dwAxisSpeed *= -1;
			}
			break;
		default:
			{
				theApp.m_fnGetViewPointer()->m_fnLogWrite( "사용할수 없는 축입니다" );
				return;
			}
			break;
	}

	switch( nNo )
	{
		case JOG_UP:
			{
				if ( bUp )
				{
					m_clUp.SetBitmaps( IDB_BITMAP_UP_1, IDB_BITMAP_UP_2 );
				}
				else
				{
					m_clUp.SetBitmaps( IDB_BITMAP_UP_3, IDB_BITMAP_UP_4 );
				}
			}
			break;
		case JOG_DOWN:
			{
				if ( bUp )
				{
					m_clDown.SetBitmaps( IDB_BITMAP_DOWN_1, IDB_BITMAP_DOWN_2 );
				}
				else
				{
					m_clDown.SetBitmaps( IDB_BITMAP_DOWN_3, IDB_BITMAP_DOWN_4 );
				}
			}
			break;
		case JOG_FRONT:
			{
				if ( bUp )
				{
					m_clFront.SetBitmaps( IDB_BITMAP_DOWN_1, IDB_BITMAP_DOWN_2 );
				}
				else
				{
					m_clFront.SetBitmaps( IDB_BITMAP_DOWN_3, IDB_BITMAP_DOWN_4 );
				}
			}
			break;
		case JOG_BACK:
			{
				if ( bUp )
				{
					m_clBack.SetBitmaps( IDB_BITMAP_UP_1, IDB_BITMAP_UP_2 );
				}
				else
				{
					m_clBack.SetBitmaps( IDB_BITMAP_UP_3, IDB_BITMAP_UP_4 );
				}
			}
			break;
		case JOG_LEFT:
			{
				if ( bUp )
				{
					m_clLeft.SetBitmaps( IDB_BITMAP_RIGHT_1, IDB_BITMAP_RIGHT_2 );
				}
				else
				{
					m_clLeft.SetBitmaps( IDB_BITMAP_RIGHT_3, IDB_BITMAP_RIGHT_4 );
				}
			}
			break;
		case JOG_RIGHT:
			{
				if ( bUp )
				{
					m_clRight.SetBitmaps( IDB_BITMAP_LEFT_1, IDB_BITMAP_LEFT_2 );
				}
				else
				{
					m_clRight.SetBitmaps( IDB_BITMAP_LEFT_3, IDB_BITMAP_LEFT_4 );
				}
			}
			break;
		case JOG_CW:
			{
				if ( bUp )
				{
					m_clCw.SetBitmaps( IDB_BITMAP_RIGHT_1, IDB_BITMAP_RIGHT_2 );
				}
				else
				{
					m_clCw.SetBitmaps( IDB_BITMAP_RIGHT_3, IDB_BITMAP_RIGHT_4 );
				}
			}
			break;
		case JOG_CCW:
			{
				if ( bUp )
				{
					m_clCcw.SetBitmaps( IDB_BITMAP_LEFT_1, IDB_BITMAP_LEFT_2 );
				}
				else
				{
					m_clCcw.SetBitmaps( IDB_BITMAP_LEFT_3, IDB_BITMAP_LEFT_4 );
				}
			}
			break;
	}
	if ( bUp )
	{
#if AJINTEX_BOARD
		theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveSStop( nAxisNo );	
#endif
	}
	else
	{
#if AJINTEX_BOARD
		double dwPosition = 0.0;
		if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( Z_AXIS, dwPosition ) == RC_NORMAL )
		{
			if ( nAxisNo != Z_AXIS )
			{
				if ( dwPosition > theApp.m_fnGetViewPointer()->m_pTask->stCommon.dZLimit )
				{
					if ( m_bJogDisp ) return;
					m_bJogDisp = TRUE;
					TECmsgOut( L"Z축이 낮아서 Stage와 충돌할수 있어서 이동불가 합니다.", RED_1, 1 );
					m_bJogDisp = FALSE;
					return;
				}
			}
			if ( theApp.m_fnGetViewPointer()->m_pModel->m_fnGetMeasureSafetyZ( theApp.m_fnGetViewPointer()->m_pTask->stModel.dWaperThickness ) > theApp.m_fnGetViewPointer()->m_pTask->stCommon.dZLimit )
			{
				if ( m_bJogDisp ) return;
				m_bJogDisp = TRUE;
				TECmsgOut( L"Z축 Limit 위치보다 Z측정 안정위치가 낮아서 Stage와 충돌할수 있어서 이동불가 합니다.", RED_1, 1 );
				m_bJogDisp = FALSE;
				return;
			}
		}
		CString strData("");
		if ( !theApp.m_fnGetViewPointer( )->m_pAjin->m_bInitEnd )
		{
			CString strMsg = "장비를 초기화후 진행해 주세요";
			theApp.m_fnGetViewPointer()->m_fnLogWrite( strMsg );
			TECmsgOut( m_Util.CStringTobstr(strMsg), GREEN_1, 1 );
			return;
		}
		DWORD dwRetCode = theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveVel(nAxisNo, dwAxisSpeed, JOG_ACC_DEC, JOG_ACC_DEC );
		if(dwRetCode != AXT_RT_SUCCESS)
		{
		}
#endif
	}
}

void CJogMove::OnCbnSelchangeCboJogMove()
{
	GetDlgItem( IDC_BTN_UP )->ShowWindow( TRUE );
	GetDlgItem( IDC_BTN_DOWN )->ShowWindow( TRUE );
	GetDlgItem( IDC_BTN_FRONT )->ShowWindow( TRUE );
	GetDlgItem( IDC_BTN_BACK )->ShowWindow( TRUE );
	GetDlgItem( IDC_BTN_LEFT )->ShowWindow( TRUE );
	GetDlgItem( IDC_BTN_RIGHT )->ShowWindow( TRUE );
	GetDlgItem( IDC_BACK )->ShowWindow( TRUE );
	GetDlgItem( IDC_FRONT )->ShowWindow( TRUE );
	GetDlgItem( IDC_LEFT )->ShowWindow( TRUE );
	GetDlgItem( IDC_RIGHT )->ShowWindow( TRUE );
	GetDlgItem( IDC_UP )->ShowWindow( TRUE );
	GetDlgItem( IDC_DOWN )->ShowWindow( TRUE );
	GetDlgItem( IDC_CW )->ShowWindow( TRUE );
	GetDlgItem( IDC_CCW )->ShowWindow( TRUE );
	GetDlgItem( IDC_EDT_JOG_X )->ShowWindow( TRUE );
	GetDlgItem( IDC_EDT_JOG_Y )->ShowWindow( TRUE );
	GetDlgItem( IDC_EDT_JOG_Z )->ShowWindow( TRUE );
	GetDlgItem( IDC_EDT_JOG_R )->ShowWindow( TRUE );
}

LRESULT CJogMove::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	// Jog move End
	m_fnSetImageButton( true, wParam );
	return 0;
}

LRESULT CJogMove::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	// Jog Move Start
	m_fnSetImageButton( false, wParam );
	return 0;
}

LRESULT CJogMove::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	// Jog Move
	return 0;
}

void CJogMove::m_fnSetEdit( int nIDCtl, double dPosition ) 
{
	CString strData("");
	strData.Format( "%7.3f", dPosition );
	GetDlgItem( nIDCtl )->SetWindowText( strData );
}

UINT JogMoveThread(LPVOID pFuncData)
{
	CJogMove		*pQI = (CJogMove *)pFuncData;
	double dwPositionX = 0L;
	double dwPositionY = 0L;
	double dwPositionZ = 0L;
	double dwPositionR = 0L;

	while ( pQI->m_bJogMoveEnd )
	{
#if AJINTEX_BOARD
		CString strData("");
		if ( pQI->m_hWnd != NULL )
		{
			if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( X_AXIS, dwPositionX ) == RC_NORMAL )
			{
				pQI->m_dX = dwPositionX;
				pQI->PostMessageA( WM_SET_JOG, X_AXIS, 0 );
			}
			if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( Y_AXIS, dwPositionY ) == RC_NORMAL )
			{
				pQI->m_dY = dwPositionY;
				pQI->PostMessageA( WM_SET_JOG, Y_AXIS, 0 );
			}
			if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos(Z_AXIS, dwPositionZ ) == RC_NORMAL )
			{
				pQI->m_dZ = dwPositionZ;
				pQI->PostMessageA( WM_SET_JOG, Z_AXIS, 0 );
			}
// 			if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos(R_AXIS, dwPositionR ) == RC_NORMAL )
// 			{
// 				pQI->m_dR = dwPositionR;
// 				pQI->PostMessageA( WM_SET_JOG, R_AXIS, 0 );
// 			}

			if ( theApp.m_fnGetViewPointer()->m_pTask->stEqp.nJoyStickArrowXY != pQI->m_nXY )
			{
				pQI->m_nXY = theApp.m_fnGetViewPointer()->m_pTask->stEqp.nJoyStickArrowXY;
				pQI->PostMessageA( WM_SET_JOYSTICK_XY, theApp.m_fnGetViewPointer()->m_pTask->stEqp.nJoyStickArrowXY, 0 );
			}
			if ( theApp.m_fnGetViewPointer()->m_pTask->stEqp.nJoyStickArrowZ != pQI->m_nZ )
			{
				pQI->m_nZ = theApp.m_fnGetViewPointer()->m_pTask->stEqp.nJoyStickArrowZ;
				pQI->PostMessageA( WM_SET_JOYSTICK_Z, theApp.m_fnGetViewPointer()->m_pTask->stEqp.nJoyStickArrowZ, 0 );
			}
		}

#endif
		Sleep( JOG_MOVE );
	}

	if ( pQI->m_hWnd == NULL ) return 0;

	if ( pQI->m_hJogMoveThread != NULL )
	{
		SuspendThread(pQI->m_hJogMoveThread);
		CloseHandle(pQI->m_hJogMoveThread);
		pQI->m_hJogMoveThread	= NULL;
	}
	return 0;
}



void CJogMove::OnBnClickedBtnUp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CJogMove::OnBnClickedButtonIncZMove()
{
	UpdateData(TRUE);
	double dAxisZMove = m_dEditIncZ;
	pAxis[Z_AXIS]->MoveAxis(dAxisZMove, 1, TRUE);
}
