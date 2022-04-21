#pragma once
#include "./Chart/chartdir.h"
#include "./chart/ChartViewer.h"
#include "afxwin.h"

class CChartDLG : public CDialogEx
{
	DECLARE_DYNAMIC(CChartDLG)
	void							m_fnSetShowFlag( BOOL bShow );

	void							m_fnSetInit(); 
	void							m_fnSetEnd( void );
	void drawCrossSectionX(CChartViewer* viewer);
	void drawCrossSectionY(CChartViewer* viewer);
	BOOL                            m_bShow;
	void							m_fnSetUpdateData( void ); 
	void							m_fnSetChart( void ); 
	void							m_fnSetDefault( void );

	CTaskHandle*					m_pTaskHandle;
	SHEET_RESISTANCE_Format*		m_pTask;

	CChartViewer	                m_ChartViewer;
	CChartViewer					m_CrossSectionViewerX;
	CChartViewer					m_CrossSectionViewerY;
	void							scattercontour( void );
	void							scattersurface( void );
	BOOL                            m_bKind;
	void							m_fnSetContourChartToImage( BOOL bKind, CString strImageInfo );
	void							m_fnSetChangeBtn( int nSwitch ); 
	int                             m_nSwitchNo;
	void							m_fnSetBtnOperation( int nID, 
														   int nBtnColor, 
														   int nTextColor,
														   CString strMsg );
	BOOL                            m_bSave;

public:
	CChartDLG(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CChartDLG();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CComboBox m_clLayer;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CHART_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CColorBtn m_cl2D2;
	CColorBtn m_cl3D2;
	CColorBtn m_clMap2;
	afx_msg void OnBnClickedBtnMap2();
	afx_msg void OnBnClickedBtn2d2();
	afx_msg void OnBnClickedBtn3d2();
	CFpspread_point m_clContour;
	DECLARE_EVENTSINK_MAP()
	void ClickFpspreadContour(long Col, long Row);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCbnSelchangeCboLayer();
	

	void OnViewPortChanged();
// 	void drawChart(CChartViewer *viewer);
	void OnMouseMoveChart();
	void OnMouseUpChart();

	void drawCrossHair(CChartViewer* viewer);
	// 3D view angles
	double m_elevationAngle;
	double m_rotationAngle;

	// Keep track of mouse drag
	int m_lastMouseX;
	int m_lastMouseY;

	// Keep track of mouse dragging state
	enum DragState { Drag_None, Drag_Vertical, Drag_Horizontal };
	DragState m_isDragging;
// 	bool m_isDragging;
	int m_dragOffset;

	// Crosshair position
	int m_crossHairX;
	int m_crossHairY;

	ContourLayer *m_ContourLayer;
};
