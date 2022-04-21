
// OpenCV_LibDlg.cpp : 구현 파일
//

#include "stdafx.h"
#if OPENCV
#include "OpenCV_Lib.h"
#include "OpenCV_LibDlg.h"
#include "afxdialogex.h"

// #include "InspFunc.h"

#include "AlignModuleDlg.h"

#include "ModelData.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// COpenCV_LibDlg 대화 상자

COpenCV_LibDlg *pCvDlg;


COpenCV_LibDlg::COpenCV_LibDlg(stcInfoDlgInit stcInitInfo, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DISPLAY_DIALOG, pParent)
	, m_RadioAreaShape(0)
	, m_bInsideCircle(FALSE)
	, m_bLiveMode(FALSE)
	, m_bAutoMode(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_bConnectCam = FALSE;

	m_SizeX = stcInitInfo.iImgSizeX;
	m_SizeY = stcInitInfo.iImgSizeY;
	m_ImgCh = stcInitInfo.iImgCh;

	m_iDisplayNum = stcInitInfo.iDispNum;
	m_iDispStPosX = stcInitInfo.iDispStPosX;
	m_iDispStPosY = stcInitInfo.iDispStPosY;
	m_iDispSizeX = stcInitInfo.iDispSizeX;
	m_iDispSizeY = (int)((double)m_iDispSizeX * (double)m_SizeY / (double)m_SizeX);

	bLClick = FALSE;
	bDragPt = FALSE;

	cvLib = new COpenCV_LibApp(m_SizeX, m_SizeY);

	m_pDspThread = NULL;
}

void COpenCV_LibDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_AREA1, (int&)m_RadioAreaShape);
}

BEGIN_MESSAGE_MAP(COpenCV_LibDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	// 	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_IMG_LOAD, &COpenCV_LibDlg::OnBnClickedButtonImgLoad)
	ON_BN_CLICKED(IDC_BUTTON_IMG_SAVE, &COpenCV_LibDlg::OnBnClickedButtonImgSave)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_IMAGELOAD, &COpenCV_LibDlg::OnPopMenuImageLoad)
	ON_COMMAND(ID_IMAGESAVE, &COpenCV_LibDlg::OnPopMenuImageSave)
	ON_WM_RBUTTONUP()
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_AREA1, IDC_RADIO_AREA2, &COpenCV_LibDlg::OnBnClickedRadioAreaShape)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()
// COpenCV_LibDlg 메시지 처리기



BOOL COpenCV_LibDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ModifyStyleEx(WS_EX_APPWINDOW, 0);//작업표시줄에서 숨기기
	
	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	ImgBuff_Original = cvLib->CreateBuffer(m_SizeX, m_SizeY, m_ImgCh);

	ImgBuff_OverlayDisp = cvLib->CreateBuffer(m_SizeX, m_SizeY, 3);
	ImgBuff_CurDisp = cvLib->CreateBuffer(m_SizeX, m_SizeY, 3);

	ImgBuff_Grab = cvLib->CreateBuffer(m_SizeX, m_SizeY, m_ImgCh);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void COpenCV_LibDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void COpenCV_LibDlg::OnPaint()
{
	CRect rect;
	CRect rectDlg;
	this->GetClientRect(rectDlg);

	rect.left	= rectDlg.left		+ DLG2DISP_GAP;
	rect.right	= rectDlg.right		- DLG2DISP_GAP;
	rect.top	= rectDlg.top		+ DLG2DISP_GAP;
	rect.bottom = rectDlg.bottom	- DLG2DISP_GAP;

	GetDlgItem(IDC_STATIC_DISP)->SetWindowPos(NULL, DLG2DISP_GAP, DLG2DISP_GAP, rect.right - rect.left, rect.bottom - rect.top, NULL);
	GetDlgItem(IDC_STATIC_DISP_OVERLAY)->SetWindowPos(NULL, DLG2DISP_GAP, DLG2DISP_GAP, rect.right - rect.left, rect.bottom - rect.top, NULL);

	GetDlgItem(IDC_RADIO_AREA1)->ShowWindow(FALSE);
	GetDlgItem(IDC_RADIO_AREA2)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_GRP_AREA_SHAPE)->ShowWindow(FALSE);

}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR COpenCV_LibDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL COpenCV_LibDlg::DestroyWindow()
{
	DWORD dwResult = 0;
	m_bLiveMode = FALSE;
	if (m_pDspThread != NULL)
		dwResult = WaitForSingleObject(m_pDspThread->m_hThread, INFINITE);

	if (ImgBuff_Original != nullptr)
		cvLib->ReleaseBuffer(ImgBuff_Original);
	if (ImgBuff_Grab != nullptr)
		cvLib->ReleaseBuffer(ImgBuff_Grab);
	if (ImgBuff_CurDisp != nullptr)
		cvLib->ReleaseBuffer(ImgBuff_CurDisp);
	if (ImgBuff_OverlayDisp != nullptr)
		cvLib->ReleaseBuffer(ImgBuff_OverlayDisp);

	if (cvLib != NULL)
		delete cvLib;

	return CDialogEx::DestroyWindow();
}

