// StageMapViewDLG.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EllipsoMeter.h"
#include "StageMapViewDLG.h"
#include "afxdialogex.h"

#include "CamJAI.h"
#include "CommThread.h"
#include "ModelData.h"
#include "MilVision.h"
#include "MilVisionDlg.h"

// CStageMapViewDLG 대화 상자입니다.
#define TIMER_SET_CLICK_MOVE 100
#define TIMER_GET_IMAGE 200

IMPLEMENT_DYNAMIC(CStageMapViewDLG, CDialogEx)

CStageMapViewDLG::CStageMapViewDLG(CWnd* pParent /*=NULL*/)
	: CDialogEx(CStageMapViewDLG::IDD, pParent)
	, m_dX(0)
	, m_dY(0)
{
	this->Create( IDD_STAGE_MAP_DLG );
	m_bShow				= FALSE;
	m_hStageMapThread	= NULL;
	m_bStageMapEnd		= FALSE;	
	m_nMoveStep			= STEP_HALT;
	m_bMoveStop			= FALSE;
	m_dX                = 0.0L;
	m_dY                = 0.0L;
	m_bMouseMove        = FALSE;
	m_bCamera           = FALSE;
	m_hDispThread		= NULL;
	m_bDispEnd			= FALSE;
	m_bPreView          = FALSE;
	m_nWaperSize        = WAPER_4;
	m_bTimerClickMove	= FALSE;
	m_dExclusion        = 0.0;

	m_iPntAlignCnt = 0;
}

CStageMapViewDLG::~CStageMapViewDLG()
{
}

void CStageMapViewDLG::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDT_MOVE_X, m_dX);
	DDX_Text(pDX, IDC_EDT_MOVE_Y, m_dY);
	DDX_Control(pDX, IDC_CHK_CAMERA, m_ctrlCheckCamera);
}


BEGIN_MESSAGE_MAP(CStageMapViewDLG, CDialogEx)
	ON_MESSAGE(UM_STAGE_MAP_DRAW, m_fnSetMapDraw)
	ON_MESSAGE(UM_STAGE_MAP_MOVE, m_fnSetMapMove)
	ON_BN_CLICKED(IDC_BTN_MOVE, &CStageMapViewDLG::OnBnClickedBtnMove)
	ON_BN_CLICKED(IDC_BTN_MOVE_STOP, &CStageMapViewDLG::OnBnClickedBtnMoveStop)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_CHK_CAMERA, &CStageMapViewDLG::OnBnClickedChkCamera)
	ON_BN_CLICKED(IDC_CHK_ALIGN_LIGHT_ON, &CStageMapViewDLG::OnBnClickedChkAlignLightOn)
	ON_WM_TIMER()
END_MESSAGE_MAP()

//---------------------------------------------------------------------------------------------
// 외부 참조용 Function
//---------------------------------------------------------------------------------------------
BOOL CStageMapViewDLG::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (( nID   == 2 ) &&
		( nCode == -1 ))
	{
		m_fnSetShowFlag( FALSE );
	}
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CStageMapViewDLG::m_fnSetShowFlag( BOOL bShow )
{
	m_bShow = bShow;
	if ( m_bShow )
	{
		this->ShowWindow( SW_SHOW );
		SetTimer(TIMER_SET_CLICK_MOVE, 300, NULL);
	}
	else
	{
		KillTimer(TIMER_SET_CLICK_MOVE);
		if(m_ctrlCheckCamera.GetCheck())
		{
			((CButton*)GetDlgItem(IDC_CHK_CAMERA))->SetCheck(FALSE);
			OnBnClickedChkCamera();
		}
		this->ShowWindow( SW_HIDE );
	}
}

