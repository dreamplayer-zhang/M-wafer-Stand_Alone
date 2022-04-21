// CamJAI.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#if JAI_CAMERA
#include "EllipsoMeter.h"
#include "CamJAI.h"
#include "afxdialogex.h"
#include <VLib.h>
#include <vector>
#include "MilVision.h"
#include "Task.h"
#include "CommThread.h"

CCamJAI* pCamJAI;
//Utility function to set the frame grabber's width/height (if one is present in the system).
void SetFramegrabberValue(CAM_HANDLE hCam, int8_t* szName, int64_t int64Val, int8_t* sCameraId)
{
	//Set frame grabber value, if applicable
	DEV_HANDLE hDev = NULL; //If a frame grabber exists, it is at the GenTL "local device layer".
	J_STATUS_TYPE retval = J_Camera_GetLocalDeviceHandle(hCam, &hDev);
	if (J_ST_SUCCESS != retval)
		return;

	if (NULL == hDev)
		return;

	NODE_HANDLE hNode;
	retval = J_Camera_GetNodeByName(hDev, szName, &hNode);
	if (J_ST_SUCCESS != retval)
		return;

	retval = J_Node_SetValueInt64(hNode, false, int64Val);
	if (J_ST_SUCCESS != retval)
		return;

	//Special handling for Active Silicon CXP boards, which also has nodes prefixed
	//with "Incoming":
	std::string strTransportName((char*)sCameraId);
	if (std::string::npos != strTransportName.find("TLActiveSilicon"))
	{
		std::string strName((char*)szName);
		if (std::string::npos != strName.find("Width")
			|| std::string::npos != strName.find("Height"))
		{
			std::string strIncoming = "Incoming" + strName;
			NODE_HANDLE hNodeIncoming;
			J_STATUS_TYPE retval = J_Camera_GetNodeByName(hDev, (int8_t*)strIncoming.c_str(), &hNodeIncoming);
			if (retval == J_ST_SUCCESS)
			{
				retval = J_Node_SetValueInt64(hNodeIncoming, false, int64Val);
			}
		}

	}//if(std::string::npos != strTransportName.find("TLActiveSilicon"))
}

//Utility function to set the frame grabber's pixel format (if one is present in the system).
void SetFramegrabberPixelFormat(CAM_HANDLE hCam, int8_t* szName, int64_t jaiPixelFormat, int8_t* sCameraId)
{
	DEV_HANDLE hDev = NULL; //If a frame grabber exists, it is at the GenTL "local device layer".
	J_STATUS_TYPE retval = J_Camera_GetLocalDeviceHandle(hCam, &hDev);
	if (J_ST_SUCCESS != retval)
		return;

	if (NULL == hDev)
		return;

	int8_t szJaiPixelFormatName[512];
	uint32_t iSize = 512;
	retval = J_Image_Get_PixelFormatName(hCam, jaiPixelFormat, szJaiPixelFormatName, iSize);
	if (J_ST_SUCCESS != retval)
		return;

	NODE_HANDLE hLocalDeviceNode = 0;
	retval = J_Camera_GetNodeByName(hDev, (int8_t *)"PixelFormat", &hLocalDeviceNode);
	if (J_ST_SUCCESS != retval)
		return;

	if (0 == hLocalDeviceNode)
		return;

	//NOTE: this may fail if the camera and/or frame grabber does not use the SFNC naming convention for pixel formats!
	//Check the camera and frame grabber for details.
	retval = J_Node_SetValueString(hLocalDeviceNode, false, szJaiPixelFormatName);
	if (J_ST_SUCCESS != retval)
		return;

	//Special handling for Active Silicon CXP boards, which also has nodes prefixed
	//with "Incoming":
	std::string strTransportName((char*)sCameraId);
	if (std::string::npos != strTransportName.find("TLActiveSilicon"))
	{
		std::string strIncoming = std::string("Incoming") + std::string((char*)szName);
		NODE_HANDLE hNodeIncoming;
		J_STATUS_TYPE retval = J_Camera_GetNodeByName(hDev, (int8_t*)strIncoming.c_str(), &hNodeIncoming);
		if (retval == J_ST_SUCCESS)
		{
			//NOTE: this may fail if the camera and/or frame grabber does not use the SFNC naming convention for pixel formats!
			//Check the camera and frame grabber for details.
			retval = J_Node_SetValueString(hNodeIncoming, false, szJaiPixelFormatName);
		}
	}
}

//============================================================================
// Thread Process Caller
//============================================================================
DWORD ProcessCaller(CCamJAI* pThread)
{
	pThread->StreamProcess();

	return 0;
}


// CCamJAI 대화 상자입니다.

IMPLEMENT_DYNAMIC(CCamJAI, CDialogEx)

CCamJAI::CCamJAI(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCamJAI::IDD, pParent)
{
	m_hFactory = NULL;
	m_hCam = NULL;
	m_hView = NULL;
	m_hThread = NULL;

	m_hStreamThread = NULL;
	m_hStreamEvent = NULL;
	m_hDS = NULL;
	m_iStreamChannel = 0;
	m_hCondition = NULL;
	m_bStreamStarted = false;

	m_LeftMargin = 0;
	m_RightMargin = 0;
	m_TopMargin = 0;
	m_BottomMargin = 0;
	m_Initialized = false;

	m_iWidthInc = 1;
	m_iHeightInc = 1;
	m_bEnableStreaming = false;

	m_bShow = FALSE;
	m_bClickMoveFlag = FALSE;

	m_bEnableUserDrawImageGrid = false;
	m_bEnableUserDrawViewWindowGrid = true;
	m_CnvImageInfo.pImageBuffer = NULL;

	m_bUseUI = FALSE;

	// We use a critical section to protect the update of the pixel value display
	InitializeCriticalSection(&m_CriticalSection);

	// Initialize GDI+
	GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);



	this->Create(IDD_CHILDWINDOWSAMPLE_DIALOG);

	m_bLockedGrabBuf = FALSE;
	m_bGrabBufCpyConfirm = FALSE;
}

CCamJAI::~CCamJAI()
{
}

void CCamJAI::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICTURE_CTRL, m_PictureCtrl);
}


BEGIN_MESSAGE_MAP(CCamJAI, CDialogEx)
	ON_WM_PAINT()
	// 	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_START, &CCamJAI::OnBnClickedStart)
	ON_BN_CLICKED(IDC_STOP, &CCamJAI::OnBnClickedStop)
	ON_WM_HSCROLL()
// 	ON_WM_SIZE()
	// 	ON_BN_CLICKED(IDCANCEL, &CCamJAI::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK2, &CCamJAI::OnBnClickedOk)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_CONNECT_CAM, &CCamJAI::OnBnClickedBtnConnectCam)
END_MESSAGE_MAP()


