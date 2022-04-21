#pragma once
#if MARIA_DB
#include <mysql.h>
#include <WinSock2.h>
#pragma comment(lib,"libmariadb.lib")
#pragma comment(lib,"ws2_32.lib")
#include "afxwin.h"
#include "afxcmn.h"

/****  mysql ������ �߰��Ѵ�.  ****/

extern MYSQL_ROW row;		// mysql �� ���� �ô´�.
extern MYSQL_RES *m_res;	// mysql�� ����� �޾ƿ´�
extern MYSQL mysql;			// mysql�� ��ü���� ������ ����Ѵ�

#define MYSQL_IP			"127.0.0.1"
#define MYSQL_USERID		"root"
#define MYSQL_PWD			"0000"
#define	PORT_NO				3306
#define DB_NAME				"EllipsoMeter"
#endif
// CSearchDLG ��ȭ �����Դϴ�.

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
	CSearchDLG(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CSearchDLG();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SEARCH_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnCreateTbl();
};
