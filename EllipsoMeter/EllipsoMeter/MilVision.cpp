#include "stdafx.h"
#if MIL_USE
#include "MilVision.h"
#include "MilVisionDlg.h"
#include "ModelData.h"
#include "Task.h"


CVision*		pVision;
CDisplay*		pDisplay[MAX_DISP_CNT];
CMilInspFunc*	pInsp[MAX_DISP_CNT];

CMilVision::CMilVision()
{
}


CMilVision::~CMilVision()
{
}


CVision::CVision()
{
	sprintf_s(curDirName, sizeof(curDirName), _T("%s%s"), m_fnGetCurrentDrive().GetString(), DEFAULT_PATH);

	InitMilSystem();
}
CVision::~CVision()
{
	CloseMil();
}

void CVision::InitMilSystem()
{
	MappAlloc(M_DEFAULT, &MilApplicatoin);
#if !_DEBUG
	MappControl(M_ERROR, M_PRINT_DISABLE);
#endif

	TCHAR* SystemTypePTR;
	SystemTypePTR = M_SYSTEM_VGA;

	MsysAlloc(SystemTypePTR, M_DEV0, M_DEFAULT, &MilSystem);
}

void CVision::CloseMil()
{
	if (MilSystem)
	{
		MsysFree(MilSystem);
		MilSystem = M_NULL;
	}

	if (MilApplicatoin)
	{
		MappFree(MilApplicatoin);
		MilApplicatoin = M_NULL;
	}
}


CDisplay::CDisplay(MIL_ID MilSysID, MIL_INT32 imgBand, MIL_INT32 imgXsize, MIL_INT32 imgYsize, MIL_INT32 zoomX, MIL_INT32 zoomY, MIL_INT32 dispNum)
{
	m_MilSys = MilSysID;
	m_ImgBand = imgBand;
	m_ImgXsize = imgXsize;
	m_ImgYsize = imgYsize;
	m_ZoomX = zoomX;
	m_ZoomY = zoomY;
	m_DispNum = dispNum;

	int i = 0;
	for (i = 0; i < MAX_OVR_CNT; i++)
	{
		MilOvr[i].MilDisplay = M_NULL;
		MilOvr[i].bOverlayIsInitialized = FALSE;
		MilOvr[i].bOverlayIsEnable = FALSE;

		MilOvr[i].pixelList.cnt =
			MilOvr[i].crossList.cnt =
			MilOvr[i].lineList.cnt =
			MilOvr[i].boxList.cnt =
			MilOvr[i].circleList.cnt =
			MilOvr[i].textList.cnt = 0;
	}

	InitImgBuf();
}

CDisplay::~CDisplay()
{
	FreeImgBuf();
}

void CDisplay::InitImgBuf()
{
	long BufferAttributes;
	BufferAttributes = M_IMAGE + M_DISP + M_PROC;
	if (m_ImgBand == 3)
	{//컬러disp일때 모노버퍼부터 Alloc
		BufferAttributes += M_PACKED + M_BGR24;
	}

	//Image Buffer Alloc
	MbufAllocColor(m_MilSys, m_ImgBand, m_ImgXsize, m_ImgYsize, 8 + M_UNSIGNED, BufferAttributes, &MilBuf_Origin_Image);										//Top 검사할 실제 원본 이미지
	MbufAllocColor(m_MilSys, m_ImgBand, m_ImgXsize, m_ImgYsize, 8 + M_UNSIGNED, BufferAttributes, &MilBuf_Set_View);						//Top 선택 영역 이미지

	MbufClear(MilBuf_Origin_Image, 0);
	MbufClear(MilBuf_Set_View, 0);

	MbufAllocColor(m_MilSys, m_ImgBand, DEF_MARK_SIZE_X, DEF_MARK_SIZE_Y, 8 + M_UNSIGNED, BufferAttributes, &MilBuf_Mark_View);												//Top 선택 영역 이미지
	MbufClear(MilBuf_Mark_View, 0);

}
void CDisplay::FreeImgBuf()
{
	int i = 0;

	//Display free
	for (i = MAX_OVR_CNT - 1; i >= 0; i--)
	{
		if (MilOvr[i].MilDisplay!=M_NULL)
		{
			MdispFree(MilOvr[i].MilDisplay);
// 			MilOvr[i].MilDisplay = M_NULL;
		}
	}

	if (MilBuf_Mark_View)
	{
		MbufFree(MilBuf_Mark_View);
		MilBuf_Mark_View = M_NULL;
	}
	if (MilBuf_Set_View)
	{
		MbufFree(MilBuf_Set_View);
		MilBuf_Set_View = M_NULL;
	}
	if (MilBuf_Origin_Image)
	{
		MbufFree(MilBuf_Origin_Image);
		MilBuf_Origin_Image = M_NULL;
	}

}

