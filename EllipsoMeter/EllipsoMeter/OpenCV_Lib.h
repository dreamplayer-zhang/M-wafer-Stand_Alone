
// OpenCV_Lib.h : PROJECT_NAME 응용 프로그램에 대한 주 헤더 파일입니다.
//

#pragma once

// #ifndef __AFXWIN_H__
// 	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
// #endif

// #include "resource.h"		// 주 기호입니다.
#if OPENCV
// #include <iostream>
#include <string>

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

// COpenCV
using namespace cv;
using namespace std;

#define		IMG_CH_BW		1
#define		IMG_CH_COLOR	3

#define		ERR_INIT			0
#define		ERR_NONE			1
#define		ERR_BUFF_EMPTY		2

#define   CV_BLACK					cv::Scalar(0,		0,		0)
#define   CV_BLUE					cv::Scalar(128,	0,		0)
#define   CV_GREEN					cv::Scalar(0,		128,	0)
#define   CV_CYAN					cv::Scalar(128,	128,	0)
#define   CV_RED					cv::Scalar(0,		0,		128)
#define   CV_MAGENTA				cv::Scalar(128,	0,		128)
#define   CV_BROWN					cv::Scalar(0,		128,	128)
#define   CV_DARKGRAY				cv::Scalar(128,	128,	128)
#define   CV_LIGHTGRAY				cv::Scalar(192,	192,	192)
#define   CV_LIGHTBLUE				cv::Scalar(255,	0,		0)
#define   CV_LIGHTGREEN				cv::Scalar(0,		255,	0)
#define   CV_LIGHTCYAN				cv::Scalar(255,	255,	0)
#define   CV_LIGHTRED				cv::Scalar(0,		0,		255)
#define   CV_LIGHTMAGENTA			cv::Scalar(255,	0,		255)
#define   CV_YELLOW					cv::Scalar(0,		255,	255)
#define   CV_WHITE					cv::Scalar(255,	255,	255)
#define   CV_DLGCOLOR				cv::Scalar(240,	240,	240)
#define   CV_ORANGE					cv::Scalar(50,	50,		150)
#define   CV_LIGHTORANGE			cv::Scalar(150,	150,	255)

enum DRAW_TYPE
{
	DRAW_TYPE_POINT=0,
	DRAW_TYPE_CROSS,
	DRAW_TYPE_LINE,
	DRAW_TYPE_RECT,
	DRAW_TYPE_CIRCLE,
	DRAW_TYPE_POLYGON
};

// COpenCV_LibApp:
// 이 클래스의 구현에 대해서는 OpenCV_Lib.cpp을 참조하십시오.
//

class COpenCV_LibApp/* : public CWinApp*/
{
public:
	COpenCV_LibApp(int iImgSzX, int iImgSzY);
	virtual ~COpenCV_LibApp();

	int m_iImageSizeX;
	int m_iImageSizeY;
// 재정의입니다.
public:
// 	virtual BOOL InitInstance();
	IplImage*	CreateBuffer(int nSizeW, int nSizeH, int nChannels = IMG_CH_BW);
	int ReleaseBuffer(IplImage* pImgBuffer);
// 	IplImage* LoadImgFile(char* pathImg, int nChannels = 1);
	IplImage* LoadImgFile(LPSTR pathImg, int nChannels /*= 1*/);
// 	int SaveImgFile(char* pathImg, IplImage* srcImg, int nChannels = 1);
	int SaveImgFile(LPSTR pathImg, IplImage* srcImg, int nChannels /*= 1*/);

	void DoDrawOverlay(IplImage* iplDrawImg, int nCvMarkerType, CvPoint* ptPoint, CvScalar color, int nTickness, int nPtCnt = 1, int nRadius = 0);

	void ImgBuffCnvt_ipl2char(IplImage* pImgBuff, unsigned char * dstImg, int imgBand = 3);
	void ImgBuffCnvt_char2ipl(unsigned char * srcImg, IplImage * dstImg);

	void Rotate(IplImage* srcImg, IplImage * dstImg, double angle);
// 구현입니다.

// 	DECLARE_MESSAGE_MAP()
};

extern COpenCV_LibApp *cvLib;
#endif