#pragma once
#include "afxwin.h"


// CStageMapViewDLG 대화 상자입니다.

UINT MapDispThread(LPVOID pFuncData);
class CMapViewDLG : public CDialogEx
{
	DECLARE_DYNAMIC(CMapViewDLG)
	BOOL							m_bShow;	
	void							m_fnSetShowFlag( BOOL bShow );

	void							m_fnSetInit( void ); 
	void							m_fnSetEnd( void ); 

	CTaskHandle*					m_pTaskHandle;
	SHEET_RESISTANCE_Format*		m_pTask;
	BOOL                            m_bCamera;
	BOOL                            m_bPreView;
	int                             m_nWaperSize;

	HANDLE							m_hMapDispThread;
	BOOL                            m_bMapDispEnd;

	LRESULT							m_fnSetMapDraw( WPARAM nNo, LPARAM lParam );
	void							m_fnSetMouseCoord( CPoint &point, double dX, double dY );
	void							m_fnSetMouseCoord(CPoint &point, double dX, double dY, double dResol, int offsetX/*=0*/, int offsetY/*=0 */);
	void							m_fnSetBtnOperation(int nID,
														   int nBtnColor, 
														   int nTextColor,
														   CString strMsg );
	void							m_fnSetChangeBtn( int nSwitch );
	void							m_fnSetContourChartToImage(CString strImageInfo);

	int                             m_nSwitchNo;
	CString							m_strImageInfo;
	double                          m_dExclusion;
	BOOL                            m_bView;
	BOOL                            m_bSave;
public:
	CMapViewDLG(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CMapViewDLG();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_MAP_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CColorBtn m_cl2D;
	CColorBtn m_cl3D;
	CColorBtn m_clMap;
	afx_msg void OnBnClickedBtn2d();
	afx_msg void OnBnClickedBtn3d();
	afx_msg void OnBnClickedBtnMap();
// 	CEdit m_ctrlEditOffsetX;
// 	CEdit m_ctrlEditOffsetY;
};
