// PasswordDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EllipsoMeter.h"
#include "PasswordDlg.h"
#include "Direct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg dialog


CPasswordDlg::CPasswordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPasswordDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPasswordDlg)
	this->Create( IDD_PASSWORD );
	m_strPassword = _T("");
	m_strUserID = _T("");
	//}}AFX_DATA_INIT
}

BOOL CPasswordDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (( nID   == 2 ) &&
		( nCode == -1 ))
	{
		return FALSE;
	}
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CPasswordDlg::PreTranslateMessage(MSG* pMsg) 
{
	// ENTER KEY or ESC KEY CANCEL
	if (( pMsg->wParam == 13 ) || 
		( pMsg->wParam == 27 ))
	{
		return FALSE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CPasswordDlg::m_fnSetShowFlag( BOOL bShow )
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

//---------------------------------------------------------------------------------------------------------
// 시작에 필요한 작업을 한다.
//---------------------------------------------------------------------------------------------------------
void CPasswordDlg::m_fnSetInit( ) 
{
	CString s("");
	s.Format( "사용자 확인창" );
	this->SetWindowText(s);

	m_pTaskHandle = new CTaskHandle( this, SHARED_NAME, 
										1, 
										sizeof( SHEET_RESISTANCE_Format ) + 1,
										0,
										FALSE );

	m_pTask					= ( SHEET_RESISTANCE_Format* )m_pTaskHandle->m_pFilePointer( );

	m_fnGetUserInfo();
	for( int i = 0; i < m_pTask->stLogin.nTotal; i++ )
	{
		m_clPassword.AddString( m_pTask->stUsrList[i].szUserID );
	}

	m_clPassword.SetCurSel(1);
}

//---------------------------------------------------------------------------------------------------------
// 종료에 필요한 작업을 한다.
//---------------------------------------------------------------------------------------------------------
void CPasswordDlg::m_fnSetEnd() 
{
	if ( m_pTaskHandle != NULL )
	{
		m_pTaskHandle->m_fnSetEnd();
		delete m_pTaskHandle;
		m_pTaskHandle = NULL;
	}
}


void CPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPasswordDlg)
	DDX_Text(pDX, IDC_EDT_PASSWORD, m_strPassword);
	DDX_Text(pDX, IDC_EDT_USERID, m_strUserID);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_EDT_USERID, m_clUserID);
	DDX_Control(pDX, IDC_CBO_PASSWORD, m_clPassword);
}


BEGIN_MESSAGE_MAP(CPasswordDlg, CDialog)
	//{{AFX_MSG_MAP(CPasswordDlg)
	ON_BN_CLICKED(IDC_BTN_CANCEL, OnBtnCancel)
	ON_BN_CLICKED(IDC_BTN_OK, OnBtnOk)
	//}}AFX_MSG_MAP
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg message handlers

void CPasswordDlg::OnBtnCancel() 
{
	m_fnSetShowFlag( FALSE );
}

void CPasswordDlg::m_fnDefaultUserAdd() 
{
	memset( m_pTask->stUsrList, 0x00, sizeof(m_pTask->stUsrList));
	m_pTask->stLogin.nTotal = 2;
	sprintf_s( m_pTask->stUsrList[0].szUserID, "%s", "AIM" );
	sprintf_s( m_pTask->stUsrList[0].szName, "%s", "(주)에이아이엠" );
	sprintf_s( m_pTask->stUsrList[0].szJob, "%s", "면 저항측정 검사기 엔지니어" );
	sprintf_s( m_pTask->stUsrList[0].szDescription, "%s", "" );
	sprintf_s( m_pTask->stUsrList[0].szPasswd, "%s", "0000" );
	m_pTask->stUsrList[0].nLevel = MAKER_ENGINEER;
	sprintf_s( m_pTask->stUsrList[1].szUserID, "%s", "NANO" );
	sprintf_s( m_pTask->stUsrList[1].szName, "%s", "한국나노기술원" );
	sprintf_s( m_pTask->stUsrList[1].szJob, "%s", "면 저항측정 검사기 엔지니어" );
	sprintf_s( m_pTask->stUsrList[1].szDescription, "%s", "" );
	sprintf_s( m_pTask->stUsrList[1].szPasswd, "%s", "0000" );
	m_pTask->stUsrList[1].nLevel = SYSTEM_ENGINEER;
	m_fnSetUserInfo();
}