void COpenCV_LibDlg::DisplayImage(int IDC_PICTURE_TARGET, IplImage* targetMat, float* ImgResol)
{
	memset(ImgResol, 0, sizeof(float) * 2);

	IplImage* targetIplImage = nullptr;
// 	targetIplImage = (IplImage*)new IplImage;
//	memset(&targetIplImage, 0x00, sizeof(targetIplImage));
	if (targetMat != NULL)
		targetIplImage = targetMat;

	if (targetIplImage != NULL)
	{
		CWnd* pWnd_ImageTraget = GetDlgItem(IDC_PICTURE_TARGET);
		CClientDC dcImageTraget(pWnd_ImageTraget);
		RECT rcImageTraget;
		pWnd_ImageTraget->GetClientRect(&rcImageTraget);

		BITMAPINFO bitmapInfo;
		memset(&bitmapInfo, 0, sizeof(bitmapInfo));
		bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitmapInfo.bmiHeader.biPlanes = 1;
		bitmapInfo.bmiHeader.biCompression = BI_RGB;
		bitmapInfo.bmiHeader.biWidth = targetIplImage->width;
		bitmapInfo.bmiHeader.biHeight = -targetIplImage->height;

		IplImage *tempImage = NULL;

		if (targetIplImage->nChannels == 1)
		{
			tempImage = cvCreateImage(cvSize(targetIplImage->width, targetIplImage->height), IPL_DEPTH_8U, 3);
			cvCvtColor(targetIplImage, tempImage, CV_GRAY2BGR);
		}
		else if (targetIplImage->nChannels == 3)
		{
			tempImage = cvCloneImage(targetIplImage);
		}

		bitmapInfo.bmiHeader.biBitCount = tempImage->depth * tempImage->nChannels;

		dcImageTraget.SetStretchBltMode(COLORONCOLOR);

		::StretchDIBits(dcImageTraget.GetSafeHdc(), rcImageTraget.left, rcImageTraget.top, rcImageTraget.right, rcImageTraget.bottom,
			0, 0, tempImage->width, tempImage->height, tempImage->imageData, &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);


		ImgResol[0] = (float)(rcImageTraget.right - rcImageTraget.left) / (float)tempImage->width;
		ImgResol[1] = (float)(rcImageTraget.bottom - rcImageTraget.top) / (float)tempImage->height;

		cvReleaseImage(&tempImage);
	}

	cvCopy(ImgBuff_CurDisp, ImgBuff_OverlayDisp);
}



void COpenCV_LibDlg::OnBnClickedButtonImgLoad()
{
// 	cvSetZero(ImgBuff_Disp);
// // 	ImgBuff_Disp = cvLib->LoadImgFile(TODO);
// 
// 
// 	if (ImgBuff_Disp != nullptr)
// // 		memcpy(cvLib->m_pImageBuffer, ImgBuff_Disp->imageData, sizeof(unsigned char)*m_SizeX*m_SizeY*m_ImgCh);
// 	DisplayImage(IDC_STATIC_DISP, ImgBuff_Disp, ImgSizeResol);
// 
}