// CCamJAI 메시지 처리기입니다.
//--------------------------------------------------------------------------------------------------
// OnInitDialog
//--------------------------------------------------------------------------------------------------
BOOL CCamJAI::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
// 	SetIcon(m_hIcon, TRUE);			// Set big icon
// 	SetIcon(m_hIcon, FALSE);		// Set small icon

									// We want to be able to resize the Picture Control automatically so here we find the margins


	CRect mainWindowRect;
	CRect clientWindowRect;
	this->SetWindowPos(NULL, 1920, 100, 0, 0, SWP_NOSIZE);
	this->GetClientRect(&mainWindowRect);
	this->ClientToScreen(&mainWindowRect);

	GetDlgItem(IDC_PICTURE_CTRL)->GetClientRect(&clientWindowRect);
	GetDlgItem(IDC_PICTURE_CTRL)->SetWindowPos(NULL, 0, 0, clientWindowRect.Width(), clientWindowRect.Width(), SWP_NOMOVE);
	GetDlgItem(IDC_PICTURE_CTRL)->GetClientRect(&clientWindowRect);
	GetDlgItem(IDC_PICTURE_CTRL)->ClientToScreen(&clientWindowRect);
	
	m_LeftMargin = clientWindowRect.left - mainWindowRect.left;
	m_RightMargin = mainWindowRect.right - clientWindowRect.right;
	m_TopMargin = clientWindowRect.top - mainWindowRect.top;
	m_BottomMargin = mainWindowRect.bottom - clientWindowRect.bottom;
	m_Initialized = true;


	BOOL retval;

	// Open factory & camera
	retval = OpenFactoryAndCamera();
	if (retval)
	{
		// Display camera ID
		GetDlgItem(IDC_CAMERAID)->SetWindowText(CString((char*)m_sCameraId));

		// Enable Controls
		EnableControls(TRUE, FALSE);
	}
	else
	{
		// Write error message in textbox
		GetDlgItem(IDC_CAMERAID)->SetWindowText(CString("Error opening factory and camera"));

		// Disable controls
		EnableControls(FALSE, FALSE);
	}
	InitializeControls();   // Initialize Controls

	EnterCriticalSection(&m_CriticalSection);
	m_PixelValue.PixelValueUnion.Mono16Type.Value = 0;
	m_PixelType = J_GVSP_PIX_MONO8;
	LeaveCriticalSection(&m_CriticalSection);


	return TRUE;  // return TRUE  unless you set the focus to a control
}
//--------------------------------------------------------------------------------------------------
// OnDestroy
//--------------------------------------------------------------------------------------------------
void CCamJAI::OnDestroy()
{
	CDialog::OnDestroy();

	// Stop acquisition
	OnBnClickedStop();

	// Close factory & camera
	CloseFactoryAndCamera();

	DeleteCriticalSection(&m_CriticalSection);

	GdiplusShutdown(m_gdiplusToken);

}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCamJAI::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
// 		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
// HCURSOR CCamJAI::OnQueryDragIcon()
// {
// 	return static_cast<HCURSOR>(m_hIcon);
// }

void CCamJAI::ShowErrorMsg(CString message, J_STATUS_TYPE error)
{
	CString errorMsg;
	errorMsg.Format(_T("%s: Error = %d: "), message, error);

	switch (error)
	{
	case J_ST_INVALID_BUFFER_SIZE:	errorMsg += "Invalid buffer size ";	                break;
	case J_ST_INVALID_HANDLE:		errorMsg += "Invalid handle ";		                break;
	case J_ST_INVALID_ID:			errorMsg += "Invalid ID ";			                break;
	case J_ST_ACCESS_DENIED:		errorMsg += "Access denied ";		                break;
	case J_ST_NO_DATA:				errorMsg += "No data ";				                break;
	case J_ST_ERROR:				errorMsg += "Generic error ";		                break;
	case J_ST_INVALID_PARAMETER:	errorMsg += "Invalid parameter ";	                break;
	case J_ST_TIMEOUT:				errorMsg += "Timeout ";				                break;
	case J_ST_INVALID_FILENAME:		errorMsg += "Invalid file name ";	                break;
	case J_ST_INVALID_ADDRESS:		errorMsg += "Invalid address ";		                break;
	case J_ST_FILE_IO:				errorMsg += "File IO error ";		                break;
	case J_ST_GC_ERROR:				errorMsg += "GenICam error ";		                break;
	case J_ST_VALIDATION_ERROR:		errorMsg += "Settings File Validation Error ";		break;
	case J_ST_VALIDATION_WARNING:	errorMsg += "Settings File Validation Warning ";    break;
	}

	AfxMessageBox(errorMsg, MB_OKCANCEL | MB_ICONINFORMATION);

}

void CCamJAI::StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo)
{
// 	memcpy(m_pSrc->imageData, pAqImageInfo->pImageBuffer, pAqImageInfo->iSizeX * pAqImageInfo->iSizeY);
// 	if (m_bCapture)
// 	{
// 		cvSaveImage("D:\\1.bmp", m_pSrc);
// 		m_bCapture = false;
// 	}

	if (!m_bProcess)
	{
		m_nStartX = 0;
		m_nStartY = 0;
		m_nMaskno = 0;
		m_nStep = 5;
		m_bProcess = true;
		int nSize = pAqImageInfo->iSizeX * pAqImageInfo->iSizeY;
		unsigned char *pBuffer = new unsigned char[nSize];
		memset(pBuffer, 0x00, sizeof(nSize));
		memcpy(pBuffer, pAqImageInfo->pImageBuffer, nSize);
		m_lResultFocus = CalculateEnergy(pBuffer, pAqImageInfo->iSizeX, pAqImageInfo->iSizeY, m_nStartX, m_nStartY,
			pAqImageInfo->iSizeX, pAqImageInfo->iSizeY, m_nMaskno, m_nStep);

		m_UserDrawViewWindowEventText.Format("%ld", m_lResultFocus);
// 		GetDlgItem(IDC_EDT_AGV_CONTRAST)->SetWindowText(m_strContrast);

// 		if (m_bAF)
// 		{
// 			m_bAFSave = false;
// 			if (m_lResult > m_lMaxContrast)
// 			{
// 				m_bAFSave = true;
// 				m_lMaxContrast = m_lResult;
// 			}
// 			m_bAF = false;
// 		}
		delete pBuffer;
		pBuffer = NULL;
		m_bProcess = false;
	}

// 	m_cImage.CopyOf(m_pSrc, 1);
}

//--------------------------------------------------------------------------------------------------
// OpenFactoryAndCamera
//--------------------------------------------------------------------------------------------------
BOOL CCamJAI::OpenFactoryAndCamera()
{
	J_STATUS_TYPE   retval;
	uint32_t        iSize;
	uint32_t        iNumDev;
	bool8_t         bHasChange;

	m_bEnableStreaming = false;

	// Open factory
	retval = J_Factory_Open((int8_t*)"", &m_hFactory);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(CString("Could not open factory!"), retval);
		return FALSE;
	}
	TRACE("Opening factory succeeded\n");

	//Update camera list
	retval = J_Factory_UpdateCameraList(m_hFactory, &bHasChange);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(CString("Could not update camera list!"), retval);
		return FALSE;
	}
	TRACE("Updating camera list succeeded\n");

	// Get the number of Cameras
	retval = J_Factory_GetNumOfCameras(m_hFactory, &iNumDev);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(CString("Invalid number of cameras!"), retval);
		return FALSE;
	}

	if (iNumDev == 0)
	{
		ShowErrorMsg(CString("No cameras were found."), retval);
		return FALSE;
	}
	TRACE("%d cameras were found\n", iNumDev);

	// Get camera ID
	iSize = (uint32_t)sizeof(m_sCameraId);
	retval = J_Factory_GetCameraIDByIndex(m_hFactory, 0, m_sCameraId, &iSize);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(CString("Could not get the camera ID!"), retval);
		return FALSE;
	}
	TRACE("Camera ID: %s\n", m_sCameraId);

	// Open camera
	retval = J_Camera_Open(m_hFactory, m_sCameraId, &m_hCam);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(CString("Could not open the camera!"), retval);
		return FALSE;
	}

	//Make sure streaming is supported!
	uint32_t numStreams = 0;
	retval = J_Camera_GetNumOfDataStreams(m_hCam, &numStreams);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(CString("Error with J_Camera_GetNumOfDataStreams."), retval);
		return FALSE;
	}

	if (0 == numStreams)
	{
		m_bEnableStreaming = false;
	}
	else
	{
		m_bEnableStreaming = true;
	}

	if (iNumDev > 0 && 0 != m_hCam)
	{
		TRACE("Opening camera succeeded\n");
	}
	else
	{
		ShowErrorMsg(CString("Invalid number of Devices!"), iNumDev);
		return FALSE;
	}

	return TRUE;
}
//--------------------------------------------------------------------------------------------------
// CloseFactoryAndCamera
//--------------------------------------------------------------------------------------------------
void CCamJAI::CloseFactoryAndCamera()
{
    J_STATUS_TYPE   retval;
    if (m_hCam)
    {
        // Close camera
        retval = J_Camera_Close(m_hCam);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not close camera!"), retval);
        }
        m_hCam = NULL;
        TRACE("Closed camera\n");
    }

    if (m_hFactory)
    {
        // Close factory
        retval = J_Factory_Close(m_hFactory);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not close factory!"), retval);
        }
        m_hFactory = NULL;
        TRACE("Closed factory\n");
    }
}

