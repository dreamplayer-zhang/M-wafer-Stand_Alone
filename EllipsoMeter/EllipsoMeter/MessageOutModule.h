// MessageOutModule.h : main header file for the MESSAGEOUTMODULE DLL

#if !defined(AFX_MESSAGEOUTMODULE_H__C15BD3E5_8BD3_11D7_8233_0050BF5F97AF__INCLUDED_)
#define AFX_MESSAGEOUTMODULE_H__C15BD3E5_8BD3_11D7_8233_0050BF5F97AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

//Color
#define   BLACK			RGB(0,0,0)
#define   CYAN			RGB(0,128,128)

#define   RED_1			RGB(128,0,0)
#define   BLUE_1		RGB(0,0,128)
#define   GREEN_1		RGB(0,128,0)

#define   MAGENTA		RGB(128,0,128)
#define   BROWN			RGB(128,128,0)
#define   DARKGRAY		RGB(128,128,128)
#define   LIGHTGRAY		RGB(192,192,192)
#define   LIGHTBLUE		RGB(0,0,255)
#define   LIGHTGREEN	RGB(0,255,0)
#define   LIGHTCYAN		RGB(0,255,255)
#define   LIGHTRED		RGB(255,0,0)
#define   LIGHTMAGENTA	RGB(255,0,255)
#define   YELLOW		RGB(255,255,0)
#define   WHITE			RGB(255,255,255)

//<<인자설명>>
//1. wchar_t *msg, COLORREF color, int flag : 기존과 동일 ( (flag!=1 && flag!=2)이면 입력된 숫자(ms)만큼 알림창 떳다 사라짐. 단, flag<10 (1,2제외) 이면 500ms 기본값 ) + 3: OK/NG 추가
//2. int FontSize : 글자 크기 (설정된 폰트종류에 따라 크기 변화가 다름)
//3. COLORREF Fcolor : 글자 색상 (기본값:흰색)
//4. int dlgPosFlag : 다이얼로그 팝업 위치 (0-현재Dlg의 중앙  /  1-화면의 중앙  /  2-마우스위치(화면밖으로잘릴경우 안쪽으로 이동됨)  /  3-화면의 하단중앙)
//*추가된 인자는 기본값이 이전과 동일함으로 기존코드는 손대지 않아도 됨.
extern "C" _declspec(dllimport) bool TECmsgOut(wchar_t *msg, COLORREF color, int flag, int FontSize = 17, COLORREF Fcolor = RGB(255,255,255), int dlgPosFlag = 0);

#endif // !defined(AFX_MESSAGEOUTMODULE_H__C15BD3E5_8BD3_11D7_8233_0050BF5F97AF__INCLUDED_)