void COpenCV_LibDlg::OnBnClickedButtonImgSave()
{
// 	cvLib->SaveImgFile(TODO,ImgBuff_Disp, TODO);
}

BOOL COpenCV_LibDlg::PreTranslateMessage(MSG* pMsg)
{
// 	char tmpStr[50];
// 	CString tmpCtrlStr;
// 	GetClassName(pMsg->hwnd, tmpStr, 50);
// 	tmpCtrlStr.Format(tmpStr);
// 
// 	if (tmpCtrlStr == "Static")
// 	{
// 
// 		if (pMsg->message == WM_RBUTTONDOWN)
// 		{
// 		}
// 		else
// 		{
// 			if (pMsg->message == WM_LBUTTONDOWN)
// 			{
// 
// 			}
// 
// 			if (pMsg->message == WM_LBUTTONUP)
// 			{
// 			}
// 		}
// 
// 	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void COpenCV_LibDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (bDragPt)
	{
		if (ImgBuff_CurDisp != nullptr)
			cvCopy(ImgBuff_CurDisp, ImgBuff_OverlayDisp);
		DisplayImage(IDC_STATIC_DISP, ImgBuff_OverlayDisp, ImgSizeResol);
	}

	if(bLClick)
	{
		drawPt[1] = ConvertPnt(point);

		if ((drawPt[0].x != drawPt[1].x) || (drawPt[0].y != drawPt[1].y))
		{
			bDragPt = TRUE;
		}
	}

	if (bDragPt)
	{
		if(m_RadioAreaShape == 0)
		{
			cvLib->DoDrawOverlay(ImgBuff_OverlayDisp, DRAW_TYPE_RECT, drawPt, cvScalar(CV_LIGHTMAGENTA), 5, 2, 0);
		}
		else
		{
			m_fDrawRadius = sqrtf(((float)(drawPt[1].y - drawPt[0].y)*(float)(drawPt[1].y - drawPt[0].y)) + ((float)(drawPt[1].x - drawPt[0].x)*(float)(drawPt[1].x - drawPt[0].x)));
			cvLib->DoDrawOverlay(ImgBuff_OverlayDisp, DRAW_TYPE_CIRCLE, drawPt, cvScalar(CV_LIGHTMAGENTA), 3, 1, (int)m_fDrawRadius);
		}

		DisplayImage(IDC_STATIC_DISP, ImgBuff_OverlayDisp, ImgSizeResol);
	}

	CDialogEx::OnMouseMove(nFlags, point);
}



void COpenCV_LibDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (ImgBuff_CurDisp != nullptr)
		cvCopy(ImgBuff_CurDisp, ImgBuff_OverlayDisp);
	DisplayImage(IDC_STATIC_DISP, ImgBuff_OverlayDisp, ImgSizeResol);

	if(m_RadioAreaShape != 1)//내원그릴때 좌표 초기화 안함
		drawPt[0] = ConvertPnt(point);

	bLClick = TRUE;

	CDialogEx::OnLButtonDown(nFlags, point);
}


void COpenCV_LibDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	int pos;
	unsigned char redPX, greenPX, bluePX;

