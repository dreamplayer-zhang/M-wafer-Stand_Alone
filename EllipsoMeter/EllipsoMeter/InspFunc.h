#pragma once
#if OPENCV
#include "OpenCV_Lib.h"
#include "OpenCV_LibDlg.h"
#include <math.h>


#define ON  1 // merging 
#define OFF 0

#define DEF_IMAGE_X		1080
#define DEF_IMAGE_Y		1080

#define		DEF_IMG_RESOL	6.6755 //9.8039	//3.45/0.345
#define		DEF_INCH_RESOL	25.4

#define		PI			3.1415926535897



struct SEGMENT_PARA  //region segment parameter STRUCT
{
	int n_cand;     	//how many region to find
	int min_area, max_area;
	int center[MAX_PNT_NO + 5][3]; //center coordinate of found regions
	int minX[MAX_PNT_NO + 5], maxX[MAX_PNT_NO + 5], minY[MAX_PNT_NO + 5], maxY[MAX_PNT_NO + 5];
	int sizeX[MAX_PNT_NO + 5], sizeY[MAX_PNT_NO + 5];
	int n_region;   	//no of regions found
	int RGB, pixelCnt;
};

class CResult
{
public:
	// 		CDPoint leftEdgeCut[18][1][MAX_PNT_NO][2];
	// 		int    leftEdgeCutCnt[18][1][2];
	double sigmaValue;
	double lineValue[20][8];
	double lineValueBodyGap[20][8];			// ¹Ùµð±úÁü ³ôÀÌÂ÷ °Ë»ç

};

class CDPoint :public CPoint
{
public:
	double x, y;
	CDPoint()
	{
		x = y = 6.5;  // Camera Resolution
	}
};


class CInspFunc
{
public:
	CInspFunc(void);
	~CInspFunc(void);

	IplImage* m_InspImage;

	int seg_x[MAX_PNT_NO];  // x_axis
	int seg_y[MAX_PNT_NO];  // y_axis
	int seg_c[MAX_PNT_NO];  // total pixel number
	unsigned char seg_f[MAX_PNT_NO];

	int rgn_cx[MAX_PNT_NO];
	int rgn_cy[MAX_PNT_NO];	
	int rgn_sx[MAX_PNT_NO];
	int rgn_ex[MAX_PNT_NO];	
	int rgn_sy[MAX_PNT_NO];
	int rgn_ey[MAX_PNT_NO];
	unsigned long rgn_cnt[MAX_PNT_NO];

	int l_snseg[MAX_PNT_NO];
	int l_nseg[MAX_PNT_NO];
	SEGMENT_PARA sp;
	int RegionSegmentation(unsigned char* pBuf_TargetImg, int BufSizeX, int BufSizeY, CRect* r, int minTh, int maxTh, int minlimitSizeX, int minlimitSizeY, int scanPixel, int drawFlag, COpenCV_LibDlg* pTmpDlg);
	int CheckOverlap(int s1, int n1, int s2, int n2);



	CResult* result;
	double gradientAverage(int areaNo, POINT Point[MAX_PNT_NO], int cnt, int direction);
	


	CDPoint resEdgePoint[MAX_INSP_NUM][DEF_IMAGE_X];
	int resEdgeCnt[MAX_INSP_NUM];
	CRect resPartRect;
	int SearchEdge(int imgeBand, int imgSizeX, int imgSizeY, int colorIndex, unsigned char *ImageBuffer, CRect r, int area, int diffTh, int diffPixel, int val, int valDir, int scandir, int overlayColor, float sigmaSTD);

	double lineValue[MAX_INSP_NUM][10];
	double LinegradientAverage_Double(int imgSizeX, int imgSizeY, CDPoint Point[], int cnt, int direction, int areaNo, float sigmaSTD, double* gradient, double* yValue, BOOL nOverlay=FALSE);
	CDPoint Sigma_Point[MAX_INSP_NUM][DEF_IMAGE_X];
	int Sigma_PointCnt[MAX_INSP_NUM];
	double Parabola_gradientAverage_Double(int imgSizeX, int imgSizeY, CDPoint Point[], int cnt, int direction, int areaNo, float sigmaSTD, bool m_bOverlay = FALSE);

	BOOL AlignWafer(unsigned char *ImageBuffer, int imgSizeX, int imgSizeY, int NochKind, double WaferSize, IplImage** resImg, double *dDegree);
};

extern CInspFunc* pInsp;
#endif