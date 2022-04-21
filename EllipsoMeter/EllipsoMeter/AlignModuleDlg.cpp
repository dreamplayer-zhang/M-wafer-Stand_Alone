
// AlignModuleDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "EllipsoMeter.h"
#if OPENCV
#include "AlignModuleDlg.h"
#include "afxdialogex.h"

#include "OpenCV_LibDlg.h"
#include "DoubleScrollBar.h"

#include "InspFunc.h"
#include "ModelData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CAlignModuleDlg::CAlignModuleDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAlignModuleDlg::IDD, pParent)
{
	this->Create( IDD_ALIGNMODULE_DIALOG );
	m_bShow			= FALSE;
	m_hIcon			= AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nCameraCount	= 0;

}

BOOL CAlignModuleDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (( nID   == 2 ) &&
		( nCode == -1 ))
	{
		m_fnSetShowFlag( FALSE );
	}
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CAlignModuleDlg::m_fnSetShowFlag( BOOL bShow )
{
	m_bShow = bShow;
	if ( m_bShow )
	{
		this->ShowWindow( SW_SHOW );
		if ( pCvDlg != NULL )
		{
			pCvDlg->ShowWindow(SW_SHOW);
		}
	}
	else
	{
		this->ShowWindow( SW_HIDE );
		if ( pCvDlg != NULL )
		{
			pCvDlg->ShowWindow(SW_HIDE);
		}
	}
}

BOOL CAlignModuleDlg::PreTranslateMessage(MSG* pMsg) 
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
void CAlignModuleDlg::m_fnSetInit( ) 
{
	CString s("");
	s.Format( "웨이퍼 얼라인 설정 메인창" );
	this->SetWindowText(s);

	pModelData = (CModelData*)new CModelData;
	pModelData_Backup = (CModelData*)new CModelData;

	pModelData->ListLoad();
	pModelData->Load();

	pInsp = (CInspFunc*)new CInspFunc;

	SetListParaData();

	InitDisplay();

	m_clWaperSize.SetCurSel( 3 );
}

//---------------------------------------------------------------------------------------------------------
// 종료에 필요한 작업을 한다.
//---------------------------------------------------------------------------------------------------------
void CAlignModuleDlg::m_fnSetEnd( ) 
{
	pCvDlg->DestroyWindow();

	delete pCvDlg;

	delete pInsp;
	delete pModelData_Backup;
	delete pModelData;
}

void CAlignModuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_DISP, m_ctrlDisp);
	DDX_Control(pDX, IDC_LIST_PARA, m_ctrlListPara);
	DDX_Control(pDX, IDC_SCROLLBAR_PARA, m_ctrlScrollVal);
	DDX_Control(pDX, IDC_LIST_MSG, m_ctrlListMsg);
	DDX_Control(pDX, IDC_CBO_SIZE, m_clWaperSize);
}

BEGIN_MESSAGE_MAP(CAlignModuleDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_VSCROLL()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_PARA, &OnNMDblclkListPara)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_PARA, &CAlignModuleDlg::OnBnClickedButtonSavePara)
	ON_BN_CLICKED(IDC_BUTTON_INSP, &CAlignModuleDlg::OnBnClickedButtonInsp)
	ON_BN_CLICKED(IDC_BUTTON_LOADIMG, &CAlignModuleDlg::OnBnClickedButtonLoadimg)
	ON_BN_CLICKED(IDC_BUTTON_SAVEIMG, &CAlignModuleDlg::OnBnClickedButtonSaveimg)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CAlignModuleDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECT, &CAlignModuleDlg::OnBnClickedButtonDisconnect)
	ON_BN_CLICKED(IDC_BTN_VIEW, &CAlignModuleDlg::OnBnClickedBtnView)
	ON_BN_CLICKED(IDC_HIDE, &CAlignModuleDlg::OnBnClickedHide)
END_MESSAGE_MAP()


// CAlignModuleDlg 메시지 처리기

