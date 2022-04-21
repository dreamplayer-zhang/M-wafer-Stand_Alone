// Common.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EllipsoMeter.h"
#include "Common.h"
#include "afxdialogex.h"
#include "CommThread.h"
#include "ATXMotion.h"


// CCommon 대화 상자입니다.

IMPLEMENT_DYNAMIC(CCommon, CDialogEx)

CCommon::CCommon(CWnd* pParent /*=NULL*/)
						: CDialogEx(CCommon::IDD, pParent)
	, m_iEditLightVal(0)
{
	m_bShow				= FALSE;

	this->Create( IDD_COMMON_DLG );
	m_hCommonMoveThread = NULL;
	m_bCommonMoveEnd	= FALSE;
}

CCommon::~CCommon()
{
}

void CCommon::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDT_LIGHT_VALUE, m_iEditLightVal);
	DDV_MinMaxInt(pDX, m_iEditLightVal, 0, 255);
}


BEGIN_MESSAGE_MAP(CCommon, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_EXIT, &CCommon::OnBnClickedBtnExit)
	ON_BN_CLICKED(IDC_BTN_COMMON_SAVE, &CCommon::OnBnClickedBtnCommonSave)
	ON_BN_CLICKED(IDC_BTN_X_M_LIMIT, &CCommon::OnBnClickedBtnXMLimit)
	ON_BN_CLICKED(IDC_BTN_X_P_LIMIT, &CCommon::OnBnClickedBtnXPLimit)
	ON_BN_CLICKED(IDC_BTN_Y_M_LIMIT, &CCommon::OnBnClickedBtnYMLimit)
	ON_BN_CLICKED(IDC_BTN_Y_P_LIMIT, &CCommon::OnBnClickedBtnYPLimit)
	ON_EN_CHANGE(IDC_EDT_HIGH_SPEED, &CCommon::OnEnChangeEdtHighSpeed)
	ON_EN_CHANGE(IDC_EDT_MIDDLE_SPEED, &CCommon::OnEnChangeEdtMiddleSpeed)
	ON_EN_CHANGE(IDC_EDT_SLOW_SPEED, &CCommon::OnEnChangeEdtSlowSpeed)
	ON_EN_CHANGE(IDC_EDT_HIGH_SPEED_Z, &CCommon::OnEnChangeEdtHighSpeedZ)
	ON_EN_CHANGE(IDC_EDT_MIDDLE_SPEED_Z, &CCommon::OnEnChangeEdtMiddleSpeedZ)
	ON_EN_CHANGE(IDC_EDT_SLOW_SPEED_Z, &CCommon::OnEnChangeEdtSlowSpeedZ)
	ON_BN_CLICKED(IDC_CHK_LOG_SAVE, &CCommon::OnBnClickedChkLogSave)
	ON_BN_CLICKED(IDC_BTN_LOADING_READ, &CCommon::OnBnClickedBtnLoadingRead)
	ON_BN_CLICKED(IDC_BTN_LOADING_MOVE, &CCommon::OnBnClickedBtnLoadingMove)
	ON_BN_CLICKED(IDC_BTN_Z_LIMIT_READ, &CCommon::OnBnClickedBtnZLimitRead)
	ON_BN_CLICKED(IDC_BTN_Z_LIMIT_MOVE, &CCommon::OnBnClickedBtnZLimitMove)
	ON_BN_CLICKED(IDC_BTN_Z_ALIGN_READ, &CCommon::OnBnClickedBtnZAlignRead)
	ON_BN_CLICKED(IDC_BTN_MEASURE_READ, &CCommon::OnBnClickedBtnMeasureRead)
	ON_BN_CLICKED(IDC_BTN_Z_MEASURE_MOVE, &CCommon::OnBnClickedBtnZMeasureMove)
	ON_BN_CLICKED(IDC_BTN_Z_ALIGN_MOVE, &CCommon::OnBnClickedBtnZAlignMove)
	ON_BN_CLICKED(IDC_BTN_Z_ALIGN_SAFETY_READ, &CCommon::OnBnClickedBtnZAlignSafetyRead)
	ON_BN_CLICKED(IDC_BTN_Z_ALIGN_SAFETY_MOVE, &CCommon::OnBnClickedBtnZAlignSafetyMove)
	ON_BN_CLICKED(IDC_BTN_Z_MEASURE_SAFETY_READ, &CCommon::OnBnClickedBtnZMeasureSafetyRead)
	ON_BN_CLICKED(IDC_BTN_Z_MEASURE_SAFETY_MOVE, &CCommon::OnBnClickedBtnZMeasureSafetyMove)
	ON_BN_CLICKED(IDC_BTN_NORCH_4INCH_READ, &CCommon::OnBnClickedBtnNorch4inchRead)
	ON_BN_CLICKED(IDC_BTN_NORCH_6INCH_READ, &CCommon::OnBnClickedBtnNorch6inchRead)
	ON_BN_CLICKED(IDC_BTN_NORCH_8INCH_READ, &CCommon::OnBnClickedBtnNorch8inchRead)
	ON_BN_CLICKED(IDC_BTN_NORCH_12INCH_READ, &CCommon::OnBnClickedBtnNorch12inchRead)
	ON_BN_CLICKED(IDC_BTN_FLAT_4INCH_READ, &CCommon::OnBnClickedBtnFlat4inchRead)
	ON_BN_CLICKED(IDC_BTN_FLAT_6INCH_READ, &CCommon::OnBnClickedBtnFlat6inchRead)
	ON_BN_CLICKED(IDC_BTN_FLAT_8INCH_READ, &CCommon::OnBnClickedBtnFlat8inchRead)
	ON_BN_CLICKED(IDC_BTN_FLAT_12INCH_READ, &CCommon::OnBnClickedBtnFlat12inchRead)
	ON_BN_CLICKED(IDC_BTN_NORCH_4INCH_MOVE, &CCommon::OnBnClickedBtnNorch4inchMove)
	ON_BN_CLICKED(IDC_BTN_NORCH_6INCH_MOVE, &CCommon::OnBnClickedBtnNorch6inchMove)
	ON_BN_CLICKED(IDC_BTN_NORCH_8INCH_MOVE, &CCommon::OnBnClickedBtnNorch8inchMove)
	ON_BN_CLICKED(IDC_BTN_NORCH_12INCH_MOVE, &CCommon::OnBnClickedBtnNorch12inchMove)
	ON_BN_CLICKED(IDC_BTN_FLAT_4INCH_MOVE, &CCommon::OnBnClickedBtnFlat4inchMove)
	ON_BN_CLICKED(IDC_BTN_FLAT_6INCH_MOVE, &CCommon::OnBnClickedBtnFlat6inchMove)
	ON_BN_CLICKED(IDC_BTN_FLAT_8INCH_MOVE, &CCommon::OnBnClickedBtnFlat8inchMove)
	ON_BN_CLICKED(IDC_BTN_FLAT_12INCH_MOVE, &CCommon::OnBnClickedBtnFlat12inchMove)
	ON_BN_CLICKED(IDC_BTN_LOADING_READ2, &CCommon::OnBnClickedBtnLoadingRead2)
	ON_BN_CLICKED(IDC_BTN_LOADING_MOVE2, &CCommon::OnBnClickedBtnLoadingMove2)
	ON_BN_CLICKED(IDC_BTN_EXCEL_PATH, &CCommon::OnBnClickedBtnExcelPath)
	ON_BN_CLICKED(IDC_BTN_PATH_XML, &CCommon::OnBnClickedBtnPathXml)
	ON_BN_CLICKED(IDC_BTN_TREND_PATH, &CCommon::OnBnClickedBtnTrendPath)

	ON_BN_CLICKED(IDC_BTN_LIGHT_VALUE, &CCommon::OnBnClickedBtnLightValue)
END_MESSAGE_MAP()

BOOL CCommon::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (( nID   == 2 ) &&
		( nCode == -1 ))
	{
		m_fnSetShowFlag( FALSE );
	}
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CCommon::m_fnSetShowFlag( BOOL bShow )
{
	m_bShow = bShow;
	if ( m_bShow )
	{
		m_fnSetView();
		this->ShowWindow( SW_SHOW );
	}
	else
	{
		this->ShowWindow( SW_HIDE );
	}
}

BOOL CCommon::PreTranslateMessage(MSG* pMsg) 
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
void CCommon::m_fnSetInit( ) 
{
	CString s("");
	s.Format( "공통 파라메터 관리" );
	this->SetWindowText(s);

	this->MoveWindow( 150, 100, 900,860, TRUE );

	m_pTaskHandle = new CTaskHandle( this, SHARED_NAME, 
									 1, 
									 sizeof( SHEET_RESISTANCE_Format ) + 1,
									 0,
									 FALSE );

	m_pTask					= ( SHEET_RESISTANCE_Format* )m_pTaskHandle->m_pFilePointer( );

	m_fnSetView();

}

