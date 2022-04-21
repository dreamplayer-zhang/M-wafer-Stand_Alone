#pragma once
#include "afxwin.h"


// CStageMapViewDLG 대화 상자입니다.

UINT StageMapThread(LPVOID pFuncData);
UINT DispThread(LPVOID pFuncData);
class CStageMapViewDLG : public CDialogEx
{
	DECLARE_DYNAMIC(CStageMapViewDLG)
	BOOL							m_bShow;	
	void							m_fnSetShowFlag( BOOL bShow );
	int                             m_nWaperSize;

	void							m_fnSetInit( void ); 
	void							m_fnSetEnd( void ); 

	CTaskHandle*					m_pTaskHandle;
	SHEET_RESISTANCE_Format*		m_pTask;
	BOOL                            m_bCamera;
	BOOL                            m_bPreView;

	HANDLE							m_hDispThread;
	BOOL                            m_bDispEnd;

	HANDLE							m_hStageMapThread;
	BOOL                            m_bStageMapEnd;
	int                             m_nMoveStep;
	BOOL                            m_bMoveStop;
	BOOL                            m_bMouseMove;
	double                          m_dX;
	double                          m_dY;
	double                          m_dExclusion;
	LRESULT							m_fnSetMapDraw( WPARAM nNo, LPARAM lParam );
	LRESULT							m_fnSetMapMove( WPARAM nNo, LPARAM lParam );
	void m_fnGetMouseCoord(CPoint point, double &dX, double &dY, double dResol, int offsetX=0, int offsetY=0 );
	void m_fnGetMouseCoord(CPoint point, double &dX, double &dY);
	void m_fnSetMouseCoord(CPoint &point, double dX, double dY, double dResol, int offsetX = 0, int offsetY = 0);
	void m_fnSetMouseCoord(CPoint &point, double dX, double dY);


public:
	CStageMapViewDLG(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CStageMapViewDLG();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_STAGE_MAP_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnMove();
	afx_msg void OnBnClickedBtnMoveStop();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedChkCamera();
	void m_fnSetRunningCamera(BOOL bRun);

	afx_msg void OnBnClickedChkAlignLightOn();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	BOOL m_bTimerClickMove;
	CButton m_ctrlCheckCamera;
	void m_fnCheckCamera(BOOL bON);

	CDPoint m_dPntAlign[5];
	int m_iPntAlignCnt;
};
