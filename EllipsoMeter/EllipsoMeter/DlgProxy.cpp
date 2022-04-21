
// DlgProxy.cpp : 구현 파일
//

#include "stdafx.h"
#include "EllipsoMeter.h"
#include "DlgProxy.h"
#include "EllipsoMeterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEllipsoMeterDlgAutoProxy

IMPLEMENT_DYNCREATE(CEllipsoMeterDlgAutoProxy, CCmdTarget)

CEllipsoMeterDlgAutoProxy::CEllipsoMeterDlgAutoProxy()
{
	EnableAutomation();
	
	// 자동화 개체가 활성화되어 있는 동안 계속 응용 프로그램을 실행하기 위해 
	//	생성자에서 AfxOleLockApp를 호출합니다.
	AfxOleLockApp();

	// 응용 프로그램의 주 창 포인터를 통해 대화 상자에 대한
	//  액세스를 가져옵니다. 프록시의 내부 포인터를 설정하여
	//  대화 상자를 가리키고 대화 상자의 후방 포인터를 이 프록시로
	//  설정합니다.
	ASSERT_VALID(AfxGetApp()->m_pMainWnd);
	if (AfxGetApp()->m_pMainWnd)
	{
		ASSERT_KINDOF(CEllipsoMeterDlg, AfxGetApp()->m_pMainWnd);
		if (AfxGetApp()->m_pMainWnd->IsKindOf(RUNTIME_CLASS(CEllipsoMeterDlg)))
		{
			m_pDialog = reinterpret_cast<CEllipsoMeterDlg*>(AfxGetApp()->m_pMainWnd);
			m_pDialog->m_pAutoProxy = this;
		}
	}
}

CEllipsoMeterDlgAutoProxy::~CEllipsoMeterDlgAutoProxy()
{
	// 모든 개체가 OLE 자동화로 만들어졌을 때 응용 프로그램을 종료하기 위해
	// 	소멸자가 AfxOleUnlockApp를 호출합니다.
	//  이러한 호출로 주 대화 상자가 삭제될 수 있습니다.
	if (m_pDialog != NULL)
		m_pDialog->m_pAutoProxy = NULL;
	AfxOleUnlockApp();
}

void CEllipsoMeterDlgAutoProxy::OnFinalRelease()
{
	// 자동화 개체에 대한 마지막 참조가 해제되면
	// OnFinalRelease가 호출됩니다. 기본 클래스에서 자동으로 개체를 삭제합니다.
	// 기본 클래스를 호출하기 전에 개체에 필요한 추가 정리 작업을
	// 추가하십시오.

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(CEllipsoMeterDlgAutoProxy, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CEllipsoMeterDlgAutoProxy, CCmdTarget)
END_DISPATCH_MAP()

// 참고: IID_IEllipsoMeter에 대한 지원을 추가하여
//  VBA에서 형식 안전 바인딩을 지원합니다.
//  이 IID는 .IDL 파일에 있는 dispinterface의 GUID와 일치해야 합니다.

// {575B25D2-8771-4C83-821D-0F7831018540}
static const IID IID_IEllipsoMeter =
{ 0x575B25D2, 0x8771, 0x4C83, { 0x82, 0x1D, 0xF, 0x78, 0x31, 0x1, 0x85, 0x40 } };

BEGIN_INTERFACE_MAP(CEllipsoMeterDlgAutoProxy, CCmdTarget)
	INTERFACE_PART(CEllipsoMeterDlgAutoProxy, IID_IEllipsoMeter, Dispatch)
END_INTERFACE_MAP()

// IMPLEMENT_OLECREATE2 매크로가 이 프로젝트의 StdAfx.h에 정의됩니다.
// {3037E892-6752-4A08-8ACA-32D86ED59AE0}
IMPLEMENT_OLECREATE2(CEllipsoMeterDlgAutoProxy, "EllipsoMeter.Application", 0x3037e892, 0x6752, 0x4a08, 0x8a, 0xca, 0x32, 0xd8, 0x6e, 0xd5, 0x9a, 0xe0)


// CEllipsoMeterDlgAutoProxy 메시지 처리기