void CCommon::m_fnSetView() 
{
	m_fnGetCommon_Load();

	m_fnGetLastModel_Load();

	CString strData("");
	strData.Format( "%7.3f", m_pTask->stCommon.dSpeedHigh );
	GetDlgItem( IDC_EDT_HIGH_SPEED )->SetWindowText( strData );
	strData.Format( "%7.3f", m_pTask->stCommon.dSpeedMiddle );
	GetDlgItem( IDC_EDT_MIDDLE_SPEED )->SetWindowText( strData );
	strData.Format( "%7.3f", m_pTask->stCommon.dSpeedSlow );
	GetDlgItem( IDC_EDT_SLOW_SPEED )->SetWindowText( strData );
	strData.Format("%7.3f", m_pTask->stCommon.dSpeedHighZ);
	GetDlgItem(IDC_EDT_HIGH_SPEED_Z)->SetWindowText(strData);
	strData.Format("%7.3f", m_pTask->stCommon.dSpeedMiddleZ);
	GetDlgItem(IDC_EDT_MIDDLE_SPEED_Z)->SetWindowText(strData);
	strData.Format("%7.3f", m_pTask->stCommon.dSpeedSlowZ);
	GetDlgItem(IDC_EDT_SLOW_SPEED_Z)->SetWindowText(strData);

	strData.Format("%7.3f", m_pTask->stCommon.dLoadingX);
	GetDlgItem(IDC_EDT_LOADING_X)->SetWindowText(strData);
	strData.Format("%7.3f", m_pTask->stCommon.dLoadingY);
	GetDlgItem(IDC_EDT_LOADING_Y)->SetWindowText(strData);

	strData.Format("%7.3f", m_pTask->stCommon.dLoadingX2);
	GetDlgItem(IDC_EDT_LOADING_X2)->SetWindowText(strData);
	strData.Format("%7.3f", m_pTask->stCommon.dLoadingY2);
	GetDlgItem(IDC_EDT_LOADING_Y2)->SetWindowText(strData);

	strData.Format( "%7.3f", m_pTask->stCommon.dInspXYSpeed );
	GetDlgItem( IDC_EDT_XY_INSP_SPEED )->SetWindowText( strData );
	strData.Format( "%7.3f", m_pTask->stCommon.dInspZSpeed );
	GetDlgItem( IDC_EDT_Z_INSP_SPEED )->SetWindowText( strData );
	strData.Format( "%01d", m_pTask->stCommon.nInspLight );
	GetDlgItem( IDC_EDT_LIGHT_VALUE )->SetWindowText( strData );
	
	((CButton*)GetDlgItem( IDC_CHK_ALARM_MELODY_ON ))->SetCheck( m_pTask->stCommon.bAlarmOn );
	((CButton*)GetDlgItem( IDC_CHK_EXCEL_SAVE ))->SetCheck( m_pTask->stCommon.bExcelSave );
	((CButton*)GetDlgItem( IDC_CHK_LOG_SAVE ))->SetCheck( m_pTask->stCommon.bLogSave );
	((CButton*)GetDlgItem( IDC_CHK_CHART_INCLUDE ))->SetCheck( m_pTask->stCommon.bChartReport );

	strData.Format( "%7.3f", m_pTask->stCommon.dXMLimit );
	GetDlgItem( IDC_EDT_X_M_LIMIT )->SetWindowText( strData );
	strData.Format( "%7.3f", m_pTask->stCommon.dXPLimit );
	GetDlgItem( IDC_EDT_X_P_LIMIT )->SetWindowText( strData );
	if(pAxis[X_AXIS]!=NULL)
		pAxis[X_AXIS]->SetSoftLimit((float)m_pTask->stCommon.dXPLimit, (float)m_pTask->stCommon.dXMLimit);
	
	strData.Format( "%7.3f", m_pTask->stCommon.dYMLimit );
	GetDlgItem( IDC_EDT_Y_M_LIMIT )->SetWindowText( strData );
	strData.Format( "%7.3f", m_pTask->stCommon.dYPLimit );
	GetDlgItem( IDC_EDT_Y_P_LIMIT )->SetWindowText( strData );
	if (pAxis[Y_AXIS] != NULL)
		pAxis[Y_AXIS]->SetSoftLimit((float)m_pTask->stCommon.dYPLimit, (float)m_pTask->stCommon.dYMLimit);

	strData.Format( "%7.3f", m_pTask->stCommon.dZLimit );
	GetDlgItem( IDC_EDT_Z_LIMIT )->SetWindowText( strData );
	if (pAxis[Z_AXIS] != NULL)
		pAxis[Z_AXIS]->SetSoftLimit((float)m_pTask->stCommon.dZLimit, (-1.f)*(float)m_pTask->stCommon.dZLimit);

	strData.Format( "%7.3f", m_pTask->stCommon.dZAlign );
	GetDlgItem( IDC_EDT_Z_ALIGN )->SetWindowText( strData );
	strData.Format( "%7.3f", m_pTask->stCommon.dZMesaureOffset );
	GetDlgItem( IDC_EDT_Z_MEASURE )->SetWindowText( strData );
	strData.Format( "%7.3f", m_pTask->stCommon.dZReadySafety );
	GetDlgItem( IDC_EDT_Z_ALIGN_SAFETY )->SetWindowText( strData );
	strData.Format( "%7.3f", m_pTask->stCommon.dZMSafety );
	GetDlgItem( IDC_EDT_Z_MEASURE_SAFETY )->SetWindowText( strData );
	strData.Format( "%s", m_pTask->stCommon.szExcelResultPath );
	GetDlgItem( IDC_EDT_RESULT_EXCEL_PATH )->SetWindowText( strData );
	strData.Format( "%s", m_pTask->stCommon.szXmlResultPath );
	GetDlgItem( IDC_EDT_RESULT_XML_PATH )->SetWindowText( strData );
	strData.Format( "%s", m_pTask->stCommon.szTrendResultPath );
	GetDlgItem( IDC_EDT_RESULT_TREND_PATH )->SetWindowText( strData );

	// FLAT
	strData.Format( "%7.3f", m_pTask->stCommon.dAlignX[FLAT][WAPER_4] );
	GetDlgItem( IDC_EDT_X_4INCH_FLAT )->SetWindowText( strData );
	strData.Format( "%7.3f", m_pTask->stCommon.dAlignY[FLAT][WAPER_4] );
	GetDlgItem( IDC_EDT_Y_4INCH_FLAT )->SetWindowText( strData );
	strData.Format( "%7.3f", m_pTask->stCommon.dAlignX[FLAT][WAPER_6] );
	GetDlgItem( IDC_EDT_X_6INCH_FLAT )->SetWindowText( strData );
	strData.Format( "%7.3f", m_pTask->stCommon.dAlignY[FLAT][WAPER_6] );
	GetDlgItem( IDC_EDT_Y_6INCH_FLAT )->SetWindowText( strData );
	strData.Format( "%7.3f", m_pTask->stCommon.dAlignX[FLAT][WAPER_8] );
	GetDlgItem( IDC_EDT_X_8INCH_FLAT )->SetWindowText( strData );
	strData.Format( "%7.3f", m_pTask->stCommon.dAlignY[FLAT][WAPER_8] );
	GetDlgItem( IDC_EDT_Y_8INCH_FLAT )->SetWindowText( strData );
	strData.Format( "%7.3f", m_pTask->stCommon.dAlignX[FLAT][WAPER_12] );
	GetDlgItem( IDC_EDT_X_12INCH_FLAT )->SetWindowText( strData );
	strData.Format( "%7.3f", m_pTask->stCommon.dAlignY[FLAT][WAPER_12] );
	GetDlgItem( IDC_EDT_Y_12INCH_FLAT )->SetWindowText( strData );

	// NORCH
	strData.Format( "%7.3f", m_pTask->stCommon.dAlignX[NORCH][WAPER_4] );
	GetDlgItem( IDC_EDT_X_4INCH_NORCH )->SetWindowText( strData );
	strData.Format( "%7.3f", m_pTask->stCommon.dAlignY[NORCH][WAPER_4] );
	GetDlgItem( IDC_EDT_Y_4INCH_NORCH )->SetWindowText( strData );
	strData.Format( "%7.3f", m_pTask->stCommon.dAlignX[NORCH][WAPER_6] );
	GetDlgItem( IDC_EDT_X_6INCH_NORCH )->SetWindowText( strData );
	strData.Format( "%7.3f", m_pTask->stCommon.dAlignY[NORCH][WAPER_6] );
	GetDlgItem( IDC_EDT_Y_6INCH_NORCH )->SetWindowText( strData );
	strData.Format( "%7.3f", m_pTask->stCommon.dAlignX[NORCH][WAPER_8] );
	GetDlgItem( IDC_EDT_X_8INCH_NORCH )->SetWindowText( strData );
	strData.Format( "%7.3f", m_pTask->stCommon.dAlignY[NORCH][WAPER_8] );
	GetDlgItem( IDC_EDT_Y_8INCH_NORCH )->SetWindowText( strData );
	strData.Format( "%7.3f", m_pTask->stCommon.dAlignX[NORCH][WAPER_12] );
	GetDlgItem( IDC_EDT_X_12INCH_NORCH )->SetWindowText( strData );
	strData.Format( "%7.3f", m_pTask->stCommon.dAlignY[NORCH][WAPER_12] );
	GetDlgItem( IDC_EDT_Y_12INCH_NORCH )->SetWindowText( strData );
}
//---------------------------------------------------------------------------------------------------------
// 종료에 필요한 작업을 한다.
//---------------------------------------------------------------------------------------------------------
void CCommon::m_fnSetEnd() 
{
	if ( m_pTaskHandle != NULL )
	{
		m_pTaskHandle->m_fnSetEnd();
		delete m_pTaskHandle;
		m_pTaskHandle = NULL;
	}
	if ( m_hCommonMoveThread != NULL )
	{
		m_bCommonMoveEnd = FALSE;
		SuspendThread( m_hCommonMoveThread );
		CloseHandle( m_hCommonMoveThread );
		m_hCommonMoveThread = NULL;
	}
}

void CCommon::OnBnClickedBtnExit()
{
	m_fnSetShowFlag( FALSE );
}

