// CElipsoMeterRobotIF.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "EllipsoMeter.h"
#include "ElipsoMeterRobotIF.h"
#include "afxdialogex.h"
#include "WorkRun.h"

// CILJINInspectIF 대화 상자입니다.

IMPLEMENT_DYNAMIC(CElipsoMeterRobotIF, CDialogEx)
CElipsoMeterRobotIF::CElipsoMeterRobotIF(CWnd* pParent /*=NULL*/)
							: CDialogEx(CElipsoMeterRobotIF::IDD, pParent)
{
	m_bShow = FALSE;

	this->Create( IDD_ELIPSOMETER_ROBOT_IF );

	m_pClient			= NULL;
	m_pServer			= NULL;
	m_bConnect          = FALSE;
	m_bServerRun		= FALSE;
	m_bCommandSend      = false;
	m_pTaskHandle       = NULL;
	m_pTask		        = NULL;

	m_strData           = _T("");
	m_strData.FreeExtra();
	m_strRcvData.RemoveAll();
	m_strRcvData.FreeExtra();
	m_bProcess          = FALSE;

}

CElipsoMeterRobotIF::~CElipsoMeterRobotIF()
{
}

void CElipsoMeterRobotIF::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMM_LIST, m_lstComm);
}


BEGIN_MESSAGE_MAP(CElipsoMeterRobotIF, CDialogEx)
	ON_MESSAGE(WM_LAN_RECEIVE, OnReceive)					// LAN RECEIVE Event
	ON_MESSAGE(WM_LAN_ACCEPT, OnAccept)					// LAN ACCEPT Event
	ON_MESSAGE(WM_LAN_CLOSE, OnClose)						// LAN CLOSE Event
	ON_MESSAGE(WM_LAN_CONNECT, OnConnectCheck)		// LAN CONNECTCHECK Event
	ON_BN_CLICKED(IDC_BTN_OPEN, &CElipsoMeterRobotIF::OnBnClickedBtnOpen)
	ON_BN_CLICKED(IDC_BTN_CLOSE, &CElipsoMeterRobotIF::OnBnClickedBtnClose)
	ON_BN_CLICKED(IDC_BTN_LINE_SET, &CElipsoMeterRobotIF::OnBnClickedBtnLineSet)
	ON_BN_CLICKED(IDC_BTN_CLR, &CElipsoMeterRobotIF::OnBnClickedBtnClr)
	ON_BN_CLICKED(IDC_BTN_EXIT, &CElipsoMeterRobotIF::OnBnClickedBtnExit)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_LAN_SEND, &CElipsoMeterRobotIF::OnBnClickedBtnLanSend)
	ON_BN_CLICKED(IDC_BTN_SAVE_DATA, &CElipsoMeterRobotIF::OnBnClickedBtnSaveData)
END_MESSAGE_MAP()

BOOL CElipsoMeterRobotIF::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (( nID   == 2 ) &&
		( nCode == -1 ))
	{
		m_fnSetShowFlag( FALSE );
	}
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CElipsoMeterRobotIF::m_fnSetShowFlag( BOOL bShow )
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

BOOL CElipsoMeterRobotIF::PreTranslateMessage(MSG* pMsg) 
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
void CElipsoMeterRobotIF::m_fnSetInit( ) 
{
	GetDlgItem( IDC_EDT_LINE_CNT )->SetWindowText( "1000" );

	CString s("");
	s.Format( "ElipsoMeter 로봇 프로그램 인터페이스 메인창" );
	this->SetWindowText(s);

	m_Log.m_fnSetLogPath( "ElipsoMeterRobot", "ElipsoMeterRobot" );
	m_Log.m_fnSetLogWriteFlag( true );
	m_fnLogWrite( "<<< 통신개시 >>>" );	
	m_pTaskHandle = new CTaskHandle( this, SHARED_NAME, 
											1, 
											sizeof( SHEET_RESISTANCE_Format ) + 1,
											0,
											FALSE );

	m_pTask					= ( SHEET_RESISTANCE_Format* )m_pTaskHandle->m_pFilePointer( );

	m_fnGetELIPSOMETER_ROBOT_Load();

	CString strPort("");
	CString strIP("");
	CString strReConnectTime("");
	strPort.Format( "%01d", m_stLan.nPort1 );
	strIP.Format( "%s", m_stLan.szIP );
	GetDlgItem( IDC_EDT_IP )->SetWindowText( strIP );
	GetDlgItem( IDC_EDT_PORT )->SetWindowText( strPort );
	((CButton*)GetDlgItem( IDC_CHK_MODE ))->SetCheck( m_stLan.bPassive );
	strReConnectTime.Format( "%01d", m_stLan.nReConnectTime );
	GetDlgItem( IDC_EDT_RECONNECT_TIME )->SetWindowText( strReConnectTime );

	GetDlgItem( IDC_EDT_LINE_CNT )->SetWindowText( "1000" );

	m_bConnect				    = false;

	m_fnSetOpen();

	SetTimer( ELIPSOMETER_CONNECT, m_stLan.nReConnectTime, NULL );
	SetTimer( ELIPSOMETER_COMM, ELIPSOMETER_TIME, NULL );
}

