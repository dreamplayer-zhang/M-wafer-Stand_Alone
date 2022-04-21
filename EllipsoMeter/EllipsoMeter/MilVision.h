#pragma once
#if MIL_USE
#define DLL_EXPORT

#include "mil.h"
//! LIB ==========================================================
#pragma comment (lib, "mil.lib")
#pragma comment (lib, "milim.lib")
#pragma comment (lib, "milmod.lib")
#pragma comment (lib, "milblob.lib")
#pragma comment (lib, "milcal.lib")
// #pragma comment (lib, "mil3dmap.lib")
#pragma  comment (lib, "milreg.lib")
#pragma  comment (lib, "milcolor.lib")
#pragma comment (lib, "MilEdge.lib")
//================================================================

#define DEF_MAX_PIXEL_CNT			64000
#define DEF_MAX_LINE_CNT			32000
#define DEF_MAX_BOX_CNT				2000
#define DEF_MAX_TRIANGLE_CNT		2000
#define DEF_MAX_CROSS_CNT			32000
#define DEF_MAX_CIRCLE_CNT			500
#define DEF_MAX_TEXT_CNT			50
#define DEF_MESSAGE_SIZE			256
#define MAX_BLOB_CNT				10000

//! Struct =======================================================
class CDrawPixelElem {
public:
	CPoint p;
	int c;
};

class CDrawPixel {
public:
	CDrawPixelElem elem[DEF_MAX_PIXEL_CNT];
	int cnt;
};

class CDrawLineElem {
public:
	CPoint p[2];
	int c;
};

class CDrawLine {
public:
	CDrawLineElem elem[DEF_MAX_LINE_CNT];
	int cnt;
};

class CDrawBoxElem {
public:
	CRect r;
	int c;
	bool f;
};
class CDrawTriAngleElem {
public:
	CPoint p[3];
	int c;
};
class CDrawTriAngle {
public:
	CDrawTriAngleElem elem[DEF_MAX_TRIANGLE_CNT];
	int cnt;
};
class CDrawBox {
public:
	CDrawBoxElem elem[DEF_MAX_BOX_CNT];
	int cnt;

	CDrawBox()
	{
		cnt = 0;
	}
};

class CDrawCircleElem {
public:
	CRect r;
	int c;
};

class CDrawCircle {
public:
	CDrawCircleElem elem[DEF_MAX_CIRCLE_CNT];
	int cnt;
};

class CDrawCrossElem {
public:
	CPoint p;
	int c;
	int w;
};

class CDrawCross {
public:
	CDrawCrossElem elem[DEF_MAX_CROSS_CNT];
	int cnt;
};

class CDrawTextElem {
public:
	TCHAR szMessage[DEF_MESSAGE_SIZE];
	CString str;
	int c;
	double xFontScale, yFontScale;
	int sx, sy;
};

class CDrawText {
public:
	CDrawTextElem elem[DEF_MAX_TEXT_CNT];
	int cnt;
};
//////////////////////////////////////////////////////////////////////////

typedef enum DISPLAY_NUM_
{
	DEF_DISP_SET = 0,

	MAX_DISP_CNT
}_DISPLAY_NUM;

typedef enum OVERLAY_NUM_
{
	DEF_OVR_SET = 0,
	DEF_OVR_MARK,

	MAX_OVR_CNT
}_OVERLAY_NUM;

class CMilVision
{
public:
	CMilVision();
	~CMilVision();
};

class CVision
{
public:
	CVision();
	virtual ~CVision();
	TCHAR curDirName[MAX_PATH];

	//MIL Val 
	MIL_ID	MilApplicatoin;
	MIL_ID	MilSystem;			//Grab - Solios

								//MIL Control
	void InitMilSystem();
	void CloseMil();
};
#define M_Alloc8(imgSzX, imgSzY, M_In)		MbufAlloc2d( pVision->MilSystem, imgSzX, imgSzY, M_UNSIGNED+8L, M_IMAGE+M_PROC, &M_In );
#define M_Alloc16(imgSzX, imgSzY, M_In)		MbufAlloc2d( pVision->MilSystem, imgSzX, imgSzY, M_SIGNED+16L, M_IMAGE+M_PROC, &M_In );
#define M_AllocColor(imgSzX, imgSzY, M_In)	MbufAllocColor( pVision->MilSystem, 3, imgSzX, imgSzY, M_UNSIGNED+8L, M_IMAGE+M_PROC, &M_In );
#define M_Alloc32F(imgSzX, imgSzY, M_In)		MbufAlloc2d( pVision->MilSystem, imgSzX, imgSzY, 32+M_FLOAT, M_IMAGE+M_PROC, &M_In );


#define DEF_IMAGE_X			2000				//���� ���� ���� ������
#define DEF_IMAGE_Y			2000				//���� ���� ���� ������
#define DEF_ZOOM			3					//���� ��ü ������ View â�� Zoom ����
#define DEF_MARK_SIZE_X		1000					//��ũ�� ���� ������
#define DEF_MARK_SIZE_Y		1000					//��ũ�� ���� ������

