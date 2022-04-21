// DSIInspectorControl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EllipsoMeter.h"
#include "DSIInspectorControl.h"
#include "afxdialogex.h"
#if DSP_INSPECTOR
IMPLEMENT_DYNAMIC(CDSIInspectorControl, CDialogEx)

CDSIInspectorControl::CDSIInspectorControl(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDSIInspectorControl::IDD, pParent)
{
	this->Create( IDD_DSI_CONTROL );
	m_bShow = FALSE;

	m_byLastCH			= NULL;
	m_nLen				= 0;
	m_strASCII          = _T("");
	m_strHEXA			= _T("");
	m_PortID            = _T("");
	memset( m_byData, 0x00, sizeof(m_byData));
	m_nMsgCnt			= 0;

	m_strLog            = _T("");
	m_bMeasure          = FALSE;
	m_bError            = FALSE;

	m_strV				= _T("");
	m_nStepNo			= 0;
	m_dR				= 0.0;
	m_dS				= 0.0;
	m_dV				= 0.0;
	m_dC				= 0.0;
	m_strRecData.RemoveAll();
	m_strRecData.FreeExtra();

	m_hRcpDataThread    = NULL;
	m_bRcpDataEnd	    = FALSE;


}

CDSIInspectorControl::~CDSIInspectorControl()
{
}

// CUtilityClass 메시지 처리기입니다.
//---------------------------------------------------------------------------------
// 작업중인 드라이버를 읽어온다 ex) C:\
//---------------------------------------------------------------------------------
CString CDSIInspectorControl::m_fnGetCurrentDrive()
{
	TCHAR szCurDir[MAX_PATH + 1] = _T("");
	::GetCurrentDirectory(MAX_PATH, szCurDir);

	szCurDir[3] = NULL;
	return szCurDir;
}

void CDSIInspectorControl::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CArduinoControl)
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
	DDX_Control(pDX, IDC_CBO_MEASURE_MODE, m_clMeasureMode);
	DDX_Control(pDX, IDC_CBO_RESISTER_MODE, m_clResisterMode);
	DDX_Control(pDX, IDC_CBO_MX_MODE, m_clMxMode);
	DDX_Control(pDX, IDC_CBO_MCT_MODE, m_clMctMode);
	DDX_Control(pDX, IDC_CBO_MP_MEASURE_SPEED, m_clMpMeasureSpeed);
	DDX_Control(pDX, IDC_CBO_HIGH_LIMIT, m_clHighLimit);
	DDX_Control(pDX, IDC_CBO_LOW_LIMIT, m_clLowLimit);
	DDX_Control(pDX, IDC_CBO_CURRENT_STEP, m_clCurrStep);
}

BEGIN_EVENTSINK_MAP(CDSIInspectorControl, CDialogEx)
	ON_EVENT(CDSIInspectorControl, IDC_MULTIPORTXX, 201, CDSIInspectorControl::OnSerialReceiveMultiportxx, VTS_BSTR VTS_I4 VTS_I4 VTS_I4 VTS_I4)
END_EVENTSINK_MAP()

BEGIN_MESSAGE_MAP(CDSIInspectorControl, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_CLR, OnBtnClr)
	ON_BN_CLICKED(IDC_BTN_LINE_SET, OnBtnLineSet)
	ON_BN_CLICKED(IDC_BTN_OPEN, OnBtnOpen)
	ON_BN_CLICKED(IDC_BTN_EXIT, &CDSIInspectorControl::OnBnClickedBtnExit)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CDSIInspectorControl::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_CLOSE, &CDSIInspectorControl::OnBnClickedBtnClose)
	ON_BN_CLICKED(IDC_BTN_SEND, &CDSIInspectorControl::OnBnClickedBtnSend)
	ON_BN_CLICKED(IDC_BTN_LF, &CDSIInspectorControl::OnBnClickedBtnLf)
	ON_BN_CLICKED(IDC_BTN_CR, &CDSIInspectorControl::OnBnClickedBtnCr)
	ON_BN_CLICKED(IDC_BTN_EOT, &CDSIInspectorControl::OnBnClickedBtnEot)
	ON_BN_CLICKED(IDC_BTN_SOH, &CDSIInspectorControl::OnBnClickedBtnSoh)
	ON_BN_CLICKED(IDC_BTN_MEASURE_MODE, &CDSIInspectorControl::OnBnClickedBtnMeasureMode)
	ON_BN_CLICKED(IDC_BTN_RESISTER_MODE, &CDSIInspectorControl::OnBnClickedBtnResisterMode)
	ON_BN_CLICKED(IDC_BTN_MX_MODE, &CDSIInspectorControl::OnBnClickedBtnMxMode)
	ON_BN_CLICKED(IDC_BTN_MCT_MODE, &CDSIInspectorControl::OnBnClickedBtnMctMode)
	ON_BN_CLICKED(IDC_BTN_MP_SPEED, &CDSIInspectorControl::OnBnClickedBtnMpSpeed)
	ON_BN_CLICKED(IDC_BTN_RE_MEASURE_DATA_REQ, &CDSIInspectorControl::OnBnClickedBtnReMeasureDataReq)
	ON_BN_CLICKED(IDC_BTN_DELAY_TIME, &CDSIInspectorControl::OnBnClickedBtnDelayTime)
	ON_BN_CLICKED(IDC_BTN_HIGH_LIMIT, &CDSIInspectorControl::OnBnClickedBtnHighLimit)
	ON_BN_CLICKED(IDC_BTN_LOW_LIMIT, &CDSIInspectorControl::OnBnClickedBtnLowLimit)
	ON_BN_CLICKED(IDC_BTN_CURRENT_STEP, &CDSIInspectorControl::OnBnClickedBtnCurrentStep)
	ON_BN_CLICKED(IDC_BTN_MEASURE_START, &CDSIInspectorControl::OnBnClickedBtnMeasureStart)
	ON_BN_CLICKED(IDC_BTN_MEASURE_PAUSE, &CDSIInspectorControl::OnBnClickedBtnMeasurePause)
	ON_BN_CLICKED(IDC_BTN_REMOTE_CONTROL_ENABLE, &CDSIInspectorControl::OnBnClickedBtnRemoteControlEnable)
	ON_BN_CLICKED(IDC_BTN_REMOTE_CONTROL_DISABLE, &CDSIInspectorControl::OnBnClickedBtnRemoteControlDisable)
	ON_BN_CLICKED(IDC_BTN_BEEP_ON, &CDSIInspectorControl::OnBnClickedBtnBeepOn)
	ON_BN_CLICKED(IDC_BTN_BEEP_OFF, &CDSIInspectorControl::OnBnClickedBtnBeepOff)
	ON_BN_CLICKED(IDC_BTN_CURRENT_MEASURE_STEP_NO_REQ, &CDSIInspectorControl::OnBnClickedBtnCurrentMeasureStepNoReq)
	ON_BN_CLICKED(IDC_BTN_BEFORE_MEASURE_DATA, &CDSIInspectorControl::OnBnClickedBtnBeforeMeasureData)
	ON_BN_CLICKED(IDC_BTN_ONLY_1_DATA_SEND, &CDSIInspectorControl::OnBnClickedBtnOnly1DataSend)
	ON_BN_CLICKED(IDC_BTN_RSVC_SEND, &CDSIInspectorControl::OnBnClickedBtnRsvcSend)
	ON_BN_CLICKED(IDC_BTN_DATA_MEMORY_CLEAR, &CDSIInspectorControl::OnBnClickedBtnDataMemoryClear)
	ON_BN_CLICKED(IDC_BTN_REQUEST_MEMORY_DATA, &CDSIInspectorControl::OnBnClickedBtnRequestMemoryData)
