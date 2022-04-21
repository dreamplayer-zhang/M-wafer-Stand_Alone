
// AlignModuleDlg.h : 헤더 파일
//

#pragma once
#include "resource.h"		// 주 기호입니다.
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

// CAlignModuleDlg 대화 상자
class CAlignModuleDlg : public CDialogEx
{
// 생성입니다.
public:
	CAlignModuleDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

	void							m_fnSetShowFlag( BOOL bShow );

	void							m_fnSetInit( void ); 
	void							m_fnSetEnd( void );
	BOOL                            m_bShow;
	unsigned int                    m_nCameraCount;
	void							m_fnSetImageRotation( IplImage*  pSrcImage, IplImage*  pDestImage, int nAngle, double dScale );
	BOOL							m_fnGetAlignValue( int nInch, int iNochKind, double &dAngle );

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ALIGNMODULE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
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