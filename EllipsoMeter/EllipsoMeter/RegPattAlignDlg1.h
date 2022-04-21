#pragma once


// CRegPattAlignDlg1 대화 상자입니다.

class CRegPattAlignDlg1 : public CDialogEx
{
	DECLARE_DYNAMIC(CRegPattAlignDlg1)

public:
	CRegPattAlignDlg1(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CRegPattAlignDlg1();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_RPA_STEP1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