BOOL CStageMapViewDLG::PreTranslateMessage(MSG* pMsg) 
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
void CStageMapViewDLG::m_fnSetInit( ) 
{
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

	CString s("");
	s.Format( "Stage Map View 메인창" );
	this->SetWindowText(s);
	
	CRect rect;
	this->MoveWindow( 1200, 100, 700,700, TRUE );

	GetDlgItem( IDC_STAGE_MAP )->MoveWindow( 50, 60, 600,600, TRUE );

	m_pTaskHandle = new CTaskHandle( this, SHARED_NAME, 
									 1, 
									 sizeof( SHEET_RESISTANCE_Format ) + 1,
									 0,
									 FALSE );


	m_pTask					= ( SHEET_RESISTANCE_Format* )m_pTaskHandle->m_pFilePointer( );

	GetDlgItem( IDC_BTN_MOVE )->EnableWindow( TRUE );
	GetDlgItem( IDC_BTN_MOVE_STOP )->EnableWindow( FALSE );
	CString strData("");
	strData.Format( "%7.3f", 0.0 );
	GetDlgItem( IDC_EDT_MOUSE_X )->SetWindowText( strData );
	strData.Format( "%7.3f", 0.0 );
	GetDlgItem( IDC_EDT_MOUSE_Y )->SetWindowText( strData );
	strData.Format( "%01d", 0 );
	GetDlgItem( IDC_EDT_POINT_X )->SetWindowText( strData );
	strData.Format( "%01d", 0 );
	GetDlgItem( IDC_EDT_POINT_Y )->SetWindowText( strData );
	m_bPreView = TRUE;

	unsigned long	ThreadID;
	m_bStageMapEnd    = TRUE;
	m_hStageMapThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StageMapThread, this, NULL, &ThreadID);

	unsigned long	ThreadID1;
	m_bDispEnd    = TRUE;
	m_hDispThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DispThread, this, NULL, &ThreadID1);
}

//---------------------------------------------------------------------------------------------------------
// 종료에 필요한 작업을 한다.
//---------------------------------------------------------------------------------------------------------
void CStageMapViewDLG::m_fnSetEnd() 
{
	if ( m_hDispThread != NULL )
	{
		m_bDispEnd			= FALSE;
		SuspendThread( m_hDispThread );
		CloseHandle(m_hDispThread);
		m_hDispThread	    = NULL;
	}

	if ( m_hStageMapThread != NULL )
	{
		m_bStageMapEnd			= FALSE;
		SuspendThread( m_hStageMapThread );
		CloseHandle(m_hStageMapThread);
		m_hStageMapThread	    = NULL;
	}

	if ( m_pTaskHandle != NULL )
	{
		m_pTask->stElipsoIO.bConnect = FALSE;
		m_pTaskHandle->m_fnSetEnd();
		delete m_pTaskHandle;
		m_pTaskHandle = NULL;
	}

}