//---------------------------------------------------------------------------------------------------------
// 포트 OPEN.
//---------------------------------------------------------------------------------------------------------
void CElipsoMeterRobotIF::m_fnSetOpen() 
{
	CString strIP("");
	CString strPort("");
	int     nPort = 0;
	BOOL bMode = ((CButton*)GetDlgItem( IDC_CHK_MODE ))->GetCheck( );
	GetDlgItem( IDC_EDT_IP )->GetWindowText( strIP );
	GetDlgItem( IDC_EDT_PORT )->GetWindowText( strPort );
	nPort = _ttoi( strPort );

	if ( !bMode )		// CLIENT
	{
		if ( m_pClient != NULL )
		{
			delete m_pClient;
			m_pClient                   = NULL;
			m_bConnect                  = false;
			((CButton*)GetDlgItem( IDC_CHK_ONLINE ))->SetCheck( m_bConnect );
		}
		m_pClient = new CDataClientSocketEx( this, IP_PORT_ROBOT );
		if(!m_pClient->Create())
		{
			MessageBox(TEXT("소켓 생성에 실패했습니다.(Ip Port)"), TEXT("확인"), MB_OK+MB_ICONEXCLAMATION);
			return;
		}
		m_pClient->Connect( strIP, nPort );
	}
	else				// SERVER
	{
		if ( m_pServer != NULL )
		{
			delete m_pServer;
			m_pServer = NULL;
		}
		if ( m_pClient != NULL )
		{
			delete m_pClient;
			m_pClient                   = NULL;
			m_bConnect                  = false;
			((CButton*)GetDlgItem( IDC_CHK_ONLINE ))->SetCheck( m_bConnect );
		}
		m_pServer = new CDataServerSocketEx(IP_PORT_ROBOT);
		m_pServer->SetOwner(this);
		m_pServer->Create(nPort);
		m_pServer->Listen();
		m_bServerRun = TRUE;
	}
}

//---------------------------------------------------------------------------------------------------------
// 종료에 필요한 작업을 한다.
//---------------------------------------------------------------------------------------------------------
void CElipsoMeterRobotIF::m_fnSetEnd() 
{
	if ( m_hWnd != NULL )
	{
		KillTimer( ELIPSOMETER_CONNECT );
		KillTimer( ELIPSOMETER_COMM );
		m_fnLogWrite( "<<< 통신종료 >>>" );	
	}

	if ( m_pClient != NULL )
	{
		delete m_pClient;
		m_pClient                   = NULL;
		m_bConnect                  = false;
	}

	if ( m_pServer != NULL )
	{
		delete m_pServer;
		m_pServer = NULL;
	}

	if ( m_pTaskHandle != NULL )
	{
		m_pTaskHandle->m_fnSetEnd();
		delete m_pTaskHandle;
		m_pTaskHandle = NULL;
	}

}

void CElipsoMeterRobotIF::OnBnClickedBtnExit()
{
	m_fnSetShowFlag( FALSE );
}