//--------------------------------------------------------------------------------------------------
// OnBnClickedStart
//--------------------------------------------------------------------------------------------------
afx_msg void CCamJAI::OnBnClickedStart()
{
	if (m_bStreamStarted)
		return;
// 	OnBnClickedStop();

	J_STATUS_TYPE   retval;
	int64_t int64Val;

	SIZE	ViewSize;
	RECT    PictureRect;

	if (!m_bEnableStreaming)
	{
		ShowErrorMsg(CString("Streaming not enabled on this device."), 0);
		return;
	}

	// Get Width from the camera based on GenICam name
	retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_WIDTH, &int64Val);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Could not get Width!"), retval);
        return;
    }
	ViewSize.cx = (LONG)int64Val;     // Set window size cx

									  //Set frame grabber dimension, if applicable
	SetFramegrabberValue(m_hCam, NODE_NAME_WIDTH, int64Val, &m_sCameraId[0]);

    // Get Height from the camera
    retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_HEIGHT, &int64Val);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Could not get Height!"), retval);
        return;
    }
    ViewSize.cy = (LONG)int64Val;     // Set window size cy

									  //Set frame grabber dimension, if applicable
	SetFramegrabberValue(m_hCam, NODE_NAME_HEIGHT, int64Val, &m_sCameraId[0]);

	// Get the pixelformat from the camera
	int64_t pixelFormat = 0;
	uint64_t jaiPixelFormat = 0;
	retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_PIXELFORMAT, &pixelFormat);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(CString("Unable to get PixelFormat value!"), retval);
		return;
	}
	J_Image_Get_PixelFormat(m_hCam, pixelFormat, &jaiPixelFormat);

	//Set frame grabber pixel format, if applicable
	SetFramegrabberPixelFormat(m_hCam, NODE_NAME_PIXELFORMAT, pixelFormat, &m_sCameraId[0]);

	// Calculate number of bits (not bytes) per pixel using macro
	int bpp = J_BitsPerPixel(jaiPixelFormat);

	// Get the client window rectangle to be used for the display
	
	
	if (m_bUseUI)
	{
		if (m_bShow)
			m_HwndPic = m_PictureCtrl.GetSafeHwnd();
		CWnd::FromHandle(m_HwndPic)->GetClientRect(&PictureRect);

		// Open view window  //J_IVW_OVERLAPPED_STRETCH//새창만들기
		retval = J_Image_OpenViewWindowEx(J_IVW_CHILD_STRETCH, _T("Image View Window"), &PictureRect, &ViewSize, m_HwndPic, &m_hView);
		if (retval != J_ST_SUCCESS) {
			AfxMessageBox(CString("Could not open view window!"), MB_OK | MB_ICONEXCLAMATION);
			return;
		}
		TRACE("Opening view window succeeded\n");

		// Assign the View Window notification callback
		retval = J_Image_SetViewWindowCallback(m_hView, reinterpret_cast<J_IVW_CALLBACK_OBJECT>(this), reinterpret_cast<J_IVW_CALLBACK_FUNCTION>(&CCamJAI::ViewWindowNotificationCallback));
		if (retval != J_ST_SUCCESS) {
			ShowErrorMsg(CString("Could not set the view window notification callback!"), retval);
			return;
		}
		TRACE("View window notification callback assignment succeeded\n");
	}