UINT DispThread(LPVOID pFuncData)
{
	CStageMapViewDLG		*pQI = (CStageMapViewDLG *)pFuncData;
	while ( pQI->m_bDispEnd )
	{
		Sleep( STAGE_MAP_TIME );
// 		if ( !pQI->m_bCamera ) 
// 		{
// 			continue;
// 		}

		if ( pQI->m_hWnd != NULL )
		{
			pQI->PostMessage( UM_STAGE_MAP_DRAW, 0,0 );
		}
	}

	if ( pQI->m_hWnd == NULL ) return 0;

	if ( pQI->m_hDispThread != NULL )
	{
		SuspendThread(pQI->m_hDispThread);
		CloseHandle(pQI->m_hDispThread);
		pQI->m_hDispThread	= NULL;
	}
	return 0;
}
UINT StageMapThread(LPVOID pFuncData)
{
	CStageMapViewDLG		*pQI = (CStageMapViewDLG *)pFuncData;
	DWORD  dwBusyStatus = 0;
	DWORD  dwBusyStatus1 = 0;
	DWORD  dwBusyStatus2 = 0;
	double dSpeed        = 0;
	DWORD dwRtnCode      = 0;
	DWORD dwRtnCode1     = 0;
	DWORD dwRtnCode2     = 0;
	double dPositionZ    = 0.0;

	while ( pQI->m_bStageMapEnd )
	{
		if ( theApp.m_fnGetViewPointer()->m_pTask->stEqp.bEqpEmergencySwitch)
		{
			pQI->m_nMoveStep    = STEP_HALT;
			pQI->m_bStageMapEnd = FALSE;
			theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmMoveAllStop();
			continue;
		}

		if ( pQI->m_hWnd != NULL )
		{
			if ( pQI->m_ctrlCheckCamera.GetCheck())
			{
				Sleep( STAGE_MAP_TIME );
				continue;
			}
			pQI->PostMessage( UM_STAGE_MAP_DRAW, 0,0 );
#if AJINTEX_BOARD
			switch ( pQI->m_nMoveStep )
			{
				case 100:
					{
						dwBusyStatus	= 0;
						double dwSpeed  = theApp.m_fnGetViewPointer( )->m_pTask->stCommon.dSpeedHighZ;
						DWORD dwRtnCode = theApp.m_fnGetViewPointer()->m_pAjin->m_fnGetAxmStatusReadInMotion( Z_AXIS, dwBusyStatus );
						if ( dwBusyStatus == 0 )
						{
							dPositionZ = theApp.m_fnGetViewPointer()->m_pModel->m_fnGetMeasureSafetyZ( pQI->m_pTask->stModel.dWaperThickness );
							dwRtnCode = theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMotSetAbsRelMode( Z_AXIS, POS_ABS_MODE );
							theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveStartPos( Z_AXIS, dPositionZ, dwSpeed, dwSpeed * 2, dwSpeed * 2 );
							if ( !pQI->m_bMoveStop )
							{
								pQI->m_nMoveStep = 200;
							}
						}
					}
					break;
				case 200: 
					{
						DWORD dwRtnCode = theApp.m_fnGetViewPointer()->m_pAjin->m_fnGetAxmStatusReadInMotion( X_AXIS, dwBusyStatus );
						if ( dwBusyStatus == 0 )
						{
							pQI->m_nMoveStep = 300;
						}
					}
					break;
				case 300:
					{
						dwBusyStatus1	= 0;
						dwBusyStatus2	= 0;
						double dwSpeed  = theApp.m_fnGetViewPointer( )->m_pTask->stCommon.dSpeedHigh;
						DWORD dwRtnCode1 = theApp.m_fnGetViewPointer()->m_pAjin->m_fnGetAxmStatusReadInMotion( X_AXIS, dwBusyStatus1 );
						DWORD dwRtnCode2 = theApp.m_fnGetViewPointer()->m_pAjin->m_fnGetAxmStatusReadInMotion( Y_AXIS, dwBusyStatus2 );
						if (( dwBusyStatus1 == 0 ) &&
							( dwBusyStatus2 == 0 ))
						{
							dwRtnCode1 = theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMotSetAbsRelMode( X_AXIS, POS_ABS_MODE );
							dwRtnCode2 = theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMotSetAbsRelMode( Y_AXIS, POS_ABS_MODE );
							theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveStartPos( X_AXIS, pQI->m_dX, dwSpeed, dwSpeed * 2, dwSpeed * 2 );
							theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveStartPos( Y_AXIS, pQI->m_dY, dwSpeed, dwSpeed * 2, dwSpeed * 2 );
							if ( !pQI->m_bMoveStop )
							{
								pQI->m_nMoveStep = 400;
							}
						}
					}
					break;
				case 400: 
					{
						DWORD dwRtnCode1 = theApp.m_fnGetViewPointer()->m_pAjin->m_fnGetAxmStatusReadInMotion( X_AXIS, dwBusyStatus1 );
						DWORD dwRtnCode2 = theApp.m_fnGetViewPointer()->m_pAjin->m_fnGetAxmStatusReadInMotion( Y_AXIS, dwBusyStatus2 );
						if (( dwBusyStatus1 == 0 ) &&
							( dwBusyStatus2 == 0 ))
						{
							pQI->PostMessage( UM_STAGE_MAP_MOVE, pQI->m_bMoveStop,0 );
							pQI->m_nMoveStep = STEP_HALT;
						}
					}
					break;
			}
#endif
			Sleep( STAGE_MAP_TIME );
		}
	}

	if ( pQI->m_hWnd == NULL ) return 0;

	if ( pQI->m_hStageMapThread != NULL )
	{
		SuspendThread(pQI->m_hStageMapThread);
		CloseHandle(pQI->m_hStageMapThread);
		pQI->m_hStageMapThread	= NULL;
	}
	return 0;
}

