// DataServerSocketEx.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "EllipsoMeter.h"
#include "DataServerSocketEx.h"


// CDataServerSocketEx

CDataServerSocketEx::CDataServerSocketEx( int nPort )
{
	m_nPort = nPort;
}

CDataServerSocketEx::~CDataServerSocketEx()
{
}


// CDataServerSocketEx ��� �Լ�


void CDataServerSocketEx::SetOwner(CDialog* pDlg )
{
	m_pMainDlg = pDlg;
}


void CDataServerSocketEx::OnAccept(int nErrorCode)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	m_pMainDlg->PostMessage( WM_LAN_ACCEPT, m_nPort, 0 );

	CAsyncSocket::OnAccept(nErrorCode);
}