void CDisplay::OverlayEnableCam(int iOvrNum /*= DEF_OVR_SET*/)
{
	long ImageWidth = 0;
	long ImageHeight = 0;
	MIL_TEXT_CHAR MilName[CHAR_BUFF];
	MIL_INT DispType;

	int i = 0;

	if (!MilOvr[iOvrNum].bOverlayIsInitialized && MilOvr[iOvrNum].MilDisplay)
	{
		// 		MilOvr[iOvrNum].MilOverlayImage = NULL;
		DispType = MdispInquire(MilOvr[iOvrNum].MilDisplay, M_DISPLAY_TYPE, M_NULL);
		if ((DispType & M_WINDOWED) == M_WINDOWED)//== M_WINDOWED + M_USER_WINDOW))
		{
			MdispControl(MilOvr[iOvrNum].MilDisplay, M_OVERLAY_SHOW, M_DISABLE);
			MdispControl(MilOvr[iOvrNum].MilDisplay, M_OVERLAY, M_ENABLE);
			MdispInquire(MilOvr[iOvrNum].MilDisplay, M_OVERLAY_ID, &MilOvr[iOvrNum].MilOverlayImage);

			MdispInquire(MilOvr[iOvrNum].MilDisplay, M_TITLE, MilName);

			if (!MilOvr[iOvrNum].MilOverlayImage) return;
			MdispInquire(MilOvr[iOvrNum].MilDisplay, M_TRANSPARENT_COLOR, &MilOvr[iOvrNum].TransparentColor);
			//MbufClear(vision.Mil_Image, vision.TransparentColor); 
			ImageWidth = (long)MbufInquire(MilOvr[iOvrNum].MilOverlayImage, M_SIZE_X, M_NULL);
			ImageHeight = (long)MbufInquire(MilOvr[iOvrNum].MilOverlayImage, M_SIZE_Y, M_NULL);
			MgraControl(M_DEFAULT, M_BACKGROUND_MODE, M_TRANSPARENT);
			MgraColor(M_DEFAULT, M_RGB888(MilOvr[iOvrNum].oc_red, MilOvr[iOvrNum].oc_green, MilOvr[iOvrNum].oc_blue));
			MilOvr[iOvrNum].bOverlayIsInitialized = TRUE;
		}

		// LUT data for PSEUDO-COLOR:           R    G    B     Entry:    Color: 
		static unsigned char LutLow[30] = { 0, 0, 0, // Index 0 = black 
			128, 0, 0, // Index 1 = dark-red
			0, 128, 0, // Index 2 = dark-green
			128, 128, 0, // Index 3 = dark-yellow
			0, 0, 128, // Index 4 = dark-blue
			128, 0, 128, // Index 5 = dark-magenta
			0, 128, 128, // Index 6 = dark-cyan
			192, 192, 192, // Index 7 = bright-grey
			192, 220, 192, // Index 8 = lite-green
			166, 202, 240  // Index 9 = lite-blue
		};
		static unsigned char LutHigh[30] = { 255, 251, 240, // Index 246 = lite-white
			160, 160, 164, // Index 247 = lite-grey
			128, 128, 128, // Index 248 = dark-grey
			255, 0, 0, // Index 249 = red
			0, 255, 0, // Index 250 = green
			255, 255, 0, // Index 251 = yellow
			0, 0, 255, // Index 252 = blue
			255, 0, 255, // Index 253 = magenta
			0, 255, 255, // Index 254 = cyan
			255, 255, 255  // Index 255 = white
		};

		if (MbufInquire(MilOvr[iOvrNum].MilOverlayImage, M_SIZE_BAND, M_NULL) == 1)
		{
			MIL_ID       MILLut, MILLutChildLow, MILLutChildHigh, MILLutChildKey;
			PALETTEENTRY PalEntry;
			HDC          hDC;
			COLORREF     ColRef;

			MbufAllocColor(m_MilSys, 3L, 256L, 1L, 8L + M_UNSIGNED, M_LUT, &MILLut);

			MbufChild2d(MILLut, 0, 0, 10, 1, &MILLutChildLow);
			MbufChild2d(MILLut, 246, 0, 10, 1, &MILLutChildHigh);
			MgenLutRamp(MILLut, 0, 0, 255, 255);
			MbufPutColor(MILLutChildLow, M_RGB24 + M_PACKED, M_ALL_BAND, &LutLow);
			MbufPutColor(MILLutChildHigh, M_RGB24 + M_PACKED, M_ALL_BAND, &LutHigh);

			hDC = ::GetDC(NULL);
			GetSystemPaletteEntries(hDC, (UINT)MilOvr[iOvrNum].TransparentColor, 1, &PalEntry);
			ColRef = RGB(PalEntry.peRed, PalEntry.peGreen, PalEntry.peBlue);
			MbufChild1d(MILLut, MilOvr[iOvrNum].TransparentColor, 1, &MILLutChildKey);
			MbufPutColor(MILLutChildKey, (M_RGB24 | M_PACKED), M_ALL_BAND, &ColRef);
			::ReleaseDC(NULL, hDC);

			MdispControl(MilOvr[iOvrNum].MilDisplay, M_OVERLAY_LUT, MILLut);

			MbufFree(MILLutChildKey);
			MbufFree(MILLutChildHigh);
			MbufFree(MILLutChildLow);
			MbufFree(MILLut);
			MilOvr[iOvrNum].bOverlayIsInitialized = TRUE;
		}
// 		else
// 		{
// 			MIL_ID MilOverlayImageGreen;
// 			MbufChildColor(MilOvr[iOvrNum].MilOverlayImage, M_GREEN, &MilOverlayImageGreen);
// 		}
	}

	if (MilOvr[iOvrNum].bOverlayIsInitialized)
	{
		if (MilOvr[iOvrNum].bOverlayIsEnable == false)
		{
			MdispControl(MilOvr[iOvrNum].MilDisplay, M_OVERLAY_SHOW, M_ENABLE);
			MilOvr[iOvrNum].bOverlayIsEnable = true;
		}
		else
		{
			MdispControl(MilOvr[iOvrNum].MilDisplay, M_OVERLAY_SHOW, M_DISABLE);
			MilOvr[iOvrNum].bOverlayIsEnable = false;
		}
	}
	// 	MgraControl(M_DEFAULT, M_COLOR, MIL_LIGHTGREEN);
}
void CDisplay::drawOverlay(int iOvrNum /*= DEF_OVR_SET*/)
{
	// 	if(pViewThis->visionData->iMode == 0)
	// 		return;
	int i;

	if (!MilOvr[iOvrNum].bOverlayIsEnable)	return;

	// 	if (iOvrNum == DEF_OVR_MAIN)
	MbufClear(MilOvr[iOvrNum].MilOverlayImage, (MIL_DOUBLE)MilOvr[iOvrNum].TransparentColor);

	for (i = 0; i < MilOvr[iOvrNum].lineList.cnt; i++)
	{
		MilOvr[iOvrNum].oc_red = (unsigned long)GetRValue(MilOvr[iOvrNum].lineList.elem[i].c);
		MilOvr[iOvrNum].oc_green = (unsigned long)GetGValue(MilOvr[iOvrNum].lineList.elem[i].c);
		MilOvr[iOvrNum].oc_blue = (unsigned long)GetBValue(MilOvr[iOvrNum].lineList.elem[i].c);
		MgraColor(M_DEFAULT, M_RGB888(MilOvr[iOvrNum].oc_red, MilOvr[iOvrNum].oc_green, MilOvr[iOvrNum].oc_blue));
		MgraLine(M_DEFAULT, MilOvr[iOvrNum].MilOverlayImage, MilOvr[iOvrNum].lineList.elem[i].p[0].x, MilOvr[iOvrNum].lineList.elem[i].p[0].y, MilOvr[iOvrNum].lineList.elem[i].p[1].x, MilOvr[iOvrNum].lineList.elem[i].p[1].y);
	}
	for (i = 0; i < MilOvr[iOvrNum].crossList.cnt; i++)
	{
		MilOvr[iOvrNum].oc_red = (unsigned long)GetRValue(MilOvr[iOvrNum].crossList.elem[i].c);
		MilOvr[iOvrNum].oc_green = (unsigned long)GetGValue(MilOvr[iOvrNum].crossList.elem[i].c);
		MilOvr[iOvrNum].oc_blue = (unsigned long)GetBValue(MilOvr[iOvrNum].crossList.elem[i].c);
		MgraColor(M_DEFAULT, M_RGB888(MilOvr[iOvrNum].oc_red, MilOvr[iOvrNum].oc_green, MilOvr[iOvrNum].oc_blue));
		MgraLine(M_DEFAULT, MilOvr[iOvrNum].MilOverlayImage, MilOvr[iOvrNum].crossList.elem[i].p.x - MilOvr[iOvrNum].crossList.elem[i].w, MilOvr[iOvrNum].crossList.elem[i].p.y - MilOvr[iOvrNum].crossList.elem[i].w, MilOvr[iOvrNum].crossList.elem[i].p.x + MilOvr[iOvrNum].crossList.elem[i].w, MilOvr[iOvrNum].crossList.elem[i].p.y + MilOvr[iOvrNum].crossList.elem[i].w);
		MgraLine(M_DEFAULT, MilOvr[iOvrNum].MilOverlayImage, MilOvr[iOvrNum].crossList.elem[i].p.x + MilOvr[iOvrNum].crossList.elem[i].w, MilOvr[iOvrNum].crossList.elem[i].p.y - MilOvr[iOvrNum].crossList.elem[i].w, MilOvr[iOvrNum].crossList.elem[i].p.x - MilOvr[iOvrNum].crossList.elem[i].w, MilOvr[iOvrNum].crossList.elem[i].p.y + MilOvr[iOvrNum].crossList.elem[i].w);
	}
	for (i = 0; i < MilOvr[iOvrNum].boxList.cnt; i++)
	{
		MilOvr[iOvrNum].oc_red = (unsigned long)GetRValue(MilOvr[iOvrNum].boxList.elem[i].c);
		MilOvr[iOvrNum].oc_green = (unsigned long)GetGValue(MilOvr[iOvrNum].boxList.elem[i].c);
		MilOvr[iOvrNum].oc_blue = (unsigned long)GetBValue(MilOvr[iOvrNum].boxList.elem[i].c);
		MgraColor(M_DEFAULT, M_RGB888(MilOvr[iOvrNum].oc_red, MilOvr[iOvrNum].oc_green, MilOvr[iOvrNum].oc_blue));
		if (MilOvr[iOvrNum].boxList.elem[i].f)//가득찬 사각형
			MgraRectFill(M_DEFAULT, MilOvr[iOvrNum].MilOverlayImage, MilOvr[iOvrNum].boxList.elem[i].r.left, MilOvr[iOvrNum].boxList.elem[i].r.top, MilOvr[iOvrNum].boxList.elem[i].r.right, MilOvr[iOvrNum].boxList.elem[i].r.bottom);
		else
			MgraRect(M_DEFAULT, MilOvr[iOvrNum].MilOverlayImage, MilOvr[iOvrNum].boxList.elem[i].r.left, MilOvr[iOvrNum].boxList.elem[i].r.top, MilOvr[iOvrNum].boxList.elem[i].r.right, MilOvr[iOvrNum].boxList.elem[i].r.bottom);
	}
	for (i = 0; i < MilOvr[iOvrNum].circleList.cnt; i++)
	{
		MilOvr[iOvrNum].oc_red = (unsigned long)GetRValue(MilOvr[iOvrNum].circleList.elem[i].c);
		MilOvr[iOvrNum].oc_green = (unsigned long)GetGValue(MilOvr[iOvrNum].circleList.elem[i].c);
		MilOvr[iOvrNum].oc_blue = (unsigned long)GetBValue(MilOvr[iOvrNum].circleList.elem[i].c);
		long cx = ((MilOvr[iOvrNum].circleList.elem[i].r.left + MilOvr[iOvrNum].circleList.elem[i].r.right) / 2);
		long cy = ((MilOvr[iOvrNum].circleList.elem[i].r.top + MilOvr[iOvrNum].circleList.elem[i].r.bottom) / 2);
		long rx = ((MilOvr[iOvrNum].circleList.elem[i].r.right - MilOvr[iOvrNum].circleList.elem[i].r.left) / 2);
		long ry = ((MilOvr[iOvrNum].circleList.elem[i].r.bottom - MilOvr[iOvrNum].circleList.elem[i].r.top) / 2);
		MgraColor(M_DEFAULT, M_RGB888(MilOvr[iOvrNum].oc_red, MilOvr[iOvrNum].oc_green, MilOvr[iOvrNum].oc_blue));
		MgraArc(M_DEFAULT, MilOvr[iOvrNum].MilOverlayImage, cx, cy, rx, ry, 0, 360);
	}

	for (i = 0; i < MilOvr[iOvrNum].pixelList.cnt; i++)
	{
		MilOvr[iOvrNum].oc_red = (unsigned long)GetRValue(MilOvr[iOvrNum].pixelList.elem[i].c);
		MilOvr[iOvrNum].oc_green = (unsigned long)GetGValue(MilOvr[iOvrNum].pixelList.elem[i].c);
		MilOvr[iOvrNum].oc_blue = (unsigned long)GetBValue(MilOvr[iOvrNum].pixelList.elem[i].c);
		MgraColor(M_DEFAULT, M_RGB888(MilOvr[iOvrNum].oc_red, MilOvr[iOvrNum].oc_green, MilOvr[iOvrNum].oc_blue));
		MgraDot(M_DEFAULT, MilOvr[iOvrNum].MilOverlayImage, MilOvr[iOvrNum].pixelList.elem[i].p.x, MilOvr[iOvrNum].pixelList.elem[i].p.y);
	}


	for (i = 0; i < MilOvr[iOvrNum].textList.cnt; i++)
	{
		MilOvr[iOvrNum].oc_red = (unsigned long)GetRValue(MilOvr[iOvrNum].textList.elem[i].c);
		MilOvr[iOvrNum].oc_green = (unsigned long)GetGValue(MilOvr[iOvrNum].textList.elem[i].c);
		MilOvr[iOvrNum].oc_blue = (unsigned long)GetBValue(MilOvr[iOvrNum].textList.elem[i].c);
		MgraColor(M_DEFAULT, M_RGB888(MilOvr[iOvrNum].oc_red, MilOvr[iOvrNum].oc_green, MilOvr[iOvrNum].oc_blue));

		TCHAR* cTemp = new TCHAR[MilOvr[iOvrNum].textList.elem[i].str.GetLength() + 1];
		strcpy_s(cTemp, MilOvr[iOvrNum].textList.elem[i].str.GetLength() + 1, MilOvr[iOvrNum].textList.elem[i].str);
		MgraText(M_DEFAULT, MilOvr[iOvrNum].MilOverlayImage, MilOvr[iOvrNum].textList.elem[i].sx, MilOvr[iOvrNum].textList.elem[i].sy, cTemp);
		delete cTemp;
	}
	MilOvr[iOvrNum].pixelList.cnt = MilOvr[iOvrNum].crossList.cnt = MilOvr[iOvrNum].lineList.cnt = MilOvr[iOvrNum].boxList.cnt = MilOvr[iOvrNum].circleList.cnt = MilOvr[iOvrNum].textList.cnt = 0;

}
void CDisplay::OverlayClear(int iOvrNum /*= DEF_OVR_SET*/)
{
	MilOvr[iOvrNum].pixelList.cnt = MilOvr[iOvrNum].crossList.cnt = MilOvr[iOvrNum].lineList.cnt = MilOvr[iOvrNum].boxList.cnt = MilOvr[iOvrNum].circleList.cnt = 0;

	MbufClear(MilOvr[iOvrNum].MilOverlayImage, (MIL_DOUBLE)MilOvr[iOvrNum].TransparentColor);
}

