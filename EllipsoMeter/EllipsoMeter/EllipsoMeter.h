
// EllipsoMeter.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.
#include "EllipsoMeterDlg.h"


// CEllipsoMeterApp:
// �� Ŭ������ ������ ���ؼ��� EllipsoMeter.cpp�� �����Ͻʽÿ�.
//

class CEllipsoMeterApp : public CWinApp
{
public:
	ULONG_PTR m_gdiplusToken;

	CEllipsoMeterApp();
	CEllipsoMeterDlg* m_fnGetViewPointer( void );
	void m_fnSetViewPointer( CEllipsoMeterDlg* pView );
	CEllipsoMeterDlg* m_pView;

// �������Դϴ�.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CEllipsoMeterApp theApp;