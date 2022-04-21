// PasswordManagerDLG.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EllipsoMeter.h"
#include "PasswordManagerDLG.h"
#include "afxdialogex.h"


// CPasswordManagerDLG 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPasswordManagerDLG, CDialogEx)

CPasswordManagerDLG::CPasswordManagerDLG(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPasswordManagerDLG::IDD, pParent)
{
	this->Create( IDD_PASSWORD_MANAGER );
	m_bShow    = FALSE;
}

CPasswordManagerDLG::~CPasswordManagerDLG()
{
}

void CPasswordManagerDLG::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CBO_LEVEL, m_clLevel);
	DDX_Control(pDX, IDC_LST_USER_LIST, m_clUserList);
}


BEGIN_MESSAGE_MAP(CPasswordManagerDLG, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_HIDE, &CPasswordManagerDLG::OnBnClickedBtnHide)
	ON_BN_CLICKED(IDC_BTN_ADD, &CPasswordManagerDLG::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_DEL, &CPasswordManagerDLG::OnBnClickedBtnDel)
	ON_BN_CLICKED(IDC_BTN_UPDATE, &CPasswordManagerDLG::OnBnClickedBtnUpdate)
	ON_BN_CLICKED(IDC_BTN_CHANGE, &CPasswordManagerDLG::OnBnClickedBtnChange)
	ON_LBN_SELCHANGE(IDC_LST_USER_LIST, &CPasswordManagerDLG::OnLbnSelchangeLstUserList)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------------------------
// 외부 참조용 Function
//---------------------------------------------------------------------------------------------
BOOL CPasswordManagerDLG::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (( nID   == 2 ) &&
		( nCode == -1 ))
	{
		m_fnSetShowFlag( FALSE );
	}
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CPasswordManagerDLG::m_fnSetShowFlag( BOOL bShow )
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

BOOL CPasswordManagerDLG::PreTranslateMessage(MSG* pMsg) 
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
void CPasswordManagerDLG::m_fnSetInit( ) 
{
	CString s("");
	s.Format( "패스워드 변경창" );
	this->SetWindowText(s);

	m_pTaskHandle = new CTaskHandle( this, SHARED_NAME, 
										1, 
										sizeof( SHEET_RESISTANCE_Format ) + 1,
										0,
										FALSE );

	m_pTask					= ( SHEET_RESISTANCE_Format* )m_pTaskHandle->m_pFilePointer( );

	GetDlgItem( IDC_EDT_PASSWORD )->EnableWindow(FALSE);
	GetDlgItem( IDC_BTN_CHANGE )->EnableWindow(TRUE);

	m_clUserList.ResetContent();
	for( int i = 0; i < m_pTask->stLogin.nTotal; i++ )
	{
		m_clUserList.AddString( m_pTask->stUsrList[i].szUserID );
	}
}

//---------------------------------------------------------------------------------------------------------
// 종료에 필요한 작업을 한다.
//---------------------------------------------------------------------------------------------------------
void CPasswordManagerDLG::m_fnSetEnd() 
{
	if ( m_pTaskHandle != NULL )
	{
		m_pTaskHandle->m_fnSetEnd();
		delete m_pTaskHandle;
		m_pTaskHandle = NULL;
	}
}

void CPasswordManagerDLG::OnBnClickedBtnHide()
{
	m_fnSetShowFlag( FALSE );
}


void CPasswordManagerDLG::OnBnClickedBtnAdd()
{
	GetDlgItem( IDC_EDT_USERID )->SetWindowText( "" );
	GetDlgItem( IDC_EDT_PASSWORD )->EnableWindow(TRUE);
	GetDlgItem( IDC_EDT_JOB )->SetWindowText( "" );
	GetDlgItem( IDC_EDT_NAME )->SetWindowText( "" );
	GetDlgItem( IDC_EDT_DESCRIPTION )->SetWindowText( "" );
	GetDlgItem( IDC_BTN_CHANGE )->EnableWindow(FALSE);
	m_clLevel.SetCurSel( UNKNOWN_USER );
}


void CPasswordManagerDLG::OnBnClickedBtnDel()
{
	CString strWork("");
	CString strUserID("");
	int nTotal = m_pTask->stLogin.nTotal; 
	GetDlgItem( IDC_EDT_USERID )->GetWindowText( strUserID );
	strUserID.Trim();
	if (( strUserID == "AIM" ) ||
		( strUserID == "NANO" ))
	{
		TECmsgOut( L"메이커 지정 USERID는 삭제 불가합니다.", RED_1, 1 );
		return;
	}
	strWork.Format( "%s", m_pTask->stLogin.szUserID );
	strWork.Trim();
	if ( strUserID == strWork )
	{
		TECmsgOut( L"로그인중인 USERID는 삭제 불가합니다.", RED_1, 1 );
		return;
	}
	for( int i = 0; i < nTotal; i++ )
	{
		strWork.Format( "%s", m_pTask->stUsrList[i].szUserID );
		strWork.Trim();
		if ( strUserID == strWork )
		{
			sprintf_s( m_pTask->stUsrList[i].szUserID, "%s", "" );
			sprintf_s( m_pTask->stUsrList[i].szPasswd, "%s", ""	);
			sprintf_s( m_pTask->stUsrList[i].szName, "%s", "" );
			sprintf_s( m_pTask->stUsrList[i].szJob, "%s", "" );
			sprintf_s( m_pTask->stUsrList[i].szDescription, "%s", "" );
			m_pTask->stUsrList[i].nLevel = UNKNOWN_USER;
			m_pTask->stLogin.nTotal--;
		}
	}
	theApp.m_fnGetViewPointer()->m_pLogin->m_fnSetUserInfo();
	m_clUserList.ResetContent();
	for( int i = 0; i < m_pTask->stLogin.nTotal; i++ )
	{
		m_clUserList.AddString( m_pTask->stUsrList[i].szUserID );
	}
}