//---------------------------------------------------------------------------------------------------------
// 맵 그리기
//---------------------------------------------------------------------------------------------------------
LRESULT CStageMapViewDLG::m_fnSetMapMove(  WPARAM nNo, LPARAM lParam )
{
	if (m_ctrlCheckCamera.GetCheck()) return 0;

	CString strMsg("");
	if ( nNo == 0 )
	{
		strMsg = "정상적으로 이동완료했습니다.";
		theApp.m_fnGetViewPointer( )->m_fnLogWrite( strMsg );
	}
	else
	{
		strMsg = "이동중지했습니다.";
		theApp.m_fnGetViewPointer( )->m_fnLogWrite( strMsg );
	}
	GetDlgItem( IDC_BTN_MOVE )->EnableWindow( TRUE );
	GetDlgItem( IDC_BTN_MOVE_STOP )->EnableWindow( FALSE );
	m_bMouseMove = FALSE;
	return 0;
}
//---------------------------------------------------------------------------------------------------------
// 맵 그리기
//---------------------------------------------------------------------------------------------------------
LRESULT CStageMapViewDLG::m_fnSetMapDraw(  WPARAM nNo, LPARAM lParam )
{
	if (m_ctrlCheckCamera.GetCheck())
		return 0;

	CvvImage							cImage;
	IplImage*							pImage = NULL;
	double dPositionX = 0L;
	double dPositionY = 0L;
	double dPositionZ = 0L;

	CRect rect;
	CDC* pStaticDC = GetDlgItem(IDC_STAGE_MAP)->GetDC();
	GetDlgItem(IDC_STAGE_MAP)->GetClientRect(&rect);
	double dPixelPerInch = (double)rect.Width() / (double)13;

	int iDrawCircleSize = 0;
	CvScalar cvCircleColor;
	int iThickless = 0;

	if (!m_ctrlCheckCamera.GetCheck())
	{
		pImage = cvCreateImage(cvSize(rect.right, rect.bottom), IPL_DEPTH_8U, 3);
		cvSet(pImage, cvScalar(CV_RGB(50, 50, 50)));

		for (int i = MAX_WAPER_CNT - 1; i >= 0; i--)
		{
			switch (i)
			{
			case WAPER_12:
				iDrawCircleSize = (int)((12.0* dPixelPerInch) / 2.0);
				break;
			case WAPER_8:
				iDrawCircleSize = (int)((8.0 * dPixelPerInch) / 2.0);
				break;
			case WAPER_6:
				iDrawCircleSize = (int)((6.0 * dPixelPerInch) / 2.0);
				break;
			case WAPER_4:
				iDrawCircleSize = (int)((4.0 * dPixelPerInch) / 2.0);
				break;
			}

			cvCircleColor = cvScalar(CV_RGB(0, 0, 0));
			iThickless = 1;
			if (m_nWaperSize == i)
			{
				cvCircleColor = cvScalar(CV_RGB(220, 220, 220));
				iThickless = -1;
			}

			cvCircle(pImage, cvPoint((rect.bottom / 2), (rect.right / 2)), iDrawCircleSize, cvCircleColor, iThickless);

			if (m_nWaperSize == i)
			{
				if (m_dExclusion > 0.0)
				{
					iDrawCircleSize = (int)((double)iDrawCircleSize - m_dExclusion*2.0);
					cvCircleColor = cvScalar(CV_RGB(255, 0, 255));
					iThickless = 1;
					cvCircle(pImage, cvPoint((rect.bottom / 2), (rect.right / 2)), iDrawCircleSize, cvCircleColor, iThickless);
				}
			}

		}

		//Z축 표시
		cvRectangle(pImage, cvPoint(15, (rect.bottom / 2) + 40), cvPoint(19, (rect.bottom) - 10), cvScalar(CV_RGB(0, 0, 0)), 2);

		cvLine(pImage, cvPoint(0, (rect.bottom / 2)), cvPoint(rect.Height(), (rect.bottom / 2)), cvScalar(CV_RGB(0, 0, 0)), 1);
		cvLine(pImage, cvPoint((rect.right / 2), 0), cvPoint((rect.bottom / 2), rect.Width()), cvScalar(CV_RGB(0, 0, 0)), 1);

		CString strWaferSize;
		strWaferSize.Format(_T("%.0f"), 13.0 * 25.4 / 2.0);

		m_Util.setPutText(pImage, pImage, (LPSTR)strWaferSize.GetString(), cvPoint((rect.right / 2), (rect.bottom - 15)), cvScalar(CV_RGB(130, 130, 130)));
		m_Util.setPutText(pImage, pImage, (LPSTR)("-" + strWaferSize).GetString(), cvPoint(1, (rect.bottom / 2)), cvScalar(CV_RGB(130, 130, 130)));
		m_Util.setPutText(pImage, pImage, (LPSTR)("-" + strWaferSize).GetString(), cvPoint((rect.right / 2 + 2), 1), cvScalar(CV_RGB(130, 130, 130)));
		m_Util.setPutText(pImage, pImage, (LPSTR)strWaferSize.GetString(), cvPoint(rect.right - 27, (rect.bottom / 2)), cvScalar(CV_RGB(130, 130, 130)));

		if (theApp.m_fnGetViewPointer()->m_nPage == SW_MODEL)
		{
			if (m_bPreView)
			{
				int nX = 0;
				int nY = 0;
				CPoint pointXY;
				for (int i = 0; i < m_pTask->stModel.nSRData; i++)
				{
					m_fnSetMouseCoord(pointXY, m_pTask->stModel.stSRData[i].dwXPosition, m_pTask->stModel.stSRData[i].dwYPosition, dPixelPerInch, rect.Width() / 2, rect.Height() / 2);
					// 							m_fnSetMouseCoord(pointXY, m_pTask->stModel.stSRData[i].dwXPosition, m_pTask->stModel.stSRData[i].dwYPosition);
					cvLine(pImage, cvPoint(pointXY.x - 3, pointXY.y), cvPoint(pointXY.x + 3, pointXY.y), cvScalar(CV_RGB(0, 0, 128)), 2);
					cvLine(pImage, cvPoint(pointXY.x, pointXY.y - 3), cvPoint(pointXY.x, pointXY.y + 3), cvScalar(CV_RGB(0, 0, 128)), 2);
				}

				for (int i=0; i < m_iPntAlignCnt; i++)
				{
					m_fnSetMouseCoord(pointXY, m_dPntAlign[i].x, m_dPntAlign[i].y, dPixelPerInch, rect.Width() / 2, rect.Height() / 2);
					cvLine(pImage, cvPoint(pointXY.x - 3, pointXY.y), cvPoint(pointXY.x + 3, pointXY.y), cvScalar(CV_RGB(255, 100, 100)), 2);
					cvLine(pImage, cvPoint(pointXY.x, pointXY.y - 3), cvPoint(pointXY.x, pointXY.y + 3), cvScalar(CV_RGB(255, 100, 100)), 2);
				}
			}
		}

		//임시 패턴 좌표 그리기 추가장소
		//t_DATA 임시선언 or CDpoint 에 넣어서 뿌림
		//스텝 이후 포인트 갯수 =0 








#if AJINTEX_BOARD
		CString strData("");
		if (this->m_hWnd != NULL)
		{
			if (theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos(X_AXIS, dPositionX) == RC_NORMAL)
			{
				strData.Format("%7.3f", dPositionX);
				GetDlgItem(IDC_EDT_CURR_X)->SetWindowText(strData);
			}
			if (theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos(Y_AXIS, dPositionY) == RC_NORMAL)
			{
				strData.Format("%7.3f", dPositionY);
				GetDlgItem(IDC_EDT_CURR_Y)->SetWindowText(strData);
			}
			theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos(Z_AXIS, dPositionZ);
		}
#endif
		CPoint point;
		m_fnSetMouseCoord(point, dPositionX, dPositionY, dPixelPerInch, rect.Width() / 2, rect.Height() / 2);
		CPoint InRectPnt;
		InRectPnt.x = point.x < 0 ? 0 : point.x > rect.right ? rect.right : point.x;
		InRectPnt.y = point.y < 0 ? 0 : point.y > rect.bottom ? rect.bottom : point.y;
		cvLine(pImage, cvPoint(InRectPnt.x - 3, InRectPnt.y), cvPoint(InRectPnt.x + 3, InRectPnt.y), cvScalar(CV_RGB(0, 255, 0)), 2);
		cvLine(pImage, cvPoint(InRectPnt.x, InRectPnt.y - 3), cvPoint(InRectPnt.x, InRectPnt.y + 3), cvScalar(CV_RGB(0, 255, 0)), 2);

		//Z축표시
		point.x = 17;
		point.y = (LONG)((double)(((rect.bottom) - 10) - ((rect.bottom / 2) + 40)) / (theApp.m_fnGetViewPointer()->m_pTask->stCommon.dZLimit) * dPositionZ);
		point.y = point.y < 0 ? 0 : point.y >(((rect.bottom) - 10) - ((rect.bottom / 2) + 40)) ? (((rect.bottom) - 10) - ((rect.bottom / 2) + 40)) : point.y;
		point.y += ((rect.bottom / 2) + 40);
		cvLine(pImage, cvPoint(point.x - 3, point.y), cvPoint(point.x + 3, point.y), cvScalar(CV_RGB(0, 255, 0)), 2);
		cvLine(pImage, cvPoint(point.x, point.y - 3), cvPoint(point.x, point.y + 3), cvScalar(CV_RGB(0, 255, 0)), 2);

	}
	else
	{
	}
	if (pImage != NULL)
	{
		cImage.CopyOf(pImage, 1);
		if (pStaticDC != NULL)
		{
			cImage.DrawToHDC(pStaticDC->m_hDC, &rect);
			ReleaseDC(pStaticDC);
		}
		cvReleaseImage(&pImage);
	}
	pImage = NULL;
	return 0;
}