// 	if (pVision->m_bGrabStatus)
// 		return;

	if (!bDragPt)
	{
		drawPt[0] = ConvertPnt(point);

		cvLib->DoDrawOverlay(ImgBuff_OverlayDisp, DRAW_TYPE_CROSS, drawPt, cvScalar(CV_LIGHTMAGENTA), 5, 1, 20);
		DisplayImage(IDC_STATIC_DISP, ImgBuff_OverlayDisp, ImgSizeResol);

		TCHAR msg[128];
		//버퍼색상
		pos = drawPt[0].y * ImgBuff_CurDisp->widthStep + drawPt[0].x * m_ImgCh;
		if (m_ImgCh == 3)
		{
			redPX	= ImgBuff_CurDisp->imageData[pos + 2];
			greenPX = ImgBuff_CurDisp->imageData[pos + 1];
			bluePX = ImgBuff_CurDisp->imageData[pos + 0];

			sprintf_s(msg, sizeof(msg), _T("Disp#%d 픽셀좌표(%d,%d) - RGB(%d/%d/%d)"), m_iDisplayNum + 1, drawPt[0].x, drawPt[0].y, (int)redPX, (int)greenPX, (int)bluePX);
		} 
		else
		{
// 			try
			{
				bluePX = ImgBuff_CurDisp->imageData[pos + 0];
			}
// 			catch (CMemoryException* e)
// 			{
// 				return;
// 			}
// 			catch (CFileException* e)
// 			{
// 				return;
// 			}
// 			catch (CException* e)
// 			{
// 				return;
// 			}

			sprintf_s(msg, sizeof(msg), _T("Disp#%d 픽셀좌표(%d,%d) - Mono(%d)"), m_iDisplayNum + 1, drawPt[0].x, drawPt[0].y, (int)bluePX);
		}

		OutputDebugString(msg);

// 		pView->putListData(msg);

// 		if (redPX == greenPX && greenPX == bluePX)
// 		{
// 			pInsp->ThresholdVal = redPX;
// 		}
// 		else
// 		{
// 			pInsp->SelPxVal[0] = (int)redPX;
// 			pInsp->SelPxVal[1] = (int)greenPX;
// 			pInsp->SelPxVal[2] = (int)bluePX;
// 
// 			if(pVisionDlg->IsWindowVisible() == TRUE)
// 			{
// 				if (m_bSelectArea == ROTATE_AREA)
// 				{
// 					pModelData->BrValRotate[0] = pInsp->SelPxVal[0];
// 					pModelData->BrValRotate[1] = pInsp->SelPxVal[1];
// 					pModelData->BrValRotate[2] = pInsp->SelPxVal[2];
// 				}
// 				else if (m_bSelectArea == INSP_AREA)
// 				{
// 					pModelData->BrValSubImg[0] = pInsp->SelPxVal[0];
// 					pModelData->BrValSubImg[1] = pInsp->SelPxVal[1];
// 					pModelData->BrValSubImg[2] = pInsp->SelPxVal[2];
// 				}
// 			}
// 
// 		}
	}
// 	else
// 	{
// 		if (m_RadioAreaShape == 0)
// 		{
// 			pInsp->rectROI.left = drawPt[0].x;
// 			pInsp->rectROI.top = drawPt[0].y;
// 			pInsp->rectROI.right = drawPt[1].x;
// 			pInsp->rectROI.bottom = drawPt[1].y;
// 
// 			if (pVisionDlg->IsWindowVisible() == TRUE)
// 			{
// 				if (m_bSelectArea == ROTATE_AREA)
// 					pModelData->rectImgRotateStd = pInsp->rectROI;
// 				else if (m_bSelectArea == INSP_AREA)
// 					pModelData->rectInsp = pInsp->rectROI;
// 			}
// 		}
// 		else
// 		{
// 			pInsp->CircleRadius[m_bInsideCircle] = pInsp->fRadius;
// 			if (pVisionDlg->IsWindowVisible() == TRUE)
// 				pModelData->CircleRadius[m_bInsideCircle] = pInsp->fRadius;
// 			if(m_bInsideCircle == OUTSIDE)
// 			{
// 				pInsp->CircleCtr[OUTSIDE].x = drawPt[0].x;
// 				pInsp->CircleCtr[OUTSIDE].y = drawPt[0].y;
// 				pInsp->CircleCtr[INSIDE].x = drawPt[0].x;
// 				pInsp->CircleCtr[INSIDE].y = drawPt[0].y;
// 
// 				if (pVisionDlg->IsWindowVisible() == TRUE)
// 				{
// 					pModelData->CircleCtr[OUTSIDE] = pInsp->CircleCtr[OUTSIDE];
// 					pModelData->CircleCtr[INSIDE] = pInsp->CircleCtr[INSIDE];
// 				}
// 			}
// 			else
// 			{
// 				pInsp->CircleArea = (float)(M_PI*pInsp->CircleRadius[OUTSIDE] * pInsp->CircleRadius[OUTSIDE]) - (float)(M_PI*pInsp->CircleRadius[INSIDE] * pInsp->CircleRadius[INSIDE]);
// 				if (pVisionDlg->IsWindowVisible() == TRUE)
// 					pModelData->CircleArea = pInsp->CircleArea;
// 			}
// 			m_bInsideCircle = m_bInsideCircle != OUTSIDE ? OUTSIDE : INSIDE;
// 		}
// 	}


	bLClick = FALSE;
	bDragPt = FALSE;

