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

//<<���ڼ���>>
//1. wchar_t *msg, COLORREF color, int flag : ������ ���� ( (flag!=1 && flag!=2)�̸� �Էµ� ����(ms)��ŭ �˸�â ���� �����. ��, flag<10 (1,2����) �̸� 500ms �⺻�� ) + 3: OK/NG �߰�
//2. int FontSize : ���� ũ�� (������ ��Ʈ������ ���� ũ�� ��ȭ�� �ٸ�)
//3. COLORREF Fcolor : ���� ���� (�⺻��:���)
//4. int dlgPosFlag : ���̾�α� �˾� ��ġ (0-����Dlg�� �߾�  /  1-ȭ���� �߾�  /  2-���콺��ġ(ȭ��������߸���� �������� �̵���)  /  3-ȭ���� �ϴ��߾�)
//*�߰��� ���ڴ� �⺻���� ������ ���������� �����ڵ�� �մ��� �ʾƵ� ��.
extern "C" _declspec(dllimport) bool TECmsgOut(wchar_t *msg, COLORREF color, int flag, int FontSize = 17, COLORREF Fcolor = RGB(255,255,255), int dlgPosFlag = 0);

#endif // !defined(AFX_MESSAGEOUTMODULE_H__C15BD3E5_8BD3_11D7_8233_0050BF5F97AF__INCLUDED_)
