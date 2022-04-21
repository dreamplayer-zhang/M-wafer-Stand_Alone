#pragma once


// CModelMapDLG 대화 상자입니다.

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
	CModelMapDLG(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CModelMapDLG();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_MODEL_MAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnModelMapHide();
	afx_msg void OnBnClickedBtnCurrLoc();
	afx_msg void OnBnClickedBtnSet();
	afx_msg void OnBnClickedBtnRepeat();
	afx_msg void OnBnClickedBtnLightValue();
	CComboBox m_clMethod;
};
