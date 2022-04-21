
// OpenCV_Lib.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#if OPENCV
#include "OpenCV_Lib.h"
#include "OpenCV_LibDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// COpenCV_LibApp

// BEGIN_MESSAGE_MAP(COpenCV_LibApp, CWinApp)
// 	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
// END_MESSAGE_MAP()


// COpenCV_LibApp 생성

COpenCV_LibApp::COpenCV_LibApp(int iImgSzX, int iImgSzY)
{
	m_iImageSizeX = iImgSzX;
	m_iImageSizeY = iImgSzY;
}


COpenCV_LibApp::~COpenCV_LibApp()
{
}

// 유일한 COpenCV_LibApp 개체입니다.

COpenCV_LibApp *cvLib;


// COpenCV_LibApp 초기화




// BOOL COpenCV_LibApp::InitInstance()
// {
// 	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
// 	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다.
// 	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
// 	INITCOMMONCONTROLSEX InitCtrls;
// 	InitCtrls.dwSize = sizeof(InitCtrls);
// 	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
// 	// 이 항목을 설정하십시오.
// 	InitCtrls.dwICC = ICC_WIN95_CLASSES;
// 	InitCommonControlsEx(&InitCtrls);
// 
// 	CWinApp::InitInstance();
// 
// 
// 	AfxEnableControlContainer();
// 
// 	// 대화 상자에 셸 트리 뷰 또는
// 	// 셸 목록 뷰 컨트롤이 포함되어 있는 경우 셸 관리자를 만듭니다.
// 	CShellManager *pShellManager = new CShellManager;
// 
// 	// MFC 컨트롤의 테마를 사용하기 위해 "Windows 원형" 비주얼 관리자 활성화
// 	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
// 
// 	// 표준 초기화
// 	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
// 	// 아래에서 필요 없는 특정 초기화
// 	// 루틴을 제거해야 합니다.
// 	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
// 	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
// 	// 적절한 내용으로 수정해야 합니다.
// 	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));
// 
// 	COpenCV_LibDlg dlg;
// 	m_pMainWnd = &dlg;
// 	INT_PTR nResponse = dlg.DoModal();
// 	if (nResponse == IDOK)
// 	{
// 		// TODO: 여기에 [확인]을 클릭하여 대화 상자가 없어질 때 처리할
// 		//  코드를 배치합니다.
// 	}
// 	else if (nResponse == IDCANCEL)
// 	{
// 		// TODO: 여기에 [취소]를 클릭하여 대화 상자가 없어질 때 처리할
// 		//  코드를 배치합니다.
// 	}
// 	else if (nResponse == -1)
// 	{
// 		TRACE(traceAppMsg, 0, "경고: 대화 상자를 만들지 못했으므로 응용 프로그램이 예기치 않게 종료됩니다.\n");
// 		TRACE(traceAppMsg, 0, "경고: 대화 상자에서 MFC 컨트롤을 사용하는 경우 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS를 수행할 수 없습니다.\n");
// 	}
// 
// 	// 위에서 만든 셸 관리자를 삭제합니다.
// 	if (pShellManager != NULL)
// 	{
// 		delete pShellManager;
// 	}
// 
// 	// 대화 상자가 닫혔으므로 응용 프로그램의 메시지 펌프를 시작하지 않고  응용 프로그램을 끝낼 수 있도록 FALSE를
// 	// 반환합니다.
// 	return FALSE;
// }

/** @brief
@description
IplImage Buffer 생성 함수

@return
영상 Buffer

@param
int nSizeW : 영상 가로 사이즈
int nSizeH : 영상 세로 사이즈
int nChannels : 영상 채널 수(기본 3채널 지정)
*/
IplImage* COpenCV_LibApp::CreateBuffer(int nSizeW, int nSizeH, int nChannels)
{
	IplImage* rstImg;

	rstImg = cvCreateImage(cvSize(nSizeW, nSizeH), IPL_DEPTH_8U, nChannels);
	cvSetZero(rstImg);

	return rstImg;
}

/** @brief

@description
IplImage Buffer 해제 함수

@return
Error Message - "ERR_NONE"이 정상 출력

@param
IplImage* srcImg : 영상

*/
int COpenCV_LibApp::ReleaseBuffer(IplImage* pImgBuffer)
{
	int nRet = ERR_INIT;

	if (pImgBuffer != NULL)
	{
		cvReleaseImage(&pImgBuffer);
		pImgBuffer = NULL;

		nRet = ERR_NONE;
	}
	else
	{
		nRet = ERR_BUFF_EMPTY;
	}

	return nRet;
}

