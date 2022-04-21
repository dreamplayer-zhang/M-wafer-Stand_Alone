// Init.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "EllipsoMeter.h"
#include "Init.h"
#include "afxdialogex.h"
#include "ATXMotion.h"
#include "WorkRun.h"

#define WM_SET_INIT_END							4000
IMPLEMENT_DYNAMIC(CInit, CDialogEx)

CInit::CInit(CWnd* pParent /*=NULL*/)
	: CDialogEx(CInit::IDD, pParent)
{
	m_bFirst            = FALSE;
	m_hEqInitThread		= NULL;
	m_bEqInitEnd		= FALSE;

	for( int i = 0; i < MAX_AXIS; i++ )
	{
		m_strProcess[i] = "";
	}
	m_bProcess          = FALSE;
}

CInit::~CInit()
{
}

void CInit::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FPSPREAD, m_clInit);
}


BEGIN_MESSAGE_MAP(CInit, CDialogEx)
	ON_MESSAGE(WM_SET_INIT_END, m_fnSetInitEnd ) 
	ON_BN_CLICKED(IDC_BTN_INIT, &CInit::OnBnClickedBtnInit)
	ON_BN_CLICKED(IDC_BTN_INIT_STOP, &CInit::OnBnClickedBtnInitStop)
	ON_BN_CLICKED(IDC_BTN_APP_EXT, &CInit::OnBnClickedBtnAppExt)
	ON_WM_ACTIVATE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

BOOL CInit::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (( nID   == 2 ) &&
		( nCode == -1 ))
	{
		return FALSE;
	}
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CInit::PreTranslateMessage(MSG* pMsg) 
{
	// ENTER KEY or ESC KEY CANCEL
	if (( pMsg->wParam == 13 ) || 
		( pMsg->wParam == 27 ))
	{
		return FALSE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}


void CInit::OnBnClickedBtnInit()
{
	if ( m_bProcess ) return;

	BOOL bError = FALSE;
	CString strMsg("");
// 	DWORD   dResult[MAX_AXIS];

	//EMO����Ȯ���� I/O��...
	if ((m_pTask->stElipsoIO.stIn.EMS_1 == 0 )
		|| (m_pTask->stElipsoIO.stIn.EMS_2 == 0)
		|| (m_pTask->stElipsoIO.stIn.EMS_3 == 0)
		|| (m_pTask->stElipsoIO.stIn.EMS_4 == 0)
		)
	{
		bError = TRUE;
		strMsg = "���� '���� �� Alarm'�� Ȯ���� �Ŀ� �ʱ�ȭ �� �ּ���.";
		theApp.m_fnGetViewPointer( )->m_fnLogWrite( strMsg );
		m_bProcess = TRUE;
		TECmsgOut( m_Util.CStringTobstr(strMsg), RED_1, 1 );
		m_bProcess = FALSE;
	}


	if  ( theApp.m_fnGetViewPointer( )->m_pTask->stEqp.bIoStartButton )
	{
		bError = TRUE;
		strMsg = "Start��ư ��������(����)�� Stop��ư���� ��ȯ�� �ʱ�ȭ�� ������ �ּ���";
		theApp.m_fnGetViewPointer( )->m_fnLogWrite( strMsg );
		m_bProcess = TRUE;
		TECmsgOut( m_Util.CStringTobstr(strMsg), RED_1, 1 );
		m_bProcess = FALSE;
	}

	if ( bError )
	{
		return;
	}

	m_nStep												= 0;

	unsigned long	ThreadID1;
	m_bEqInitEnd	= TRUE;
	m_hEqInitThread	= NULL;
	m_hEqInitThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)EqInitThread, this, NULL, &ThreadID1);

	pAxis[X_AXIS]->m_dOffsetPos = 0.0;
	pAxis[Y_AXIS]->m_dOffsetPos = 0.0;
	pAxis[Z_AXIS]->m_dOffsetPos = 0.0;

	thread thrdReadReceipe = thread(ThrdLampOnCheck, TRUE);
	thrdReadReceipe.detach();

}

