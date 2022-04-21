
// AlignModuleDlg.h : ��� ����
//

#pragma once
#include "resource.h"		// �� ��ȣ�Դϴ�.
#include "OpenCV_Lib.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "doublescrollbar.h"

#define		IMG_SIZE_X		1440
#define		IMG_SIZE_Y		1080

#if OPENCV

typedef enum PATT_PARA_
{
	Para_iDiffTh	=0,
	Para_iDiffPixel	,
	Para_iVal		,
	Para_iValDir	,
	Para_iScandir	,
	Para_dSigmaSTD	,
	Para_iNochKind	,

	Max_Para_Cnt_Pattern
}_PATT_PARA;

struct stcScroll
{
	BOOL	ValType; //0:int / 1: double
	int		iMin;
	int		iMax;
	int		iLine;
	int		iPage;
	int		iPos;
	double	dMin;
	double	dMax;
	double	dLine;
	double	dPage;
	double	dPos;
};

// CAlignModuleDlg ��ȭ ����
class CAlignModuleDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CAlignModuleDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

	void							m_fnSetShowFlag( BOOL bShow );

	void							m_fnSetInit( void ); 
	void							m_fnSetEnd( void );
	BOOL                            m_bShow;
	unsigned int                    m_nCameraCount;
	void							m_fnSetImageRotation( IplImage*  pSrcImage, IplImage*  pDestImage, int nAngle, double dScale );
	BOOL							m_fnGetAlignValue( int nInch, int iNochKind, double &dAngle );

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ALIGNMODULE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:

// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();

	CStatic m_ctrlDisp;
	void InitDisplay();

	CListCtrl m_ctrlListPara;
	CDoubleScrollBar m_ctrlScrollVal;
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void OnNMDblclkListPara(NMHDR *pNMHDR, LRESULT *pResult);

	struct stcListItem
	{
		TCHAR	name[CHAR_BUFF];
		int		iVal;
		double	dVal;
		int		ValType;//0:int / 1:double
	};
	void AddParaItem(int raw, int cal, stcListItem ParaData);
	stcListItem ReadParaItem( int raw, int cal );
	void GetListParaData();
	void SetListParaData();



	afx_msg void OnBnClickedButtonSavePara();
	afx_msg void OnBnClickedButtonInsp();
	afx_msg void OnBnClickedButtonLoadimg();
	afx_msg void OnBnClickedButtonSaveimg();
	CListBox m_ctrlListMsg;
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonDisconnect();
	afx_msg void OnBnClickedBtnView();
	CComboBox m_clWaperSize;
	afx_msg void OnBnClickedHide();
};

extern CAlignModuleDlg* pAlignModuleDlg;

#endif