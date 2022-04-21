// SearchDLG.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EllipsoMeter.h"
#include "SearchDLG.h"
#include "afxdialogex.h"

#if MARIA_DB
MYSQL_ROW row;
MYSQL_RES *m_res;
MYSQL mysql; 
#endif
IMPLEMENT_DYNAMIC(CSearchDLG, CDialogEx)

CSearchDLG::CSearchDLG(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSearchDLG::IDD, pParent)
{
	this->Create( IDD_SEARCH_DLG );
	m_bShow    = FALSE;
	m_bConnect = FALSE;
}

CSearchDLG::~CSearchDLG()
{
}

void CSearchDLG::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSearchDLG, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_CREATE_TBL, &CSearchDLG::OnBnClickedBtnCreateTbl)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------------------------
// 외부 참조용 Function
//---------------------------------------------------------------------------------------------
BOOL CSearchDLG::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (( nID   == 2 ) &&
		( nCode == -1 ))
	{
		m_fnSetShowFlag( FALSE );
	}
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CSearchDLG::m_fnSetShowFlag( BOOL bShow )
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

BOOL CSearchDLG::PreTranslateMessage(MSG* pMsg) 
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
void CSearchDLG::m_fnSetInit( ) 
{
	CString s("");
	s.Format( "ReView 메인창" );
	this->SetWindowText(s);

	this->MoveWindow( 10, 100, 1900, 870, TRUE );
	m_pTaskHandle = new CTaskHandle( this, SHARED_NAME, 
										1, 
										sizeof( SHEET_RESISTANCE_Format ) + 1,
										0,
										FALSE );

	m_pTask					= ( SHEET_RESISTANCE_Format* )m_pTaskHandle->m_pFilePointer( );
#if MARIA_DB
	mysql_init(&mysql);
	m_fnMySQLConnect( MYSQL_IP, PORT_NO, MYSQL_USERID, MYSQL_PWD, DB_NAME );  
#endif
}

//---------------------------------------------------------------------------------------------------------
// 종료에 필요한 작업을 한다.
//---------------------------------------------------------------------------------------------------------
void CSearchDLG::m_fnSetEnd() 
{
	m_fnMySQLClose();

	if ( m_pTaskHandle != NULL )
	{
		m_pTaskHandle->m_fnSetEnd();
		delete m_pTaskHandle;
		m_pTaskHandle = NULL;
	}
}

void CSearchDLG::m_fnMySQLConnect(CString strIP, int nPort, CString strUserID, CString strPwd, CString strDbName )
{
	if ( m_bConnect )
	{
		m_fnMySQLClose();
	}

#if MARIA_DB
	if(!mysql_real_connect(&mysql, strIP, strUserID, strPwd, strDbName ,nPort,0,0))
	{
		m_bConnect = FALSE;
		theApp.m_fnGetViewPointer()->m_fnLogWrite(mysql_error(&mysql));
	}
	else
	{
		mysql_query(&mysql,"set names euckr"); //한글 인식
		m_bConnect = TRUE;
		theApp.m_fnGetViewPointer()->m_fnLogWrite("DB 연결 성공");
	}
#endif
}
void CSearchDLG::m_fnMySQLClose()
{
	if ( m_bConnect )
	{
#if MARIA_DB
		mysql_close(&mysql);
#endif
		m_bConnect = FALSE;
		theApp.m_fnGetViewPointer()->m_fnLogWrite("DB 닫기 성공");
	}
}



void CSearchDLG::OnBnClickedBtnCreateTbl()
{
}
