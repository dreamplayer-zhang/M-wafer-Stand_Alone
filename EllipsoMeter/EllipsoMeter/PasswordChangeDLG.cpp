// PasswordChangeDLG.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "EllipsoMeter.h"
#include "PasswordChangeDLG.h"
#include "afxdialogex.h"


// CPasswordChangeDLG ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CPasswordChangeDLG, CDialogEx)

CPasswordChangeDLG::CPasswordChangeDLG(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPasswordChangeDLG::IDD, pParent)
{
	this->Create( IDD_PASSWORD_CHANGE );
	m_bShow    = FALSE;
}

CPasswordChangeDLG::~CPasswordChangeDLG()
{
}

void CPasswordChangeDLG::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPasswordChangeDLG, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_OK, &CPasswordChangeDLG::OnBnClickedBtnOk)
	ON_BN_CLICKED(IDC_BTN_CANCEL, &CPasswordChangeDLG::OnBnClickedBtnCancel)
END_MESSAGE_MAP()


void CPasswordChangeDLG::OnBnClickedBtnOk()
{
	CString strData(""); 
	CString strData1(""); 
	GetDlgItem( IDC_EDT_PASSWORD )->GetWindowText( strData );
	if ( strData != m_strPassword )
	{
		TECmsgOut( L"���� �н������ ��ġ���� �ʽ��ϴ�.", RED_1, 1 );
		return;
	}
	GetDlgItem( IDC_EDT_NEW_PASSWORD )->GetWindowText( strData );
	GetDlgItem( IDC_EDT_PASSWORD_CONFIRM )->GetWindowText( strData1 );
	if ( strData1 != strData )
	{
		TECmsgOut( L"���ο� �н������ Ȯ�� �н����尡 ��ġ���� �ʽ��ϴ�.", RED_1, 1 );
		return;
	}

	theApp.m_fnGetViewPointer()->m_pUserInfo->GetDlgItem( IDC_EDT_PASSWORD )->SetWindowText( strData );
	m_fnSetShowFlag( FALSE );
}


void CPasswordChangeDLG::OnBnClickedBtnCancel()
{
	m_fnSetShowFlag( FALSE );
}

//---------------------------------------------------------------------------------------------
// �ܺ� ������ Function
//---------------------------------------------------------------------------------------------
BOOL CPasswordChangeDLG::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (( nID   == 2 ) &&
		( nCode == -1 ))
	{
		m_fnSetShowFlag( FALSE );
	}
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CPasswordChangeDLG::m_fnSetShowFlag( BOOL bShow )
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

BOOL CPasswordChangeDLG::PreTranslateMessage(MSG* pMsg) 
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
void CPasswordChangeDLG::m_fnSetCurrentPassword( CString strPassword )
{
	m_strPassword = strPassword;
}

//---------------------------------------------------------------------------------------------------------
// ���ۿ� �ʿ��� �۾��� �Ѵ�.
//---------------------------------------------------------------------------------------------------------
void CPasswordChangeDLG::m_fnSetInit( ) 
{
	CString s("");
	s.Format( "�н����� ����â" );
	this->SetWindowText(s);

	m_pTaskHandle = new CTaskHandle( this, SHARED_NAME, 
										1, 
										sizeof( SHEET_RESISTANCE_Format ) + 1,
										0,
										FALSE );

	m_pTask					= ( SHEET_RESISTANCE_Format* )m_pTaskHandle->m_pFilePointer( );

}

//---------------------------------------------------------------------------------------------------------
// ���ῡ �ʿ��� �۾��� �Ѵ�.
//---------------------------------------------------------------------------------------------------------
void CPasswordChangeDLG::m_fnSetEnd() 
{
	if ( m_pTaskHandle != NULL )
	{
		m_pTaskHandle->m_fnSetEnd();
		delete m_pTaskHandle;
		m_pTaskHandle = NULL;
	}
}


