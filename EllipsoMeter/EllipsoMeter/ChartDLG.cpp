// ChartDLG.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EllipsoMeter.h"
#include "ChartDLG.h"
#include "afxdialogex.h"


// CChartDLG 대화 상자입니다.
static int nChartBtn [] = { 
	                      IDC_BTN_MAP2, 
	                      IDC_BTN_2D2,
	                      IDC_BTN_3D2
};

static CString strChartBtn[] = { "MAP 보기",
	                           "2D 차트",
	                           "3D 차트"
};

IMPLEMENT_DYNAMIC(CChartDLG, CDialogEx)

CChartDLG::CChartDLG(CWnd* pParent /*=NULL*/)
	: CDialogEx(CChartDLG::IDD, pParent)
{
	this->Create( IDD_CHART_DLG );
	m_bShow = FALSE;
	m_pTaskHandle       = NULL;
	m_pTask             = NULL;
	m_bKind             = FALSE;
	m_nSwitchNo         = 0;
	m_bSave             = FALSE;

	// 3D view angles
	m_elevationAngle = 30;
	m_rotationAngle = 45;

	// Keep track of mouse drag
	m_isDragging = (DragState)FALSE;
	m_lastMouseX = -1;
	m_lastMouseY = -1;

}

CChartDLG::~CChartDLG()
{
// 	delete m_ChartViewer.getChart();
	delete m_CrossSectionViewerX.getChart();
	delete m_CrossSectionViewerY.getChart();
}

void CChartDLG::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_CHART, m_ChartViewer);
	DDX_Control(pDX, IDC_CrossSectionViewerX, m_CrossSectionViewerX);
	DDX_Control(pDX, IDC_CrossSectionViewerY, m_CrossSectionViewerY);
	DDX_Control(pDX, IDC_BTN_MAP2, m_clMap2);
	DDX_Control(pDX, IDC_BTN_2D2, m_cl2D2);
	DDX_Control(pDX, IDC_BTN_3D2, m_cl3D2);
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FPSPREAD_CONTOUR, m_clContour);
	DDX_Control(pDX, IDC_CBO_LAYER, m_clLayer);
}


BEGIN_MESSAGE_MAP(CChartDLG, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_MAP2, &CChartDLG::OnBnClickedBtnMap2)
	ON_BN_CLICKED(IDC_BTN_2D2, &CChartDLG::OnBnClickedBtn2d2)
	ON_BN_CLICKED(IDC_BTN_3D2, &CChartDLG::OnBnClickedBtn3d2)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_CBN_SELCHANGE(IDC_CBO_LAYER, &CChartDLG::OnCbnSelchangeCboLayer)
	ON_CONTROL(CVN_ViewPortChanged, IDC_CHART, OnViewPortChanged)
	ON_CONTROL(CVN_MouseMoveChart, IDC_CHART, OnMouseMoveChart)
	ON_CONTROL(BN_CLICKED, IDC_CHART, OnMouseUpChart)
END_MESSAGE_MAP()

BOOL CChartDLG::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (( nID   == 2 ) &&
		( nCode == -1 ))
	{
		m_fnSetShowFlag( FALSE );
	}
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CChartDLG::m_fnSetShowFlag( BOOL bShow )
{
	m_bShow = bShow;
	if ( m_bShow )
	{
		this->ShowWindow( SW_SHOW );
	}
	else
	{
		this->ShowWindow( SW_HIDE );
	}
}

