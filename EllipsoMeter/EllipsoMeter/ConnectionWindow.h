#if SPINNAKER_CAMERA
#pragma once
#include "EasyAccessToolDlg.h"

// ConnectionWindow 대화 상자입니다.

class ConnectionWindow : public CDialogEx
{
	DECLARE_DYNAMIC(ConnectionWindow)

public:
	ConnectionWindow(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~ConnectionWindow();

public:
	CListCtrl		m_LtcDevice;
	CIPAddressCtrl	m_PersistantIP;

	int m_nSelectPos;

	void OnInitial();
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CONNECTIONWINDOW };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnRefresh();
	afx_msg void OnBnClickedBtnForceip();
	afx_msg void OnBnClickedBtnConnect();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnPersistnat();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
#endif