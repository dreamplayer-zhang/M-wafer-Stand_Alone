// DataServerSocketEx.cpp : 구현 파일입니다.
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


// CDataServerSocketEx 멤버 함수


void CDataServerSocketEx::SetOwner(CDialog* pDlg )
{
	m_pMainDlg = pDlg;
}


void CDataServerSocketEx::OnAccept(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	m_pMainDlg->PostMessage( WM_LAN_ACCEPT, m_nPort, 0 );

	CAsyncSocket::OnAccept(nErrorCode);
}
