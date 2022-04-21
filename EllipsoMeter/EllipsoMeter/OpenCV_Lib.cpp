
// OpenCV_Lib.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
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


// COpenCV_LibApp ����

COpenCV_LibApp::COpenCV_LibApp(int iImgSzX, int iImgSzY)
{
	m_iImageSizeX = iImgSzX;
	m_iImageSizeY = iImgSzY;
}


COpenCV_LibApp::~COpenCV_LibApp()
{
}

// ������ COpenCV_LibApp ��ü�Դϴ�.

COpenCV_LibApp *cvLib;


// COpenCV_LibApp �ʱ�ȭ




// BOOL COpenCV_LibApp::InitInstance()
// {
// 	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
// 	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControlsEx()�� �ʿ��մϴ�.
// 	// InitCommonControlsEx()�� ������� ������ â�� ���� �� �����ϴ�.
// 	INITCOMMONCONTROLSEX InitCtrls;
// 	InitCtrls.dwSize = sizeof(InitCtrls);
// 	// ���� ���α׷����� ����� ��� ���� ��Ʈ�� Ŭ������ �����ϵ���
// 	// �� �׸��� �����Ͻʽÿ�.
// 	InitCtrls.dwICC = ICC_WIN95_CLASSES;
// 	InitCommonControlsEx(&InitCtrls);
// 
// 	CWinApp::InitInstance();
// 
// 
// 	AfxEnableControlContainer();
// 
// 	// ��ȭ ���ڿ� �� Ʈ�� �� �Ǵ�
// 	// �� ��� �� ��Ʈ���� ���ԵǾ� �ִ� ��� �� �����ڸ� ����ϴ�.
// 	CShellManager *pShellManager = new CShellManager;
// 
// 	// MFC ��Ʈ���� �׸��� ����ϱ� ���� "Windows ����" ���־� ������ Ȱ��ȭ
// 	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
// 
// 	// ǥ�� �ʱ�ȭ
// 	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
// 	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
// 	// ��ƾ�� �����ؾ� �մϴ�.
// 	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
// 	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
// 	// ������ �������� �����ؾ� �մϴ�.
// 	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));
// 
// 	COpenCV_LibDlg dlg;
// 	m_pMainWnd = &dlg;
// 	INT_PTR nResponse = dlg.DoModal();
// 	if (nResponse == IDOK)
// 	{
// 		// TODO: ���⿡ [Ȯ��]�� Ŭ���Ͽ� ��ȭ ���ڰ� ������ �� ó����
// 		//  �ڵ带 ��ġ�մϴ�.
// 	}
// 	else if (nResponse == IDCANCEL)
// 	{
// 		// TODO: ���⿡ [���]�� Ŭ���Ͽ� ��ȭ ���ڰ� ������ �� ó����
// 		//  �ڵ带 ��ġ�մϴ�.
// 	}
// 	else if (nResponse == -1)
// 	{
// 		TRACE(traceAppMsg, 0, "���: ��ȭ ���ڸ� ������ �������Ƿ� ���� ���α׷��� ����ġ �ʰ� ����˴ϴ�.\n");
// 		TRACE(traceAppMsg, 0, "���: ��ȭ ���ڿ��� MFC ��Ʈ���� ����ϴ� ��� #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS�� ������ �� �����ϴ�.\n");
// 	}
// 
// 	// ������ ���� �� �����ڸ� �����մϴ�.
// 	if (pShellManager != NULL)
// 	{
// 		delete pShellManager;
// 	}
// 
// 	// ��ȭ ���ڰ� �������Ƿ� ���� ���α׷��� �޽��� ������ �������� �ʰ�  ���� ���α׷��� ���� �� �ֵ��� FALSE��
// 	// ��ȯ�մϴ�.
// 	return FALSE;
// }

/** @brief
@description
IplImage Buffer ���� �Լ�

@return
���� Buffer

@param
int nSizeW : ���� ���� ������
int nSizeH : ���� ���� ������
int nChannels : ���� ä�� ��(�⺻ 3ä�� ����)
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
IplImage Buffer ���� �Լ�

@return
Error Message - "ERR_NONE"�� ���� ���

@param
IplImage* srcImg : ����

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
// 		Mat *tmpMatImage;						//���ᶧ �޸� �� �߻�.
// 		tmpMatImage = (Mat*)new Mat;
// 
// 		*tmpMatImage = cvarrToMat(srcImg, true);

		Mat tmpMatImage = cvarrToMat(srcImg, true);

// 		vector<int> param_jpg, param_png, param_bmp;		//vector�̶�� int�� ���� ����. ���� ��Ҹ� �߰������� �� ����. �迭���� ��.
// 		param_jpg.push_back(cv::IMWRITE_JPEG_QUALITY);		//JPEG ���Ϸ� �����ϱ� ����
// 		param_jpg.push_back(50);							//���� ���ϼ��� ȭ�� ����.�뷮 ����. 0~100. defalut ���� 95)
// 		param_png.push_back(cv::IMWRITE_PNG_COMPRESSION);	//PNG ���Ϸ� �����ϱ� ����
// 		param_png.push_back(1);								//PNG ����� (0~9. default ���� 3). ���� �� �־ ��� �뷮���� ������


#ifdef _DEBUG
		imwrite((cv::String)(pathImg), tmpMatImage);// , param_jpg);
#endif

// 		delete tmpMatImage;
// 		tmpMatImage = nullptr;


// 		nRet = cvSaveImage(pathImg, srcImg); //�����ڵ忡�� ���������鼭 ���α׷� ����.

		nRet = ERR_NONE;
	}
	return nRet;

}

/** @brief

@description
Marker Draw �Լ�

@return


@param
IplImage* iplDrawImg : ��� ����
int nCvMarkerType : �׸� ������ ����
DRAW_TYPE_POINT : ��
DRAW_TYPE_LINE : ��
DRAW_TYPE_RECT : �簢��
DRAW_TYPE_CIRCLE : ��
DRAW_TYPE_POLYGON : �ٰ���

CvPoint* ptPoint : �׷����� �� ������( �� : 1��, �� : 2��(������, ����), �簢�� : 2��(������, ����), �� : 1��(�߽���), �ٰ��� : N��)
CvScalar color : ���� ����
int nTickness : ���� �β�

int nPtCnt: �� �������� ����(�⺻ 1�� ����)
int nRadius : �ݰ�(���ڼ�,���� �׸� ���� �Է�)

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

	CvPoint2D32f    centralPoint = cvPoint2D32f((int)(srcImg->width / 2), (int)(srcImg->height / 2));            // ȸ�� ������ ����(�̹����� �߽���)
	CvMat*            rotationMatrix = cvCreateMat(2, 3, CV_32FC1);                        // ȸ�� ���� ���

																						   // Rotation ���� ��� ���� �� ����(90������ ������ ������ ���� �����̹���(�ʿ�� ����))
	cv2DRotationMatrix(centralPoint, angle, 1, rotationMatrix);

	// Image Rotation
	cvWarpAffine(srcImg, dstImg, rotationMatrix, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS);

	// Memory ����
	cvReleaseMat(&rotationMatrix);
}
#endif