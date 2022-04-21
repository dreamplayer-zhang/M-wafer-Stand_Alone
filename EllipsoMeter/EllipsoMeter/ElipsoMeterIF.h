#pragma once
#include "StdAfx.h"
#include "DataClientSocketEx.h"
#include "DataServerSocketEx.h"
#include "afxwin.h"
#include "XmlDocument.h"
#include "TaskHandle.h"

UINT LoadRecipeThread(LPVOID pFuncData);
UINT ReadyThread(LPVOID pFuncData);
UINT AlignThread(LPVOID pFuncData);
UINT ResultThread(LPVOID pFuncData);
// CElipsoMeterIF 대화 상자입니다.

class CElipsoMeterIF : public CDialogEx
{
	DECLARE_DYNAMIC(CElipsoMeterIF)

	CDataClientSocketEx*			m_pClient1;
	CDataServerSocketEx*			m_pServer1;
	CDataClientSocketEx*			m_pClient2;
	CDataServerSocketEx*			m_pServer2;

	BOOL							m_bConnect1;
	BOOL							m_bConnect2;
	BOOL							m_bServerRun;
	BOOL                            m_bCommandSend;
	BOOL							m_bShow;	

	CTaskHandle*					m_pTaskHandle;
	SHEET_RESISTANCE_Format*		m_pTask;

	int                             m_nStep;
	BOOL                            m_bResultProcess;
	HANDLE							m_hResultThread;
	BOOL                            m_bResultEnd;
	BOOL                            m_bComplete;
	BOOL                            m_bError;
	BOOL                            m_bYesNo;

	CString                         m_strRunRecipe;
	double                          m_dLampHour;
	double							m_dAlignZ;

	void							m_fnSetShowFlag( BOOL bShow );

	void							m_fnSetInit( ); 
	void							m_fnSetEnd( void ); 
	BOOL							m_fnSetELIPSOMETER_Save( void );
	BOOL							m_fnGetELIPSOMETER_Load( void );
	CString							m_fnGetELIPSOMETERFileFullPath( void );
	void							m_fnSetOpen( void ); 
	void							m_fnSend1( CString strData );
	void							m_fnSend2( CString strData );
	void							m_fnAnalysis_Data1( void );
	void							m_fnAnalysis_Data2( void );
	void							m_fnSetRunRecipe(CString strRecipeName);
	void							m_fnSetLoadRecipe(CString strRecipeName);
	void m_fnSetLoadRecipe();
	void                            m_fnGetStatus(void);
	void                            m_fnGetInitStatus( void );
	void							m_fnGetModelStructure( void );
	void							m_fnGetSystemGetLampHours( void );
	void							m_fnSetSystemLampShutterOpen( void );
	void							m_fnSetSystemFilterWheelAutoOn( void );
	void							m_fnSetSystemLampShutterClosed( void );
	void							m_fnSetSystemFilterWheelAutoOff( void );
	void							m_fnGetSystemGetReceiverCameraAlign( void );
	void							m_fnSetReadyEllipso();
	void							m_fnSetSystemLampOn(void);
	void							m_fnSetSystemLampOff( void );
	void							m_fnGetSystemLampIsOn( void );
	void							m_fnGetSystemLampPowerIsOn( void );

	void m_fnGetAlignZ();
	void							m_fnGetResult(t_ELIPSO_RESULT_INFO& stResult);
	void							m_fnSetMeasureStart( CString strRecipeName );
	void							m_fnGetRecipeList(t_GET_RECIPE& stRcpList);
	void							m_fnGetListRecipes( void );
	void							m_fnGetLampHour(t_GET_LAMP_HOUR& stLampHour);
	void							m_fnGetSearch(CString strItem, CString &strResult, int &nFindResult);
	CString                         m_fnGetDelete(CString strItem);

	char							m_szData[MAX_BUFF_SIZE+1];
	void							m_fnLogWrite( CString strMsg, BOOL bWrite = true );
	int                             m_nCmdNo;

	CStringArray                    m_strResult;
	CStringArray                    m_strRecipeList;
	t_ELIPSO_RESULT_INFO			m_stResult;
	t_MODEL_STRUCT                  m_stModelStruct;

	LRESULT							OnReceive(WPARAM nID, LPARAM lParam);
	LRESULT							OnClose(WPARAM nID, LPARAM lParam);
	LRESULT							OnConnectCheck(WPARAM nID, LPARAM lParam);
	LRESULT							OnAccept(WPARAM nID, LPARAM lParam);
	t_LAN_INFO                      m_stLan;

	CString		                    m_strData1;
	CString		                    m_strData2;
	CLogProcess						m_Log;
	CStringArray                    m_strRcvData1;
	CStringArray                    m_strRcvData2;
	BOOL                            m_bProcess;

	CString                         m_strMSE;
	CString                         m_strAngleOffset;
	CString                         m_strTotalThickness;

	CString                         m_strAnalysisData;



public:
	CElipsoMeterIF(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CElipsoMeterIF();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ELIPSOMETER_IF };
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
	afx_msg void OnBnClickedBtnStartAcq();
	afx_msg void OnBnClickedBtnListRecipes();
	afx_msg void OnBnClickedBtnRunRecipe();
	CListBox m_clRecipeList;
	afx_msg void OnBnClickedBtnLoadRecipe();
	afx_msg void OnBnClickedBtnStatus();
	afx_msg void OnBnClickedBtnInitStatus();
	afx_msg void OnBnClickedBtnGetModelStructure();
	afx_msg void OnBnClickedBtnSystemGetLampHours();
	afx_msg void OnBnClickedBtnSystemLampShutterOpen();
	afx_msg void OnBnClickedBtnSystemFilterWheelAutoOn();
	afx_msg void OnBnClickedBtnSystemLampShutterClosed();
	afx_msg void OnBnClickedBtnSystemFilterWheelAutoOff();
	afx_msg void OnBnClickedBtnSystemGetReceiverCameraAlign();
	afx_msg void OnBnClickedBtnMeasure();
	afx_msg void OnBnClickedBtnAlign();
	afx_msg void OnBnClickedBtnSystemLampOn();
	afx_msg void OnBnClickedBtnSystemLampOff();
	afx_msg void OnBnClickedBtnSystemLampIsOn();
	afx_msg void OnBnClickedBtnSystemLampPowerIsOn();
};
