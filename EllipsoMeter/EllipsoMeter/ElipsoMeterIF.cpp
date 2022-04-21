// ElipsoMeterIF.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "EllipsoMeter.h"
#include "ElipsoMeterIF.h"
#include "afxdialogex.h"
#include "WorkRun.h"
CTimeInterval CheckTimeSend;

// CILJINInspectIF 대화 상자입니다.

IMPLEMENT_DYNAMIC(CElipsoMeterIF, CDialogEx)
CElipsoMeterIF::CElipsoMeterIF(CWnd* pParent /*=NULL*/)
							: CDialogEx(CElipsoMeterIF::IDD, pParent)
{
	m_bShow = FALSE;

	this->Create( IDD_ELIPSOMETER_IF );

	m_pClient1			= NULL;
	m_pServer1			= NULL;
	m_pClient2			= NULL;
	m_pServer2			= NULL;
	m_bConnect1         = FALSE;
	m_bConnect2         = FALSE;
	m_bServerRun		= FALSE;
	m_bCommandSend      = false;
	m_pTaskHandle       = NULL;
	m_pTask		        = NULL;

	m_strData1    = _T("");
	m_strData1.FreeExtra();
	m_strRcvData1.RemoveAll();
	m_strRcvData1.FreeExtra();
	m_strData2    = _T("");
	m_strData2.FreeExtra();
	m_strRcvData2.RemoveAll();
	m_strRcvData2.FreeExtra();
	m_bProcess          = FALSE;
	m_nCmdNo            = -1;
	m_hResultThread     = NULL;
	m_bResultEnd        = FALSE;
	m_nStep             = 0;
	m_bResultProcess    = FALSE;
	m_bYesNo            = FALSE;
}

CElipsoMeterIF::~CElipsoMeterIF()
{
}

void CElipsoMeterIF::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMM_LIST, m_lstComm);
	DDX_Control(pDX, IDC_RECIPE_LIST, m_clRecipeList);
}


BEGIN_MESSAGE_MAP(CElipsoMeterIF, CDialogEx)
	ON_MESSAGE(WM_LAN_RECEIVE, OnReceive)					// LAN RECEIVE Event
	ON_MESSAGE(WM_LAN_ACCEPT, OnAccept)					// LAN ACCEPT Event
	ON_MESSAGE(WM_LAN_CLOSE, OnClose)						// LAN CLOSE Event
	ON_MESSAGE(WM_LAN_CONNECT, OnConnectCheck)		// LAN CONNECTCHECK Event
	ON_BN_CLICKED(IDC_BTN_OPEN, &CElipsoMeterIF::OnBnClickedBtnOpen)
	ON_BN_CLICKED(IDC_BTN_CLOSE, &CElipsoMeterIF::OnBnClickedBtnClose)
	ON_BN_CLICKED(IDC_BTN_LINE_SET, &CElipsoMeterIF::OnBnClickedBtnLineSet)
	ON_BN_CLICKED(IDC_BTN_CLR, &CElipsoMeterIF::OnBnClickedBtnClr)
	ON_BN_CLICKED(IDC_BTN_EXIT, &CElipsoMeterIF::OnBnClickedBtnExit)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_LAN_SEND, &CElipsoMeterIF::OnBnClickedBtnLanSend)
	ON_BN_CLICKED(IDC_BTN_SAVE_DATA, &CElipsoMeterIF::OnBnClickedBtnSaveData)
	ON_BN_CLICKED(IDC_BTN_START_ACQ, &CElipsoMeterIF::OnBnClickedBtnStartAcq)
	ON_BN_CLICKED(IDC_BTN_LIST_RECIPES, &CElipsoMeterIF::OnBnClickedBtnListRecipes)
	ON_BN_CLICKED(IDC_BTN_RUN_RECIPE, &CElipsoMeterIF::OnBnClickedBtnRunRecipe)
	ON_BN_CLICKED(IDC_BTN_LOAD_RECIPE, &CElipsoMeterIF::OnBnClickedBtnLoadRecipe)
	ON_BN_CLICKED(IDC_BTN_STATUS, &CElipsoMeterIF::OnBnClickedBtnStatus)
	ON_BN_CLICKED(IDC_BTN_INIT_STATUS, &CElipsoMeterIF::OnBnClickedBtnInitStatus)
	ON_BN_CLICKED(IDC_BTN_GET_MODEL_STRUCTURE, &CElipsoMeterIF::OnBnClickedBtnGetModelStructure)
	ON_BN_CLICKED(IDC_BTN_SYSTEM_GET_LAMP_HOURS, &CElipsoMeterIF::OnBnClickedBtnSystemGetLampHours)
	ON_BN_CLICKED(IDC_BTN_SYSTEM_LAMP_SHUTTER_OPEN, &CElipsoMeterIF::OnBnClickedBtnSystemLampShutterOpen)
	ON_BN_CLICKED(IDC_BTN_SYSTEM_FILTER_WHEEL_AUTO_ON, &CElipsoMeterIF::OnBnClickedBtnSystemFilterWheelAutoOn)
	ON_BN_CLICKED(IDC_BTN_SYSTEM_LAMP_SHUTTER_CLOSED, &CElipsoMeterIF::OnBnClickedBtnSystemLampShutterClosed)
	ON_BN_CLICKED(IDC_BTN_SYSTEM_FILTER_WHEEL_AUTO_OFF, &CElipsoMeterIF::OnBnClickedBtnSystemFilterWheelAutoOff)
	ON_BN_CLICKED(IDC_BTN_SYSTEM_GET_RECEIVER_CAMERA_ALIGN, &CElipsoMeterIF::OnBnClickedBtnSystemGetReceiverCameraAlign)
	ON_BN_CLICKED(IDC_BTN_MEASURE, &CElipsoMeterIF::OnBnClickedBtnMeasure)
	ON_BN_CLICKED(IDC_BTN_ALIGN, &CElipsoMeterIF::OnBnClickedBtnAlign)
	ON_BN_CLICKED(IDC_BTN_SYSTEM_LAMP_ON, &CElipsoMeterIF::OnBnClickedBtnSystemLampOn)
	ON_BN_CLICKED(IDC_BTN_SYSTEM_LAMP_OFF, &CElipsoMeterIF::OnBnClickedBtnSystemLampOff)
	ON_BN_CLICKED(IDC_BTN_SYSTEM_LAMP_IS_ON, &CElipsoMeterIF::OnBnClickedBtnSystemLampIsOn)
	ON_BN_CLICKED(IDC_BTN_SYSTEM_LAMP_POWER_IS_ON, &CElipsoMeterIF::OnBnClickedBtnSystemLampPowerIsOn)
END_MESSAGE_MAP()

BOOL CElipsoMeterIF::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (( nID   == 2 ) &&
		( nCode == -1 ))
	{
		m_fnSetShowFlag( FALSE );
	}
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CElipsoMeterIF::m_fnSetShowFlag( BOOL bShow )
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

BOOL CElipsoMeterIF::PreTranslateMessage(MSG* pMsg) 
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
void CElipsoMeterIF::m_fnSetInit( ) 
{
	GetDlgItem( IDC_EDT_LINE_CNT )->SetWindowText( "1000" );

	CString s("");
	s.Format( "ElipsoMeter 프로그램 인터페이스 메인창" );
	this->SetWindowText(s);

	m_Log.m_fnSetLogPath( "ElipsoMeter", "ElipsoMeter" );
	m_Log.m_fnSetLogWriteFlag( true );
	m_fnLogWrite( "<<< 통신개시 >>>" );	
	m_pTaskHandle = new CTaskHandle( this, SHARED_NAME, 
											1, 
											sizeof( SHEET_RESISTANCE_Format ) + 1,
											0,
											FALSE );

	m_pTask					= ( SHEET_RESISTANCE_Format* )m_pTaskHandle->m_pFilePointer( );

	m_fnGetELIPSOMETER_Load();

	CString strPort1("");
	CString strPort2("");
	CString strIP("");
	CString strReConnectTime("");
	strPort1.Format( "%01d", m_stLan.nPort1 );
	strPort2.Format( "%01d", m_stLan.nPort2 );
	strIP.Format( "%s", m_stLan.szIP );
	GetDlgItem( IDC_EDT_IP )->SetWindowText( strIP );
	GetDlgItem( IDC_EDT_PORT1 )->SetWindowText( strPort1 );
	GetDlgItem( IDC_EDT_PORT2 )->SetWindowText( strPort2 );
	((CButton*)GetDlgItem( IDC_CHK_MODE ))->SetCheck( m_stLan.bPassive );
	strReConnectTime.Format( "%01d", m_stLan.nReConnectTime );
	GetDlgItem( IDC_EDT_RECONNECT_TIME )->SetWindowText( strReConnectTime );

	GetDlgItem( IDC_EDT_LINE_CNT )->SetWindowText( "1000" );

	m_bConnect1				    = false;
	m_bConnect2				    = false;

	m_fnSetOpen();

	SetTimer( ELIPSOMETER_CONNECT, m_stLan.nReConnectTime, NULL );
	SetTimer( ELIPSOMETER_COMM, ELIPSOMETER_TIME, NULL );
}