void CPasswordManagerDLG::OnBnClickedBtnUpdate()
{
	CString strWork("");
	CString strUserID("");
	CString strUpdateID("");
	GetDlgItem( IDC_EDT_USERID )->GetWindowText( strUserID );
	strUserID.Trim();
	BOOL bNewUser = TRUE;
	for( int i = 0; i < m_pTask->stLogin.nTotal; i++ )
	{
		strUpdateID.Format( "%s", m_pTask->stUsrList[i].szUserID );
		strUpdateID.Trim();
		if ( strUserID == strUpdateID )
		{
			GetDlgItem( IDC_EDT_USERID )->GetWindowText( strWork );
			sprintf_s(m_pTask->stUsrList[i].szUserID, "%s", strWork.GetString());
			
			GetDlgItem( IDC_EDT_PASSWORD )->GetWindowText( strWork );
			sprintf_s( m_pTask->stUsrList[i].szPasswd, "%s", strWork.GetString());

			GetDlgItem( IDC_EDT_NAME )->GetWindowText( strWork );
			sprintf_s( m_pTask->stUsrList[i].szName, "%s", strWork.GetString());

			GetDlgItem( IDC_EDT_JOB )->GetWindowText( strWork );
			sprintf_s( m_pTask->stUsrList[i].szJob, "%s", strWork.GetString());
			
			GetDlgItem( IDC_EDT_DESCRIPTION )->GetWindowText( strWork );
			sprintf_s( m_pTask->stUsrList[i].szDescription, "%s", strWork.GetString());

			m_pTask->stUsrList[i].nLevel = m_clLevel.GetCurSel( );
			bNewUser = FALSE;
		}
	}

	if ( m_pTask->stLogin.nTotal > MAX_USER ) return;

	if ( bNewUser )
	{
		GetDlgItem( IDC_EDT_USERID )->GetWindowText( strWork );
		sprintf_s(m_pTask->stUsrList[m_pTask->stLogin.nTotal].szUserID, "%s", strWork.GetString());
		GetDlgItem( IDC_EDT_PASSWORD )->GetWindowText( strWork );
		sprintf_s( m_pTask->stUsrList[m_pTask->stLogin.nTotal].szPasswd, "%s", strWork.GetString());
		GetDlgItem( IDC_EDT_NAME )->GetWindowText( strWork );
		sprintf_s( m_pTask->stUsrList[m_pTask->stLogin.nTotal].szName, "%s", strWork.GetString());
		GetDlgItem( IDC_EDT_JOB )->GetWindowText( strWork );
		sprintf_s( m_pTask->stUsrList[m_pTask->stLogin.nTotal].szJob, "%s", strWork.GetString());
		GetDlgItem( IDC_EDT_DESCRIPTION )->GetWindowText( strWork );
		sprintf_s( m_pTask->stUsrList[m_pTask->stLogin.nTotal].szDescription, "%s", strWork.GetString());
		m_pTask->stUsrList[m_pTask->stLogin.nTotal].nLevel = m_clLevel.GetCurSel( );
		m_pTask->stLogin.nTotal++;
	}
	theApp.m_fnGetViewPointer()->m_pLogin->m_fnSetUserInfo();
	m_clUserList.ResetContent();
	for( int i = 0; i < m_pTask->stLogin.nTotal; i++ )
	{
		m_clUserList.AddString( m_pTask->stUsrList[i].szUserID );
	}
}


void CPasswordManagerDLG::OnBnClickedBtnChange()
{
	CString strUserID("");
	CString strPassword("");
	GetDlgItem( IDC_EDT_PASSWORD )->GetWindowText( strPassword );
	theApp.m_fnGetViewPointer()->m_pPwdChg->m_fnSetCurrentPassword( strPassword );
	theApp.m_fnGetViewPointer()->m_pPwdChg->m_fnSetShowFlag( TRUE );
}


void CPasswordManagerDLG::OnLbnSelchangeLstUserList()
{
	CString strUserID("");
	CString strListID("");
	m_clUserList.GetText( m_clUserList.GetCurSel(), strListID );
	strListID.Trim();
	for( int i = 0; i < m_pTask->stLogin.nTotal; i++ )
	{
		strUserID.Format( "%s", m_pTask->stUsrList[i].szUserID );
		strUserID.Trim();
		if ( strUserID == strListID )
		{
			GetDlgItem( IDC_EDT_USERID )->SetWindowText( strUserID );
			GetDlgItem( IDC_EDT_PASSWORD )->SetWindowText( m_pTask->stUsrList[i].szPasswd );
			GetDlgItem( IDC_EDT_NAME )->SetWindowText( m_pTask->stUsrList[i].szName );
			GetDlgItem( IDC_EDT_JOB )->SetWindowText( m_pTask->stUsrList[i].szJob );
			GetDlgItem( IDC_EDT_DESCRIPTION )->SetWindowText( m_pTask->stUsrList[i].szDescription );
			m_clLevel.SetCurSel( m_pTask->stUsrList[i].nLevel);
		}

	}
}
