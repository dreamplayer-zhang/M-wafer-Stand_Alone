#pragma once


// CModelMapDLG ��ȭ �����Դϴ�.

class CModelMapDLG : public CDialogEx
{
	DECLARE_DYNAMIC(CModelMapDLG)
	void							m_fnSetShowFlag( BOOL bShow );

	void							m_fnSetInit(); 
	void							m_fnSetEnd( void );
	BOOL                            m_bShow;

	CTaskHandle*					m_pTaskHandle;
	SHEET_RESISTANCE_Format*		m_pTask;

public:
	CModelMapDLG(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CModelMapDLG();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_MODEL_MAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnModelMapHide();
	afx_msg void OnBnClickedBtnCurrLoc();
	afx_msg void OnBnClickedBtnSet();
	afx_msg void OnBnClickedBtnRepeat();
	afx_msg void OnBnClickedBtnLightValue();
	CComboBox m_clMethod;
};
