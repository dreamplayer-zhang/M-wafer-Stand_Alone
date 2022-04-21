// ManualDLG.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EllipsoMeter.h"
#include "EllipsoMeterDlg.h"
#include "ManualDLG.h"
#include "afxdialogex.h"
#include "MilVisionDlg.h"
#include "WorkRun.h"

// CManualDLG 대화 상자입니다.

IMPLEMENT_DYNAMIC(CManualDLG, CDialogEx)

CManualDLG::CManualDLG(CWnd* pParent /*=NULL*/)
	: CDialogEx(CManualDLG::IDD, pParent)
{
	this->Create( IDD_MANUAL_DLG );
	m_bShow = FALSE;

	m_pElipso   = NULL;  
	m_pIO = NULL;  
	m_pElipsoR  = NULL;  

	m_pElipso = new CElipsoMeterIF();
	m_pElipso->m_fnSetInit();
	m_pElipso->ShowWindow(SW_HIDE);

	m_pElipsoR = new CElipsoMeterRobotIF();
	m_pElipsoR->m_fnSetInit();
	m_pElipsoR->ShowWindow(SW_HIDE);
}

CManualDLG::~CManualDLG()
{
}

void CManualDLG::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_AUTO_MODE, m_ctrlCheckAutoMode);
	DDX_Control(pDX, IDC_CHECK_Z_ZERO_MOVE, m_ctrlCheckZzeroMove);
}


BEGIN_MESSAGE_MAP(CManualDLG, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_CAMERA_CTRL, &CManualDLG::OnBnClickedBtnCameraCtrl)
	ON_BN_CLICKED(IDC_BTN_AUTO_DEL, &CManualDLG::OnBnClickedBtnAutoDel)
	ON_BN_CLICKED(IDC_BTN_ALIGN_CTRL, &CManualDLG::OnBnClickedBtnAlignCtrl)
	ON_BN_CLICKED(IDC_BTN_ELIPSOMETER, &CManualDLG::OnBnClickedBtnElipsoMeter)
	ON_BN_CLICKED(IDC_BTN_ELIPSOMETER_IO, &CManualDLG::OnBnClickedBtnElipsometerIo)
	ON_BN_CLICKED(IDC_BTN_USER_MANAGE, &CManualDLG::OnBnClickedBtnUserManage)
	ON_BN_CLICKED(IDC_BTN_ELIPSOMETER_ROBOT, &CManualDLG::OnBnClickedBtnElipsometerRobot)
	ON_BN_CLICKED(IDC_BUTTON2, &CManualDLG::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON11, &CManualDLG::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON12, &CManualDLG::OnBnClickedButton12)
	ON_BN_CLICKED(IDC_BUTTON13, &CManualDLG::OnBnClickedButton13)
	ON_BN_CLICKED(IDC_CHECK_AUTO_MODE, &CManualDLG::OnBnClickedCheckAutoMode)
	ON_BN_CLICKED(IDC_BUTTON17, &CManualDLG::OnBnClickedButton17)
	ON_BN_CLICKED(IDC_BUTTON18, &CManualDLG::OnBnClickedButton18)
	ON_BN_CLICKED(IDC_BUTTON16, &CManualDLG::OnBnClickedButton16)
	ON_BN_CLICKED(IDC_BUTTON14, &CManualDLG::OnBnClickedButton14)
	ON_BN_CLICKED(IDC_BUTTON15, &CManualDLG::OnBnClickedButton15)
	ON_BN_CLICKED(IDC_BUTTON4, &CManualDLG::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON21, &CManualDLG::OnBnClickedButton21)
	ON_BN_CLICKED(IDC_BUTTON19, &CManualDLG::OnBnClickedButton19)
	ON_BN_CLICKED(IDC_BUTTON20, &CManualDLG::OnBnClickedButton20)
	ON_BN_CLICKED(IDC_CHECK_Z_ZERO_MOVE, &CManualDLG::OnBnClickedCheckZZeroMove)
END_MESSAGE_MAP()

BOOL CManualDLG::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (( nID   == 2 ) &&
		( nCode == -1 ))
	{
		m_fnSetShowFlag( FALSE );
	}
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CManualDLG::m_fnSetShowFlag( BOOL bShow )
{
	m_bShow = bShow;
	if ( m_bShow )
	{
#if MIL_USE
		m_ctrlCheckAutoMode.SetCheck(pWorkRun->m_bAutoMode);
#endif
		this->ShowWindow( SW_SHOW );
	}
	else
	{
		this->ShowWindow( SW_HIDE );
	}
}

BOOL CManualDLG::PreTranslateMessage(MSG* pMsg) 
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
void CManualDLG::m_fnSetInit() 
{
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

	this->MoveWindow( 100, 100, 1750, 860, TRUE );
}

//---------------------------------------------------------------------------------------------------------
// 종료에 필요한 작업을 한다.
//---------------------------------------------------------------------------------------------------------
void CManualDLG::m_fnSetEnd( ) 
{
	if ( m_pElipso != NULL )
	{
		m_pElipso->m_fnSetEnd();
		delete m_pElipso;
		m_pElipso = NULL;
	}

	if ( m_pIO != NULL )
	{
		m_pIO->m_fnSetEnd();
		delete m_pIO;
		m_pIO = NULL;
	}

	if ( m_pElipsoR != NULL )
	{
		m_pElipsoR->m_fnSetEnd();
		delete m_pElipsoR;
		m_pElipsoR = NULL;
	}
}

