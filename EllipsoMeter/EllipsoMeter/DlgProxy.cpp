
// DlgProxy.cpp : ���� ����
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
	
	// �ڵ�ȭ ��ü�� Ȱ��ȭ�Ǿ� �ִ� ���� ��� ���� ���α׷��� �����ϱ� ���� 
	//	�����ڿ��� AfxOleLockApp�� ȣ���մϴ�.
	AfxOleLockApp();

	// ���� ���α׷��� �� â �����͸� ���� ��ȭ ���ڿ� ����
	//  �׼����� �����ɴϴ�. ���Ͻ��� ���� �����͸� �����Ͽ�
	//  ��ȭ ���ڸ� ����Ű�� ��ȭ ������ �Ĺ� �����͸� �� ���Ͻ÷�
	//  �����մϴ�.
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
	// ��� ��ü�� OLE �ڵ�ȭ�� ��������� �� ���� ���α׷��� �����ϱ� ����
	// 	�Ҹ��ڰ� AfxOleUnlockApp�� ȣ���մϴ�.
	//  �̷��� ȣ��� �� ��ȭ ���ڰ� ������ �� �ֽ��ϴ�.
	if (m_pDialog != NULL)
		m_pDialog->m_pAutoProxy = NULL;
	AfxOleUnlockApp();
}

void CEllipsoMeterDlgAutoProxy::OnFinalRelease()
{
	// �ڵ�ȭ ��ü�� ���� ������ ������ �����Ǹ�
	// OnFinalRelease�� ȣ��˴ϴ�. �⺻ Ŭ�������� �ڵ����� ��ü�� �����մϴ�.
	// �⺻ Ŭ������ ȣ���ϱ� ���� ��ü�� �ʿ��� �߰� ���� �۾���
	// �߰��Ͻʽÿ�.

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(CEllipsoMeterDlgAutoProxy, CCmdTarget)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CEllipsoMeterDlgAutoProxy, CCmdTarget)
END_DISPATCH_MAP()

// ����: IID_IEllipsoMeter�� ���� ������ �߰��Ͽ�
//  VBA���� ���� ���� ���ε��� �����մϴ�.
//  �� IID�� .IDL ���Ͽ� �ִ� dispinterface�� GUID�� ��ġ�ؾ� �մϴ�.

// {575B25D2-8771-4C83-821D-0F7831018540}
static const IID IID_IEllipsoMeter =
{ 0x575B25D2, 0x8771, 0x4C83, { 0x82, 0x1D, 0xF, 0x78, 0x31, 0x1, 0x85, 0x40 } };

BEGIN_INTERFACE_MAP(CEllipsoMeterDlgAutoProxy, CCmdTarget)
	INTERFACE_PART(CEllipsoMeterDlgAutoProxy, IID_IEllipsoMeter, Dispatch)
END_INTERFACE_MAP()

// IMPLEMENT_OLECREATE2 ��ũ�ΰ� �� ������Ʈ�� StdAfx.h�� ���ǵ˴ϴ�.
// {3037E892-6752-4A08-8ACA-32D86ED59AE0}
IMPLEMENT_OLECREATE2(CEllipsoMeterDlgAutoProxy, "EllipsoMeter.Application", 0x3037e892, 0x6752, 0x4a08, 0x8a, 0xca, 0x32, 0xd8, 0x6e, 0xd5, 0x9a, 0xe0)


// CEllipsoMeterDlgAutoProxy �޽��� ó����
