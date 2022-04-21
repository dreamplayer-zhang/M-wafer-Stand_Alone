
// MilVisionDlg.h : 헤더 파일
//

#pragma once
#if MIL_USE
// #include "afxwin.h"
// #include "afxcmn.h"
#include "DoubleScrollBar.h"

typedef enum PATT_MIL_PARA_
{
	Para_iModNumber=0,
	Para_iModSmoothness,
	Para_iModAcceptance,
	Para_dModScaleMax,
	Para_dModScaleMin,
	Para_dModAngleDeltaNeg,
	Para_dModAngleDeltaPos,
	Para_dModReferenceAngle,
	Para_iModTimeout,
	Para_iModDntSize,

	Max_Mil_Para_Cnt
}_PATT_MIL_PARA;

// CMilVisionDlg 대화 상자
class CMilVisionDlg : public CDialog
{
// 생성입니다.
public:
	CMilVisionDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
	~CMilVisionDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_MILVISION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

	int m_iDispNum;

// 구현입니다.
protected:
// 	HICON m_hIcon;
	HANDLE								m_hMutexOneInstance;

	BOOL bLClick, bDragPt;
	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
// 	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	CPoint pntDrag[2];

public:
	void Initial_ObjPtr();
	void initDisplay();
	afx_msg void OnDestroy();

	void Close_Object();

	CRect rectDraw;
	CRect rcMark;
	afx_msg void OnBnClickedButtonPattRegi();
	void putListData(TCHAR *tmpStr);
	CListBox m_ctrlListResult;
	CComboBox m_ctrlComboPattNo;
	afx_msg void OnBnClickedButtonPattFind();
	afx_msg void PopupImageLoad();
	afx_msg void PopupImageSave();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedCheckDispCenter();
	CButton m_ctrlChkDispCenter;
	afx_msg void OnCbnSelchangeComboPatt();
	int m_iComboPattNo;

	CListCtrl m_ctrlListPara;
	void SetListParaData();
	void GetListParaData();

	struct stcListItem
	{
		TCHAR	name[CHAR_BUFF];
		int		iVal;
		double	dVal;
		int	ValType;//0:int / 1:double
	};
	void AddParaItem(int raw, int cal, stcListItem ParData);
	stcListItem ReadParaItem(int raw, int cal);

	afx_msg void OnBnClickedButtonSave();
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
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CDoubleScrollBar m_ctrlScrollVal;
	afx_msg void OnNMDblclkListPara(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedButtonCal();
	afx_msg void OnBnClickedButtonPointCali();
	double m_dEditImgResol;
	CEdit m_ctrlEditImgResol;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	BOOL FindWaferLine();
	BOOL FindWaferEdge(double dPntEdge[3]);
	afx_msg void OnBnClickedButtonFindEdgePnt();
	afx_msg void OnBnClickedButtonCurImgAplly();

	void m_fnBringImage(void* InputImgBuff, int iImgBuffSize);
	afx_msg void OnBnClickedButtonPatternLoad();

	BOOL m_bRegModFlag;
	afx_msg void OnBnClickedOk();

	void m_fnTranferCamBuff2PattBuff(BOOL bAutoPattAlignMode /*= FALSE*/);

	afx_msg void OnBnClickedButtonDegreeCal();
	CDPoint m_dDegCalPos[2];
	int m_iDegCalStep;
	afx_msg void OnBnClickedButtonReflashImg();
};

extern CMilVisionDlg* pMILAlignDlg;
#endif