#pragma once
#if MARIA_DB
#include <mysql.h>
#include <WinSock2.h>
#pragma comment(lib,"libmariadb.lib")
#pragma comment(lib,"ws2_32.lib")
#include "afxwin.h"
#include "afxcmn.h"

/****  mysql 변수를 추가한다.  ****/

extern MYSQL_ROW row;		// mysql 의 행을 맡는다.
extern MYSQL_RES *m_res;	// mysql의 결과를 받아온다
extern MYSQL mysql;			// mysql의 전체적인 연동을 담당한다

#define MYSQL_IP			"127.0.0.1"
#define MYSQL_USERID		"root"
#define MYSQL_PWD			"0000"
#define	PORT_NO				3306
#define DB_NAME				"EllipsoMeter"
#endif
// CSearchDLG 대화 상자입니다.

class CSearchDLG : public CDialogEx
{
	DECLARE_DYNAMIC(CSearchDLG)
	BOOL							m_bShow;	
	void							m_fnSetShowFlag( BOOL bShow );

	void							m_fnSetInit( void ); 
	void							m_fnSetEnd( void ); 

	CTaskHandle*					m_pTaskHandle;
	SHEET_RESISTANCE_Format*		m_pTask;

	BOOL                            m_bConnect;
	void							m_fnMySQLConnect(CString strIP, int nPort, CString strUserID, CString strPwd, CString strDbName );
	void							m_fnMySQLClose();

public:
	CSearchDLG(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSearchDLG();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SEARCH_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnCreateTbl();
};