BOOL CAlignModuleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CAlignModuleDlg::InitDisplay()
{
	CRect rcDisp;
	m_ctrlDisp.GetWindowRect(rcDisp);

	stcInfoDlgInit stcDlgInfo;
	stcDlgInfo.iImgSizeX = IMG_SIZE_X;
	stcDlgInfo.iImgSizeY = IMG_SIZE_Y;
	stcDlgInfo.iImgCh = 1;

	stcDlgInfo.iDispNum = 0;
	stcDlgInfo.iDispStPosX = rcDisp.left;
	stcDlgInfo.iDispStPosY = rcDisp.top;
	stcDlgInfo.iDispSizeX = rcDisp.Width();
	stcDlgInfo.iDispSizeY = 0;
	pCvDlg = (COpenCV_LibDlg *)new COpenCV_LibDlg(stcDlgInfo);	//display Dlg붙이기

	//	//Dlg생성
	pCvDlg->Create(IDD_DISPLAY_DIALOG, this);
	//pCvDlg->ShowWindow(SW_SHOW);

	if (pCvDlg->ImgBuff_CurDisp != NULL)
	{
		pCvDlg->DisplayImage(IDC_STATIC_DISP, pCvDlg->ImgBuff_CurDisp, pCvDlg->ImgSizeResol);
	}
}


void CAlignModuleDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CAlignModuleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CAlignModuleDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

}


void CAlignModuleDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if(nSBCode==SB_THUMBTRACK || nSBCode==SB_ENDSCROLL)  return;

	stcScroll scrollItem;

	scrollItem.ValType = -1; //0:int / 1: double
	scrollItem.iMin = 0;
	scrollItem.iMax = 0;
	scrollItem.iLine = 0;
	scrollItem.iPage = 0;
	scrollItem.iPos = 0;
	scrollItem.dMin = 0.0;
	scrollItem.dMax = 0.0;
	scrollItem.dLine = 0.0;
	scrollItem.dPage = 0.0;
	scrollItem.dPos = 0.0;


	int count = m_ctrlListPara.GetItemCount();
	int index = -1;
	CString strPara;

	int i;
	for(i = 0; i < count; i++)
	{
		// 개별 항목의 현재 상태가 선택되어진 상태일 경우
		if(LVIS_SELECTED == m_ctrlListPara.GetItemState(i, LVIS_SELECTED))
		{
			strPara = m_ctrlListPara.GetItemText(i,1);
			switch(i)
			{
			case Para_iDiffTh:
				scrollItem.ValType = 0;
				scrollItem.iMin = 5;
				scrollItem.iMax = 240;
				scrollItem.iLine = 1;
				scrollItem.iPage = 10;
				scrollItem.iPos	= atoi(strPara);
				break;
			case Para_iDiffPixel:
				scrollItem.ValType = 0;
				scrollItem.iMin = 1;
				scrollItem.iMax = 100;
				scrollItem.iLine = 1;
				scrollItem.iPage = 5;
				scrollItem.iPos	= atoi(strPara);
				break;
			case Para_iVal:
				scrollItem.ValType = 0;
				scrollItem.iMin = 0;
				scrollItem.iMax = 255;
				scrollItem.iLine = 1;
				scrollItem.iPage = 5;
				scrollItem.iPos	= atoi(strPara);
				break;
			case Para_iValDir:
				scrollItem.ValType = 0;
				scrollItem.iMin = 0;
				scrollItem.iMax = 1;
				scrollItem.iLine = 1;
				scrollItem.iPage = 1;
				scrollItem.iPos	= atoi(strPara);
				break;
			case Para_iScandir:
				scrollItem.ValType = 0;
				scrollItem.iMin = 0;
				scrollItem.iMax = 0;
				scrollItem.iLine = 0;
				scrollItem.iPage = 0;
				scrollItem.iPos	= atoi(strPara);
				break;
			case Para_dSigmaSTD:
				scrollItem.ValType = 1;
				scrollItem.dMin = 0.1;
				scrollItem.dMax = 5.0;
				scrollItem.dLine = 0.1;
				scrollItem.dPage = 0.2;
				scrollItem.dPos	= atof(strPara);
				break;
			case Para_iNochKind:
				scrollItem.ValType = 0;
				scrollItem.iMin = 0;
				scrollItem.iMax = 1;
				scrollItem.iLine = 1;
				scrollItem.iPage = 1;
				scrollItem.iPos = atoi(strPara);
				break;
			}

			index = i;
			break;
		}
		else
		{
			continue;
		}
	}

	if(index == -1)
		return;


	CString StrVal;
	CDoubleScrollBar* pDoubleScrollBar;
	if (scrollItem.ValType)
	{//double형 스크롤바
		pDoubleScrollBar = (CDoubleScrollBar*)pScrollBar;

		pDoubleScrollBar->SetScrollRange(scrollItem.dMin, scrollItem.dMax);

		if(nSBCode==SB_LINELEFT)    scrollItem.dPos -= scrollItem.dLine;
		else if(nSBCode==SB_LINERIGHT)   scrollItem.dPos += scrollItem.dLine;
		else if(nSBCode==SB_PAGELEFT)   scrollItem.dPos -= scrollItem.dPage;
		else if(nSBCode==SB_PAGERIGHT)   scrollItem.dPos += scrollItem.dPage;
		else if(nSBCode == SB_THUMBPOSITION)
		{
			scrollItem.dPos = pDoubleScrollBar->GetTrackPos();
			pDoubleScrollBar->SetScrollPos(scrollItem.dPos);
		}

		scrollItem.dPos = scrollItem.dPos < scrollItem.dMin ? scrollItem.dMin : scrollItem.dPos;
		scrollItem.dPos = scrollItem.dPos > scrollItem.dMax ? scrollItem.dMax : scrollItem.dPos;

		pDoubleScrollBar->SetScrollPos(scrollItem.dPos);

		StrVal.Format(_T("%.2f"), scrollItem.dPos);
		m_ctrlListPara.SetItem(index, 1, LVIF_TEXT, StrVal, 0, 0, 0, NULL );
	} 
	else
	{//int형 스크롤바
		pScrollBar->SetScrollRange(scrollItem.iMin, scrollItem.iMax);

		if(nSBCode==SB_LINELEFT)    scrollItem.iPos -= scrollItem.iLine;
		else if(nSBCode==SB_LINERIGHT)   scrollItem.iPos += scrollItem.iLine;
		else if(nSBCode==SB_PAGELEFT)   scrollItem.iPos -= scrollItem.iPage;
		else if(nSBCode==SB_PAGERIGHT)   scrollItem.iPos += scrollItem.iPage;
		else if(nSBCode == SB_THUMBPOSITION) scrollItem.iPos = nPos;

		scrollItem.iPos = scrollItem.iPos < scrollItem.iMin ? scrollItem.iMin : scrollItem.iPos;
		scrollItem.iPos = scrollItem.iPos > scrollItem.iMax ? scrollItem.iMax : scrollItem.iPos;

		pScrollBar->SetScrollPos(scrollItem.iPos);

		StrVal.Format(_T("%d"), scrollItem.iPos);
		m_ctrlListPara.SetItem(index, 1, LVIF_TEXT, StrVal, 0, 0, 0, NULL );
	}


	GetListParaData();

	UpdateData(false);

	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CAlignModuleDlg::OnNMDblclkListPara(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	stcScroll scrollItem;

	scrollItem.ValType = -1; //0:int / 1: double
	scrollItem.iMin = 0;
	scrollItem.iMax = 0;
	scrollItem.iLine = 0;
	scrollItem.iPage = 0;
	scrollItem.iPos = 0;
	scrollItem.dMin = 0.0;
	scrollItem.dMax = 0.0;
	scrollItem.dLine = 0.0;
	scrollItem.dPage = 0.0;
	scrollItem.dPos = 0.0;


	int count = m_ctrlListPara.GetItemCount();
	int index = 0;
	CString strPara;

	int i;
	for(i = 0; i < count; i++)
	{
		// 개별 항목의 현재 상태가 선택되어진 상태일 경우
		if(LVIS_SELECTED == m_ctrlListPara.GetItemState(i, LVIS_SELECTED))
		{
			strPara = m_ctrlListPara.GetItemText(i,1);
			switch(i)
			{
			case Para_iDiffTh:
				scrollItem.ValType = 0;
				scrollItem.iMin = 5;
				scrollItem.iMax = 240;
				scrollItem.iLine = 1;
				scrollItem.iPage = 10;
				scrollItem.iPos	= atoi(strPara);
				break;
			case Para_iDiffPixel:
				scrollItem.ValType = 0;
				scrollItem.iMin = 1;
				scrollItem.iMax = 100;
				scrollItem.iLine = 1;
				scrollItem.iPage = 5;
				scrollItem.iPos	= atoi(strPara);
				break;
			case Para_iVal:
				scrollItem.ValType = 0;
				scrollItem.iMin = 0;
				scrollItem.iMax = 255;
				scrollItem.iLine = 1;
				scrollItem.iPage = 5;
				scrollItem.iPos	= atoi(strPara);
				break;
			case Para_iValDir:
				scrollItem.ValType = 0;
				scrollItem.iMin = 0;
				scrollItem.iMax = 1;
				scrollItem.iLine = 1;
				scrollItem.iPage = 1;
				scrollItem.iPos	= atoi(strPara);
				break;
			case Para_iScandir:
				scrollItem.ValType = 0;
				scrollItem.iMin = 0;
				scrollItem.iMax = 0;
				scrollItem.iLine = 0;
				scrollItem.iPage = 0;
				scrollItem.iPos	= atoi(strPara);
				break;
			case Para_dSigmaSTD:
				scrollItem.ValType = 1;
				scrollItem.dMin = 0.1;
				scrollItem.dMax = 5.0;
				scrollItem.dLine = 0.1;
				scrollItem.dPage = 0.2;
				scrollItem.dPos = atof(strPara);
				break;
			case Para_iNochKind:
				scrollItem.ValType = 0;
				scrollItem.iMin = 0;
				scrollItem.iMax = 1;
				scrollItem.iLine = 1;
				scrollItem.iPage = 1;
				scrollItem.iPos = atoi(strPara);
				break;
			}

			index = i;
			break;
		}
	}

	CString StrVal;
	CDoubleScrollBar* pDoubleScrollBar;
	CScrollBar* pScrollBar;
	if (scrollItem.ValType)
	{//double형 스크롤바
		pDoubleScrollBar = (CDoubleScrollBar*)&m_ctrlScrollVal;

		pDoubleScrollBar->SetScrollRange(scrollItem.dMin, scrollItem.dMax);
		pDoubleScrollBar->SetScrollPos(scrollItem.dPos);
	} 
	else
	{//int형 스크롤바
		pScrollBar = (CScrollBar*)&m_ctrlScrollVal;
		pScrollBar->SetScrollRange(scrollItem.iMin, scrollItem.iMax);
		pScrollBar->SetScrollPos(scrollItem.iPos);
	}
	*pResult = 0;
}

