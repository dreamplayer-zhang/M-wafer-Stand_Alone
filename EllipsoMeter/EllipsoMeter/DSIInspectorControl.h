#pragma once

#include "EllipsoMeter.h"
#include "EllipsoMeterDlg.h"
#include "multiportxx.h"
#include "afxwin.h"

// CDSIInspectorControl 대화 상자입니다.

#if DSP_INSPECTOR
UINT RcpDataThread(LPVOID pFuncData);
class CDSIInspectorControl : public CDialogEx
{
	DECLARE_DYNAMIC(CDSIInspectorControl)
public:
	CDSIInspectorControl(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDSIInspectorControl();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_DSI_CONTROL };

	void							m_fnSetShowFlag( BOOL bShow );

	void							m_fnSetInit( void ); 
	void							m_fnSetEnd( void );

	BYTE							m_byLastCH;
	WORD                            m_wLightData;
	int                             m_nLen;
	CString                         m_strASCII;
	CString                         m_strHEXA;
	BOOL                            m_bConnect;
	CString                         m_strLog;
	CStringArray                    m_strRecData;

	HANDLE							m_hRcpDataThread;
	BOOL                            m_bRcpDataEnd;

	BOOL							m_bShow;	
	t_COM_PORT_INFO					m_stComm;

	BOOL							m_bMeasure;
	BOOL                            m_bError;

	CString                         m_strV;
	int                             m_nStepNo;
	double                          m_dR;
	double                          m_dS;
	double                          m_dV;
	double                          m_dC;

	CLogProcess						m_Log;
	CLogProcess						m_HexaLog;
	char							m_szData[MAX_BUFF_SIZE+1];

	void							m_fnLogWrite( CString strMsg );
	void							m_fnLogRvcDataWrite( CString strMsg );
	BOOL							m_fnSetMakeFolder( void );
	CString							m_fnGetCurrentDrive( void );
	BOOL							CharToHex( char* szBuffer, int nLen, CString& strResult );
	int                             m_fnSetCreateDir(CString strFolder);

	void							m_fnPortOpen( void ); 
	BOOL							m_fnGetDSI_Load( void );
	BOOL							m_fnSetDSI_Save( void );
	BOOL							m_fnSetSend( BYTE* byBuffer, int nLen, BOOL bDisp = true );
	void							m_fnSetSend( CString strData, BOOL bDisp = true );
	CString							m_fnGetDSIFileFullPath( void );
	BOOL							m_fnDataAnaylsys( CString strData );
	BOOL							m_fnSetResultDisplayR( double dValue, int &nDani, double &dResult );
	BOOL							m_fnSetResultDisplayS( double dValue, int &nDani, double &dResult );
	BOOL							m_fnSetResultDisplayV( double dValue, int &nDani, double &dResult );
	BOOL							m_fnSetResultDisplayC( double dValue, int &nDani, double &dResult );
	BOOL							m_fnSetEtoDecial( CString strValue, double &dResult );

	t_DATA_INFO                     m_stData; 

	CString							m_PortID;
	BYTE							m_byData[BUFF_SIZE];
	int                             m_nMsgCnt;

	void							m_fnSetMeasureMode( int nMode );
	void							m_fnSetResisterMode( int nMode );
	void							m_fnSetMxMode( int nMode );
	void							m_fnSetMctMode( int nMode );
	void							m_fnSetMPMeasureSpeed( int nSpeed );
	void							m_fnSetReMeasureDataReq( void );
	void							m_fnSetMeasureDelayTime( int nDelayTime );
	void							m_fnSetHighLimit( int nStep );
	void							m_fnSetLowLimit( int nStep );
	void							m_fnSetCurrentStep( int nStep );
	void							m_fnSetMeasureStart( void );
	void							m_fnSetMeasurePause( void );
	void							m_fnSetRemoteControlEnable( void );
	void							m_fnSetRemoteControlDisable( void );
	void							m_fnSetBeepOn( void );
	void							m_fnSetBeepOff( void );
	void							m_fnSetCurrentMeasureStepNoReq( void );
	void							m_fnSetBeforeMeasureData( void );
	void							m_fnSetOnly1DataSend( void );
	void							m_fnSetRsvcSend( void );
	void							m_fnSetDataMemoryClear( void );
	void							m_fnSetRequestMemoryData( void );

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
public:
	afx_msg void OnBnClickedBtnLf();
	afx_msg void OnBnClickedBtnCr();
	afx_msg void OnBnClickedBtnEot();
	afx_msg void OnBnClickedBtnSoh();
	CMultiportxx m_Multiportxx;
	DECLARE_EVENTSINK_MAP()
	void OnSerialReceiveMultiportxx(LPCTSTR idstr, long BufferPointer, long BufferSize, long UserMemorySize, long UserMemoryPointer);
	afx_msg void OnBnClickedBtnMeasureMode();
	CComboBox m_clMeasureMode;
	afx_msg void OnBnClickedBtnResisterMode();
	CComboBox m_clResisterMode;
	CComboBox m_clMxMode;
	afx_msg void OnBnClickedBtnMxMode();
	CComboBox m_clMctMode;
	afx_msg void OnBnClickedBtnMctMode();
	afx_msg void OnBnClickedBtnMpSpeed();
	CComboBox m_clMpMeasureSpeed;
	afx_msg void OnBnClickedBtnReMeasureDataReq();
	afx_msg void OnBnClickedBtnDelayTime();
	afx_msg void OnBnClickedBtnHighLimit();
	afx_msg void OnBnClickedBtnLowLimit();
	afx_msg void OnBnClickedBtnCurrentStep();
	CComboBox m_clHighLimit;
	CComboBox m_clLowLimit;
	CComboBox m_clCurrStep;
	afx_msg void OnBnClickedBtnMeasureStart();
	afx_msg void OnBnClickedBtnMeasurePause();
	afx_msg void OnBnClickedBtnRemoteControlEnable();
	afx_msg void OnBnClickedBtnRemoteControlDisable();
	afx_msg void OnBnClickedBtnBeepOn();
	afx_msg void OnBnClickedBtnBeepOff();
	afx_msg void OnBnClickedBtnCurrentMeasureStepNoReq();
	afx_msg void OnBnClickedBtnBeforeMeasureData();
	afx_msg void OnBnClickedBtnOnly1DataSend();
	afx_msg void OnBnClickedBtnRsvcSend();
	afx_msg void OnBnClickedBtnDataMemoryClear();
	afx_msg void OnBnClickedBtnRequestMemoryData();
};
#endif