//------------------------------------------------------------------------
// 장비 구동에 필요한 폴더를 생성한다.
//------------------------------------------------------------------------
BOOL CCommon::m_fnSetMakeFolder()
{
	CString strDirve("");

	// 현재 사용중인 드라이브 정보를 설정한다.
	strDirve = m_Util.m_fnGetCurrentDrive();

	// 환경파일 폴더를 생성한다. 
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
// EFU 파일의 경로를 읽어온다.
//------------------------------------------------------------------------
CString CCommon::m_fnGetCommonFileFullPath()
{
	char szFullPath[MAX_PATH + 1];
	memset( szFullPath, 0x00, sizeof( szFullPath ));

	sprintf_s( szFullPath, "%s%s%s%s", m_Util.m_fnGetCurrentDrive().GetString(),
									 DEFAULT_PATH,
									 DEFAULT_ENV_PATH,
									 DEFAULT_COMMON_FILE );
	szFullPath[MAX_PATH] = NULL;

	return szFullPath;
}

//------------------------------------------------------------------------
// EFU 파일의 경로를 읽어온다.
//------------------------------------------------------------------------
CString CCommon::m_fnGetLastModelFileFullPath()
{
	char szFullPath[MAX_PATH + 1];
	memset( szFullPath, 0x00, sizeof( szFullPath ));

	sprintf_s( szFullPath, "%s%s%s%s", m_Util.m_fnGetCurrentDrive().GetString(),
									 DEFAULT_PATH,
									 DEFAULT_ENV_PATH,
									 DEFAULT_LAST_MODEL_FILE );
	szFullPath[MAX_PATH] = NULL;

	return szFullPath;
}

//------------------------------------------------------------------------
// EFU 파일의 경로를 읽어온다.
//------------------------------------------------------------------------
CString CCommon::m_fnGetDefaultResultPath()
{
	char szFullPath[MAX_PATH + 1];
	memset(szFullPath, 0x00, sizeof(szFullPath));

	sprintf_s(szFullPath, "%s%s", m_Util.m_fnGetCurrentDrive().GetString(), DEFAULT_RESULT_PATH);
	szFullPath[MAX_PATH] = NULL;

	return szFullPath;
}

//------------------------------------------------------------------------
// 설정 환경파일을 저장한다.
//------------------------------------------------------------------------
BOOL CCommon::m_fnSetLastModel_Save( )
{
	CString strWork("");
	CString strData("");
	CString strFullPath("");
	int     i 			= 0;
	int     nNo			= 0;

	CStringArray strLst;
	CXmlElement *pElement     = NULL;
	CXmlElement *pRootElement = NULL;

	strFullPath		=	m_fnGetLastModelFileFullPath();

	CXmlDocument* pXML = new CXmlDocument();
	pXML->Load( strFullPath );

	pXML->GetTagToString( MODEL , strData );
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
	pElement = pXML->FindElement( pRootElement, strLst );
	if ( pElement != NULL )
	{
		strWork.Format( "%s", m_pTask->stLastModel.szLastModel );
		pXML->DataSet( pElement, LAST_MODEL, strWork );
		pXML->Store( strFullPath );
	}

	delete pXML;
	pXML = NULL;

	return TRUE;
}
//------------------------------------------------------------------------
// 설정 환경파일을 저장한다.
//------------------------------------------------------------------------
BOOL CCommon::m_fnSetCommon_Save( )
{
	CString strWork("");
	CString strData("");
	CString strFullPath("");
	int     i 			= 0;
	int     nNo			= 0;

	CStringArray strLst;
	CXmlElement *pElement     = NULL;
	CXmlElement *pRootElement = NULL;

	strFullPath		=	m_fnGetCommonFileFullPath();

	CXmlDocument* pXML = new CXmlDocument();
	pXML->Load( strFullPath );

	pXML->GetTagToString( COMMON , strData );
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
	pElement = pXML->FindElement( pRootElement, strLst );
	if ( pElement != NULL )
	{
		strWork.Format( "%s", m_pTask->stCommon.szLogoPath );
		pXML->DataSet( pElement, AIM_LOGO_PATH, strWork );
		strWork.Format( "%s", m_pTask->stCommon.szQrCodePath );
		pXML->DataSet( pElement, AIM_QRCODE_PATH, strWork );
		strWork.Format( "%s", m_pTask->stCommon.szTitle );
		pXML->DataSet( pElement, EQUIP_TITLE, strWork );

		strWork.Format( "%7.3f", m_pTask->stCommon.dSpeedHigh );
		pXML->DataSet( pElement, JOG_SPEED_HIGH, strWork );			
		strWork.Format( "%7.3f", m_pTask->stCommon.dSpeedMiddle );
		pXML->DataSet( pElement, JOG_SPEED_MIDDLE, strWork );		
		strWork.Format( "%7.3f", m_pTask->stCommon.dSpeedSlow );
		pXML->DataSet( pElement, JOG_SPEED_SLOW, strWork );			
		strWork.Format("%7.3f", m_pTask->stCommon.dSpeedHighZ);
		pXML->DataSet(pElement, JOG_SPEED_HIGH_Z, strWork);
		strWork.Format("%7.3f", m_pTask->stCommon.dSpeedMiddleZ);
		pXML->DataSet(pElement, JOG_SPEED_MIDDLE_Z, strWork);
		strWork.Format("%7.3f", m_pTask->stCommon.dSpeedSlowZ);
		pXML->DataSet(pElement, JOG_SPEED_SLOW_Z, strWork);
		strWork.Format("%s", m_pTask->stCommon.szExcelResultPath);
		pXML->DataSet(pElement, EXCEL_RESULT_PATH, strWork);
		strWork.Format("%s", m_pTask->stCommon.szXmlResultPath);
		pXML->DataSet(pElement, XML_RESULT_PATH, strWork);
		strWork.Format("%s", m_pTask->stCommon.szTrendResultPath);
		pXML->DataSet(pElement, TREND_RESULT_PATH, strWork);

		strWork.Format( "%7.3f", m_pTask->stCommon.dInspXYSpeed );
		pXML->DataSet( pElement, INSP_XY_SPEED, strWork );			
		strWork.Format( "%7.3f", m_pTask->stCommon.dInspZSpeed );
		pXML->DataSet( pElement, INSP_Z_SPEED, strWork );			
		strWork.Format( "%01d", m_pTask->stCommon.nInspLight );
		pXML->DataSet( pElement, INSP_LIGHT_VALUE, strWork );			

		strWork = STR_NO;
		if ( m_pTask->stCommon.bAlarmOn )
		{
			strWork = STR_YES;
		}
		pXML->DataSet( pElement, ALARM_ON, strWork );	

		strWork = STR_NO;
		if ( m_pTask->stCommon.bLogSave )
		{
			strWork = STR_YES;
		}
		pXML->DataSet( pElement, LOG_SAVE, strWork );	

		strWork = STR_NO;
		if ( m_pTask->stCommon.bExcelSave ) 
		{
			strWork = STR_YES;
		}
		pXML->DataSet( pElement, EXCEL_SAVE, strWork );				

		strWork = STR_NO;
		if ( m_pTask->stCommon.bChartReport ) 
		{
			strWork = STR_YES;
		}
		pXML->DataSet( pElement, CHART_INCLUDE, strWork );	

		strWork.Format( "%7.3f", m_pTask->stCommon.dXMLimit );
		pXML->DataSet( pElement, X_M_LIMIT, strWork );
		strWork.Format( "%7.3f", m_pTask->stCommon.dXPLimit );
		pXML->DataSet( pElement, X_P_LIMIT, strWork );
		strWork.Format( "%7.3f", m_pTask->stCommon.dYMLimit );
		pXML->DataSet( pElement, Y_M_LIMIT, strWork );
		strWork.Format( "%7.3f", m_pTask->stCommon.dYPLimit );
		pXML->DataSet( pElement, Y_P_LIMIT, strWork );

		strWork.Format("%7.3f", m_pTask->stCommon.dLoadingX);
		pXML->DataSet(pElement, LOADING_X, strWork);
		strWork.Format("%7.3f", m_pTask->stCommon.dLoadingY);
		pXML->DataSet(pElement, LOADING_Y, strWork);

		strWork.Format("%7.3f", m_pTask->stCommon.dLoadingX2);
		pXML->DataSet(pElement, LOADING_X2, strWork);
		strWork.Format("%7.3f", m_pTask->stCommon.dLoadingY2);
		pXML->DataSet(pElement, LOADING_Y2, strWork);

		strWork.Format( "%7.3f", m_pTask->stCommon.dZReadySafety );
		pXML->DataSet( pElement, Z_ALIGN_SAFETY, strWork );
		strWork.Format( "%7.3f", m_pTask->stCommon.dZMSafety );
		pXML->DataSet( pElement, Z_MEASURE_SAFETY, strWork );
		strWork.Format( "%7.3f", m_pTask->stCommon.dZAlign );
		pXML->DataSet( pElement, Z_ALIGN, strWork );
		strWork.Format( "%7.3f", m_pTask->stCommon.dZMesaureOffset );
		pXML->DataSet( pElement, Z_MEASURE, strWork );
		strWork.Format( "%7.3f", m_pTask->stCommon.dZLimit );
		pXML->DataSet( pElement, Z_LIMIT, strWork );

		// FLAT
		strWork.Format( "%7.3f", m_pTask->stCommon.dAlignX[FLAT][WAPER_4] );
		pXML->DataSet( pElement, ALIGN_4INCH_FLAT_X, strWork );
		strWork.Format( "%7.3f", m_pTask->stCommon.dAlignY[FLAT][WAPER_4] );
		pXML->DataSet( pElement, ALIGN_4INCH_FLAT_Y, strWork );
		strWork.Format( "%7.3f", m_pTask->stCommon.dAlignX[FLAT][WAPER_6] );
		pXML->DataSet( pElement, ALIGN_6INCH_FLAT_X, strWork );
		strWork.Format( "%7.3f", m_pTask->stCommon.dAlignY[FLAT][WAPER_6] );
		pXML->DataSet( pElement, ALIGN_6INCH_FLAT_Y, strWork );
		strWork.Format( "%7.3f", m_pTask->stCommon.dAlignX[FLAT][WAPER_8] );
		pXML->DataSet( pElement, ALIGN_8INCH_FLAT_X, strWork );
		strWork.Format( "%7.3f", m_pTask->stCommon.dAlignY[FLAT][WAPER_8] );
		pXML->DataSet( pElement, ALIGN_8INCH_FLAT_Y, strWork );
		strWork.Format( "%7.3f", m_pTask->stCommon.dAlignX[FLAT][WAPER_12] );
		pXML->DataSet( pElement, ALIGN_12INCH_FLAT_X, strWork );
		strWork.Format( "%7.3f", m_pTask->stCommon.dAlignY[FLAT][WAPER_12] );
		pXML->DataSet( pElement, ALIGN_12INCH_FLAT_Y, strWork );

		// NORCH
		strWork.Format( "%7.3f", m_pTask->stCommon.dAlignX[NORCH][WAPER_4] );
		pXML->DataSet( pElement, ALIGN_4INCH_NORCH_X, strWork );
		strWork.Format( "%7.3f", m_pTask->stCommon.dAlignY[NORCH][WAPER_4] );
		pXML->DataSet( pElement, ALIGN_4INCH_NORCH_Y, strWork );
		strWork.Format( "%7.3f", m_pTask->stCommon.dAlignX[NORCH][WAPER_6] );
		pXML->DataSet( pElement, ALIGN_6INCH_NORCH_X, strWork );
		strWork.Format( "%7.3f", m_pTask->stCommon.dAlignY[NORCH][WAPER_6] );
		pXML->DataSet( pElement, ALIGN_6INCH_NORCH_Y, strWork );
		strWork.Format( "%7.3f", m_pTask->stCommon.dAlignX[NORCH][WAPER_8] );
		pXML->DataSet( pElement, ALIGN_8INCH_NORCH_X, strWork );
		strWork.Format( "%7.3f", m_pTask->stCommon.dAlignY[NORCH][WAPER_8] );
		pXML->DataSet( pElement, ALIGN_8INCH_NORCH_Y, strWork );
		strWork.Format( "%7.3f", m_pTask->stCommon.dAlignX[NORCH][WAPER_12] );
		pXML->DataSet( pElement, ALIGN_12INCH_NORCH_X, strWork );
		strWork.Format( "%7.3f", m_pTask->stCommon.dAlignY[NORCH][WAPER_12] );
		pXML->DataSet( pElement, ALIGN_12INCH_NORCH_Y, strWork );

		pXML->Store( strFullPath );
	}

	delete pXML;
	pXML = NULL;

	return TRUE;
}

//------------------------------------------------------------------------
// 설정 환경파일을 로드한다.
//------------------------------------------------------------------------
BOOL CCommon::m_fnGetLastModel_Load( )
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

	strFullPath					=	m_fnGetLastModelFileFullPath();

	CXmlDocument* pXML = new CXmlDocument();
	pXML->Load( strFullPath );

	pXML->GetTagToString( MODEL , strData );
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
	pElement = pXML->FindElement( pRootElement, strLst );
	if ( pElement != NULL )
	{
		pXML->FindData( pElement, LAST_MODEL, strWork );
		sprintf_s( m_pTask->stLastModel.szLastModel, "%s", strWork.GetString());
	}
	delete pXML;
	pXML = NULL;

	return TRUE;
}