void ThrdLampOnCheck(BOOL bOn /*= TRUE*/)
{
#if MIL_USE
	if (pWorkRun->m_bThrdFlag[TF_ELLIPSORUN])
		return;
	pWorkRun->m_bThrdFlag[TF_ELLIPSORUN] = TRUE;

	int tmpStep = 20;
	BOOL bTmpLampOnCheck = pWorkRun->m_bThrdFlag[TF_ELLIPSORUN];
	CString strLampHours = "";

	theApp.m_fnGetViewPointer()->m_bLampOn = FALSE;
	while (bTmpLampOnCheck)
	{
		Sleep(500);
		if(tmpStep < 50)
			TECmsgOut(L"EllipsoMeter Lamp �������Դϴ�. ��� ��ٷ��ּ���.", YELLOW, 500, 17, BLACK);

		switch (tmpStep)
		{
		case 20://lamppower on check
			theApp.m_fnGetViewPointer()->m_pManual->m_pElipso->m_fnGetSystemLampPowerIsOn();
			tmpStep++;
			break;

		case 21:
			if (!theApp.m_fnGetViewPointer()->m_pManual->m_pElipso->m_bComplete)
				break;

			if (theApp.m_fnGetViewPointer()->m_pManual->m_pElipso->m_bError)
			{//����
				TECmsgOut(L"EllipsoMeter�� ���� ���¸� Ȯ���� �ּ���._LampPowerIsOn", LIGHTMAGENTA, 1);
				bTmpLampOnCheck = FALSE;
				break;
			}
			else
			{
				if (!theApp.m_fnGetViewPointer()->m_pManual->m_pElipso->m_bYesNo)
				{//����
					TECmsgOut(L"Lamp Power�� Ȯ���� �ּ���.", LIGHTMAGENTA, 1);
					bTmpLampOnCheck = FALSE;
					break;
				}
			}
			tmpStep = 30;
			break;

		case 30: //lamp on/off
			if (bOn)
			{
				theApp.m_fnGetViewPointer()->m_pManual->m_pElipso->m_fnSetSystemLampOn();
			} 
			else
			{
				theApp.m_fnGetViewPointer()->m_pManual->m_pElipso->m_fnSetSystemLampOff();
			}
			tmpStep++;
			break;

		case 31:
			if (!theApp.m_fnGetViewPointer()->m_pManual->m_pElipso->m_bComplete)
				break;

			if (theApp.m_fnGetViewPointer()->m_pManual->m_pElipso->m_bError)
			{//����
				TECmsgOut(L"EllipsoMeter�� ���� ���¸� Ȯ���� �ּ���._LampOn", LIGHTMAGENTA, 1);
				bTmpLampOnCheck = FALSE;
				break;
			}

			tmpStep = 40;
			break;

		case 40://lamp on check
			theApp.m_fnGetViewPointer()->m_pManual->m_pElipso->m_fnGetSystemLampIsOn();
			tmpStep++;
			break;

		case 41:
			if (!theApp.m_fnGetViewPointer()->m_pManual->m_pElipso->m_bComplete)
				break;

			if (theApp.m_fnGetViewPointer()->m_pManual->m_pElipso->m_bError)
			{//����
				TECmsgOut(L"EllipsoMeter�� ���� ���¸� Ȯ���� �ּ���._LampIsOn", LIGHTMAGENTA, 1);
				bTmpLampOnCheck = FALSE;
				break;
			}
			else
			{
				if (bOn)
				{
					if (!theApp.m_fnGetViewPointer()->m_pManual->m_pElipso->m_bYesNo)
					{
						TECmsgOut(L"Lamp On�� �Ϸ���� �ʾҽ��ϴ�. Ȯ�����ּ���", LIGHTMAGENTA, 1);
						bTmpLampOnCheck = FALSE;
						break;
					}
					else
					{
						TECmsgOut(L"Lamp On�� �Ϸ�Ǿ����ϴ�.", LIGHTBLUE, 1000);
						theApp.m_fnGetViewPointer()->m_bLampOn = TRUE;
						theApp.m_fnGetViewPointer()->m_fnSetBtnOperation(IDC_BTN_LAMP_OFF, L_BIT_BTN_ON, L_BIT_CHAR_ON, "Lamp ON");
					}
				}
				else
				{
					if (theApp.m_fnGetViewPointer()->m_pManual->m_pElipso->m_bYesNo)
					{
						TECmsgOut(L"Lamp Off�� �Ϸ���� �ʾҽ��ϴ�. Ȯ�����ּ���", LIGHTMAGENTA, 1);
						bTmpLampOnCheck = FALSE;
						break;
					}
					else
					{
						TECmsgOut(L"Lamp Off�� �Ϸ�Ǿ����ϴ�.", LIGHTBLUE, 1000);
						theApp.m_fnGetViewPointer()->m_bLampOn = FALSE;
						theApp.m_fnGetViewPointer()->m_fnSetBtnOperation(IDC_BTN_LAMP_OFF, L_BIT_BTN_OFF, L_BIT_CHAR_OFF, "Lamp OFF");
					}
				}
			}
			tmpStep = 50;
			break;

		case 50://lamp hours check
			theApp.m_fnGetViewPointer()->m_pManual->m_pElipso->m_fnGetSystemGetLampHours();
			tmpStep++;
			break;

		case 51:
			if (!theApp.m_fnGetViewPointer()->m_pManual->m_pElipso->m_bComplete)
				break;

			if (theApp.m_fnGetViewPointer()->m_pManual->m_pElipso->m_bError)
			{//����
				TECmsgOut(L"EllipsoMeter�� ���� ���¸� Ȯ���� �ּ���._LampIsOn", LIGHTMAGENTA, 1);
				bTmpLampOnCheck = FALSE;
				break;
			}
			else
			{
				if (theApp.m_fnGetViewPointer()->m_pManual->m_pElipso->m_dLampHour > 1400.0)
				{
					strLampHours.Format("Lamp ���ð��� 1400�ð��� �Ѿ����ϴ�. Lamp�� ��ü���ּ���.(������ð� : %.1f)", theApp.m_fnGetViewPointer()->m_pManual->m_pElipso->m_dLampHour);
					TECmsgOut(m_Util.CStringTobstr(strLampHours), LIGHTMAGENTA, 1);
				}
				else
				{
					strLampHours.Format("Lamp ������ð� : %.1f", theApp.m_fnGetViewPointer()->m_pManual->m_pElipso->m_dLampHour);
					TECmsgOut(m_Util.CStringTobstr(strLampHours), LIGHTGREEN, 3500);
				}
			}
			tmpStep = 60;
			break;

		default:
			bTmpLampOnCheck = FALSE;
			break;

		}
	}

	pWorkRun->m_bThrdFlag[TF_ELLIPSORUN] = FALSE;
#endif
}


