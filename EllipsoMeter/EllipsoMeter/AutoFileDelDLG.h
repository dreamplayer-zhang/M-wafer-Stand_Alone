#pragma once

#include <direct.h>
#include "FolderDlg.h"

static  int nUSE[]  = { IDC_CHK_USE01, IDC_CHK_USE02, IDC_CHK_USE03, IDC_CHK_USE04, IDC_CHK_USE05,
	IDC_CHK_USE06, IDC_CHK_USE07, IDC_CHK_USE08, IDC_CHK_USE09, IDC_CHK_USE10 };

static  int nPATH[] = { IDC_EDT_PATH01, IDC_EDT_PATH02, IDC_EDT_PATH03, IDC_EDT_PATH04, IDC_EDT_PATH05,
	IDC_EDT_PATH06, IDC_EDT_PATH07, IDC_EDT_PATH08, IDC_EDT_PATH09, IDC_EDT_PATH10 };

static  int nDAYS[] = { IDC_EDT_TIME01, IDC_EDT_TIME02, IDC_EDT_TIME03, IDC_EDT_TIME04, IDC_EDT_TIME05,
	IDC_EDT_TIME06, IDC_EDT_TIME07, IDC_EDT_TIME08, IDC_EDT_TIME09, IDC_EDT_TIME10 };

static  int nEXT[]  = { IDC_EDT_EXT01, IDC_EDT_EXT02, IDC_EDT_EXT03, IDC_EDT_EXT04, IDC_EDT_EXT05,
	IDC_EDT_EXT06, IDC_EDT_EXT07, IDC_EDT_EXT08, IDC_EDT_EXT09, IDC_EDT_EXT10 };

// CAutoFileDelDLG 대화 상자입니다.
UINT AutoDelThread(LPVOID pFuncData);

class CAutoFileDelDLG : public CDialogEx
{
	DECLARE_DYNAMIC(CAutoFileDelDLG)

	void							m_fnSetShowFlag( BOOL bShow );

	void							m_fnSetInit( void ); 
	void							m_fnSetEnd( void ); 

	BOOL							m_bShow;

	CString m_fnGetFolderPath( void );

	t_DEL_INFO				m_stDEL[MAX_CNT+1];

	HANDLE								m_hAutoDelThread;
	BOOL								m_bAutoDelEnd;

	CString					m_strToday;
	void					m_fnAutoDelProcess( void );
	static CString          m_fnGetToday( void );
	static BOOL             m_fnLogFind(CString cstrFilePath, CString strExt, int nDay);
	static CString          m_fnGetCurrentTime3(int nDay);
	static int				m_fnGetFileList( CString strPath, CString strName, CStringArray& strFileName );
	static BOOL				Del(LPCTSTR lpSource, BOOL bHidePrompt = TRUE);
	static int				Existence(LPCTSTR lpSource);

public:
	CAutoFileDelDLG(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAutoFileDelDLG();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_AUTO_DEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBtnSave();
	afx_msg void OnBtnPath01();
	afx_msg void OnBtnPath02();
	afx_msg void OnBtnPath03();
	afx_msg void OnBtnPath04();
	afx_msg void OnBtnPath05();
	afx_msg void OnBtnPath06();
	afx_msg void OnBtnPath07();
	afx_msg void OnBtnPath08();
	afx_msg void OnBtnPath009();
	afx_msg void OnBtnPath10();
	afx_msg void OnBnClickedBtnHide();
};