// 	if (pVisionDlg != nullptr)
// 	{
// 		pVisionDlg->SetData();
// 	}

	CDialogEx::OnLButtonUp(nFlags, point);
}


void COpenCV_LibDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	return;

// 	if (m_iDisplayNum != MAIN_DISP)
// 		return;

	if (ImgBuff_Original != nullptr)
	{
		if (m_ImgCh == 1)
			cvCvtColor(ImgBuff_Original, ImgBuff_CurDisp, CV_GRAY2BGR);
		else
			cvCopy(ImgBuff_Original, ImgBuff_CurDisp);
	}
	DisplayImage(IDC_STATIC_DISP, ImgBuff_CurDisp, ImgSizeResol);

	m_bInsideCircle = 0;

	CDialogEx::OnRButtonDown(nFlags, point);
}


void COpenCV_LibDlg::OnPopMenuImageLoad()
{
	char *loadFileName = (char *)NULL;

	CString szFilter = _T("Image (*.TIF, *.BMP, *.GIF, *.JPG, *.PNG) | *.TIF; *.BMP; *.GIF; *.JPG; *.PNG; *.tif; *.bmp;*.gif;*.jpg;*.png | All Files(*.*)|*.*||");

	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST, szFilter, AfxGetMainWnd());
	if (fileDlg.DoModal() == IDOK)
	{
		if (ImgBuff_Original != nullptr)
			cvLib->ReleaseBuffer(ImgBuff_Original);
		if (ImgBuff_CurDisp != nullptr)
			cvLib->ReleaseBuffer(ImgBuff_CurDisp);
		if (ImgBuff_OverlayDisp != nullptr)
			cvLib->ReleaseBuffer(ImgBuff_OverlayDisp);

// 		if (ImgBuff_Original != NULL)
// 			cvReleaseImage(&ImgBuff_Original); //ImgBuffer가 비어있어야 함.  아니면 메모리 계속 쌓인다
// 		cvSetZero(ImgBuff_Original);
		IplImage* pLoadImgBuffer = nullptr;

		m_strCurFolderPath = fileDlg.GetFolderPath();
		m_strCurLoadImgFileName = fileDlg.GetFileName();

		loadFileName = _strdup(CT2A(fileDlg.GetPathName()));
		pLoadImgBuffer = cvLib->LoadImgFile(loadFileName, CV_LOAD_IMAGE_GRAYSCALE);
		
		ImgBuff_Original = cvLib->CreateBuffer(pLoadImgBuffer->width, pLoadImgBuffer->height, pLoadImgBuffer->nChannels);
		ImgBuff_CurDisp = cvLib->CreateBuffer(pLoadImgBuffer->width, pLoadImgBuffer->height, 3);
		ImgBuff_OverlayDisp = cvLib->CreateBuffer(pLoadImgBuffer->width, pLoadImgBuffer->height, 3);

		cvCopy(pLoadImgBuffer, ImgBuff_Original);
	}

	delete loadFileName;
	loadFileName = NULL;

	if (ImgBuff_Original != nullptr)
	{
		if(m_ImgCh == 1)
			cvCvtColor(ImgBuff_Original, ImgBuff_CurDisp, CV_GRAY2BGR);
		else
			cvCopy(ImgBuff_Original, ImgBuff_CurDisp);
	}

	DisplayImage(IDC_STATIC_DISP, ImgBuff_CurDisp, ImgSizeResol);
}