END_MESSAGE_MAP()

BOOL CDSIInspectorControl::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (( nID   == 2 ) &&
		( nCode == -1 ))
	{
		m_fnSetShowFlag( FALSE );
	}
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CDSIInspectorControl::m_fnSetShowFlag( BOOL bShow )
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

BOOL CDSIInspectorControl::PreTranslateMessage(MSG* pMsg) 
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
void CDSIInspectorControl::m_fnSetInit( ) 
{
	CString strMsg("");
	GetDlgItem( IDC_EDT_LINE_CNT )->SetWindowText( "1000" );

//	this->MoveWindow( 10, 80, 830,570, TRUE );

	m_PortID				= "DSI";
	CString s("");
	s.Format( "DSI 제어 메인창");
	this->SetWindowText(s);
	m_bConnect       = FALSE;

	m_fnGetDSI_Load();

	m_Log.m_fnSetLogPath( "DSIInspector", "DSIInspector" );
	m_Log.m_fnSetLogWriteFlag( TRUE );

	m_HexaLog.m_fnSetLogPath( "DSIInspector", "DSIInspector_HEXA" );
	m_HexaLog.m_fnSetLogWriteFlag( TRUE );
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

	for( int i = 0; i < MAX_MMAc; i++ )
	{
		m_clMeasureMode.AddString( strMMAc[i]);
	}
	m_clMeasureMode.SetCurSel(0);

	for( int i = 0; i < MAX_MRSVG; i++ )
	{
		m_clResisterMode.AddString( strMSRVG[i]);
	}
	m_clResisterMode.SetCurSel(0);

	for( int i = 0; i < MAX_Mx; i++ )
	{
		m_clMxMode.AddString( str1345[i]);
	}
	m_clMxMode.SetCurSel(0);

	for( int i = 0; i < MAX_MCT; i++ )
	{
		m_clMctMode.AddString( strCT[i]);
	}
	m_clMctMode.SetCurSel(0);
	m_clMpMeasureSpeed.SetCurSel(0);
	GetDlgItem( IDC_EDT_DELAY_TIME )->SetWindowText( "0" );

	for( int i = 0; i < MAX_RHL; i++ )
	{
		m_clHighLimit.AddString( strRHL[i]);
		m_clLowLimit.AddString( strRHL[i]);
		m_clCurrStep.AddString( strRHL[i]);
	}
	m_clHighLimit.SetCurSel(0);
	m_clLowLimit.SetCurSel(0);
	m_clCurrStep.SetCurSel(0);

	unsigned long	ThreadID;
	m_bRcpDataEnd    = TRUE;
	m_hRcpDataThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RcpDataThread, this, NULL, &ThreadID);
}

//---------------------------------------------------------------------------------------------------------
// 종료에 필요한 작업을 한다.
//---------------------------------------------------------------------------------------------------------
void CDSIInspectorControl::m_fnSetEnd() 
{
	if ( m_hRcpDataThread != NULL )
	{
		m_bRcpDataEnd		= FALSE;
		SuspendThread( m_hRcpDataThread );
		CloseHandle(m_hRcpDataThread);
		m_hRcpDataThread	= NULL;
	}

	if ( this->m_hWnd != NULL )
	{
		if ( m_Multiportxx.IsOpenPort( m_PortID ) )
		{
			m_Multiportxx.ClosePort( m_PortID );
		}
	}
}

void CDSIInspectorControl::OnBnClickedBtnExit()
{
	m_fnSetShowFlag( FALSE );
}

void CDSIInspectorControl::OnBnClickedBtnSave()
{
	CString strMsg("");
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

	m_fnSetDSI_Save();
}