void CStageMapViewDLG::OnBnClickedBtnMove()
{
	if (m_ctrlCheckCamera.GetCheck())
	{
		((CButton*)theApp.m_fnGetViewPointer()->m_pStageMap->GetDlgItem(IDC_CHK_CAMERA))->SetCheck(FALSE);
		theApp.m_fnGetViewPointer()->m_pStageMap->OnBnClickedChkCamera();
	}


#if AJINTEX_BOARD
	double dwPosition = 0.0;
	if ( theApp.m_fnGetViewPointer()->m_pModel->m_fnGetMeasureSafetyZ( theApp.m_fnGetViewPointer()->m_pTask->stModel.dWaperThickness ) > theApp.m_fnGetViewPointer()->m_pTask->stCommon.dZLimit )
	{
			TECmsgOut( L"Z축 Limit 위치보다 Z측정 안정위치가 낮아서 Stage와 충돌할수 있어서 이동불가 합니다.", RED_1, 1 );
			return;
	}
#endif
	CString strMsg("");
	CString strDataX("");
	CString strDataY("");
	if  ( theApp.m_fnGetViewPointer( )->m_pTask->stEqp.bIoStartButton )
	{
		strMsg = "검사중에는 Stage를 수동으로 이동할수 없습니다.";
		theApp.m_fnGetViewPointer( )->m_fnLogWrite( strMsg );
		return;
	}

	if  ( !theApp.m_fnGetViewPointer( )->m_pTask->stEqp.bEqpInit )
	{
		strMsg = "장비초기화후에 진행해 주세요.";
		theApp.m_fnGetViewPointer( )->m_fnLogWrite( strMsg );
		return;
	}

	if ( m_nMoveStep != STEP_HALT )
	{
		theApp.m_fnGetViewPointer()->m_fnLogWrite("Stage가 이동중입니다.");
		return;
	}
	if ( m_bMouseMove )
	{
		theApp.m_fnGetViewPointer()->m_fnLogWrite("Stage가 이동중입니다.");
		return;
	}

	GetDlgItem( IDC_EDT_MOVE_X )->GetWindowText( strDataX );
	GetDlgItem( IDC_EDT_MOVE_Y )->GetWindowText( strDataY );
	strDataX.Trim();
	strDataY.Trim();
	m_dX = atof( strDataX );
	m_dY = atof( strDataY );

	m_bMoveStop  = FALSE;
	m_bMouseMove = FALSE;
	m_nMoveStep = 100;
	GetDlgItem( IDC_BTN_MOVE )->EnableWindow( FALSE );
	GetDlgItem( IDC_BTN_MOVE_STOP )->EnableWindow( TRUE );
}