void CDisplay::addCrossList(CPoint p, int wdt, int c, int iOvrNum /*= DEF_OVR_SET*/, BOOL bZooming /*= FALSE*/)
{
	if (bZooming)
	{
		p = ConvertPnt(p, TRUE);
	}
	MilOvr[iOvrNum].crossList.elem[MilOvr[iOvrNum].crossList.cnt].c = c;
	MilOvr[iOvrNum].crossList.elem[MilOvr[iOvrNum].crossList.cnt].p = p;
	MilOvr[iOvrNum].crossList.elem[MilOvr[iOvrNum].crossList.cnt].w = wdt;
	if (MilOvr[iOvrNum].crossList.cnt < DEF_MAX_CROSS_CNT) MilOvr[iOvrNum].crossList.cnt++;
}
void CDisplay::addCrossList(int x, int y, int wdt, int c, int iOvrNum /*= DEF_OVR_SET*/, BOOL bZooming /*= FALSE*/)
{
	if (bZooming)
	{
		CPoint tmpPtn;
		tmpPtn.x = x;
		tmpPtn.y = y;
		tmpPtn = ConvertPnt(tmpPtn, TRUE);
		x = tmpPtn.x;
		y = tmpPtn.y;
	}
	MilOvr[iOvrNum].crossList.elem[MilOvr[iOvrNum].crossList.cnt].c = c;
	MilOvr[iOvrNum].crossList.elem[MilOvr[iOvrNum].crossList.cnt].p.x = x;
	MilOvr[iOvrNum].crossList.elem[MilOvr[iOvrNum].crossList.cnt].p.y = y;
	MilOvr[iOvrNum].crossList.elem[MilOvr[iOvrNum].crossList.cnt].w = wdt;
	if (MilOvr[iOvrNum].crossList.cnt < DEF_MAX_CROSS_CNT) MilOvr[iOvrNum].crossList.cnt++;
}

void CDisplay::addLineList(CPoint sl, CPoint el, int c, int iOvrNum /*= DEF_OVR_SET*/, BOOL bZooming /*= FALSE*/)
{
	if (bZooming)
	{
		sl = ConvertPnt(sl, TRUE);
		el = ConvertPnt(el, TRUE);
	}
	MilOvr[iOvrNum].lineList.elem[MilOvr[iOvrNum].lineList.cnt].c = c;
	MilOvr[iOvrNum].lineList.elem[MilOvr[iOvrNum].lineList.cnt].p[0].x = (int)sl.x;
	MilOvr[iOvrNum].lineList.elem[MilOvr[iOvrNum].lineList.cnt].p[1].x = (int)el.x;

	MilOvr[iOvrNum].lineList.elem[MilOvr[iOvrNum].lineList.cnt].p[0].y = (int)sl.y;
	MilOvr[iOvrNum].lineList.elem[MilOvr[iOvrNum].lineList.cnt].p[1].y = (int)el.y;

	if (MilOvr[iOvrNum].lineList.cnt < DEF_MAX_LINE_CNT) MilOvr[iOvrNum].lineList.cnt++;
}

void CDisplay::addPixelList(int x, int y, int c, int iOvrNum /*= DEF_OVR_SET*/, BOOL bZooming /*= FALSE*/)
{
	if (bZooming)
	{
		CPoint tmpPtn;
		tmpPtn.x = x;
		tmpPtn.y = y;
		tmpPtn = ConvertPnt(tmpPtn, TRUE);
		x = tmpPtn.x;
		y = tmpPtn.y;
	}
	MilOvr[iOvrNum].pixelList.elem[MilOvr[iOvrNum].pixelList.cnt].c = c;
	MilOvr[iOvrNum].pixelList.elem[MilOvr[iOvrNum].pixelList.cnt].p.x = x;
	MilOvr[iOvrNum].pixelList.elem[MilOvr[iOvrNum].pixelList.cnt].p.y = y;
	if (MilOvr[iOvrNum].pixelList.cnt < DEF_MAX_PIXEL_CNT) MilOvr[iOvrNum].pixelList.cnt++;
}
void CDisplay::addPixelList(CPoint p, int c, int iOvrNum /*= DEF_OVR_SET*/, BOOL bZooming /*= FALSE*/)
{
	if (bZooming)
	{
		p = ConvertPnt(p, TRUE);
	}
	MilOvr[iOvrNum].pixelList.elem[MilOvr[iOvrNum].pixelList.cnt].c = c;
	MilOvr[iOvrNum].pixelList.elem[MilOvr[iOvrNum].pixelList.cnt].p.x = p.x;
	MilOvr[iOvrNum].pixelList.elem[MilOvr[iOvrNum].pixelList.cnt].p.y = p.y;
	if (MilOvr[iOvrNum].pixelList.cnt < DEF_MAX_PIXEL_CNT) MilOvr[iOvrNum].pixelList.cnt++;
}

void CDisplay::addBoxList(CRect r, int c, int iOvrNum /*= DEF_OVR_SET*/, BOOL bZooming /*= FALSE*/)
{
	if (bZooming)
	{
		CPoint tmpPtn;
		tmpPtn.x = r.left;
		tmpPtn.y = r.top;
		tmpPtn = ConvertPnt(tmpPtn, TRUE);
		r.left = tmpPtn.x;
		r.top = tmpPtn.y;

		tmpPtn.x = r.right;
		tmpPtn.y = r.bottom;
		tmpPtn = ConvertPnt(tmpPtn, TRUE);
		r.right = tmpPtn.x;
		r.bottom = tmpPtn.y;
	}
	MilOvr[iOvrNum].boxList.elem[MilOvr[iOvrNum].boxList.cnt].f = FALSE;
	MilOvr[iOvrNum].boxList.elem[MilOvr[iOvrNum].boxList.cnt].c = c;
	MilOvr[iOvrNum].boxList.elem[MilOvr[iOvrNum].boxList.cnt].r = r;
	if (MilOvr[iOvrNum].boxList.cnt < DEF_MAX_BOX_CNT) MilOvr[iOvrNum].boxList.cnt++;
}