CString CPasswordDlg::m_fnGetCurrentTime()
{
	SYSTEMTIME st;
	char szWork[80];
	CString cstrCurTime = "";
	memset(szWork, 0x00, sizeof(szWork));
	GetLocalTime( &st );
	
	cstrCurTime.Format("%04d%02d%02d", 
		                st.wYear, st.wMonth, st.wDay);
	return cstrCurTime;
}

void CPasswordDlg::m_fnGetUserInfo()
{
	CString			strWork("");
	CString			strFullPath("");
	CString			strCurrPath("");
	int             nLevel = UNKNOWN_USER;

	CStringArray strID;
	CStringArray strNAME;
	CStringArray strJOB;
	CStringArray strLEVEL;
	CStringArray strDESCRIPTION;
	CStringArray strPASSWORD;

	CStringArray strLst;
	CString      strData("");

	CXmlElement *pElement     = NULL;
	CXmlElement *pRootElement = NULL;

	strFullPath = m_fnGetUserFileFullPath( );
	if ( m_Util.Existence(strFullPath) != FM_FILE )
	{
		m_fnDefaultUserAdd();
		return;
	}

	CXmlDocument* pXML = new CXmlDocument();
	pXML->Load( strFullPath );

	pXML->GetTagToString( USER_SECTION , strData );
	pXML->Parse( strData );
	pRootElement = pXML->GetRootElement();
	if ( pRootElement == NULL )
	{
		delete pXML;
		pXML = NULL;
		return;
	}

	strLst.RemoveAll();
	strLst.Add(ELEMENT);
	pElement = pXML->FindElement( pRootElement, strLst );
	if ( pElement != NULL )
	{
		pXML->FindData( pElement, USER_TOTAL, strWork );
	}

	strLst.RemoveAll();
	strLst.Add(BODY);
	strLst.Add(USER_LIST);
	pElement = pXML->FindElement( pRootElement, strLst );
	if ( pElement != NULL )
	{
		strID.RemoveAll();
		strNAME.RemoveAll();
		strJOB.RemoveAll();
		strPASSWORD.RemoveAll();
		strDESCRIPTION.RemoveAll();
		strLEVEL.RemoveAll();
		pXML->FindUserList( pElement, strID, strNAME, strJOB, strPASSWORD, strLEVEL, strDESCRIPTION );
		m_pTask->stLogin.nTotal = strID.GetSize();
		for ( int i = 0; i < strID.GetSize(); i++ )
		{
			strWork = strID.GetAt( i );
			strWork.Trim();
			sprintf_s(m_pTask->stUsrList[i].szUserID, "%s", strID.GetAt(i).GetString());
			sprintf_s( m_pTask->stUsrList[i].szName, "%s", strNAME.GetAt(i).GetString());
			sprintf_s( m_pTask->stUsrList[i].szJob, "%s", strJOB.GetAt(i).GetString());
			sprintf_s( m_pTask->stUsrList[i].szPasswd, "%s", strPASSWORD.GetAt(i).GetString());
			sprintf_s( m_pTask->stUsrList[i].szDescription, "%s", strDESCRIPTION.GetAt(i).GetString());
			nLevel = atoi(strLEVEL.GetAt(i));
			if (( nLevel == UNKNOWN_USER ) ||
			    ( nLevel == OPERATOR ) ||
			    ( nLevel == SYSTEM_ENGINEER ) ||
			    ( nLevel == MAKER_ENGINEER ))
			{
				m_pTask->stUsrList[i].nLevel = nLevel;
			}
			else
			{
				m_pTask->stUsrList[i].nLevel = UNKNOWN_USER;
			}
		}
	}

	delete pXML;
	pXML = NULL;

}

