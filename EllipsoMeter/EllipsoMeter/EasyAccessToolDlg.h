
// EasyAccessToolDlg.h : ��� ����
//
#if SPINNAKER_CAMERA
#pragma once
#include "FlirCamera.h"
#include "INI.h"

// CEasyAccessToolDlg ��ȭ ����
class CEasyAccessToolDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CEasyAccessToolDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

	void							m_fnSetShowFlag( BOOL bShow );

	void							m_fnSetInit( void ); 
	void							m_fnSetEnd( void );
	BOOL                            m_bShow;
	void							m_fnCameraInit( void );
	BOOL							m_fnGetIsConnect( void );
	void							m_fnSetConnect( BOOL bConnect );
	BYTE*							m_pBuffer;
	BYTE*							m_pDispBuffer;
	BOOL							m_bGrab;
	BOOL							m_bGrabed;


// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EASYACCESSTOOL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

public:
	CCamera m_Camera;
	CINI m_INI;

	bool m_bThWork;
	CWinThread* m_pDisplayThread_Cam;
	void OnDisplay();

	CComboBox m_cBoxExposureMode;
	CComboBox m_cBoxTriggerMode;
	CComboBox m_cBoxTriggerSource;
	CComboBox m_cBoxExposureAuto;
	CComboBox m_cBoxGainAuto;
	CComboBox m_cBoxTargetGreyAuto;
	CComboBox m_cBoxWhiteBalanceAuto;
	CComboBox m_cBoxExposureCompensationAuto;

	CListBox m_lBoxLog;
	void WriteLog(IN CString message);
private:
	int m_nWidth;
	int m_nHeight;
	int m_nBpp;

	bool m_bCrossHair;

	BITMAPINFO* m_pBitmapInfo;

	void OnCreateBitmapInfo(IN int nWidth, IN int nHeight, IN int nBpp);
	void GetParameters();
	void SetParameters();
	
// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnConnectionWindow();
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnWrite();
	afx_msg void OnBnClickedBtnRead();
	afx_msg void OnBnClickedCbtnCrossHair();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnCbnSelchangeComboAutoExposure();
	afx_msg void OnCbnSelchangeComboAutoGain();
	afx_msg void OnCbnSelchangeComboAutoTargetGrey();
	afx_msg void OnCbnSelchangeComboAutoWhiteBalance();
	afx_msg void OnCbnSelchangeComboExposureMode();
	afx_msg void OnCbnSelchangeComboTriggerMode();
	afx_msg void OnCbnSelchangeComboTriggerSource();
	afx_msg void OnBnClickedBtnIniLoad();
	afx_msg void OnBnClickedBtnIniSave();
	afx_msg void OnBnClickedCbtnSharpness();
	afx_msg void OnCbnSelchangeComboAutoExposureCompensation();
	afx_msg void OnBnClickedBtnHide();
};

#endif