void CDisplay::addFillBoxList(CRect r, int c, int iOvrNum /*= DEF_OVR_SET*/, BOOL bZooming /*= FALSE*/)
{
	if (bZooming)
	{
		CPoint tmpPtn;
		tmpPtn.x = r.left;
		tmpPtn.y = r.top;
		tmpPtn = ConvertPnt(tmpPtn, TRUE);
		r.left = tmpPtn.x;
		r.top = tmpPtn.y;

		tmpPtn.x = r.right;
		tmpPtn.y = r.bottom;
		tmpPtn = ConvertPnt(tmpPtn, TRUE);
		r.right = tmpPtn.x;
		r.bottom = tmpPtn.y;
	}
	MilOvr[iOvrNum].boxList.elem[MilOvr[iOvrNum].boxList.cnt].f = TRUE;
	MilOvr[iOvrNum].boxList.elem[MilOvr[iOvrNum].boxList.cnt].c = c;
	MilOvr[iOvrNum].boxList.elem[MilOvr[iOvrNum].boxList.cnt].r = r;
	if (MilOvr[iOvrNum].boxList.cnt < DEF_MAX_BOX_CNT) MilOvr[iOvrNum].boxList.cnt++;
}

void CDisplay::addTriAngleList(CPoint p1, CPoint p2, CPoint p3, int c, int iOvrNum /*= DEF_OVR_SET*/, BOOL bZooming /*= FALSE*/)
{
	if (bZooming)
	{
		p1 = ConvertPnt(p1, TRUE);
		p2 = ConvertPnt(p2, TRUE);
		p3 = ConvertPnt(p3, TRUE);
	}
	MilOvr[iOvrNum].TriangleList.elem[MilOvr[iOvrNum].TriangleList.cnt].p[0] = p1;
	MilOvr[iOvrNum].TriangleList.elem[MilOvr[iOvrNum].TriangleList.cnt].p[1] = p2;
	MilOvr[iOvrNum].TriangleList.elem[MilOvr[iOvrNum].TriangleList.cnt].p[2] = p3;

	MilOvr[iOvrNum].TriangleList.elem[MilOvr[iOvrNum].TriangleList.cnt].c = c;

	if (MilOvr[iOvrNum].TriangleList.cnt < DEF_MAX_TRIANGLE_CNT) MilOvr[iOvrNum].TriangleList.cnt++;
}
void CDisplay::addCircleList(CRect r, int c, int iOvrNum /*= DEF_OVR_SET*/, BOOL bZooming /*= FALSE*/)
{
	int x, y;
	if (bZooming)
	{
		CPoint tmpPtn;
		tmpPtn.x = r.left;
		tmpPtn.y = r.top;
		tmpPtn = ConvertPnt(tmpPtn, TRUE);
		x = tmpPtn.x;
		y = tmpPtn.y;
		r.left = x;
		r.top = y;

		tmpPtn.x = r.right;
		tmpPtn.y = r.bottom;
		tmpPtn = ConvertPnt(tmpPtn, TRUE);
		x = tmpPtn.x;
		y = tmpPtn.y;
		r.right = x;
		r.bottom = y;
	}
	MilOvr[iOvrNum].circleList.elem[MilOvr[iOvrNum].circleList.cnt].c = c;
	MilOvr[iOvrNum].circleList.elem[MilOvr[iOvrNum].circleList.cnt].r = r;
	if (MilOvr[iOvrNum].circleList.cnt < DEF_MAX_CIRCLE_CNT) MilOvr[iOvrNum].circleList.cnt++;
}
void CDisplay::addTextList(char* szMessage, double xFontScale, double yFontScale, int sx, int sy, int c, int iOvrNum /*= DEF_OVR_SET*/, BOOL bZooming /*= FALSE*/)
{
	if (bZooming)
	{
		CPoint tmpPtn;
		tmpPtn.x = sx;
		tmpPtn.y = sy;
		tmpPtn = ConvertPnt(tmpPtn, TRUE);
		sx = tmpPtn.x;
		sy = tmpPtn.y;
	}
	MilOvr[iOvrNum].textList.elem[MilOvr[iOvrNum].textList.cnt].c = c;
	sprintf_s(MilOvr[iOvrNum].textList.elem[MilOvr[iOvrNum].textList.cnt].szMessage, sizeof(MilOvr[iOvrNum].textList.elem[MilOvr[iOvrNum].textList.cnt].szMessage), _T("%s"), szMessage);
	MilOvr[iOvrNum].textList.elem[MilOvr[iOvrNum].textList.cnt].xFontScale = xFontScale;
	MilOvr[iOvrNum].textList.elem[MilOvr[iOvrNum].textList.cnt].yFontScale = yFontScale;
	MilOvr[iOvrNum].textList.elem[MilOvr[iOvrNum].textList.cnt].sx = sx;
	MilOvr[iOvrNum].textList.elem[MilOvr[iOvrNum].textList.cnt].sy = sy;
	if (MilOvr[iOvrNum].textList.cnt < 30) MilOvr[iOvrNum].textList.cnt++;
}

void CDisplay::ImageLoad_Browser()
{
	CString strPath;
	TCHAR szFilter[] = _T("TIF 파일열기(*.tif)|*.tif|모든파일(*.*) |*.*|");

	//CFileDialog dlg(FALSE,"*", "*", OFN_HIDEREADONLY, szFilter) ;
	// 	CFileDialog dlg(TRUE, _T("tif"), _T("*.tif"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	CFileDialog dlg(TRUE, _T("*"), _T("*.*"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);

	if (IDOK == dlg.DoModal())
	{
		strPath = dlg.GetPathName();
		MbufClear(MilBuf_Origin_Image, 0);
		MbufClear(MilBuf_Set_View, 0);

		MbufLoad((TCHAR*)(const TCHAR*)strPath, MilBuf_Origin_Image);
	}
}

void CDisplay::ImageSave(int BuffNum /*= -1*/)
{
	TCHAR tmpStr[MAX_PATH];
	TCHAR cFilePath[100];

	int nCnt = 1;

	sprintf_s(tmpStr, sizeof(tmpStr), _T("%s\\Image"), pModelData->curDirName);
	CountingImgFile(tmpStr, BuffNum, _T("tif"), &nCnt, 0);
	sprintf_s(tmpStr, sizeof(tmpStr), _T("%s\\%03d-%02d.tif"), tmpStr, nCnt, BuffNum + 1);

	CFileDialog* fileSaveDialog = NULL;
	fileSaveDialog = new CFileDialog(FALSE, _T("tif"), tmpStr, OFN_OVERWRITEPROMPT | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, _T("Image Files(*.tif)|*.tif|"));

	if (fileSaveDialog)
	{
		if (fileSaveDialog->DoModal() == IDOK)
		{
			CString strTemp = fileSaveDialog->GetPathName().Left(fileSaveDialog->GetPathName().GetLength() - 4);
			CString strFilename;
			delete fileSaveDialog;

			strFilename.Format(_T("%s.tif"), strTemp);
			sprintf_s(cFilePath, sizeof(cFilePath), strFilename.GetBuffer());
			MbufExport(cFilePath, M_TIFF, MilBuf_Origin_Image);
		}
	}
}

BOOL CDisplay::CheckInDisp(CPoint point, int iOvrNum /*= DEF_OVR_SET*/)
{
	if (point.x > MilOvr[iOvrNum].rectPic.left &&
		point.x < MilOvr[iOvrNum].rectPic.right &&
		point.y > MilOvr[iOvrNum].rectPic.top &&
		point.y < MilOvr[iOvrNum].rectPic.bottom)	//영역 안에 클릭했을 경우
	{
		return TRUE;
	}
	return FALSE;
}

int CDisplay::WhichInDisp(CPoint point) /*Set 화면에서만 玲諛〈?*/
{
	int i = 0;

	for (i = 0; i < MAX_OVR_CNT; i++)
	{
		if (point.x > MilOvr[i].rectPic.left &&
			point.x < MilOvr[i].rectPic.right &&
			point.y > MilOvr[i].rectPic.top &&
			point.y < MilOvr[i].rectPic.bottom)	//영역 안에 클릭했을 경우
			return i;

	}
	return -1;
}

CPoint CDisplay::ConvertPnt(CPoint point, BOOL iZooming /*= FALSE*/)
{
	CPoint szPnt;

	if (iZooming)
	{
		szPnt.x = point.x / m_ZoomX;
		szPnt.y = point.y / m_ZoomY;
	}
	else
	{
		szPnt.x = point.x * m_ZoomX;
		szPnt.y = point.y * m_ZoomY;
	}

	return szPnt;
}

CPoint CDisplay::ConvertPnt_Click(CPoint point, int iOvrNum /*= DEF_OVR_SET*/)
{
	CPoint szPnt;

	szPnt.x = (int)point.x - MilOvr[iOvrNum].rectPic.left;
	szPnt.y = (int)point.y - MilOvr[iOvrNum].rectPic.top;

	return szPnt;
}

BOOL CDisplay::CheckInBox(CPoint point, CRect rcBox)
{
	if (point.x > rcBox.left + 50 &&
		point.x < rcBox.right - 50 &&
		point.y > rcBox.top + 50 &&
		point.y < rcBox.bottom - 50)	//영역 안에 클릭했을 경우
	{
		return TRUE;
	}
	return FALSE;
}



void CMilInspFunc::Calibration(int GroupNum, double XPixArray[], double YPixArray[], double XWorldArray[], double YWorldArray[], int NumPoint)
{
	ErrorCode = 0;
	if (MilCalibration[GroupNum])
	{
		McalFree(MilCalibration[GroupNum]);
		MilCalibration[GroupNum] = M_NULL;
	}
	McalAlloc(pVision->MilSystem, M_DEFAULT, M_DEFAULT, &MilCalibration[GroupNum]);

	MIL_INT res;
	McalList(MilCalibration[GroupNum], XPixArray, YPixArray, XWorldArray, YWorldArray, NULL, NumPoint, M_PERSPECTIVE_TRANSFORMATION, M_DEFAULT);

	res = McalInquire(MilCalibration[GroupNum], M_CALIBRATION_STATUS, M_NULL);
	if (res == M_CALIBRATED)
	{
		sprintf_s(msgVision, sizeof(msgVision), _T("영상-모터간 좌표 일치화 완료."));
		//AfxMessageBox(msgVision);
	}
	else
	{
		sprintf_s(msgVision, sizeof(msgVision), _T("영상-모터간 좌표 일치화 실패."));
		//AfxMessageBox(msgVision);
		ErrorCode = 1;
	}
}

void CMilInspFunc::GetCamTransformCoordinate(int GroupNum, double x, double y, double* resX, double* resY, int tansformType /*=0*/)
{
	//tansformType == 0: M_PIXEL_TO_WORLD  1: M_WORLD_TO_PIXEL
	int att = M_PIXEL_TO_WORLD;
	if (tansformType)	att = M_WORLD_TO_PIXEL;
	McalTransformCoordinate(MilCalibration[GroupNum], att, x, y, resX, resY);
}

void CMilInspFunc::InitCal()
{
	int i;
	for (i = 0; i < DEF_MAX_GROUP; i++)
		McalAlloc(pVision->MilSystem, M_DEFAULT, M_DEFAULT, &MilCalibration[i]);
}

void CMilInspFunc::CloseCal()
{
	int i;
	for (i = 0; i < DEF_MAX_GROUP; i++)
	{
		McalFree(MilCalibration[i]);
	}
}

void CMilInspFunc::SaveCal(LPCSTR nPathStr, int GroupNum /*= 0*/)
{
	char tmpStr[CHAR_BUFF];
	sprintf_s(tmpStr, sizeof(char)*CHAR_BUFF, _T("%s"), nPathStr);
	m_Util.CreateDir(tmpStr);

	sprintf_s(tmpStr, sizeof(char)*CHAR_BUFF, _T("%sCamera%dCalMilFile.cal"), nPathStr, GroupNum + 1);
// 	sprintf_s(tmpStr, sizeof(char)*CHAR_BUFF, _T("%s\\Data\\Camera%dCalMilFile.cal"), nPathStr, GroupNum + 1);

	McalSave(tmpStr, MilCalibration[GroupNum], M_DEFAULT);
}

void CMilInspFunc::LoadCal(LPCSTR nPathStr, int GroupNum /*= 0*/)
{
	if (MilCalibration[GroupNum])
	{
		McalFree(MilCalibration[GroupNum]);
		MilCalibration[GroupNum] = M_NULL;
	}

	char tmpStr[CHAR_BUFF];
	sprintf_s(tmpStr, sizeof(char)*CHAR_BUFF, _T("%sCamera%dCalMilFile.cal"), nPathStr, GroupNum + 1);
// 	sprintf_s(tmpStr, sizeof(char)*CHAR_BUFF, _T("%s\\Data\\Camera%dCalMilFile.cal"), nPathStr, GroupNum + 1);
	McalRestore(tmpStr, pVision->MilSystem, M_DEFAULT, &MilCalibration[GroupNum]);
}


void CMilInspFunc::ModInit(MIL_DOUBLE ModColor)
{
	//Geometric - ModelFind
	MgraAlloc(pVision->MilSystem, &Milmodgra);
	MgraAlloc(pVision->MilSystem, &MilmodResultgra);

	MgraColor(Milmodgra, ModColor);
	MgraColor(MilmodResultgra, M_COLOR_BLUE);

	int i;
	for (i = 0; i < DEF_MAX_MARK_NUM; i++)
	{
		MmodAlloc(pVision->MilSystem, M_GEOMETRIC, M_DEFAULT, &MilImageMod[i]);
		MmodAllocResult(pVision->MilSystem, M_DEFAULT, &MilResultMod[i]);
	}

}
void CMilInspFunc::ModLoad(int iMarkNo)
{
	MmodPreprocess(MilImageMod[iMarkNo], M_RESET);

	TCHAR tmpStr[CHAR_BUFF];
	sprintf_s(tmpStr, sizeof(tmpStr), _T("%sModel\\%s\\Mark_%d.mmf"), pModelData->curDirName, pModelData->modelName, iMarkNo + 1);

	//File확인기능 필요함.
	if (m_Util.Existence(tmpStr) != FM_FILE)
	{
		TECmsgOut(L"저장된 패턴파일이 없습니다.", LIGHTMAGENTA, 1);
		return;
	}

	//MmodRestore(tmpStr, pVision->MilSystem, M_DEFAULT, &MilImageMod[iMarkNo]);
	MmodStream(tmpStr, pVision->MilSystem, M_LOAD, M_FILE, M_DEFAULT, M_DEFAULT, &MilImageMod[iMarkNo], M_NULL);
	MgraColor(Milmodgra, M_COLOR_MAGENTA);
}

void CMilInspFunc::ModRegist(int iMarkNo, MIL_DOUBLE ModColor)
{
	MmodPreprocess(MilImageMod[iMarkNo], M_RESET);
	if (MilImageMod[iMarkNo])
	{
		MmodFree(MilImageMod[iMarkNo]);
		MilImageMod[iMarkNo] = M_NULL;
	}
	MmodAlloc(pVision->MilSystem, M_GEOMETRIC, M_DEFAULT, &MilImageMod[iMarkNo]);

	if (MilResultMod[iMarkNo])
	{
		MmodFree(MilResultMod[iMarkNo]);
		MilResultMod[iMarkNo] = M_NULL;
	}
	MmodAllocResult(pVision->MilSystem, M_DEFAULT, &MilResultMod[iMarkNo]);

	//색상빼기
	// 	MimConvert(MilBuf_Insp_Image, MilBuf_Insp_Image, M_RGB_TO_L);

	//영상축소
	MbufClear(MilBuf_Mark[iMarkNo], 0);
	// 	MIL_DOUBLE sX1, sX2;
	MIL_DOUBLE rateSize;
	// 	sX1 = (MIL_DOUBLE)MbufInquire(MilBuf_Insp_Image, M_SIZE_X, M_NULL);//2464
	// 	sX2 = (MIL_DOUBLE)MbufInquire(MilBuf_Mark[iMarkNo], M_SIZE_X, M_NULL);
	// 	rateSize = sX2 / sX1;
	rateSize = 1;
	// 	MimResize(MilBuf_Insp_Image, MilBuf_Mark[iMarkNo], rateSize, rateSize, M_DEFAULT);
	MbufCopy(MilBuf_Insp_Image, MilBuf_Mark[iMarkNo]);


	MgraColor(Milmodgra, M_COLOR_RED);
	//영상잘라서 마크 등록
	MIL_DOUBLE nXst = (MIL_DOUBLE)(ptrMarkStart.x / rateSize);
	MIL_DOUBLE nYst = (MIL_DOUBLE)(ptrMarkStart.y / rateSize);
	MIL_DOUBLE nXsz = (MIL_DOUBLE)(ptrMarkSize.x / rateSize);
	MIL_DOUBLE nYsz = (MIL_DOUBLE)(ptrMarkSize.y / rateSize);
	MmodDefine(MilImageMod[iMarkNo], M_IMAGE, MilBuf_Mark[iMarkNo], nXst, nYst, nXsz, nYsz);
	ModControl(iMarkNo);

	MmodMask(MilImageMod[iMarkNo], M_DEFAULT, MilBuf_Mask, M_DONT_CARE, M_DEFAULT);
// 	MbufExport(_T("D:\\asd.tif"), M_TIFF, MilBuf_Mask);

	TCHAR tmpStr[CHAR_BUFF];
	sprintf_s(tmpStr, sizeof(tmpStr), _T("%sModel\\%s\\"), pModelData->curDirName, pModelData->modelName);
	m_Util.CreateDir(tmpStr);
// 	MmodSave(tmpStr, MilImageMod[iMarkNo], M_DEFAULT);
	sprintf_s(tmpStr, sizeof(tmpStr), _T("%sMark_%d.mmf"), tmpStr, iMarkNo + 1);
	MmodStream(tmpStr, pVision->MilSystem, M_SAVE, M_FILE, M_DEFAULT, M_DEFAULT, &MilImageMod[iMarkNo], M_NULL);
}
void CMilInspFunc::ModControl(int iMarkNo/*=0*/)
{
	MmodPreprocess(MilImageMod[iMarkNo], M_RESET);

	//마크 설정
// 	MIL_DOUBLE MilIndex = 0;
// 	MmodInquire(MilImageMod[iMarkNo], M_DEFAULT, M_INDEX_FROM_LABEL, &MilIndex);

	MmodControl(MilImageMod[iMarkNo], M_CONTEXT, M_NUMBER, pModelData->iModNumber);
	MmodControl(MilImageMod[iMarkNo], M_CONTEXT, M_SMOOTHNESS, pModelData->iModSmoothness);
	MmodControl(MilImageMod[iMarkNo], M_CONTEXT, M_TIMEOUT, pModelData->iModTimeout);


	MmodControl(MilImageMod[iMarkNo], M_DEFAULT, M_ACCEPTANCE, (MIL_DOUBLE)pModelData->iModAcceptance);
	MmodControl(MilImageMod[iMarkNo], M_DEFAULT, M_SCALE_MAX_FACTOR, pModelData->dModScaleMax);
	MmodControl(MilImageMod[iMarkNo], M_DEFAULT, M_SCALE_MIN_FACTOR, pModelData->dModScaleMin);
	MmodControl(MilImageMod[iMarkNo], M_DEFAULT, M_ANGLE_DELTA_NEG, pModelData->dModAngleDeltaNeg);
	MmodControl(MilImageMod[iMarkNo], M_DEFAULT, M_ANGLE_DELTA_POS, pModelData->dModAngleDeltaPos);
// 	MmodControl(MilImageMod[iMarkNo], M_DEFAULT, M_SIZE_X, pModelData->iModRectHor);
// 	MmodControl(MilImageMod[iMarkNo], M_DEFAULT, M_SIZE_Y, pModelData->iModRectVer);

	double ReferAngle;
	if (pModelData->dModReferenceAngle < 0)
		ReferAngle = 360 + pModelData->dModReferenceAngle;
	else
		ReferAngle = pModelData->dModReferenceAngle;
	MmodControl(MilImageMod[iMarkNo], M_DEFAULT, M_REFERENCE_ANGLE, ReferAngle);
}