void CAlignModuleDlg::AddParaItem(int raw, int cal, stcListItem ParaData)
{
	m_ctrlListPara.InsertItem(raw, ParaData.name);

	CString StrVal;
	if (ParaData.ValType == 0)//int
	{
		StrVal.Format(_T("%d"), ParaData.iVal);
	} 
	else
	{
		StrVal.Format(_T("%.2f"), ParaData.dVal);
	}

	m_ctrlListPara.SetItem(raw, cal, LVIF_TEXT, StrVal, 0, 0, 0, NULL );
}

CAlignModuleDlg::stcListItem CAlignModuleDlg::ReadParaItem(int raw, int cal)
{
	stcListItem	tmpItem;

	CString StrTemp;

	StrTemp = m_ctrlListPara.GetItemText(raw, cal);
	tmpItem.iVal = atoi(StrTemp);
	tmpItem.dVal = atof(StrTemp);

	return tmpItem;
}

void CAlignModuleDlg::GetListParaData()
{
	UpdateData(TRUE);

	stcListItem item[Max_Para_Cnt_Pattern];

	int count = m_ctrlListPara.GetItemCount();

	int i;
	for(i = 0; i < count; i++)
	{
		item[i] = ReadParaItem(i, 1);
	}

	pModelData->iDiffTh		= item[Para_iDiffTh		].iVal;
	pModelData->iDiffPixel	= item[Para_iDiffPixel	].iVal;
	pModelData->iVal		= item[Para_iVal		].iVal;
	pModelData->iValDir		= item[Para_iValDir		].iVal;
	pModelData->iScandir	= item[Para_iScandir	].iVal;
	pModelData->dSigmaSTD	= item[Para_dSigmaSTD	].dVal;
	pModelData->iNochKind	= item[Para_iNochKind	].iVal;
}