void CDSIInspectorControl::OnSerialReceiveMultiportxx(LPCTSTR idstr, long BufferPointer, long BufferSize, long UserMemorySize, long UserMemoryPointer)
{
	BYTE    aByte  = 0x00;
	int     size   = 0;

	CString strData("");
	CString strASCII("");
	CString strHEXA("");
	CString strCNV("");
	CString strTMP("");

	size = BufferSize;

	BYTE *byRecvData = (BYTE *)BufferPointer;

	if ( size > 0 )
	{
		for ( int i = 0; i < size; i++ )
		{
			aByte = (BYTE)*byRecvData;
			if ( aByte == NULL )
			{
				m_byData[m_nMsgCnt++] = aByte;
				m_strASCII  += "<NULL>";
				strCNV.Format( "%02x", aByte );
				m_strHEXA += strCNV;
				m_strHEXA += " ";
			}
			else
			{
				if ( aByte == EOT )
				{
					m_byData[m_nMsgCnt++] = aByte;
					m_strASCII  += "<EOT>";
					strCNV.Format( "%02x", aByte );
					m_strHEXA += strCNV;
					m_strHEXA += " ";
				}
				else
				{
					if ( aByte == SOH )
					{
						m_byData[m_nMsgCnt++] = aByte;
						m_strASCII  += "<EOT>";
						strCNV.Format( "%02x", aByte );
						m_strHEXA += strCNV;
						m_strHEXA += " ";
					}
					else
					{
						if ( aByte == LF )
						{
							m_byData[m_nMsgCnt++] = aByte;
							m_strASCII  += "<LF>";
							strCNV.Format( "%02x", aByte );
							m_strHEXA += strCNV;
							m_strHEXA += " ";
						}
						else
						{
							if ( aByte == ETX )
							{
								m_byData[m_nMsgCnt++] = aByte;
								m_strASCII  += "<ETX>";
								strCNV.Format( "%02x", aByte );
								m_strHEXA += strCNV;
								m_strHEXA += " ";
							}
							else
							{
								if ( aByte == STX )
								{
									m_byData[m_nMsgCnt++] = aByte;
									m_strASCII  += "<STX>";
									strCNV.Format( "%02x", aByte );
									m_strHEXA += strCNV;
									m_strHEXA += " ";
								}
								else
								{
									if ( aByte == CR )
									{
										m_byData[m_nMsgCnt++] = aByte;
										m_strASCII  += "<CR>";
										strCNV.Format( "%02x", aByte );
										m_strHEXA += strCNV;
										m_strHEXA += " ";
									}
									else
									{
										if ( aByte == DOLLAR )
										{
											if ( !m_strASCII.IsEmpty() )
											{
												strTMP.Format( "RECV %s", m_strASCII );
												m_fnLogWrite( strTMP );
												strTMP.Format( "RECV %s", m_strHEXA );
												m_fnLogRvcDataWrite( strTMP );
											}
											m_strASCII.Empty();
											m_strHEXA.Empty();
											m_strASCII.FreeExtra();
											m_strHEXA.FreeExtra();
											m_nMsgCnt = 0;
											m_byData[m_nMsgCnt++] = aByte;
											m_strASCII  += (char)aByte;
											strCNV.Format( "%02x", aByte );
											m_strHEXA += strCNV;
											m_strHEXA += " ";
										}
										else
										{
											m_byData[m_nMsgCnt++] = aByte;
											m_strASCII  += aByte;
											strCNV.Format( "%02x", aByte );
											m_strHEXA += strCNV;
											m_strHEXA += " ";
										}
									}
								}
							}
						}
					}
				}
			}
			if ( aByte == SHARP )
			{
				strTMP.Format( "RECV %s", m_strASCII );
				m_fnLogWrite( strTMP );
				strTMP.Format( "RECV %s", m_strHEXA );
				m_fnLogRvcDataWrite( strTMP );
				m_strRecData.Add( (CString)m_byData );
				m_strASCII.Empty();
				m_strHEXA.Empty();
				m_strASCII.FreeExtra();
				m_strHEXA.FreeExtra();
				m_nMsgCnt = 0;
				memset( m_byData, 0x00, sizeof( m_byData ) );
			}
			if (( aByte == LF ) &&
				( m_byLastCH ))
			{
				if ( m_strASCII.GetLength() > 2 )
				{
					strTMP.Format( "RECV %s", m_strASCII );
					m_fnLogWrite( strTMP );
					strTMP.Format( "RECV %s", m_strHEXA );
					m_fnLogRvcDataWrite( strTMP );
					m_strRecData.Add( (CString)m_byData );
					m_strASCII.Empty();
					m_strHEXA.Empty();
					m_strASCII.FreeExtra();
					m_strHEXA.FreeExtra();
					m_nMsgCnt = 0;
					memset( m_byData, 0x00, sizeof( m_byData ) );
				}
			}
			m_byLastCH = aByte;
		}
	}
	else
	{
		return;
	}
}

UINT RcpDataThread(LPVOID pFuncData)
{
	CDSIInspectorControl		*pQI = (CDSIInspectorControl *)pFuncData;
	while ( pQI->m_bRcpDataEnd )
	{
		if ( pQI->m_hWnd != NULL )
		{
			if ( pQI->m_strRecData.GetCount() > 0 )
			{
				pQI->m_fnDataAnaylsys( pQI->m_strRecData.GetAt( 0 ));
				pQI->m_strRecData.RemoveAt( 0 );
				pQI->m_strRecData.FreeExtra();
			}
		}
		Sleep( AUTO_TIME );
	}
	if ( pQI->m_hWnd == NULL ) return 0;

	if ( pQI->m_hRcpDataThread != NULL )
	{
		SuspendThread(pQI->m_hRcpDataThread);
		CloseHandle(pQI->m_hRcpDataThread);
		pQI->m_hRcpDataThread	= NULL;
	}
	return 0;
}

