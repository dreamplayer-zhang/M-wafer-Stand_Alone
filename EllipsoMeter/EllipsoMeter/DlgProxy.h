
// DlgProxy.h: ��� ����
//

#pragma once

class CEllipsoMeterDlg;


// CEllipsoMeterDlgAutoProxy ��� ���

class CEllipsoMeterDlgAutoProxy : public CCmdTarget
{
	DECLARE_DYNCREATE(CEllipsoMeterDlgAutoProxy)

	CEllipsoMeterDlgAutoProxy();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.

// Ư���Դϴ�.
public:
	CEllipsoMeterDlg* m_pDialog;

// �۾��Դϴ�.
public:

// �������Դϴ�.
	public:
	virtual void OnFinalRelease();

// �����Դϴ�.
protected:
	virtual ~CEllipsoMeterDlgAutoProxy();

	// ������ �޽��� �� �Լ�

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CEllipsoMeterDlgAutoProxy)

	// ������ OLE ����ġ �� �Լ�

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

