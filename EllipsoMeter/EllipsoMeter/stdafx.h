
// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������ 
// ��� �ִ� ���� �����Դϴ�.

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // �Ϻ� CString �����ڴ� ��������� ����˴ϴ�.

// MFC�� ���� �κа� ���� ������ ��� �޽����� ���� ����⸦ �����մϴ�.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC �ٽ� �� ǥ�� ���� ����Դϴ�.
#include <afxext.h>         // MFC Ȯ���Դϴ�.

#include <afxdisp.h>        // MFC �ڵ�ȭ Ŭ�����Դϴ�.

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // Windows ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC�� ���� �� ��Ʈ�� ���� ����

#include <afxsock.h>            // MFC ���� Ȯ��
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

//TRUE:����//FALSE:����
BOOL inCircle(int in_x, int in_y, int CircleCenterX, int CircleCenterY, float radius);
BOOL inCircle(double in_x, double in_y, double CircleCenterX, double CircleCenterY, double radius);

//	pDlg = this;//����dlg ������
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
	CTimeInterval();	// ǥ�� �������Դϴ�.
	~CTimeInterval();
private:
public:
	SYSTEMTIME ChkTime[2];	//GetSystemTime()���� ����� ��õ
	double	dItvTime;

	SYSTEMTIME SetTimeStart();
	SYSTEMTIME SetTimeEnd();

	CString GetStrTime(SYSTEMTIME ntime);
	//GetTimeEnd����
	//Unit : 0=100nsec 1=1msec 2=1sec

	double GetTimeInterval(int Unit = 1, BOOL AutoEndCheck = TRUE);
};


// �� ��ũ�δ� COleObjectFactory �����ڿ� ���� bMultiInstance �Ű� ������ TRUE��
// �����ϴ� ���� �����ϸ� IMPLEMENT_OLECREATE�� �����ϴ�.
// �ڵ�ȭ ��Ʈ�ѷ����� ��û�� �� �ڵ�ȭ ���Ͻ� ��ü�� ����
// �� ���� ���α׷��� ���� ������ �ν��Ͻ��� ���۵ǵ��� �մϴ�.
#ifndef IMPLEMENT_OLECREATE2
#define IMPLEMENT_OLECREATE2(class_name, external_name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        AFX_DATADEF COleObjectFactory class_name::factory(class_name::guid, \
                RUNTIME_CLASS(class_name), TRUE, _T(external_name)); \
        const AFX_DATADEF GUID class_name::guid = \
                { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } };
#endif // IMPLEMENT_OLECREATE2