void CStageMapViewDLG::OnBnClickedBtnMoveStop()
{
	if  ( m_nMoveStep == STEP_HALT )
	{
		return;
	}

#if AJINTEX_BOARD
	theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveSStop( X_AXIS );	
	theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetAxmMoveSStop( Y_AXIS );
#endif
	m_bMoveStop = TRUE;
}

void CStageMapViewDLG::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_ctrlCheckCamera.GetCheck()) return;

	CString strData("");
	double dX = 0.0;
	double dY = 0.0;

	CRect rect;
	GetDlgItem(IDC_STAGE_MAP)->GetClientRect(&rect);
	double dPixelPerInch = (double)rect.Width() / (double)13;

	m_fnGetMouseCoord(point, dX, dY, dPixelPerInch, rect.Width() / 2, rect.Height() / 2);
	strData.Format( "%7.3f", dX );
	GetDlgItem( IDC_EDT_MOUSE_X )->SetWindowText( strData );
	strData.Format( "%7.3f", dY );
	GetDlgItem( IDC_EDT_MOUSE_Y )->SetWindowText( strData );
	strData.Format( "%01d", point.x );
	GetDlgItem( IDC_EDT_POINT_X )->SetWindowText( strData );
	strData.Format( "%01d", point.y );
	GetDlgItem( IDC_EDT_POINT_Y )->SetWindowText( strData );

	CDialogEx::OnMouseMove(nFlags, point);
}

