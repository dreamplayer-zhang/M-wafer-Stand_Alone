// VitLedControl.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "EllipsoMeter.h"
#include "VitLedControl.h"
#include "afxdialogex.h"


// CVitLedControl ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CVitLedControl, CDialogEx)

CVitLedControl::CVitLedControl(CWnd* pParent /*=NULL*/)
	: CDialogEx(CVitLedControl::IDD, pParent)
{
#if VIT_LED_LIGHT_CTL
	this->Create( IDD_VIT_LIGHT );
	m_bShow = FALSE;

	m_PortID            = _T("");
	memset( m_byData, 0x00, sizeof(m_byData));
	m_nMsgCnt			= 0;
	m_byLastCH			= NULL;
	m_wLightData        = 0;
	m_pTaskHandle       = NULL;
	m_bConnect			= FALSE;
#endif
}

CVitLedControl::~CVitLedControl()
{
}

#if VIT_LED_LIGHT_CTL
void CVitLedControl::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CVitLedControl)
	DDX_Control(pDX, IDC_COMM_RCV_LIST, m_lstRcvComm);
	DDX_Control(pDX, IDC_COMM_LIST, m_lstComm);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_CBO_PORT, m_clPort);
	DDX_Control(pDX, IDC_CBO_SPEED, m_clBaudRate);
	DDX_Control(pDX, IDC_CBO_DATA_BIT, m_clDataBit);
	DDX_Control(pDX, IDC_CBO_STOP_BIT, m_clStopBit);
	DDX_Control(pDX, IDC_CBO_PARITY_BIT, m_clParityBit);
	DDX_Control(pDX, IDC_CBO_FLOWCTRL, m_clFlowCtl);
	DDX_Control(pDX, IDC_MULTIPORTXX, m_Multiportxx);
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CBO_CHDATAREQ, m_clRequest);
	DDX_Control(pDX, IDC_CBO_CHDATA, m_clChData);
}

BEGIN_EVENTSINK_MAP(CVitLedControl, CDialogEx)
	ON_EVENT(CVitLedControl, IDC_MULTIPORTXX, 201, CVitLedControl::OnSerialReceiveMultiportxx, VTS_BSTR VTS_I4 VTS_I4 VTS_I4 VTS_I4)
END_EVENTSINK_MAP()

BEGIN_MESSAGE_MAP(CVitLedControl, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_CLR, OnBtnClr)
	ON_BN_CLICKED(IDC_BTN_LINE_SET, OnBtnLineSet)
	ON_BN_CLICKED(IDC_BTN_OPEN, OnBtnOpen)
	ON_BN_CLICKED(IDC_BTN_EXIT, &CVitLedControl::OnBnClickedBtnExit)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CVitLedControl::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_CLOSE, &CVitLedControl::OnBnClickedBtnClose)
	ON_BN_CLICKED(IDC_BTN_SEND, &CVitLedControl::OnBnClickedBtnSend)
	ON_BN_CLICKED(IDC_BTN_LIGHT_DATA_REQ, &CVitLedControl::OnBnClickedBtnLightDataReq)
	ON_BN_CLICKED(IDC_BTN_LIGHT__ON_OFF_STS, &CVitLedControl::OnBnClickedBtnLight)
	ON_BN_CLICKED(IDC_BTN_LIGHT_DATA, &CVitLedControl::OnBnClickedBtnLightData)
	ON_BN_CLICKED(IDC_BTN_LIGHT_DATA_ONLY, &CVitLedControl::OnBnClickedBtnLightDataOnly)
END_MESSAGE_MAP()

BOOL CVitLedControl::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (( nID   == 2 ) &&
		( nCode == -1 ))
	{
		m_fnSetShowFlag( FALSE );
	}
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CVitLedControl::m_fnSetShowFlag( BOOL bShow )
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

