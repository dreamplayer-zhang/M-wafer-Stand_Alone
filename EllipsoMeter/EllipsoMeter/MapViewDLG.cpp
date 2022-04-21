// MapViewDLG.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EllipsoMeter.h"
#include "MapViewDLG.h"
#include "afxdialogex.h"
#include "MilVision.h"


// CStageMapViewDLG 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMapViewDLG, CDialogEx)
static int nMapBtn [] = { 
	                      IDC_BTN_MAP, 
	                      IDC_BTN_2D,
	                      IDC_BTN_3D
};

static CString strMapBtn[] = { "MAP 보기",
	                           "2D 차트",
	                           "3D 차트"
};

CMapViewDLG::CMapViewDLG(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMapViewDLG::IDD, pParent)
{
	this->Create( IDD_MAP_DLG );
	m_bShow				= FALSE;
	m_hMapDispThread	= NULL;
	m_bMapDispEnd		= FALSE;
	m_bPreView          = FALSE;
	m_nSwitchNo         = 0;
	m_bSave             = FALSE;
	m_strImageInfo      = _T("");
	m_nWaperSize        = WAPER_8;
	m_dExclusion        = 0.0;
	m_bView             = FALSE;
	m_bSave             = FALSE;
}

CMapViewDLG::~CMapViewDLG()
{
}

void CMapViewDLG::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_BTN_2D, m_cl2D);
	DDX_Control(pDX, IDC_BTN_3D, m_cl3D);
	DDX_Control(pDX, IDC_BTN_MAP, m_clMap);
	CDialogEx::DoDataExchange(pDX);
// 	DDX_Control(pDX, IDC_EDT_OFFSET_X, m_ctrlEditOffsetX);
// 	DDX_Control(pDX, IDC_EDT_OFFSET_Y, m_ctrlEditOffsetY);
}

void CMapViewDLG::m_fnSetChangeBtn( int nSwitch ) 
{
	m_nSwitchNo = nSwitch;
	for( int i = 0; i < 3; i++ )
	{
		if ( nSwitch == i )
		{
			m_fnSetBtnOperation( nMapBtn[i],L_BIT_BTN_ON,L_BIT_CHAR_ON,strMapBtn[i]);
		}
		else
		{
			m_fnSetBtnOperation( nMapBtn[i],L_BIT_BTN_OFF,L_BIT_CHAR_OFF,strMapBtn[i]);
		}
	}
}