void CManualDLG::OnBnClickedBtnCameraCtrl()
{
	theApp.m_fnGetViewPointer()->m_fnSetViewCamera();
}


void CManualDLG::OnBnClickedBtnAutoDel()
{
	theApp.m_fnGetViewPointer()->m_fnSetViewAutoDel();
}

void CManualDLG::OnBnClickedBtnAlignCtrl()
{
#if MIL_USE
	if(pMILAlignDlg->IsWindowVisible())
		pMILAlignDlg->ShowWindow(SW_HIDE);
	else
		pMILAlignDlg->ShowWindow(SW_SHOW);
#endif
}


void CManualDLG::OnBnClickedBtnElipsoMeter()
{
	if ( m_pElipso == NULL )
	{
		m_pElipso = new CElipsoMeterIF();
		m_pElipso->m_fnSetInit();
	}

	m_pElipso->m_fnSetShowFlag( !m_pElipso->m_bShow );
}



void CManualDLG::OnBnClickedBtnElipsometerIo()
{
	if ( m_pIO == NULL )
	{
		m_pIO = new CElipsoMeter_IO();
		m_pIO->m_fnSetInit();
	}

	m_pIO->m_fnSetShowFlag( !m_pIO->m_bShow );
}


void CManualDLG::OnBnClickedBtnUserManage()
{
	theApp.m_fnGetViewPointer()->m_pUserInfo->m_fnSetShowFlag( TRUE );
}


void CManualDLG::OnBnClickedBtnElipsometerRobot()
{
	if ( m_pElipsoR == NULL )
	{
		m_pElipsoR = new CElipsoMeterRobotIF();
		m_pElipsoR->m_fnSetInit();
	}

	m_pElipsoR->m_fnSetShowFlag( !m_pElipsoR->m_bShow );
}


void CManualDLG::OnBnClickedCheckAutoMode()
{
#if MIL_USE
	pWorkRun->m_bAutoMode = m_ctrlCheckAutoMode.GetCheck();
#endif
}


void CManualDLG::OnBnClickedButton2()
{
#if MIL_USE
	//투입위치이동 - 자동/수동
	pWorkRun->SqncMoveSupplyPosition();
#endif
}


void CManualDLG::OnBnClickedButton11()
{
#if MIL_USE
	//투입대기
	pWorkRun->SqncStandbySupply();
#endif
}


void CManualDLG::OnBnClickedButton12()
{
#if MIL_USE
	//시작신호 - 자동/수동
	pWorkRun->SqncInputStartSignal();
#endif
}


void CManualDLG::OnBnClickedButton13()
{
#if MIL_USE
	//웨이퍼 감지
	pWorkRun->SqncDetectWafer();
#endif
}


void CManualDLG::OnBnClickedButton17()
{
#if MIL_USE
	//얼라인 논패턴
	thread thrdTemp = thread(ThrdWaferAlignTest);
	thrdTemp.detach();
#endif
}


void CManualDLG::OnBnClickedButton18()
{
#if MIL_USE
	// 얼라인 패턴
	pWorkRun->m_bThrdFlag[TF_AUTORUN] = TRUE;
	pWorkRun->SqncAlignPatt();
	pWorkRun->m_bThrdFlag[TF_AUTORUN] = FALSE;

#endif
}


void CManualDLG::OnBnClickedButton16()
{
#if MIL_USE
	//자동 이동-측정
	pWorkRun->SqncAutoMoveMeasure();
#endif
}


void CManualDLG::OnBnClickedButton14()
{
#if MIL_USE
	CDPoint dPntTempPos;
	dPntTempPos.x = 0;
	dPntTempPos.y = 0;
	pWorkRun->SqncTargetMove(dPntTempPos);
#endif
}


void CManualDLG::OnBnClickedButton15()
{
#if MIL_USE
	t_ELIPSO_RESULT_INFO stcResultAlign;
	pWorkRun->SqncMeasure(stcResultAlign);
#endif
}


void CManualDLG::OnBnClickedButton4()
{
#if MIL_USE
	thread thrdTemp = thread(ThrdPattTest, 0);
	thrdTemp.detach();
#endif
}


void CManualDLG::OnBnClickedButton21()
{
#if MIL_USE
	thread thrdTemp = thread(ThrdPattTest, 1);
	thrdTemp.detach();
#endif
}


void CManualDLG::OnBnClickedButton19()
{
#if MIL_USE
	thread thrdTemp = thread(ThrdPattTest, 2);
	thrdTemp.detach();
#endif
}


void CManualDLG::OnBnClickedButton20()
{
#if MIL_USE
	thread thrdTemp = thread(ThrdPattTest, 3);
	thrdTemp.detach();
#endif
}


void CManualDLG::OnBnClickedCheckZZeroMove()
{
#if MIL_USE
	pWorkRun->m_bZzeroMove = m_ctrlCheckZzeroMove.GetCheck();
#endif
}