void CInit::OnBnClickedBtnInitStop()
{
	if ( m_bProcess ) return;

	CString strMsg("");
	if ( !theApp.m_fnGetViewPointer()->m_pTask->stEqp.bEqpInit )
	{
		strMsg = "��� �ʱ�ȭ�Ϸ������ ��� ������ ���� �ʽ��ϴ�";
		theApp.m_fnGetViewPointer( )->m_fnLogWrite( strMsg );
		m_bProcess = TRUE;
		TECmsgOut( m_Util.CStringTobstr(strMsg), RED_1, 1 );
		m_bProcess = FALSE;
	}

	if ( m_nStep != STEP_HALT )
	{
		theApp.m_fnGetViewPointer( )->m_pAjin->m_fnAxmMoveAllStop( );
	}
	m_nStep									= STEP_HALT;
	OnCancel();
}


void CInit::OnBnClickedBtnAppExt()
{
	int nResult = TECmsgOut( L"���α׷��� �����Ͻðڽ��ϱ�?", GREEN_1, 2 );
	if(nResult == 1)
	{
		AfxGetMainWnd()->PostMessage( WM_CLOSE );
		OnCancel();
	}
}

void CInit::m_fnInit()
{
	// ó�� �ѹ��� ����Ѵ�.
	if ( m_bFirst )    return;

	m_bFirst = TRUE;

	m_clInit.SetCol( 3 );
	for( int i = 0; i < MAX_AXIS; i++ )
	{
		m_clInit.SetRow( i + 2 );
		m_clInit.SetText( "�� �ʱ�ȭ" );
	}

	m_pTask = (SHEET_RESISTANCE_Format*)m_pTaskHandle->m_pFilePointer();

}


