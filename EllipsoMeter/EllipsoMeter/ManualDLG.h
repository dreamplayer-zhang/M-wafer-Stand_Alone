#pragma once


// CManualDLG ��ȭ �����Դϴ�.
#include "ElipsoMeterIF.h"
#include "ElipsoMeter_IO.h"
#include "ElipsoMeterRobotIF.h"
#include "afxwin.h"
class CManualDLG : public CDialogEx
{
	DECLARE_DYNAMIC(CManualDLG)
	void							m_fnSetInit(); 
	void							m_fnSetEnd( void );
	BOOL                            m_bShow;
	void							m_fnSetShowFlag( BOOL bShow );
	CElipsoMeterIF*                 m_pElipso;
	CElipsoMeter_IO*                m_pIO;
	CElipsoMeterRobotIF*            m_pElipsoR;

public:
	CManualDLG(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual ~CManualDLG();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_MANUAL_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnDsi();
	afx_msg void OnBnClickedBtnCameraCtrl();
	afx_msg void OnBnClickedBtnIoCtrl();
	afx_msg void OnBnClickedBtnAutoDel();
	afx_msg void OnBnClickedBtnLight();
	afx_msg void OnBnClickedBtnAlignCtrl();
	afx_msg void OnBnClickedBtnElipsoMeter();
	afx_msg void OnBnClickedBtnElipsometerIo();
	afx_msg void OnBnClickedBtnUserManage();
	afx_msg void OnBnClickedBtnElipsometerRobot();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton11();
	afx_msg void OnBnClickedButton12();
	afx_msg void OnBnClickedButton13();
	CButton m_ctrlCheckAutoMode;
	afx_msg void OnBnClickedCheckAutoMode();
	afx_msg void OnBnClickedButton17();
	afx_msg void OnBnClickedButton18();
	afx_msg void OnBnClickedButton16();
	afx_msg void OnBnClickedButton14();
	afx_msg void OnBnClickedButton15();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton21();
	afx_msg void OnBnClickedButton19();
	afx_msg void OnBnClickedButton20();
	afx_msg void OnBnClickedCheckZZeroMove();
	CButton m_ctrlCheckZzeroMove;
};