void CStageMapViewDLG::m_fnGetMouseCoord(CPoint point, double &dX, double &dY, double dResol, int offsetX/*=0*/, int offsetY/*=0 */)
{
	dX = (double)(point.x - offsetX - 50) * (25.4) / dResol;
	dY = (double)(point.y - offsetY - 60) * (25.4) / dResol;
}

void CStageMapViewDLG::m_fnGetMouseCoord(CPoint point, double &dX, double &dY)
{
	dX = (point.x - 350.) / (2.);
	dY = (370. - point.y) / (-2.);
}

void CStageMapViewDLG::m_fnSetMouseCoord(CPoint &point, double dX, double dY, double dResol, int offsetX/*=0*/, int offsetY/*=0 */)
{
	point.x = (LONG)(dX * dResol / 25.4 + offsetX);
	point.y = (LONG)(dY * dResol / 25.4 + offsetY);
}

void CStageMapViewDLG::m_fnSetMouseCoord(CPoint &point, double dX, double dY)
{
	point.x = 400 + (LONG)(dX * 2.);
	point.y = 400 + (LONG)(dY * 2.);
}

void CStageMapViewDLG::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_ctrlCheckCamera.GetCheck()) return;

	double dX = 0.0;
	double dY = 0.0;
	CString strMsg("");
	if ( theApp.m_fnGetViewPointer()->m_pTask->stEqp.bIoStartButton )
	{
		theApp.m_fnGetViewPointer()->m_fnLogWrite("Start 버튼이 ON(자동검사)시에는 수동 Move는 사용할수 없습니다.");
		return;
	}
	if ( m_nMoveStep != STEP_HALT )
	{
		theApp.m_fnGetViewPointer()->m_fnLogWrite("Stage가 이동중입니다.");
		return;
	}
	if ( m_bMouseMove )
	{
		theApp.m_fnGetViewPointer()->m_fnLogWrite("Stage가 이동중입니다.");
		return;
	}

	if  ( !theApp.m_fnGetViewPointer( )->m_pTask->stEqp.bEqpInit )
	{
		strMsg = "장비초기화후에 진행해 주세요.";
		theApp.m_fnGetViewPointer( )->m_fnLogWrite( strMsg );
		TECmsgOut( m_Util.CStringTobstr(strMsg), GREEN_1, 1 );
		return;
	}

	CRect rect;
	GetDlgItem(IDC_STAGE_MAP)->GetClientRect(&rect);
	double dPixelPerInch = (double)rect.Width() / (double)13;

	m_fnGetMouseCoord(point, dX, dY, dPixelPerInch, rect.Width() / 2, rect.Height() / 2);



	int iWaferSizeArr[4] = { 4,6,8,12 };
// 	if (( dX >= m_pTask->stCommon.dXMLimit ) &&
// 	    ( dX <= m_pTask->stCommon.dXPLimit ) &&
// 	    ( dY >= m_pTask->stCommon.dYMLimit ) &&
// 	    ( dY <= m_pTask->stCommon.dYPLimit ))
	if (sqrt(dX*dX + dY*dY) <= iWaferSizeArr[m_nWaperSize]*25.4*0.5)
	{
		m_dX = dX;
		m_dY = dY;

		m_bMoveStop = FALSE;
		m_nMoveStep = 100;
		GetDlgItem( IDC_BTN_MOVE )->EnableWindow( FALSE );
		GetDlgItem( IDC_BTN_MOVE_STOP )->EnableWindow( TRUE );
		m_bMouseMove = TRUE;
	}
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CStageMapViewDLG::OnBnClickedChkCamera()
{
	return;

#if JAI_CAMERA
	CString strData("");
	BOOL    bConnect = false;
	if (m_ctrlCheckCamera.GetCheck())
	{
		theApp.m_fnGetViewPointer()->m_pJog->m_clJogSpeed.SetCurSel(2);
		theApp.m_fnGetViewPointer()->m_pJog->m_clJogSpeedZ.SetCurSel(2);

		pCamJAI->m_HwndPic = GetDlgItem(IDC_STAGE_MAP)->GetSafeHwnd();
		pCamJAI->m_bUseUI = TRUE;

		m_fnSetRunningCamera(TRUE);
	}
	else
	{
		m_fnSetRunningCamera(FALSE);

		theApp.m_fnGetViewPointer()->m_pJog->m_clJogSpeed.SetCurSel(1);
		theApp.m_fnGetViewPointer()->m_pJog->m_clJogSpeedZ.SetCurSel(1);
	}


#endif
}