BOOL CChartDLG::PreTranslateMessage(MSG* pMsg) 
{
	// ENTER KEY or ESC KEY CANCEL
	if (( pMsg->wParam == 13 ) || 
		( pMsg->wParam == 27 ))
	{
		return FALSE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CChartDLG::m_fnSetChangeBtn( int nSwitch ) 
{
	m_nSwitchNo = nSwitch;
	for( int i = 0; i < 3; i++ )
	{
		if ( nSwitch == i )
		{
			m_fnSetBtnOperation( nChartBtn[i],L_BIT_BTN_ON,L_BIT_CHAR_ON,strChartBtn[i]);
		}
		else
		{
			m_fnSetBtnOperation( nChartBtn[i],L_BIT_BTN_OFF,L_BIT_CHAR_OFF,strChartBtn[i]);
		}
	}
}

//------------------------------------------------------------
// 각 버튼의 색상과 글자색을 변경한다.
//------------------------------------------------------------
void CChartDLG::m_fnSetBtnOperation( int nID, 
										   int nBtnColor, 
										   int nTextColor,
										   CString strMsg )
{
	if ( this->m_hWnd == NULL ) return;

	CColorBtn* pColorBtn = NULL;
	CRect rect;
	pColorBtn = (CColorBtn*) GetDlgItem( nID );
	if ( pColorBtn == NULL ) return;
	pColorBtn->m_fnSetParam( nBtnColor, nTextColor );
	GetDlgItem( nID )->GetWindowRect(rect);
	if ( !strMsg.IsEmpty() )
	{
		GetDlgItem( nID )->SetWindowText( strMsg );
	}
	ScreenToClient( rect );
	InvalidateRect( rect, FALSE );
}

//---------------------------------------------------------------------------------------------------------
// 시작에 필요한 작업을 한다.
//---------------------------------------------------------------------------------------------------------
void CChartDLG::m_fnSetInit() 
{
	this->MoveWindow( 1090, 100, 810, 860, TRUE );
	m_pTaskHandle = new CTaskHandle( this, SHARED_NAME, 
											1, 
											sizeof( SHEET_RESISTANCE_Format ) + 1,
											0,
											FALSE );

	char szBuffer[4096];
	CChart_setLicenseCode("DEVP-2NKS-5YMB-XJBJ-21EB-87DA", szBuffer);

	m_pTask					= ( SHEET_RESISTANCE_Format* )m_pTaskHandle->m_pFilePointer( );

	m_clLayer.SetCurSel(0);


	// To handle mouse drags, it is necessary to enable mouse capture. By default, MFC does not
	// enable mouse capture. In ChartDirector, we can use MouseUsageDefaultCapture to enable it.
	m_ChartViewer.setMouseUsage(-1);

	// Update the viewport to display the chart
	m_ChartViewer.updateViewPort(true, false);


	m_fnSetChart();
}

void CChartDLG::m_fnSetChart() 
{
	if ( theApp.m_fnGetViewPointer()->m_nPage == SW_AUTO )
	{
		if ( m_bKind )
		{
			m_clContour.ShowWindow( FALSE );
			memcpy( &m_pTask->stData3D, &m_pTask->stData, sizeof(t_RESULT_INFO));
			scattersurface();
		}
		else
		{
			m_clContour.ShowWindow( TRUE );
			memcpy( &m_pTask->stData2D, &m_pTask->stData, sizeof(t_RESULT_INFO));
			scattercontour();
		}
	}
	if ( theApp.m_fnGetViewPointer()->m_nPage == SW_REVIEW )
	{
		if ( m_bKind )
		{
			m_clContour.ShowWindow( FALSE );
			memcpy( &m_pTask->stData3D, &m_pTask->stReviewData, sizeof(t_RESULT_INFO));
			scattersurface();
		}
		else
		{
			m_clContour.ShowWindow( TRUE );
			memcpy( &m_pTask->stData2D, &m_pTask->stReviewData, sizeof(t_RESULT_INFO));
			scattercontour();
		}
	}
}

void CChartDLG::m_fnSetUpdateData() 
{
	CString strWork("");

	if ( theApp.m_fnGetViewPointer()->m_nPage == SW_AUTO )
	{
		m_clContour.SetRow( 1 );
		strWork.Format( "%7.3f", m_pTask->stData.m_dMin[theApp.m_fnGetViewPointer()->m_pAuto->m_nSelLayer][theApp.m_fnGetViewPointer()->m_pAuto->m_nMeasureItem] );
		m_clContour.SetCol( 3 );
		m_clContour.SetText( strWork );

		strWork.Format( "%7.3f", m_pTask->stData.m_dMax[theApp.m_fnGetViewPointer()->m_pAuto->m_nSelLayer][theApp.m_fnGetViewPointer()->m_pAuto->m_nMeasureItem]);;
		m_clContour.SetCol( 5 );
		m_clContour.SetText( strWork );

		strWork.Format( "%7.3f", (m_pTask->stData.m_dMax_Min[theApp.m_fnGetViewPointer()->m_pAuto->m_nSelLayer][theApp.m_fnGetViewPointer()->m_pAuto->m_nMeasureItem]) / 10. );
		m_clContour.SetCol( 7 );
		m_clContour.SetText( strWork );
	}
	if ( theApp.m_fnGetViewPointer()->m_nPage == SW_REVIEW )
	{
		m_clContour.SetRow( 1 );
		strWork.Format( "%7.3f", m_pTask->stReviewData.m_dContourMin[theApp.m_fnGetViewPointer()->m_pReview->m_nSelLayer][theApp.m_fnGetViewPointer()->m_pReview->m_nMeasureItem] );
		m_clContour.SetCol( 3 );
		m_clContour.SetText( strWork );

		strWork.Format( "%7.3f", m_pTask->stReviewData.m_dContourMax[theApp.m_fnGetViewPointer()->m_pReview->m_nSelLayer][theApp.m_fnGetViewPointer()->m_pReview->m_nMeasureItem] );
		m_clContour.SetCol( 5 );
		m_clContour.SetText( strWork );

		strWork.Format( "%7.3f", m_pTask->stReviewData.m_dContourStep[theApp.m_fnGetViewPointer()->m_pReview->m_nSelLayer][theApp.m_fnGetViewPointer()->m_pReview->m_nMeasureItem] );
		m_clContour.SetCol( 7 );
		m_clContour.SetText( strWork );
	}

	m_elevationAngle = 50;
	m_rotationAngle = -25;

	// Initializse the crosshair position to the center of the chart
	m_crossHairX = 80 + 550 / 2;
	m_crossHairY = 40 + 550 / 2;

	m_fnSetChart();
}

//---------------------------------------------------------------------------------------------------------
// 종료에 필요한 작업을 한다.
//---------------------------------------------------------------------------------------------------------
void CChartDLG::m_fnSetEnd( ) 
{
	if ( m_pTaskHandle != NULL )
	{
		m_pTaskHandle->m_fnSetEnd();
		delete m_pTaskHandle;
		m_pTaskHandle = NULL;
	}
}

//
// Draw the X cross section
//
void CChartDLG::drawCrossSectionX(CChartViewer* viewer)
{
	// Get data of the vertical cross section data at the given x coordinate
	XYChart* mainChart = (XYChart*)m_ChartViewer.getChart();
	PlotArea* p = mainChart->getPlotArea();
	DoubleArray z = m_ContourLayer->getCrossSection(m_crossHairX, p->getBottomY(), m_crossHairX,
		p->getTopY());

	// Create XYChat of the same height as the main chart. Align the plot area with that of the 
	// main chart.
	XYChart* c = new XYChart(100, mainChart->getHeight());
	c->setPlotArea(10, p->getTopY(), c->getWidth() - 22, p->getHeight(), -1, -1, -1,
		c->dashLineColor((int)0xaf000000, Chart::DotLine), -1);

	// The vertical chart will have the x-axis vertical and y-axis horizontal. Synchroinze the
	// vertical axis (x-axis) with the y-axis of the main chart. Synchroinze the horizontal 
	// axis (y-axis) with the color axis.
	c->swapXY();
	c->xAxis()->syncAxis(mainChart->yAxis());
	c->yAxis()->syncScale(m_ContourLayer->colorAxis());

	// The vertical axis (x-axis) does not need labels as it is aligned with the main chart y-axis.
	c->xAxis()->setLabelStyle("normal", 8, Chart::Transparent);

	// Rotate the horizontal axis (y-axis) labels by 270 degrees
	c->yAxis()->setLabelStyle("normal", 8, Chart::TextColor, 270);

	// Add an area layer using the cross section data and the color scale of the color axis.
	int scaleColor = c->yScaleColor(m_ContourLayer->colorAxis()->getColorScale());
	AreaLayer* layer = c->addAreaLayer(z, scaleColor);
	layer->setBorderColor(Chart::SameAsMainColor);
	layer->setXData(mainChart->getYValue(p->getBottomY()), mainChart->getYValue(p->getTopY()));

// 	c->makeChart("D:\\profileX.png");

	// Display the chart
	delete viewer->getChart();
	viewer->setChart(c);
}

//
// Draw the Y cross section
//
void CChartDLG::drawCrossSectionY(CChartViewer* viewer)
{
	// Get the vertical horizontal section data at the given y coordinate
	XYChart* mainChart = (XYChart*)m_ChartViewer.getChart();
	PlotArea* p = mainChart->getPlotArea();
	DoubleArray z = m_ContourLayer->getCrossSection(p->getLeftX(), m_crossHairY, p->getRightX(),
		m_crossHairY);

	// Create XYChat of the same width as the main chart. Align the plot area with that of the 
	// main chart.
	XYChart* c = new XYChart(mainChart->getWidth(), 100);
	c->setPlotArea(p->getLeftX(), 10, p->getWidth(), c->getHeight() - 22, -1, -1, -1,
		c->dashLineColor((int)0xaf000000, Chart::DotLine), -1);

	// Synchroinze the x-axis with the x-axis of the main chart. Synchroinze the y-axis with the
	// color axis.
	c->xAxis()->syncAxis(mainChart->xAxis());
	c->yAxis()->syncScale(m_ContourLayer->colorAxis());

	// The x-axis does not need labels as it is aligned with the main chart x-axis.
	c->xAxis()->setLabelStyle("normal", 8, Chart::Transparent);

	// Add an area layer using the cross section data and the color scale of the color axis.
	int scaleColor = c->yScaleColor(m_ContourLayer->colorAxis()->getColorScale());
	AreaLayer* layer = c->addAreaLayer(z, scaleColor);
	layer->setBorderColor(Chart::SameAsMainColor);
	layer->setXData(mainChart->getXValue(p->getLeftX()), mainChart->getXValue(p->getRightX()));

	// Display the chart
	delete viewer->getChart();
	viewer->setChart(c);

}


void CChartDLG::scattercontour()
{
	m_CrossSectionViewerX.ShowWindow(TRUE);
	m_CrossSectionViewerY.ShowWindow(TRUE);

	double dX = 0.0;
	double dY = 0.0;
	BOOL   bOK = FALSE;
	if (theApp.m_fnGetViewPointer()->m_nPage == SW_AUTO)
	{
		for (int i = 0; i < m_pTask->stData.m_nDataCnt; i++)
		{
			if (i == 0)
			{
				dX = m_pTask->stData.m_dDataX[i];
				dY = m_pTask->stData.m_dDataY[i];
			}
			else
			{
				if ((dX != m_pTask->stData.m_dDataX[i]) &&
					(dY != m_pTask->stData.m_dDataY[i]))
				{
					bOK = TRUE;
				}
			}
		}
	}
	if (theApp.m_fnGetViewPointer()->m_nPage == SW_REVIEW)
	{
		for (int i = 0; i < m_pTask->stReviewData.m_nDataCnt; i++)
		{
			if (i == 0)
			{
				dX = m_pTask->stReviewData.m_dDataX[i];
				dY = m_pTask->stReviewData.m_dDataY[i];
			}
			else
			{
				if ((dX != m_pTask->stReviewData.m_dDataX[i]) &&
					(dY != m_pTask->stReviewData.m_dDataY[i]))
				{
					bOK = TRUE;
				}
			}
		}
	}

	// The (x, y, z) coordinates of the scattered data
	// Create a XYChart object of size 450 x 540 pixels
	XYChart *c = new XYChart(680, 640);
	if (c == NULL) return;


	// Add a title to the chart using 15 points Arial Italic font
	c->addTitle("      EllipsoMeter Data Result", "ariali.ttf", 15);

	// Set the plotarea at (65, 40) and of size 360 x 360 pixels. Use
	// semi-transparent black (c0000000) for both horizontal and vertical grid lines
	c->setPlotArea(80, 40, 550, 550, -1, -1, -1, 0xc0000000, -1);

	// Put the y-axis on the right side of the chart
	c->setYAxisOnRight();

	// Set x-axis and y-axis title using 12 points Arial Bold Italic font
	c->xAxis()->setTitle("X-Axis", "arialbi.ttf", 10);
	c->yAxis()->setTitle("Y-Axis", "arialbi.ttf", 10);

	// Set x-axis and y-axis labels to use Arial Bold font
	c->xAxis()->setLabelStyle("arialbd.ttf");
	c->yAxis()->setLabelStyle("arialbd.ttf");

	// When x-axis and y-axis color to transparent
	c->xAxis()->setColors(Chart::Transparent);
	c->yAxis()->setColors(Chart::Transparent);

	// When auto-scaling, use tick spacing of 40 pixels as a guideline
	c->yAxis()->setTickDensity(20);
	c->xAxis()->setTickDensity(20);

#if Y_AXIS_REVERSE
	c->yAxis()->setReverse(TRUE);
#endif
#if X_AXIS_REVERSE
	c->xAxis()->setReverse(TRUE);
#endif
// 	if ((!bOK))
// 		//||(m_pTask->stData2D.m_nDataCnt < 3))
// 	{
// 		c->makeChart();
// 
// 		m_ChartViewer.setChart(c);
// 		delete c;
// 		c = NULL;
//  		return;
// 	}


	// Add a scatter layer to the chart to show the position of the data points
	DoubleArray dArrDataX = DoubleArray();
	DoubleArray dArrDataY = DoubleArray();
	dArrDataX = DoubleArray(m_pTask->stData2D.m_dDataX, m_pTask->stData2D.m_nDataCnt);
	dArrDataY = DoubleArray(m_pTask->stData2D.m_dDataY, m_pTask->stData2D.m_nDataCnt);
	c->addScatterLayer(dArrDataX, dArrDataY, "", Chart::Cross2Shape(0.2), 7, 0x000000);


	int nLayer;
	int nMeasureItem;
	if (theApp.m_fnGetViewPointer()->m_nPage == SW_AUTO)
	{
		nLayer = theApp.m_fnGetViewPointer()->m_pAuto->m_nSelLayer;
		nMeasureItem = theApp.m_fnGetViewPointer()->m_pAuto->m_nMeasureItem;
	}
	if (theApp.m_fnGetViewPointer()->m_nPage == SW_REVIEW)
	{
		nLayer = theApp.m_fnGetViewPointer()->m_pReview->m_nSelLayer;
		nMeasureItem = theApp.m_fnGetViewPointer()->m_pReview->m_nMeasureItem;
	}

	if (nLayer == -1)
	{
		nLayer = 0;
		m_clLayer.SetCurSel(0);
	}

	double dZData[MAX_DATA + 1];
	memset(dZData, 0x00, sizeof(dZData));
	for (int i = 0; i < m_pTask->stData2D.m_nDataCnt; i++)
	{
		dZData[i] = m_pTask->stData2D.m_dDataZ[i][nLayer][nMeasureItem];
	}

// 	if ((bOK == FALSE) )
// 	{
// 		if (theApp.m_fnGetViewPointer()->m_pModel != nullptr && theApp.m_fnGetViewPointer()->m_pAuto != nullptr)
// 		{
// 			double dMM = 0.0;
// 			int arrWaferInch[4] = { 4,6,8,12 };
// 			theApp.m_fnGetViewPointer()->m_pModel->m_fnGetInchToMM(arrWaferInch[theApp.m_fnGetViewPointer()->m_pAuto->m_clWaperSize.GetCurSel()], dMM, 0);
// 			for (int i = 0; i < 30; i++)
// 			{
// 				m_pTask->stData2D.m_dDataX[m_pTask->stData2D.m_nDataCnt] = dMM*sin((12.0*i)*PI / 180.0);
// 				m_pTask->stData2D.m_dDataY[m_pTask->stData2D.m_nDataCnt] = dMM*cos((12.0*i)*PI / 180.0);
// 				dZData[m_pTask->stData2D.m_nDataCnt] = m_pTask->stData2D.m_dAvg[nLayer][nMeasureItem];
// 				m_pTask->stData2D.m_nDataCnt++;
// 			}
// 		}
// 	}

	m_ContourLayer = c->addContourLayer(DoubleArray(m_pTask->stData2D.m_dDataX, m_pTask->stData2D.m_nDataCnt),
		DoubleArray(m_pTask->stData2D.m_dDataY, m_pTask->stData2D.m_nDataCnt),
		DoubleArray(dZData, m_pTask->stData2D.m_nDataCnt));

	// Move the grid lines in front of the contour layer
	c->getPlotArea()->moveGridBefore(m_ContourLayer);

	// Add a vertical color axis (the legend) at x = 0 at the same y-position as the plot area.
	ColorAxis *cAxis = m_ContourLayer->setColorAxis(0, Chart::Top+40, Chart::TopLeft, 550-40, Chart::Right);
// 	ColorAxis* cAxis = m_ContourLayer->setColorAxis(0, p->getTopY(), Chart::TopLeft,	p->getHeight(), Chart::Right);

	// Use continuous gradient coloring (as opposed to step colors)
	cAxis->setColorGradient(true);

// 	cAxis->setBoundingBox(Chart::Transparent, Chart::LineColor);
	cAxis->setTitle("EllipsoMeter Color Level", "arialbi.ttf", 10);
	cAxis->setLabelStyle("arialbd.ttf");
	cAxis->setLinearScale(m_pTask->stData2D.m_dContourMin[nLayer][nMeasureItem],
								m_pTask->stData2D.m_dContourMax[nLayer][nMeasureItem],
								m_pTask->stData2D.m_dContourStep[nLayer][nMeasureItem]);

	// Output the chart
// 	c->makeChart();

	m_ChartViewer.setChart(c);

	// Tooltip for the contour chart
// 	m_ChartViewer.setImageMap(c->getHTMLImageMap("", "",
// 		"title='<*cdml*>X: {x|3}<*br*>Y: {y|3}<*br*>Z: {z|3}'"));

	drawCrossSectionX(&m_CrossSectionViewerX);
	drawCrossSectionY(&m_CrossSectionViewerY);
	drawCrossHair(&m_ChartViewer);

	delete c;
	c = NULL;

	m_bSave = FALSE;
}

void CChartDLG::scattersurface()
{
	m_CrossSectionViewerX.ShowWindow(FALSE);
	m_CrossSectionViewerY.ShowWindow(FALSE);

	double dX = 0.0;
	double dY = 0.0;
	BOOL   bOK = FALSE;
	if (theApp.m_fnGetViewPointer()->m_nPage == SW_AUTO)
	{
		for (int i = 0; i < m_pTask->stData.m_nDataCnt; i++)
		{
			if (i == 0)
			{
				dX = m_pTask->stData.m_dDataX[i];
				dY = m_pTask->stData.m_dDataY[i];
			}
			else
			{
				if ((dX != m_pTask->stData.m_dDataX[i]) &&
					(dY != m_pTask->stData.m_dDataY[i]))
				{
					bOK = TRUE;
				}
			}
		}
	}
	if (theApp.m_fnGetViewPointer()->m_nPage == SW_REVIEW)
	{
		for (int i = 0; i < m_pTask->stReviewData.m_nDataCnt; i++)
		{
			if (i == 0)
			{
				dX = m_pTask->stReviewData.m_dDataX[i];
				dY = m_pTask->stReviewData.m_dDataY[i];
			}
			else
			{
				if ((dX != m_pTask->stReviewData.m_dDataX[i]) &&
					(dY != m_pTask->stReviewData.m_dDataY[i]))
				{
					bOK = TRUE;
				}
			}
		}
	}

	// Create a SurfaceChart object of size 680 x 550 pixels. Set background to
	// brushed silver and border to grey (888888). Set the top-left and bottom-right
	// corners to rounded corners with 20 pixels radius.
	SurfaceChart *c = new SurfaceChart(780, 750);// , Chart::brushedSilverColor(), 0x888888);

	if (c == NULL) 
		return;

	// Add a title to the chart using 20 points Times New Roman Italic font. Set
	// top/bottom margin to 8 pixels.
	TextBox *title = c->addTitle("EllipsoMeter Data Points", "timesi.ttf", 20);
	title->setMargin(0, 0, 8, 8);

	// Add a 2 pixel wide black (000000) separator line under the title
	c->addLine(10, title->getHeight(), c->getWidth() - 10, title->getHeight(), 0x000000, 2);

	// Set the center of the plot region at (290, 235), and set width x depth x
	// height to 360 x 360 x 180 pixels
	c->setPlotRegion(380, 300, 560, 560, 150);

#if Y_AXIS_REVERSE
	c->yAxis()->setReverse(TRUE);
#endif
#if X_AXIS_REVERSE
	c->xAxis()->setReverse(TRUE);
#endif

	// Set the elevation and rotation angles to 45 and -45 degrees
// 	c->setViewAngle(50, -25);
	c->setViewAngle(m_elevationAngle, m_rotationAngle);

	// Set the perspective level to 30
	c->setPerspective(50);

	if ((!bOK))
		//||(m_pTask->stData3D.m_nDataCnt < 3))
	{
// 		c->makeChart();

		m_ChartViewer.setChart(c);
		delete c;
		c = NULL;
		return;
	}

	int nLayer;
	int nMeasureItem;
	if (theApp.m_fnGetViewPointer()->m_nPage == SW_AUTO)
	{
		nLayer = theApp.m_fnGetViewPointer()->m_pAuto->m_nSelLayer;
		nMeasureItem = theApp.m_fnGetViewPointer()->m_pAuto->m_nMeasureItem;
	}
	if (theApp.m_fnGetViewPointer()->m_nPage == SW_REVIEW)
	{
		nLayer = theApp.m_fnGetViewPointer()->m_pReview->m_nSelLayer;
		nMeasureItem = theApp.m_fnGetViewPointer()->m_pReview->m_nMeasureItem;
	}
	
	if (nLayer == -1)
	{
		nLayer = 0;
		m_clLayer.SetCurSel(0);
	}
	double dZData[MAX_DATA + 1];
	memset(dZData, 0x00, sizeof(dZData));

	for (int i = 0; i < m_pTask->stData3D.m_nDataCnt; i++)
	{
		dZData[i] = m_pTask->stData3D.m_dDataZ[i][nLayer][nMeasureItem];
	}


// 	if (theApp.m_fnGetViewPointer()->m_pModel != nullptr && theApp.m_fnGetViewPointer()->m_pAuto != nullptr)
// 	{
// 		double dMM = 0.0;
// 		int arrWaferInch[4] = { 4,6,8,12 };
// 		theApp.m_fnGetViewPointer()->m_pModel->m_fnGetInchToMM(arrWaferInch[theApp.m_fnGetViewPointer()->m_pAuto->m_clWaperSize.GetCurSel()], dMM, 0);
// 		for (int i = 0; i < 30; i++)
// 		{
// 			m_pTask->stData3D.m_dDataX[m_pTask->stData3D.m_nDataCnt] = dMM*sin((12.0*i)*PI / 180.0);
// 			m_pTask->stData3D.m_dDataY[m_pTask->stData3D.m_nDataCnt] = dMM*cos((12.0*i)*PI / 180.0);
// 			dZData[m_pTask->stData3D.m_nDataCnt] = m_pTask->stData3D.m_dAvg[nLayer][nMeasureItem];
// 			m_pTask->stData3D.m_nDataCnt++;
// 		}
// 	}
	c->setData(DoubleArray(m_pTask->stData3D.m_dDataX, m_pTask->stData3D.m_nDataCnt),
		DoubleArray(m_pTask->stData3D.m_dDataY, m_pTask->stData3D.m_nDataCnt),
		DoubleArray(dZData, m_pTask->stData3D.m_nDataCnt));


	// Add a color axis (the legend) in which the top right corner is anchored at
	// (660, 80). Set the length to 200 pixels and the labels on the right side.
	ColorAxis *cAxis = c->setColorAxis(760, Chart::Top + 60, Chart::TopRight, 550, Chart::Right);

	// Set the color axis title with 12 points Arial Bold font
	cAxis->setTitle("Z Resistance", "arialbd.ttf", 12);
	cAxis->setColorGradient(true);

	cAxis->setBoundingBox(0xeeeeee, 0x888888);
	cAxis->setRoundedCorners(10, 0, 10, 0);

	c->setRoundedFrame(0xffffff, 20, 0, 20, 0);
	c->setSurfaceAxisGrid(0xcc000000);
	c->setContourColor(0x80ffffff);
	c->setWallColor(0x000000);
	c->setWallGrid(0xffffff, 0xffffff, 0xffffff, 0x888888, 0x888888, 0x888888);
	c->setWallThickness(0, 0, 0);
	c->setWallVisibility(true, false, false);

	// Set the x, y and z axis titles using 12 points Arial Bold font
	c->xAxis()->setTitle("X Axis(mm)", "arialbd.ttf", 12);
	c->yAxis()->setTitle("Y Axis(mm)", "arialbd.ttf", 12);

	// Output the chart
// 	c->makeChart();

	m_ChartViewer.setChart(c);


	//include tool tip for the chart
	m_ChartViewer.setImageMap(c->getHTMLImageMap("", "",
		"title='<*cdml*>X: {x|3}<*br*>Y: {y|3}<*br*>Z: {z|3}'"));

	delete c;
	c = NULL;

	m_bSave = FALSE;

}

void CChartDLG::m_fnSetContourChartToImage( BOOL bKind, CString strImageInfo)
{
	if ( m_bKind != bKind )
	{
		m_bKind = bKind;
		m_fnSetChart();
	}

	CDC *pDC1 = m_ChartViewer.GetDC();
	HDC hDC = pDC1->m_hDC;

	RECT rc;
	m_ChartViewer.GetClientRect(&rc);

	HDC hMemDc = CreateCompatibleDC(hDC);
	HBITMAP hBitmap = CreateCompatibleBitmap(hDC, rc.right, rc.bottom);
	HBITMAP HBmpOld = (HBITMAP)SelectObject(hMemDc, hBitmap);
	BitBlt(hMemDc, 0, 0, rc.right, rc.bottom, hDC, 0, 0, SRCCOPY);
	SelectObject(hMemDc, HBmpOld);

	BITMAPINFOHEADER bmih;
	ZeroMemory(&bmih, sizeof(BITMAPINFOHEADER));
	bmih.biSize = sizeof(BITMAPINFOHEADER);
	bmih.biWidth = rc.right;
	bmih.biHeight = rc.bottom;
	bmih.biPlanes = 1;
	bmih.biBitCount = 24;
	bmih.biCompression = BI_RGB;

	GetDIBits(hDC, hBitmap, 0, rc.bottom, NULL, (LPBITMAPINFO)&bmih, DIB_RGB_COLORS);
	LPBYTE lpBits = new BYTE[bmih.biSizeImage];
	GetDIBits(hDC, hBitmap, 0, rc.bottom, lpBits, (LPBITMAPINFO)&bmih, DIB_RGB_COLORS);
	ReleaseDC(pDC1);
	DeleteObject(hBitmap);

	BITMAPFILEHEADER bmfh;
	bmfh.bfType = 'MB';
	bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPFILEHEADER) + bmih.biSizeImage;
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;

	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	SYSTEMTIME time;
	GetLocalTime(&time);
	CString Name;

	Name.Format(_T("c:\\"));

	DWORD dwWritten;
	HANDLE hFile = CreateFile( strImageInfo , GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	WriteFile(hFile, &bmfh, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	WriteFile(hFile, &bmih, sizeof(BITMAPINFOHEADER), &dwWritten, NULL);
	WriteFile(hFile, lpBits, bmih.biSizeImage, &dwWritten, NULL);

	CloseHandle(hFile);
	delete [] lpBits;
}


void CChartDLG::OnBnClickedBtnMap2()
{
	theApp.m_fnGetViewPointer()->m_fnSetChartView( SW_MAP );
}

void CChartDLG::OnBnClickedBtn2d2()
{
	theApp.m_fnGetViewPointer()->m_fnSetChartView( SW_2D );
}


void CChartDLG::OnBnClickedBtn3d2()
{
	theApp.m_fnGetViewPointer()->m_fnSetChartView( SW_3D );
}
BEGIN_EVENTSINK_MAP(CChartDLG, CDialogEx)
	ON_EVENT(CChartDLG, IDC_FPSPREAD_CONTOUR, 5, CChartDLG::ClickFpspreadContour, VTS_I4 VTS_I4)
END_EVENTSINK_MAP()


void CChartDLG::ClickFpspreadContour(long Col, long Row)
{
	if ( Col != 8 ) return;

	CString strWork("");

	if ( theApp.m_fnGetViewPointer()->m_nPage == SW_AUTO )
	{
		m_clContour.SetRow( 1 );

		m_clContour.SetCol(3);
		strWork = m_clContour.GetText( );
		m_pTask->stData.m_dContourMin[theApp.m_fnGetViewPointer()->m_pAuto->m_nSelLayer][theApp.m_fnGetViewPointer()->m_pAuto->m_nMeasureItem] = atof(strWork);

		m_clContour.SetCol(5);
		strWork = m_clContour.GetText( );
		m_pTask->stData.m_dContourMax[theApp.m_fnGetViewPointer()->m_pAuto->m_nSelLayer][theApp.m_fnGetViewPointer()->m_pAuto->m_nMeasureItem] = atof(strWork);

		m_clContour.SetCol(7);
		strWork = m_clContour.GetText( );
		m_pTask->stData.m_dContourStep[theApp.m_fnGetViewPointer()->m_pAuto->m_nSelLayer][theApp.m_fnGetViewPointer()->m_pAuto->m_nMeasureItem] = atof(strWork);
	}
	if ( theApp.m_fnGetViewPointer()->m_nPage == SW_REVIEW )
	{
		m_clContour.SetRow( 1 );

		m_clContour.SetCol(3);
		strWork = m_clContour.GetText( );
		m_pTask->stReviewData.m_dContourMin[theApp.m_fnGetViewPointer()->m_pReview->m_nSelLayer][theApp.m_fnGetViewPointer()->m_pReview->m_nMeasureItem] = atof(strWork);

		m_clContour.SetCol(5);
		strWork = m_clContour.GetText( );
		m_pTask->stReviewData.m_dContourMax[theApp.m_fnGetViewPointer()->m_pReview->m_nSelLayer][theApp.m_fnGetViewPointer()->m_pReview->m_nMeasureItem] = atof(strWork);

		m_clContour.SetCol(7);
		strWork = m_clContour.GetText( );
		m_pTask->stReviewData.m_dContourStep[theApp.m_fnGetViewPointer()->m_pReview->m_nSelLayer][theApp.m_fnGetViewPointer()->m_pReview->m_nMeasureItem] = atof(strWork);
	}
	m_fnSetChart();
}

BOOL m_bLClickFlag = FALSE;
BOOL m_bRClickFlag = FALSE;
void CChartDLG::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_bRClickFlag)
		return;
	m_bLClickFlag = TRUE;

	CDialogEx::OnLButtonDown(nFlags, point);


}


void CChartDLG::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bLClickFlag = FALSE;

	CDialogEx::OnLButtonUp(nFlags, point);
}