//------------------------------------------------------------------------
// 설정 환경파일을 로드한다.
//------------------------------------------------------------------------
BOOL CCommon::m_fnGetCommon_Load( )
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

	strFullPath					=	m_fnGetCommonFileFullPath();


	CXmlDocument* pXML = new CXmlDocument();
	pXML->Load( strFullPath );

	pXML->GetTagToString( COMMON , strData );
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
	pElement = pXML->FindElement( pRootElement, strLst );
	if ( pElement != NULL )
	{
		pXML->FindData( pElement, AIM_LOGO_PATH, strWork );
		sprintf_s(m_pTask->stCommon.szLogoPath, "%s", strWork.GetString());
		pXML->FindData( pElement, AIM_QRCODE_PATH, strWork );
		sprintf_s( m_pTask->stCommon.szQrCodePath, "%s", strWork.GetString());
		pXML->FindData( pElement, EQUIP_TITLE, strWork );
		sprintf_s( m_pTask->stCommon.szTitle, "%s", strWork.GetString());
		pXML->FindData( pElement, EXCEL_RESULT_PATH, strWork );
		strWork.Trim();
		if (strWork.IsEmpty())
		{
			strWork = m_fnGetDefaultResultPath();
		}
		sprintf_s( m_pTask->stCommon.szExcelResultPath, "%s", strWork.GetString() );
		pXML->FindData( pElement, XML_RESULT_PATH, strWork );
		strWork.Trim();
		if (strWork.IsEmpty())
		{
			strWork = m_fnGetDefaultResultPath();
		}
		sprintf_s(m_pTask->stCommon.szXmlResultPath, "%s", strWork.GetString());
		pXML->FindData( pElement, TREND_RESULT_PATH, strWork );
		strWork.Trim();
		if (strWork.IsEmpty())
		{
			strWork = m_fnGetDefaultResultPath();
		}
		sprintf_s( m_pTask->stCommon.szTrendResultPath, "%s", strWork.GetString() );

		pXML->FindData( pElement, JOG_SPEED_HIGH, strWork );			
		m_pTask->stCommon.dSpeedHigh = atof( strWork );
		pXML->FindData( pElement, JOG_SPEED_MIDDLE, strWork );		
		m_pTask->stCommon.dSpeedMiddle = atof( strWork );
		pXML->FindData( pElement, JOG_SPEED_SLOW, strWork );			
		m_pTask->stCommon.dSpeedSlow = atof( strWork );
		pXML->FindData(pElement, JOG_SPEED_HIGH_Z, strWork);
		m_pTask->stCommon.dSpeedHighZ = atof(strWork);
		pXML->FindData(pElement, JOG_SPEED_MIDDLE_Z, strWork);
		m_pTask->stCommon.dSpeedMiddleZ = atof(strWork);
		pXML->FindData(pElement, JOG_SPEED_SLOW_Z, strWork);
		m_pTask->stCommon.dSpeedSlowZ = atof(strWork);

		pXML->FindData( pElement, INSP_XY_SPEED, strWork );			
		m_pTask->stCommon.dInspXYSpeed = atof( strWork );
		pXML->FindData( pElement, INSP_Z_SPEED, strWork );			
		m_pTask->stCommon.dInspZSpeed = atof( strWork );
		pXML->FindData( pElement, INSP_LIGHT_VALUE, strWork );			
		m_pTask->stCommon.nInspLight = atoi( strWork );

		pXML->FindData( pElement, ALARM_ON, strWork );				
		m_pTask->stCommon.bAlarmOn = FALSE;
		if ( strWork == STR_YES )
		{
			m_pTask->stCommon.bAlarmOn = TRUE;
		}

		pXML->FindData( pElement, LOG_SAVE, strWork );				
		m_pTask->stCommon.bLogSave = FALSE;
		if ( strWork == STR_YES )
		{
			m_pTask->stCommon.bLogSave = TRUE;
		}

		pXML->FindData( pElement, EXCEL_SAVE, strWork );				
		m_pTask->stCommon.bExcelSave = FALSE;
		if ( strWork == STR_YES )
		{
			m_pTask->stCommon.bExcelSave = TRUE;
		}

		pXML->FindData( pElement, CHART_INCLUDE, strWork );				
		m_pTask->stCommon.bChartReport = FALSE;
		if ( strWork == STR_YES )
		{
			m_pTask->stCommon.bChartReport = TRUE;
		}

		pXML->FindData( pElement, X_M_LIMIT, strWork );
		m_pTask->stCommon.dXMLimit = atof( strWork );
		pXML->FindData( pElement, X_P_LIMIT, strWork );
		m_pTask->stCommon.dXPLimit = atof( strWork );
		pXML->FindData( pElement, Y_M_LIMIT, strWork );
		m_pTask->stCommon.dYMLimit = atof( strWork );
		pXML->FindData( pElement, Y_P_LIMIT, strWork );
		m_pTask->stCommon.dYPLimit = atof( strWork );

		pXML->FindData(pElement, LOADING_X, strWork);
		m_pTask->stCommon.dLoadingX = atof(strWork);
		pXML->FindData(pElement, LOADING_Y, strWork);
		m_pTask->stCommon.dLoadingY = atof(strWork);

		pXML->FindData(pElement, LOADING_X2, strWork);
		m_pTask->stCommon.dLoadingX2 = atof(strWork);
		pXML->FindData(pElement, LOADING_Y2, strWork);
		m_pTask->stCommon.dLoadingY2 = atof(strWork);

		pXML->FindData( pElement, Z_LIMIT, strWork );
		m_pTask->stCommon.dZLimit = atof( strWork );
		pXML->FindData( pElement, Z_ALIGN, strWork );
		m_pTask->stCommon.dZAlign = atof( strWork );
		pXML->FindData( pElement, Z_MEASURE, strWork );
		m_pTask->stCommon.dZMesaureOffset = atof( strWork );
		pXML->FindData( pElement, Z_ALIGN_SAFETY, strWork );
		m_pTask->stCommon.dZReadySafety = atof( strWork );
		pXML->FindData( pElement, Z_MEASURE_SAFETY, strWork );
		m_pTask->stCommon.dZMSafety = atof( strWork );

		// FLAT
		pXML->FindData( pElement, ALIGN_4INCH_FLAT_X, strWork );
		m_pTask->stCommon.dAlignX[FLAT][WAPER_4] = atof( strWork );
		pXML->FindData( pElement, ALIGN_4INCH_FLAT_Y, strWork );
		m_pTask->stCommon.dAlignY[FLAT][WAPER_4] = atof( strWork );
		pXML->FindData( pElement, ALIGN_6INCH_FLAT_X, strWork );
		m_pTask->stCommon.dAlignX[FLAT][WAPER_6] = atof( strWork );
		pXML->FindData( pElement, ALIGN_6INCH_FLAT_Y, strWork );
		m_pTask->stCommon.dAlignY[FLAT][WAPER_6] = atof( strWork );
		pXML->FindData( pElement, ALIGN_8INCH_FLAT_X, strWork );
		m_pTask->stCommon.dAlignX[FLAT][WAPER_8] = atof( strWork );
		pXML->FindData( pElement, ALIGN_8INCH_FLAT_Y, strWork );
		m_pTask->stCommon.dAlignY[FLAT][WAPER_8] = atof( strWork );
		pXML->FindData( pElement, ALIGN_12INCH_FLAT_X, strWork );
		m_pTask->stCommon.dAlignX[FLAT][WAPER_12] = atof( strWork );
		pXML->FindData( pElement, ALIGN_12INCH_FLAT_Y, strWork );
		m_pTask->stCommon.dAlignY[FLAT][WAPER_12] = atof( strWork );

		// NORCH
		pXML->FindData( pElement, ALIGN_4INCH_NORCH_X, strWork );
		m_pTask->stCommon.dAlignX[NORCH][WAPER_4] = atof( strWork );
		pXML->FindData( pElement, ALIGN_4INCH_NORCH_Y, strWork );
		m_pTask->stCommon.dAlignY[NORCH][WAPER_4] = atof( strWork );
		pXML->FindData( pElement, ALIGN_6INCH_NORCH_X, strWork );
		m_pTask->stCommon.dAlignX[NORCH][WAPER_6] = atof( strWork );
		pXML->FindData( pElement, ALIGN_6INCH_NORCH_Y, strWork );
		m_pTask->stCommon.dAlignY[NORCH][WAPER_6] = atof( strWork );
		pXML->FindData( pElement, ALIGN_8INCH_NORCH_X, strWork );
		m_pTask->stCommon.dAlignX[NORCH][WAPER_8] = atof( strWork );
		pXML->FindData( pElement, ALIGN_8INCH_NORCH_Y, strWork );
		m_pTask->stCommon.dAlignY[NORCH][WAPER_8] = atof( strWork );
		pXML->FindData( pElement, ALIGN_12INCH_NORCH_X, strWork );
		m_pTask->stCommon.dAlignX[NORCH][WAPER_12] = atof( strWork );
		pXML->FindData( pElement, ALIGN_12INCH_NORCH_Y, strWork );
		m_pTask->stCommon.dAlignY[NORCH][WAPER_12] = atof( strWork );
	}
	delete pXML;
	pXML = NULL;

	return TRUE;
}