BOOL CVitLedControl::PreTranslateMessage(MSG* pMsg) 
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
// ���ۿ� �ʿ��� �۾��� �Ѵ�.
//---------------------------------------------------------------------------------------------------------
void CVitLedControl::m_fnSetInit( ) 
{
	GetDlgItem( IDC_EDT_LINE_CNT )->SetWindowText( "1000" );

	CString s("");
	s.Format( "Vit Led ���� ����â");
	this->SetWindowText(s);

	m_Log.m_fnSetLogPath( "VitLedLight", "VitLedLight" );
	m_Log.m_fnSetLogWriteFlag( TRUE );

	m_HexaLog.m_fnSetLogPath( "VitLedLight", "VitLedLight_HEXA" );
	m_HexaLog.m_fnSetLogWriteFlag( TRUE );

	m_pTaskHandle = new CTaskHandle( this, SHARED_NAME, 
											1, 
											sizeof( SHEET_RESISTANCE_Format ) + 1,
											0,
											FALSE );

	m_pTask					= ( SHEET_RESISTANCE_Format* )m_pTaskHandle->m_pFilePointer( );
	m_bConnect = FALSE;

	m_fnGetLight_Load();

	m_fnPortOpen();

	int nPort = m_stComm.nComPort - 1;
	m_clPort.SetCurSel( nPort );

	int nBaudRate = m_stComm.nBaudRate;
	for ( int i = 0; i < 9; i++ )
	{
		if ( atoi(szBaudRate[i]) == nBaudRate )
		{
			m_clBaudRate.SetCurSel( i );
			break;
		}
	}

	int nDataBit = m_stComm.nDataBit - 5;
	if ( nDataBit < 0 )
	{
		nDataBit = 0;
	}
	m_clDataBit.SetCurSel( nDataBit );

	int nStopBit = m_stComm.nStopBit - 1;
	m_clStopBit.SetCurSel( nStopBit );

	int nParityBit = m_stComm.nParityBit;
	m_clParityBit.SetCurSel( nParityBit );

	int nFlowCtl = m_stComm.nFlowCtrl;
	m_clFlowCtl.SetCurSel( nFlowCtl );

	for( int i = 0; i <= MAX_CHANNEL; i++)
	{
		m_clRequest.AddString(szCHName[i]);
		m_clChData.AddString(szCHName[i]);
	}

	if ( m_bConnect )
	{
		m_fnSetAllLightOff( );
	}
}

//---------------------------------------------------------------------------------------------------------
// ���ῡ �ʿ��� �۾��� �Ѵ�.
//---------------------------------------------------------------------------------------------------------
void CVitLedControl::m_fnSetEnd() 
{
	if ( this->m_hWnd != NULL )
	{
		if ( m_Multiportxx.IsOpenPort( m_PortID ) )
		{
			m_Multiportxx.ClosePort( m_PortID );
		}
	}
	if ( m_pTaskHandle != NULL )
	{
		m_bConnect = FALSE;
		m_pTaskHandle->m_fnSetEnd();
		delete m_pTaskHandle;
		m_pTaskHandle = NULL;
	}
}

void CVitLedControl::OnBnClickedBtnExit()
{
	m_fnSetShowFlag( FALSE );
}

void CVitLedControl::OnBnClickedBtnSave()
{
	int nPort = m_clPort.GetCurSel( );
	m_stComm.nComPort = nPort + 1;

	int nBaudRate = atoi(szBaudRate[m_clBaudRate.GetCurSel( )]);
	m_stComm.nBaudRate = nBaudRate;

	int nDataBit = m_clDataBit.GetCurSel( ) + 5;
	m_stComm.nDataBit = nDataBit;

	int nStopBit = m_clStopBit.GetCurSel( ) + 1;
	m_stComm.nStopBit = nStopBit;

	int nParityBit = m_clParityBit.GetCurSel( );
	m_stComm.nParityBit = nParityBit;

	int nFlowCtl = m_clFlowCtl.GetCurSel( );
	m_stComm.nFlowCtrl  = nFlowCtl;


	m_stComm.nChannel = 1;

	m_fnSetLight_Save();
}

void CVitLedControl::OnSerialReceiveMultiportxx(LPCTSTR idstr, long BufferPointer, long BufferSize, long UserMemorySize, long UserMemoryPointer)
{
	BYTE    aByte  = 0x00;
	int     size   = 0;

	CString strData("");
	CString strASCII("");
	CString strHEXA("");
	CString strCNV("");

	size = BufferSize;

	BYTE *byRecvData = (BYTE *)BufferPointer;

	if ( size > 0 )
	{
		for ( int i = 0; i < size; i++ )
		{
			aByte = (BYTE)*byRecvData;
			if ( aByte != NULL )
			{
				m_byData[m_nMsgCnt++] = aByte;
				strASCII  += (char)aByte;
				strCNV.Format( "%02x", aByte );
				strHEXA += strCNV;
				strHEXA += " ";
			}
			else
			{
				m_byData[m_nMsgCnt++] = aByte;
				strASCII  += "<NULL>";
				strCNV.Format( "%02x", aByte );
				strHEXA += strCNV;
				strHEXA += " ";
			}
			if (( aByte == LF ) &&
				( m_byLastCH == CR ))
			{
				m_fnSetParsing( m_byData, m_nMsgCnt );
				memset( m_byData, 0x00, sizeof( m_byData ) );
				m_nMsgCnt = 0;
			}
			m_byLastCH = aByte;
		}
	}
	else
	{
		return;
	}

	CString strTMP("");

	strTMP.Format( "RECV %s", strASCII );
	m_fnLogWrite( strTMP );
	strTMP.Format( "RECV %s", strHEXA );
	m_fnLogRvcDataWrite( strTMP );
}