void CChartDLG::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bLClickFlag)
	{
	}

	CDialogEx::OnMouseMove(nFlags, point);
}


void CChartDLG::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (m_bLClickFlag)
		return;
	m_bRClickFlag = TRUE;

	CDialogEx::OnRButtonDown(nFlags, point);
}


void CChartDLG::OnRButtonUp(UINT nFlags, CPoint point)
{
	m_bRClickFlag = FALSE;

	CDialogEx::OnRButtonUp(nFlags, point);
}


void CChartDLG::OnCbnSelchangeCboLayer()
{
	if (theApp.m_fnGetViewPointer()->m_nPage == SW_AUTO)
	{
		theApp.m_fnGetViewPointer()->m_pAuto->m_nSelLayer = m_clLayer.GetCurSel();
		theApp.m_fnGetViewPointer()->m_pAuto->m_clLayer.SetCurSel(theApp.m_fnGetViewPointer()->m_pAuto->m_nSelLayer);
		theApp.m_fnGetViewPointer()->m_pAuto->m_fnSetView();
	}
	if (theApp.m_fnGetViewPointer()->m_nPage == SW_REVIEW)
	{
		theApp.m_fnGetViewPointer()->m_pReview->m_nSelLayer = m_clLayer.GetCurSel();
		theApp.m_fnGetViewPointer()->m_pReview->m_clLayer.SetCurSel(theApp.m_fnGetViewPointer()->m_pReview->m_nSelLayer);
		theApp.m_fnGetViewPointer()->m_pReview->m_fnSetView();
	}
}

