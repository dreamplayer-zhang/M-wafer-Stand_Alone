// AutoMeasureParamDLG.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EllipsoMeter.h"
#include "AutoMeasureParamDLG.h"
#include "afxdialogex.h"


// CAutoMeasureParamDLG 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAutoMeasureParamDLG, CDialogEx)

CAutoMeasureParamDLG::CAutoMeasureParamDLG(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAutoMeasureParamDLG::IDD, pParent)
{
	this->Create( IDD_USER_AUTO_MEASURE );
	m_bShow = FALSE;
	m_pTaskHandle       = NULL;
	m_pTask             = NULL;
}

CAutoMeasureParamDLG::~CAutoMeasureParamDLG()
{
}

void CAutoMeasureParamDLG::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAutoMeasureParamDLG, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_HIDE, &CAutoMeasureParamDLG::OnBnClickedBtnHide)
END_MESSAGE_MAP()

BOOL CAutoMeasureParamDLG::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (( nID   == 2 ) &&
		( nCode == -1 ))
	{
		m_fnSetShowFlag( FALSE );
	}
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CAutoMeasureParamDLG::m_fnSetShowFlag( BOOL bShow )
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

BOOL CAutoMeasureParamDLG::PreTranslateMessage(MSG* pMsg) 
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
void CAutoMeasureParamDLG::m_fnSetInit() 
{
	CString s("");
	s.Format( "자동 측정 파라메터 설정" );
	this->SetWindowText(s);
	m_pTaskHandle = new CTaskHandle( this, SHARED_NAME, 
											1, 
											sizeof( SHEET_RESISTANCE_Format ) + 1,
											0,
											FALSE );

	m_pTask					= ( SHEET_RESISTANCE_Format* )m_pTaskHandle->m_pFilePointer( );
}

//---------------------------------------------------------------------------------------------------------
// 종료에 필요한 작업을 한다.
//---------------------------------------------------------------------------------------------------------
void CAutoMeasureParamDLG::m_fnSetEnd( ) 
{
	if ( m_pTaskHandle != NULL )
	{
		m_pTaskHandle->m_fnSetEnd();
		delete m_pTaskHandle;
		m_pTaskHandle = NULL;
	}
}


void CAutoMeasureParamDLG::OnBnClickedBtnHide()
{
	m_fnSetShowFlag( FALSE );
}