void CVitLedControl::m_fnSetParsing( BYTE* szData, int nLen )
{
	CString strMsg("");
	if ( nLen != 7 )	// ��� �����ʹ� 7��Ʈ�� �����ȴ�.
	{
		strMsg.Format( "Vit���� ��Ʈ�ѷ����� ���� ������ ���̰� 7����Ʈ�� �ƴմϴ�. Len=%d", nLen );
		m_fnLogWrite( strMsg );
		return;
	}

	if (( szData[5] != CR ) ||
		( szData[6] != LF ))
	{
		strMsg.Format( "Vit���� ��Ʈ�ѷ����� ���� �����Ϳ� CR + LF�� �����ϴ�." );
		m_fnLogWrite( strMsg );
		return;
	}

	szData[5] = NULL;
	szData[6] = NULL;

}

void CVitLedControl::m_fnSetSend( CString strData )
{
	if ( m_Multiportxx.IsOpenPort( m_PortID ) )
	{
		BYTE    byBuffer[128];
		BYTE    byCheckSum = 0x00;
		CString strResult("");
		CString strMSG("");

		strMSG += strData;

		int nLen = strMSG.GetLength();
		memset( byBuffer, 0x00, sizeof( byBuffer ));
		memcpy( byBuffer, strMSG, nLen );

		// ASCII �α� ����
		strResult.Format( "SEND %s", strMSG );
		m_fnLogWrite( strResult );

		// HEXA �α� ����
		m_Util.CharToHex( byBuffer, nLen, strResult );
		strMSG.Format( "SEND %s", strResult );
		m_fnLogRvcDataWrite( strMSG );

		if  ( (m_Multiportxx.Send( m_PortID, (long)byBuffer, nLen)) != 1 )
		{
			m_fnLogWrite( "SEND ó�� ����" );
		}
		else
		{
			m_fnLogWrite( "SEND ó�� ����" );
		}
	}
	else
	{
		m_fnLogWrite( "��Ʈ�� ������ READ COMMAND ���� �Ұ�!!!!!" );
	}
}

//---------------------------------------------------------------------------------------------------------
// Log ������ �����Ѵ�.
//---------------------------------------------------------------------------------------------------------
void CVitLedControl::m_fnLogWrite( CString strMsg )
{
	CString strCnt("");
	int     nCnt		= 0;
	int     nTotal		= 0;

	memset( m_szData, 0x00, sizeof( m_szData ));

	sprintf_s( m_szData, "%s", strMsg.GetString() );
	if ( m_pTask == NULL ) return;
	if ( m_pTask->stCommon.bLogSave )
	{
		m_Log.DEBUG_LOG( m_szData );
	}

	if ( m_hWnd == NULL )
	{
		return;
	}

	GetDlgItem( IDC_EDT_LINE_CNT )->GetWindowText( strCnt );
	nCnt			= atoi( strCnt ) - 1;

	nTotal			= m_lstComm.GetCount();
	if ( nCnt < nTotal )
	{
		for( int i = nTotal; i >= nCnt; i-- )
		{
			m_lstComm.DeleteString( i );	
		}
	}
	m_lstComm.InsertString( 0, strMsg );
}