void CInit::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialogEx::OnActivate(nState, pWndOther, bMinimized);
	m_fnInit();
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}

void CInit::m_fnSetInitCtl( BOOL bEnable )
{
	GetDlgItem( IDC_BTN_INIT )->EnableWindow(bEnable);
}

void CInit::m_fnSetEnd(  )
{
	if ( m_hEqInitThread != NULL )
	{
		m_bEqInitEnd    = FALSE;
		SuspendThread( m_hEqInitThread );
		CloseHandle(m_hEqInitThread);
		m_hEqInitThread	= NULL;
	}
	OnOK();
}

LRESULT CInit::m_fnSetInitEnd(  WPARAM nNo, LPARAM lParam )
{
	if ( m_hEqInitThread != NULL )
	{
		m_bEqInitEnd    = FALSE;
		SuspendThread( m_hEqInitThread );
		CloseHandle(m_hEqInitThread);
		m_hEqInitThread	= NULL;
	}
	OnOK();
	return 0;
}

UINT EqInitThread(LPVOID pFuncData)
{
	CInit		*pQI = (CInit *)pFuncData;

	CString     strProcess[MAX_AXIS+1];
	int			nInitStep[MAX_AXIS+1];
	BOOL		bComplete[MAX_AXIS+1];

	BOOL bAlarmOnX = FALSE;
	BOOL bAlarmOnY = FALSE;
	BOOL bAlarmOnZ = FALSE;

	CString strMsg;

	for( int i = 0; i < MAX_AXIS; i++ )
	{
		strProcess[i] = "";
	}
	memset( nInitStep, 0x00, sizeof(nInitStep));
	memset( bComplete, 0x00, sizeof(bComplete));
	while ( pQI->m_bEqInitEnd )
	{
#if EQ_STATUS_CHECK
// 		DWORD   dResult[MAX_AXIS];

		//EMO����Ȯ���� I/O��...
		if ((pQI->m_pTask->stElipsoIO.stIn.EMS_1 == 0)
			&& (pQI->m_pTask->stElipsoIO.stIn.EMS_2 == 0)
			&& (pQI->m_pTask->stElipsoIO.stIn.EMS_3 == 0)
			&& (pQI->m_pTask->stElipsoIO.stIn.EMS_4 == 0)
			)
		{
			pQI->m_fnSetInitCtl( TRUE );
			pQI->m_bEqInitEnd							= FALSE;
			pQI->m_nStep								= STEP_HALT;
			theApp.m_fnGetViewPointer( )->m_pAjin->m_fnAxmMoveAllStop();
			continue;
		}
#endif
		switch( pQI->m_nStep )
		{
			case 0:
				{
					pQI->m_fnSetInitCtl( FALSE );
					pQI->m_clInit.SetCol( 3 );
					for( int i = 0; i < MAX_AXIS; i++ )
					{
						pQI->m_clInit.SetRow( i + 2 );
						pQI->m_clInit.SetText( "�� �ʱ�ȭ" );

					}
					char szMotFile[FILE_LEN+1];
					memset( szMotFile, 0x00, sizeof(szMotFile));
					sprintf_s(szMotFile, "%s%s%s%s", m_Util.m_fnGetCurrentDrive().GetString(), DEFAULT_PATH, DEFAULT_ENV_PATH, DEFAULT_MOT_FILE);
					theApp.m_fnGetViewPointer()->m_pAjin->LoadMotFile( szMotFile );
					theApp.m_fnGetViewPointer()->m_fnLogWrite( "����ʱ�ȭ��" );
					pQI->m_nCount								= 0;
					pQI->m_nStep								= 1;
				}
				break;
			case 1:
				{
					pQI->m_nInit									= 0;
					pQI->m_nCount									= 0;

					for( int i = 0; i < MAX_AXIS; i++ )
					{
						strProcess[i] = "";
					}
					memset( nInitStep, 0x00, sizeof(nInitStep));
					memset( bComplete, 0x00, sizeof(bComplete));
					nInitStep[0] = 100;
					nInitStep[1] = 100;
					nInitStep[2] = 100;
					theApp.m_fnGetViewPointer( )->m_pAjin->m_bProcess[0] = FALSE;
					theApp.m_fnGetViewPointer( )->m_pAjin->m_bProcess[1] = FALSE;
					theApp.m_fnGetViewPointer( )->m_pAjin->m_bProcess[2] = FALSE;
					pQI->m_nStep									= 2;
				}
				break;
			case 2:	// Z�� �ʱ�ȭ
				{
					if ( !bComplete[2] )
					{
						pQI->m_nRtn[2] = theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetSingleInit( Z_AXIS, nInitStep[2], strProcess[2], bComplete[2] );
					}
					if ( pQI->m_strProcess[2] != strProcess[2] )
					{
						pQI->m_strProcess[2] = strProcess[2];
						pQI->m_clInit.SetCol( 3 );
						pQI->m_clInit.SetRow( Z_AXIS + 2 );
						pQI->m_clInit.SetText( strProcess[2] );
					}
					if (( bComplete[2] )		&&
						( pQI->m_nRtn[2] == 0 ))
						{
							pQI->m_nStep									= 3;
						}
						else
						{
							if ( pQI->m_nRtn[2] != 0 )
							{
								pQI->m_fnSetInitCtl( TRUE );
								pQI->m_bEqInitEnd							= FALSE;
								pQI->m_nStep								= STEP_HALT;
								continue;
							}
						}
				}
				break;
			case 3:	// X�� �ʱ�ȭ
			{
				if (!bComplete[0])
				{
					pQI->m_nRtn[0] = theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetSingleInit(X_AXIS, nInitStep[0], strProcess[0], bComplete[0]);
				}
				if (!bComplete[1])
				{
					pQI->m_nRtn[1] = theApp.m_fnGetViewPointer()->m_pAjin->m_fnSetSingleInit(Y_AXIS, nInitStep[1], strProcess[1], bComplete[1]);
				}
				if (pQI->m_strProcess[0] != strProcess[0])
				{
					pQI->m_strProcess[0] = strProcess[0];
					pQI->m_clInit.SetCol(3);
					pQI->m_clInit.SetRow(X_AXIS + 2);
					pQI->m_clInit.SetText(strProcess[0]);
				}
				if (pQI->m_strProcess[1] != strProcess[1])
				{
					pQI->m_strProcess[1] = strProcess[1];
					pQI->m_clInit.SetCol(3);
					pQI->m_clInit.SetRow(Y_AXIS + 2);
					pQI->m_clInit.SetText(strProcess[1]);
				}
				if ((bComplete[0]) &&
					(pQI->m_nRtn[0] == 0))
				{
					pQI->m_nStep = 4;
				}
				else
				{
					if (pQI->m_nRtn[0] != 0)
					{
						pQI->m_fnSetInitCtl(TRUE);
						pQI->m_bEqInitEnd = FALSE;
						pQI->m_nStep = STEP_HALT;
						continue;
					}
				}
			}
			break;
			case 4:	// Y �� �ʱ�ȭ
				{
					if ( !bComplete[1] )
					{
						pQI->m_nRtn[1] = theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetSingleInit( Y_AXIS, nInitStep[1], strProcess[1], bComplete[1] );
					}
					if ( pQI->m_strProcess[1] != strProcess[1] )
					{
						pQI->m_strProcess[1] = strProcess[1];
						pQI->m_clInit.SetCol( 3 );
						pQI->m_clInit.SetRow( Y_AXIS + 2 );
						pQI->m_clInit.SetText( strProcess[1] );
					}
					if (( bComplete[0] )		&&
						(pQI->m_nRtn[0] == 0) &&
						(bComplete[1]) &&
						(pQI->m_nRtn[1] == 0) &&
						(bComplete[2]) &&
						( pQI->m_nRtn[2] == 0 ))
					{
						bAlarmOnX			= FALSE;
						bAlarmOnY			= FALSE;
						bAlarmOnZ			= FALSE;
						theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAlarm( X_AXIS, bAlarmOnX );
						theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAlarm( Y_AXIS, bAlarmOnY );
						theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAlarm( Z_AXIS, bAlarmOnZ );
						if (( !bAlarmOnX ) &&
							( !bAlarmOnY ) &&
							( !bAlarmOnZ ))
						{
							pQI->m_bEqInitEnd							= FALSE;
							pQI->m_nStep								= STEP_HALT;
							theApp.m_fnGetViewPointer( )->m_pTask->stEqp.bEqpInit     = TRUE;
							strMsg = "��� ��ü �ʱ�ȭ�� �Ϸ�Ǿ����ϴ�.";
							pQI->m_bProcess = TRUE;
							TECmsgOut( m_Util.CStringTobstr( strMsg ), GREEN_1, 1 );
							pQI->m_bProcess = FALSE;
							theApp.m_fnGetViewPointer( )->m_pAjin->m_bInitEnd = TRUE;
							theApp.m_fnGetViewPointer( )->m_fnLogWrite( strMsg );
							theApp.m_fnGetViewPointer()->m_fnLogWrite( "����ʱ�ȭ�Ϸ�" );
							if ( pQI->m_hWnd != NULL )
							{
								pQI->PostMessage( WM_SET_INIT_END, 0, 0 );
							}
						}
						else
						{
							pQI->m_bEqInitEnd							= FALSE;
							pQI->m_nStep								= STEP_HALT;
							theApp.m_fnGetViewPointer( )->m_pTask->stEqp.bEqpInit     = FALSE;
							strMsg = "��� ��ü �ʱ�ȭ�� �����߽��ϴ�.";
							pQI->m_bProcess = TRUE;
							TECmsgOut( m_Util.CStringTobstr( strMsg ), GREEN_1, 1 );
							pQI->m_bProcess = FALSE;
							theApp.m_fnGetViewPointer( )->m_pAjin->m_bInitEnd = FALSE;
							theApp.m_fnGetViewPointer( )->m_fnLogWrite( strMsg );
							theApp.m_fnGetViewPointer()->m_fnLogWrite( "����ʱ�ȭ����" );
						}
					}
					else
					{
						if (( pQI->m_nRtn[0] != 0 ) ||
							( pQI->m_nRtn[1] != 0 ) ||
							( pQI->m_nRtn[2] != 0 ))
						{
							pQI->m_fnSetInitCtl( TRUE );
							pQI->m_bEqInitEnd							= FALSE;
							pQI->m_nStep								= STEP_HALT;
						}
					}
				}
				break;
		}
		Sleep( 100 );
	}

	if ( pQI->m_hWnd == NULL ) return 0;

	if ( pQI->m_hEqInitThread != NULL )
	{
		pQI->m_bEqInitEnd = FALSE;
		SuspendThread( pQI->m_hEqInitThread );
		CloseHandle(pQI->m_hEqInitThread);
		pQI->m_hEqInitThread	= NULL;
	}

	return 0;
}

void CInit::OnBnClickedBtnInitShow()
{
	m_fnSetEnd();
}


void CInit::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}
