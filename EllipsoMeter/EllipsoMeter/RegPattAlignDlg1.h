#pragma once


// CRegPattAlignDlg1 ��ȭ �����Դϴ�.

class CRegPattAlignDlg1 : public CDialogEx
{
	DECLARE_DYNAMIC(CRegPattAlignDlg1)

public:
	CRegPattAlignDlg1(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CRegPattAlignDlg1();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_RPA_STEP1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};
