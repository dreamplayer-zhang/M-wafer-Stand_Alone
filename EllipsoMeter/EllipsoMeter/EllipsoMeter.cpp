
// EllipsoMeter.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "EllipsoMeter.h"
#include "EllipsoMeterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEllipsoMeterApp

BEGIN_MESSAGE_MAP(CEllipsoMeterApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CEllipsoMeterApp ����

CEllipsoMeterApp::CEllipsoMeterApp()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
}


// ������ CEllipsoMeterApp ��ü�Դϴ�.

CEllipsoMeterApp theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0x7A911419, 0x40CA, 0x459B, { 0x8B, 0x62, 0xC, 0xB1, 0xAE, 0xC2, 0x29, 0xAC } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;


// CEllipsoMeterApp �ʱ�ȭ

BOOL CEllipsoMeterApp::InitInstance()
{
	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}


	// OLE ���̺귯���� �ʱ�ȭ�մϴ�.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);


	// ��ȭ ���ڿ� �� Ʈ�� �� �Ǵ�
	// �� ��� �� ��Ʈ���� ���ԵǾ� �ִ� ��� �� �����ڸ� ����ϴ�.
	CShellManager *pShellManager = new CShellManager;

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
	// ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.
	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));
	// �ڵ�ȭ �Ǵ� reg/unreg ����ġ�� ���� ������� ���� �м��մϴ�.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// ���� ���α׷��� /Embedding �Ǵ� /Automation ����ġ�� ���۵Ǿ����ϴ�.
	// ���� ���α׷��� �ڵ�ȭ ������ �����մϴ�.
	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
	{
		// CoRegisterClassObject()�� ���� Ŭ���� ���͸��� ����մϴ�.
		COleTemplateServer::RegisterAll();
	}
	// ���� ���α׷��� /Unregserver �Ǵ� /Unregister ����ġ�� ���۵Ǿ����ϴ�. ������Ʈ������ �׸���
	// �����մϴ�.
	else if (cmdInfo.m_nShellCommand == CCommandLineInfo::AppUnregister)
	{
		COleObjectFactory::UpdateRegistryAll(FALSE);
		AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor);
		return FALSE;
	}
	// ���� ���α׷��� ���� ���������� ���۵Ǿ��ų� �ٸ� ����ġ�� ���۵Ǿ����ϴ�(��: /Register
	// �Ǵ� /Regserver). typelibrary�� �����Ͽ� ������Ʈ�� �׸��� ������Ʈ�մϴ�.
	else
	{
		COleObjectFactory::UpdateRegistryAll();
		AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid);
		if (cmdInfo.m_nShellCommand == CCommandLineInfo::AppRegister)
			return FALSE;
	}

	CEllipsoMeterDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: ���⿡ [Ȯ��]�� Ŭ���Ͽ� ��ȭ ���ڰ� ������ �� ó����
		//  �ڵ带 ��ġ�մϴ�.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ���⿡ [���]�� Ŭ���Ͽ� ��ȭ ���ڰ� ������ �� ó����
		//  �ڵ带 ��ġ�մϴ�.
	}

	// ������ ���� �� �����ڸ� �����մϴ�.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ��ȭ ���ڰ� �������Ƿ� ���� ���α׷��� �޽��� ������ �������� �ʰ�  ���� ���α׷��� ���� �� �ֵ��� FALSE��
	// ��ȯ�մϴ�.
	return FALSE;
}

int CEllipsoMeterApp::ExitInstance()
{
	AfxOleTerm(FALSE);

	return CWinApp::ExitInstance();
}

CEllipsoMeterDlg* CEllipsoMeterApp::m_fnGetViewPointer()
{
	return m_pView;
}
void CEllipsoMeterApp::m_fnSetViewPointer( CEllipsoMeterDlg* pView )
{
	m_pView = pView;
}