void CMilInspFunc::ModDntCare(int iMarkNo, MIL_ID MaskImage, MIL_INT* MarkSize)
{
	// 	MmodPreprocess(MilImageMod[iMarkNo], M_RESET);
	// 	MilImageCheck(MaskImage);
	MIL_ID tmpMilMask;
	MbufAlloc2d(pVision->MilSystem, MarkSize[0], MarkSize[1], 8 + M_UNSIGNED, M_IMAGE + M_DISP + M_PROC, &tmpMilMask);

	MbufCopyColor2d(MaskImage, tmpMilMask, M_BLUE, 0, 0, M_ALL_BAND, 0, 0, MarkSize[0], MarkSize[1]);
	MimBinarize(tmpMilMask, tmpMilMask, M_IN_RANGE + M_GREATER, 250, M_NULL);


	MmodMask(MilImageMod[iMarkNo], M_DEFAULT, tmpMilMask, M_DONT_CARE, M_DEFAULT);
	MmodSave(_T("E:\\Test\\Test_Mask.mod"), MilImageMod[iMarkNo], M_DEFAULT);

	MbufFree(tmpMilMask);

	// 	MmodPreprocess(MilImageMod[iMarkNo], M_DEFAULT);
}

CDPoint CMilInspFunc::ModFind(MIL_ID InspImage, TCHAR* tmpStr, int iMarkNo /*=0*/)
{
	memset(tmpStr, 0x00, sizeof(TCHAR)*CHAR_BUFF);
	long szCnt = 0;
	int i = 0, j = 0;
	nCount = 0;
	long lTimeoutFlag = M_FALSE;
	double sizeX, sizeY;
	sizeX = sizeY = 0;
	int iFailCnt = 0;

	memset(fScore, 0x00, sizeof(CPoint) * 200);
	memset(fAngle, 0x00, sizeof(CPoint) * 200);
	memset(cdCenterPos, 0x00, sizeof(CPoint) * 200);

	//	for (iMarkNo=0; iMarkNo<DEF_MAX_MARK_NUM; iMarkNo++)
	{
		//색상빼기
		// 		MimConvert(InspImage, InspImage, M_RGB_TO_L);

		//영상축소
		MbufClear(MilBuf_Mark[iMarkNo], 0);
		// 		MIL_DOUBLE sX1, sX2;
		// 		MIL_DOUBLE rateSize;
		// 		sX1 = (MIL_DOUBLE)MbufInquire(InspImage, M_SIZE_X, M_NULL);//2464
		// 		sX2 = (MIL_DOUBLE)MbufInquire(MilBuf_Mark[iMarkNo], M_SIZE_X, M_NULL);
		// 		rateSize = sX2 / sX1;
		// 		rateSize = 1;
		// 		MimResize(InspImage, MilBuf_Mark[iMarkNo], rateSize, rateSize, M_DEFAULT);
		MbufCopy(InspImage, MilBuf_Mark[iMarkNo]);

		// 		MmodControl(MilImageMod[iMarkNo], M_CONTEXT, M_SPEED, M_VERY_HIGH);
		// 		MmodControl(MilImageMod[iMarkNo], M_DEFAULT, M_NUMBER, M_ALL);
		// 		MmodControl(MilImageMod[iMarkNo], M_DEFAULT, M_ACCEPTANCE, 90);
		// 		MmodControl(MilImageMod[iMarkNo], M_DEFAULT, M_TIMEOUT, DEF_MARK_SIZE_X);

		MmodPreprocess(MilImageMod[iMarkNo], M_DEFAULT);
		MmodFind(MilImageMod[iMarkNo], MilBuf_Mark[iMarkNo], MilResultMod[iMarkNo]);		//칩 검출

#ifdef _DEBUG
																							// 		MilImageCheck(InspImage, pDisplay[DEF_DISP_SET]->MilBuf_Set_View,_T("2"));
// 		MbufExport(_T("D:\\Desk\\ModTest.tif"), M_TIFF, MilBuf_Mark[iMarkNo]);
// 		MmodSave(_T("D:\\Desk\\ModTest.mod"), MilImageMod[iMarkNo], M_DEFAULT);
#endif

		MmodGetResult(MilResultMod[iMarkNo], M_DEFAULT, M_NUMBER + M_TYPE_LONG, &szCnt);
		// 		MmodGetResult(MilResultMod[iMarkNo], M_DEFAULT, M_TIMEOUT_END, &lTimeoutFlag);


		if (szCnt > 0)
		{
			for (i = 0; i < szCnt; i++)
			{
				cdCenterPos[nCount + i].x = 0;
				cdCenterPos[nCount + i].y = 0;
				fAngle[nCount + i] = 0;
				fScore[nCount + i] = 0;

				MmodGetResult(MilResultMod[iMarkNo], i, M_POSITION_X, &cdCenterPos[nCount + i].x);//제품 센터값-비전
				MmodGetResult(MilResultMod[iMarkNo], i, M_POSITION_Y, &cdCenterPos[nCount + i].y);
				MmodGetResult(MilResultMod[iMarkNo], i, M_ANGLE, &fAngle[nCount + i]);
				MmodGetResult(MilResultMod[iMarkNo], i, M_SCORE, &fScore[nCount + i]);

				if (pModelData->iModAcceptance > fScore[nCount + i])
				{
					iFailCnt++;
				}
			}
		}
		nCount += szCnt;
	}

	// 	//내림차순[5▶4▶3▶2▶1]
	// 	double  tmpNo = 0.0;
	// 	CPoint  tmpPnt;
	// 	int cnt = nCount;
	// 	double *array = fScore;
	// 	double *etc1 = fAngle;
	// 	CPoint etc2[200];
	// 	memcpy(etc2, cdCenterPos, sizeof(CPoint) * 200);
	// 
	// 	for (j = 0; j < (cnt - 1); j++)
	// 	{
	// 		for (i = 0; i<(cnt - 1); i++)
	// 		{
	// 			if (array[i + 1] > array[i])
	// 			{
	// 				tmpNo = array[i + 1];
	// 				array[i + 1] = array[i];
	// 				array[i] = tmpNo;
	// 
	// 				tmpNo = etc1[i + 1];
	// 				etc1[i + 1] = etc1[i];
	// 				etc1[i] = tmpNo;
	// 
	// 				tmpPnt = etc2[i + 1];
	// 				etc2[i + 1] = etc2[i];
	// 				etc2[i] = tmpPnt;
	// 			}
	// 		}
	// 	}
	// 	memcpy(fScore, array, sizeof(CPoint) * 200);
	// 	memcpy(fAngle, etc1, sizeof(CPoint) * 200);
	// 	memcpy(cdCenterPos, etc2, sizeof(CPoint) * 200);
	// 
	// 	nCount -= iFailCnt;
	// 
	pTask->cdTargetPos = cdCenterPos[0];
	// 
	// 	for (i = 0; i < nCount; i++)
	// 	{
	// 		if (fAngle[i] <= 90)
	// 			fAngle[i] = fAngle[i];
	// 		else if (fAngle[i] <= 180)
	// 			fAngle[i] = fAngle[i] - 180.0;
	// 		else if (fAngle[i] <= 270)
	// 			fAngle[i] = fAngle[i] - 180.0;
	// 		else if (fAngle[i] <= 360)
	// 			fAngle[i] = fAngle[i] - 360.0;
	// 	}
	// 
	pTask->fAlign_T = fAngle[0];

	sprintf_s(tmpStr, sizeof(TCHAR)*CHAR_BUFF, _T("Pattern Result -> MatchRate:%.3f / Angle:%.3f / PosX:%.3f / PosY:%.3f"), fScore[0], fAngle[0], cdCenterPos[0].x, cdCenterPos[0].y);

	ErrorCode = 0;
	if (nCount < 1)
	{
		sprintf_s(msgVision, sizeof(msgVision), _T("패턴이 검출되지 않았습니다."));
		ErrorCode = 1;
	}


	return cdCenterPos[0];
}