void CAlignModuleDlg::SetListParaData()
{
	CRect tmpRect;
	m_ctrlListPara.GetWindowRect(tmpRect);

	// 리스트 초기화 
	m_ctrlListPara.DeleteAllItems();

	// 리스트 스타일 설정
	m_ctrlListPara.SetExtendedStyle(LVS_EX_GRIDLINES);

	// 타이틀 삽입
	m_ctrlListPara.InsertColumn(0, _T("파라미터 이름"), LVCFMT_LEFT, (int)(0.6*tmpRect.Width()), -1);
	m_ctrlListPara.InsertColumn(1, _T("설정값"), LVCFMT_RIGHT, (int)(0.38*tmpRect.Width()), -1);

	stcListItem item[Max_Para_Cnt_Pattern];


	int i=0;
	for (i=0; i<Max_Para_Cnt_Pattern; i++)
	{
		switch(i)
		{
		case Para_iDiffTh:
			sprintf_s(item[i].name, sizeof(item[i].name), _T("밝기 차이값"));
			item[i].iVal = pModelData->iDiffTh;	
			item[i].ValType = 0;
			break;
		case Para_iDiffPixel:
			sprintf_s(item[i].name, sizeof(item[i].name), _T("밝기 비교 픽셀"));
			item[i].iVal = pModelData->iDiffPixel;	
			item[i].ValType = 0;
			break;
		case Para_iVal:
			sprintf_s(item[i].name, sizeof(item[i].name), _T("기준 밝기값"));
			item[i].iVal = pModelData->iVal;	
			item[i].ValType = 0;
			break;
		case Para_iValDir:
			sprintf_s(item[i].name, sizeof(item[i].name), _T("밝기 범위"));
			item[i].iVal = pModelData->iValDir;	
			item[i].ValType = 0;
			break;
		case Para_iScandir:
			sprintf_s(item[i].name, sizeof(item[i].name), _T("스캔방향"));
			item[i].iVal = pModelData->iScandir;	
			item[i].ValType = 0;
			break;
		case Para_dSigmaSTD:
			sprintf_s(item[i].name, sizeof(item[i].name), _T("편차설정"));
			item[i].dVal = pModelData->dSigmaSTD;
			item[i].ValType = 1;
			break;
		case Para_iNochKind:
			sprintf_s(item[i].name, sizeof(item[i].name), _T("플랫/노치"));
			item[i].iVal = pModelData->iNochKind;
			item[i].ValType = 0;
			break;
		}

		AddParaItem(i, 1, item[i]);
	}

	UpdateData(FALSE);
}


void CAlignModuleDlg::OnBnClickedButtonSavePara()
{
	GetListParaData();
	pModelData->Save();
}

void CAlignModuleDlg::m_fnSetImageRotation( IplImage*  pSrcImage, IplImage*  pDestImage, int nAngle, double dScale )
{
	CvMat *matrix = cvCreateMat( 2, 3, CV_32FC1);
    cv2DRotationMatrix(cvPoint2D32f(pSrcImage->width / 2 , pSrcImage->height / 2 ) , nAngle , dScale , matrix);
    cvWarpAffine(pSrcImage , pDestImage , matrix , CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS , cvScalarAll(0));
}

BOOL CAlignModuleDlg::m_fnGetAlignValue( int nInch, int nNochKind, double &dAngle )
{
	BOOL bInspSuccess = FALSE;
	BOOL bFind        = TRUE;

#if SPINNAKER_CAMERA
	CString strData("");
	IplImage* pDestImage = NULL;
	while( bFind )
	{
		if ( theApp.m_fnGetViewPointer()->m_pCam->m_bGrabed )
		{
			bFind = FALSE;
		}
		Sleep(1);
	}
	pCvDlg->ImgBuff_Original->imageData = (char*)theApp.m_fnGetViewPointer()->m_pCam->m_pBuffer;
	pDestImage = cvCreateImage(cvSize(pCvDlg->ImgBuff_Original->width, pCvDlg->ImgBuff_Original->height), IPL_DEPTH_8U, pCvDlg->ImgBuff_Original->nChannels);
	m_fnSetImageRotation( pCvDlg->ImgBuff_Original, pDestImage, 90, 1 ); 
	cvCvtColor(pDestImage, pCvDlg->ImgBuff_OverlayDisp, CV_GRAY2BGR);
	cvCvtColor(pDestImage, pCvDlg->ImgBuff_CurDisp, CV_GRAY2BGR);
	
	unsigned char* srcImg = new unsigned char[pDestImage->width * pDestImage->height * pDestImage->nChannels];
	pInsp->m_InspImage = cvLib->CreateBuffer(pDestImage->width, pDestImage->height, pDestImage->nChannels);
	cvCopy(pDestImage, pInsp->m_InspImage);

	cvLib->ImgBuffCnvt_ipl2char(pInsp->m_InspImage, srcImg, pInsp->m_InspImage->nChannels);

	IplImage* resImg;
	double dResDeg = 0.0L;

	dAngle = 0.0;
	pModelData->iNochKind = nNochKind; 
	bInspSuccess = pInsp->AlignWafer(srcImg, pDestImage->width,pDestImage->height, pModelData->iNochKind, nInch, &resImg, &dResDeg);

	if (bInspSuccess)
	{
		strData.Format( "%.3f", dResDeg );
		GetDlgItem( IDC_EDT_ANGLE )->SetWindowText( strData );
		pCvDlg->DisplayImage(IDC_STATIC_DISP, resImg, pCvDlg->ImgSizeResol);
		dAngle = dResDeg;
	}
	cvReleaseImage(&pDestImage);
	delete[] srcImg;
#endif
	return bInspSuccess;
}

