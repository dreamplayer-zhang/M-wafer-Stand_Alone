#pragma once
#include "StdAfx.h"
#include "DataClientSocketEx.h"
#include "DataServerSocketEx.h"
#include "afxwin.h"
#include "XmlDocument.h"
#include "TaskHandle.h"

// CElipsoMeterIF 대화 상자입니다.

class CElipsoMeterRobotIF : public CDialogEx
{
	DECLARE_DYNAMIC(CElipsoMeterRobotIF)

	CDataClientSocketEx*			m_pClient;
	CDataServerSocketEx*			m_pServer;

	BOOL							m_bConnect;
	BOOL							m_bServerRun;
	BOOL                            m_bCommandSend;
	BOOL							m_bShow;	

	CTaskHandle*					m_pTaskHandle;
	SHEET_RESISTANCE_Format*		m_pTask;

	void							m_fnSetShowFlag( BOOL bShow );

	void							m_fnSetInit( ); 
	void							m_fnSetEnd( void ); 
	BOOL							m_fnSetELIPSOMETER_ROBOT_Save( void );
	BOOL							m_fnGetELIPSOMETER_ROBOT_Load( void );
	CString							m_fnGetELIPSOMETERROBOTFileFullPath( void );
	void							m_fnSetOpen( void ); 
	void							m_fnSend( CString strData );
	void							m_fnAnalysis_Data( void );

	char							m_szData[MAX_BUFF_SIZE+1];
	void							m_fnLogWrite( CString strMsg, BOOL bWrite = true );

	LRESULT							OnReceive(WPARAM nID, LPARAM lParam);
	LRESULT							OnClose(WPARAM nID, LPARAM lParam);
	LRESULT							OnConnectCheck(WPARAM nID, LPARAM lParam);
	LRESULT							OnAccept(WPARAM nID, LPARAM lParam);
	t_LAN_INFO                      m_stLan;

	CString		                    m_strData;
	CLogProcess						m_Log;
	CStringArray                    m_strRcvData;
	BOOL                            m_bProcess;


public:
	CElipsoMeterRobotIF(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CElipsoMeterRobotIF();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ELIPSOMETER_ROBOT_IF };
	CListBox						m_lstComm;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	afx_msg void OnBnClickedBtnOpen();
	afx_msg void OnBnClickedBtnClose();
	afx_msg void OnBnClickedBtnLineSet();
	afx_msg void OnBnClickedBtnClr();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedBtnExit();
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnLanSend();
	afx_msg void OnBnClickedBtnSaveData();
};