/*-----------------------------------------------------------------------
Func Name : m_fnDataAnaylsys
Reference : 
-----------------------------------------------------------------------*/
BOOL CDSIInspectorControl::m_fnDataAnaylsys( CString strData )
{
	int nFind  = -1;
	int nStart = -1;
	int nEnd   = -1;
	BOOL bOK   = FALSE;
	CString strMsg("");
	CString strRecvData = strData;

	if ( strRecvData.Right(1) == LF )
	{
		if ( strRecvData.GetLength() <= 2 ) 
		{
			return FALSE;
		}
		strRecvData.Replace( CR, ' ' );
		strRecvData.Replace( LF, ' ' );
		strRecvData.Trim();
		if ( strRecvData.IsEmpty() )
		{
			return FALSE;
		}
		if ( strRecvData.Left(2) == "DE" )
		{
			strMsg.Format( "측정오류입니다. 확인후 진행해 주세요." );
			//TECmsgOut( m_Util.CStringTobstr(strMsg), GREEN_1, 1 );
			m_fnLogWrite( strMsg );
			m_bMeasure = true;
			m_bError   = true;
			return FALSE;
		}

		if ( strRecvData.Left(2) == "DN" )
		{
			strRecvData.Replace( "DN", "" );
			strRecvData.Trim();
			nFind = strRecvData.Find(",");	//	R
			if ( nFind == -1 ) 
			{
				m_bMeasure = true;
				m_bError   = true;
				return FALSE;
			}
			strRecvData.Delete( 0,1 );
			strRecvData.FreeExtra();
			if ( !m_fnSetEtoDecial( strRecvData.Left( nFind - 1), m_dR))
			{
				m_bMeasure = true;
				m_bError   = true;
				return FALSE;
			}
			strRecvData.Delete( 0,nFind );
			strRecvData.FreeExtra();
			nFind = strRecvData.Find(",");	// S
			if ( nFind == -1 ) 
			{
				m_bMeasure = true;
				m_bError   = true;
				return FALSE;
			}
			strRecvData.Delete( 0, 1 );
			strRecvData.FreeExtra();
			if ( !m_fnSetEtoDecial( strRecvData.Left( nFind - 1), m_dS) )
			{
				m_bMeasure = true;
				m_bError   = true;
				return FALSE;
			}
			strRecvData.Delete( 0,nFind );
			nFind = strRecvData.Find(",");	// V
			strRecvData.FreeExtra();
			if ( nFind == -1 ) 
			{
				m_bMeasure = true;
				m_bError   = true;
				return FALSE;
			}
			strRecvData.Delete( 0, 1 );
			strRecvData.FreeExtra();
			nFind = strRecvData.Find(",");	// V
			if ( !m_fnSetEtoDecial( strRecvData.Left( nFind -1 ), m_dV))
			{
				m_bMeasure = true;
				m_bError   = true;
				return FALSE;
			}
			strRecvData.Delete( 0, nFind + 2 );
			strRecvData.FreeExtra();
			if ( !m_fnSetEtoDecial( strRecvData, m_dC))
			{
				m_bMeasure = true;
				m_bError   = true;
				return FALSE;
			}
			m_bMeasure = true;
			m_bError   = false;
			return FALSE;
		}
		return FALSE;
	}

	if ( strRecvData.GetLength() <= 2 ) 
	{
		return FALSE;
	}

	if ( strRecvData.Left(1) != DOLLAR )
	{
		return FALSE;
	}

	if ( strRecvData.Right(1) != SHARP )
	{
		return FALSE;
	}
	strRecvData.Replace( SHARP, ' ' );
	strRecvData.Replace( DOLLAR, ' ' );
	strRecvData.Trim();
	if ( strRecvData.IsEmpty() )
	{
		return FALSE;
	}

	if ( strRecvData.Left(1) == "C" )
	{
		strRecvData.Replace( 'C', ' ' );
		strRecvData.Trim();
		return FALSE;
	}

	if ( strRecvData.Left(1) == "V" )
	{
		strRecvData.Delete( 0, 1 );
		strRecvData.Trim();
		return FALSE;
	}

	if ( strRecvData.Left(1) == "D" )
	{
		strRecvData.Replace( 'D', ' ' );
		strRecvData.Trim();
		m_dR = atof( strRecvData );
		m_bMeasure = true;
		m_bError   = false;
		return FALSE;
	}
	nFind = strRecvData.Find( "PO", 0 );
	if ( nFind != -1 )
	{
		strMsg.Format( "Probe Open상태입니다. 확인후 진행해 주세요." );
//		TECmsgOut( m_Util.CStringTobstr(strMsg), GREEN_1, 1 );
		m_fnLogWrite( strMsg );
		m_bMeasure = true;
		m_bError   = true;
		return FALSE;
	}

	nFind = strRecvData.Find( "PE", 0 );
	if ( nFind != -1 )
	{
		strMsg.Format( "Probe 접촉이상발생입니다. 확인후 진행해 주세요." );
//		TECmsgOut( m_Util.CStringTobstr(strMsg), GREEN_1, 1 );
		m_fnLogWrite( strMsg );
		m_bMeasure = true;
		m_bError   = true;
		return FALSE;
	}

	nFind = strRecvData.Find( "RO", 0 );
	if ( nFind != -1 )
	{
		strMsg.Format( "측정 범위(RANGE HIGH)를 벗어났습니다. 확인후 진행해 주세요." );
//		TECmsgOut( m_Util.CStringTobstr(strMsg), GREEN_1, 1 );
		m_fnLogWrite( strMsg );
		m_bMeasure = true;
		m_bError   = true;
		return FALSE;
	}

	nFind = strRecvData.Find( "RL", 0 );
	if ( nFind != -1 )
	{
		strMsg.Format( "측정 범위(RANGE LOW)를 벗어났습니다. 확인후 진행해 주세요." );
//		TECmsgOut( m_Util.CStringTobstr(strMsg), GREEN_1, 1 );
		m_fnLogWrite( strMsg );
		m_bMeasure = true;
		m_bError   = true;
		return FALSE;
	}

	return bOK;
}

BOOL CDSIInspectorControl::m_fnSetEtoDecial( CString strValue, double &dResult )
{
	double dValue = 0.0L;
	int    nValue = 0;
	int    nNo    = 0;

	nNo = strValue.Find( "E" );
	if ( nNo < 2 )
	{
		return FALSE;
	}
	dValue = atof(strValue.Left( nNo ));
	strValue.Delete( 0, nNo + 1 );
	strValue.FreeExtra();
	nValue  = atoi( strValue );
	dResult = dValue;
	switch( nValue )
	{
		case -2:
			{
				dResult /= 100;
			}
			break;
		case -1:
			{
				dResult /= 10;
			}
			break;
		case 1:
			{
				dResult *= 10;
			}
			break;
		case 2:
			{
				dResult *= 100;
			}
			break;
	}
	return TRUE;
}
/*-----------------------------------------------------------------------
Func Name : CharToHex
Reference : 
-----------------------------------------------------------------------*/
BOOL CDSIInspectorControl::CharToHex( char* szBuffer, int nLen, CString& strResult )
{
	strResult.Empty();
	CString strData("");

	if ( nLen == 0 ) return TRUE;

	for( int i = 0; i < nLen; i++ )
	{
		strData.Format( "%02x ", szBuffer[i] );
		strResult += strData;
	}
	return TRUE;
}