//---------------------------------------------------------------------------------------------------------
// Log ������ �����Ѵ�.
//---------------------------------------------------------------------------------------------------------
void CVitLedControl::m_fnLogRvcDataWrite( CString strMsg )
{
	CString strCnt("");
	int     nCnt		= 0;
	int     nTotal		= 0;

	memset( m_szData, 0x00, sizeof( m_szData ));

	sprintf_s( m_szData, "%s", strMsg.GetString() );
	m_HexaLog.DEBUG_LOG( m_szData );

	if ( m_hWnd == NULL )
	{
		return;
	}

	GetDlgItem( IDC_EDT_LINE_CNT )->GetWindowText( strCnt );
	nCnt			= atoi( strCnt ) - 1;

	nTotal			= m_lstRcvComm.GetCount();
	if ( nCnt < nTotal )
	{
		for( int i = nTotal; i >= nCnt; i-- )
		{
			m_lstRcvComm.DeleteString( i );
		}
	}
	m_lstRcvComm.InsertString( 0, strMsg );
}


void CVitLedControl::OnBtnClr() 
{
	m_lstComm.ResetContent();	
	m_lstRcvComm.ResetContent();	
}

void CVitLedControl::OnBtnLineSet() 
{
	CString strCnt("");
	CString strMsg("");
	int nCnt		= 0;

	GetDlgItem( IDC_EDT_LINE_CNT )->GetWindowText( strCnt );
	nCnt			= atoi(strCnt);

	if ( nCnt <= 0 )
	{
		GetDlgItem( IDC_EDT_LINE_CNT )->SetWindowText( DEFAULT_MIN_LINE_CNT );
		nCnt = 1;

	}
	strMsg.Format( "��ŷα� ���μ� ���� = %d" , nCnt );
	m_fnLogWrite( strMsg );

	UpdateData(FALSE);
}

void CVitLedControl::m_fnPortOpen() 
{
	BOOL bOnLine = ((CButton*)GetDlgItem( IDC_CHK_ONLINE ))->GetCheck( );

	if ( bOnLine )
	{
		if ( m_Multiportxx.IsOpenPort( m_PortID ) )
		{
			m_Multiportxx.ClosePort( m_PortID );
		}
		m_fnLogWrite( "Port CLOSE ����" );
		m_bConnect = FALSE;
		((CButton*)GetDlgItem( IDC_CHK_ONLINE ))->SetCheck( m_bConnect );
	}
	Sleep( 100 );

	if ( m_Multiportxx.OpenPort( m_PortID, 
									m_stComm.nComPort,
									m_stComm.nBaudRate,
									m_stComm.nParityBit,
									m_stComm.nDataBit,
									m_stComm.nStopBit,
								    0 ) == 1 )
	{
		m_bConnect = TRUE;
		m_fnLogWrite( "PORT ���� ����" );
		m_byLastCH = NULL;
	}
	else
	{
		m_bConnect = FALSE;
		m_fnLogWrite( "PORT ���� ����" );
	}
	((CButton*)GetDlgItem( IDC_CHK_ONLINE ))->SetCheck(  m_bConnect );

}

void CVitLedControl::OnBtnOpen() 
{
	m_fnPortOpen();
}

//------------------------------------------------------------------------
// ��� ������ �ʿ��� ������ �����Ѵ�.
//------------------------------------------------------------------------
BOOL CVitLedControl::m_fnSetMakeFolder()
{
	CString strDirve("");

	// ���� ������� ����̺� ������ �����Ѵ�.
	strDirve = m_Util.m_fnGetCurrentDrive();

	// ȯ������ ������ �����Ѵ�. 
	char szFullPath[MAX_PATH + 1];
	memset( szFullPath, 0x00, sizeof( szFullPath ));

	sprintf_s( szFullPath, "%s%s%s", strDirve.GetString(),
										DEFAULT_PATH,
										DEFAULT_ENV_PATH );
									    szFullPath[MAX_PATH] = NULL;
	m_Util.m_fnSetCreateDir( szFullPath );

	return TRUE;
}

//------------------------------------------------------------------------
// EFU ������ ��θ� �о�´�.
//------------------------------------------------------------------------
CString CVitLedControl::m_fnGetLightFileFullPath()
{
	char szFullPath[MAX_PATH + 1];
	memset( szFullPath, 0x00, sizeof( szFullPath ));

	sprintf_s( szFullPath, "%s%s%s%s", m_Util.m_fnGetCurrentDrive().GetString(),
										DEFAULT_PATH,
										DEFAULT_ENV_PATH,
										DEFAULT_VITLEDLIGHT_FILE );
	szFullPath[MAX_PATH] = NULL;

	return szFullPath;
}