IplImage* COpenCV_LibApp::LoadImgFile(LPSTR pathImg, int nChannels /*= 1*/)
{
	IplImage* pImgBuffer = nullptr;

	if (nChannels == CV_LOAD_IMAGE_GRAYSCALE)
	{
		pImgBuffer = cvLoadImage(pathImg, CV_LOAD_IMAGE_GRAYSCALE);
	}
	else if (nChannels == CV_LOAD_IMAGE_COLOR)
	{
		pImgBuffer = cvLoadImage(pathImg, CV_LOAD_IMAGE_COLOR);
	}

	return pImgBuffer;
}

int COpenCV_LibApp::SaveImgFile(LPSTR pathImg, IplImage* srcImg, int nChannels /*= 1*/)
{
// 	return 0;

	int nRet = ERR_INIT;

	if (srcImg == NULL)
	{
		nRet = ERR_BUFF_EMPTY;
	}
	else
	{
// 		Mat *tmpMatImage;						//종료때 메모리 릭 발생.
// 		tmpMatImage = (Mat*)new Mat;
// 
// 		*tmpMatImage = cvarrToMat(srcImg, true);

		Mat tmpMatImage = cvarrToMat(srcImg, true);

// 		vector<int> param_jpg, param_png, param_bmp;		//vector이라는 int형 변수 선언. 여러 요소를 추가삽입할 수 있음. 배열같은 것.
// 		param_jpg.push_back(cv::IMWRITE_JPEG_QUALITY);		//JPEG 파일로 저장하기 위해
// 		param_jpg.push_back(50);							//높은 값일수록 화질 좋음.용량 높음. 0~100. defalut 값은 95)
// 		param_png.push_back(cv::IMWRITE_PNG_COMPRESSION);	//PNG 파일로 저장하기 위해
// 		param_png.push_back(1);								//PNG 압축률 (0~9. default 값은 3). 여러 값 넣어본 결과 용량차이 없었음


#ifdef _DEBUG
		imwrite((cv::String)(pathImg), tmpMatImage);// , param_jpg);
#endif

// 		delete tmpMatImage;
// 		tmpMatImage = nullptr;


// 		nRet = cvSaveImage(pathImg, srcImg); //유니코드에서 오류뱉으면서 프로그램 터짐.

		nRet = ERR_NONE;
	}
	return nRet;

}

/** @brief

@description
Marker Draw 함수

@return


@param
IplImage* iplDrawImg : 출력 영상
int nCvMarkerType : 그릴 도형의 유형
DRAW_TYPE_POINT : 점
DRAW_TYPE_LINE : 선
DRAW_TYPE_RECT : 사각형
DRAW_TYPE_CIRCLE : 원
DRAW_TYPE_POLYGON : 다각형

CvPoint* ptPoint : 그려야할 점 데이터( 점 : 1개, 선 : 2개(시작점, 끝점), 사각형 : 2개(시작점, 끝점), 원 : 1개(중심점), 다각형 : N개)
CvScalar color : 선의 색깔
int nTickness : 선의 두께

int nPtCnt: 점 데이터의 개수(기본 1로 지정)
int nRadius : 반경(십자선,원을 그릴 때만 입력)

*/
void COpenCV_LibApp::DoDrawOverlay(IplImage* iplDrawImg, int nCvMarkerType, CvPoint* ptPoint, CvScalar color, int nTickness, int nPtCnt /*= 1*/, int nRadius /*= 0*/)
{
	if (iplDrawImg == NULL)
	{
		return;
	}
	for (int nFori = 0; nFori < nPtCnt; nFori++)
	{
		if (ptPoint[nFori].x < 0 || ptPoint[nFori].x > iplDrawImg->width ||
			ptPoint[nFori].y < 0 || ptPoint[nFori].y > iplDrawImg->height)
		{
			return;
		}
	}

	switch (nCvMarkerType)
	{
	case DRAW_TYPE_POINT:
	{
		if (nPtCnt != 1)
		{
			return;
		}
		cvLine(iplDrawImg, ptPoint[0], ptPoint[0], color, nTickness);
		//cvCircle(iplDrawImg, ptPoint[0], nRadius, color, nTickness);
	}break;
	case DRAW_TYPE_CROSS:
	{
		if (nPtCnt != 1)
		{
			return;
		}
		CvPoint tempP1, tempP2;
		tempP1.x = ptPoint[0].x - nRadius;
		tempP1.y = ptPoint[0].y - nRadius;
		tempP2.x = ptPoint[0].x + nRadius;
		tempP2.y = ptPoint[0].y + nRadius;
		cvLine(iplDrawImg, tempP1, tempP2, color, nTickness);
		tempP1.x = ptPoint[0].x + nRadius;
		tempP1.y = ptPoint[0].y - nRadius;
		tempP2.x = ptPoint[0].x - nRadius;
		tempP2.y = ptPoint[0].y + nRadius;
		cvLine(iplDrawImg, tempP1, tempP2, color, nTickness);
	}break;
	case DRAW_TYPE_LINE:
	{
		if (nPtCnt != 2)
		{
			return;
		}
		cvLine(iplDrawImg, ptPoint[0], ptPoint[1], color, nTickness);
	}break;
	case DRAW_TYPE_RECT:
	{
		int nTemp_X = 0;
		int nTemp_Y = 0;

		if (ptPoint[0].x > ptPoint[1].x)
		{
			return;
			nTemp_X = ptPoint[0].x;
			ptPoint[0].x = ptPoint[1].x;
			ptPoint[1].x = nTemp_X;
		}
		if (ptPoint[0].y > ptPoint[1].y)
		{
			return;
			nTemp_Y = ptPoint[0].y;
			ptPoint[0].y = ptPoint[1].y;
			ptPoint[1].y = nTemp_Y;
		}
		if (nPtCnt != 2)
		{
			return;
		}
		CvRect rRect = cvRect(ptPoint[0].x, ptPoint[0].y, ptPoint[1].x - ptPoint[0].x, ptPoint[1].y - ptPoint[0].y);

		cvRectangleR(iplDrawImg, rRect, color, nTickness);
	}break;
	case DRAW_TYPE_CIRCLE:
	{
		if (nPtCnt != 1 || nRadius<0 || nRadius > m_iImageSizeX || nRadius >m_iImageSizeY)
		{
			return;
		}
		cvCircle(iplDrawImg, ptPoint[0], nRadius, color, nTickness);
	}break;
	case DRAW_TYPE_POLYGON:
	{
		if (nPtCnt < 3)
		{
			return;
		}

		for (int nFori = 1; nFori <= nPtCnt; nFori++)
		{
			if (nFori == nPtCnt)
			{
				cvLine(iplDrawImg, ptPoint[nFori - 1], ptPoint[0], color, nTickness);
			}
			else
			{
				cvLine(iplDrawImg, ptPoint[nFori], ptPoint[nFori - 1], color, nTickness);
			}
		}
	}break;

	default:
		break;
	}
}