/*-----------------------------------------------------------------------
Func Name : m_fnSetResultDisplay
Reference : 
         0.001    은 1 m 입니다. 10^-3
         0.000001 은 1 u 입니다. 10^-6
      1000.00     은 1 k 입니다. 10^3
   1000000.0      은 1 M 입니다. 10^6
1000000000.0      은 1 G 입니다. 10^9
-----------------------------------------------------------------------*/
BOOL CDSIInspectorControl::m_fnSetResultDisplayR( double dValue, int &nDani, double &dResult )
{
	dResult = dValue;
	if ( dResult == 0.0 ) 
	{
		nDani = 0;
		return TRUE;
	}

	if ( dValue >= 1000000000.0 )
	{
		nDani = 3;
		dResult /= 1000000000.0;
	}
	else
	{
		if ( dValue >= 1000000.0 )
		{
			nDani = 2;
			dResult /= 1000000.0;
		}
		else
		{
			if ( dValue >= 1000.0 )
			{
				nDani = 1;
				dResult /= 1000.0;
			}
			else
			{
				if ( dValue >= 1. )
				{
					nDani = 0;
				}
				else
				{
					if ( dValue >= 0.001 )
					{
						nDani = 4;
						dResult *= 1000.0;
					}
					else
					{
						nDani = 5;
						dResult *= 100000.0;
					}
				}
			}
		}
	}
	return TRUE;
}
BOOL CDSIInspectorControl::m_fnSetResultDisplayS( double dValue, int &nDani, double &dResult )
{
	dResult = dValue;
	if ( dResult == 0.0 ) 
	{
		nDani = 0;
		return TRUE;
	}

	if ( dValue >= 1000000000.0 )
	{
		nDani = 3;
		dResult /= 1000000000.0;
	}
	else
	{
		if ( dValue >= 1000000.0 )
		{
			nDani = 2;
			dResult /= 1000000.0;
		}
		else
		{
			if ( dValue >= 1000.0 )
			{
				nDani = 1;
				dResult /= 1000.0;
			}
			else
			{
				if ( dValue >= 1. )
				{
					nDani = 0;
				}
				else
				{
					if ( dValue >= 0.001 )
					{
						nDani = 4;
						dResult *= 1000.0;
					}
					else
					{
						nDani = 5;
						dResult *= 100000.0;
					}
				}
			}
		}
	}
	return TRUE;
}
BOOL CDSIInspectorControl::m_fnSetResultDisplayV( double dValue, int &nDani, double &dResult )
{
	dResult = dValue;
	if ( dResult == 0.0 ) 
	{
		nDani = 0;
		return TRUE;
	}

	if ( dValue >= 1000000000.0 )
	{
		nDani = 3;
		dResult /= 1000000000.0;
	}
	else
	{
		if ( dValue >= 1000000.0 )
		{
			nDani = 2;
			dResult /= 1000000.0;
		}
		else
		{
			if ( dValue >= 1000.0 )
			{
				nDani = 1;
				dResult /= 1000.0;
			}
			else
			{
				if ( dValue >= 1. )
				{
					nDani = 0;
				}
				else
				{
					if ( dValue >= 0.001 )
					{
						nDani = 4;
						dResult *= 1000.0;
					}
					else
					{
						nDani = 5;
						dResult *= 100000.0;
					}
				}
			}
		}
	}
	return TRUE;
}
BOOL CDSIInspectorControl::m_fnSetResultDisplayC( double dValue, int &nDani, double &dResult )
{
	dResult = dValue;
	if ( dResult == 0.0 ) 
	{
		nDani = 0;
		return TRUE;
	}

	if ( dValue >= 1000000000.0 )
	{
		nDani = 3;
		dResult /= 1000000000.0;
	}
	else
	{
		if ( dValue >= 1000000.0 )
		{
			nDani = 2;
			dResult /= 1000000.0;
		}
		else
		{
			if ( dValue >= 1000.0 )
			{
				nDani = 1;
				dResult /= 1000.0;
			}
			else
			{
				if ( dValue >= 1. )
				{
					nDani = 0;
				}
				else
				{
					if ( dValue >= 0.001 )
					{
						nDani = 4;
						dResult *= 1000.0;
					}
					else
					{
						nDani = 5;
						dResult *= 100000.0;
					}
				}
			}
		}
	}
	return TRUE;
}

