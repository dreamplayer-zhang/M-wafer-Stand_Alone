// ModelManage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EllipsoMeter.h"
#include "ModelManage.h"
#include "afxdialogex.h"


// CModelManage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CModelManage, CDialogEx)

CModelManage::CModelManage(CWnd* pParent/*=NULL*/)
	: CDialogEx(CModelManage::IDD, pParent)
	, m_strMODEL(_T(""))
	, m_strNewMODEL(_T(""))
{
	m_pView				= theApp.m_fnGetViewPointer( );
	m_bFirst            = FALSE;
	m_nNO               = 0;
}

CModelManage::~CModelManage()
{
}

void CModelManage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDT_MODEL, m_strMODEL);
	DDX_Text(pDX, IDC_EDT_NEW_MODEL, m_strNewMODEL);
}


BEGIN_MESSAGE_MAP(CModelManage, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_APPLY, &CModelManage::OnBnClickedBtnApply)
	ON_BN_CLICKED(IDC_BTN_CANCEL, &CModelManage::OnBnClickedBtnCancel)
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()

BOOL CModelManage::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (( nID   == 2 ) &&
		( nCode == -1 ))
	{
		return FALSE;
	}
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CModelManage::PreTranslateMessage(MSG* pMsg) 
{
	// ENTER KEY or ESC KEY CANCEL
	if (( pMsg->wParam == 13 ) || 
		( pMsg->wParam == 27 ))
	{
		return FALSE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

// CModelManage 메시지 처리기입니다.


void CModelManage::OnBnClickedBtnApply()
{
	UpdateData( TRUE );
	OnOK();
}


void CModelManage::OnBnClickedBtnCancel()
{
	OnCancel();
}

void CModelManage::m_fnInit( int nNO, CString strName )
{
	m_nNO		= nNO;
	m_strName	= strName;
}

void CModelManage::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialogEx::OnActivate(nState, pWndOther, bMinimized);
	
	if ( !m_bFirst )
	{
		switch ( m_nNO )
		{
			case 1 :
				{
					GetDlgItem( IDC_NEW_MODEL )->ShowWindow(FALSE);
					GetDlgItem( IDC_EDT_NEW_MODEL )->ShowWindow(FALSE);
				}
				break;
			case 2 :
				{
					GetDlgItem( IDC_EDT_MODEL )->SetWindowText( m_strName );
				}
				break;
		}
	}
}