//     // Set View Window options
//     if (m_hView != NULL)
//     {
//         J_Image_SetViewWindowOption(m_hView, J_IVW_MOUSE_ZOOM, &m_bEnableMouseZoom);
//         J_Image_SetViewWindowOption(m_hView, J_IVW_MOUSE_CURSOR, &m_bEnableMouseCursor);
//         J_Image_SetViewWindowOption(m_hView, J_IVW_DOUBLE_BUFFERING, &m_bEnableDoubleBuffering);
// 
//         SetupViewWindowCallbackMask();
//     }
	
	
	// Create image acquisition thread (this allocates buffers)
	if (!CreateStreamThread(m_hCam, 0, (ViewSize.cx*ViewSize.cy*bpp) / 8))
	{
		AfxMessageBox(CString("Could not open start stream!"), MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	// Wait for the stream thread to start the acquisition
	while (!m_bStreamStarted)
		Sleep(10);
	TRACE("Opening stream succeeded\n");

	// Start Acquisition
	retval = J_Camera_ExecuteCommand(m_hCam, NODE_NAME_ACQSTART);

	if (m_hView)
	{
		CRect mainWindowRect;
		CRect clientWindowRect;
		CWnd::FromHandle(m_HwndPic)->GetParent()->GetClientRect(&mainWindowRect);
		CWnd::FromHandle(m_HwndPic)->GetParent()->ClientToScreen(&mainWindowRect);
		CWnd::FromHandle(m_HwndPic)->GetClientRect(&clientWindowRect);
		CWnd::FromHandle(m_HwndPic)->ClientToScreen(&clientWindowRect);
		m_LeftMargin = clientWindowRect.left - mainWindowRect.left;
		m_RightMargin = mainWindowRect.right - clientWindowRect.right;
		m_TopMargin = clientWindowRect.top - mainWindowRect.top;
		m_BottomMargin = mainWindowRect.bottom - clientWindowRect.bottom;


		RECT childRect;
		childRect.top = 0;
		childRect.left = 0;
		childRect.right = mainWindowRect.Width() - (m_RightMargin + m_LeftMargin);
		childRect.bottom = mainWindowRect.Height() - (m_TopMargin + m_BottomMargin);

		J_Image_ResizeChildWindow(m_hView, &childRect);

		// Enable controls
		EnableControls(TRUE, TRUE);
	}

//	SetTimer(1, 100, NULL);

}

//--------------------------------------------------------------------------------------------------
// OnBnClickedStop
//--------------------------------------------------------------------------------------------------
void CCamJAI::OnBnClickedStop()
{
	J_STATUS_TYPE retval;

	if (!m_bEnableStreaming)
	{
		return;
	}

	// Start Acquisition
	if (m_hCam) {
		retval = J_Camera_ExecuteCommand(m_hCam, NODE_NAME_ACQSTOP);
	}

	// Close stream (this frees all allocated buffers)
	TerminateStreamThread();
	TRACE("Closed stream\n");

	// View window opened?
	if (m_hView)
	{
		// Close view window
		retval = J_Image_CloseViewWindow(m_hView);
		m_hView = NULL;
		TRACE("Closed view window\n");

		EnableControls(TRUE, FALSE);
	}

}

//--------------------------------------------------------------------------------------------------
// InitializeControls
//--------------------------------------------------------------------------------------------------
void CCamJAI::InitializeControls()
{
	J_STATUS_TYPE   retval;
	NODE_HANDLE hNode;
	int64_t int64Val;
	double dVal;

	CSliderCtrl* pSCtrl;

	//- Width ------------------------------------------------

	// Get SliderCtrl for Width
	pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_WIDTH);
	if (m_bEnableStreaming)
	{
		// Get Width Node
		retval = J_Camera_GetNodeByName(m_hCam, NODE_NAME_WIDTH, &hNode);
		if (retval == J_ST_SUCCESS)
		{
			// Get/Set Min
			retval = J_Node_GetMinInt64(hNode, &int64Val);
			pSCtrl->SetRangeMin((int)int64Val, TRUE);

			// Get/Set Max
			retval = J_Node_GetMaxInt64(hNode, &int64Val);
			pSCtrl->SetRangeMax((int)int64Val, TRUE);

			// Get/Set Value
			retval = J_Node_GetValueInt64(hNode, FALSE, &int64Val);
			pSCtrl->SetPos((int)int64Val);

			SetDlgItemInt(IDC_WIDTH, (int)int64Val);

			retval = J_Node_GetInc(hNode, &m_iWidthInc);
			m_iWidthInc = max(1, (int32_t)m_iWidthInc);

			//Set frame grabber dimension, if applicable
			SetFramegrabberValue(m_hCam, NODE_NAME_WIDTH, int64Val, &m_sCameraId[0]);
		}
		else
		{
			ShowErrorMsg(CString("Could not get Width node!"), retval);

			pSCtrl->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_LBL_WIDTH)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_WIDTH)->ShowWindow(SW_HIDE);
		}
	}
	else
	{
		pSCtrl->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LBL_WIDTH)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_WIDTH)->ShowWindow(SW_HIDE);
	}

	//- Height -----------------------------------------------
	// Get SliderCtrl for Height
	pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_HEIGHT);

	if (m_bEnableStreaming)
	{
		// Get Height Node
		retval = J_Camera_GetNodeByName(m_hCam, NODE_NAME_HEIGHT, &hNode);
		if (retval == J_ST_SUCCESS && m_bEnableStreaming)
		{
			// Get/Set Min
			retval = J_Node_GetMinInt64(hNode, &int64Val);
			pSCtrl->SetRangeMin((int)int64Val, TRUE);

			// Get/Set Max
			retval = J_Node_GetMaxInt64(hNode, &int64Val);
			pSCtrl->SetRangeMax((int)int64Val, TRUE);

			// Get/Set Value
			retval = J_Node_GetValueInt64(hNode, FALSE, &int64Val);
			pSCtrl->SetPos((int)int64Val);

			SetDlgItemInt(IDC_HEIGHT, (int)int64Val);

			retval = J_Node_GetInc(hNode, &m_iHeightInc);
			m_iHeightInc = max(1, (int32_t)m_iHeightInc);

			//Set frame grabber dimension, if applicable
			SetFramegrabberValue(m_hCam, NODE_NAME_HEIGHT, int64Val, &m_sCameraId[0]);
		}
		else
		{
			ShowErrorMsg(CString("Could not get Height node!"), retval);

			pSCtrl->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_LBL_HEIGHT)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_HEIGHT)->ShowWindow(SW_HIDE);
		}
	}
	else
	{
		pSCtrl->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LBL_HEIGHT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_HEIGHT)->ShowWindow(SW_HIDE);
	}

	//- ExposureTime -----------------------------------------------

	// Get SliderCtrl for ExposureTime
	pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_GAIN);

	// Get ExposureTime Node
	retval = J_Camera_GetNodeByName(m_hCam, NODE_NAME_EXP_TIME, &hNode);
	if (retval == J_ST_SUCCESS)
	{
		// Get/Set Min
		retval = J_Node_GetMinDouble(hNode, &dVal);
		pSCtrl->SetRangeMin((int)dVal, TRUE);

		// Get/Set Max
		retval = J_Node_GetMaxDouble(hNode, &dVal);
		pSCtrl->SetRangeMax((int)dVal, TRUE);

		// Get/Set Value
		retval = J_Node_GetValueDouble(hNode, FALSE, &dVal);
		pSCtrl->SetPos((int)dVal);

		SetDlgItemInt(IDC_GAIN, (int)dVal);
	}
	else
	{
		pSCtrl->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LBL_GAIN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_GAIN)->ShowWindow(SW_HIDE);
	}
}

//--------------------------------------------------------------------------------------------------
// EnableControls
//--------------------------------------------------------------------------------------------------
void CCamJAI::EnableControls(BOOL bIsCameraReady, BOOL bIsImageAcquiring)
{
	GetDlgItem(IDC_START)->EnableWindow((bIsCameraReady && m_bEnableStreaming) ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
	GetDlgItem(IDC_STOP)->EnableWindow((bIsCameraReady && m_bEnableStreaming) ? (bIsImageAcquiring ? TRUE : FALSE) : FALSE);
	GetDlgItem(IDC_SLIDER_WIDTH)->EnableWindow(bIsCameraReady ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
	GetDlgItem(IDC_WIDTH)->EnableWindow(bIsCameraReady ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
	GetDlgItem(IDC_SLIDER_HEIGHT)->EnableWindow(bIsCameraReady ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
	GetDlgItem(IDC_HEIGHT)->EnableWindow(bIsCameraReady ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
	GetDlgItem(IDC_SLIDER_GAIN)->EnableWindow(bIsCameraReady ? TRUE : FALSE);
	GetDlgItem(IDC_GAIN)->EnableWindow(bIsCameraReady ? TRUE : FALSE);
}

//--------------------------------------------------------------------------------------------------
// OnHScroll
//--------------------------------------------------------------------------------------------------
void CCamJAI::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// Get SliderCtrl for Width
	CSliderCtrl* pSCtrl;
	int iPos;
	J_STATUS_TYPE   retval;
	int64_t int64Val;
	double dVal;

	//- Width ------------------------------------------------

	// Get SliderCtrl for Width
	pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_WIDTH);
	if (pSCtrl == (CSliderCtrl*)pScrollBar) {

		iPos = pSCtrl->GetPos();
		int64Val = (int64_t)iPos;

		//To avoid J_Camera_SetValueInt64 errors,
		//make the slider value be an even multiple of the increment + min value
		int64_t inc = m_iWidthInc;
		if (inc > 1)
		{
			int64_t iMin = pSCtrl->GetRangeMin();

			int64_t diff = int64Val - iMin;

			ldiv_t divResult = div((long)diff, (long)inc);
			int64Val = iMin + (divResult.quot * inc);

			if (iPos != (int)int64Val)
			{
				pSCtrl->SetPos((int)int64Val);
			}
		}

		// Set Width value
		retval = J_Camera_SetValueInt64(m_hCam, NODE_NAME_WIDTH, int64Val);
		if (retval != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("Could not set Width!"), retval);
		}

		//Set frame grabber dimension, if applicable
		SetFramegrabberValue(m_hCam, NODE_NAME_WIDTH, int64Val, &m_sCameraId[0]);

		SetDlgItemInt(IDC_WIDTH, iPos);
	}

	//- Height -----------------------------------------------

	// Get SliderCtrl for Height
	pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_HEIGHT);
	if (pSCtrl == (CSliderCtrl*)pScrollBar) {

		iPos = pSCtrl->GetPos();
		int64Val = (int64_t)iPos;

		//To avoid J_Camera_SetValueInt64 errors,
		//make the slider value be an even multiple of the increment + min value
		int64_t inc = m_iHeightInc;
		if (inc > 1)
		{
			int64_t iMin = pSCtrl->GetRangeMin();

			int64_t diff = int64Val - iMin;

			ldiv_t divResult = div((long)diff, (long)inc);
			int64Val = iMin + (divResult.quot * inc);

			if (iPos != (int)int64Val)
			{
				pSCtrl->SetPos((int)int64Val);
			}
		}

		// Set Height Value
		retval = J_Camera_SetValueInt64(m_hCam, NODE_NAME_HEIGHT, int64Val);
		if (retval != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("Could not set Height!"), retval);
		}

		//Set frame grabber dimension, if applicable
		SetFramegrabberValue(m_hCam, NODE_NAME_HEIGHT, int64Val, &m_sCameraId[0]);

		SetDlgItemInt(IDC_HEIGHT, iPos);
	}

	//- ExposureTime -----------------------------------------------

	// Get SliderCtrl for ExposureTime
	pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_GAIN);
	if (pSCtrl == (CSliderCtrl*)pScrollBar) {

		iPos = pSCtrl->GetPos();
		dVal = (double)iPos;

		// Get ExposureTime Node
		retval = J_Camera_SetValueDouble(m_hCam, NODE_NAME_EXP_TIME, dVal);

		SetDlgItemInt(IDC_GAIN, iPos);
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

//--------------------------------------------------------------------------------------------------
// Create Stream Thread
//--------------------------------------------------------------------------------------------------
BOOL CCamJAI::CreateStreamThread(CAM_HANDLE hCam, uint32_t iChannel, uint32_t iBufferSize)
{
	// Is it already created?
	if (m_hStreamThread)
		return FALSE;

	// Open the stream channel(GVSP)
	if (m_hDS == NULL)
	{
		if ((J_ST_SUCCESS != J_Camera_CreateDataStream(hCam, iChannel, &m_hDS))
			|| (m_hDS == NULL))
		{
			OutputDebugString(_T("*** Error : J_Camera_CreateDataStream\n"));
			return FALSE;
		}
	}

	// Prepare the frame buffers (this announces the buffers to the acquisition engine)
	if (0 == PrepareBuffer(iBufferSize))
	{
		OutputDebugString(_T("*** Error : PrepareBuffer\n"));
		J_DataStream_Close(m_hDS);
		return FALSE;
	}

	// Stream thread event created?
	if (m_hStreamEvent == NULL)
		m_hStreamEvent = CreateEvent(NULL, true, false, NULL);
	else
		ResetEvent(m_hStreamEvent);

	// Set the thread execution flag
	m_bEnableThread = true;

	// Create a Stream Thread.
	if (NULL == (m_hStreamThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ProcessCaller, this, NULL, NULL)))
	{
		OutputDebugString(_T("*** Error : CreateThread\n"));
		J_DataStream_Close(m_hDS);
		return FALSE;
	}

	return TRUE;
}

//==============================================================////
// Terminate Stream Thread
//==============================================================////
BOOL CCamJAI::TerminateStreamThread(void)
{
	J_STATUS_TYPE   retval;

	// Is the data stream opened?
	if (m_hDS == NULL)
		return FALSE;

	// Reset the thread execution flag.
	m_bEnableThread = false;

	// Signal the image thread to stop faster
	if (m_hCondition)
	{
		retval = J_Event_ExitCondition(m_hCondition);
		if (retval != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("Could not exit Condition!"), retval);
		}
	}

	// Stop the image acquisition engine
	retval = J_DataStream_StopAcquisition(m_hDS, ACQ_STOP_FLAG_KILL);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(CString("Could not stop DataStream acquisition!"), retval);
	}

	// Mark stream acquisition as stopped
	m_bStreamStarted = false;

	// Wait for the thread to end
	WaitForThreadToTerminate();

	// UnPrepare Buffers (this removed the buffers from the acquisition engine and frees buffers)
	UnPrepareBuffer();

	// Close Stream
	if (m_hDS)
	{
		retval = J_DataStream_Close(m_hDS);
		if (retval != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("Could not close DataStream!"), retval);
		}
		m_hDS = NULL;
	}

	return TRUE;
}

//==============================================================////
// Stream Processing Function
//==============================================================////
void CCamJAI::StreamProcess(void)
{
	J_STATUS_TYPE	iResult;
	uint32_t        iSize;
	BUF_HANDLE	    iBufferID;
	uint64_t	    iQueued = 0;

	// Create structure to be used for image display
	J_tIMAGE_INFO	tAqImageInfo = { 0, 0, 0, 0, NULL, 0, 0, 0, 0, 0, 0 };

	J_COND_WAIT_RESULT	WaitResult;

	// Create the condition used for signaling the new image event
	iResult = J_Event_CreateCondition(&m_hCondition);

	EVT_HANDLE	hEvent;					// Buffer event handle
	EVENT_NEW_BUFFER_DATA eventData;	// Strut for EventGetData

										// Register the event with the acquisition engine
	uint32_t iAcquisitionFlag = ACQ_START_NEXT_IMAGE;

	iResult = J_DataStream_RegisterEvent(m_hDS, EVENT_NEW_BUFFER, m_hCondition, &hEvent);

	// Start image acquisition
	iResult = J_DataStream_StartAcquisition(m_hDS, iAcquisitionFlag, ULLONG_MAX);

	// Mark stream acquisition as started
	m_bStreamStarted = true;

	// Loop of Stream Processing
	OutputDebugString(_T(">>> Start Stream Process Loop.\n"));

	const uint64_t iTimeout = 1000;
	uint64_t jaiPixelFormat = 0;

	while (m_bEnableThread)
	{
		// Wait for Buffer event (or kill event)
		iResult = J_Event_WaitForCondition(m_hCondition, iTimeout, &WaitResult);

		// Did we get a new buffer event?
		if (J_COND_WAIT_SIGNAL == WaitResult)
		{
			// Get the Buffer Handle from the event
			iSize = (uint32_t)sizeof(EVENT_NEW_BUFFER_DATA);
			iResult = J_Event_GetData(hEvent, &eventData, &iSize);

			iBufferID = eventData.BufferHandle;

			// Did we receive the event data?
			if (iResult == J_ST_SUCCESS)
			{
				// Fill in structure for image display

				// Get the pointer to the frame buffer.
				uint64_t infoValue = 0;
				iSize = sizeof(void *);
				iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_BASE, &(infoValue), &iSize);
				tAqImageInfo.pImageBuffer = (uint8_t*)infoValue;
				if (GC_ERR_SUCCESS != iResult)
				{
					OutputDebugString(_T("Error with J_DataStream_GetBufferInfo in CStreamThread::StreamProcess ==> BUFFER_INFO_BASE.\n"));
					continue;
				}

				// Get the effective data size.
				iSize = sizeof(size_t);
				iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_SIZE, &(infoValue), &iSize);
				tAqImageInfo.iImageSize = (uint32_t)infoValue;
				if (GC_ERR_SUCCESS != iResult)
				{
					OutputDebugString(_T("Error with J_DataStream_GetBufferInfo in CStreamThread::StreamProcess ==> BUFFER_INFO_SIZE.\n"));
					continue;
				}

				// Get Frame Width.
				iSize = sizeof(size_t);
				iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_WIDTH, &(infoValue), &iSize);
				tAqImageInfo.iSizeX = (uint32_t)infoValue;
				if (GC_ERR_SUCCESS != iResult)
				{
					OutputDebugString(_T("Error with J_DataStream_GetBufferInfo in CStreamThread::StreamProcess ==> BUFFER_INFO_WIDTH.\n"));
					continue;
				}

				// Get Frame Height.
				iSize = sizeof(size_t);
				iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_HEIGHT, &(infoValue), &iSize);
				tAqImageInfo.iSizeY = (uint32_t)infoValue;
				if (GC_ERR_SUCCESS != iResult)
				{
					OutputDebugString(_T("Error with J_DataStream_GetBufferInfo in CStreamThread::StreamProcess ==> BUFFER_INFO_HEIGHT.\n"));
					continue;
				}

				// Get Timestamp.
				iSize = sizeof(infoValue);
				iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_TIMESTAMP, &(infoValue), &iSize);
				tAqImageInfo.iTimeStamp = infoValue;

				// Get Pixel Format Type.
				try
				{
					iSize = sizeof(uint64_t);
					iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_PIXELTYPE, &(infoValue), &iSize);
					if (GC_ERR_SUCCESS != iResult)
					{
						OutputDebugString(_T("Error with J_DataStream_GetBufferInfo in CStreamThread::StreamProcess ==> BUFFER_INFO_PIXELTYPE - assuming GVSP_PIX_MONO8.\n"));
						tAqImageInfo.iPixelType = GVSP_PIX_MONO8;
					}

					//Convert the camera's pixel format value to one understood by the JAI SDK.
					iResult = J_Image_Get_PixelFormat(m_hCam, infoValue, &jaiPixelFormat);

					tAqImageInfo.iPixelType = jaiPixelFormat;

					if (GC_ERR_SUCCESS != iResult)
					{
						OutputDebugString(_T("Error with J_Image_Get_PixelFormat in CStreamThread::StreamProcess ==> BUFFER_INFO_PIXELTYPE - assuming GVSP_PIX_MONO8.\n"));
						tAqImageInfo.iPixelType = GVSP_PIX_MONO8;
					}
				}
				catch (...)
				{
					OutputDebugString(_T("Exception with J_DataStream_GetBufferInfo in CStreamThread::StreamProcess ==> BUFFER_INFO_PIXELTYPE - assuming GVSP_PIX_MONO8.\n"));
					tAqImageInfo.iPixelType = GVSP_PIX_MONO8;
				}

				if(!m_bLockedGrabBuf)
				{
					memcpy(&m_CnvImageInfo, &tAqImageInfo, sizeof(J_tIMAGE_INFO));
					m_bGrabBufCpyConfirm = TRUE;
				}


				if (m_bEnableThread)
				{
					// Display image
					if (m_hView)
					{
						// Shows image
						J_Image_ShowImage(m_hView, &tAqImageInfo);
					}
				}

				// Queue This Buffer Again for reuse in acquisition engine
				iResult = J_DataStream_QueueBuffer(m_hDS, iBufferID);