//---------------------------------------------------------------------------------------------------------
// Log 파일을 저장한다.
//---------------------------------------------------------------------------------------------------------
void CDSIInspectorControl::m_fnLogWrite( CString strMsg )
{
	CString strCnt("");
	int     nCnt		= 0;
	int     nTotal		= 0;

	memset( m_szData, 0x00, sizeof( m_szData ));

	sprintf_s( m_szData, "%s", strMsg );
	if ( theApp.m_fnGetViewPointer()->m_pTask == NULL ) return;
	if ( theApp.m_fnGetViewPointer()->m_pTask->stCommon.bLogSave )
	{
		m_Log.DEBUG_LOG( m_szData );
	}

	if ( m_hWnd == NULL )
	{
		return;
	}
	if ( m_hWnd == NULL )
	{
		return;
	}

	if ( strMsg == m_strLog ) return;

	m_strLog = strMsg;

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
// Log 파일을 저장한다.
//---------------------------------------------------------------------------------------------------------
void CDSIInspectorControl::m_fnLogRvcDataWrite( CString strMsg )
{
	CString strCnt("");
	int     nCnt		= 0;
	int     nTotal		= 0;

	memset( m_szData, 0x00, sizeof( m_szData ));

	sprintf_s( m_szData, "%s", strMsg );
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

void CDSIInspectorControl::OnBtnClr() 
{
	m_lstComm.ResetContent();	
	m_lstRcvComm.ResetContent();	
}

void CDSIInspectorControl::OnBtnLineSet() 
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
	strMsg.Format( "통신로그 라인수 변경 = %d" , nCnt );
	m_fnLogWrite( strMsg );

	UpdateData(FALSE);
}

void CDSIInspectorControl::m_fnPortOpen() 
{
	BOOL bOnLine = ((CButton*)GetDlgItem( IDC_CHK_ONLINE ))->GetCheck( );

	if ( bOnLine )
	{
		if ( m_Multiportxx.IsOpenPort( m_PortID ) )
		{
			m_Multiportxx.ClosePort( m_PortID );
		}
		m_fnLogWrite( "Port CLOSE 성공" );
		m_bConnect = FALSE;
		((CButton*)GetDlgItem( IDC_CHK_ONLINE ))->SetCheck( m_bConnect );
	}

	Sleep( 100 );

	CString strPort("");
	strPort.Format( "COM%d",m_stComm.nComPort );
	if ( m_Multiportxx.OpenPort( m_PortID, 
										m_stComm.nComPort,
										m_stComm.nBaudRate,
										m_stComm.nParityBit,
										m_stComm.nDataBit,
										m_stComm.nStopBit,
										m_stComm.nFlowCtrl ) == 1 )
	{
		m_bConnect = TRUE;
		m_fnLogWrite( "PORT 오픈 성공" );
		m_byLastCH = NULL;
	}
	else
	{
		m_bConnect = FALSE;
		m_fnLogWrite( "PORT 오픈 실패" );
	}
	((CButton*)GetDlgItem( IDC_CHK_ONLINE ))->SetCheck(  m_bConnect );
}

void CDSIInspectorControl::OnBtnOpen() 
{
	m_fnPortOpen();
}

//------------------------------------------------------------------------
// 장비 구동에 필요한 폴더를 생성한다.
//------------------------------------------------------------------------
BOOL CDSIInspectorControl::m_fnSetMakeFolder()
{
	CString strDirve("");

	// 현재 사용중인 드라이브 정보를 설정한다.
	strDirve = m_fnGetCurrentDrive();

	// 환경파일 폴더를 생성한다. 
	char szFullPath[MAX_PATH + 1];
	memset( szFullPath, 0x00, sizeof( szFullPath ));

	sprintf_s( szFullPath, "%s%s%s", strDirve,
										DEFAULT_PATH,
										DEFAULT_ENV_PATH );
	szFullPath[MAX_PATH] = NULL;
	m_fnSetCreateDir( szFullPath );

	return TRUE;
}

//---------------------------------------------------------------------------------
// 폴더를 생성한다.
//---------------------------------------------------------------------------------
int CDSIInspectorControl::m_fnSetCreateDir(CString strFolder)
{
	CFileStatus	clStatus;
	CString		strDir("");
	int			nIndex = 0;

	// Space Charactor Del
	while ( ( nIndex = strFolder.Find(STR_SPACE) ) >= 0 )
	{
		strFolder = strFolder.Left( nIndex ) + strFolder.Right( strFolder.GetLength() - nIndex - 1 );
	}

	if ( ( nIndex = strFolder.Find(STR_COLON) ) >= 0 )
	{
		// Get Drive Name
		strDir      = strFolder.Left( nIndex + 1 );
		strFolder = strFolder.Right(strFolder.GetLength() - nIndex - 1 );
	}
	if ( strFolder.Find(STR_BACKSLASH) == 0 )
	{
		strDir     += STR_BACKSLASH;
		strFolder = strFolder.Right( strFolder.GetLength() - 1 );
	}

	while ( ( nIndex = strFolder.Find(STR_BACKSLASH) ) >= 0 )
	{
		strDir     += strFolder.Left( nIndex );
		strFolder = strFolder.Right( strFolder.GetLength() - nIndex - 1 );

		if( !CFile::GetStatus( (const char*)strDir, clStatus ) )
		{
			// Not Found Folder
			if ( CreateDirectoryA( (const char*)strDir, RC_NORMAL ) != 0 )
			{
				// 이미 폴더 존재시 하위 폴더를 작성 한다
				// 폴더가 존재시 진행을 멈추려면 하기 스텝의 Comment를 제거한다.
				// return RC_FILE_ACCESS_ERR;
			}
		}
		strDir += STR_BACKSLASH;
	}
	return RC_NORMAL;

}

//------------------------------------------------------------------------
// EFU 파일의 경로를 읽어온다.
//------------------------------------------------------------------------
CString CDSIInspectorControl::m_fnGetDSIFileFullPath()
{
	char szFullPath[MAX_PATH + 1];
	memset( szFullPath, 0x00, sizeof( szFullPath ));

	sprintf_s( szFullPath, "%s%s%s%s",  m_fnGetCurrentDrive(),
										DEFAULT_PATH,
										DEFAULT_ENV_PATH,
										DEFAULT_DSI_FILE );
	szFullPath[MAX_PATH] = NULL;

	return szFullPath;
}

//------------------------------------------------------------------------
// 설정 환경파일을 저장한다.
//------------------------------------------------------------------------
BOOL CDSIInspectorControl::m_fnSetDSI_Save( )
{
	CString strWork("");
	CString strData("");
	CString strFullPath("");
	int     i 			= 0;
	int     nNo			= 0;

	CStringArray strLst;
	CXmlElement *pElement     = NULL;
	CXmlElement *pRootElement = NULL;

	strFullPath		=	m_fnGetDSIFileFullPath();

	m_fnLogWrite( "Comm Data 저장" );

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
	strLst.Add(RS232C);
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

		pXML->Store( strFullPath );
	}

	delete pXML;
	pXML = NULL;

	m_fnLogWrite( "Comm Data 저장 완료" );

	return TRUE;
}

//------------------------------------------------------------------------
// 설정 환경파일을 로드한다.
//------------------------------------------------------------------------
BOOL CDSIInspectorControl::m_fnGetDSI_Load( )
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

	strFullPath					=	m_fnGetDSIFileFullPath();

	m_fnLogWrite( "Comm Data 읽기" );


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
	strLst.Add(RS232C);
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

	delete pXML;
	pXML = NULL;

	m_fnLogWrite( "Comm Data 읽기 완료" );

	return TRUE;
}

void CDSIInspectorControl::OnBnClickedBtnClose()
{
	if ( m_Multiportxx.IsOpenPort( m_PortID ) )
	{
		m_Multiportxx.ClosePort( m_PortID );
	}
	m_fnLogWrite( "Port CLOSE 성공" );
	m_bConnect = FALSE;
	m_byLastCH = NULL;
	((CButton*)GetDlgItem( IDC_CHK_ONLINE ))->SetCheck( m_bConnect );
}

void CDSIInspectorControl::OnBnClickedBtnSend()
{
	CString strSend("");
	GetDlgItem( IDC_EDT_DATA )->GetWindowText( strSend );
	m_fnSetSend( strSend );
}

