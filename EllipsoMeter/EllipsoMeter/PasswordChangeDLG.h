#pragma once


// CPasswordChangeDLG ��ȭ �����Դϴ�.

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
	CPasswordChangeDLG(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CPasswordChangeDLG();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_PASSWORD_CHANGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnOk();
	afx_msg void OnBnClickedBtnCancel();
};