void CCommon::OnBnClickedBtnCommonSave()
{
	CString strData("");
	GetDlgItem( IDC_EDT_HIGH_SPEED )->GetWindowText( strData );
	m_pTask->stCommon.dSpeedHigh = atof( strData );
	GetDlgItem( IDC_EDT_MIDDLE_SPEED )->GetWindowText( strData );
	m_pTask->stCommon.dSpeedMiddle = atof( strData );
	GetDlgItem( IDC_EDT_SLOW_SPEED )->GetWindowText( strData );
	m_pTask->stCommon.dSpeedSlow = atof( strData );
	GetDlgItem(IDC_EDT_HIGH_SPEED_Z)->GetWindowText(strData);
	m_pTask->stCommon.dSpeedHighZ = atof(strData);
	GetDlgItem(IDC_EDT_MIDDLE_SPEED_Z)->GetWindowText(strData);
	m_pTask->stCommon.dSpeedMiddleZ = atof(strData);
	GetDlgItem(IDC_EDT_SLOW_SPEED_Z)->GetWindowText(strData);
	m_pTask->stCommon.dSpeedSlowZ = atof(strData);
	GetDlgItem(IDC_EDT_RESULT_EXCEL_PATH)->GetWindowText(strData);
	sprintf_s( m_pTask->stCommon.szExcelResultPath, "%s", strData.GetString());
	GetDlgItem(IDC_EDT_RESULT_XML_PATH)->GetWindowText(strData);
	sprintf_s( m_pTask->stCommon.szXmlResultPath, "%s", strData.GetString());
	GetDlgItem(IDC_EDT_RESULT_TREND_PATH)->GetWindowText(strData);
	sprintf_s( m_pTask->stCommon.szTrendResultPath, "%s", strData.GetString());

	GetDlgItem( IDC_EDT_XY_INSP_SPEED )->GetWindowText( strData );
	m_pTask->stCommon.dInspXYSpeed = atof( strData );
	GetDlgItem( IDC_EDT_Z_INSP_SPEED )->GetWindowText( strData );
	m_pTask->stCommon.dInspZSpeed = atof( strData );
	GetDlgItem( IDC_EDT_LIGHT_VALUE )->GetWindowText( strData );
	m_pTask->stCommon.nInspLight = atoi( strData );

	m_pTask->stCommon.bAlarmOn = ((CButton*)GetDlgItem( IDC_CHK_ALARM_MELODY_ON ))->GetCheck();
	m_pTask->stCommon.bExcelSave     = ((CButton*)GetDlgItem( IDC_CHK_EXCEL_SAVE ))->GetCheck();
	m_pTask->stCommon.bLogSave       = ((CButton*)GetDlgItem( IDC_CHK_LOG_SAVE ))->GetCheck();
	m_pTask->stCommon.bChartReport   = ((CButton*)GetDlgItem( IDC_CHK_CHART_INCLUDE ))->GetCheck();

	GetDlgItem( IDC_EDT_X_M_LIMIT )->GetWindowText( strData );
	m_pTask->stCommon.dXMLimit = atof( strData);
	GetDlgItem( IDC_EDT_X_P_LIMIT )->GetWindowText( strData );
	m_pTask->stCommon.dXPLimit = atof( strData);



	GetDlgItem( IDC_EDT_Y_M_LIMIT )->GetWindowText( strData );
	m_pTask->stCommon.dYMLimit = atof( strData);
	GetDlgItem( IDC_EDT_Y_P_LIMIT )->GetWindowText( strData );
	m_pTask->stCommon.dYPLimit = atof( strData);

	GetDlgItem(IDC_EDT_LOADING_X)->GetWindowText(strData);
	m_pTask->stCommon.dLoadingX = atof(strData);
	GetDlgItem(IDC_EDT_LOADING_Y)->GetWindowText(strData);
	m_pTask->stCommon.dLoadingY = atof(strData);

	GetDlgItem(IDC_EDT_LOADING_X2)->GetWindowText(strData);
	m_pTask->stCommon.dLoadingX2 = atof(strData);
	GetDlgItem(IDC_EDT_LOADING_Y2)->GetWindowText(strData);
	m_pTask->stCommon.dLoadingY2 = atof(strData);

	GetDlgItem( IDC_EDT_Z_LIMIT )->GetWindowText( strData );
	m_pTask->stCommon.dZLimit = atof( strData);
	GetDlgItem( IDC_EDT_Z_ALIGN )->GetWindowText( strData );
	m_pTask->stCommon.dZAlign = atof( strData);
	GetDlgItem( IDC_EDT_Z_MEASURE )->GetWindowText( strData );
	m_pTask->stCommon.dZMesaureOffset = atof( strData);
	GetDlgItem( IDC_EDT_Z_ALIGN_SAFETY )->GetWindowText( strData );
	m_pTask->stCommon.dZReadySafety = atof( strData);
	GetDlgItem( IDC_EDT_Z_MEASURE_SAFETY )->GetWindowText( strData );
	m_pTask->stCommon.dZMSafety = atof( strData);

	// FLAT
	GetDlgItem( IDC_EDT_X_4INCH_FLAT )->GetWindowText( strData );
	m_pTask->stCommon.dAlignX[FLAT][WAPER_4] = atof( strData);
	GetDlgItem( IDC_EDT_Y_4INCH_FLAT )->GetWindowText( strData );
	m_pTask->stCommon.dAlignY[FLAT][WAPER_4] = atof( strData);
	GetDlgItem( IDC_EDT_X_6INCH_FLAT )->GetWindowText( strData );
	m_pTask->stCommon.dAlignX[FLAT][WAPER_6] = atof( strData);
	GetDlgItem( IDC_EDT_Y_6INCH_FLAT )->GetWindowText( strData );
	m_pTask->stCommon.dAlignY[FLAT][WAPER_6] = atof( strData);
	GetDlgItem( IDC_EDT_X_8INCH_FLAT )->GetWindowText( strData );
	m_pTask->stCommon.dAlignX[FLAT][WAPER_8] = atof( strData);
	GetDlgItem( IDC_EDT_Y_8INCH_FLAT )->GetWindowText( strData );
	m_pTask->stCommon.dAlignY[FLAT][WAPER_8] = atof( strData);
	GetDlgItem( IDC_EDT_X_12INCH_FLAT )->GetWindowText( strData );
	m_pTask->stCommon.dAlignX[FLAT][WAPER_12] = atof( strData);
	GetDlgItem( IDC_EDT_Y_12INCH_FLAT )->GetWindowText( strData );
	m_pTask->stCommon.dAlignY[FLAT][WAPER_12] = atof( strData);

	// NORCH
	GetDlgItem( IDC_EDT_X_4INCH_NORCH )->GetWindowText( strData );
	m_pTask->stCommon.dAlignX[NORCH][WAPER_4] = atof( strData);
	GetDlgItem( IDC_EDT_Y_4INCH_NORCH )->GetWindowText( strData );
	m_pTask->stCommon.dAlignY[NORCH][WAPER_4] = atof( strData);
	GetDlgItem( IDC_EDT_X_6INCH_NORCH )->GetWindowText( strData );
	m_pTask->stCommon.dAlignX[NORCH][WAPER_6] = atof( strData);
	GetDlgItem( IDC_EDT_Y_6INCH_NORCH )->GetWindowText( strData );
	m_pTask->stCommon.dAlignY[NORCH][WAPER_6] = atof( strData);
	GetDlgItem( IDC_EDT_X_8INCH_NORCH )->GetWindowText( strData );
	m_pTask->stCommon.dAlignX[NORCH][WAPER_8] = atof( strData);
	GetDlgItem( IDC_EDT_Y_8INCH_NORCH )->GetWindowText( strData );
	m_pTask->stCommon.dAlignY[NORCH][WAPER_8] = atof( strData);
	GetDlgItem( IDC_EDT_X_12INCH_NORCH )->GetWindowText( strData );
	m_pTask->stCommon.dAlignX[NORCH][WAPER_12] = atof( strData);
	GetDlgItem( IDC_EDT_Y_12INCH_NORCH )->GetWindowText( strData );
	m_pTask->stCommon.dAlignY[NORCH][WAPER_12] = atof( strData);

	m_fnSetCommon_Save();
}

void CCommon::OnBnClickedBtnXMLimit()
{
#if AJINTEX_BOARD
	double dwPosition = 0L;
	CString strData("");
	if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( X_AXIS, dwPosition ) == RC_NORMAL )
	{
		strData.Format( "%7.3f", dwPosition);
		GetDlgItem( IDC_EDT_X_M_LIMIT )->SetWindowText( strData );
	}
#endif
}


void CCommon::OnBnClickedBtnXPLimit()
{
#if AJINTEX_BOARD
	double dwPosition = 0L;
	CString strData("");
	if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( X_AXIS, dwPosition ) == RC_NORMAL )
	{
		strData.Format( "%7.3f", dwPosition);
		GetDlgItem( IDC_EDT_X_P_LIMIT )->SetWindowText( strData );
	}
#endif
}


void CCommon::OnBnClickedBtnYMLimit()
{
#if AJINTEX_BOARD
	double dwPosition = 0L;
	CString strData("");
	if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( Y_AXIS, dwPosition ) == RC_NORMAL )
	{
		strData.Format( "%7.3f", dwPosition);
		GetDlgItem( IDC_EDT_Y_M_LIMIT )->SetWindowText( strData );
	}
#endif
}


void CCommon::OnBnClickedBtnYPLimit()
{
#if AJINTEX_BOARD
	double dwPosition = 0L;
	CString strData("");
	if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( Y_AXIS, dwPosition ) == RC_NORMAL )
	{
		strData.Format( "%7.3f", dwPosition);
		GetDlgItem( IDC_EDT_Y_P_LIMIT )->SetWindowText( strData );
	}
#endif
}


void CCommon::OnEnChangeEdtHighSpeed()
{
	CString strData("");
	double  dData = 0.0f;
	GetDlgItem( IDC_EDT_HIGH_SPEED )->GetWindowText( strData );
	dData = atof( strData );
	if ( dData > MAX_SPEED )
	{
		strData.Format( "최고 지정가능한 속도범위(%7.3f~%7.3f)를 참조해주세요.", MIN_SPEED, MAX_SPEED );
		TECmsgOut( m_Util.CStringTobstr( strData), GREEN_1, 1 );
		strData.Format( "%7.3f", m_pTask->stCommon.dSpeedHigh );
		GetDlgItem( IDC_EDT_HIGH_SPEED )->SetWindowText( strData );
	}
}