//---------------------------------------------------------------------------------------------------------
// 포트 OPEN.
//---------------------------------------------------------------------------------------------------------
void CElipsoMeterIF::m_fnSetOpen() 
{
	CString strIP("");
	CString strPort1("");
	CString strPort2("");
	int     nPort1 = 0;
	int     nPort2 = 0;
	BOOL bMode = ((CButton*)GetDlgItem( IDC_CHK_MODE ))->GetCheck( );
	GetDlgItem( IDC_EDT_IP )->GetWindowText( strIP );
	GetDlgItem( IDC_EDT_PORT1 )->GetWindowText( strPort1 );
	GetDlgItem( IDC_EDT_PORT2 )->GetWindowText( strPort2 );
	nPort1 = _ttoi( strPort1 );
	nPort2 = _ttoi( strPort2 );

	if ( !bMode )		// CLIENT
	{
		if ( m_pClient1 != NULL )
		{
			delete m_pClient1;
			m_pClient1                   = NULL;
			m_bConnect1                  = false;
			((CButton*)GetDlgItem( IDC_CHK_ONLINE_1 ))->SetCheck( m_bConnect1 );
		}
		m_pClient1 = new CDataClientSocketEx( this, IP_PORT );
		if(!m_pClient1->Create())
		{
			MessageBox(TEXT("소켓 생성에 실패했습니다.(Ip Port)"), TEXT("확인"), MB_OK+MB_ICONEXCLAMATION);
			return;
		}

		m_pClient1->Connect( strIP, nPort1 );
		if ( m_pClient2 != NULL )
		{
			delete m_pClient2;
			m_pClient2                   = NULL;
			m_bConnect2                  = false;
			((CButton*)GetDlgItem( IDC_CHK_ONLINE_2 ))->SetCheck( m_bConnect2 );
		}
		m_pClient2 = new CDataClientSocketEx( this, ASYNC_IP_PORT );
		if(!m_pClient2->Create())
		{
			MessageBox(TEXT("소켓 생성에 실패했습니다.(Async Ip Port)"), TEXT("확인"), MB_OK+MB_ICONEXCLAMATION);
			return;
		}

		m_pClient2->Connect( strIP, nPort2 );
	}
	else				// SERVER
	{
		if ( m_pServer1 != NULL )
		{
			delete m_pServer1;
			m_pServer1 = NULL;
		}
		if ( m_pClient1 != NULL )
		{
			delete m_pClient1;
			m_pClient1                   = NULL;
			m_bConnect1                  = false;
			((CButton*)GetDlgItem( IDC_CHK_ONLINE_1 ))->SetCheck( m_bConnect1 );
		}
		m_pServer1 = new CDataServerSocketEx( IP_PORT );
		m_pServer1->SetOwner(this);
		m_pServer1->Create(nPort1);
		m_pServer1->Listen();

		if ( m_pServer2 != NULL )
		{
			delete m_pServer2;
			m_pServer2 = NULL;
		}
		if ( m_pClient2 != NULL )
		{
			delete m_pClient2;
			m_pClient2                  = NULL;
			m_bConnect2                 = false;
			((CButton*)GetDlgItem( IDC_CHK_ONLINE_2 ))->SetCheck( m_bConnect2 );
		}
		m_pServer2 = new CDataServerSocketEx(ASYNC_IP_PORT);
		m_pServer2->SetOwner(this);
		m_pServer2->Create(nPort2);
		m_pServer2->Listen();

		m_bServerRun = TRUE;
	}
}

//---------------------------------------------------------------------------------------------------------
// 종료에 필요한 작업을 한다.
//---------------------------------------------------------------------------------------------------------
void CElipsoMeterIF::m_fnSetEnd() 
{
	if ( m_hWnd != NULL )
	{
		KillTimer( ELIPSOMETER_CONNECT );
		KillTimer( ELIPSOMETER_COMM );
		m_fnLogWrite( "<<< 통신종료 >>>" );	
	}

	if ( m_pClient1 != NULL )
	{
		delete m_pClient1;
		m_pClient1                   = NULL;
		m_bConnect1                  = false;
	}
	if ( m_pClient2 != NULL )
	{
		delete m_pClient2;
		m_pClient2                   = NULL;
		m_bConnect2                  = false;
	}

	if ( m_pServer1 != NULL )
	{
		delete m_pServer1;
		m_pServer1 = NULL;
	}

	if ( m_pServer2 != NULL )
	{
		delete m_pServer2;
		m_pServer2 = NULL;
	}

	if ( m_pTaskHandle != NULL )
	{
		m_pTaskHandle->m_fnSetEnd();
		delete m_pTaskHandle;
		m_pTaskHandle = NULL;
	}
	if (m_hResultThread != NULL)
	{
		m_bResultEnd = FALSE;
		SuspendThread(m_hResultThread);
		CloseHandle(m_hResultThread);
		m_hResultThread = NULL;
	}

}

void CElipsoMeterIF::OnBnClickedBtnExit()
{
	m_fnSetShowFlag( FALSE );
}

/*-----------------------------------------------------------------------
Func Name : OnReceive
Reference : OnReceive
-----------------------------------------------------------------------*/
LRESULT CElipsoMeterIF::OnReceive(WPARAM nID, LPARAM lParam)
{
	CString strReceiveData1("");
	CString strReceiveData2("");
	CString strMsg("");
	CString strTemp("");
	CString strData("");
	BOOL    bFind1  = true;
	int     nFind1  = -1;
	int     nCount1 = 0;
	BOOL    bFind2  = true;
	int     nFind2  = -1;
	int     nCount2 = 0;
	if ( nID == IP_PORT )
	{
		BYTE byReceiveData1[MAX_BUFF_SIZE] = {0,};
		memset( byReceiveData1, 0x00, sizeof(byReceiveData1) );
		nCount1 = m_pClient1->Receive(byReceiveData1, sizeof(byReceiveData1));
		m_strData1  += (char*)byReceiveData1;
		strMsg.Format( "RECV1 : %s", (char*)byReceiveData1 );
		m_fnLogWrite( strMsg );
		while ( bFind1 )
		{
			nFind1 = m_strData1.Find( LF, 0 );
			if ( nFind1 == -1 )
			{
				bFind1 = false;
			}
			else
			{
				m_strRcvData1.Add( m_strData1.Left( nFind1 + 1 ));
				m_strData1.Delete( 0, nFind1 + 1 );
				m_strData1.FreeExtra();
			}
		}
	}
	else
	{
		BYTE byReceiveData2[MAX_BUFF_SIZE] = {0,};
		memset( byReceiveData2, 0x00, sizeof(byReceiveData2) );
		nCount2 = m_pClient2->Receive(byReceiveData2, sizeof(byReceiveData2));
		m_strData2  += (char*)byReceiveData2;
		strMsg.Format( "RECV2 : %s", (char*)byReceiveData2 );
		m_fnLogWrite( strMsg );
		while ( bFind2 )
		{
			nFind2 = m_strData2.Find( LF, 0 );
			if ( nFind2 == -1 )
			{
				bFind2 = false;
			}
			else
			{
				m_strRcvData2.Add( m_strData2.Left( nFind2 + 1 ));
				m_strData2.Delete( 0, nFind2 + 1 );
				m_strData2.FreeExtra();
			}
		}
	}

	return 0;
}

/*-----------------------------------------------------------------------
Func Name : OnReceive
Reference : OnReceive
-----------------------------------------------------------------------*/
LRESULT CElipsoMeterIF::OnAccept(WPARAM nID, LPARAM lParam)
{
	if ( nID == IP_PORT )
	{
		if ( m_pClient1 != NULL )
		{
			delete m_pClient1;
			m_pClient1 = NULL;
		}

		m_pClient1 = new CDataClientSocketEx( this, IP_PORT );
		if(!m_pServer1->Accept( *m_pClient1 ))
		{
			return 0;
		}
		m_bConnect1                  = TRUE;
		((CButton*)GetDlgItem( IDC_CHK_ONLINE_1 ))->SetCheck( m_bConnect1 );
		m_fnLogWrite( "Accept OK1" );
	}
	if ( nID == ASYNC_IP_PORT )
	{
		if ( m_pClient2 != NULL )
		{
			delete m_pClient2;
			m_pClient2 = NULL;
		}

		m_pClient2 = new CDataClientSocketEx( this, ASYNC_IP_PORT );
		if(!m_pServer2->Accept( *m_pClient2 ))
		{
			return 0;
		}
		m_bConnect2                  = TRUE;
		((CButton*)GetDlgItem( IDC_CHK_ONLINE_2 ))->SetCheck( m_bConnect2 );
		m_fnLogWrite( "Accept OK2" );
	}
	return 0;
}

/*-----------------------------------------------------------------------
Func Name : OnReceive
Reference : OnReceive
-----------------------------------------------------------------------*/
LRESULT CElipsoMeterIF::OnClose(WPARAM nID, LPARAM lParam)
{
	if ( nID == IP_PORT )
	{
		m_bConnect1                  = false;
		if ( m_hWnd != NULL )
		{
			((CButton*)GetDlgItem( IDC_CHK_ONLINE_1 ))->SetCheck( m_bConnect1 );
		}
		m_fnLogWrite( "DisConnected1" );
	}
	if ( nID == ASYNC_IP_PORT )
	{
		m_bConnect2                  = false;
		if ( m_hWnd != NULL )
		{
			((CButton*)GetDlgItem( IDC_CHK_ONLINE_2 ))->SetCheck( m_bConnect2 );
		}
		m_fnLogWrite( "DisConnected2" );
	}
	return 0;
}

/*-----------------------------------------------------------------------
Func Name : OnReceive
Reference : OnReceive
-----------------------------------------------------------------------*/
LRESULT CElipsoMeterIF::OnConnectCheck(WPARAM nID, LPARAM lParam)
{
	if ( nID == IP_PORT )
	{
		((CButton*)GetDlgItem( IDC_CHK_ONLINE_1 ))->SetCheck( m_pClient1->m_bConnect );
		m_bConnect1              = m_pClient1->m_bConnect;
		if ( m_pClient1->m_bConnect )
		{
			m_fnLogWrite( "Connected1" );
		}
		else
		{
			m_fnLogWrite( "DisConnected1" );
		}
	}
	if ( nID == ASYNC_IP_PORT )
	{
		((CButton*)GetDlgItem( IDC_CHK_ONLINE_2 ))->SetCheck( m_pClient2->m_bConnect );
		m_bConnect2              = m_pClient2->m_bConnect;
		if ( m_pClient2->m_bConnect )
		{
			m_fnLogWrite( "Connected2" );
		}
		else
		{
			m_fnLogWrite( "DisConnected2" );
		}
	}
	return 0;
}