//
// View port changed event
//
void CChartDLG::OnViewPortChanged()
{
	// Update the chart if necessary
	if (m_ChartViewer.needUpdateChart())
		m_fnSetChart();
}

//
// Draw the chart and display it in the given viewer
//
// void CChartDLG::drawChart(CChartViewer *viewer)
// {
// 	// The x and y coordinates of the grid
// 	double dataX[] = { -10, -9, -8, -7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
// 	const int dataXLength = (int)(sizeof(dataX) / sizeof(*dataX));
// 
// 	double dataY[] = { -10, -9, -8, -7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
// 	const int dataYLength = (int)(sizeof(dataY) / sizeof(*dataY));
// 
// 	// The values at the grid points. In this example, we will compute the values using the
// 	// formula z = x * sin(y) + y * sin(x).
// 	double dataZ[dataXLength * dataYLength];
// 	for (int yIndex = 0; yIndex < dataYLength; ++yIndex)
// 	{
// 		double y = dataY[yIndex];
// 		for (int xIndex = 0; xIndex < dataXLength; ++xIndex)
// 		{
// 			double x = dataX[xIndex];
// 			dataZ[yIndex * dataXLength + xIndex] = x * sin(y) + y * sin(x);
// 		}
// 	}
// 
// 	// Create a SurfaceChart object of size 720 x 600 pixels
// 	SurfaceChart *c = new SurfaceChart(720, 600);
// 
// 	// Set the center of the plot region at (330, 290), and set width x depth x height to
// 	// 360 x 360 x 270 pixels
// 	c->setPlotRegion(330, 290, 360, 360, 270);
// 
// 	// Set the data to use to plot the chart
// 	c->setData(DoubleArray(dataX, dataXLength), DoubleArray(dataY, dataYLength),
// 		DoubleArray(dataZ, dataXLength * dataYLength));
// 
// 	// Spline interpolate data to a 80 x 80 grid for a smooth surface
// 	c->setInterpolation(80, 80);
// 
// 	// Set the view angles
// 	c->setViewAngle(m_elevationAngle, m_rotationAngle);
// 
// 	// Check if draw frame only during rotation
// 	if (m_isDragging && m_DrawFrameOnRotate.GetCheck())
// 		c->setShadingMode(Chart::RectangularFrame);
// 
// 	// Add a color axis (the legend) in which the left center is anchored at (660, 270). Set
// 	// the length to 200 pixels and the labels on the right side.
// 	c->setColorAxis(650, 270, Chart::Left, 200, Chart::Right);
// 
// 	// Set the x, y and z axis titles using 10 points Arial Bold font
// 	c->xAxis()->setTitle("X", "Arial Bold", 15);
// 	c->yAxis()->setTitle("Y", "Arial Bold", 15);
// 
// 	// Set axis label font
// 	c->xAxis()->setLabelStyle("Arial", 10);
// 	c->yAxis()->setLabelStyle("Arial", 10);
// 	c->zAxis()->setLabelStyle("Arial", 10);
// 	c->colorAxis()->setLabelStyle("Arial", 10);
// 
// 	// Output the chart
// 	delete viewer->getChart();
// 	viewer->setChart(c);
// 
// 	//include tool tip for the chart
// 	viewer->setImageMap(c->getHTMLImageMap("", "",
// 		"title='<*cdml*>X: {x|2}<*br*>Y: {y|2}<*br*>Z: {z|2}'"));
// }

