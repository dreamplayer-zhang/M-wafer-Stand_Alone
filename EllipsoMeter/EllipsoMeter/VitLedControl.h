#pragma once

#include "EllipsoMeter.h"
#include "EllipsoMeterDlg.h"
#include "TaskHandle.h"
#include "multiportxx.h"
#include "afxwin.h"

// CVitLedControl 대화 상자입니다.

class CVitLedControl : public CDialogEx
{
	DECLARE_DYNAMIC(CVitLedControl)
public:
	CVitLedControl(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CVitLedControl();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_VIT_LIGHT };

#if VIT_LED_LIGHT_CTL
	void							m_fnSetShowFlag( BOOL bShow );

	void							m_fnSetInit( void ); 
	void							m_fnSetEnd( void );

	#define	BUFF_SIZE				8192
	CString							m_PortID;
	BYTE							m_byData[BUFF_SIZE];
	int                             m_nMsgCnt;
	BYTE							m_byLastCH;
	WORD                            m_wLightData;

	BOOL							m_bShow;	
	CTaskHandle*					m_pTaskHandle;
	SHEET_RESISTANCE_Format*		m_pTask;
	t_COM_PORT_INFO					m_stComm;
	CLogProcess						m_Log;
	CLogProcess						m_HexaLog;

	BOOL                            m_bConnect;

	char							m_szData[MAX_BUFF_SIZE+1];
	void							m_fnLogWrite( CString strMsg );
	void							m_fnLogRvcDataWrite( CString strMsg );
	BOOL							m_fnSetMakeFolder( void );
	CString							m_fnGetLightFileFullPath( void );

	void							m_fnPortOpen( void ); 
	BOOL							m_fnGetLight_Load( void );
	BOOL							m_fnSetLight_Save( void );
	BOOL							m_fnSetSend( BYTE* byBuffer, int nLen );
	void							m_fnSetSend( CString strData );
	BOOL							m_fnSetONOFF( WORD wData );
	BOOL							m_fnSetChannelData( BOOL bApply, int nCh, CString strData );
	BOOL							m_fnSetDataReqest( int nCh );
	void							m_fnSetParsing( BYTE* szData, int nLen );
	void							m_fnSetLightOnOff( BOOL bOnOff );
	void							m_fnSetLightValue( BOOL bApply, CString strData );
	void							m_fnSetAllLightOff( void );
	void							m_fnSetLightOnOff( int nCh, BOOL bOnOff );
	void							m_fnSetLightValue( int nCh, BOOL bApply, CString strData );

public:

	afx_msg void OnBtnClr();
	afx_msg void OnBtnLineSet();
	afx_msg void OnBtnCommSet();
	afx_msg void OnBtnOpen();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedBtnClose();
	CListBox m_lstRcvComm;
	CListBox m_lstComm;
	afx_msg void OnBnClickedBtnExit();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnSend();
	CComboBox m_clPort;
	CComboBox m_clBaudRate;
	CComboBox m_clDataBit;
	CComboBox m_clStopBit;
	CComboBox m_clParityBit;
	CComboBox m_clFlowCtl;

	DECLARE_MESSAGE_MAP()
	CMultiportxx m_Multiportxx;
	DECLARE_EVENTSINK_MAP()
	void OnSerialReceiveMultiportxx(LPCTSTR idstr, long BufferPointer, long BufferSize, long UserMemorySize, long UserMemoryPointer);
public:
	afx_msg void OnBnClickedBtnLightDataReq();
	afx_msg void OnBnClickedBtnLight();
	afx_msg void OnBnClickedBtnLightData();
	afx_msg void OnBnClickedBtnLightDataOnly();
	CComboBox m_clRequest;
	CComboBox m_clChData;
#endif
};