void CStageMapViewDLG::m_fnSetRunningCamera(BOOL bRun)
{
#if JAI_CAMERA
	if (bRun)
	{
		LightControl(0, 1, m_pTask->stCommon.nInspLight);

		pCamJAI->SendMessage(WM_COMMAND, IDC_START, 0);
	}
	else
	{
		pCamJAI->SendMessage(WM_COMMAND, IDC_STOP, 0);

		if (!pCamJAI->m_bShow)
			pCamJAI->m_bUseUI = FALSE;

// 		LightControl(0, 1, 0);
	}
#endif
}


void CStageMapViewDLG::OnBnClickedChkAlignLightOn()
{
}


void CStageMapViewDLG::OnTimer(UINT_PTR nIDEvent)
{
#if JAI_CAMERA
	CDPoint pntClickMove;
// 	int iAxisNo;
// 	double dMovePos;
	switch (nIDEvent)
	{
	case TIMER_SET_CLICK_MOVE:
		if (pCamJAI == NULL)
			break;
		if (pCamJAI->m_bClickMoveFlag)
		{
			pntClickMove.x = pCamJAI->m_MouseClickPos.x - (double)((DEF_IMAGE_X) >> 1);
			pntClickMove.y = pCamJAI->m_MouseClickPos.y - (double)((DEF_IMAGE_Y) >> 1);
			pntClickMove.x = pModelData->dImgResol*pntClickMove.x;
			pntClickMove.y = pModelData->dImgResol*pntClickMove.y;

			pAxis[X_AXIS]->fVelocity = (float)theApp.m_fnGetViewPointer()->m_pTask->stCommon.dSpeedMiddleZ;
			pAxis[X_AXIS]->fAccel = (float)pAxis[X_AXIS]->fVelocity*4.f;
			pAxis[X_AXIS]->fDeccel = (float)pAxis[X_AXIS]->fVelocity*4.f;

			pAxis[Y_AXIS]->fVelocity = (float)theApp.m_fnGetViewPointer()->m_pTask->stCommon.dSpeedMiddleZ;
			pAxis[Y_AXIS]->fAccel = (float)pAxis[Y_AXIS]->fVelocity*4.f;
			pAxis[Y_AXIS]->fDeccel = (float)pAxis[Y_AXIS]->fVelocity*4.f;
// 			pAxis[iAxisNo]->m_fSpeedFlag = 1.f;
//			dMovePos = theApp.m_fnGetViewPointer()->m_pModel->m_fnGetAlignZ(-50.0);
	
			pAxis[X_AXIS]->MoveAxis(pntClickMove.x, 1, TRUE);
			pAxis[Y_AXIS]->MoveAxis(pntClickMove.y, 1, TRUE);

			SetTimer(TIMER_GET_IMAGE, 500, 0);
			pCamJAI->m_bClickMoveFlag = FALSE;
		}
		break;
	case TIMER_GET_IMAGE:
		if (pAxis[X_AXIS]->CheckInMotion() || pAxis[Y_AXIS]->CheckInMotion())
			break;

		pMILAlignDlg->m_fnBringImage(pCamJAI->m_CnvImageInfo.pImageBuffer, pCamJAI->m_CnvImageInfo.iImageSize);
		KillTimer(TIMER_GET_IMAGE);
		break;
	}
#endif
	CDialogEx::OnTimer(nIDEvent);
}

void CStageMapViewDLG::m_fnCheckCamera(BOOL bON)
{
#if JAI_CAMERA
	if (bON && pCamJAI->m_bStreamStarted)
		return;

// 	m_ctrlCheckCamera.SetCheck(bON);
	SendMessageA(WM_COMMAND, IDC_CHK_CAMERA, 0);
#endif
}