//----------------------------------------------------------------------------
// 외부에서 $ CMD #을 만들어서 Send
//----------------------------------------------------------------------------
BOOL CDSIInspectorControl::m_fnSetSend( BYTE* byBuffer, int nLen, BOOL bDisp )
{
	CString strResult("");
	CString strMSG("");
	if ( m_Multiportxx.IsOpenPort( m_PortID ) )
	{
		// ASCII 로그 저장
		for ( int i = 0; i < nLen; i++ )
		{
			if ( byBuffer[i] == NULL )
			{
				strMSG += "<NULL>";
			}
			else
			{
				if ( byBuffer[i] == SOH )
				{
					strMSG += "<SOH>";
				}
				else
				{
					if ( byBuffer[i] == EOT )
					{
						strMSG += "<EOT>";
					}
					else
					{
						if ( byBuffer[i] == STX )
						{
							strMSG += "<STX>";
						}
						else
						{
							if ( byBuffer[i] == ETX )
							{
								strMSG += "<ETX>";
							}
							else
							{
								strMSG += (char)byBuffer[i]; 
							}
						}
					}
				}

			}
		}

		if ( bDisp )
		{
			strResult.Format( "SEND %s", strMSG );
			m_fnLogWrite( strResult );

			// HEXA 로그 저장
			CharToHex( (char*)byBuffer, nLen, strResult );
			strMSG.Format( "SEND %s", strResult );
			m_fnLogRvcDataWrite( strMSG );
		}

		if  ( (m_Multiportxx.Send( m_PortID, (long)byBuffer, nLen)) != 1 )
		{
			m_fnLogWrite( "SEND 처리 실패" );
			return FALSE;
		}
		else
		{
			if ( bDisp )
			{
				m_fnLogWrite( "SEND 처리 성공" );
			}
		}
	}
	else
	{
		strMSG.Format( "%s(%s)","포트가 닫혀서 WRITE COMMAND 발행 불가!!!!!", (char*)byBuffer ); 
		m_fnLogWrite( strMSG );
		return FALSE;
	}
	return TRUE;
}
//--------------------------------------------------------------------
// 내부적으로 $ CMD #을 붙인다.
//--------------------------------------------------------------------
void CDSIInspectorControl::m_fnSetSend( CString strData, BOOL bDisp )
{
	CString strResult("");
	CString strMSG("");
	if ( m_Multiportxx.IsOpenPort( m_PortID ) )
	{
		BYTE    byBuffer[128];
		CString strTEMP("");

		// strTEMP.Format( "%c%c%s%c%c", LF, DOLLAR, strData, SHARP, CR );
		strTEMP.Format( "%c%s%c", DOLLAR, strData, SHARP );
		strMSG  += strTEMP;

		int nLen = strMSG.GetLength();
		memset( byBuffer, 0x00, sizeof( byBuffer ));
		sprintf( (char*)byBuffer, "%s", strMSG );

		if ( bDisp )
		{
			// ASCII 로그 저장
			strResult.Format( "SEND %s", strMSG );
			m_fnLogWrite( strResult );

			// HEXA 로그 저장
			CharToHex( (char*)byBuffer, nLen, strResult );
			strMSG.Format( "SEND %s", strResult );
			m_fnLogRvcDataWrite( strMSG );
		}
		if  ( (m_Multiportxx.Send( m_PortID, (long)byBuffer, nLen)) != 1 )
		{
			m_fnLogWrite( "SEND 처리 실패" );
		}
		else
		{
			if ( bDisp )
			{
				m_fnLogWrite( "SEND 처리 성공" );
			}
		}
	}
	else
	{
		strMSG.Format( "%s(%s)","포트가 닫혀서 WRITE COMMAND 발행 불가!!!!!", strData ); 
		m_fnLogWrite( strMSG );
	}
}

void CDSIInspectorControl::OnBnClickedBtnLf()
{
	CString strMSG("");
	GetDlgItem( IDC_EDT_DATA )->GetWindowText( strMSG );
	strMSG += (char)LF;
	GetDlgItem( IDC_EDT_DATA )->SetWindowText( strMSG );
}


void CDSIInspectorControl::OnBnClickedBtnCr()
{
	CString strMSG("");
	GetDlgItem( IDC_EDT_DATA )->GetWindowText( strMSG );
	strMSG += (char)CR;
	GetDlgItem( IDC_EDT_DATA )->SetWindowText( strMSG );
}


void CDSIInspectorControl::OnBnClickedBtnEot()
{
	CString strMSG("");
	GetDlgItem( IDC_EDT_DATA )->GetWindowText( strMSG );
	strMSG += (char)EOT;
	GetDlgItem( IDC_EDT_DATA )->SetWindowText( strMSG );
}


void CDSIInspectorControl::OnBnClickedBtnSoh()
{
	CString strMSG("");
	GetDlgItem( IDC_EDT_DATA )->GetWindowText( strMSG );
	strMSG += (char)SOH;
	GetDlgItem( IDC_EDT_DATA )->SetWindowText( strMSG );
}



void CDSIInspectorControl::OnBnClickedBtnMeasureMode()
{
	int nMode = m_clMeasureMode.GetCurSel();
	if ( nMode != -1 )
	{
		m_fnSetMeasureMode( nMode );
	}
}

void CDSIInspectorControl::m_fnSetMeasureMode( int nMode )
{
	if (( nMode != MM ) &&
	    ( nMode != MA ) &&
	    ( nMode != Mc ))
	{
		m_fnLogWrite( "ERR> Measure Mode Parameter Error" );
		return;
	}

	m_fnSetSend( strMMAcCmd[nMode] );
}

void CDSIInspectorControl::OnBnClickedBtnResisterMode()
{
	int nMode = m_clResisterMode.GetCurSel();
	if ( nMode != -1 )
	{
		m_fnSetResisterMode( nMode );
	}
}

void CDSIInspectorControl::m_fnSetResisterMode( int nMode )
{
	if (( nMode != MR ) &&
	    ( nMode != MS ) &&
	    ( nMode != MV ) &&
	    ( nMode != MG ))
	{
		m_fnLogWrite( "ERR> Resister Mode Parameter Error" );
		return;
	}

	m_fnSetSend( strMSRVGCmd[nMode] );
}


void CDSIInspectorControl::OnBnClickedBtnMxMode()
{
	int nMode = m_clMxMode.GetCurSel();
	if ( nMode != -1 )
	{
		m_fnSetMxMode( nMode + 1 );
	}
}

void CDSIInspectorControl::m_fnSetMxMode( int nMode )
{
	if (( nMode != M1 ) &&
	    ( nMode != M3 ) &&
	    ( nMode != M4 ) &&
	    ( nMode != M5 ))
	{
		m_fnLogWrite( "ERR> Mx Mode Parameter Error" );
		return;
	}

	m_fnSetSend( strM1345Cmd[nMode - 1] );
}


void CDSIInspectorControl::OnBnClickedBtnMctMode()
{
	int nMode = m_clMctMode.GetCurSel();
	if ( nMode != -1 )
	{
		m_fnSetMctMode( nMode );
	}
}

void CDSIInspectorControl::m_fnSetMctMode( int nMode )
{
	if (( nMode != MC ) &&
	    ( nMode != MT ))
	{
		m_fnLogWrite( "ERR> MCT Mode Parameter Error" );
		return;
	}

	m_fnSetSend( strMCTCmd[nMode] );
}


void CDSIInspectorControl::OnBnClickedBtnMpSpeed()
{
	int nMode = m_clMpMeasureSpeed.GetCurSel();
	if ( nMode != -1 )
	{
		m_fnSetMPMeasureSpeed( nMode );
	}
}

void CDSIInspectorControl::m_fnSetMPMeasureSpeed( int nSpeed )
{
	if (( nSpeed < MP1 ) &&
	    ( nSpeed > MP9 ))
	{
		m_fnLogWrite( "ERR> MP Measure Speed Parameter Error" );
		return;
	}

	m_fnSetSend( strM19Cmd[nSpeed] );
}


