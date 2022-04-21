#pragma once

#include "EllipsoMeterDlg.h"
#include "EllipsoMeter.h"
#include "BtnST.h"
#include "afxwin.h"

UINT JogMoveThread(LPVOID pFuncData);

class CJogMove : public CDialogEx
{
	DECLARE_DYNAMIC(CJogMove)
	void							m_fnSetShowFlag( BOOL bShow );

	void							m_fnSetInit( ); 
	void							m_fnSetEnd( void ); 

	BOOL							m_bShow;	

	void							m_fnSetImageButton( BOOL bUp, short nNo );

	double                          m_dX;
	double                          m_dY;
	double                          m_dZ;
	double                          m_dR;

	int                             m_nXY;
	int                             m_nZ;

	BOOL                            m_bJogDisp;

	HANDLE							m_hJogMoveThread;
	BOOL                            m_bJogMoveEnd;
	void							m_fnSetEdit( int nIDCtl, double dPosition );

public:
	CJogMove(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CJogMove();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_JOGMOVE };
	CButtonST						m_clUp;
	CButtonST						m_clDown;
	CButtonST						m_clFront;
	CButtonST						m_clBack;
	CButtonST						m_clLeft;
	CButtonST						m_clRight;
	CButtonST						m_clCw;
	CButtonST						m_clCcw;
	CComboBox						m_clJogMove;
	CComboBox						m_clJogSpeed;
	CComboBox						m_clJogSpeedZ;
	CComboBox						m_clJogSpeedR;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg LRESULT m_fnSetJog( WPARAM nNo, LPARAM lParam );
	afx_msg LRESULT m_fnSetJoyStickXY(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT m_fnSetJoyStickZ(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCbnSelchangeCboJogMove();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnUp();
	CEdit m_ctrlEditIncZ;
	double m_dEditIncZ;
	afx_msg void OnBnClickedButtonIncZMove();
};