//------------------------------------------------------------------------
// ���� ȯ�������� �����Ѵ�.
//------------------------------------------------------------------------
BOOL CVitLedControl::m_fnSetLight_Save( )
{
	CString strWork("");
	CString strData("");
	CString strFullPath("");
	int     i 			= 0;
	int     nNo			= 0;

	CStringArray strLst;
	CXmlElement *pElement     = NULL;
	CXmlElement *pRootElement = NULL;

	strFullPath		=	m_fnGetLightFileFullPath();

	m_fnLogWrite( "Comm Data ����" );

	CXmlDocument* pXML = new CXmlDocument();
	pXML->Load( strFullPath );

	pXML->GetTagToString( RS232C , strData );
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
	strLst.Add(LIGHT);
	pElement = pXML->FindElement( pRootElement, strLst );
	if ( pElement != NULL )
	{
		// COMPORT
		strWork.Format( "%01d", m_stComm.nComPort );
		pXML->DataSet( pElement, COMPORT, strWork );

		// BAUDRATE
		strWork.Format( "%01d", m_stComm.nBaudRate );
		pXML->DataSet( pElement, BAUDRATE, strWork );

		// STOPBIT
		strWork.Format( "%01d", m_stComm.nStopBit );
		pXML->DataSet( pElement, STOPBIT, strWork );

		// DATABIT
		strWork.Format( "%01d", m_stComm.nDataBit );
		pXML->DataSet( pElement, DATABIT, strWork );

		// PARITYBIT
		strWork.Format( "%s", NONE__ );
		if ( m_stComm.nParityBit == 1 )
		{
			strWork.Format( "%s", ODD );
		}
		if ( m_stComm.nParityBit == 2 )
		{
			strWork.Format( "%s", EVEN );
		}
		pXML->DataSet( pElement, PARITYBIT, strWork );

		// FLOWCTL
		strWork.Format( "%s", STR_OFF );
		if ( m_stComm.nFlowCtrl == 1 )
		{
			strWork.Format( "%s", STR_ON );
		}
		pXML->DataSet( pElement, FLOWCTRL, strWork );

		// ����ϴ� ä��
		strWork.Format( "%01d", m_stComm.nChannel );
		pXML->DataSet( pElement, CHANNEL, strWork );

		pXML->Store( strFullPath );
	}

	delete pXML;
	pXML = NULL;

	m_fnLogWrite( "Comm Data ���� �Ϸ�" );

	return TRUE;
}

//------------------------------------------------------------------------
// ���� ȯ�������� �ε��Ѵ�.
//------------------------------------------------------------------------
BOOL CVitLedControl::m_fnGetLight_Load( )
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

	strFullPath					=	m_fnGetLightFileFullPath();

	m_fnLogWrite( "Comm Data �б�" );

	CXmlDocument* pXML = new CXmlDocument();
	pXML->Load( strFullPath );

	pXML->GetTagToString( RS232C , strData );
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
	strLst.Add(LIGHT);
	pElement = pXML->FindElement( pRootElement, strLst );
	if ( pElement != NULL )
	{
		// COMPORT
		pXML->FindData( pElement, COMPORT, strWork );
		m_stComm.nComPort = atoi( strWork );

		// BAUDRATE
		pXML->FindData( pElement, BAUDRATE, strWork );
		m_stComm.nBaudRate		= atoi( strWork );

		// STOPBIT
		pXML->FindData( pElement, STOPBIT, strWork );
		m_stComm.nStopBit		= atoi( strWork );

		// DATABIT
		pXML->FindData( pElement, DATABIT, strWork );
		m_stComm.nDataBit		= atoi( strWork );

		// PARITY
		pXML->FindData( pElement, PARITYBIT, strWork );
		if ( strWork == NONE__ )
		{
			m_stComm.nParityBit		= 0;
		}
		if ( strWork == ODD )
		{
			m_stComm.nParityBit		= 1;
		}
		if ( strWork == EVEN )
		{
			m_stComm.nParityBit		= 2;
		}
		// FLOWCTL
		pXML->FindData( pElement, FLOWCTRL, strWork );
		if ( strWork == STR_OFF )
		{
			m_stComm.nFlowCtrl		= 0;
		}
		else
		{
			m_stComm.nFlowCtrl		= 1;
		}
	}

	// FLOWCTL
	pXML->FindData( pElement, CHANNEL, strWork );
	m_stComm.nChannel = atoi( strWork );

	delete pXML;
	pXML = NULL;

	m_fnLogWrite( "Comm Data �б� �Ϸ�" );

	return TRUE;
}