void CElipsoMeterIF::m_fnSend1( CString strData )
{
	CString strSendData("");
	if ( !m_bConnect1 )
	{
		strSendData.Format("접속중인 Client1 가 없습니다.(%s)", strData );
		m_fnLogWrite( strSendData );
		return;
	}

	CString strMsg("");

	strSendData += strData;
	strSendData += (char)CR;
	strSendData += (char)LF;

	m_bComplete  = FALSE;
	m_bError     = FALSE;
	m_bYesNo     = FALSE;

	strMsg.Format( "SEND1 : %s", strSendData );
	m_fnLogWrite( strMsg );
	m_pClient1->Send( strSendData.GetBuffer(strSendData.GetLength()), strSendData.GetLength() );

	CheckTimeSend.SetTimeStart();
}

void CElipsoMeterIF::m_fnSend2( CString strData )
{
	CString strSendData("");
	if ( !m_bConnect2 )
	{
		strSendData.Format("접속중인 Client2 가 없습니다.(%s)", strData );
		m_fnLogWrite( strSendData );
		return;
	}

	CString strMsg("");

	strSendData += strData;
	strSendData += (char)CR;
	strSendData += (char)LF;

	strMsg.Format( "SEND2 : %s", strSendData );
	m_fnLogWrite( strMsg );
	m_pClient2->Send( strSendData.GetBuffer(strSendData.GetLength()), strSendData.GetLength() );
}

void CElipsoMeterIF::OnBnClickedBtnOpen()
{
	m_fnSetOpen();
}


void CElipsoMeterIF::OnBnClickedBtnClose()
{
	if(m_pClient1 != NULL)
	{
		delete m_pClient1;
		m_pClient1 = NULL;
	}
	m_bConnect1      = false;
	((CButton*)GetDlgItem( IDC_CHK_ONLINE_1 ))->SetCheck( m_bConnect1 );
	m_fnLogWrite( "DisConnected1" );
	if(m_pClient2 != NULL)
	{
		delete m_pClient2;
		m_pClient2 = NULL;
	}
	m_bConnect2      = false;
	((CButton*)GetDlgItem( IDC_CHK_ONLINE_2 ))->SetCheck( m_bConnect2);
	m_fnLogWrite( "DisConnected2" );
}


void CElipsoMeterIF::OnBnClickedBtnLineSet()
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


void CElipsoMeterIF::OnBnClickedBtnClr()
{
	m_lstComm.ResetContent();	
}

//---------------------------------------------------------------------------------------------------------
// Log 파일을 저장한다.
//---------------------------------------------------------------------------------------------------------
void CElipsoMeterIF::m_fnLogWrite( CString strData, BOOL bWrite )
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
// 		bWrite = m_pTask->stCommon.bLogSave;
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
CString CElipsoMeterIF::m_fnGetELIPSOMETERFileFullPath()
{
	CString strPath("");
	strPath.Format( "%s%s%s%s", m_Util.m_fnGetCurrentDrive(),
								DEFAULT_PATH,
								DEFAULT_ENV_PATH,
								DEFAULT_ELIPSOMETER_FILE );
	return strPath;
}

//------------------------------------------------------------------------
// 설정 환경파일을 저장한다.
//------------------------------------------------------------------------
BOOL CElipsoMeterIF::m_fnSetELIPSOMETER_Save( )
{
	CString strWork("");
	CString strData("");
	CString strFullPath("");

	CStringArray strLst;
	CXmlElement *pElement     = NULL;
	CXmlElement *pRootElement = NULL;

	strFullPath		=	m_fnGetELIPSOMETERFileFullPath();

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

		// LAN PORT2
		strWork.Format( "%01d", m_stLan.nPort2 );
		pXML->DataSet( pElement, LAN_PORT2, strWork );

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
BOOL CElipsoMeterIF::m_fnGetELIPSOMETER_Load( )
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

	strFullPath					=	m_fnGetELIPSOMETERFileFullPath();

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

		// LAN PORT2
		pXML->FindData( pElement, LAN_PORT2, strWork );
		m_stLan.nPort2		= _ttoi( strWork );

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



void CElipsoMeterIF::OnTimer(UINT_PTR nIDEvent)
{
	if ( nIDEvent == ELIPSOMETER_CONNECT )
	{
		if (( !m_bConnect1 ) ||
		    ( !m_bConnect2 ))
		{
			if ( !m_stLan.bPassive )
			{
				m_fnSetOpen();
			}
		}
	}

	if ( nIDEvent == ELIPSOMETER_COMM )
	{
		if ( !m_bProcess)
		{
			m_bProcess = true;
			if ( m_strRcvData1.GetCount() > 0 )
			{
				m_fnAnalysis_Data1();
			}
			if ( m_strRcvData2.GetCount() > 0 )
			{
				m_fnAnalysis_Data2();
			}
			m_bProcess = false;
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CElipsoMeterIF::m_fnAnalysis_Data1()
{
	char tmpCheckTime[256];
	sprintf_s(tmpCheckTime, sizeof(char) * 256, "%d___send: %.1f s", m_nCmdNo, CheckTimeSend.GetTimeInterval(2));
	OutputDebugString(tmpCheckTime);
	CheckTimeSend.SetTimeStart();

	if ( m_strRcvData1.GetCount() == 0 ) return;

	CString strAnalysisData("");
	int     nNo         = 0;
	int		nFind       = -1;
	int		nFindKind   = -1;
	BOOL    bFindOK     = FALSE;
	int     nFindItem = 0;
	int     nFindComma	= 0;
	int     nLayerCnt   = 0;

	int     nFindDisable = 0;
	CString strLayerName("");

	strAnalysisData = m_strRcvData1.GetAt( 0 );
	strAnalysisData.Replace(CRLF, "");

	nFind = strAnalysisData.Find("Error");
	switch (m_nCmdNo)
	{
		case CMD_START_ACQ:
		{
			if (nFind != -1)	// ERROR
			{
				sprintf_s(m_stResult.szErrorMsg, "%s", strAnalysisData.GetString());
				m_bError = TRUE;
				m_bComplete = TRUE;
				GetDlgItem(IDC_EDT_MSG)->SetWindowText(strAnalysisData);
				strAnalysisData.Delete(0, strAnalysisData.GetLength());
				strAnalysisData.FreeExtra();
			}
			else
			{
				GetDlgItem(IDC_EDT_MSG)->SetWindowText(strAnalysisData);
				strAnalysisData.Delete(0, strAnalysisData.GetLength());
				strAnalysisData.FreeExtra();
			}
		}
		break;
		case CMD_LIST_RECIPES:
		{
			if (nFind != -1)	// ERROR
			{
				GetDlgItem(IDC_EDT_MSG)->SetWindowText(strAnalysisData);
				strAnalysisData.Delete(0, strAnalysisData.GetLength());
				strAnalysisData.FreeExtra();
			}
			else
			{
				if (strAnalysisData.IsEmpty())
				{
					m_bError    = FALSE;
					m_bComplete = TRUE;
				}
				else
				{
					bFindOK = TRUE;
					m_strRecipeList.RemoveAll();
					m_strRecipeList.FreeExtra();
					m_clRecipeList.ResetContent();
					while (bFindOK)
					{
						if (!strAnalysisData.IsEmpty())
						{
							nFindKind = strAnalysisData.Find("/");
							if (nFindKind != -1)
							{
								m_strRecipeList.Add(strAnalysisData.Left(nFindKind));
								m_clRecipeList.AddString(strAnalysisData.Left(nFindKind));
								strAnalysisData.Delete(0, nFindKind + 1);
								strAnalysisData.FreeExtra();
							}
							else
							{
								m_strRecipeList.Add(strAnalysisData);
								m_clRecipeList.AddString(strAnalysisData);
								strAnalysisData.Delete(0, strAnalysisData.GetLength());
								strAnalysisData.FreeExtra();
							}
						}
						else
						{
							bFindOK = FALSE;
						}
					}
					sprintf_s(m_stResult.szErrorMsg, "%s", strAnalysisData.GetString());
					m_bError    = FALSE;
					m_bComplete = TRUE;
				}
			}
		}
		break;
		case CMD_SYSTEM_GET_RECEIVER_CAMERA_ALIGN:
		{
			if (nFind != -1)	// ERROR
			{
				sprintf_s(m_stResult.szErrorMsg, "%s", strAnalysisData.GetString());
				m_bError = TRUE;
				m_bComplete = TRUE;
				GetDlgItem(IDC_EDT_MSG)->SetWindowText(strAnalysisData);
				strAnalysisData.Delete(0, strAnalysisData.GetLength());
				strAnalysisData.FreeExtra();
			}
			else
			{
				GetDlgItem(IDC_EDT_MSG)->SetWindowText(strAnalysisData);
				strAnalysisData.Trim();
				m_dAlignZ = atof(strAnalysisData);
				m_bError = FALSE;
				m_bComplete = TRUE;
				strAnalysisData.Delete(0, strAnalysisData.GetLength());
				strAnalysisData.FreeExtra();
			}
		}
		break;
		case CMD_LOAD_RECIPE:
		{
			if (nFind != -1)	// ERROR
			{
				sprintf_s(m_stResult.szErrorMsg, "%s", strAnalysisData.GetString());
				m_bError = TRUE;
				m_bComplete = TRUE;
				GetDlgItem(IDC_EDT_MSG)->SetWindowText(strAnalysisData);
				strAnalysisData.Delete(0, strAnalysisData.GetLength());
				strAnalysisData.FreeExtra();
			}
			else
			{
				if (strAnalysisData == STR_OK)
				{
					m_bError    = FALSE;
					m_bComplete = TRUE;
				}
				else
				{
					sprintf_s(m_stResult.szErrorMsg, "%s", strAnalysisData.GetString());
					m_bError    = TRUE;
					m_bComplete = TRUE;
				}
				GetDlgItem(IDC_EDT_MSG)->SetWindowText(strAnalysisData);
				strAnalysisData.Delete(0, strAnalysisData.GetLength());
				strAnalysisData.FreeExtra();
			}
		}
		break;
		case CMD_RUN_RECIPE:
		{
			m_strMSE            = _T("");
			m_strAngleOffset    = _T("");
			m_strTotalThickness = _T("");
			if (nFind != -1)	// ERROR
			{
				sprintf_s(m_stResult.szErrorMsg, "%s", strAnalysisData.GetString());
				m_bError    = TRUE;
				m_bComplete = TRUE;
				GetDlgItem(IDC_EDT_MSG)->SetWindowText(strAnalysisData);
				strAnalysisData.Delete(0, strAnalysisData.GetLength());
				strAnalysisData.FreeExtra();
			}
			else
			{
				m_strResult.RemoveAll();
				m_strResult.FreeExtra();
				GetDlgItem(IDC_EDT_MSG)->SetWindowText(strAnalysisData);
				m_strAnalysisData = strAnalysisData;
				BOOL bEnd = TRUE;
				while (bEnd)
				{
					if (!strAnalysisData.IsEmpty())
					{
						nFindComma = strAnalysisData.Find(",");
						if (nFindComma != -1)
						{
							m_strResult.Add(strAnalysisData.Left(nFindComma));
							strAnalysisData.Delete(0, nFindComma + 1);
							strAnalysisData.FreeExtra();
						}
						else
						{
							m_strResult.Add(strAnalysisData);
							strAnalysisData.Delete(0, strAnalysisData.GetLength());
							strAnalysisData.FreeExtra();
							bEnd = FALSE;
						}
					}
					else
					{
						bEnd = FALSE;
					}
				}
				CString strWork("");
				CString strResult("");
				int nFindResult = -1;
				if (!m_bError)
				{
					m_fnGetSearch(R_MSE, strResult, nFindResult);
					if (nFindResult != -1)
					{
						sprintf_s(m_stResult.szMSE, "%s", strResult.GetString());
						m_stResult.dMSE = atof(m_fnGetDelete(strResult.Right(nFindResult)));
					}
					else
					{
						m_bError = TRUE;
						m_bComplete = TRUE;
					}
				}
				if (!m_bError)
				{
					m_fnGetSearch(R_TOTAL_THICKNESS, strResult, nFindResult);
					if (nFindResult != -1)
					{
						sprintf_s(m_stResult.szTotalThickness, "%s", strResult.GetString());
						m_stResult.dTotalThickness = atof(m_fnGetDelete(strResult.Right(nFindResult)));
					}
					else
					{
						m_bError = TRUE;
						m_bComplete = TRUE;
					}
				}


				if (!m_bError)
				{
					m_fnGetSearch(R_ROUGHNESS, strResult, nFindResult);
					if (nFindResult != -1)
					{
						sprintf_s(m_stResult.szRoughness, "%s", strResult.GetString());
						m_stResult.dRoughness = atof(m_fnGetDelete(strResult.Right(nFindResult)));

// 						m_stResult.nLayerCnt = m_stModelStruct.nLayerCnt - 1 ;
					}
					else
					{
						m_stResult.dRoughness = 0.0;
// 						m_bError = TRUE;
// 						m_bComplete = TRUE;
// 						m_stResult.nLayerCnt = m_stModelStruct.nLayerCnt;
					}
				}
				m_stResult.nLayerCnt = m_stModelStruct.nLayerCnt;

				for (int i = 0; i < m_stResult.nLayerCnt; i++)
				{
					strWork.Format(R_THICKNESS, i + 1);
					m_fnGetSearch(strWork, strResult, nFindResult);
					if (nFindResult != -1)
					{
						sprintf_s(m_stResult.stLayer[i].szThickness, "%s", strResult.GetString());
						m_stResult.stLayer[i].dThickness = atof(m_fnGetDelete(strResult.Right(nFindResult)));
					}
					else
					{
						m_stResult.stLayer[i].dThickness = 0.0;
// 						m_bError = TRUE;
// 						m_bComplete = TRUE;
						continue;
					}
					strWork.Format(R_N_OF_550, m_stModelStruct.szLayerItem[i]);
					m_fnGetSearch(strWork, strResult, nFindResult);
					if (nFindResult != -1)
					{
						sprintf_s(m_stResult.stLayer[i].szN_550, "%s", strResult.GetString());
						m_stResult.stLayer[i].dN_550 = atof(m_fnGetDelete(strResult.Right(nFindResult)));
					}
					else
					{
						m_stResult.stLayer[i].dN_550 = 0.0;
// 						m_bError = TRUE;
// 						m_bComplete = TRUE;
// 						continue;
					}
					strWork.Format(R_K_OF_550, m_stModelStruct.szLayerItem[i]);
					m_fnGetSearch(strWork, strResult, nFindResult);
					if (nFindResult != -1)
					{
						sprintf_s(m_stResult.stLayer[i].szK_550, "%s", strResult.GetString());
						m_stResult.stLayer[i].dK_550 = atof(m_fnGetDelete(strResult.Right(nFindResult)));
					}
					else
					{
						m_stResult.stLayer[i].dK_550 = 0.0;
// 						m_bError = TRUE;
// 						m_bComplete = TRUE;
// 						continue;
					}
					strWork.Format(R_N_OF_632, m_stModelStruct.szLayerItem[i]);
					m_fnGetSearch(strWork, strResult, nFindResult);
					if (nFindResult != -1)
					{
						sprintf_s(m_stResult.stLayer[i].szN_632, "%s", strResult.GetString());
						m_stResult.stLayer[i].dN_632 = atof(m_fnGetDelete( strResult.Right(nFindResult)));
					}
					else
					{
						m_stResult.stLayer[i].dN_632 = 0.0;
// 						m_bError = TRUE;
// 						m_bComplete = TRUE;
// 						continue;
					}
					strWork.Format(R_K_OF_632, m_stModelStruct.szLayerItem[i]);
					m_fnGetSearch(strWork, strResult, nFindResult);
					if (nFindResult != -1)
					{
						sprintf_s(m_stResult.stLayer[i].szK_632, "%s", strResult.GetString());
						m_stResult.stLayer[i].dK_632 = atof(m_fnGetDelete(strResult.Right(nFindResult)));
					}
					else
					{
						m_stResult.stLayer[i].dK_632 = 0.0;
// 						m_bError = TRUE;
// 						m_bComplete = TRUE;
// 						continue;
					}
				}
				if (!m_bError)
				{
					m_bComplete = TRUE;
				}
			}
		}
		break;
		case CMD_STATUS:
		{
			if (nFind != -1)	
			{
				sprintf_s(m_stResult.szErrorMsg, "%s", strAnalysisData.GetString());
				m_bError = TRUE;
				m_bComplete = TRUE;
				GetDlgItem(IDC_EDT_MSG)->SetWindowText(strAnalysisData);
				strAnalysisData.Delete(0, strAnalysisData.GetLength());
				strAnalysisData.FreeExtra();
			}
			else
			{
				if (strAnalysisData == WAITING_TO_ACQUIRE_DATA)
				{
					m_bError = FALSE;
					m_bComplete = TRUE;
				}
				else
				{
					sprintf_s(m_stResult.szErrorMsg, "%s", strAnalysisData.GetString());
					m_bError = TRUE;
					m_bComplete = TRUE;
				}
				GetDlgItem(IDC_EDT_MSG)->SetWindowText(strAnalysisData);
				strAnalysisData.Delete(0, strAnalysisData.GetLength());
				strAnalysisData.FreeExtra();
			}
		}
		break;
		case CMD_INIT_STATUS:
		{
			if (nFind != -1)	// ERROR
			{
				sprintf_s(m_stResult.szErrorMsg, "%s", strAnalysisData.GetString());
				m_bError = TRUE;
				m_bComplete = TRUE;
				GetDlgItem(IDC_EDT_MSG)->SetWindowText(strAnalysisData);
				strAnalysisData.Delete(0, strAnalysisData.GetLength());
				strAnalysisData.FreeExtra();
			}
			else
			{
				if (strAnalysisData == HARDWARE_INITIALIZED)
				{
					m_bError = FALSE;
					m_bComplete = TRUE;
				}
				else
				{
					sprintf_s(m_stResult.szErrorMsg, "%s", strAnalysisData.GetString());
					m_bError = TRUE;
					m_bComplete = TRUE;
				}
				GetDlgItem(IDC_EDT_MSG)->SetWindowText(strAnalysisData);
				strAnalysisData.Delete(0, strAnalysisData.GetLength());
				strAnalysisData.FreeExtra();
			}
		}
		break;
		case CMD_SYSTEM_LAMP_SHUTTER_OPEN:
		{
			if (nFind != -1)	// ERROR
			{
				sprintf_s(m_stResult.szErrorMsg, "%s", strAnalysisData.GetString());
				m_bError    = TRUE;
				m_bComplete = TRUE;
				GetDlgItem(IDC_EDT_MSG)->SetWindowText(strAnalysisData);
				strAnalysisData.Delete(0, strAnalysisData.GetLength());
				strAnalysisData.FreeExtra();
			}
			else
			{
				if (strAnalysisData == STR_OK )
				{
					m_bError = FALSE;
					m_bComplete = TRUE;
				}
				else
				{
					sprintf_s(m_stResult.szErrorMsg, "%s", strAnalysisData.GetString());
					m_bError = TRUE;
					m_bComplete = TRUE;
				}
				GetDlgItem(IDC_EDT_MSG)->SetWindowText(strAnalysisData);
				strAnalysisData.Delete(0, strAnalysisData.GetLength());
				strAnalysisData.FreeExtra();
			}
		}
		break;
		case CMD_SYSTEM_LAMP_SHUTTER_CLOSED:
		{
			if (nFind != -1)	// ERROR
			{
				sprintf_s(m_stResult.szErrorMsg, "%s", strAnalysisData.GetString());
				m_bError = TRUE;
				m_bComplete = TRUE;
				GetDlgItem(IDC_EDT_MSG)->SetWindowText(strAnalysisData);
				strAnalysisData.Delete(0, strAnalysisData.GetLength());
				strAnalysisData.FreeExtra();
			}
			else
			{
				if (strAnalysisData == STR_OK)
				{
					m_bError = FALSE;
					m_bComplete = TRUE;
				}
				else
				{
					sprintf_s(m_stResult.szErrorMsg, "%s", strAnalysisData.GetString());
					m_bError = TRUE;
					m_bComplete = TRUE;
				}
				GetDlgItem(IDC_EDT_MSG)->SetWindowText(strAnalysisData);
				strAnalysisData.Delete(0, strAnalysisData.GetLength());
				strAnalysisData.FreeExtra();
			}
		}
		break;
		case CMD_SYSTEM_FILTER_WHEEL_AUTO_ON:
		{
			if (nFind != -1)	// ERROR
			{
				sprintf_s(m_stResult.szErrorMsg, "%s", strAnalysisData.GetString());
				m_bError    = TRUE;
				m_bComplete = TRUE;
				GetDlgItem(IDC_EDT_MSG)->SetWindowText(strAnalysisData);
				strAnalysisData.Delete(0, strAnalysisData.GetLength());
				strAnalysisData.FreeExtra();
			}
			else
			{
				if (strAnalysisData == STR_OK)
				{
					m_bError = FALSE;
					m_bComplete = TRUE;
				}
				else
				{
					sprintf_s(m_stResult.szErrorMsg, "%s", strAnalysisData.GetString());
					m_bError = TRUE;
					m_bComplete = TRUE;
				}
				GetDlgItem(IDC_EDT_MSG)->SetWindowText(strAnalysisData);
				strAnalysisData.Delete(0, strAnalysisData.GetLength());
				strAnalysisData.FreeExtra();
			}
		}
		break;
		case CMD_SYSTEM_FILTER_WHEEL_AUTO_OFF:
		{
			if (nFind != -1)	// ERROR
			{
				sprintf_s(m_stResult.szErrorMsg, "%s", strAnalysisData.GetString());
				m_bError    = TRUE;
				m_bComplete = TRUE;
				GetDlgItem(IDC_EDT_MSG)->SetWindowText(strAnalysisData);
				strAnalysisData.Delete(0, strAnalysisData.GetLength());
				strAnalysisData.FreeExtra();
			}
			else
			{
				if (strAnalysisData == STR_OK)
				{
					m_bError = FALSE;
					m_bComplete = TRUE;
				}
				else
				{
					sprintf_s(m_stResult.szErrorMsg, "%s", strAnalysisData.GetString());
					m_bError = TRUE;
					m_bComplete = TRUE;
				}
				GetDlgItem(IDC_EDT_MSG)->SetWindowText(strAnalysisData);
				strAnalysisData.Delete(0, strAnalysisData.GetLength());
				strAnalysisData.FreeExtra();
			}
		}
		break;
		case CMD_SYSTEM_GET_LAMP_HOURS:
		{
			if (nFind != -1)	// ERROR
			{
				sprintf_s(m_stResult.szErrorMsg, "%s", strAnalysisData.GetString());
				m_bError    = TRUE;
				m_bComplete = TRUE;
				GetDlgItem(IDC_EDT_MSG)->SetWindowText(strAnalysisData);
				strAnalysisData.Delete(0, strAnalysisData.GetLength());
				strAnalysisData.FreeExtra();
			}
			else
			{
				GetDlgItem(IDC_EDT_MSG)->SetWindowText(strAnalysisData);
				nFindItem = strAnalysisData.Find("TMR");
				if (nFindItem != -1)
				{
					strAnalysisData.Delete(0, 3);
					strAnalysisData.FreeExtra();
					strAnalysisData.Trim();
					m_dLampHour = atof(strAnalysisData);
					strAnalysisData.Delete(0, strAnalysisData.GetLength());
					strAnalysisData.FreeExtra();
					m_bError = FALSE;
					m_bComplete = TRUE;
				}
				else
				{
					m_bError = TRUE;
					m_bComplete = TRUE;
				}
			}
		}
		break;
		case CMD_GET_MODEL_STRUCTURE:
		{
			if (nFind != -1)	// ERROR
			{
				sprintf_s(m_stResult.szErrorMsg, "%s", strAnalysisData.GetString());
				m_bError = TRUE;
				m_bComplete = TRUE;
				GetDlgItem(IDC_EDT_MSG)->SetWindowText(strAnalysisData);
				strAnalysisData.Delete(0, strAnalysisData.GetLength());
				strAnalysisData.FreeExtra();
			}
			else
			{
				GetDlgItem(IDC_EDT_MSG)->SetWindowText(strAnalysisData);
				nFindItem = strAnalysisData.Find("\\");
				if (nFindItem != -1)
				{
					nFindDisable = strAnalysisData.Find("SROUGH");
					if (nFindDisable != -1)
					{
						// LAYER는 2개가 불필요하여 5로 들어올경우 3 LAYTER로 처리
						nLayerCnt = atoi(strAnalysisData.Left(nFindItem)) - 2;
						strAnalysisData.Delete(0, nFindItem + 1);
						strAnalysisData.FreeExtra();
					}
					else
					{
						// LAYER는 1개가 불필요하여 5로 들어올경우 4 LAYTER로 처리
						nLayerCnt = atoi(strAnalysisData.Left(nFindItem)) - 1;
						strAnalysisData.Delete(0, nFindItem + 1);
						strAnalysisData.FreeExtra();
					}

				}
				else
				{
					m_bError = TRUE;
					m_bComplete = TRUE;
					strAnalysisData.Delete(0, strAnalysisData.GetLength());
					strAnalysisData.FreeExtra();
				}
				if (nLayerCnt > 0)
				{
					nFindDisable = strAnalysisData.Find("SROUGH");
					if (nFindDisable != -1)
					{
						// ROUGHNESS는 불필요하여 SKIP
						nFindItem = strAnalysisData.Find("\\");
						if (nFindItem != -1)
						{
							strAnalysisData.Delete(0, nFindItem + 1);
							strAnalysisData.FreeExtra();
						}
						nFindItem = strAnalysisData.Find("\\");
						if (nFindItem != -1)
						{
							strAnalysisData.Delete(0, nFindItem + 1);
							strAnalysisData.FreeExtra();
						}
					}
					m_stModelStruct.nLayerCnt = nLayerCnt;
					for (int i = 0; i < nLayerCnt; i++)
					{
						nFindItem = strAnalysisData.Find("\\");
						if (nFindItem != -1)
						{
							strLayerName = strAnalysisData.Left(nFindItem);

							nFindDisable = strLayerName.Find("-GRADED");
							if (nFindDisable != -1)
							{
								strLayerName.Delete(nFindDisable, 7);
								strLayerName.FreeExtra();
							}
							sprintf_s(m_stModelStruct.szLayerItem[nLayerCnt - i - 1], "%s", strLayerName.GetString());// -1;
							strAnalysisData.Delete(0, nFindItem + 1);
							strAnalysisData.FreeExtra();
						}
						else
						{
							m_bError = TRUE;
							m_bComplete = TRUE;
							break;
						}
						nFindItem = strAnalysisData.Find("\\");
						if (nFindItem != -1)
						{
							strAnalysisData.Delete(0, nFindItem + 1);
							strAnalysisData.FreeExtra();
						}
						else
						{
							m_bError = TRUE;
							m_bComplete = TRUE;
							break;
						}
					}
					if (!m_bError)
					{
						m_stModelStruct.nLayerCnt = nLayerCnt;
					}
					m_stModelStruct.bError =  m_bError;
					m_stModelStruct.bComplete = true;
					m_bComplete = TRUE;
					strAnalysisData.Delete(0, strAnalysisData.GetLength());
					strAnalysisData.FreeExtra();
				}
				else
				{
					m_bError = TRUE;
					m_bComplete = TRUE;
					m_strTotalThickness = strAnalysisData;
					strAnalysisData.Delete(0, strAnalysisData.GetLength());
					strAnalysisData.FreeExtra();
				}
			}
		}
		break;
		case CMD_SYSTEM_LAMP_ON:
		{
			if (nFind != -1)	// ERROR
			{
				sprintf_s(m_stResult.szErrorMsg, "%s", strAnalysisData.GetString());
				m_bError = TRUE;
				m_bComplete = TRUE;
				GetDlgItem(IDC_EDT_MSG)->SetWindowText(strAnalysisData);
				strAnalysisData.Delete(0, strAnalysisData.GetLength());
				strAnalysisData.FreeExtra();
			}
			else
			{
				if (strAnalysisData == STR_OK)
				{
					m_bError = FALSE;
					m_bComplete = TRUE;
				}
				else
				{
					sprintf_s(m_stResult.szErrorMsg, "%s", strAnalysisData.GetString());
					m_bError = TRUE;
					m_bComplete = TRUE;
				}
				GetDlgItem(IDC_EDT_MSG)->SetWindowText(strAnalysisData);
				strAnalysisData.Delete(0, strAnalysisData.GetLength());
				strAnalysisData.FreeExtra();
			}
		}
		break;
		case CMD_SYSTEM_LAMP_OFF:
		{
			if (nFind != -1)	// ERROR
			{
				sprintf_s(m_stResult.szErrorMsg, "%s", strAnalysisData.GetString());
				m_bError = TRUE;
				m_bComplete = TRUE;
				GetDlgItem(IDC_EDT_MSG)->SetWindowText(strAnalysisData);
				strAnalysisData.Delete(0, strAnalysisData.GetLength());
				strAnalysisData.FreeExtra();
			}
			else
			{
				if (strAnalysisData == STR_OK)
				{
					m_bError = FALSE;
					m_bComplete = TRUE;
				}
				else
				{
					sprintf_s(m_stResult.szErrorMsg, "%s", strAnalysisData.GetString());
					m_bError = TRUE;
					m_bComplete = TRUE;
				}
				GetDlgItem(IDC_EDT_MSG)->SetWindowText(strAnalysisData);
				strAnalysisData.Delete(0, strAnalysisData.GetLength());
				strAnalysisData.FreeExtra();
			}
		}
		break;
		case CMD_SYSTEM_LAMP_IS_ON:
		{
			if (nFind != -1)	// ERROR
			{
				sprintf_s(m_stResult.szErrorMsg, "%s", strAnalysisData.GetString());
				m_bError    = TRUE;
				m_bYesNo    = FALSE;
				m_bComplete = TRUE;
				GetDlgItem(IDC_EDT_MSG)->SetWindowText(strAnalysisData);
				strAnalysisData.Delete(0, strAnalysisData.GetLength());
				strAnalysisData.FreeExtra();
			}
			else
			{
				if (strAnalysisData == STR_YES)
				{
					m_bError    = FALSE;
					m_bComplete = TRUE;
					m_bYesNo    = TRUE;
				}
				else
				{
					if (strAnalysisData == STR_NO)
					{
						m_bError    = FALSE;
						m_bComplete = TRUE;
						m_bYesNo    = FALSE;
					}
					else
					{
						sprintf_s(m_stResult.szErrorMsg, "%s", strAnalysisData.GetString());
						m_bError    = TRUE;
						m_bComplete = TRUE;
						m_bYesNo    = FALSE;
					}
				}
				GetDlgItem(IDC_EDT_MSG)->SetWindowText(strAnalysisData);
				strAnalysisData.Delete(0, strAnalysisData.GetLength());
				strAnalysisData.FreeExtra();
			}
		}
		break;
		case CMD_SYSTEM_LAMP_POWER_IS_ON:
		{
			if (nFind != -1)	// ERROR
			{
				sprintf_s(m_stResult.szErrorMsg, "%s", strAnalysisData.GetString());
				m_bError = TRUE;
				m_bYesNo = FALSE;
				m_bComplete = TRUE;
				GetDlgItem(IDC_EDT_MSG)->SetWindowText(strAnalysisData);
				strAnalysisData.Delete(0, strAnalysisData.GetLength());
				strAnalysisData.FreeExtra();
			}
			else
			{
				if (strAnalysisData == STR_YES)
				{
					m_bError    = FALSE;
					m_bComplete = TRUE;
					m_bYesNo    = TRUE;
				}
				else
				{
					if (strAnalysisData == STR_NO)
					{
						m_bError    = FALSE;
						m_bComplete = TRUE;
						m_bYesNo    = FALSE;
					}
					else
					{
						sprintf_s(m_stResult.szErrorMsg, "%s", strAnalysisData.GetString());
						m_bError    = TRUE;
						m_bComplete = TRUE;
						m_bYesNo    = FALSE;
					}
				}
				GetDlgItem(IDC_EDT_MSG)->SetWindowText(strAnalysisData);
				strAnalysisData.Delete(0, strAnalysisData.GetLength());
				strAnalysisData.FreeExtra();
			}
		}
		break;
	}
	m_strRcvData1.RemoveAt( 0 );

	sprintf_s(tmpCheckTime, sizeof(char) * 256, "%d___Anal_Data: %.1f s", m_nCmdNo, CheckTimeSend.GetTimeInterval(2));
	OutputDebugString(tmpCheckTime);

}

CString CElipsoMeterIF::m_fnGetDelete(CString strItem )
{
	int nFind = strItem.Find(" ");
	if (nFind != -1)
	{
		return strItem.Left(nFind);
	}
	return strItem;
}
void CElipsoMeterIF::m_fnGetSearch( CString strItem, CString &strResult, int &nFindResult )
{
	CString strWork("");
	int     nFind = -1;
	strResult     = _T("");
	nFindResult   = -1;
	for (int i = 0; i < m_strResult.GetCount(); i++)
	{
		strWork = m_strResult.GetAt(i);
		nFind   = strWork.Find(strItem);
		if (nFind != -1)
		{
			strResult   = strWork;
			nFindResult = strResult.GetLength() - strItem.GetLength();
			break;
		}
	}
}

void CElipsoMeterIF::m_fnAnalysis_Data2()
{
	if ( m_strRcvData2.GetCount() == 0 ) return;

	CString strAnalysisData("");
	int     nCmd      = 0;
	int     nNo     = 0;
	BOOL    bFindOK = FALSE;

	strAnalysisData = m_strRcvData2.GetAt( 0 );
	strAnalysisData.Replace(CRLF, "");

	GetDlgItem(IDC_EDT_MSG2)->SetWindowText(strAnalysisData);
	strAnalysisData.Delete(0, strAnalysisData.GetLength());

	strAnalysisData.FreeExtra();

	m_strRcvData2.RemoveAt( 0 );
}

void CElipsoMeterIF::m_fnGetRecipeList(t_GET_RECIPE& stRcpList)
{
	for (int i = 0; i < m_strRecipeList.GetCount(); i++)
	{
		sprintf_s(stRcpList.szRecipeList[i], "%s", m_strRecipeList.GetAt(i).GetString());
	}
	stRcpList.nRecipeCnt = m_strRecipeList.GetCount();
	stRcpList.bComplete = m_bComplete;
	stRcpList.bError = m_bError;
}

void CElipsoMeterIF::m_fnGetLampHour(t_GET_LAMP_HOUR& stLampHour)
{
	stLampHour.dLampHour = m_dLampHour;
	stLampHour.bComplete = m_bComplete;
	stLampHour.bError = m_bError;
}

void CElipsoMeterIF::OnBnClickedBtnLanSend()
{
	CString strSend("");
	GetDlgItem( IDC_EDT_DATA )->GetWindowText( strSend );
	if ( !strSend.IsEmpty())
	{
		m_fnSend1( strSend );
	}
}

void CElipsoMeterIF::OnBnClickedBtnSaveData()
{
	CString strIp("");
	CString strPort1("");
	CString strPort2("");
	CString strReconnectTime("");
	strPort1.Format( "%01d", m_stLan.nPort1 );
	strPort2.Format( "%01d", m_stLan.nPort2 );
	GetDlgItem( IDC_EDT_IP )->GetWindowText( strIp  );
	sprintf_s(m_stLan.szIP, sizeof(m_stLan.szIP), "%s", strIp.GetString());
	GetDlgItem( IDC_EDT_PORT1 )->GetWindowText( strPort1 );
	m_stLan.nPort1     = _ttoi( strPort1 );
	GetDlgItem( IDC_EDT_PORT2 )->GetWindowText( strPort2 );
	m_stLan.nPort2     = _ttoi( strPort2 );
	m_stLan.bPassive = ((CButton*)GetDlgItem( IDC_CHK_MODE ))->GetCheck(  );
	GetDlgItem( IDC_EDT_RECONNECT_TIME )->GetWindowText( strReconnectTime );
	m_stLan.nReConnectTime     = _ttoi( strReconnectTime );

	m_fnSetELIPSOMETER_Save();
}



void CElipsoMeterIF::OnBnClickedBtnStartAcq()
{
	m_fnSend1( START_ACQ );
	m_nCmdNo = CMD_START_ACQ;
}


void CElipsoMeterIF::OnBnClickedBtnListRecipes()
{
	m_fnGetListRecipes();
}

void CElipsoMeterIF::m_fnGetListRecipes()
{
//	m_bComplete = FALSE;
//	m_bError = FALSE;
	m_fnSend1(LIST_RECIPES);
	m_nCmdNo = CMD_LIST_RECIPES;
}

void CElipsoMeterIF::OnBnClickedBtnRunRecipe()
{
	CString strRecipeName("");
	int nNo = m_clRecipeList.GetCurSel();
	if (nNo == -1)
	{
		return;
	}
	m_clRecipeList.GetText(nNo, strRecipeName);
	m_fnSetRunRecipe(strRecipeName);
}

void CElipsoMeterIF::m_fnSetRunRecipe(CString strRecipeName)
{
	CString strSendMsg("");
	memset(&m_stResult, 0x00, sizeof(t_ELIPSO_RESULT_INFO));

	CString strDate = theApp.m_fnGetViewPointer()->m_pAuto->m_stUser.szDate;
	CString strLotStartTime = theApp.m_fnGetViewPointer()->m_pAuto->m_strLotStartTime;
	CString strLotID = theApp.m_fnGetViewPointer()->m_pAuto->m_stUser.szLotID;
	CString strWaperNo;
	theApp.m_fnGetViewPointer()->m_pAuto->m_ctrlEditWaferNum.GetWindowTextA(strWaperNo);

	CString strFullPath;
	strFullPath.Format("D:\\%sSE data\\%s\\%s_%s\\%s\\",
		DEFAULT_RESULT_PATH, strDate, strLotStartTime, strLotID, strWaperNo); //저장될 경로 /RESULT/SE Data/날짜/시간_LOTID/웨이퍼번호
	sprintf_s(theApp.m_fnGetViewPointer()->m_pAuto->m_stUser.szDataFile, "%s", strFullPath.GetString());
	
	strSendMsg.Format(RUN_RECIPE
		, strRecipeName//로딩 레시피명
		, strFullPath //저장될 경로
		, m_Util.m_fnGetCurrentTime1()); //
	m_fnSend1(strSendMsg);
	m_nCmdNo = CMD_RUN_RECIPE;
	m_strRunRecipe = strRecipeName;
}


void CElipsoMeterIF::OnBnClickedBtnLoadRecipe()
{
#if MIL_USE
	CString strRecipeName("");
	int nNo = m_clRecipeList.GetCurSel();
	if (nNo == -1)
	{
		return;
	}
	m_clRecipeList.GetText(nNo, strRecipeName);
	m_fnSetLoadRecipe(strRecipeName);

#if	ELLIPSO	
	pWorkRun->m_strRecipeName = strRecipeName;
#endif
#endif
}

void CElipsoMeterIF::m_fnSetLoadRecipe(CString strRecipeName)
{
	CString strSendMsg("");
	strSendMsg.Format(LOAD_RECIPE, strRecipeName);
	m_fnSend1(strSendMsg);
	m_nCmdNo = CMD_LOAD_RECIPE;
	m_strRunRecipe = strRecipeName;
}



void CElipsoMeterIF::OnBnClickedBtnStatus()
{
	m_fnGetStatus();
}

void CElipsoMeterIF::m_fnGetStatus()
{
	m_fnSend1(STATUS);
	m_nCmdNo = CMD_STATUS;
}

void CElipsoMeterIF::OnBnClickedBtnInitStatus()
{
	m_fnGetInitStatus();
}

void CElipsoMeterIF::m_fnGetInitStatus()
{
	m_fnSend1(INIT_STATUS);
	m_nCmdNo = CMD_INIT_STATUS;
}

void CElipsoMeterIF::OnBnClickedBtnGetModelStructure()
{
	m_fnGetModelStructure();
}

void CElipsoMeterIF::m_fnGetModelStructure()
{
	memset( &m_stModelStruct, 0x00, sizeof(t_MODEL_STRUCT) );
	m_fnSend1(GET_MODEL_STRUCTURE);
	m_nCmdNo = CMD_GET_MODEL_STRUCTURE;
}

void CElipsoMeterIF::OnBnClickedBtnSystemGetLampHours()
{
	m_fnGetSystemGetLampHours();
}

void CElipsoMeterIF::m_fnGetSystemGetLampHours()
{
	m_fnSend1(SYSTEM_GET_LAMP_HOURS);
	m_nCmdNo = CMD_SYSTEM_GET_LAMP_HOURS;
}

void CElipsoMeterIF::OnBnClickedBtnSystemLampShutterOpen()
{
	m_fnSetSystemLampShutterOpen();
}

void CElipsoMeterIF::m_fnSetSystemLampShutterOpen()
{
	m_fnSend1(SYSTEM_LAMP_SHUTTER_OPEN);
	m_nCmdNo = CMD_SYSTEM_LAMP_SHUTTER_OPEN;
}

void CElipsoMeterIF::OnBnClickedBtnSystemFilterWheelAutoOn()
{
	m_fnSetSystemFilterWheelAutoOn();
}

void CElipsoMeterIF::m_fnSetSystemFilterWheelAutoOn()
{
	m_fnSend1(SYSTEM_FILTER_WHEEL_AUTO_ON);
	m_nCmdNo = CMD_SYSTEM_FILTER_WHEEL_AUTO_ON;
}

void CElipsoMeterIF::OnBnClickedBtnSystemLampShutterClosed()
{
	m_fnSetSystemLampShutterClosed();
}

void CElipsoMeterIF::m_fnSetSystemLampShutterClosed()
{
	m_fnSend1(SYSTEM_LAMP_SHUTTER_CLOSED);
	m_nCmdNo = CMD_SYSTEM_LAMP_SHUTTER_CLOSED;
}

void CElipsoMeterIF::OnBnClickedBtnSystemFilterWheelAutoOff()
{
	m_fnSetSystemFilterWheelAutoOff();
}

void CElipsoMeterIF::m_fnSetSystemFilterWheelAutoOff()
{
	m_fnSend1(SYSTEM_FILTER_WHEEL_AUTO_OFF);
	m_nCmdNo = CMD_SYSTEM_FILTER_WHEEL_AUTO_OFF;
}


void CElipsoMeterIF::OnBnClickedBtnSystemGetReceiverCameraAlign()
{
	m_fnGetSystemGetReceiverCameraAlign();
}

void CElipsoMeterIF::m_fnGetSystemGetReceiverCameraAlign()
{
// 	m_bComplete = m_bError = FALSE;
	m_fnSend1(SYSTEM_GET_RECEIVER_CAMERA_ALIGN);
	m_nCmdNo = CMD_SYSTEM_GET_RECEIVER_CAMERA_ALIGN;
}

void CElipsoMeterIF::m_fnSetLoadRecipe()
{
	memset(&m_stResult, 0x00, sizeof(t_ELIPSO_RESULT_INFO));

	if (m_hResultThread != NULL)
	{
		m_bProcess = FALSE;
		m_nStep = -1;
		m_bResultEnd = FALSE;
		SuspendThread(m_hResultThread);
		CloseHandle(m_hResultThread);
		m_hResultThread = NULL;
	}

	m_nStep = 10;
	m_bResultEnd = TRUE;

	thread thrdLoadRecipe = thread(LoadRecipeThread, this);
	thrdLoadRecipe.detach();
}

void CElipsoMeterIF::m_fnSetReadyEllipso()
{
	memset(&m_stResult, 0x00, sizeof(t_ELIPSO_RESULT_INFO));

	if (m_hResultThread != NULL)
	{
		m_bProcess = FALSE;
		m_nStep = -1;
		m_bResultEnd = FALSE;
		SuspendThread(m_hResultThread);
		CloseHandle(m_hResultThread);
		m_hResultThread = NULL;
	}

	m_nStep = 10;
	m_bResultEnd = TRUE;

	thread thrdReadyEllipso = thread(ReadyThread, this);
	thrdReadyEllipso.detach();
}

void CElipsoMeterIF::m_fnGetAlignZ()
{
	memset(&m_stResult, 0x00, sizeof(t_ELIPSO_RESULT_INFO));

	if (m_hResultThread != NULL)
	{
		m_bProcess = FALSE;
		m_nStep = -1;
		m_bResultEnd = FALSE;
		SuspendThread(m_hResultThread);
		CloseHandle(m_hResultThread);
		m_hResultThread = NULL;
	}

	m_nStep = 10;
	m_bResultEnd = TRUE;

	thread thrdReadyAlign = thread(AlignThread, this);
	thrdReadyAlign.detach();
}

void CElipsoMeterIF::m_fnSetMeasureStart( CString strRecipeName )
{
	if (m_bResultEnd == TRUE)
		return;

	memset( &m_stResult, 0x00, sizeof(t_ELIPSO_RESULT_INFO));
	m_strRunRecipe = strRecipeName;

	if (m_hResultThread != NULL)
	{
		m_bProcess   = FALSE;
		m_nStep      = -1;
		m_bResultEnd = FALSE;
		SuspendThread(m_hResultThread);
		CloseHandle(m_hResultThread);
		m_hResultThread = NULL;
	}

	m_nStep = 10;
	m_bResultEnd    = TRUE;

	thread thrdResult = thread(ResultThread, this);
	thrdResult.detach();
}

void CElipsoMeterIF::m_fnGetResult( t_ELIPSO_RESULT_INFO& stResult )
{
	memcpy(&stResult, &m_stResult, sizeof(t_ELIPSO_RESULT_INFO));
}

UINT LoadRecipeThread(LPVOID pFuncData)
{
	CElipsoMeterIF		*pQI = (CElipsoMeterIF *)pFuncData;

	while (pQI->m_bResultEnd)
	{
		Sleep(75);
		if (pQI->m_bResultProcess)
			continue;
		pQI->m_bResultProcess = TRUE;

		if (pQI->m_hWnd != NULL)
		{
			switch (pQI->m_nStep)
			{
			case 10:	// 장비상태 송신
			{
				pQI->m_bComplete = FALSE;
				pQI->m_bError = FALSE;
				pQI->m_stResult.bError = FALSE;
				pQI->m_stResult.bComplete = FALSE;
				pQI->m_nStep = 70;
			}
			break;

			case 70:			// LOAD RECIPE
			{
				if (pQI->m_strRunRecipe.IsEmpty())
				{
					pQI->m_nStep = 200;
				}
				pQI->m_bComplete = FALSE;
				pQI->m_bError = FALSE;
				pQI->m_fnSetLoadRecipe(pQI->m_strRunRecipe);
				pQI->m_nStep = 80;
			}
			break;
			case 80:
			{
				if (pQI->m_bComplete)
				{
					if (pQI->m_bError)
					{
						pQI->m_nStep = 200;
					}
					else
					{
						pQI->m_nStep = 90;
					}
				}
			}
			break;


			case 90:			// MODEL STRUCTURE
			{
				pQI->m_bComplete = FALSE;
				pQI->m_bError = FALSE;
				pQI->m_fnGetModelStructure();
				pQI->m_nStep = 100;
			}
			break;
			case 100:
			{
				if (pQI->m_bComplete)
				{
					if (pQI->m_bError)
					{
						pQI->m_nStep = 200;
					}
					else
					{
						pQI->m_nStep = 210;
					}
				}
			}
			break;


			case 200:		// 알람발생후 셔터 CLOSE
			{
				pQI->m_bComplete = TRUE;
				pQI->m_bError = TRUE;
				pQI->m_nStep = 210;
			}
			break;
			case 210:
			{
				if (pQI->m_bComplete)
				{
					if (pQI->m_bError)
					{
						pQI->m_stResult.bError = TRUE;
						pQI->m_stResult.bComplete = TRUE;
						pQI->m_bResultEnd = FALSE;
						pQI->m_nStep = -1;
					}
					else
					{
						pQI->m_stResult.bError = FALSE;
						pQI->m_stResult.bComplete = TRUE;
						pQI->m_bResultEnd = FALSE;
						pQI->m_nStep = -1;
					}
				}
			}
			break;

			}
		}
		pQI->m_bResultProcess = FALSE;
	}

	if (pQI->m_hWnd == NULL) return 0;

	if (pQI->m_hResultThread != NULL)
	{
		SuspendThread(pQI->m_hResultThread);
		CloseHandle(pQI->m_hResultThread);
		pQI->m_hResultThread = NULL;
	}
	return 0;
}


UINT ReadyThread(LPVOID pFuncData)
{
	CElipsoMeterIF		*pQI = (CElipsoMeterIF *)pFuncData;

	while (pQI->m_bResultEnd)
	{
		Sleep(75);
		if (pQI->m_bResultProcess)
			continue;
		pQI->m_bResultProcess = TRUE;

		if (pQI->m_hWnd != NULL)
		{
			switch (pQI->m_nStep)
			{
			case 10:	// 장비상태 송신
			{
				pQI->m_bComplete = FALSE;
				pQI->m_bError = FALSE;
				pQI->m_stResult.bError = FALSE;
				pQI->m_stResult.bComplete = FALSE;
				pQI->m_fnGetStatus();
				pQI->m_nStep = 20;
			}
			break;
			case 20:	// 장비상태 수신
			{
				if (pQI->m_bComplete)
				{
					if (pQI->m_bError)
					{
						pQI->m_nStep = 200;
					}
					else
					{
						pQI->m_nStep = 30;
					}
				}
			}
			break;
			case 30:		// SHUTTER OPEN
			{
				pQI->m_bComplete = FALSE;
				pQI->m_bError = FALSE;
				pQI->m_fnSetSystemLampShutterOpen();
				pQI->m_nStep = 40;
			}
			break;
			case 40:
			{
				if (pQI->m_bComplete)
				{
					if (pQI->m_bError)
					{
						pQI->m_nStep = 200;
					}
					else
					{
						pQI->m_nStep = 50;
					}
				}
			}
			break;
			case 50:		// AUTO ON
			{
				pQI->m_bComplete = FALSE;
				pQI->m_bError = FALSE;
				pQI->m_fnSetSystemFilterWheelAutoOn();
				pQI->m_nStep = 60;
			}
			break;
			case 60:
			{
				if (pQI->m_bComplete)
				{
					if (pQI->m_bError)
					{
						pQI->m_nStep = 200;
					}
					else
					{
						pQI->m_nStep = 210;
					}
				}
			}
			break;

			case 200:		// 알람발생후 셔터 CLOSE
			{
				pQI->m_bComplete = TRUE;
				pQI->m_bError = TRUE;
				pQI->m_nStep = 210;
			}
			break;
			case 210:
			{
				if (pQI->m_bComplete)
				{
					if (pQI->m_bError)
					{
						pQI->m_stResult.bError = TRUE;
						pQI->m_stResult.bComplete = TRUE;
						pQI->m_bResultEnd = FALSE;
						pQI->m_nStep = -1;
					}
					else
					{
						pQI->m_stResult.bError = FALSE;
						pQI->m_stResult.bComplete = TRUE;
						pQI->m_bResultEnd = FALSE;
						pQI->m_nStep = -1;
					}
				}
			}
			break;

			}
		}
		pQI->m_bResultProcess = FALSE;
	}

	if (pQI->m_hWnd == NULL) return 0;

	if (pQI->m_hResultThread != NULL)
	{
		SuspendThread(pQI->m_hResultThread);
		CloseHandle(pQI->m_hResultThread);
		pQI->m_hResultThread = NULL;
	}
	return 0;
}

UINT AlignThread(LPVOID pFuncData)
{
	CElipsoMeterIF		*pQI = (CElipsoMeterIF *)pFuncData;

	while (pQI->m_bResultEnd)
	{
		Sleep(75);
		if (pQI->m_bResultProcess)
			continue;
		pQI->m_bResultProcess = TRUE;

		if (pQI->m_hWnd != NULL)
		{
			switch (pQI->m_nStep)
			{
			case 10:	// 장비상태 송신
			{
				pQI->m_bComplete = FALSE;
				pQI->m_bError = FALSE;
				pQI->m_stResult.bError = FALSE;
				pQI->m_stResult.bComplete = FALSE;
				pQI->m_nStep = 70;
			}
			break;

			case 70:	
			{
				pQI->m_bComplete = FALSE;
				pQI->m_bError = FALSE;
				pQI->m_fnGetSystemGetReceiverCameraAlign();
				pQI->m_nStep = 160;
			}
			break;
			case 160:
			{
				if (pQI->m_bComplete)
				{
					if (pQI->m_bError)
					{
						pQI->m_stResult.bError = TRUE;
						pQI->m_stResult.bComplete = TRUE;
						pQI->m_bResultEnd = FALSE;
						pQI->m_nStep = -1;
						pQI->m_bResultProcess = FALSE;
						continue;
					}
					else
					{
						pQI->m_stResult.bError = FALSE;
						pQI->m_stResult.bComplete = TRUE;
						pQI->m_bResultEnd = FALSE;
						pQI->m_nStep = -1;
					}
				}
			}
			break;

			case 200:		// 알람발생후 셔터 CLOSE
			{
				pQI->m_bComplete = TRUE;
				pQI->m_bError = TRUE;
				pQI->m_nStep = 210;
			}
			break;
			case 210:
			{
				if (pQI->m_bComplete)
				{
					if (pQI->m_bError)
					{
						pQI->m_stResult.bError = TRUE;
						pQI->m_stResult.bComplete = TRUE;
						pQI->m_bResultEnd = FALSE;
						pQI->m_nStep = -1;
					}
// 					else
// 					{
// 						pQI->m_stResult.bError = TRUE;
// 						pQI->m_stResult.bComplete = TRUE;
// 						pQI->m_bResultEnd = FALSE;
// 						pQI->m_nStep = -1;
// 					}
				}
			}
			break;

			}
		}
		pQI->m_bResultProcess = FALSE;
	}

	if (pQI->m_hWnd == NULL) return 0;

	if (pQI->m_hResultThread != NULL)
	{
		SuspendThread(pQI->m_hResultThread);
		CloseHandle(pQI->m_hResultThread);
		pQI->m_hResultThread = NULL;
	}
	return 0;
}


UINT ResultThread(LPVOID pFuncData)
{
	CElipsoMeterIF		*pQI = (CElipsoMeterIF *)pFuncData;
	CTimeInterval CheckTimeIF;
	char tmpInspTackTime[256];

	while (pQI->m_bResultEnd)
	{
		Sleep(35);
		if (pQI->m_bResultProcess)
			continue;
		pQI->m_bResultProcess = TRUE;

		if (pQI->m_hWnd != NULL)
		{
			switch (pQI->m_nStep)
			{
			case 10:	// 장비상태 송신
			{
				pQI->m_bComplete = FALSE;
				pQI->m_bError = FALSE;
				pQI->m_stResult.bError = FALSE;
				pQI->m_stResult.bComplete = FALSE;
				pQI->m_nStep = 110;
			}
			break;

				case 110:			// RUN RECIPE
				{
					pQI->m_bComplete = FALSE;
					pQI->m_bError = FALSE;
					pQI->m_fnSetRunRecipe( pQI->m_strRunRecipe );
					pQI->m_nStep = 120;
					CheckTimeIF.SetTimeStart();
				}
				break;
				case 120:
				{
					if (pQI->m_bComplete)
					{
						if (pQI->m_bError)
						{
								pQI->m_stResult.bError = TRUE;
								pQI->m_stResult.bComplete = TRUE;
								pQI->m_bResultEnd = FALSE;
								pQI->m_nStep = -1;
								pQI->m_bResultProcess = FALSE;
						}
						else
						{
								pQI->m_stResult.bError = FALSE;
								pQI->m_stResult.bComplete = TRUE;
								pQI->m_bResultEnd = FALSE;
								pQI->m_nStep = -1;
								sprintf_s(tmpInspTackTime, sizeof(char) * 256, "m_fnSetRunRecipe: %.1f s", CheckTimeIF.GetTimeInterval(2));
								OutputDebugString(tmpInspTackTime);
						}
					}
				}
				break;
			}
		}
		pQI->m_bResultProcess = FALSE;
	}

	if (pQI->m_hWnd == NULL) return 0;

	if (pQI->m_hResultThread != NULL)
	{
		SuspendThread(pQI->m_hResultThread);
		CloseHandle(pQI->m_hResultThread);
		pQI->m_hResultThread = NULL;
	}
	return 0;
}


void CElipsoMeterIF::OnBnClickedBtnMeasure()
{
	m_fnSetMeasureStart(m_strRunRecipe);
}


void CElipsoMeterIF::OnBnClickedBtnAlign()
{
	m_fnGetAlignZ();
}


void CElipsoMeterIF::OnBnClickedBtnSystemLampOn()
{
	m_fnSetSystemLampOn();
}

void CElipsoMeterIF::m_fnSetSystemLampOn()
{
	m_fnSend1(SYSTEM_LAMP_ON);
	m_nCmdNo = CMD_SYSTEM_LAMP_ON;
}


void CElipsoMeterIF::OnBnClickedBtnSystemLampOff()
{
	m_fnSetSystemLampOff();
}

void CElipsoMeterIF::m_fnSetSystemLampOff()
{
	m_fnSend1(SYSTEM_LAMP_OFF);
	m_nCmdNo = CMD_SYSTEM_LAMP_OFF;
}


void CElipsoMeterIF::OnBnClickedBtnSystemLampIsOn()
{
	m_fnGetSystemLampIsOn();
}

void CElipsoMeterIF::m_fnGetSystemLampIsOn()
{
	m_fnSend1(SYSTEM_LAMP_IS_ON);
	m_nCmdNo = CMD_SYSTEM_LAMP_IS_ON;
}


void CElipsoMeterIF::OnBnClickedBtnSystemLampPowerIsOn()
{
	m_fnGetSystemLampPowerIsOn();
}

void CElipsoMeterIF::m_fnGetSystemLampPowerIsOn()
{
	// 	m_bComplete = m_bError = FALSE;
	m_fnSend1(SYSTEM_LAMP_POWER_IS_ON);
	m_nCmdNo = CMD_SYSTEM_LAMP_POWER_IS_ON;
}