/*-----------------------------------------------------------------------
Func Name : OnReceive
Reference : OnReceive
-----------------------------------------------------------------------*/
LRESULT CElipsoMeterRobotIF::OnReceive(WPARAM nID, LPARAM lParam)
{
	CString strReceiveData("");
	CString strMsg("");
	CString strTemp("");
	CString strData("");
	BOOL    bFind  = true;
	int     nFind  = -1;
	int     nCount = 0;
	if ( nID == IP_PORT_ROBOT)
	{
		BYTE byReceiveData[MAX_BUFF_SIZE] = {0,};
		memset( byReceiveData, 0x00, sizeof(byReceiveData) );
		nCount = m_pClient->Receive(byReceiveData, sizeof(byReceiveData));
		m_strData  += (char*)byReceiveData;
		strMsg.Format( "RECV : %s", (char*)byReceiveData );
		m_fnLogWrite( strMsg );
		while ( bFind )
		{
			nFind = m_strData.Find( ETX, 0 );
			if ( nFind == -1 )
			{
				bFind = false;
			}
			else
			{
				m_strRcvData.Add( m_strData.Left( nFind + 1 ));
				m_strData.Delete( 0, nFind + 1 );
				m_strData.FreeExtra();
			}
		}
	}

	return 0;
}

/*-----------------------------------------------------------------------
Func Name : OnReceive
Reference : OnReceive
-----------------------------------------------------------------------*/
LRESULT CElipsoMeterRobotIF::OnAccept(WPARAM nID, LPARAM lParam)
{
	if ( nID == IP_PORT_ROBOT)
	{
		if ( m_pClient != NULL )
		{
			delete m_pClient;
			m_pClient = NULL;
		}

		m_pClient = new CDataClientSocketEx( this, IP_PORT_ROBOT);
		if(!m_pServer->Accept( *m_pClient ))
		{
			return 0;
		}
		m_bConnect                  = TRUE;
		((CButton*)GetDlgItem( IDC_CHK_ONLINE ))->SetCheck( m_bConnect );
		m_fnLogWrite( "Accept OK" );
	}
	return 0;
}

/*-----------------------------------------------------------------------
Func Name : OnReceive
Reference : OnReceive
-----------------------------------------------------------------------*/
LRESULT CElipsoMeterRobotIF::OnClose(WPARAM nID, LPARAM lParam)
{
	if ( nID == IP_PORT_ROBOT)
	{
		m_bConnect                  = false;
		if ( m_hWnd != NULL )
		{
			((CButton*)GetDlgItem( IDC_CHK_ONLINE ))->SetCheck( m_bConnect );
		}
		m_fnLogWrite( "DisConnected" );
	}
	return 0;
}

/*-----------------------------------------------------------------------
Func Name : OnReceive
Reference : OnReceive
-----------------------------------------------------------------------*/
LRESULT CElipsoMeterRobotIF::OnConnectCheck(WPARAM nID, LPARAM lParam)
{
	if ( nID == IP_PORT_ROBOT)
	{
		((CButton*)GetDlgItem( IDC_CHK_ONLINE ))->SetCheck( m_pClient->m_bConnect );
		m_bConnect              = m_pClient->m_bConnect;
		if ( m_pClient->m_bConnect )
		{
			m_fnLogWrite( "Connected" );
		}
		else
		{
			m_fnLogWrite( "DisConnected" );
		}
	}
	return 0;
}

void CElipsoMeterRobotIF::m_fnSend( CString strData )
{
	CString strSendData("");
	if ( !m_bConnect )
	{
		strSendData.Format("접속중인 Client1 가 없습니다.(%s)", strData );
		m_fnLogWrite( strSendData );
		return;
	}

	CString strMsg("");

	strSendData += (char)STX;
	strSendData += strData;
	strSendData += (char)ETX;

	strMsg.Format( "SEND : %s", strSendData );
	m_fnLogWrite( strMsg );
	m_pClient->Send( strSendData.GetBuffer(strSendData.GetLength()), strSendData.GetLength() );
}

void CElipsoMeterRobotIF::OnBnClickedBtnOpen()
{
	m_fnSetOpen();
}


void CElipsoMeterRobotIF::OnBnClickedBtnClose()
{
	if(m_pClient != NULL)
	{
		delete m_pClient;
		m_pClient = NULL;
	}
	m_bConnect      = false;
	((CButton*)GetDlgItem( IDC_CHK_ONLINE ))->SetCheck( m_bConnect );
	m_fnLogWrite( "DisConnected" );
}


void CElipsoMeterRobotIF::OnBnClickedBtnLineSet()
{
	CString strCnt("");
	CString strMsg("");
	int nCnt		= 0;

	GetDlgItem( IDC_EDT_LINE_CNT )->GetWindowText( strCnt );
	nCnt			= _ttoi(strCnt);

	if ( nCnt <= 0 )
	{
		GetDlgItem( IDC_EDT_LINE_CNT )->SetWindowText( "1" );
		nCnt = 1;

	}
	strMsg.Format( "통신로그 라인수 변경 = %d" , nCnt );
	m_fnLogWrite( strMsg );

	UpdateData(FALSE);
}