// 				if (!m_bProcess)
				{
					m_nStartX = 500;
					m_nStartY = 500;
					m_nMaskno = 0;
					m_nStep = 5;
// 					m_bProcess = true;
					int nSize = tAqImageInfo.iSizeX * tAqImageInfo.iSizeY;
					unsigned char *pBuffer = new unsigned char[nSize];
					memset(pBuffer, 0x00, sizeof(nSize));
					memcpy(pBuffer, tAqImageInfo.pImageBuffer, nSize);
					m_lResultFocus = CalculateEnergy(pBuffer, tAqImageInfo.iSizeX, tAqImageInfo.iSizeY, m_nStartX, m_nStartY,
						tAqImageInfo.iSizeX-500, tAqImageInfo.iSizeY-500, m_nMaskno, m_nStep);

					m_UserDrawViewWindowEventText.Format("%ld", m_lResultFocus);

					delete pBuffer;
					pBuffer = NULL;
// 					m_bProcess = false;
				}


			}//if (iResult == J_ST_SUCCESS)

			 // Get # of frames awaiting delivery
			iSize = sizeof(size_t);
			iResult = J_DataStream_GetStreamInfo(m_hDS, STREAM_INFO_CMD_NUMBER_OF_FRAMES_AWAIT_DELIVERY, &iQueued, &iSize);
		}
		else
		{
			switch (WaitResult)
			{
				// Kill event
			case	WAIT_OBJECT_0 + 1:
				iResult = 1;
				break;
				// Timeout
			case	WAIT_TIMEOUT:
				iResult = 2;
				break;
				// Unknown?
			default:
				iResult = 3;
				break;
			}
		}
	}
	OutputDebugString(_T(">>> Terminated Stream Process Loop.\n"));

	m_bGrabBufCpyConfirm = FALSE;

	// Unregister new buffer event with acquisition engine
	J_DataStream_UnRegisterEvent(m_hDS, EVENT_NEW_BUFFER);

	// Free the event object
	if (hEvent != NULL)
	{
		J_Event_Close(hEvent);
		hEvent = NULL;
	}

	// Terminate the thread.
	TerminateThread();

	// Free the Condition
	if (m_hCondition != NULL)
	{
		iResult = J_Event_CloseCondition(m_hCondition);
		m_hCondition = NULL;
	}
}

//==============================================================////
// Terminate image acquisition thread
//==============================================================////
void CCamJAI::TerminateThread(void)
{
	SetEvent(m_hStreamEvent);
}

//==============================================================////
// Wait for thread to terminate
//==============================================================////
void CCamJAI::WaitForThreadToTerminate(void)
{
	WaitForSingleObject(m_hStreamEvent, INFINITE);

	// Close the thread handle and stream event handle
	CloseThreadHandle();
}

//==============================================================////
// Close handles and stream
//==============================================================////
void CCamJAI::CloseThreadHandle(void)
{
	if (m_hStreamThread)
	{
		CloseHandle(m_hStreamThread);
		m_hStreamThread = NULL;
	}

	if (m_hStreamEvent)
	{
		CloseHandle(m_hStreamEvent);
		m_hStreamEvent = NULL;
	}
}

//==============================================================////
// Prepare frame buffers
//==============================================================////
uint32_t CCamJAI::PrepareBuffer(int iBufferSize)
{
	J_STATUS_TYPE	iResult = J_ST_SUCCESS;
	int			i;

	m_iValidBuffers = 0;

	for (i = 0; i < NUM_OF_BUFFERS; i++)
	{
		// Make the buffer for one frame. 
		m_pAquBuffer[i] = new uint8_t[iBufferSize];

		// Announce the buffer pointer to the Acquisition engine.
		if (J_ST_SUCCESS != J_DataStream_AnnounceBuffer(m_hDS, m_pAquBuffer[i], iBufferSize, NULL, &(m_pAquBufferID[i])))
		{
			delete m_pAquBuffer[i];
			break;
		}

		// Queueing it.
		if (J_ST_SUCCESS != J_DataStream_QueueBuffer(m_hDS, m_pAquBufferID[i]))
		{
			delete m_pAquBuffer[i];
			break;
		}

		m_iValidBuffers++;
	}

	return m_iValidBuffers;
}

//==============================================================////
// Unprepare buffers
//==============================================================////
BOOL CCamJAI::UnPrepareBuffer(void)
{
	void		*pPrivate;
	void		*pBuffer;
	uint32_t	i;

	// Flush Queues
	J_DataStream_FlushQueue(m_hDS, ACQ_QUEUE_INPUT_TO_OUTPUT);
	J_DataStream_FlushQueue(m_hDS, ACQ_QUEUE_OUTPUT_DISCARD);

	for (i = 0; i < m_iValidBuffers; i++)
	{
		// Remove the frame buffer from the Acquisition engine.
		J_DataStream_RevokeBuffer(m_hDS, m_pAquBufferID[i], &pBuffer, &pPrivate);

		delete m_pAquBuffer[i];
		m_pAquBuffer[i] = NULL;
		m_pAquBufferID[i] = 0;
	}

	m_iValidBuffers = 0;

	return TRUE;
}