CMilInspFunc::CMilInspFunc()
{
	InitImgBuf();

	InitCal();

	ModInit(M_COLOR_YELLOW);

	// 	int i=0;
	// 	for (i=0; i<DEF_MAX_MARK_NUM; i++)
	// 	{
	// 		ModLoad(i);
	// 	}
}

CMilInspFunc::~CMilInspFunc()
{
	CloseMod();

	CloseCal();

	if (MilBuf_Mask)
	{
		MbufFree(MilBuf_Mask);
		MilBuf_Mask = M_NULL;
	}
	int i;
	//Buf Free
	for (i = DEF_MAX_MARK_NUM - 1; i >= 0; i--)
	{
		MbufFree(MilBuf_Mark[i]);
		MilBuf_Mark[i] = M_NULL;
	}

	GlobalUnlock(hImage_Insp);
	GlobalFree(hImage_Insp);

	if (MilBuf_Insp_Image)
	{
		MbufFree(MilBuf_Insp_Image);
		MilBuf_Insp_Image = M_NULL;
	}
}

void CMilInspFunc::InitImgBuf(int ImgBand /*= 1*/)
{
	int i = 0;

	long BufferAttributes;
	BufferAttributes = M_IMAGE + M_PROC;
	if (ImgBand == 3)
	{
		BufferAttributes += (M_PACKED + M_BGR24);
	}
	MbufAllocColor(pVision->MilSystem, ImgBand, DEF_IMAGE_X, DEF_IMAGE_Y, 8 + M_UNSIGNED, BufferAttributes, &MilBuf_Insp_Image);	//Top 검사할 실제 원본 이미지
	m_PitchX = (int)MbufInquire(MilBuf_Insp_Image, M_PITCH, M_NULL);//2464
	hImage_Insp = GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, (DEF_IMAGE_X * DEF_IMAGE_Y * ImgBand));
	if (hImage_Insp)
	{
		m_ImageInsp = (unsigned char *)GlobalLock(hImage_Insp);
		memset(m_ImageInsp, 0x00, sizeof(unsigned char)*(DEF_IMAGE_X * DEF_IMAGE_Y * ImgBand));
	}
	else
	{
		AfxMessageBox(_T("Memory Allocation Error[2]..."));
		return;
	}
	// Memory mapping...
	// 	m_ImageInsp = (unsigned char *)MbufInquire(MilBuf_Insp_Image, M_HOST_ADDRESS, M_NULL);
	MbufInquire(MilBuf_Insp_Image, M_HOST_ADDRESS, &m_ImageInsp);
	MbufClear(MilBuf_Insp_Image, 0);


	for (i = 0; i < DEF_MAX_MARK_NUM; i++)
	{
		MbufAlloc2d(pVision->MilSystem, DEF_IMAGE_X, DEF_IMAGE_Y, 8 + M_UNSIGNED, M_IMAGE + M_PROC, &MilBuf_Mark[i]);	//검사 마크 이미지
		if (MilBuf_Mark[i] != NULL)
			MbufClear(MilBuf_Mark[i], 0);
	}

	// 		m_PitchX = (int)MbufInquire(MilBuf_Insp_Image, M_PITCH, M_NULL);//2464

	MbufAlloc2d(pVision->MilSystem, DEF_MARK_SIZE_X, DEF_MARK_SIZE_Y, 8 + M_UNSIGNED, BufferAttributes, &MilBuf_Mask);
	MbufClear(MilBuf_Mask, 0);

}

void CMilInspFunc::CloseMod()
{
	int i;

	for (i = DEF_MAX_MARK_NUM - 1; i >= 0; i--)
	{
		//Mod free
		if (MilResultMod[i] != M_NULL)
		{
			MmodFree(MilResultMod[i]);
			MilResultMod[i] = M_NULL;
		}
		if (MilImageMod[i] != M_NULL)
		{
			MmodFree(MilImageMod[i]);
			MilImageMod[i] = M_NULL;
		}
	}

	MgraFree(MilmodResultgra);
	MgraFree(Milmodgra);
}

BOOL CMilInspFunc::BlobBlack(MIL_ID Inp)
{
	MIL_INT bb_num;

// 	MbufExport(_T("D:\\blob_ori.jpg"), M_JPEG_LOSSY, Inp);

	MIL_ID FeatureList, BlobResult;

	MblobAllocFeatureList(pVision->MilSystem, &FeatureList); 	MblobAllocResult(pVision->MilSystem, &BlobResult);
	MblobSelectFeature(FeatureList, M_BOX_X_MIN);	MblobSelectFeature(FeatureList, M_BOX_Y_MIN);
	MblobSelectFeature(FeatureList, M_BOX_X_MAX);	MblobSelectFeature(FeatureList, M_BOX_Y_MAX);
	MblobSelectFeature(FeatureList, M_FERET_X); 	MblobSelectFeature(FeatureList, M_FERET_Y);
	MblobSelectFeature(FeatureList, M_CENTER_OF_GRAVITY_X); 	MblobSelectFeature(FeatureList, M_CENTER_OF_GRAVITY_Y);
	MblobSelectFeature(FeatureList, M_MIN_PERIMETER_BOX_CENTER_X); 	MblobSelectFeature(FeatureList, M_MIN_PERIMETER_BOX_CENTER_Y);
	MblobSelectFeature(FeatureList, M_AREA + M_SORT1_DOWN);
	MblobSelectFeature(FeatureList, M_NUMBER_OF_RUNS);

	MimBinarize(Inp, Inp, M_BIMODAL + M_LESS_OR_EQUAL, M_NULL, 20);
// 	MbufExport(_T("D:\\blob_bin.jpg"), M_JPEG_LOSSY, Inp);
	MblobCalculate(Inp, M_NULL, FeatureList, BlobResult);

	MblobSelect(BlobResult, M_EXCLUDE, M_AREA, M_LESS_OR_EQUAL, 10, M_NULL);

	MblobGetNumber(BlobResult, &bb_num);
	MblobGetResult(BlobResult, M_BOX_X_MIN + M_TYPE_LONG, bb_stx);
	MblobGetResult(BlobResult, M_BOX_Y_MIN + M_TYPE_LONG, bb_sty);
	MblobGetResult(BlobResult, M_BOX_X_MAX + M_TYPE_LONG, bb_edx);
	MblobGetResult(BlobResult, M_BOX_Y_MAX + M_TYPE_LONG, bb_edy);
	MblobGetResult(BlobResult, M_FERET_X + M_TYPE_LONG, bb_szx);
	MblobGetResult(BlobResult, M_FERET_Y + M_TYPE_LONG, bb_szy);
	MblobGetResult(BlobResult, M_MIN_PERIMETER_BOX_CENTER_X + M_TYPE_LONG, bb_ctx);
	MblobGetResult(BlobResult, M_MIN_PERIMETER_BOX_CENTER_Y + M_TYPE_LONG, bb_cty);
	MblobGetResult(BlobResult, M_AREA + M_TYPE_LONG, bb_area);


	memset(&sp, 0x00, sizeof(SEGMENT_PARA));
	for (int i = 0; i < bb_num; i++)
	{
		if (i > MAX_BLOB_CNT)
			break;

		sp.minX[i] = (int)bb_stx[i];
		sp.minY[i] = (int)bb_sty[i];
		sp.maxX[i] = (int)bb_edx[i];
		sp.maxY[i] = (int)bb_edy[i];

		sp.sizeX[i] = (int)bb_szx[i];
		sp.sizeY[i] = (int)bb_szy[i];

		sp.center[i][0] = (int)bb_ctx[i];
		sp.center[i][1] = (int)bb_cty[i];
		sp.pixelCnt[i] = (int)bb_area[i];
	}

// 	if (bb_num > 0 && bb_num < MAX_CAND_NO)
// 	{
// 		memset(blobResXPtr, 0x00, sizeof(MIL_INT)*MAX_GIZ_PNT_CNT);
// 		memset(blobResYPtr, 0x00, sizeof(MIL_INT)*MAX_GIZ_PNT_CNT);
// 		memset(blobResLPtr, 0x00, sizeof(MIL_INT)*MAX_GIZ_PNT_CNT);
// 		MblobGetRuns(BlobResult, M_ALL, M_TYPE_MIL_INT, blobResXPtr, blobResYPtr, blobResLPtr);//옆으로(x방향) 연속된 픽셀의 시작점
// 	}

	MblobFree(BlobResult);
	MblobFree(FeatureList);

	return (long)bb_num;
}