void CElipsoMeterRobotIF::OnBnClickedBtnClr()
{
	m_lstComm.ResetContent();	
}

//---------------------------------------------------------------------------------------------------------
// Log 파일을 저장한다.
//---------------------------------------------------------------------------------------------------------
void CElipsoMeterRobotIF::m_fnLogWrite( CString strData, BOOL bWrite )
{
	CString strCnt("");
	int     nCnt		= 0;
	int     nTotal		= 0;

	if (( m_pTask != NULL ) &&
		( m_pTaskHandle != NULL ))
	{
		if( m_pTask->stCommon.bLogSave )
		{
			m_Log.DEBUG_LOG( strData.GetBuffer(strData.GetLength()) );
		}
	}
	else
	{
		m_Log.DEBUG_LOG( strData.GetBuffer(strData.GetLength()) );
	}

	if ( m_hWnd == NULL )
	{
		return;
	}

	if ( bWrite )
	{
		GetDlgItem( IDC_EDT_LINE_CNT )->GetWindowText( strCnt );
		nCnt			= _ttoi( strCnt ) - 1;

		nTotal			= m_lstComm.GetCount();
		if ( nCnt < nTotal )
		{
			for( int i = nTotal; i >= nCnt; i-- )
			{
				m_lstComm.DeleteString( i );	
			}
		}
		m_lstComm.InsertString( 0, strData );
	}
}

//------------------------------------------------------------------------
// 파일의 경로를 읽어온다.
//------------------------------------------------------------------------
CString CElipsoMeterRobotIF::m_fnGetELIPSOMETERROBOTFileFullPath()
{
	CString strPath("");
	strPath.Format( "%s%s%s%s", m_Util.m_fnGetCurrentDrive(),
								DEFAULT_PATH,
								DEFAULT_ENV_PATH,
								DEFAULT_ELIPSOMETERROBOT_FILE );
	return strPath;
}

//------------------------------------------------------------------------
// 설정 환경파일을 저장한다.
//------------------------------------------------------------------------
BOOL CElipsoMeterRobotIF::m_fnSetELIPSOMETER_ROBOT_Save( )
{
	CString strWork("");
	CString strData("");
	CString strFullPath("");

	CStringArray strLst;
	CXmlElement *pElement     = NULL;
	CXmlElement *pRootElement = NULL;

	strFullPath		=	m_fnGetELIPSOMETERROBOTFileFullPath();

	m_fnLogWrite( "LAN Data 저장" );

	CXmlDocument* pXML = new CXmlDocument();

	if ( m_Util.Existence( strFullPath ) != FM_FILE )
	{
		m_fnLogWrite( "LAN Data 저장 실패(File Not Found)" );
		return 0;
	}
	pXML->Load( strFullPath );

	pXML->GetTagToString( ETHERNET , strData );
	pXML->Parse( strData );
	pRootElement = pXML->GetRootElement();
	if ( pRootElement == NULL )
	{
		delete pXML;
		pXML = NULL;
		return FALSE;
	}

	strLst.RemoveAll();
	strLst.Add(ELEMENT);
	strLst.Add(INSPECT);
	pElement = pXML->FindElement( pRootElement, strLst );
	if ( pElement != NULL )
	{
		// LAN IP
		strWork.Format( "%s", m_stLan.szIP );
		pXML->DataSet( pElement, LAN_IP, strWork );

		// LAN PORT1
		strWork.Format( "%01d", m_stLan.nPort1 );
		pXML->DataSet( pElement, LAN_PORT1, strWork );

		// LAN MODE
		strWork = LAN_ACTIVE;
		if ( m_stLan.bPassive )
		{
			strWork = LAN_PASSIVE;
		}
		pXML->DataSet( pElement, LAN_MODE, strWork );

		// RECONNECT TIME
		strWork.Format( "%01d", m_stLan.nReConnectTime );
		pXML->DataSet( pElement, RECONNECT_TIME, strWork );

		pXML->Store( strFullPath );
	}

	delete pXML;
	pXML = NULL;

	m_fnLogWrite( "LAN Data 저장 완료" );
	return TRUE;
}