void COpenCV_LibDlg::OnPopMenuImageSave()
{
	char *saveFileName = (char *)NULL;

	CString szFilter = _T(" 지원하는 모든 영상 파일 | *.jpg; *.jpeg; *.jpe; *.png; *.bmp; *.dib; *.sr; *.ras; *.tiff; *.tif ||");
	
	CFileDialog fileDlg(FALSE, _T("jpg"), NULL, NULL/*OFN_OVERWRITEPROMPT*/, szFilter, AfxGetMainWnd());
	if (fileDlg.DoModal() == IDOK) 
	{
		saveFileName = _strdup(CT2A(fileDlg.GetPathName()));
		cvLib->SaveImgFile(saveFileName, ImgBuff_CurDisp,1);
//  		Mat tmpMatImage;
//  		tmpMatImage = cvarrToMat(ImgBuff_CurDisp);
// 
// 		saveFileName = _strdup(CT2A(fileDlg.GetPathName()));
//  		imwrite(string("E:\\111.jpg"), tmpMatImage);
	}

	delete saveFileName;
	saveFileName = NULL;
}


void COpenCV_LibDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (m_bAutoMode)
		return;

// 	if (m_iDisplayNum != MAIN_DISP)
 		return;


	point = ConvertPnt_Click(point);

	CMenu	m_sub_menu;			// Stamp Operation Menu
	m_sub_menu.LoadMenu(IDR_MENU_IMG);
	m_sub_menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x/* + rScreen.left*/, point.y/* + rScreen.top*/, this, NULL);

	CDialogEx::OnRButtonUp(nFlags, point);
}

CvPoint COpenCV_LibDlg::ConvertPnt(CPoint point)
{
	CvPoint szPnt;

	szPnt.x = (int)point.x - DLG2DISP_GAP;
	szPnt.y = (int)point.y - DLG2DISP_GAP;
	szPnt.x = (int)((float)szPnt.x / ImgSizeResol[0]);
	szPnt.y = (int)((float)szPnt.y / ImgSizeResol[1]);

	return szPnt;
}


CPoint COpenCV_LibDlg::ConvertPnt_Click(CPoint point)
{
	CPoint szPnt;

	szPnt.x = (int)point.x + m_iDispStPosX;
	szPnt.y = (int)point.y - m_iDispStPosY + 20;

	return szPnt;
}

void COpenCV_LibDlg::OnBnClickedRadioAreaShape(UINT nID)
{
	UpdateData(TRUE);

	m_RadioAreaShape = m_RadioAreaShape != 0 ? 1 : 0;

	if (m_RadioAreaShape == 0)
		m_bInsideCircle = 0;

// 	if (pView->iLoginStatus == USER_ADMINISTRATOR)
// 	{
// 		TCHAR tmpStr[32];
// 		wsprintf(tmpStr, _T("Select Radio %d Button"), (int&)m_RadioAreaShape);
// 		pView->putListData(tmpStr);
// 	}
}

UINT DisplayThread(LPVOID param)
{
	//라이브 용 - 비전LIB 연결 필요
// 	if (pVisionEx == nullptr)
// 		return 0;
// 
// 	COpenCV_LibDlg* pDispDlg = (COpenCV_LibDlg*)param;
// 
// 	DWORD dwResult = 0;
// 	while (pDispDlg->m_bLiveMode)
// 	{
// 		Sleep(30);
// 
// 		dwResult = WaitForSingleObject(pVisionEx->GetHandleGrabDone(), 100);
// 		if (dwResult == WAIT_OBJECT_0)
// 		{
// 			memcpy(pDispDlg->ImgBuff_Grab->imageData, pVisionEx->m_pbyBuffer, pDispDlg->ImgBuff_Grab->imageSize);
// 			cvCopy(pDispDlg->ImgBuff_Grab, pDispDlg->ImgBuff_CurDisp);
// 
// 			//자동검사 플로어중에는 동작 안하도록...
// 			cvCopy(pDispDlg->ImgBuff_CurDisp, pDispDlg->ImgBuff_Original);
// 
// 			pDispDlg->DisplayImage(IDC_STATIC_DISP, pDispDlg->ImgBuff_CurDisp, pDispDlg->ImgSizeResol);
// 			pVisionEx->ResetHandleGrabDone();
// 		}
// 	}

	return 0;
}


void COpenCV_LibDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (m_iDisplayNum == 0)
		this->SetWindowPos(NULL, m_iDispStPosX, m_iDispStPosY, m_iDispSizeX, m_iDispSizeY, NULL);

	OnPaint();
	Invalidate();
}
#endif