void CVitLedControl::OnBnClickedBtnClose()
{
	if ( m_Multiportxx.IsOpenPort( m_PortID ) )
	{
		m_Multiportxx.ClosePort( m_PortID );
	}
	m_fnLogWrite( "Port CLOSE ����" );
	m_bConnect = FALSE;
	m_byLastCH = NULL;
	((CButton*)GetDlgItem( IDC_CHK_ONLINE ))->SetCheck( m_bConnect );
}

BOOL CVitLedControl::m_fnSetSend( BYTE* byBuffer, int nLen )
{
	if ( m_Multiportxx.IsOpenPort( m_PortID ) )
	{
		CString strResult("");
		CString strMSG("");

		// ASCII �α� ����
		for ( int i = 0; i < nLen; i++ )
		{
			if ( byBuffer[i] == NULL )
			{
				strMSG += "<NULL>";
			}
			else
			{
				strMSG += (char)byBuffer[i]; 
			}
		}
		strResult.Format( "SEND %s", strMSG );
		m_fnLogWrite( strResult );

		// HEXA �α� ����
		m_Util.CharToHex( byBuffer, nLen, strResult );
		strMSG.Format( "SEND %s", strResult );
		m_fnLogRvcDataWrite( strMSG );

		if  ( (m_Multiportxx.Send( m_PortID, (long)byBuffer, nLen)) != 1 )
		{
			m_fnLogWrite( "SEND ó�� ����" );
			return FALSE;
		}
		else
		{
			m_fnLogWrite( "SEND ó�� ����" );
		}
	}
	else
	{
		m_fnLogWrite( "��Ʈ�� ������ READ COMMAND ���� �Ұ�!!!!!" );
		return FALSE;
	}
	return TRUE;
}

BOOL CVitLedControl::m_fnSetONOFF( WORD wData )
{
	BYTE byBuffer[MAX_BUFF_SIZE + 1];
	memset( byBuffer, 0x00, sizeof(byBuffer));

	int nCnt = 0;
	byBuffer[nCnt++] = 'O';
	byBuffer[nCnt++] = 'N';
	byBuffer[nCnt++] = 'F';
	byBuffer[nCnt++] = 0;
	byBuffer[nCnt++] = wData & 0x0001;
	byBuffer[nCnt++] = CR;
	byBuffer[nCnt++] = LF;

	return m_fnSetSend( byBuffer, nCnt );
}


void CVitLedControl::OnBnClickedBtnLightDataReq()
{
	int nCH = m_clRequest.GetCurSel();

	if (( nCH == -1 ) ||
		( nCH ==  0 ))
	{
		m_fnLogWrite( "���� ������ ��û�ÿ��� ä�� ������ �ʿ��մϴ�." );
		return;
	}
	if ( !m_fnSetDataReqest( nCH ) )
	{
		m_fnLogWrite( "���� ������ ��û �����Դϴ�." );
	}
}


void CVitLedControl::OnBnClickedBtnLight()
{
	int nCH = m_clRequest.GetCurSel();
	nCH = 0;
	if ( nCH !=  0 )
	{
		m_fnLogWrite( "���� ���´� ��ü ä�ηθ� Ȯ�� �����մϴ�." );
		return;
	}
	if ( !m_fnSetDataReqest( nCH ) )
	{
		m_fnLogWrite( "���� ���� ��û �����Դϴ�." );
	}
}

BOOL CVitLedControl::m_fnSetDataReqest( int nCh )
{

	if (( nCh < 0 ) ||
		( nCh > 16 ))
	{
		m_fnLogWrite( "ä���� '0' , '1��~��9��,��A��~��G���� ���������մϴ�." );
		return FALSE;
	}

	BYTE byBuffer[MAX_BUFF_SIZE + 1];
	memset( byBuffer, 0x00, sizeof(byBuffer));
	int nCnt = 0;
	byBuffer[nCnt++] = 'R';
	if( nCh == 0 )
	{
		byBuffer[nCnt++] = 'O';
		byBuffer[nCnt++] = 'O';
		byBuffer[nCnt++] = 'N';
		byBuffer[nCnt++] = 'F';
	}
	else
	{
		memcpy( &byBuffer[nCnt++], szCH[nCh], 1 );
		byBuffer[nCnt++] = 'D';
		byBuffer[nCnt++] = 'A';
		byBuffer[nCnt++] = 'T';
	}
	byBuffer[nCnt++] = CR;
	byBuffer[nCnt++] = LF;

	return m_fnSetSend( byBuffer, nCnt );
}