class CDisplay
{
public:
	CDisplay(MIL_ID MilSysID, MIL_INT32 imgBand, MIL_INT32 imgXsize, MIL_INT32 imgYsize, MIL_INT32 zoomX, MIL_INT32 zoomY, MIL_INT32 dispNum);
	~CDisplay();

	MIL_ID		m_MilSys;
	int		m_DispNum;
	int		m_ImgBand;
	int		m_ImgXsize;
	int		m_ImgYsize;
	int		m_ZoomX;
	int		m_ZoomY;

	void InitImgBuf();
	void FreeImgBuf();

	MIL_ID	MilBuf_Origin_Image;
	MIL_ID	MilBuf_Set_View;				//��ü���̹���
	MIL_ID	MilBuf_Mark_View;			   //��ũ�̹���(3band)

	void ImageLoad_Browser();
	void ImageSave(int BuffNum /*= -1*/);

	struct structMilOvr
	{
		MIL_ID	MilDisplay;
		CRect rectPic;
		//MIL Overlay
		MIL_ID	MilOverlayImage;				// Overlay image buffer identifier 
		MIL_INT	TransparentColor;			// Keying color
		long	ImageWidth;						// Overlay image size x
		long	ImageHeight;					// Overlay image size y
		BOOL	bOverlayIsInitialized;		// Overlay initialization state
		BOOL	bOverlayIsEnable;			//Overlay show
		unsigned long oc_red, oc_green, oc_blue;

		//Overlay Struct
		CDrawPixel		pixelList;
		CDrawBox		boxList;
		CDrawTriAngle   TriangleList;
		CDrawLine		lineList;
		CDrawCircle		circleList;
		CDrawCross		crossList;
		CDrawText		textList;
	};

	structMilOvr MilOvr[MAX_OVR_CNT];
	// MIL
	void addCrossList(CPoint p, int wdt, int c, int iOvrNum = DEF_OVR_SET, BOOL bZooming = FALSE);
	void addCrossList(int x, int y, int wdt, int c, int iOvrNum = DEF_OVR_SET, BOOL bZooming = FALSE);
	void addLineList(CPoint sl, CPoint el, int c, int iOvrNum = DEF_OVR_SET, BOOL bZooming = FALSE);
	void addPixelList(int x, int y, int c, int iOvrNum = DEF_OVR_SET, BOOL bZooming = FALSE);
	void addPixelList(CPoint p, int c, int iOvrNum = DEF_OVR_SET, BOOL bZooming = FALSE);
	void addBoxList(CRect r, int c, int iOvrNum = DEF_OVR_SET, BOOL bZooming = FALSE);
	void addFillBoxList(CRect r, int c, int iOvrNum = DEF_OVR_SET, BOOL bZooming = FALSE);
	void addTriAngleList(CPoint p1, CPoint p2, CPoint p3, int c, int iOvrNum = DEF_OVR_SET, BOOL bZooming = FALSE);
	void addCircleList(CRect r, int c, int iOvrNum = DEF_OVR_SET, BOOL bZooming = FALSE);
	void addTextList(char* szMessage, double xFontScale, double yFontScale, int sx, int sy, int c, int iOvrNum = DEF_OVR_SET, BOOL bZooming = FALSE);

	void OverlayEnableCam(int iOvrNum = DEF_OVR_SET);
	void drawOverlay(int OvrNum = DEF_OVR_SET);
	void OverlayClear(int iOvrNum = DEF_OVR_SET);

	BOOL CheckInDisp(CPoint point, int iOvrNum = DEF_OVR_SET);
	int	WhichInDisp(CPoint point);
	CPoint ConvertPnt(CPoint point, BOOL iZooming /*= FALSE*/);
	CPoint ConvertPnt_Click(CPoint point, int iOvrNum /*= DEF_OVR_SET*/);
	BOOL CheckInBox(CPoint point, CRect rcBox);


};

class CUserVariant
{
public:
	CUserVariant();

	CUserVariant(int iVal);
	CUserVariant(double dVal);
};

class CMilInspFunc
{
public:
	CMilInspFunc();
	~CMilInspFunc();


	HANDLE hImage_Insp;
	unsigned char *m_ImageInsp;

	void InitImgBuf(int ImgBand = 1);
	void MilImageCheck(MIL_ID MilImage, MIL_ID MilDisp, const char* strValName = NULL);

	MIL_ID MilCalibration[DEF_MAX_GROUP];

	void Calibration(int GroupNum, double XPixArray[], double YPixArray[], double XWorldArray[], double YWorldArray[], int NumPoint);
	void GetCamTransformCoordinate(int GroupNum, double x, double y, double* resX, double* resY, int tansformType = 0);
	void InitCal();
	void CloseCal();
	void SaveCal(LPCSTR nPathStr, int GroupNum = 0);
	void LoadCal(LPCSTR nPathStr, int GroupNum = 0);

	int ErrorCode;//0�̸� ����

	TCHAR	msgVision[512];


	MIL_ID	MilBuf_Insp_Image;				//Cam���� �˻��� ���� �̹��� (2448X2048)
	MIL_ID	MilBuf_Mark[DEF_MAX_MARK_NUM];  //Mark
	MIL_ID	MilBuf_Mask;

	MIL_ID MilImageMod[DEF_MAX_MARK_NUM];		//ModelFind								
	MIL_ID MilResultMod[DEF_MAX_MARK_NUM];		//ModelFind Result
	MIL_ID Milmodgra;
	MIL_ID MilmodResultgra;

	int		m_PitchX;

	CPoint ptrMarkStart, ptrMarkSize;//����ϴ� ��ũ ������
	long	nCount;
	CDPoint cdCenterPos[DEF_MAX_PATT_NO];
	double	fAngle[DEF_MAX_PATT_NO];
	double	fScore[DEF_MAX_PATT_NO];

	void ModInit(MIL_DOUBLE ModColor);
	void ModRegist(int iMarkNo, MIL_DOUBLE ModColor);
	void ModControl(int iMarkNo = 0);
	void ModDntCare(int iMarkNo, MIL_ID MaskImage, MIL_INT* MarkSize);
	void ModLoad(int iMarkNo);
	CDPoint ModFind(MIL_ID InspImage, TCHAR* tmpStr, int iMarkNo = 0);
	void CloseMod();

	long bb_stx[MAX_BLOB_CNT];
	long bb_sty[MAX_BLOB_CNT];
	long bb_edx[MAX_BLOB_CNT];
	long bb_edy[MAX_BLOB_CNT];
	long bb_szx[MAX_BLOB_CNT];
	long bb_szy[MAX_BLOB_CNT];
	long bb_ctx[MAX_BLOB_CNT];
	long bb_cty[MAX_BLOB_CNT];
	long bb_area[MAX_BLOB_CNT];

	struct SEGMENT_PARA  //region segment parameter STRUCT
	{
		int minX[MAX_BLOB_CNT],
			minY[MAX_BLOB_CNT],
			maxX[MAX_BLOB_CNT],
			maxY[MAX_BLOB_CNT],
			sizeX[MAX_BLOB_CNT],
			sizeY[MAX_BLOB_CNT],
			center[MAX_BLOB_CNT][3], //x,y,z
			pixelCnt[MAX_BLOB_CNT];
		int min_area, max_area;
	};
	SEGMENT_PARA sp;

	BOOL BlobBlack(MIL_ID Inp);
	BOOL SearchEdge(int SearchXPnt, int SearchYPnt, double resEdgePnt[3]);

};

extern CVision*			pVision;
extern CDisplay*		pDisplay[MAX_DISP_CNT];
extern CMilInspFunc*	pInsp[MAX_DISP_CNT];


//���α׷� ������ �ݵ�� ����.
extern /*"C" __declspec(dllimport)*/ void InitDLL();

//���������ϵ�� �Լ�(�������� �Ǵ� �𼭸������� ���� ���߾ӿ� ������ �ؾ���)
//*ImageBuff�� ImageBuff[2448*2048] �̿��� ��
//PatternNum �� 0�� ���� ����� ���� ���ĺ��ʹ� 1~19���� ���� ���� ���
extern /*"C" __declspec(dllexport)*/ void SetMasterPattern(unsigned char* ImageBuff, int PatternNum = 0);

//���� ����� �Լ�(NumPoint�� 9�������� ȿ���� ����. ��, 3��or4������� �Ұ��� �Ѱ��� �ƴϴ�.)
//*ImageBuff�� ImageBuff[NumPoint][2448*2048] �̿��� ��
extern /*"C" __declspec(dllimport)*/ void SetMotorAlign(int GroupNum, double XPixArray[], double YPixArray[], double XWorldArray[], double YWorldArray[], unsigned char ImageBuff[][DEF_IMAGE_X*DEF_IMAGE_Y], int NumPoint);

//������۸� �Է��ϰ� ��Ī�� ���Ϲ�ȣ�� �������ָ� ���ϼ����� �������� ���Ͱ����� ��ȯ�Ͽ� ������ش�.
//*ImageBuff�� ImageBuff[2448*2048] �̿��� ��
//PatternNum �� 0~19���� ����� �̹��� ���������� ��� //�����̹��� 0�� ���۴� ������ ���Ϲ��۷� ���������.
extern /*"C" __declspec(dllexport)*/ CDPoint GetPattAlignCenter(unsigned char * ImageBuff, int PatternNum = 0);

//�̻���ǥ->������ǥ ��ȯ�Լ� (x,y�� �������Է� ��, resX,resY�� ���Ͱ� ��µ�.)
extern /*"C" __declspec(dllimport)*/ void GetTransformCoordinate(int GroupNum, double x, double y, double* resX, double* resY, int tansformType = 0);

//���α׷� ����� �ݵ�� ����.
extern /*"C" __declspec(dllimport)*/ void CloseDLL();
#endif