//------------------------------------------------------------------------
// 설정 환경파일을 로드한다.
//------------------------------------------------------------------------
BOOL CElipsoMeterRobotIF::m_fnGetELIPSOMETER_ROBOT_Load( )
{
	CString			strWork("");
	CString			strFullPath("");
	CString			strCurrPath("");
	int				i			= 0;
	int             j           = 0;

	CStringArray strLst;
	CString      strData("");
	CXmlElement *pElement     = NULL;
	CXmlElement *pRootElement = NULL;

	strFullPath					=	m_fnGetELIPSOMETERROBOTFileFullPath();

	m_fnLogWrite( "LAN Data 읽기" );

	if ( m_Util.Existence( strFullPath ) != FM_FILE ) 
	{
		m_fnLogWrite( "LAN Data 읽기(파일없음)" );
		return false;
	}

	CXmlDocument* pXML = new CXmlDocument();
	pXML->Load( strFullPath );

	pXML->GetTagToString( ETHERNET , strData );
	pXML->Parse( strData );
	pRootElement = pXML->GetRootElement();
	if ( pRootElement == NULL )
	{
		delete pXML;
		pXML = NULL;
		return FALSE;
	}

	strLst.RemoveAll();
	strLst.Add(ELEMENT);
	strLst.Add(INSPECT);
	pElement = pXML->FindElement( pRootElement, strLst );
	if ( pElement != NULL )
	{
		// LAN IP
		pXML->FindData( pElement, LAN_IP, strWork );
		sprintf_s(m_stLan.szIP, sizeof(m_stLan.szIP), "%s", strWork.GetString());

		// LAN PORT1
		pXML->FindData( pElement, LAN_PORT1, strWork );
		m_stLan.nPort1		= _ttoi( strWork );

		// LAN MODE
		pXML->FindData( pElement, LAN_MODE, strWork );
		m_stLan.bPassive	= FALSE;
		if ( strWork == LAN_PASSIVE )
		{
			m_stLan.bPassive	= TRUE;
		}

		// RECONNECT TIME
		pXML->FindData( pElement, RECONNECT_TIME, strWork );
		m_stLan.nReConnectTime		= _ttoi( strWork );
	}

	delete pXML;
	pXML = NULL;

	m_fnLogWrite( "LAN Data 읽기 완료" );
	return TRUE;
}