void CCamJAI::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (m_Initialized)
	{
		CRect mainWindowRect;
		this->GetWindowRect(&mainWindowRect);

		//// Here we need to resize the Picture Control as well as resize the Child Window image display
		m_PictureCtrl.MoveWindow(m_LeftMargin, m_TopMargin, cx - ((m_LeftMargin + m_RightMargin)>>1), cy - ((m_TopMargin + m_BottomMargin)>>1));

		if (m_hView)
		{
			RECT childRect;
			childRect.top = 0;
			childRect.left = 0;
			childRect.right = mainWindowRect.Width() - (m_RightMargin + m_LeftMargin);
			childRect.bottom = mainWindowRect.Height() - (m_TopMargin + m_BottomMargin);

			J_Image_ResizeChildWindow(m_hView, &childRect);
		}
	}
}

void CCamJAI::OnBnClickedCancel()
{
	OnCancel();
}

void CCamJAI::OnBnClickedOk()
{
	m_bUseUI = FALSE;
	m_bShow = FALSE;
	OnOK();
}


void CCamJAI::m_fnSetShowFlag(BOOL bShow)
{
	m_bShow = bShow;
	if (m_bShow)
	{
		pCamJAI->m_bUseUI = TRUE;
		this->ShowWindow(SW_SHOW);
	}
	else
	{
		pCamJAI->m_bUseUI = FALSE;
		this->ShowWindow(SW_HIDE);
	}
}


void CCamJAI::ViewWindowNotificationCallback(VIEW_HANDLE hWin, J_IVW_CALLBACK_TYPE Type, void *pValue)
{
	HDC hDC;

	// Handle the different View Window events!
	// The event data type depends on the type of event.
	switch (Type)
	{
// 	case J_IVW_CB_WINDOW_MOVE:
// 		EnterCriticalSection(&m_CriticalSection);
// 		m_ViewWindowEventText.Format(_T("View Window Move event: Pos = (%d,%d)"), ((POINT*)pValue)->x, ((POINT*)pValue)->y);
// 		LeaveCriticalSection(&m_CriticalSection);
// 		break;
// 
// 	case J_IVW_CB_WINDOW_SIZE:
// 		EnterCriticalSection(&m_CriticalSection);
// 		m_ViewWindowEventText.Format(_T("View Window Size event: Size = (%d,%d)"), ((SIZE*)pValue)->cx, ((SIZE*)pValue)->cy);
// 		LeaveCriticalSection(&m_CriticalSection);
// 		break;
// 
// 	case J_IVW_CB_MOUSE_MOVE:
// 		// Get the mouse cursor location
// 		EnterCriticalSection(&m_CriticalSection);
// 		m_XPos = ((POINT*)pValue)->x;
// 		m_YPos = ((POINT*)pValue)->y;
// 
// 		m_bPositionChanged = true;
// 		m_ViewWindowEventText.Format(_T("Mouse Move event: Pos = (%d,%d)"), ((POINT*)pValue)->x, ((POINT*)pValue)->y);
// 		LeaveCriticalSection(&m_CriticalSection);
// 		break;
	case J_IVW_CB_MOUSE_LBUTTON_DOWN:
		EnterCriticalSection(&m_CriticalSection);
		m_ViewWindowEventText.Format(_T("Mouse Left Button Down event: Pos = (%d,%d)"), ((POINT*)pValue)->x, ((POINT*)pValue)->y);
		m_MouseClickPos.x = (double)((POINT*)pValue)->x;
		m_MouseClickPos.y = (double)((POINT*)pValue)->y;
		m_bClickMoveFlag = TRUE;
// 		pTask->NumPoint = 1;
// 		pTask->PatternNum = 0;
// 		memcpy(pTask->ImageBuff[pTask->PatternNum], m_CnvImageInfo.pImageBuffer, sizeof(unsigned char)*DEF_IMAGE_X*DEF_IMAGE_Y);
// 		pTask->iGetPattAlignCenter = TRUE;
		LeaveCriticalSection(&m_CriticalSection);
		break;
// 
// 	case J_IVW_CB_MOUSE_LBUTTON_UP:
// 		EnterCriticalSection(&m_CriticalSection);
// 		m_ViewWindowEventText.Format(_T("Mouse Left Button Up event: Pos = (%d,%d)"), ((POINT*)pValue)->x, ((POINT*)pValue)->y);
// 		LeaveCriticalSection(&m_CriticalSection);
// 		break;
// 	case J_IVW_CB_MOUSE_RBUTTON_DOWN:
// 		EnterCriticalSection(&m_CriticalSection);
// 		m_ViewWindowEventText.Format(_T("Mouse Right Button Down event: Pos = (%d,%d)"), ((POINT*)pValue)->x, ((POINT*)pValue)->y);
// 		LeaveCriticalSection(&m_CriticalSection);
// 		break;
// 	case J_IVW_CB_MOUSE_RBUTTON_UP:
// 		EnterCriticalSection(&m_CriticalSection);
// 		m_ViewWindowEventText.Format(_T("Mouse Right Button Up event: Pos = (%d,%d)"), ((POINT*)pValue)->x, ((POINT*)pValue)->y);
// 		LeaveCriticalSection(&m_CriticalSection);
// 		break;
// 	case J_IVW_CB_ZOOM_POS:
// 		EnterCriticalSection(&m_CriticalSection);
// 		m_ViewWindowEventText.Format(_T("Zoom Position event: Offset = (%d,%d), ZoomRationX = %f, ZoomRationY = %f"), ((J_IVW_ZOOM_POS*)pValue)->Offset.x, ((J_IVW_ZOOM_POS*)pValue)->Offset.y, ((J_IVW_ZOOM_POS*)pValue)->ZoomRatioX, ((J_IVW_ZOOM_POS*)pValue)->ZoomRatioY);
// 		LeaveCriticalSection(&m_CriticalSection);
// 		break;
// 
// 	case J_IVW_CB_AREA_SELECT:
// 		EnterCriticalSection(&m_CriticalSection);
// 		m_ViewWindowEventText.Format(_T("Area Selection event: Area (Left,Top,Right,Bottom)=(%d,%d,%d,%d)"), ((RECT*)pValue)->left, ((RECT*)pValue)->top, ((RECT*)pValue)->right, ((RECT*)pValue)->bottom);
// 		LeaveCriticalSection(&m_CriticalSection);
// 		break;

	case J_IVW_CB_USER_PAINT_IMAGE:
	{
		// Print out some stuff on the image!

		// First we need to get the Device Context from the callback parameter
		hDC = ((PJ_IVW_USER_PAINT)pValue)->hDC;

		// Create the GDI+ Graphics object used for the drawing
		Graphics g(hDC);

		// Set the text rendering for Cleartype
		g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

		// Create a red Solid Brush Color(A,R,G,B)
		SolidBrush brush(Color(255, 255, 0, 0));

		// Create a font
		FontFamily fontFamily(L"Arial");
		Gdiplus::Font font(&fontFamily, 18, FontStyleRegular, UnitPoint);

		PointF pointF(100.0, 100.0);

// 		g.DrawString(m_UserDrawImageEventText, -1, &font, pointF, &brush);

		if (m_bEnableUserDrawImageGrid)
		{
			// Create a Solid red pen Color(A,R,G,B)
			Pen  redPen(Color(255, 255, 0, 0));

			for (int x = 100; x < ((PJ_IVW_USER_PAINT)pValue)->ImageWidth; x += 100)
			{
				g.DrawLine(&redPen, x, 0, x, (int)((PJ_IVW_USER_PAINT)pValue)->ImageHeight - 1);
			}

			for (int y = 100; y < ((PJ_IVW_USER_PAINT)pValue)->ImageHeight; y += 100)
			{
				g.DrawLine(&redPen, 0, y, (int)((PJ_IVW_USER_PAINT)pValue)->ImageWidth - 1, y);
			}
		}
	}
	break;


	case J_IVW_CB_USER_PAINT_WINDOW:
	{
		// First we need to get the Device Context from the callback parameter
		hDC = ((PJ_IVW_USER_PAINT)pValue)->hDC;

		// Create the GDI+ Graphics object used for the drawing
		Graphics g(hDC);

		// Set the text rendering for Cleartype
		g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

		// Create a blue Solid Brush Color(A,R,G,B)
		SolidBrush brush(Color(255, 0, 0, 255));

		// Create a font
		FontFamily fontFamily(L"Arial");
		Gdiplus::Font font(&fontFamily, 15, FontStyleRegular, UnitPoint);

		PointF pointF(20.0, 20.0);

		g.DrawString(m_Util.CStringTobstr(m_UserDrawViewWindowEventText), -1, &font, pointF, &brush);

		if (m_bEnableUserDrawViewWindowGrid)
		{
			// Create a Solid red pen Color(A,R,G,B)
			Pen  yellowPen(Color(255, 255, 255, 0));

			g.DrawLine(&yellowPen
				, (int)(((PJ_IVW_USER_PAINT)pValue)->ImageWidth / 2)
				, 0
				, (int)(((PJ_IVW_USER_PAINT)pValue)->ImageWidth / 2)
				, (int)((PJ_IVW_USER_PAINT)pValue)->ImageHeight - 1);
			g.DrawLine(&yellowPen
				, 0
				, (int)(((PJ_IVW_USER_PAINT)pValue)->ImageHeight / 2)
				, (int)((PJ_IVW_USER_PAINT)pValue)->ImageWidth - 1
				, (int)(((PJ_IVW_USER_PAINT)pValue)->ImageHeight / 2));

// 			for (int x = 100; x < ((PJ_IVW_USER_PAINT)pValue)->ImageWidth; x += 100)
// 			{
// 				g.DrawLine(&bluePen, x, 0, x, (int)((PJ_IVW_USER_PAINT)pValue)->ImageHeight - 1);
// 			}
// 
// 			for (int y = 100; y < ((PJ_IVW_USER_PAINT)pValue)->ImageHeight; y += 100)
// 			{
// 				g.DrawLine(&bluePen, 0, y, (int)((PJ_IVW_USER_PAINT)pValue)->ImageWidth - 1, y);
// 			}
		}
	}
	break;

	default:
		EnterCriticalSection(&m_CriticalSection);
		m_ViewWindowEventText.Format(_T("Unknown event received!: event = %d"), Type);
		LeaveCriticalSection(&m_CriticalSection);
	}
}


