#pragma once


// CAutoMeasureParamDLG ��ȭ �����Դϴ�.

class CAutoMeasureParamDLG : public CDialogEx
{
	DECLARE_DYNAMIC(CAutoMeasureParamDLG)
	void							m_fnSetShowFlag( BOOL bShow );

	void							m_fnSetInit(); 
	void							m_fnSetEnd( void );
	BOOL                            m_bShow;

	CTaskHandle*					m_pTaskHandle;
	SHEET_RESISTANCE_Format*		m_pTask;

public:
	CAutoMeasureParamDLG(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CAutoMeasureParamDLG();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_USER_AUTO_MEASURE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnHide();
};
