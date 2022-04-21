#pragma once


// CPasswordChangeDLG 대화 상자입니다.

class CPasswordChangeDLG : public CDialogEx
{
	DECLARE_DYNAMIC(CPasswordChangeDLG)
	BOOL							m_bShow;	
	void							m_fnSetShowFlag( BOOL bShow );

	void							m_fnSetInit( void ); 
	void							m_fnSetEnd( void );

	CString                         m_strPassword;
	void							m_fnSetCurrentPassword( CString strPassword );

	CTaskHandle*					m_pTaskHandle;
	SHEET_RESISTANCE_Format*		m_pTask;

public:
	CPasswordChangeDLG(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CPasswordChangeDLG();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PASSWORD_CHANGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnOk();
	afx_msg void OnBnClickedBtnCancel();
};