void CCommon::OnEnChangeEdtMiddleSpeed()
{
	CString strData("");
	double  dData = 0.0f;
	GetDlgItem( IDC_EDT_MIDDLE_SPEED )->GetWindowText( strData );
	dData = atof( strData );
	if ( dData > MAX_SPEED )
	{
		strData.Format( "최고 지정가능한 속도범위(%7.3f~%7.3f)를 참조해주세요.", MIN_SPEED, MAX_SPEED );
		TECmsgOut( m_Util.CStringTobstr( strData), GREEN_1, 1 );
		strData.Format( "%7.3f", m_pTask->stCommon.dSpeedMiddle );
		GetDlgItem( IDC_EDT_MIDDLE_SPEED )->SetWindowText( strData );
	}
}


void CCommon::OnEnChangeEdtSlowSpeed()
{
	CString strData("");
	double  dData = 0.0f;
	GetDlgItem( IDC_EDT_SLOW_SPEED )->GetWindowText( strData );
	dData = atof( strData );
	if ( dData > MAX_SPEED )
	{
		strData.Format( "최고 지정가능한 속도범위(%7.3f~%7.3f)를 참조해주세요.", MIN_SPEED, MAX_SPEED );
		TECmsgOut( m_Util.CStringTobstr( strData), GREEN_1, 1 );
		strData.Format( "%7.3f", m_pTask->stCommon.dSpeedSlow );
		GetDlgItem( IDC_EDT_SLOW_SPEED )->SetWindowText( strData );
	}
}

void CCommon::OnEnChangeEdtHighSpeedZ()
{
	CString strData("");
	double  dData = 0.0f;
	GetDlgItem(IDC_EDT_HIGH_SPEED_Z)->GetWindowText(strData);
	dData = atof(strData);
	if ( dData > MAX_SPEED )
	{
		strData.Format("최고 지정가능한 속도범위(%7.3f~%7.3f)를 참조해주세요.", MIN_SPEED, MAX_SPEED);
		TECmsgOut( m_Util.CStringTobstr( strData), GREEN_1, 1 );
		strData.Format("%7.3f", m_pTask->stCommon.dSpeedHighZ);
		GetDlgItem(IDC_EDT_HIGH_SPEED_Z)->SetWindowText(strData);
	}
}


void CCommon::OnEnChangeEdtMiddleSpeedZ()
{
	CString strData("");
	double  dData = 0.0f;
	GetDlgItem(IDC_EDT_MIDDLE_SPEED_Z)->GetWindowText(strData);
	dData = atof(strData);
	if ( dData > MAX_SPEED )
	{
		strData.Format("최고 지정가능한 속도범위(%7.3f~%7.3f)를 참조해주세요.", MIN_SPEED, MAX_SPEED);
		TECmsgOut( m_Util.CStringTobstr( strData), GREEN_1, 1 );
		strData.Format("%7.3f", m_pTask->stCommon.dSpeedMiddleZ);
		GetDlgItem(IDC_EDT_MIDDLE_SPEED_Z)->SetWindowText(strData);
	}
}


void CCommon::OnEnChangeEdtSlowSpeedZ()
{
	CString strData("");
	double  dData = 0.0f;
	GetDlgItem(IDC_EDT_SLOW_SPEED_Z)->GetWindowText(strData);
	dData = atof(strData);
	if ( dData > MAX_SPEED )
	{
		strData.Format("최고 지정가능한 속도범위(%7.3f~%7.3f)를 참조해주세요.", MIN_SPEED, MAX_SPEED);
		TECmsgOut( m_Util.CStringTobstr( strData), GREEN_1, 1 );
		strData.Format("%7.3f", m_pTask->stCommon.dSpeedSlowZ);
		GetDlgItem(IDC_EDT_SLOW_SPEED_Z)->SetWindowText(strData);
	}
}


void CCommon::OnBnClickedChkLogSave()
{
	m_pTask->stCommon.bLogSave = ((CButton*)GetDlgItem( IDC_CHK_LOG_SAVE ))->GetCheck();
	theApp.m_fnGetViewPointer( )->m_Log.m_fnSetLogWriteFlag( m_pTask->stCommon.bLogSave );
	OnBnClickedBtnCommonSave();
}

void CCommon::OnBnClickedBtnLoadingRead()
{
#if AJINTEX_BOARD
	double dwPosition = 0L;
	CString strData("");
	if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( X_AXIS, dwPosition ) == RC_NORMAL )
	{
		strData.Format( "%7.3f", dwPosition);
		GetDlgItem( IDC_EDT_LOADING_X )->SetWindowText( strData );
	}
	if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( Y_AXIS, dwPosition ) == RC_NORMAL )
	{
		strData.Format( "%7.3f", dwPosition);
		GetDlgItem( IDC_EDT_LOADING_Y )->SetWindowText( strData );
	}
#endif
}

void CCommon::OnBnClickedBtnLoadingRead2()
{
#if AJINTEX_BOARD
	double dwPosition = 0L;
	CString strData("");
	if (theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos(X_AXIS, dwPosition) == RC_NORMAL)
	{
		strData.Format("%7.3f", dwPosition);
		GetDlgItem(IDC_EDT_LOADING_X2)->SetWindowText(strData);
	}
	if (theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos(Y_AXIS, dwPosition) == RC_NORMAL)
	{
		strData.Format("%7.3f", dwPosition);
		GetDlgItem(IDC_EDT_LOADING_Y2)->SetWindowText(strData);
	}
#endif
}


void CCommon::OnBnClickedBtnLoadingMove()
{
#if AJINTEX_BOARD
	double dwPosition = 0.0;
	if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( Z_AXIS, dwPosition ) == RC_NORMAL )
	{
		if ( theApp.m_fnGetViewPointer()->m_pModel->m_fnGetMeasureSafetyZ( theApp.m_fnGetViewPointer()->m_pTask->stModel.dWaperThickness ) > theApp.m_fnGetViewPointer()->m_pTask->stCommon.dZLimit )
		{
			TECmsgOut( L"Z축 Limit 위치보다 Z측정 안정위치가 낮아서 Stage와 충돌할수 있어서 이동불가 합니다.", RED_1, 1 );
			return;
		}
	}
#endif
#if AJINTEX_BOARD
	CString strDataX("");
	CString strDataY("");
	GetDlgItem( IDC_EDT_LOADING_X )->GetWindowText( strDataX );
	GetDlgItem( IDC_EDT_LOADING_Y )->GetWindowText( strDataY );
	DWORD dwBusyStatus1	= 0;
	DWORD dwBusyStatus2	= 0;
	double dwSpeed      = theApp.m_fnGetViewPointer( )->m_pTask->stCommon.dSpeedHigh;
	double dwMoveX      = atof( strDataX ) - pAxis[X_AXIS]->m_dOffsetPos;
	double dwMoveY      = atof( strDataY ) - pAxis[Y_AXIS]->m_dOffsetPos;
	m_fnSetMove(dwMoveX, dwMoveY);
#endif
}

void CCommon::OnBnClickedBtnLoadingMove2()
{
#if AJINTEX_BOARD
	double dwPosition = 0.0;
	if (theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos(Z_AXIS, dwPosition) == RC_NORMAL)
	{
		if (theApp.m_fnGetViewPointer()->m_pModel->m_fnGetMeasureSafetyZ(theApp.m_fnGetViewPointer()->m_pTask->stModel.dWaperThickness) > theApp.m_fnGetViewPointer()->m_pTask->stCommon.dZLimit)
		{
			TECmsgOut(L"Z축 Limit 위치보다 Z측정 안정위치가 낮아서 Stage와 충돌할수 있어서 이동불가 합니다.", RED_1, 1);
			return;
		}
	}
#endif
#if AJINTEX_BOARD
	CString strDataX("");
	CString strDataY("");
	GetDlgItem(IDC_EDT_LOADING_X2)->GetWindowText(strDataX);
	GetDlgItem(IDC_EDT_LOADING_Y2)->GetWindowText(strDataY);
	DWORD dwBusyStatus1 = 0;
	DWORD dwBusyStatus2 = 0;
	double dwSpeed = theApp.m_fnGetViewPointer()->m_pTask->stCommon.dSpeedHigh;
	double dwMoveX = atof(strDataX) - pAxis[X_AXIS]->m_dOffsetPos;
	double dwMoveY = atof(strDataY) - pAxis[Y_AXIS]->m_dOffsetPos;
	m_fnSetMove(dwMoveX, dwMoveY);
#endif
}




void CCommon::OnBnClickedBtnZLimitRead()
{
#if AJINTEX_BOARD
	double dwPosition = 0L;
	CString strData("");
	if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( Z_AXIS, dwPosition ) == RC_NORMAL )
	{
		strData.Format( "%7.3f", dwPosition);
		GetDlgItem( IDC_EDT_Z_LIMIT )->SetWindowText( strData );
	}
#endif
}
void CCommon::m_fnSetMove( double dX, double dY )
{
	if ( m_hCommonMoveThread != NULL )
	{
		m_bCommonMoveEnd = FALSE;
		SuspendThread( m_hCommonMoveThread );
		CloseHandle( m_hCommonMoveThread );
		m_hCommonMoveThread = NULL;
	}

	m_dMoveX = dX;
	m_dMoveY = dY;
	unsigned long	ThreadID;
	m_bCommonMoveEnd    = TRUE;

	m_hCommonMoveThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CommonMoveThread, this, NULL, &ThreadID);
	m_nCommonMoveStep   = 10;

}