void COpenCV_LibApp::ImgBuffCnvt_ipl2char(IplImage* srcImg, unsigned char * dstImg, int imgBand /*= 3*/)
{
	if (srcImg->nChannels == 1 && imgBand == 3)
	{
		TECmsgOut(L"src-dst Image Bane Parameter Check!!!", RED_1, 1);
		return;
	}

	int i, j;
	int pos, pos2;
	for (j = 0; j < srcImg->height; j++)
	{
		for (i = 0; i < srcImg->width; i++)
		{
			pos = j * srcImg->widthStep + i * srcImg->nChannels;
			pos2 = (j * srcImg->width + i )* srcImg->nChannels;

			if(imgBand == 3)
			{
				dstImg[pos2 + 2] = srcImg->imageData[pos + 2];
				dstImg[pos2 + 1] = srcImg->imageData[pos + 1];
			}
			dstImg[pos2 + 0] = srcImg->imageData[pos + 0];
		}
	}

	return;
}

void COpenCV_LibApp::ImgBuffCnvt_char2ipl(unsigned char * srcImg, IplImage * dstImg)
{
	memset(srcImg, 0x00, sizeof(srcImg));
	int i, j;
	int pos, pos2;
	for (j = 0; j < dstImg->height; j++)
	{
		for (i = 0; i < dstImg->width; i++)
		{
			pos = j * dstImg->widthStep + i * dstImg->nChannels;
			pos2 = (j * dstImg->width + i)* dstImg->nChannels;
			dstImg->imageData[pos + 2] = srcImg[pos2 + 2];
			dstImg->imageData[pos + 1] = srcImg[pos2 + 1];
			dstImg->imageData[pos + 0] = srcImg[pos2 + 0];
		}
	}

	return;
}

void COpenCV_LibApp::Rotate(IplImage* srcImg, IplImage * dstImg, double angle)
{
	// 		// Only 1-Channel
	// 		if (src->nChannels != 1)
	// 			return;

	CvPoint2D32f    centralPoint = cvPoint2D32f((int)(srcImg->width / 2), (int)(srcImg->height / 2));            // 회전 기준점 설정(이미지의 중심점)
	CvMat*            rotationMatrix = cvCreateMat(2, 3, CV_32FC1);                        // 회전 기준 행렬

																						   // Rotation 기준 행렬 연산 및 저장(90도에서 기울어진 각도를 빼야 본래이미지(필요시 수정))
	cv2DRotationMatrix(centralPoint, angle, 1, rotationMatrix);

	// Image Rotation
	cvWarpAffine(srcImg, dstImg, rotationMatrix, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS);

	// Memory 해제
	cvReleaseMat(&rotationMatrix);
}
#endif