//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
CString CPasswordDlg::m_fnGetUserFileFullPath( )
{
	char szFullPath[MAX_PATH + 1];
	memset( szFullPath, 0x00, sizeof( szFullPath ));

	sprintf_s(szFullPath, "%s%s%s%s", m_Util.m_fnGetCurrentDrive().GetString(),
									 DEFAULT_PATH,
									 DEFAULT_USER_PATH,
									 DEFAULT_USER_FILENAME );
	szFullPath[MAX_PATH] = NULL;

	return szFullPath;
}

//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
CString CPasswordDlg::m_fnGetStandardUserFileFullPath( )
{
	char szFullPath[MAX_PATH + 1];
	memset( szFullPath, 0x00, sizeof( szFullPath ));

	sprintf_s(szFullPath, "%s%s%s%s", m_Util.m_fnGetCurrentDrive().GetString(),
									 DEFAULT_PATH,
									 DEFAULT_ENV_PATH,
									 DEFAULT_USER_FILENAME );
	szFullPath[MAX_PATH] = NULL;

	return szFullPath;
}

void CPasswordDlg::m_fnSetUserInfo()
{
	CString strWork("");
	CString strData("");
	CString strFullPath("");
	CString strBackup("");

	CStringArray strLst;
	CXmlElement *pElement     = NULL;
	CXmlElement *pRootElement = NULL;

	strFullPath		 =	m_fnGetUserFileFullPath( );
	if ( m_Util.Existence(strFullPath) != FM_FILE )
	{

		strBackup        =   strFullPath;
		strBackup       +=   ".";
		strBackup       +=   m_Util.m_fnGetCurrentTime1();

		::rename( strFullPath, strBackup );
	}
	
	theApp.m_fnGetViewPointer()->m_fnLogWrite( "USER 관리파일 저장" );

	CXmlDocument* pXML = new CXmlDocument();
	pXML->Load( m_fnGetStandardUserFileFullPath() );

	pXML->GetTagToString( USER_SECTION , strData );
	pXML->Parse( strData );
	pRootElement = pXML->GetRootElement();
	if ( pRootElement == NULL )
	{
		delete pXML;
		pXML = NULL;
		return;
	}

	int nCnt = 0;
	strLst.RemoveAll();
	strLst.Add(BODY);
	strLst.Add(USER_LIST);
	pElement = pXML->FindElement( pRootElement, strLst );
	if ( pElement != NULL )
	{
		for ( int i = 0; i < MAX_USER; i++ )
		{
			strWork.Format( "%s", m_pTask->stUsrList[i].szUserID );
			strWork.Trim();
			if ( !strWork.IsEmpty() )
			{
				CXmlElement* pElement1 = pXML->AddElement( pElement, USER_INFO );
				pXML->AddElement( pElement1, USER_ID, m_pTask->stUsrList[i].szUserID );
				pXML->AddElement( pElement1, USER_NAME, m_pTask->stUsrList[i].szName );
				pXML->AddElement( pElement1, USER_PASSWORD, m_pTask->stUsrList[i].szPasswd );
				pXML->AddElement( pElement1, USER_DESCRIPTION, m_pTask->stUsrList[i].szDescription );
				pXML->AddElement( pElement1, USER_JOB, m_pTask->stUsrList[i].szJob );
				if (( m_pTask->stUsrList[i].nLevel == UNKNOWN_USER ) ||
					( m_pTask->stUsrList[i].nLevel == OPERATOR ) ||
					( m_pTask->stUsrList[i].nLevel == SYSTEM_ENGINEER ) ||
					( m_pTask->stUsrList[i].nLevel == MAKER_ENGINEER ))
				{
					strWork.Format( "%01d", m_pTask->stUsrList[i].nLevel ); 
				}
				else
				{
					strWork.Format( "%01d", UNKNOWN_USER ); 
				}
				pXML->AddElement( pElement1, USER_LEVEL, strWork );
				nCnt++;
			}
		}
	}
	m_pTask->stLogin.nTotal = nCnt;
	strLst.RemoveAll();
	strLst.Add(ELEMENT);
	pElement = pXML->FindElement( pRootElement, strLst );
	if ( pElement != NULL )
	{
		strWork.Format( "%01d", m_pTask->stLogin.nTotal );
		pXML->DataSet( pElement, USER_TOTAL, strWork );
	}

	pXML->Store( strFullPath );

	delete pXML;
	pXML = NULL;
}