void CElipsoMeterRobotIF::OnTimer(UINT_PTR nIDEvent)
{
	if ( nIDEvent == ELIPSOMETER_CONNECT )
	{
		if ( !m_bConnect )
		{
			if ( !m_stLan.bPassive )
			{
				m_fnSetOpen();
			}
		}
	}

	if ( nIDEvent == ELIPSOMETER_COMM )
	{
		if ( !m_bProcess )
		{
			m_bProcess = true;
			if ( m_strRcvData.GetCount() > 0 )
			{
				m_fnAnalysis_Data();
			}
			m_bProcess = false;
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CElipsoMeterRobotIF::m_fnAnalysis_Data()
{
	if ( m_strRcvData.GetCount() == 0 ) return;

	CString strAnalysisData("");
	int     nCmd      = 0;
	int     nNo       = 0;
	int     nCommand  = 0;
	BOOL    bFindOK = FALSE;
	CString strNum;

	strAnalysisData = m_strRcvData.GetAt( 0 );
	if (strAnalysisData.GetLength() < 3)
	{
		strAnalysisData.Delete(0, strAnalysisData.GetLength());
		strAnalysisData.FreeExtra();
		m_strRcvData.RemoveAt(0);
		return;
	}

	strAnalysisData.Delete(0, 1);
	strAnalysisData.FreeExtra();
	nCommand = atoi(strAnalysisData.Left(1));
	switch (nCommand)
	{
		case 1:	// LOT START
		{
			theApp.m_fnGetViewPointer()->m_pAuto->m_strInspDate = m_Util.m_fnGetDate();
			sprintf_s(theApp.m_fnGetViewPointer()->m_pAuto->m_stUser.szDate, "%s", theApp.m_fnGetViewPointer()->m_pAuto->m_strInspDate.GetString());
			theApp.m_fnGetViewPointer()->m_pAuto->m_strLotStartTime = m_Util.m_fnGetTime24();
			pWorkRun->m_bThrdFlag[TF_CASSETTE_START] = TRUE;
		}
		break;
		case 2:	// LOT END
			{
#if MIL_USE
				pWorkRun->m_bFinishedCassette = TRUE;
#endif
			}
			break;
		case 3:	// WORK END
			{
				strAnalysisData.Delete(0, 1);
				strAnalysisData.FreeExtra();
				nNo = atoi(strAnalysisData.Left(2)) + 1;
				strNum.Format("%01d", nNo);
				theApp.m_fnGetViewPointer()->m_pAuto->m_ctrlEditWaferNum.SetWindowTextA(strNum);
				strAnalysisData = "";
			}
			break;
		case 4:	// SLOT
			{
#if MIL_USE
				pWorkRun->m_bJustNowFinish = TRUE;
#endif
				EnableDlgItem(theApp.m_fnGetViewPointer()->m_pAuto, FALSE);
// 				theApp.m_fnGetViewPointer()->m_pAuto->SendMessage(WM_UPDATEUISTATE);
				TECmsgOut(L"강제 검사종료 입니다.", LIGHTRED, 2000);
			}
			break;
	}
// 	if (strAnalysisData.Left(1) == "1")		// LOT START
// 	{
// 		theApp.m_fnGetViewPointer()->m_pAuto->m_strInspDate			= m_Util.m_fnGetCurrentTime6();
// 		theApp.m_fnGetViewPointer()->m_pAuto->m_strInspTime			= m_Util.m_fnGetCurrentTime1();
// 		theApp.m_fnGetViewPointer()->m_pAuto->m_strLotStartTime		= m_Util.m_fnGetLotStartTime();
// 
// 		sprintf_s(theApp.m_fnGetViewPointer()->m_pAuto->m_stUser.szDate, "%s", theApp.m_fnGetViewPointer()->m_pAuto->m_strInspDate.GetString());
// 		sprintf_s(theApp.m_fnGetViewPointer()->m_pAuto->m_stUser.szTime, "%s", theApp.m_fnGetViewPointer()->m_pAuto->m_strInspTime.GetString());
// 	}
// 	if (strAnalysisData.Left(1) == "2")		// LOT END
// 	{
// // 		if(TECmsgOut(L"카세트 종료확인. 검사종료하시겠습니까?",LIGHTMAGENTA, 2, 17, WHITE, 3))
// 		{
// #if MIL_USE
// 			pWorkRun->m_bFinishedCassette = TRUE;
// #endif
// 		}
// 
// 	}
// 	if (strAnalysisData.Left(1) == "4")		// Work END
// 	{
// #if MIL_USE
// 		pWorkRun->m_bJustNowFinish = TRUE;
// #endif
// 		TECmsgOut(L"강제 검사종료 입니다.", LIGHTRED, 2000);
// 	}
// 	if (strAnalysisData.Left(1) == "3")
// 	{
// 		strAnalysisData.Delete(0, 1);
// 		strAnalysisData.FreeExtra();
// 		nNo = atoi(strAnalysisData.Left(2)) + 1;
// 		strNum.Format("%01d", nNo);
// 		theApp.m_fnGetViewPointer()->m_pAuto->m_ctrlEditWaferNum.SetWindowTextA(strNum);
// 		strAnalysisData = "";
// 	}

	m_strRcvData.RemoveAt( 0 );
}

void CElipsoMeterRobotIF::OnBnClickedBtnLanSend()
{
	CString strSend("");
	GetDlgItem( IDC_EDT_DATA )->GetWindowText( strSend );
	if ( !strSend.IsEmpty())
	{
		m_fnSend( strSend );
	}
}

void CElipsoMeterRobotIF::OnBnClickedBtnSaveData()
{
	CString strIp("");
	CString strPort("");
	CString strReconnectTime("");
	strPort.Format( "%01d", m_stLan.nPort1 );
	GetDlgItem( IDC_EDT_IP )->GetWindowText( strIp  );
	sprintf_s(m_stLan.szIP, sizeof(m_stLan.szIP), "%s", strIp.GetString());
	GetDlgItem( IDC_EDT_PORT )->GetWindowText( strPort );
	m_stLan.nPort1     = _ttoi( strPort );
	m_stLan.bPassive = ((CButton*)GetDlgItem( IDC_CHK_MODE ))->GetCheck(  );
	GetDlgItem( IDC_EDT_RECONNECT_TIME )->GetWindowText( strReconnectTime );
	m_stLan.nReConnectTime     = _ttoi( strReconnectTime );

	m_fnSetELIPSOMETER_ROBOT_Save();
}