UINT CommonMoveThread(LPVOID pFuncData)
{
	CCommon		*pQI = (CCommon *)pFuncData;
	DWORD   dwBusyStatus  = 0;
	DWORD   dwRtnCode     = 0;
	DWORD   dwBusyStatus1 = 0;
	DWORD   dwRtnCode1    = 0;
	DWORD   dwBusyStatus2 = 0;
	DWORD   dwRtnCode2    = 0;
	double  dMove         = 0.0;
	double  dSpeed        = 0.0;
	CString strData("");
	while ( pQI->m_bCommonMoveEnd )
	{
		if ( pQI->m_hWnd != NULL )
		{
#if AJINTEX_BOARD
			switch ( pQI->m_nCommonMoveStep )
			{
			case 10:
				{
					dwRtnCode = theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmStatusReadInMotion( Z_AXIS, dwBusyStatus );
					if ( dwBusyStatus == 0 )
					{
						dMove  = theApp.m_fnGetViewPointer( )->m_pModel->m_fnGetMeasureSafetyZ( pQI->m_pTask->stModel.dWaperThickness );
						dSpeed = pQI->m_pTask->stCommon.dSpeedHighZ;
						theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmMotSetAbsRelMode( Z_AXIS, POS_ABS_MODE );
						theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmMoveStartPos( Z_AXIS, dMove, dSpeed, dSpeed * 2, dSpeed * 2 );
						theApp.m_fnGetViewPointer()->m_fnLogWrite( "Common : 안전위치 이동시작" );
						pQI->m_nCommonMoveStep = 20;
					}
				}
				break;
			case 20:
				{
					dwRtnCode = theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmStatusReadInMotion( Z_AXIS, dwBusyStatus );
					if ( dwBusyStatus == 0 )
					{
						theApp.m_fnGetViewPointer()->m_fnLogWrite( "Model : 안전위치 이동완료" );
						dSpeed = pQI->m_pTask->stCommon.dSpeedHigh;
						theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmMotSetAbsRelMode( X_AXIS, POS_ABS_MODE );
						theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmMotSetAbsRelMode( Y_AXIS, POS_ABS_MODE );
						theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmMoveStartPos( X_AXIS, pQI->m_dMoveX, dSpeed, dSpeed * 2, dSpeed * 2 );
						theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmMoveStartPos( Y_AXIS, pQI->m_dMoveY, dSpeed, dSpeed * 2, dSpeed * 2 );
						theApp.m_fnGetViewPointer()->m_fnLogWrite( "Common : 위치 이동시작" );
						pQI->m_nCommonMoveStep = 30;
					}
				}
				break;
			case 30:
				{
					dwRtnCode1 = theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmStatusReadInMotion( Z_AXIS, dwBusyStatus1 );
					dwRtnCode2 = theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmStatusReadInMotion( Z_AXIS, dwBusyStatus2 );
					if (( dwBusyStatus1 == 0 ) &&
						( dwBusyStatus2 == 0 ))
					{
						theApp.m_fnGetViewPointer()->m_fnLogWrite( "Common : 위치 이동완료" );
						pQI->m_nCommonMoveStep = STEP_HALT;
						pQI->m_bCommonMoveEnd = FALSE;

					}
				}
				break;
			}
#endif
			Sleep( AUTO_TIME );
		}
	}

	if ( pQI->m_hWnd == NULL ) return 0;

	if ( pQI->m_hCommonMoveThread != NULL )
	{
		SuspendThread(pQI->m_hCommonMoveThread);
		CloseHandle(pQI->m_hCommonMoveThread);
		pQI->m_hCommonMoveThread	= NULL;
	}
	return 0;
}

void CCommon::OnBnClickedBtnZLimitMove()
{
#if AJINTEX_BOARD
	CString strData("");
	GetDlgItem( IDC_EDT_Z_LIMIT )->GetWindowText( strData );
	DWORD dwBusyStatus	= 0;
	double dwSpeed      = theApp.m_fnGetViewPointer( )->m_pTask->stCommon.dSpeedHighZ;
	double dwMove      = atof( strData );
	DWORD dwRtnCode = theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmStatusReadInMotion( Z_AXIS, dwBusyStatus );
	if ( dwBusyStatus == 0 )
	{
		theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmMotSetAbsRelMode( Z_AXIS, POS_ABS_MODE );
		theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmMoveStartPos( Z_AXIS, dwMove, dwSpeed, dwSpeed * 4, dwSpeed * 4 );
	}
#endif
}


void CCommon::OnBnClickedBtnZAlignRead()
{
#if AJINTEX_BOARD
	double dwPosition = 0L;
	CString strData("");
	if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( Z_AXIS, dwPosition ) == RC_NORMAL )
	{
		strData.Format( "%7.3f", dwPosition);
		GetDlgItem( IDC_EDT_Z_ALIGN )->SetWindowText( strData );
	}
#endif
}


void CCommon::OnBnClickedBtnMeasureRead()
{
#if AJINTEX_BOARD
	double dwPosition = 0L;
	CString strData("");
	if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( Z_AXIS, dwPosition ) == RC_NORMAL )
	{
		strData.Format( "%7.3f", dwPosition);
		GetDlgItem( IDC_EDT_Z_MEASURE )->SetWindowText( strData );
	}
#endif
}

void CCommon::OnBnClickedBtnZAlignMove()
{
#if AJINTEX_BOARD
	CString strData("");
	GetDlgItem( IDC_EDT_Z_ALIGN )->GetWindowText( strData );
	DWORD dwBusyStatus	= 0;
	double dwSpeed      = theApp.m_fnGetViewPointer( )->m_pTask->stCommon.dSpeedHighZ;
	double dwMove      = atof( strData );
	DWORD dwRtnCode = theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmStatusReadInMotion( Z_AXIS, dwBusyStatus );
	if ( dwBusyStatus == 0 )
	{
		theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmMotSetAbsRelMode( Z_AXIS, POS_ABS_MODE );
		theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmMoveStartPos( Z_AXIS, dwMove, dwSpeed, dwSpeed * 4, dwSpeed * 4 );
	}
#endif
}

void CCommon::OnBnClickedBtnZMeasureMove()
{
#if AJINTEX_BOARD
	CString strData("");
	GetDlgItem( IDC_EDT_Z_MEASURE )->GetWindowText( strData );
	DWORD dwBusyStatus	= 0;
	double dwSpeed      = theApp.m_fnGetViewPointer( )->m_pTask->stCommon.dSpeedSlowZ;
	double dwMove      = atof( strData );
	DWORD dwRtnCode = theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmStatusReadInMotion( Z_AXIS, dwBusyStatus );
	if ( dwBusyStatus == 0 )
	{
		theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmMotSetAbsRelMode( Z_AXIS, POS_ABS_MODE );
		theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmMoveStartPos( Z_AXIS, dwMove, dwSpeed, dwSpeed * 4, dwSpeed * 4 );
	}
#endif
}

void CCommon::OnBnClickedBtnZAlignSafetyRead()
{
#if AJINTEX_BOARD
	double dwPosition = 0L;
	CString strData("");
	if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( Z_AXIS, dwPosition ) == RC_NORMAL )
	{
		strData.Format( "%7.3f", dwPosition);
		GetDlgItem( IDC_EDT_Z_ALIGN_SAFETY )->SetWindowText( strData );
	}
#endif
}


void CCommon::OnBnClickedBtnZAlignSafetyMove()
{
#if AJINTEX_BOARD
	CString strData("");
	GetDlgItem( IDC_EDT_Z_ALIGN_SAFETY )->GetWindowText( strData );
	DWORD dwBusyStatus	= 0;
	double dwSpeed      = theApp.m_fnGetViewPointer( )->m_pTask->stCommon.dSpeedHighZ;
	double dwMove      = atof( strData );
	DWORD dwRtnCode = theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmStatusReadInMotion( Z_AXIS, dwBusyStatus );
	if ( dwBusyStatus == 0 )
	{
		theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmMotSetAbsRelMode( Z_AXIS, POS_ABS_MODE );
		theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmMoveStartPos( Z_AXIS, dwMove, dwSpeed, dwSpeed * 4, dwSpeed * 4 );
	}
#endif
}


void CCommon::OnBnClickedBtnZMeasureSafetyRead()
{
#if AJINTEX_BOARD
	double dwPosition = 0L;
	CString strData("");
	if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( Z_AXIS, dwPosition ) == RC_NORMAL )
	{
		strData.Format( "%7.3f", dwPosition);
		GetDlgItem( IDC_EDT_Z_MEASURE_SAFETY )->SetWindowText( strData );
	}
#endif
}


void CCommon::OnBnClickedBtnZMeasureSafetyMove()
{
#if AJINTEX_BOARD
	CString strData("");
	GetDlgItem( IDC_EDT_Z_MEASURE_SAFETY )->GetWindowText( strData );
	DWORD dwBusyStatus	= 0;
	double dwSpeed      = theApp.m_fnGetViewPointer( )->m_pTask->stCommon.dSpeedHighZ;
	double dwMove      = atof( strData );
	DWORD dwRtnCode = theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmStatusReadInMotion( Z_AXIS, dwBusyStatus );
	if ( dwBusyStatus == 0 )
	{
		theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmMotSetAbsRelMode( Z_AXIS, POS_ABS_MODE );
		theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmMoveStartPos( Z_AXIS, dwMove, dwSpeed, dwSpeed * 4, dwSpeed * 4 );
	}
#endif
}


void CCommon::OnBnClickedBtnNorch4inchRead()
{
#if AJINTEX_BOARD
	double dwPosition = 0L;
	CString strData("");
	if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( X_AXIS, dwPosition ) == RC_NORMAL )
	{
		strData.Format( "%7.3f", dwPosition);
		GetDlgItem( IDC_EDT_X_4INCH_NORCH )->SetWindowText( strData );
	}
	if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( Y_AXIS, dwPosition ) == RC_NORMAL )
	{
		strData.Format( "%7.3f", dwPosition);
		GetDlgItem( IDC_EDT_Y_4INCH_NORCH )->SetWindowText( strData );
	}
#endif
}


void CCommon::OnBnClickedBtnNorch6inchRead()
{
#if AJINTEX_BOARD
	double dwPosition = 0L;
	CString strData("");
	if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( X_AXIS, dwPosition ) == RC_NORMAL )
	{
		strData.Format( "%7.3f", dwPosition);
		GetDlgItem( IDC_EDT_X_6INCH_NORCH )->SetWindowText( strData );
	}
	if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( Y_AXIS, dwPosition ) == RC_NORMAL )
	{
		strData.Format( "%7.3f", dwPosition);
		GetDlgItem( IDC_EDT_Y_6INCH_NORCH )->SetWindowText( strData );
	}
#endif
}


void CCommon::OnBnClickedBtnNorch8inchRead()
{
#if AJINTEX_BOARD
	double dwPosition = 0L;
	CString strData("");
	if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( X_AXIS, dwPosition ) == RC_NORMAL )
	{
		strData.Format( "%7.3f", dwPosition);
		GetDlgItem( IDC_EDT_X_8INCH_NORCH )->SetWindowText( strData );
	}
	if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( Y_AXIS, dwPosition ) == RC_NORMAL )
	{
		strData.Format( "%7.3f", dwPosition);
		GetDlgItem( IDC_EDT_Y_8INCH_NORCH )->SetWindowText( strData );
	}