//
// Rotate surface chart when mouse is moving over plotarea
//
void CChartDLG::OnMouseMoveChart()
{
	int mouseX = m_ChartViewer.getChartMouseX();
	int mouseY = m_ChartViewer.getChartMouseY();

	// Drag occurs if mouse button is down and the mouse is captured by the m_ChartViewer
	if (((GetKeyState(VK_LBUTTON) & 0x100) != 0))
	{
		if ((m_isDragging == Drag_Vertical) && (m_crossHairX != mouseX - m_dragOffset))
		{
			// Is dragging the vertical crosshair line
			m_crossHairX = m_ChartViewer.getPlotAreaMouseX();
			// 				drawCrossSectionX(&m_CrossSectionViewerX);
			m_ChartViewer.updateViewPort(true, false);
		}

		if ((m_isDragging == Drag_Horizontal) && (m_crossHairY != mouseY - m_dragOffset))
		{
			// Is dragging the horizontal crosshair line
			m_crossHairY = m_ChartViewer.getPlotAreaMouseY();
			// 			drawCrossSectionY(&m_CrossSectionViewerY);
			m_ChartViewer.updateViewPort(true, false);
		}

		if (GetCapture() == &m_ChartViewer)
		{
			if (m_isDragging)
			{
				// The chart is configured to rotate by 90 degrees when the mouse moves from 
				// left to right, which is the plot region width (360 pixels). Similarly, the
				// elevation changes by 90 degrees when the mouse moves from top to buttom,
				// which is the plot region height (270 pixels).
				m_rotationAngle += (m_lastMouseX - mouseX) * 90.0 / 360;
				m_elevationAngle += (mouseY - m_lastMouseY) * 90.0 / 270;
				m_ChartViewer.updateViewPort(true, false);
			}

			// Keep track of the last mouse position
			m_lastMouseX = mouseX;
			m_lastMouseY = mouseY;
			if (abs(m_dragOffset = mouseX - m_crossHairX) <= 8)
				m_isDragging = Drag_Vertical;   // Mouse is a position to drag the vertical line
			else if (abs(m_dragOffset = mouseY - m_crossHairY) <= 8)
				m_isDragging = Drag_Horizontal; // Mouse is a position to drag the horizontal line
		}

	}
	else
	{
		//
		// If mouse is near the crosshair, it can drag it by pressing the mouse button.
		//
		if (abs(m_dragOffset = mouseX - m_crossHairX) <= 8)
			m_isDragging = Drag_Vertical;   // Mouse is a position to drag the vertical line
		else if (abs(m_dragOffset = mouseY - m_crossHairY) <= 8)
			m_isDragging = Drag_Horizontal; // Mouse is a position to drag the horizontal line
		else
			m_isDragging = Drag_None;
	}

}

//
// Stops dragging on mouse up
//
void CChartDLG::OnMouseUpChart()
{
	m_isDragging = (DragState)FALSE;
	m_ChartViewer.updateViewPort(true, false);
}

//
// Draw the crosshair, which is just two straight lines 
//
void CChartDLG::drawCrossHair(CChartViewer* viewer)
{
	// Get the chart to draw the crosshair on.
	XYChart* c = (XYChart*)viewer->getChart();

	// The crosshair will be drawn on the dynamic layer of the chart.
	DrawArea* d = c->initDynamicLayer();

	// Add two lines across the plot area of the chart
	PlotArea* p = c->getPlotArea();
	d->line(m_crossHairX, p->getTopY(), m_crossHairX, p->getBottomY(), 0x000000, 2);
	d->line(p->getLeftX(), m_crossHairY, p->getRightX(), m_crossHairY, 0x000000, 2);

	// Update the display
	viewer->updateDisplay();
}