void CDSIInspectorControl::OnBnClickedBtnReMeasureDataReq()
{
	m_fnSetReMeasureDataReq( );
}

void CDSIInspectorControl::m_fnSetReMeasureDataReq( )
{
	m_fnSetSend( RD );
}

void CDSIInspectorControl::OnBnClickedBtnDelayTime()
{
	CString strDelayTime("");
	int nDelayTime = 0;
	GetDlgItem( IDC_EDT_DELAY_TIME )->GetWindowText( strDelayTime );
	nDelayTime = atoi( strDelayTime );
	m_fnSetMeasureDelayTime( nDelayTime );
}

void CDSIInspectorControl::m_fnSetMeasureDelayTime( int nDelayTime )
{
	CString strDelayTime("");
	if (( nDelayTime < 0 ) ||
		( nDelayTime > MAX_DELAY_TIME ))
	{
		m_fnLogWrite( "ERR> Pxxxx Measure Delay Time Parameter Error" );
		return;
	}

	strDelayTime.Format( "P%04d", nDelayTime );
	m_fnSetSend( strDelayTime );
}

void CDSIInspectorControl::OnBnClickedBtnHighLimit()
{
	int nMode = m_clHighLimit.GetCurSel();
	if ( nMode != -1 )
	{
		m_fnSetHighLimit( nMode );
	}
}

void CDSIInspectorControl::OnBnClickedBtnLowLimit()
{
	int nMode = m_clLowLimit.GetCurSel();
	if ( nMode != -1 )
	{
		m_fnSetLowLimit( nMode );
	}
}

void CDSIInspectorControl::OnBnClickedBtnCurrentStep()
{
	int nMode = m_clCurrStep.GetCurSel();
	if ( nMode != -1 )
	{
		m_fnSetCurrentStep( nMode );
	}
}

void CDSIInspectorControl::m_fnSetHighLimit( int nStep )
{
	if (( nStep < RHL9 ) ||
		( nStep > RHL1 ))
	{
		m_fnLogWrite( "ERR> High Limit ( 1 ~ 9 ) Parameter Error" );
		return;
	}
	m_fnSetSend( strHCmd[nStep] );
}

void CDSIInspectorControl::m_fnSetLowLimit( int nStep )
{
	if (( nStep < RHL9 ) ||
		( nStep > RHL1 ))
	{
		m_fnLogWrite( "ERR> Low Limit ( 1 ~ 9 ) Parameter Error" );
		return;
	}
	m_fnSetSend( strLCmd[nStep] );
}

void CDSIInspectorControl::m_fnSetCurrentStep( int nStep )
{
	if (( nStep < RHL9 ) ||
		( nStep > RHL1 ))
	{
		m_fnLogWrite( "ERR> Current Step ( 1 ~ 9 ) Parameter Error" );
		return;
	}
	m_fnSetSend( strRCmd[nStep] );
}


void CDSIInspectorControl::OnBnClickedBtnMeasureStart()
{
	m_fnSetMeasureStart( );
}

void CDSIInspectorControl::m_fnSetMeasureStart( )
{
	m_bMeasure = FALSE;
	m_bError   = FALSE;

	m_dR		= 0.0;
	m_dS		= 0.0;
	m_dV		= 0.0;
	m_dC		= 0.0;
	m_fnSetSend( MEASURE_START );
}

void CDSIInspectorControl::OnBnClickedBtnMeasurePause()
{
	m_fnSetMeasurePause( );
}

void CDSIInspectorControl::m_fnSetMeasurePause( )
{
	m_fnSetSend( MEASURE_PAUSE );
}


void CDSIInspectorControl::OnBnClickedBtnRemoteControlEnable()
{
	m_fnSetRemoteControlEnable();
}


void CDSIInspectorControl::OnBnClickedBtnRemoteControlDisable()
{
	m_fnSetRemoteControlDisable();
}


void CDSIInspectorControl::OnBnClickedBtnBeepOn()
{
	m_fnSetBeepOn();
}


void CDSIInspectorControl::OnBnClickedBtnBeepOff()
{
	m_fnSetBeepOff();
}

void CDSIInspectorControl::m_fnSetRemoteControlEnable()
{
	m_fnSetSend( REMOTE_CONTROL_ENABLE );
}


void CDSIInspectorControl::m_fnSetRemoteControlDisable()
{
	m_fnSetSend( REMOTE_CONTROL_DISABLE );
}


void CDSIInspectorControl::m_fnSetBeepOn()
{
	m_fnSetSend( BEEP_ON );
}


void CDSIInspectorControl::m_fnSetBeepOff()
{
	m_fnSetSend( BEEP_OFF );
}


void CDSIInspectorControl::OnBnClickedBtnCurrentMeasureStepNoReq()
{
	m_fnSetCurrentMeasureStepNoReq();
}


void CDSIInspectorControl::OnBnClickedBtnBeforeMeasureData()
{
	m_fnSetBeforeMeasureData();
}

void CDSIInspectorControl::m_fnSetCurrentMeasureStepNoReq()
{
	m_fnSetSend( CURR_MEASURE_STEP_NO_REQ );
}


void CDSIInspectorControl::m_fnSetBeforeMeasureData()
{
	m_fnSetSend( BEFORE_MEASURE_DATA );
}


void CDSIInspectorControl::OnBnClickedBtnOnly1DataSend()
{
	m_fnSetOnly1DataSend();
}


void CDSIInspectorControl::OnBnClickedBtnRsvcSend()
{
	m_fnSetRsvcSend();
}


void CDSIInspectorControl::OnBnClickedBtnDataMemoryClear()
{
	m_fnSetDataMemoryClear();
}


void CDSIInspectorControl::OnBnClickedBtnRequestMemoryData()
{
	m_fnSetRequestMemoryData();
}

void CDSIInspectorControl::m_fnSetOnly1DataSend()
{
	m_fnSetSend( ONLY_1_DATA );
}

void CDSIInspectorControl::m_fnSetRsvcSend()
{
	m_fnSetSend( RSVC_4_DATA );
}

void CDSIInspectorControl::m_fnSetDataMemoryClear()
{
	m_fnSetSend( DATA_MEMORY_CLEAR );
}

void CDSIInspectorControl::m_fnSetRequestMemoryData()
{
	m_fnSetSend( REQUEST_MEMORY_DATA );
}
#endif