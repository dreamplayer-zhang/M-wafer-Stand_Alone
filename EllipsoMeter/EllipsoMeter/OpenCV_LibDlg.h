
// OpenCV_LibDlg.h : 헤더 파일
//

#pragma once

// #define		DLG_POS_X_SUB		15
// #define		DLG_POS_Y_SUB		15
// #define		DLG_SIZE_X_SUB		400
// #define		DLG_SIZE_Y_SUB		300

// #define		m_iDispStPos		DLG_POS_X_SUB+DLG_SIZE_X_SUB+15
// #define		DLG_POS_Y_MAIN		15
// #define		DLG_SIZE_X_MAIN		1200
// #define		DLG_SIZE_Y_MAIN		DLG_SIZE_X_MAIN*IMG_SIZE_Y/IMG_SIZE_X

#if OPENCV
#define		DLG2DISP_GAP	10

struct stcInfoDlgInit
{
	int iImgSizeX;
	int iImgSizeY;
	int iImgCh;

	int iDispNum;
	int iDispStPosX;
	int iDispStPosY;
	int iDispSizeX;
	int iDispSizeY;
};

// COpenCV_LibDlg 대화 상자
class COpenCV_LibDlg : public CDialogEx
{
// 생성입니다.
public:
	COpenCV_LibDlg(stcInfoDlgInit stcInitInfo, CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DISPLAY_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


public:
	CWnd* m_pParentDlg;

	BOOL m_bConnectCam;

	int m_SizeX;
	int m_SizeY;
	int m_ImgCh;//이미지채널 B/W:1 Color:3

	int m_iDisplayNum;
	int m_iDispStPosX;
	int m_iDispStPosY;
	int m_iDispSizeX;
	int m_iDispSizeY;

	IplImage* ImgBuff_OverlayDisp;
	IplImage* ImgBuff_CurDisp;

	IplImage* ImgBuff_Grab;

	IplImage* ImgBuff_Original;


	float ImgSizeResol[2];
	CvPoint drawPt[4];
	BOOL bLClick, bDragPt;

	BOOL m_bSelectArea;
	BOOL m_bInsideCircle;

	BOOL m_bLiveMode;

	CWinThread* m_pDspThread;

	CString m_strCurFolderPath;
	CString	m_strCurLoadImgFileName;

	BOOL m_bAutoMode;

	float m_fDrawRadius;

	void DisplayImage(int IDC_PICTURE_TARGET, IplImage* targetMat, float* ImgResol);

	virtual BOOL DestroyWindow();
	afx_msg void OnBnClickedButtonImgLoad();
	afx_msg void OnBnClickedButtonImgSave();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPopMenuImageLoad();
	afx_msg void OnPopMenuImageSave();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

	CvPoint ConvertPnt(CPoint point); //CPoint -> CvPoint
	CPoint ConvertPnt_Click(CPoint point); //CPoint -> CPoint;

	UINT m_RadioAreaShape;
	afx_msg void OnBnClickedRadioAreaShape(UINT nID);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

UINT DisplayThread(LPVOID param);

extern COpenCV_LibDlg *pCvDlg;

#endif