BOOL CMilInspFunc::SearchEdge(int SearchXPnt, int SearchYPnt, double resEdgePnt[3])
{
	int iPos1, iPos2, iPos3, iPos4;
	int i, j;

	j = SearchYPnt;
	for (i = 10; i < DEF_IMAGE_X - 10; i++)
	{
		iPos1 = m_PitchX*j + i - 4;
		iPos2 = m_PitchX*j + i + 1;
		iPos3 = m_PitchX*j + i - 1;
		iPos4 = m_PitchX*j + i + 4;

		if (abs(m_ImageInsp[iPos1] - m_ImageInsp[iPos2]) > 50)
		{
			if (abs(m_ImageInsp[iPos3] - m_ImageInsp[iPos4]) > 50)
			{
				resEdgePnt[0] = (double)i;
				resEdgePnt[1] = (double)j;
				return TRUE;
			}
		}
	}

	i = SearchXPnt;
	for (j = 10; j < DEF_IMAGE_Y - 10; j++)
	{
		iPos1 = m_PitchX*(j- 4) + i ;
		iPos2 = m_PitchX*(j+ 1) + i ;
		iPos3 = m_PitchX*(j- 1) + i ;
		iPos4 = m_PitchX*(j+ 4) + i ;

		if (abs(m_ImageInsp[iPos1] - m_ImageInsp[iPos2]) > 50)
		{
			if (abs(m_ImageInsp[iPos3] - m_ImageInsp[iPos4]) > 50)
			{
				resEdgePnt[0] = (double)i;
				resEdgePnt[1] = (double)j;
				return TRUE;
			}
		}
	}

	return FALSE;
}

void CMilInspFunc::MilImageCheck(MIL_ID MilImage, MIL_ID MilDisp, const char* strValName /*= NULL */)
{
	TCHAR tmpStr[CHAR_BUFF];
	MIL_DOUBLE sX1, sX2;
	MIL_DOUBLE rateX;
	sX1 = (MIL_DOUBLE)MbufInquire(MilImage, M_SIZE_X, M_NULL);
	sX2 = (MIL_DOUBLE)MbufInquire(MilDisp, M_SIZE_X, M_NULL);//2464
	rateX = sX2 / sX1;

	sprintf_s(tmpStr, sizeof(tmpStr), (_T("D:\\Desk\\CheckImage_%s.tif")), strValName);
	MbufExport(tmpStr, M_TIFF, MilImage);

	MimResize(MilImage, MilDisp, rateX, rateX, M_DEFAULT);
	sprintf_s(tmpStr, sizeof(tmpStr), _T("Mil Image Check : %s"), strValName);
	AfxMessageBox(tmpStr);

	return;
}


extern /*"C" __declspec(dllexport)*/ void InitDLL()
{
	//pMainDlg = (CMilVisionDlg *)new CMilVisionDlg;

	//pMainDlg->Initial_ObjPtr();
}

extern /*"C" __declspec(dllexport)*/ void SetMasterPattern(unsigned char* ImageBuff, int PatternNum /*= 0*/)
{
	MbufPut(pInsp[DEF_DISP_SET]->MilBuf_Insp_Image, ImageBuff);

	CRect PatternRect;

	PatternRect.left = (DEF_IMAGE_X - DEF_MARK_SIZE_X) >> 1;
	PatternRect.top = (DEF_IMAGE_Y - DEF_MARK_SIZE_Y) >> 1;
	PatternRect.right = (DEF_IMAGE_X + DEF_MARK_SIZE_X) >> 1;
	PatternRect.bottom = (DEF_IMAGE_Y + DEF_MARK_SIZE_Y) >> 1;

	pInsp[DEF_DISP_SET]->ptrMarkStart = PatternRect.TopLeft();
	pInsp[DEF_DISP_SET]->ptrMarkSize.x = PatternRect.Width();
	pInsp[DEF_DISP_SET]->ptrMarkSize.y = PatternRect.Height();

	pInsp[DEF_DISP_SET]->ModRegist(PatternNum, M_COLOR_RED);
}

extern /*"C" __declspec(dllexport)*/ void SetMotorAlign(int GroupNum, double XPixArray[], double YPixArray[], double XWorldArray[], double YWorldArray[], unsigned char ImageBuff[][DEF_IMAGE_X*DEF_IMAGE_Y], int NumPoint)
{
	TCHAR tmpStr[CHAR_BUFF];
	int i;
	for (i = 0; i < NumPoint; i++)
	{
		if (pInsp[DEF_DISP_SET]->ErrorCode == 1)
			return;

		pTask->cdTargetPos.x = 0.0;
		pTask->cdTargetPos.y = 0.0;

//		pTask->cdTargetPos = GetPattAlignCenter(ImageBuff[i], 0);// i); //이미지 저장해두고 얼라인 하는 방식

		XWorldArray[i] += pTask->cdTargetPos.x;
		YWorldArray[i] += pTask->cdTargetPos.y;
		sprintf_s(tmpStr, _T("보정얼라인좌표#%d -> PosX:%.3f / PosY:%.3f"), i + 1, XWorldArray[i], YWorldArray[i]);
// 		pMILAlignDlg->putListData(tmpStr);
	}
	pInsp[DEF_DISP_SET]->Calibration(GroupNum, XPixArray, YPixArray, XWorldArray, YWorldArray, NumPoint);
}

extern /*"C" __declspec(dllexport)*/ CDPoint GetPattAlignCenter(unsigned char * ImageBuff, int PatternNum /*= 0*/)
{
	TCHAR tmpStr[CHAR_BUFF];
	CDPoint ModCenter;
	ModCenter.x = 0.0;
	ModCenter.y = 0.0;
	double MotorPxResol = pModelData->dImgResol;

	MbufPut(pInsp[DEF_DISP_SET]->MilBuf_Insp_Image, ImageBuff);

	pInsp[DEF_DISP_SET]->ErrorCode = 0;
	ModCenter = pInsp[DEF_DISP_SET]->ModFind(pInsp[DEF_DISP_SET]->MilBuf_Insp_Image, tmpStr, PatternNum);
	pMILAlignDlg->putListData(tmpStr);

	if (pInsp[DEF_DISP_SET]->ErrorCode == 1)
		return ModCenter;//패턴매칭이 실패하면 보정하지 않음.

	if (pInsp[DEF_DISP_SET]->nCount > 0)
	{
		MgraColor(pInsp[DEF_DISP_SET]->MilmodResultgra, M_COLOR_BLUE);
		MmodDraw(pInsp[DEF_DISP_SET]->MilmodResultgra, pInsp[DEF_DISP_SET]->MilResultMod[PatternNum], pDisplay[DEF_DISP_SET]->MilOvr[DEF_OVR_SET].MilOverlayImage, M_DRAW_EDGES /*+ M_DRAW_POSITION*/, M_DEFAULT, M_DEFAULT);
		MgraColor(pInsp[DEF_DISP_SET]->MilmodResultgra, M_COLOR_YELLOW);
		MmodDraw(pInsp[DEF_DISP_SET]->MilmodResultgra, pInsp[DEF_DISP_SET]->MilResultMod[PatternNum], pDisplay[DEF_DISP_SET]->MilOvr[DEF_OVR_SET].MilOverlayImage, /*M_DRAW_EDGES + */M_DRAW_POSITION, M_DEFAULT, M_DEFAULT);
	}
						 //영상얼라인으로 Update하기
	ModCenter.x = ModCenter.x - (double)((DEF_IMAGE_X) >> 1);
	ModCenter.y = ModCenter.y - (double)((DEF_IMAGE_Y) >> 1);
	ModCenter.x *= MotorPxResol;
	ModCenter.y *= MotorPxResol;

	//ModCenter.y *= -1; //Y축이 반대방향
	sprintf_s(tmpStr, sizeof(TCHAR)*CHAR_BUFF, _T("모터보정값 -> PosX:%.3f / PosY:%.3f"), ModCenter.x, ModCenter.y);
	pMILAlignDlg->putListData(tmpStr);


	return ModCenter;
}


extern /*"C" __declspec(dllexport)*/ void GetTransformCoordinate(int GroupNum, double x, double y, double* resX, double* resY, int tansformType/*=0*/)
{
	pInsp[DEF_DISP_SET]->GetCamTransformCoordinate(GroupNum, x, y, resX, resY, tansformType);

	return;
}


extern /*"C" __declspec(dllexport)*/ void CloseDLL()
{
	//pMainDlg->Close_Object();

	//if (pMainDlg != NULL)
	//{
	//	delete pMainDlg;
	//	pMainDlg = NULL;
	//}
}
#endif