//------------------------------------------------------------
// 각 버튼의 색상과 글자색을 변경한다.
//------------------------------------------------------------
void CMapViewDLG::m_fnSetBtnOperation( int nID, 
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

BEGIN_MESSAGE_MAP(CMapViewDLG, CDialogEx)
	ON_MESSAGE(UM_MAP_DRAW, m_fnSetMapDraw)
	ON_BN_CLICKED(IDC_BTN_2D, &CMapViewDLG::OnBnClickedBtn2d)
	ON_BN_CLICKED(IDC_BTN_3D, &CMapViewDLG::OnBnClickedBtn3d)
	ON_BN_CLICKED(IDC_BTN_MAP, &CMapViewDLG::OnBnClickedBtnMap)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------------------------
// 외부 참조용 Function
//---------------------------------------------------------------------------------------------
BOOL CMapViewDLG::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (( nID   == 2 ) &&
		( nCode == -1 ))
	{
		m_fnSetShowFlag( FALSE );
	}
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMapViewDLG::m_fnSetShowFlag( BOOL bShow )
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

BOOL CMapViewDLG::PreTranslateMessage(MSG* pMsg) 
{
	// ENTER KEY or ESC KEY CANCEL
	if (( pMsg->wParam == 13 ) || 
		( pMsg->wParam == 27 ))
	{
		return FALSE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

//---------------------------------------------------------------------------------------------------------
// 시작에 필요한 작업을 한다.
//---------------------------------------------------------------------------------------------------------
void CMapViewDLG::m_fnSetInit( ) 
{
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);
	CString s("");
	s.Format( "Map View 메인창" );
	this->SetWindowText(s);
	
	CRect rect;
	this->MoveWindow( 1090, 100, 810, 860, TRUE );

	GetDlgItem( IDC_MAP )->MoveWindow( 20, 80, 750, 750, TRUE );

	m_pTaskHandle = new CTaskHandle( this, SHARED_NAME, 
									 1, 
									 sizeof( SHEET_RESISTANCE_Format ) + 1,
									 0,
									 FALSE );


	m_pTask					= ( SHEET_RESISTANCE_Format* )m_pTaskHandle->m_pFilePointer( );
	m_bView                 = FALSE;
	m_bSave                 = FALSE;

	m_fnSetChangeBtn( SW_MAP );

	GetDlgItem( IDC_EDT_CURR_X )->SetWindowText( "0.000" );
	GetDlgItem(IDC_EDT_CURR_Y)->SetWindowText("0.000");
	GetDlgItem(IDC_EDT_CURR_Z)->SetWindowText("0.000");
	unsigned long	ThreadID1;
	m_bMapDispEnd    = TRUE;
	m_hMapDispThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MapDispThread, this, NULL, &ThreadID1);
}

//---------------------------------------------------------------------------------------------------------
// 종료에 필요한 작업을 한다.
//---------------------------------------------------------------------------------------------------------
void CMapViewDLG::m_fnSetEnd() 
{
	if ( m_pTaskHandle != NULL )
	{
		m_pTaskHandle->m_fnSetEnd();
		delete m_pTaskHandle;
		m_pTaskHandle = NULL;
	}

	if ( m_hMapDispThread != NULL )
	{
		m_bMapDispEnd			= FALSE;
		SuspendThread( m_hMapDispThread );
		CloseHandle(m_hMapDispThread);
		m_hMapDispThread	    = NULL;
	}

}

UINT MapDispThread(LPVOID pFuncData)
{
	CMapViewDLG		*pQI = (CMapViewDLG *)pFuncData;
	while ( pQI->m_bMapDispEnd )
	{
		if ( pQI->m_hWnd != NULL )
		{
			if ( !pQI->m_bView )
			{
				pQI->PostMessage(UM_MAP_DRAW, 0, 0);
			}
		}
		Sleep( MAP_TIME );
	}

	if ( pQI->m_hWnd == NULL ) return 0;

	if ( pQI->m_hMapDispThread != NULL )
	{
		SuspendThread(pQI->m_hMapDispThread);
		CloseHandle(pQI->m_hMapDispThread);
		pQI->m_hMapDispThread	= NULL;
	}
	return 0;
}

void CMapViewDLG::m_fnSetContourChartToImage(CString strImageInfo)
{
	CRect rect;
	CDC* pDC1 = GetDlgItem(IDC_MAP)->GetDC();
	GetDlgItem(IDC_MAP)->GetClientRect(&rect);
	HDC hDC = pDC1->m_hDC;

	HDC hMemDc = CreateCompatibleDC(hDC);
	HBITMAP hBitmap = CreateCompatibleBitmap(hDC, rect.right, rect.bottom);
	HBITMAP HBmpOld = (HBITMAP)SelectObject(hMemDc, hBitmap);
	BitBlt(hMemDc, 0, 0, rect.right, rect.bottom, hDC, 0, 0, SRCCOPY);
	SelectObject(hMemDc, HBmpOld);

	BITMAPINFOHEADER bmih;
	ZeroMemory(&bmih, sizeof(BITMAPINFOHEADER));
	bmih.biSize = sizeof(BITMAPINFOHEADER);
	bmih.biWidth = rect.right;
	bmih.biHeight = rect.bottom;
	bmih.biPlanes = 1;
	bmih.biBitCount = 24;
	bmih.biCompression = BI_RGB;

	GetDIBits(hDC, hBitmap, 0, rect.bottom, NULL, (LPBITMAPINFO)&bmih, DIB_RGB_COLORS);
	LPBYTE lpBits = new BYTE[bmih.biSizeImage];
	GetDIBits(hDC, hBitmap, 0, rect.bottom, lpBits, (LPBITMAPINFO)&bmih, DIB_RGB_COLORS);
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
	HANDLE hFile = CreateFile(strImageInfo, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	WriteFile(hFile, &bmfh, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	WriteFile(hFile, &bmih, sizeof(BITMAPINFOHEADER), &dwWritten, NULL);
	WriteFile(hFile, lpBits, bmih.biSizeImage, &dwWritten, NULL);

	CloseHandle(hFile);
	delete[] lpBits;
}

//---------------------------------------------------------------------------------------------------------
// 맵 그리기
//---------------------------------------------------------------------------------------------------------
LRESULT CMapViewDLG::m_fnSetMapDraw(  WPARAM nNo, LPARAM lParam )
{
#if OPENCV
	IplImage*							pImage = NULL;
	double dPositionX					= 0L;
	double dPositionY					= 0L;
	double dPositionZ					= 0L;
	CString strDisp("");

	CRect rect;
	CDC* pStaticDC = GetDlgItem( IDC_MAP )->GetDC();
	GetDlgItem( IDC_MAP )->GetClientRect(&rect);
	double dPixelPerInch = (double)rect.Width() / (double)13;

	int iDrawCircleSize = 0;
	CvScalar cvCircleColor;
	int iThickless = 0;

	pImage = cvCreateImage(cvSize( rect.right, rect.bottom ), IPL_DEPTH_8U, 3);
	cvSet( pImage, cvScalar(CV_RGB(50, 50, 50)));//어두운배경

	for (int i = MAX_WAPER_CNT - 1; i >= 0; i--)
	{
		switch (i)
		{
		case WAPER_12:
			iDrawCircleSize = (int)((12.0* dPixelPerInch) / 2.0);
			break;
		case WAPER_8:
			iDrawCircleSize = (int)((8.0 * dPixelPerInch) / 2.0);
			break;
		case WAPER_6:
			iDrawCircleSize = (int)((6.0 * dPixelPerInch) / 2.0);
			break;
		case WAPER_4:
			iDrawCircleSize = (int)((4.0 * dPixelPerInch) / 2.0);
			break;
		}

		cvCircleColor = cvScalar(CV_RGB(0, 0, 0));//원테두리
		iThickless = 1;
		if (m_nWaperSize == i)
		{
			cvCircleColor = cvScalar(CV_RGB(220, 220, 220));//원표시
			iThickless = -1;//색칠
		}
		cvCircle(pImage, cvPoint((rect.bottom / 2), (rect.right / 2)), iDrawCircleSize, cvCircleColor, iThickless);

		if (m_nWaperSize == i)
		{
			cvCircleColor = cvScalar(CV_RGB(200, 200, 0));//노치 플랫 위치 표시
			iThickless = 2;
			cvLine(pImage, cvPoint((rect.right / 2) - iDrawCircleSize-5, (rect.bottom / 2)), cvPoint((rect.right / 2) - iDrawCircleSize - 20, (rect.bottom / 2) - 10), cvCircleColor, iThickless);
			cvLine(pImage, cvPoint((rect.right / 2) - iDrawCircleSize-5, (rect.bottom / 2)), cvPoint((rect.right / 2) - iDrawCircleSize - 20, (rect.bottom / 2) + 10), cvCircleColor, iThickless);

			if (m_dExclusion > 0.0)
			{
				iDrawCircleSize = (int)((double)iDrawCircleSize - m_dExclusion*2.0);

				cvCircleColor = cvScalar(CV_RGB(255, 0, 255));
				iThickless = 1;
				cvCircle(pImage, cvPoint((rect.bottom / 2), (rect.right / 2)), iDrawCircleSize, cvCircleColor, iThickless); //Exc. 원
			}
		}
	}

	//Z축 표시
	cvRectangle(pImage, cvPoint(14, (rect.bottom / 2) + 40), cvPoint(20, (rect.bottom) - 10), cvScalar(CV_RGB(0, 0, 0)), 3 );


	cvLine(pImage, cvPoint(0, (rect.bottom / 2)), cvPoint(rect.Height(), (rect.bottom / 2)), cvScalar(CV_RGB(0, 0, 0)), 1);
	cvLine(pImage, cvPoint((rect.right / 2), 0), cvPoint((rect.bottom / 2), rect.Width()), cvScalar(CV_RGB(0, 0, 0)), 1);

	CString strWaferSize;
	strWaferSize.Format(_T("%.0f"), 13.0 * 25.4 / 2.0);

	m_Util.setPutText(pImage, pImage, (LPSTR)strWaferSize.GetString(), cvPoint((rect.right / 2)-30, (rect.bottom - 17)), cvScalar(CV_RGB(130, 130, 130)));
	m_Util.setPutText(pImage, pImage, (LPSTR)("-" + strWaferSize).GetString(), cvPoint(1, (rect.bottom / 2)-17), cvScalar(CV_RGB(130, 130, 130)));
	m_Util.setPutText(pImage, pImage, (LPSTR)("-" + strWaferSize).GetString(), cvPoint((rect.right / 2 )-30, 1), cvScalar(CV_RGB(130, 130, 130)));
	m_Util.setPutText(pImage, pImage, (LPSTR)strWaferSize.GetString(), cvPoint(rect.right - 30, (rect.bottom / 2)-17), cvScalar(CV_RGB(130, 130, 130)));

	int nX = 0;
	int nY = 0;
	CPoint pointXY;
	if ( theApp.m_fnGetViewPointer()->m_nPage == SW_AUTO )
	{
		for( int i = 0;i < m_pTask->stResult.nSRData; i++ )
		{
			strDisp.Format( "%.3f",m_pTask->stData.m_dDataZ[i][theApp.m_fnGetViewPointer()->m_pAuto->m_nSelLayer][theApp.m_fnGetViewPointer()->m_pAuto->m_nMeasureItem]);
			m_fnSetMouseCoord(pointXY, m_pTask->stResult.stSRData[i].dwXPosition, m_pTask->stResult.stSRData[i].dwYPosition, dPixelPerInch, rect.Width() / 2, rect.Height() / 2);
			if ( i == m_pTask->stData.m_nCurrCnt )
			{
				cvLine(pImage, cvPoint(pointXY.x - 3, pointXY.y), cvPoint(pointXY.x + 3, pointXY.y), cvScalar(CV_RGB(255, 0, 0)), 2);
				cvLine(pImage, cvPoint(pointXY.x, pointXY.y - 3), cvPoint(pointXY.x, pointXY.y + 3), cvScalar(CV_RGB(255, 0, 0)), 2);
				if ( strDisp != "0.000" && m_pTask->stResult.nSRData < 80)
				{
					m_Util.setPutText(pImage, pImage, strDisp.GetBuffer(strDisp.GetLength()), cvPoint( pointXY.x + 2, pointXY.y ), cvScalar(CV_RGB( 255, 0, 0 )));
				}
			}
			else
			{
				cvLine(pImage, cvPoint(pointXY.x - 3, pointXY.y), cvPoint(pointXY.x + 3, pointXY.y), cvScalar(CV_RGB(128, 128, 128)), 2);
				cvLine(pImage, cvPoint(pointXY.x, pointXY.y - 3), cvPoint(pointXY.x, pointXY.y + 3), cvScalar(CV_RGB(128, 128, 128)), 2);
				if ( strDisp != "0.000" && m_pTask->stResult.nSRData < 80)
				{
					m_Util.setPutText(pImage, pImage, strDisp.GetBuffer(strDisp.GetLength()), cvPoint( pointXY.x + 2, pointXY.y ), cvScalar(CV_RGB( 128, 128, 128 )));
				}
			}
		}
	}
	if ( theApp.m_fnGetViewPointer()->m_nPage == SW_REVIEW )
	{
		for( int i = 0;i < m_pTask->stReview.nSRData; i++ )
		{
			strDisp.Format( "%.3f",m_pTask->stReviewData.m_dDataZ[i][theApp.m_fnGetViewPointer()->m_pReview->m_nSelLayer][theApp.m_fnGetViewPointer()->m_pReview->m_nMeasureItem]);
			m_fnSetMouseCoord(pointXY, m_pTask->stReview.stSRData[i].dwXPosition, m_pTask->stReview.stSRData[i].dwYPosition, dPixelPerInch, rect.Width() / 2, rect.Height() / 2);
			if ( i == m_pTask->stReviewData.m_nCurrCnt )
			{
				cvLine( pImage, cvPoint( pointXY.x - 3, pointXY.y),cvPoint( pointXY.x + 3, pointXY.y), cvScalar(CV_RGB( 255, 0, 0 )), 2);
				cvLine( pImage, cvPoint( pointXY.x, pointXY.y - 3),cvPoint( pointXY.x, pointXY.y + 3), cvScalar(CV_RGB( 255, 0, 0 )), 2);
				if ( strDisp != "0.00000" && m_pTask->stReview.nSRData < 80)
				{
					m_Util.setPutText(pImage, pImage, strDisp.GetBuffer(strDisp.GetLength()), cvPoint( pointXY.x + 2, pointXY.y ), cvScalar(CV_RGB( 255, 0, 0 )));
				}
			}
			else
			{
				cvLine( pImage, cvPoint( pointXY.x - 3, pointXY.y),cvPoint( pointXY.x + 3, pointXY.y), cvScalar(CV_RGB(128, 128, 128)), 2);
				cvLine( pImage, cvPoint( pointXY.x, pointXY.y - 3),cvPoint( pointXY.x, pointXY.y + 3), cvScalar(CV_RGB(128, 128, 128)), 2);
				if ( strDisp != "0.00000" && m_pTask->stReview.nSRData < 80)
				{
					m_Util.setPutText(pImage, pImage, strDisp.GetBuffer(strDisp.GetLength()), cvPoint( pointXY.x + 2, pointXY.y ), cvScalar(CV_RGB(128, 128, 128)));
				}
			}
		}
	}
#if AJINTEX_BOARD
	CString strData("");
	if ( this->m_hWnd != NULL )
	{
		if ( theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos( X_AXIS, dPositionX ) == RC_NORMAL )
		{
			strData.Format( "%7.3f", dPositionX );
			GetDlgItem( IDC_EDT_CURR_X )->SetWindowText( strData );
		}
		if (theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos(Y_AXIS, dPositionY) == RC_NORMAL)
		{
			strData.Format("%7.3f", dPositionY);
			GetDlgItem(IDC_EDT_CURR_Y)->SetWindowText(strData);
		}
		if (theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos(Z_AXIS, dPositionZ) == RC_NORMAL)
		{
			strData.Format("%7.3f", dPositionZ);
			GetDlgItem(IDC_EDT_CURR_Z)->SetWindowText(strData);
		}
		strData.Format("%7.3f", pAxis[X_AXIS]->m_dOffsetPos);
		GetDlgItem(IDC_EDT_OFFSET_X)->SetWindowText(strData);
		strData.Format("%7.3f", pAxis[Y_AXIS]->m_dOffsetPos);
		GetDlgItem(IDC_EDT_OFFSET_Y)->SetWindowText(strData);
	}
#endif
	CPoint point;
	m_fnSetMouseCoord(point, dPositionX, dPositionY, dPixelPerInch, rect.Width() / 2, rect.Height() / 2);
// 	m_fnSetMouseCoord(point, dPositionX, dPositionY);
	CPoint InRectPnt;
	InRectPnt.x = point.x < 0 ? 0 : point.x > rect.right ? rect.right : point.x;
	InRectPnt.y = point.y < 0 ? 0 : point.y > rect.bottom ? rect.bottom : point.y;
	cvLine( pImage, cvPoint(InRectPnt.x - 3, InRectPnt.y),cvPoint(InRectPnt.x + 3, InRectPnt.y), cvScalar(CV_RGB( 0, 255, 0 )), 2);
	cvLine( pImage, cvPoint(InRectPnt.x, InRectPnt.y - 3),cvPoint(InRectPnt.x, InRectPnt.y + 3), cvScalar(CV_RGB( 0, 255, 0 )), 2);

	//Z축표시
	point.x = 17;
	point.y = (LONG)((double)(((rect.bottom) - 10) - ((rect.bottom / 2) + 40)) / (theApp.m_fnGetViewPointer()->m_pTask->stCommon.dZLimit) * dPositionZ);
	point.y = point.y < 0 ? 0 : point.y >(((rect.bottom) - 10) - ((rect.bottom / 2) + 40)) ? (((rect.bottom) - 10) - ((rect.bottom / 2) + 40)) : point.y;
	point.y += ((rect.bottom / 2) + 40);
	cvLine(pImage, cvPoint(point.x - 3, point.y), cvPoint(point.x + 3, point.y), cvScalar(CV_RGB(0, 255, 0)), 2);
	cvLine(pImage, cvPoint(point.x, point.y - 3), cvPoint(point.x, point.y + 3), cvScalar(CV_RGB(0, 255, 0)), 2);


//	if ( m_bSave )
//	{
//		m_bSave = FALSE;
//		if (pImage != NULL)
//		{
//			cvSaveImage("D:\\a.bmp", pImage);
//		}
// #if MIL_USE
// 		MIL_ID saveImg;
// 		M_Alloc8(pImage->width, pImage->height, saveImg);
// 		MbufPut(saveImg, pImage->imageData);
// 		MbufExport(m_strImageInfo, M_BMP, saveImg);
// 		MbufFree(saveImg);
// #endif
//	}

	if ( pImage != NULL )
	{
		CvvImage							cImage;
		cImage.CopyOf( pImage, 1 );
		if(pStaticDC != NULL)
		{
			cImage.DrawToHDC(pStaticDC->m_hDC, &rect);
			ReleaseDC(pStaticDC);
		}
		cvReleaseImage( &pImage );
	}
	pImage = NULL;
#endif
	m_bSave = FALSE;
	return 0;
}

void CMapViewDLG::m_fnSetMouseCoord(CPoint &point, double dX, double dY, double dResol, int offsetX/*=0*/, int offsetY/*=0 */)
{
	point.x = (LONG)(dX * dResol / 25.4 + offsetX);
	point.y = (LONG)(dY * dResol / 25.4 + offsetY);
}

void CMapViewDLG::m_fnSetMouseCoord(CPoint &point, double dX, double dY)
{
	point.x = 375 + (LONG)(dX * 2.);
	point.y = 375 + (LONG)(dY * 2.);
}

void CMapViewDLG::OnBnClickedBtn2d()
{
	theApp.m_fnGetViewPointer()->m_fnSetChartView( SW_2D );
}


void CMapViewDLG::OnBnClickedBtn3d()
{
	theApp.m_fnGetViewPointer()->m_fnSetChartView( SW_3D );
}


void CMapViewDLG::OnBnClickedBtnMap()
{
	theApp.m_fnGetViewPointer()->m_fnSetChartView( SW_MAP );
}

