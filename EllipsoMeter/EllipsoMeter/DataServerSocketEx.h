#pragma once

#define	WM_LAN_ACCEPT					(WM_USER + 300)		

class CDataServerSocketEx : public CAsyncSocket
{
public:
	CDataServerSocketEx( int nPort = IP_PORT );

	CDialog*				m_pMainDlg;
	void					SetOwner(CDialog* pDlg);
	int                     m_nPort;

	virtual ~CDataServerSocketEx();
	virtual void OnAccept(int nErrorCode);
};