void CVitLedControl::OnBnClickedBtnLightData()
{
	CString strChData("");

	int nCH = m_clChData.GetCurSel();

	GetDlgItem( IDC_EDT_CH_DATA )->GetWindowText( strChData );

	if ( !m_fnSetChannelData( TRUE, nCH, strChData ) )
	{
		m_fnLogWrite( "ä�κ����� �����߽��ϴ�. �α׸� Ȯ���� �ּ���." );
	}
}


void CVitLedControl::OnBnClickedBtnLightDataOnly()
{
	CString strChData("");

	int nCH = m_clChData.GetCurSel();

	GetDlgItem( IDC_EDT_CH_DATA )->GetWindowText( strChData );

	if ( !m_fnSetChannelData( FALSE, nCH, strChData ) )
	{
		m_fnLogWrite( "ä�κ����� �����߽��ϴ�. �α׸� Ȯ���� �ּ���." );
	}
}

BOOL CVitLedControl::m_fnSetChannelData( BOOL bApply, int nCh, CString strData )
{

	if (( nCh < 0 ) ||
		( nCh > 16 ))
	{
		m_fnLogWrite( "ä���� ��0��~��1��~��9��,��A��~��G���� ���������մϴ�." );
		return FALSE;
	}

	int nLightData = atoi( strData );
	if (( nLightData < 0 ) ||
		( nLightData > 255 ))
	{
		m_fnLogWrite( "Data�� 000 - 255���� ���� �����մϴ�." );
		return FALSE;
	}
	int      nLightValue = atoi(strData);
	CString strLightValue("");
	BYTE   byData[MAX_BUFF_SIZE + 1];
	memset( byData, 0x00, sizeof(byData));
	strLightValue.Format( "%03d", nLightValue );
	memcpy( byData, strLightValue, 3 );

	BYTE byBuffer[MAX_BUFF_SIZE + 1];
	memset( byBuffer, 0x00, sizeof(byBuffer));
	int nCnt = 0;
	if ( bApply )
	{
		byBuffer[nCnt++] = 'C';
	}
	else
	{
		byBuffer[nCnt++] = 'D';
	}
	memcpy( &byBuffer[nCnt++],szCH[nCh], 1 );
	byBuffer[nCnt++] = (BYTE)byData[0];
	byBuffer[nCnt++] = (BYTE)byData[1];
	byBuffer[nCnt++] = (BYTE)byData[2];
	byBuffer[nCnt++] = CR;
	byBuffer[nCnt++] = LF;

	return m_fnSetSend( byBuffer, nCnt );
}

void CVitLedControl::OnBnClickedBtnSend()
{
	CString strData("");
	BYTE byBuffer[MAX_BUFF_SIZE + 1];

	memset( byBuffer, 0x00, sizeof(byBuffer));
	GetDlgItem( IDC_EDT_SEND )->GetWindowText( strData );
	memcpy( byBuffer, strData, strData.GetLength() );
	m_fnSetSend( byBuffer, strData.GetLength() );
}

void CVitLedControl::m_fnSetLightOnOff( BOOL bOnOff )
{
	int nCh = m_stComm.nChannel;

	WORD wShift	= 0x01;
	if ( bOnOff )
	{
		if ( nCh == 1 )
		{
			m_wLightData |= wShift;
		}
	}
	else
	{
		if ( nCh == 1 )
		{
			m_wLightData ^= wShift;
		}
	}
	m_fnSetONOFF( m_wLightData );
}

void CVitLedControl::m_fnSetLightOnOff( int nCh, BOOL bOnOff )
{
	WORD wShift	 = 0x01;
	m_wLightData = 0x00;
	if ( bOnOff )
	{
		if ( nCh == 1 )
		{
			m_wLightData |= wShift;
		}
	}
	m_fnSetONOFF( m_wLightData );
}

void CVitLedControl::m_fnSetLightValue( BOOL bApply, CString strData )
{
	int nCh = m_stComm.nChannel;
	m_fnSetChannelData( bApply, nCh, strData );
}

void CVitLedControl::m_fnSetLightValue( int nCh, BOOL bApply, CString strData )
{
	m_fnSetChannelData( bApply, nCh, strData );
}

void CVitLedControl::m_fnSetAllLightOff( )
{
	WORD wShift = 0x00;
	m_fnSetONOFF( wShift );
}
#endif