#endif
}


void CCommon::OnBnClickedBtnNorch12inchRead()
{
#if AJINTEX_BOARD
	double dwPosition = 0L;
	CString strData("");
	if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( X_AXIS, dwPosition ) == RC_NORMAL )
	{
		strData.Format( "%7.3f", dwPosition);
		GetDlgItem( IDC_EDT_X_12INCH_NORCH )->SetWindowText( strData );
	}
	if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( Y_AXIS, dwPosition ) == RC_NORMAL )
	{
		strData.Format( "%7.3f", dwPosition);
		GetDlgItem( IDC_EDT_Y_12INCH_NORCH )->SetWindowText( strData );
	}
#endif
}


void CCommon::OnBnClickedBtnFlat4inchRead()
{
#if AJINTEX_BOARD
	double dwPosition = 0L;
	CString strData("");
	if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( X_AXIS, dwPosition ) == RC_NORMAL )
	{
		strData.Format( "%7.3f", dwPosition);
		GetDlgItem( IDC_EDT_X_4INCH_FLAT )->SetWindowText( strData );
	}
	if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( Y_AXIS, dwPosition ) == RC_NORMAL )
	{
		strData.Format( "%7.3f", dwPosition);
		GetDlgItem( IDC_EDT_Y_4INCH_FLAT )->SetWindowText( strData );
	}
#endif
}


void CCommon::OnBnClickedBtnFlat6inchRead()
{
#if AJINTEX_BOARD
	double dwPosition = 0L;
	CString strData("");
	if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( X_AXIS, dwPosition ) == RC_NORMAL )
	{
		strData.Format( "%7.3f", dwPosition);
		GetDlgItem( IDC_EDT_X_6INCH_FLAT )->SetWindowText( strData );
	}
	if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( Y_AXIS, dwPosition ) == RC_NORMAL )
	{
		strData.Format( "%7.3f", dwPosition);
		GetDlgItem( IDC_EDT_Y_6INCH_FLAT )->SetWindowText( strData );
	}
#endif
}


void CCommon::OnBnClickedBtnFlat8inchRead()
{
#if AJINTEX_BOARD
	double dwPosition = 0L;
	CString strData("");
	if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( X_AXIS, dwPosition ) == RC_NORMAL )
	{
		strData.Format( "%7.3f", dwPosition);
		GetDlgItem( IDC_EDT_X_8INCH_FLAT )->SetWindowText( strData );
	}
	if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( Y_AXIS, dwPosition ) == RC_NORMAL )
	{
		strData.Format( "%7.3f", dwPosition);
		GetDlgItem( IDC_EDT_Y_8INCH_FLAT )->SetWindowText( strData );
	}
#endif
}


void CCommon::OnBnClickedBtnFlat12inchRead()
{
#if AJINTEX_BOARD
	double dwPosition = 0L;
	CString strData("");
	if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( X_AXIS, dwPosition ) == RC_NORMAL )
	{
		strData.Format( "%7.3f", dwPosition);
		GetDlgItem( IDC_EDT_X_12INCH_FLAT )->SetWindowText( strData );
	}
	if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( Y_AXIS, dwPosition ) == RC_NORMAL )
	{
		strData.Format( "%7.3f", dwPosition);
		GetDlgItem( IDC_EDT_Y_12INCH_FLAT )->SetWindowText( strData );
	}
#endif
}


void CCommon::OnBnClickedBtnNorch4inchMove()
{
#if AJINTEX_BOARD
	double dwPositionX = 0.0L;
	double dwPositionY = 0.0L;
	CString strDataX("");
	CString strDataY("");
	GetDlgItem( IDC_EDT_X_4INCH_NORCH )->GetWindowText( strDataX );
	GetDlgItem( IDC_EDT_Y_4INCH_NORCH )->GetWindowText( strDataY );
	dwPositionX = atof( strDataX );
	dwPositionY = atof( strDataY );
	m_fnSetMove( dwPositionX, dwPositionY );
#endif
}


void CCommon::OnBnClickedBtnNorch6inchMove()
{
#if AJINTEX_BOARD
	double dwPositionX = 0.0L;
	double dwPositionY = 0.0L;
	CString strDataX("");
	CString strDataY("");
	GetDlgItem( IDC_EDT_X_6INCH_NORCH )->GetWindowText( strDataX );
	GetDlgItem( IDC_EDT_Y_6INCH_NORCH )->GetWindowText( strDataY );
	dwPositionX = atof( strDataX );
	dwPositionY = atof( strDataY );
	m_fnSetMove( dwPositionX, dwPositionY );
#endif
}


void CCommon::OnBnClickedBtnNorch8inchMove()
{
#if AJINTEX_BOARD
	double dwPositionX = 0.0L;
	double dwPositionY = 0.0L;
	CString strDataX("");
	CString strDataY("");
	GetDlgItem( IDC_EDT_X_8INCH_NORCH )->GetWindowText( strDataX );
	GetDlgItem( IDC_EDT_Y_8INCH_NORCH )->GetWindowText( strDataY );
	dwPositionX = atof( strDataX );
	dwPositionY = atof( strDataY );
	m_fnSetMove( dwPositionX, dwPositionY );
#endif
}


void CCommon::OnBnClickedBtnNorch12inchMove()
{
#if AJINTEX_BOARD
	double dwPositionX = 0.0L;
	double dwPositionY = 0.0L;
	CString strDataX("");
	CString strDataY("");
	GetDlgItem( IDC_EDT_X_12INCH_NORCH )->GetWindowText( strDataX );
	GetDlgItem( IDC_EDT_Y_12INCH_NORCH )->GetWindowText( strDataY );
	dwPositionX = atof( strDataX );
	dwPositionY = atof( strDataY );
	m_fnSetMove( dwPositionX, dwPositionY );
#endif
}


void CCommon::OnBnClickedBtnFlat4inchMove()
{
#if AJINTEX_BOARD
	double dwPositionX = 0.0L;
	double dwPositionY = 0.0L;
	CString strDataX("");
	CString strDataY("");
	GetDlgItem( IDC_EDT_X_4INCH_FLAT )->GetWindowText( strDataX );
	GetDlgItem( IDC_EDT_Y_4INCH_FLAT )->GetWindowText( strDataY );
	dwPositionX = atof( strDataX );
	dwPositionY = atof( strDataY );
	m_fnSetMove( dwPositionX, dwPositionY );
#endif
}


void CCommon::OnBnClickedBtnFlat6inchMove()
{
#if AJINTEX_BOARD
	double dwPositionX = 0.0L;
	double dwPositionY = 0.0L;
	CString strDataX("");
	CString strDataY("");
	GetDlgItem( IDC_EDT_X_6INCH_FLAT )->GetWindowText( strDataX );
	GetDlgItem( IDC_EDT_Y_6INCH_FLAT )->GetWindowText( strDataY );
	dwPositionX = atof( strDataX );
	dwPositionY = atof( strDataY );
	m_fnSetMove( dwPositionX, dwPositionY );
#endif
}


void CCommon::OnBnClickedBtnFlat8inchMove()
{
#if AJINTEX_BOARD
	double dwPositionX = 0.0L;
	double dwPositionY = 0.0L;
	CString strDataX("");
	CString strDataY("");
	GetDlgItem( IDC_EDT_X_8INCH_FLAT )->GetWindowText( strDataX );
	GetDlgItem( IDC_EDT_Y_8INCH_FLAT )->GetWindowText( strDataY );
	dwPositionX = atof( strDataX );
	dwPositionY = atof( strDataY );
	m_fnSetMove( dwPositionX, dwPositionY );
#endif
}


void CCommon::OnBnClickedBtnFlat12inchMove()
{
#if AJINTEX_BOARD
	double dwPositionX = 0.0L;
	double dwPositionY = 0.0L;
	CString strDataX("");
	CString strDataY("");
	GetDlgItem( IDC_EDT_X_12INCH_FLAT )->GetWindowText( strDataX );
	GetDlgItem( IDC_EDT_Y_12INCH_FLAT )->GetWindowText( strDataY );
	dwPositionX = atof( strDataX );
	dwPositionY = atof( strDataY );
	m_fnSetMove( dwPositionX, dwPositionY );
#endif
}

void CCommon::OnBnClickedBtnExcelPath()
{
	LPCTSTR lpszTitle = _T( "결과파일(EXCEL) 저장경로" );
	UINT uFlags		  = BIF_RETURNONLYFSDIRS;

	CString strFullPath("");

	CFolderDialog dlg(lpszTitle, m_pTask->stCommon.szExcelResultPath, this, uFlags );

	if ( dlg.DoModal() == IDOK )
	{    
		strFullPath = dlg.GetFolderPath(); 
		strFullPath += "\\";
		GetDlgItem( IDC_EDT_RESULT_EXCEL_PATH )->SetWindowText( strFullPath );
	}
}


void CCommon::OnBnClickedBtnPathXml()
{
	LPCTSTR lpszTitle = _T( "결과파일(XML) 저장경로" );
	UINT uFlags		  = BIF_RETURNONLYFSDIRS;

	CString strFullPath("");

	CFolderDialog dlg(lpszTitle, m_pTask->stCommon.szXmlResultPath, this, uFlags );

	if ( dlg.DoModal() == IDOK )
	{    
		strFullPath = dlg.GetFolderPath(); 
		strFullPath += "\\";
		GetDlgItem( IDC_EDT_RESULT_XML_PATH )->SetWindowText( strFullPath );
	}
}


void CCommon::OnBnClickedBtnTrendPath()
{
	LPCTSTR lpszTitle = _T( "결과파일(TREND) 저장경로" );
	UINT uFlags		  = BIF_RETURNONLYFSDIRS;

	CString strFullPath("");

	CFolderDialog dlg(lpszTitle, m_pTask->stCommon.szTrendResultPath, this, uFlags );

	if ( dlg.DoModal() == IDOK )
	{    
		strFullPath = dlg.GetFolderPath(); 
		strFullPath += "\\";
		GetDlgItem( IDC_EDT_RESULT_TREND_PATH )->SetWindowText( strFullPath );
	}
}


void CCommon::OnBnClickedBtnLightValue()
{
	UpdateData(TRUE);
	LightControl(0, 1, m_iEditLightVal);
	m_pTask->stCommon.nInspLight = m_iEditLightVal;
}
