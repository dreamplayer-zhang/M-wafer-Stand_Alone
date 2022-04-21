#pragma once
#if JAI_CAMERA
#include <Jai_Factory.h>
#include "afxwin.h"
#include <GDIPLUS.H>
#include <STRSAFE.H>

using namespace Gdiplus;

#define NODE_NAME_WIDTH         (int8_t*)"Width"
#define NODE_NAME_HEIGHT        (int8_t*)"Height"
#define NODE_NAME_PIXELFORMAT   (int8_t*)"PixelFormat"
#define NODE_NAME_EXP_TIME      (int8_t*)"ExposureTime"
#define NODE_NAME_ACQSTART      (int8_t*)"AcquisitionStart"
#define NODE_NAME_ACQSTOP       (int8_t*)"AcquisitionStop"
#define	NUM_OF_BUFFERS	5

// CCamJAI 대화 상자입니다.

class CCamJAI : public CDialogEx
{
	DECLARE_DYNAMIC(CCamJAI)

public:
	CCamJAI(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CCamJAI();

// 대화 상자 데이터입니다.
// #ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHILDWINDOWSAMPLE_DIALOG };
//#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

														// Implementation
public:
// 	CTaskHandle*					m_pTaskHandle;
	SHEET_RESISTANCE_Format*		m_pTask;
	BOOL                            m_bShow;

	FACTORY_HANDLE  m_hFactory;     // Factory Handle
	CAM_HANDLE      m_hCam;         // Camera Handle
	VIEW_HANDLE     m_hView;        // View window handle
	THRD_HANDLE		m_hThread;
	int8_t          m_sCameraId[J_CAMERA_ID_SIZE];    // Camera ID string
	int32_t         m_iStreamChannel;   // Stream channel number (default 0)
	STREAM_HANDLE   m_hDS;              // Handle to the data stream
	HANDLE          m_hStreamThread;    // Handle to the image acquisition thread
	HANDLE          m_hStreamEvent;     // Thread used to signal when image thread stops
	uint32_t        m_iValidBuffers;    // Number of buffers allocated to image acquisition
	uint8_t*        m_pAquBuffer[NUM_OF_BUFFERS];    // Buffers allocated to hold image data
	BUF_HANDLE      m_pAquBufferID[NUM_OF_BUFFERS];  // Handles for all the image buffers
	HANDLE	        m_hCondition;       // Condition used for getting the New Image Events
	bool            m_bEnableThread;    // Flag indicating if the image thread should run
	bool            m_bStreamStarted;   // Flag indicating that the J_Stream_StartAcquisition() has been called
	int64_t			m_iWidthInc;
	int64_t			m_iHeightInc;
	bool			m_bEnableStreaming;

	BOOL			m_bUseUI;

	BOOL OpenFactoryAndCamera();        // Open factory and search for cameras. Open first camera
	void CloseFactoryAndCamera();       // Close open camera and close factory to clean up
	void InitializeControls();          // Initialize control on the dialog
	void EnableControls(BOOL bIsCameraReady, BOOL bIsImageAcquiring);   // Update the control states

	BOOL CreateStreamThread(CAM_HANDLE hCam, uint32_t iChannel, uint32_t iBufferSize);
	BOOL TerminateStreamThread(void);   // Terminate the image acquisition thread
	void StreamProcess(void);           // The actual image acquisition thread
	void TerminateThread(void);
	void WaitForThreadToTerminate(void);
	void CloseThreadHandle(void);
	uint32_t PrepareBuffer(int iBufferSize);
	BOOL UnPrepareBuffer(void);
	void ShowErrorMsg(CString message, J_STATUS_TYPE error);

	void StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo);

	bool                                m_bProcess;
	int									m_nStartX;
	int									m_nStartY;
	int									m_nMaskno;
	int									m_nStep;
	long                                m_lResultFocus;

protected:
	HICON m_hIcon;
	int m_LeftMargin;
	int m_RightMargin;
	int m_TopMargin;
	int m_BottomMargin;
	bool m_Initialized;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
public:
	HWND m_HwndPic;
	afx_msg void OnBnClickedStart();
public:
	afx_msg void OnBnClickedStop();
public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CStatic m_PictureCtrl;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();

	void m_fnSetShowFlag(BOOL bShow);


public:
	CRITICAL_SECTION    m_CriticalSection;
	PIXELVALUE			m_PixelValue;
	uint64_t			m_PixelType;

	J_tIMAGE_INFO		m_CnvImageInfo;    // Image info structure
	BOOL				m_bLockedGrabBuf;
	BOOL				m_bGrabBufCpyConfirm;

	int					m_XPos;
	int					m_YPos;
	CDPoint				m_MouseClickPos;
	BOOL				m_bClickMoveFlag;

	CStatic m_ValueStaticCtrl;

	bool m_bEnableUserDrawImageGrid;
	bool m_bEnableUserDrawViewWindowGrid;
	CString m_ViewWindowEventText;
	CString m_UserDrawImageEventText;
	CString m_UserDrawViewWindowEventText;

	void ViewWindowNotificationCallback(VIEW_HANDLE hWin, J_IVW_CALLBACK_TYPE Type, void *pValue);

protected:
	GdiplusStartupInput m_gdiplusStartupInput;
	ULONG_PTR m_gdiplusToken;

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnConnectCam();
};
extern CCamJAI* pCamJAI;
#endif