void CAlignModuleDlg::OnBnClickedButtonInsp()
{
#if SPINNAKER_CAMERA
	double  dAngle = 0.0;
	BOOL    bSuccess = FALSE;
	CString strSize("");
	theApp.m_fnGetViewPointer( )->m_pCam->m_bGrabed = FALSE;
	m_clWaperSize.GetLBText( m_clWaperSize.GetCurSel(), strSize );
	bSuccess = m_fnGetAlignValue( atoi(strSize), pModelData->iNochKind, dAngle );
	if ( bSuccess )
	{
#if AJINTEX_BOARD
		DWORD dwBusyStatus	= 0;
		double dwSpeed       = theApp.m_fnGetViewPointer( )->m_pTask->stCommon.dSpeedHigh;
		double dwMove        = dAngle;
		DWORD dwRtnCode = theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmStatusReadInMotion( R_AXIS, dwBusyStatus );
		if ( dwBusyStatus == 0 )
		{
			theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmMotSetAbsRelMode( R_AXIS, POS_REL_MODE );
			theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmMoveStartPos( R_AXIS, dwMove, dwSpeed, dwSpeed * 2, dwSpeed * 2 );
		}
#endif
	}
	else
	{
		TECmsgOut( L"Failed", RED_1, 1 );
	}
#endif
}


void CAlignModuleDlg::OnBnClickedButtonLoadimg()
{
	pCvDlg->OnPopMenuImageLoad();
}


void CAlignModuleDlg::OnBnClickedButtonSaveimg()
{
	pCvDlg->OnPopMenuImageSave();
}




void CAlignModuleDlg::OnBnClickedButtonConnect()
{
#if SPINNAKER_CAMERA
	BOOL bConnect = FALSE;
	if ( theApp.m_fnGetViewPointer()->m_pCam != NULL )
	{
		bConnect = theApp.m_fnGetViewPointer()->m_pCam->m_fnGetIsConnect();
		if ( !bConnect)
		{
			theApp.m_fnGetViewPointer()->m_pCam->m_fnSetConnect( true );
		}
	}
#endif
}


void CAlignModuleDlg::OnBnClickedButtonDisconnect()
{
#if SPINNAKER_CAMERA
	BOOL bConnect = FALSE;
	if ( theApp.m_fnGetViewPointer()->m_pCam != NULL )
	{
		bConnect = theApp.m_fnGetViewPointer()->m_pCam->m_fnGetIsConnect();
		if ( bConnect)
		{
			theApp.m_fnGetViewPointer()->m_pCam->m_fnSetConnect( false );
		}
	}
#endif
}


void CAlignModuleDlg::OnBnClickedBtnView()
{
#if SPINNAKER_CAMERA
	if ( theApp.m_fnGetViewPointer()->m_pCam == NULL )
	{
		theApp.m_fnGetViewPointer()->m_pCam = new CEasyAccessToolDlg();
		theApp.m_fnGetViewPointer()->m_pCam->m_fnSetInit( );
	}
	theApp.m_fnGetViewPointer()->m_pCam->m_fnSetShowFlag( !theApp.m_fnGetViewPointer()->m_pCam->m_bShow );
#endif
}


void CAlignModuleDlg::OnBnClickedHide()
{
	m_fnSetShowFlag( FALSE );
}
#endif