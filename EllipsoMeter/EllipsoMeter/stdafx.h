
// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 일부 CString 생성자는 명시적으로 선언됩니다.

// MFC의 공통 부분과 무시 가능한 경고 메시지에 대한 숨기기를 해제합니다.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.
#include <afxext.h>         // MFC 확장입니다.

#include <afxdisp.h>        // MFC 자동화 클래스입니다.

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 공용 컨트롤에 대한 MFC 지원입니다.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // Windows 공용 컨트롤에 대한 MFC 지원입니다.
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC의 리본 및 컨트롤 막대 지원

#include <afxsock.h>            // MFC 소켓 확장
#include <math.h>

// #pragma warning(disable:4091)
// #pragma warning(disable:4996)
// #pragma warning(disable:4146)
// #pragma warning(disable:4477)
// #pragma warning(disable:4711)
#pragma warning(disable:4819)
// #pragma warning(disable:4244)
// #pragma warning(disable:4005)
// #pragma warning(disable:4800)
// #pragma warning(disable:4101)
// #pragma warning(disable:4307)

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <shellapi.h>
#include <tchar.h>
#include <windows.h> 
#include <direct.h>

#include <crtdbg.h>

#include <mmsystem.h>

//joystick
#include "define.h"
#pragma comment(lib, "winmm.lib")
BOOL JoyInit(HWND hwnd);

#include <iostream>
#include <fstream>
#include <thread>

#if OPENCV
#include "putUniText.h"
#include "CvvImage.h"
using namespace cv;
using namespace std;
#endif


#pragma comment(lib, "Jai_Factory.lib")
#pragma comment(lib, "gdiplus.lib") 

// #include <GdiPlus.h> 
// using namespace Gdiplus;

#include "Struct.h"
#include "UtilityClass.h"
#include "XmlDocument.h"
#include "LogProcess.h"
#include "resource.h" 
#include "MessageOutModule.h"
#include "fpspread_point.h"
// #include "CommonStruct.h"

#include <iostream>

CUtilityClass m_Util;


class CDPoint
{
public:
	double x, y;
	CDPoint()
	{
		x = 0.0;
		y = 0.0;
	}
};

void Delay(double msec);

long CalculateEnergy(unsigned char* Buf_TargetImg, int imgSizeX, int imgSizeY, int sx,
	int sy, int ex, int ey, int maskno, int nStep);

bool desc(pair<double, double> a, pair<double, double> b);

CString m_fnGetCurrentDrive();

void CountingImgFile(LPCTSTR directory, int bdNo, LPCTSTR FileFormat, int *pCnt, int mod);

//TRUE:원안//FALSE:원밖
BOOL inCircle(int in_x, int in_y, int CircleCenterX, int CircleCenterY, float radius);
BOOL inCircle(double in_x, double in_y, double CircleCenterX, double CircleCenterY, double radius);

//	pDlg = this;//현재dlg 포인터
// 	int ArrExcepID[5] = {
// 		IDC_LST_MODEL,
// 		IDC_BTN_ADD,
// 		IDC_BTN_RENAME,
// 		IDC_BTN_COPY,
// 		IDC_BTN_SAVE
// 	};
void EnableDlgItem(CWnd* pDlg, BOOL bEnable, int* ArrExcepID = nullptr, int ArrExcepCnt = 0);
int ChildWindowsCounting(HWND parentHwnd);
void ChildWindows(HWND parentHwnd, int CtrlCount, HWND* childHwnd);

class CTimeInterval
{
public:
	CTimeInterval();	// 표준 생성자입니다.
	~CTimeInterval();
private:
public:
	SYSTEMTIME ChkTime[2];	//GetSystemTime()으로 값얻기 추천
	double	dItvTime;

	SYSTEMTIME SetTimeStart();
	SYSTEMTIME SetTimeEnd();

	CString GetStrTime(SYSTEMTIME ntime);
	//GetTimeEnd내장
	//Unit : 0=100nsec 1=1msec 2=1sec

	double GetTimeInterval(int Unit = 1, BOOL AutoEndCheck = TRUE);
};


// 이 매크로는 COleObjectFactory 생성자에 대한 bMultiInstance 매개 변수에 TRUE를
// 전달하는 점만 제외하면 IMPLEMENT_OLECREATE와 같습니다.
// 자동화 컨트롤러에서 요청한 각 자동화 프록시 개체에 대해
// 이 응용 프로그램에 대한 별도의 인스턴스가 시작되도록 합니다.
#ifndef IMPLEMENT_OLECREATE2
#define IMPLEMENT_OLECREATE2(class_name, external_name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        AFX_DATADEF COleObjectFactory class_name::factory(class_name::guid, \
                RUNTIME_CLASS(class_name), TRUE, _T(external_name)); \
        const AFX_DATADEF GUID class_name::guid = \
                { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } };
#endif // IMPLEMENT_OLECREATE2

