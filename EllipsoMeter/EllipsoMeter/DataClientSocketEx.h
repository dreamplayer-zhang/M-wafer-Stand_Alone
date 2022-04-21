#pragma once

#define WM_LAN_CONNECT						(WM_USER + 310)	
#define WM_LAN_RECEIVE						(WM_USER + 320)
#define WM_LAN_CLOSE						(WM_USER + 330)
class CDataClientSocketEx : public CAsyncSocket
{
public:
	CDataClientSocketEx( CDialog* pDlg, int nPort = IP_PORT );

	CDialog*				m_pMainDlg;
	BOOL                    m_bConnect;
	BOOL					IsConnect( void );
	int                     m_nPort;

	virtual ~CDataClientSocketEx();
	virtual					void OnReceive(int nErrorCode);
	virtual					void OnClose(int nErrorCode);
	virtual					void OnConnect(int nErrorCode);
};


