// DataClientSocketEx.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "EllipsoMeter.h"
#include "DataClientSocketEx.h"


CDataClientSocketEx::CDataClientSocketEx( CDialog* pDlg, int nPort )
{
	m_pMainDlg  = pDlg;
	m_bConnect  = FALSE;
	m_nPort     = nPort;

}

CDataClientSocketEx::~CDataClientSocketEx()
{
}


// CDataClientSocketEx ��� �Լ�


void CDataClientSocketEx::OnReceive(int nErrorCode)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	if ( m_pMainDlg->m_hWnd != NULL )
		m_pMainDlg->SendMessage( WM_LAN_RECEIVE, m_nPort, 0 );

	CAsyncSocket::OnReceive(nErrorCode);
}


void CDataClientSocketEx::OnClose(int nErrorCode)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	if ( m_pMainDlg->m_hWnd != NULL )
		m_pMainDlg->SendMessage( WM_LAN_CLOSE, m_nPort, 0 );

	CAsyncSocket::OnClose(nErrorCode);
}


void CDataClientSocketEx::OnConnect(int nErrorCode)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	if(nErrorCode != 0)
	{
		m_bConnect = FALSE;
	}
	else
	{
		m_bConnect = TRUE;
	}
	if ( m_pMainDlg->m_hWnd != NULL )
		m_pMainDlg->SendMessage( WM_LAN_CONNECT, m_nPort, 0 );

	CAsyncSocket::OnConnect(nErrorCode);
}

BOOL CDataClientSocketEx::IsConnect( )
{
	return m_bConnect;
}