void CCamJAI::OnTimer(UINT_PTR nIDEvent)
{
	CString valueString;

	if (nIDEvent == 1)
	{
		EnterCriticalSection(&m_CriticalSection);
		switch (m_PixelType)
		{
		case J_GVSP_PIX_MONO8:
			valueString.Format(_T("Mono8 PixelValue at (%d,%d): %d"), m_XPos, m_YPos, m_PixelValue.PixelValueUnion.Mono8Type.Value);
			break;

		case J_GVSP_PIX_MONO16:
			// Be aware that the Mono 16 values are "normalized" into 16-bit values by shifting the 10- and 12-bit values from the cameras up to MSB!
			// If you need to get 10-bit or 12-bit values instead of the normalized valeus you will ned to do like this:
			// value = m_PixelValue.PixelValueUnion.Mono16Type.Value & OxFFC0) >> 6 // 10bit mono
			// ... or
			// value = m_PixelValue.PixelValueUnion.Mono16Type.Value & OxFFF0) >> 4 // 12bit mono
			valueString.Format(_T("Mono16 Pixel Value at (%d,%d): %d"), m_XPos, m_YPos, m_PixelValue.PixelValueUnion.Mono16Type.Value);
			break;

		case J_GVSP_PIX_BGR8_PACKED:
			valueString.Format(_T("BGR24 Pixel Value at (%d,%d): (R,G,B)=(%d,%d,%d)"), m_XPos, m_YPos, m_PixelValue.PixelValueUnion.BGR24Type.RValue, m_PixelValue.PixelValueUnion.BGR24Type.GValue, m_PixelValue.PixelValueUnion.BGR24Type.BValue);
			break;

		case J_GVSP_PIX_BGR16_PACKED_INTERNAL:
			// Be aware that the BGR 16 values are "normalized" into 16-bit values by shifting the 10- and 12-bit values from the cameras up to MSB!
			// If you need to get 10-bit or 12-bit values instead of the normalized valeus you will ned to do like this:
			// RValue = m_PixelValue.PixelValueUnion.BGR48Type.RValue & OxFFC0) >> 6 // 10bit Color
			// GValue = m_PixelValue.PixelValueUnion.BGR48Type.GValue & OxFFC0) >> 6 // 10bit Color
			// BValue = m_PixelValue.PixelValueUnion.BGR48Type.BValue & OxFFC0) >> 6 // 10bit Color
			// ... or
			// RValue = m_PixelValue.PixelValueUnion.BGR48Type.RValue & OxFFF0) >> 4 // 12bit Color
			// GValue = m_PixelValue.PixelValueUnion.BGR48Type.GValue & OxFFF0) >> 4 // 12bit Color
			// BValue = m_PixelValue.PixelValueUnion.BGR48Type.BValue & OxFFF0) >> 4 // 12bit Color
			valueString.Format(_T("BGR48 Pixel Value at (%d,%d): (R,G,B)=(%d,%d,%d)"), m_XPos, m_YPos, m_PixelValue.PixelValueUnion.BGR48Type.RValue, m_PixelValue.PixelValueUnion.BGR48Type.GValue, m_PixelValue.PixelValueUnion.BGR48Type.BValue);
			break;

		}
		m_ValueStaticCtrl.SetWindowText(valueString);

// 		GetDlgItem(IDC_EVENT_STATIC)->SetWindowText(m_ViewWindowEventText);

		LeaveCriticalSection(&m_CriticalSection);
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CCamJAI::OnBnClickedBtnConnectCam()
{
	BOOL retval;

	// Close factory & camera
	CloseFactoryAndCamera();

	// Open factory & camera
	retval = OpenFactoryAndCamera();
	if (retval)
	{
		// Display camera ID
		GetDlgItem(IDC_CAMERAID)->SetWindowText(CString((char*)m_sCameraId));

		// Enable Controls
		EnableControls(TRUE, FALSE);
	}
	else
	{
		// Write error message in textbox
		GetDlgItem(IDC_CAMERAID)->SetWindowText(CString("Error opening factory and camera"));

		// Disable controls
		EnableControls(FALSE, FALSE);
	}
	InitializeControls();   // Initialize Controls

	EnterCriticalSection(&m_CriticalSection);
	m_PixelValue.PixelValueUnion.Mono16Type.Value = 0;
	m_PixelType = J_GVSP_PIX_MONO8;
	LeaveCriticalSection(&m_CriticalSection);
}
#endif