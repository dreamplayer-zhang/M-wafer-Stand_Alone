#if SPINNAKER_CAMERA
#pragma once
#include "EasyAccessToolDlg.h"

// ConnectionWindow ��ȭ �����Դϴ�.

class ConnectionWindow : public CDialogEx
{
	DECLARE_DYNAMIC(ConnectionWindow)

public:
	ConnectionWindow(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~ConnectionWindow();

public:
	CListCtrl		m_LtcDevice;
	CIPAddressCtrl	m_PersistantIP;

	int m_nSelectPos;

	void OnInitial();
// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CONNECTIONWINDOW };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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