void CPasswordDlg::OnBtnOk() 
{
	UpdateData(TRUE);

	CString      strWork("");
	CString      strUserID("");
	CString      strPwd("");
	int          nLevel = 0;


	m_strUserID.Trim();
	m_strPassword.Trim();

	// USERID
	int nNo = m_clPassword.GetCurSel();
	if ( nNo == -1 )
	{
		TECmsgOut( L"사용자ID를 선택해 주십시요.....", GREEN_1, 1 );
		return;
	}

	m_clPassword.GetLBText( nNo, m_strUserID );

	if ( m_strUserID.IsEmpty() )
	{
		TECmsgOut( L"사용자ID를 선택해 주십시요.....", GREEN_1, 1 );
		return;
	}

	if ( m_strPassword.IsEmpty() )
	{
		TECmsgOut( L"패스워드를 입력해 주십시요.....", GREEN_1, 1 );
		return;
	}

	for( int i = 0; i < m_pTask->stLogin.nTotal; i++ )
	{
		strUserID.Format( "%s", m_pTask->stUsrList[i].szUserID );
		strPwd.Format( "%s", m_pTask->stUsrList[i].szPasswd );
		if (( strUserID == m_strUserID ) &&
			( strPwd == m_strPassword ))
		{
			nLevel = m_pTask->stUsrList[i].nLevel;

			if (nLevel == MAKER_ENGINEER)
				theApp.m_fnGetViewPointer()->m_pAuto->m_fnSetBtnEnable(0);
			else if (nLevel == UNKNOWN_USER)
				theApp.m_fnGetViewPointer()->m_pAuto->m_fnSetBtnEnable(100);
			else
				theApp.m_fnGetViewPointer()->m_pAuto->m_fnSetBtnEnable(1);


			if (( nLevel == SYSTEM_ENGINEER ) ||
				( nLevel == OPERATOR )        ||
				( nLevel == MAKER_ENGINEER )  ||
				( nLevel == UNKNOWN_USER ))
			{
				sprintf_s(theApp.m_fnGetViewPointer()->m_pTask->stLogin.szUserID, "%s", m_strUserID.GetString());
				m_strPassword = _T("");
				sprintf_s( theApp.m_fnGetViewPointer()->m_pTask->stLogin.szPasswd, "%s", m_strPassword.GetString());
				theApp.m_fnGetViewPointer()->m_pTask->stLogin.nLevel = nLevel;
				theApp.m_fnGetViewPointer()->m_pTask->stLogin.bLogin = TRUE;
				theApp.m_fnGetViewPointer()->m_fnSetBtnOperation( IDC_BTN_LOGIN,L_BIT_BTN_ON,L_BIT_CHAR_ON,"로그아웃");

				m_fnSetShowFlag( FALSE );
				UpdateData(FALSE);
				return;
			}
			else
			{
				TECmsgOut( L"LEVEL 설정이 잘못되어 있습니다. 담당자에게 문의하세요.....", GREEN_1, 1 );
				theApp.m_fnGetViewPointer()->m_pTask->stLogin.nLevel = UNKNOWN_USER;
				theApp.m_fnGetViewPointer()->m_pTask->stLogin.bLogin = FALSE;
				theApp.m_fnGetViewPointer()->m_fnSetBtnOperation( IDC_BTN_LOGIN,L_BIT_BTN_OFF,L_BIT_CHAR_OFF,"로그인");
				m_strPassword = _T("");
				UpdateData(FALSE);
				return;
			}
		}
	}

	m_strPassword = _T("");
	TECmsgOut( L"UserID 또는 Password가 일치하지 않습니다. 담당자에게 문의하세요.....", GREEN_1, 1 );
	theApp.m_fnGetViewPointer()->m_pTask->stLogin.nLevel = UNKNOWN_USER;
	theApp.m_fnGetViewPointer()->m_pTask->stLogin.bLogin = FALSE;
	theApp.m_fnGetViewPointer()->m_fnSetBtnOperation( IDC_BTN_LOGIN,L_BIT_BTN_OFF,L_BIT_CHAR_OFF,"로그인");

	UpdateData(FALSE);
}

