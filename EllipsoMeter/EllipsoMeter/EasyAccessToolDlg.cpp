
// EasyAccessToolDlg.cpp : 구현 파일
//
#include "stdafx.h"
#if SPINNAKER_CAMERA
#include "EllipsoMeter.h"
#include "EasyAccessToolDlg.h"
#include "afxdialogex.h"
#include "ConnectionWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEasyAccessToolDlg 대화 상자
UINT DisplayThread_Cam(LPVOID param)
{
	CEasyAccessToolDlg* pMain = (CEasyAccessToolDlg*)param;

	DWORD dwResult = 0;
	while (pMain->m_bThWork)
	{
		Sleep(30);

		dwResult = WaitForSingleObject(pMain->m_Camera.GetHandleGrabDone(), 1000);
		if (dwResult == WAIT_OBJECT_0)
		{
			if ( pMain->m_hWnd != NULL )
			{
				pMain->OnDisplay();
				pMain->m_Camera.ResetHandleGrabDone();
			}
		}
	}
	return 0;
}

CEasyAccessToolDlg::CEasyAccessToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_EASYACCESSTOOL_DIALOG, pParent)
{
	this->Create( IDD_EASYACCESSTOOL_DIALOG );
	m_bShow = FALSE;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEasyAccessToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_EXPOSURE_MODE, m_cBoxExposureMode);
	DDX_Control(pDX, IDC_COMBO_TRIGGER_MODE, m_cBoxTriggerMode);
	DDX_Control(pDX, IDC_COMBO_TRIGGER_SOURCE, m_cBoxTriggerSource);
	DDX_Control(pDX, IDC_COMBO_AUTO_EXPOSURE, m_cBoxExposureAuto);
	DDX_Control(pDX, IDC_COMBO_AUTO_GAIN, m_cBoxGainAuto);
	DDX_Control(pDX, IDC_COMBO_AUTO_TARGET_GREY, m_cBoxTargetGreyAuto);
	DDX_Control(pDX, IDC_COMBO_AUTO_WHITE_BALANCE, m_cBoxWhiteBalanceAuto);
	DDX_Control(pDX, IDC_COMBO_AUTO_EXPOSURE_COMPENSATION, m_cBoxExposureCompensationAuto);
	DDX_Control(pDX, IDC_LTB_LOG, m_lBoxLog);
}

BEGIN_MESSAGE_MAP(CEasyAccessToolDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_CONNECTION_WINDOW, &CEasyAccessToolDlg::OnBnClickedBtnConnectionWindow)
	ON_BN_CLICKED(IDC_BTN_START, &CEasyAccessToolDlg::OnBnClickedBtnStart)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_WRITE, &CEasyAccessToolDlg::OnBnClickedBtnWrite)
	ON_BN_CLICKED(IDC_BTN_READ, &CEasyAccessToolDlg::OnBnClickedBtnRead)
	ON_BN_CLICKED(IDC_CBTN_CROSS_HAIR, &CEasyAccessToolDlg::OnBnClickedCbtnCrossHair)
	ON_CBN_SELCHANGE(IDC_COMBO_AUTO_EXPOSURE, &CEasyAccessToolDlg::OnCbnSelchangeComboAutoExposure)
	ON_CBN_SELCHANGE(IDC_COMBO_AUTO_GAIN, &CEasyAccessToolDlg::OnCbnSelchangeComboAutoGain)
	ON_CBN_SELCHANGE(IDC_COMBO_AUTO_TARGET_GREY, &CEasyAccessToolDlg::OnCbnSelchangeComboAutoTargetGrey)
	ON_CBN_SELCHANGE(IDC_COMBO_AUTO_WHITE_BALANCE, &CEasyAccessToolDlg::OnCbnSelchangeComboAutoWhiteBalance)
	ON_CBN_SELCHANGE(IDC_COMBO_EXPOSURE_MODE, &CEasyAccessToolDlg::OnCbnSelchangeComboExposureMode)
	ON_CBN_SELCHANGE(IDC_COMBO_TRIGGER_MODE, &CEasyAccessToolDlg::OnCbnSelchangeComboTriggerMode)
	ON_CBN_SELCHANGE(IDC_COMBO_TRIGGER_SOURCE, &CEasyAccessToolDlg::OnCbnSelchangeComboTriggerSource)
	ON_BN_CLICKED(IDC_BTN_INI_LOAD, &CEasyAccessToolDlg::OnBnClickedBtnIniLoad)
	ON_BN_CLICKED(IDC_BTN_INI_SAVE, &CEasyAccessToolDlg::OnBnClickedBtnIniSave)
	ON_BN_CLICKED(IDC_CBTN_SHARPNESS, &CEasyAccessToolDlg::OnBnClickedCbtnSharpness)
	ON_CBN_SELCHANGE(IDC_COMBO_AUTO_EXPOSURE_COMPENSATION, &CEasyAccessToolDlg::OnCbnSelchangeComboAutoExposureCompensation)
	ON_BN_CLICKED(IDC_BTN_HIDE, &CEasyAccessToolDlg::OnBnClickedBtnHide)
END_MESSAGE_MAP()

BOOL CEasyAccessToolDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (( nID   == 2 ) &&
		( nCode == -1 ))
	{
		m_fnSetShowFlag( FALSE );
	}
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CEasyAccessToolDlg::m_fnSetShowFlag( BOOL bShow )
{
	m_bShow = bShow;
	if ( m_bShow )
	{
		this->ShowWindow( SW_SHOW );
	}
	else
	{
		this->ShowWindow( SW_HIDE );
	}
}

//---------------------------------------------------------------------------------------------------------
// 시작에 필요한 작업을 한다.
//---------------------------------------------------------------------------------------------------------
void CEasyAccessToolDlg::m_fnSetInit( ) 
{
	m_bGrabed       = FALSE;
	CString s("");
	s.Format( "카메라 설정 메인창" );
	this->SetWindowText(s);
}

//---------------------------------------------------------------------------------------------------------
// 종료에 필요한 작업을 한다.
//---------------------------------------------------------------------------------------------------------
void CEasyAccessToolDlg::m_fnSetEnd( ) 
{
	if (m_pDisplayThread_Cam != NULL)
	{
		m_bThWork = false;
		DWORD dwResult = 0;
		dwResult = WaitForSingleObject(m_pDisplayThread_Cam->m_hThread, 5000);
	}


	if (m_Camera.GetActiveStatus())
	{
		m_Camera.OnStopAcquisition();
	}

	if (m_Camera.GetConnectionStatus())
	{
		m_Camera.OnDisConnect();
	}

	if (m_pBitmapInfo != NULL)
	{
		delete[] m_pBitmapInfo;
		m_pBitmapInfo = NULL;
	}
}

BOOL CEasyAccessToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	m_nWidth = 0;
	m_nHeight = 0;
	m_nBpp = 0;

	m_bCrossHair = false;

	m_pBitmapInfo = NULL;
	m_pDisplayThread_Cam = NULL;

	m_cBoxExposureMode.AddString(_T("Timed"));
	m_cBoxExposureMode.AddString(_T("Trigger Width"));

	m_cBoxTriggerMode.AddString(_T("Off"));
	m_cBoxTriggerMode.AddString(_T("On"));

	m_cBoxTriggerSource.AddString(_T("Software"));
	m_cBoxTriggerSource.AddString(_T("Line0"));

	m_cBoxExposureAuto.AddString(_T("Off"));
	m_cBoxExposureAuto.AddString(_T("Once"));
	m_cBoxExposureAuto.AddString(_T("Continuous"));


	m_cBoxGainAuto.AddString(_T("Off"));
	m_cBoxGainAuto.AddString(_T("Once"));
	m_cBoxGainAuto.AddString(_T("Continuous"));


	m_cBoxTargetGreyAuto.AddString(_T("Off"));
	m_cBoxTargetGreyAuto.AddString(_T("Continuous"));

	m_cBoxWhiteBalanceAuto.AddString(_T("Off"));
	m_cBoxWhiteBalanceAuto.AddString(_T("Once"));
	m_cBoxWhiteBalanceAuto.AddString(_T("Continuous"));

	m_cBoxExposureCompensationAuto.AddString(_T("Off"));
	m_cBoxExposureCompensationAuto.AddString(_T("Once"));
	m_cBoxExposureCompensationAuto.AddString(_T("Continuous"));

	GetDlgItem(IDC_CBTN_CROSS_HAIR)->EnableWindow(FALSE);

	m_fnCameraInit();
	
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CEasyAccessToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CEasyAccessToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CEasyAccessToolDlg::m_fnCameraInit( )
{
	m_bThWork       = false;
	m_bGrab         = false; 
	m_pBuffer       = NULL;
	m_pDispBuffer   = NULL;
	m_Camera.OnUpdateList();
	m_Camera.OnSearchCameraOnSystem();

	int nDevice = 0;
	m_Camera.GetCameraCount(&nDevice);
	if ( nDevice > 0)
	{
		bool ret = false;
		m_Camera.OnConnectByIndex(0);
		m_nWidth = m_Camera.GetWidth();
		m_nHeight = m_Camera.GetHeight();
		m_nBpp = m_Camera.GetBpp();

		OnCreateBitmapInfo(m_nWidth, m_nHeight, m_nBpp);

		SetDlgItemText(IDC_BTN_CONNECTION_WINDOW, _T("DisConnect"));
		WriteLog(_T("Successed to connect camera"));

		GetParameters();
	}
}

void CEasyAccessToolDlg::OnDisplay()
{

	CClientDC dc(GetDlgItem(IDC_PC_VIEW));

	if ( this->m_hWnd == NULL ) return;
	if ( GetDlgItem(IDC_PC_VIEW) == NULL ) return;

	CRect rect;
	GetDlgItem(IDC_PC_VIEW)->GetClientRect(&rect);


	BYTE* pBuffer   = m_Camera.GetBuffer();
	m_pDispBuffer   = m_Camera.GetBuffer();
	if ( !m_bGrabed )
	{
		m_bGrabed       = TRUE;
		m_pBuffer       = m_Camera.GetBuffer();
	}
	m_bGrab         = true; 
	SetStretchBltMode(dc.GetSafeHdc(), COLORONCOLOR);
	StretchDIBits(dc.GetSafeHdc(), 0, 0, rect.Width(), rect.Height(), 0, 0, m_nWidth, m_nHeight, pBuffer, m_pBitmapInfo, DIB_RGB_COLORS, SRCCOPY);
	
	if (m_bCrossHair)
	{
		CPen pen, *oldPen;
		pen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
		oldPen = dc.SelectObject(&pen);
		dc.MoveTo(rect.Width() / 2, 0);
		dc.LineTo(rect.Width() / 2, rect.Height());
		dc.MoveTo(0, rect.Height() / 2);
		dc.LineTo(rect.Width(), rect.Height() / 2);
	}
}

void CEasyAccessToolDlg::OnCreateBitmapInfo(IN int nWidth, IN int nHeight, IN int nBpp)
{
	if (m_pBitmapInfo != NULL)
	{
		delete[]m_pBitmapInfo;
		m_pBitmapInfo = NULL;
	}

	if (m_nBpp == 8)
		m_pBitmapInfo = (BITMAPINFO *) new BYTE[sizeof(BITMAPINFO) + 255 * sizeof(RGBQUAD)];
	else if (m_nBpp == 24)
		m_pBitmapInfo = (BITMAPINFO*) new BYTE[sizeof(BITMAPINFO)];
	m_pBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pBitmapInfo->bmiHeader.biPlanes = 1;
	m_pBitmapInfo->bmiHeader.biBitCount = m_nBpp;
	m_pBitmapInfo->bmiHeader.biCompression = BI_RGB;
	m_pBitmapInfo->bmiHeader.biSizeImage = 0;
	m_pBitmapInfo->bmiHeader.biXPelsPerMeter = 0;
	m_pBitmapInfo->bmiHeader.biYPelsPerMeter = 0;
	m_pBitmapInfo->bmiHeader.biClrUsed = 0;
	m_pBitmapInfo->bmiHeader.biClrImportant = 0;

	if (m_nBpp == 8)
	{
		for (int i = 0; i < 256; i++)
		{
			m_pBitmapInfo->bmiColors[i].rgbBlue = (BYTE)i;
			m_pBitmapInfo->bmiColors[i].rgbGreen = (BYTE)i;
			m_pBitmapInfo->bmiColors[i].rgbRed = (BYTE)i;
			m_pBitmapInfo->bmiColors[i].rgbReserved = 0;
		}
	}

	m_pBitmapInfo->bmiHeader.biWidth = nWidth;
	m_pBitmapInfo->bmiHeader.biHeight = -nHeight;
}
void CEasyAccessToolDlg::OnBnClickedBtnConnectionWindow()
{
	CString caption;
	GetDlgItemText(IDC_BTN_CONNECTION_WINDOW, caption);

	if (caption == _T("Connection Window"))
	{
		WriteLog(_T("Clicked Connection Window"));
		ConnectionWindow connectionWindow;
		connectionWindow.DoModal();
		
		connectionWindow.DestroyWindow();

		if (!m_Camera.GetConnectionStatus())
		{
			WriteLog(_T("Failed to connect camera"));
			MessageBox(_T("Camera is not connected"), _T("Camera Connection Error"));
			return;
		}
		else
		{
			m_nWidth = m_Camera.GetWidth();
			m_nHeight = m_Camera.GetHeight();
			m_nBpp = m_Camera.GetBpp();

			OnCreateBitmapInfo(m_nWidth, m_nHeight, m_nBpp);

			SetDlgItemText(IDC_BTN_CONNECTION_WINDOW, _T("DisConnect"));
			WriteLog(_T("Successed to connect camera"));

			GetParameters();
		}
	}
	else if (caption == _T("DisConnect"))
	{
		bool ret = m_Camera.OnDisConnect();
		if (ret)
			WriteLog(_T("Successed to disconnect camera"));
		SetDlgItemText(IDC_BTN_CONNECTION_WINDOW, _T("Connection Window"));
	}
}

BOOL CEasyAccessToolDlg::m_fnGetIsConnect()
{
	return m_bThWork;
}

void CEasyAccessToolDlg::m_fnSetConnect( BOOL bConnect )
{
	if ( bConnect )
	{
		if ( !m_bThWork )
		{
			OnBnClickedBtnStart();
		}
	}
	else
	{
		if ( m_bThWork )
		{
			OnBnClickedBtnStart();
		}
	}
}
void CEasyAccessToolDlg::OnBnClickedBtnStart()
{
	CString caption;
	GetDlgItemText(IDC_BTN_START, caption);
	bool ret = false;
	m_bGrab         = false; 
	m_pBuffer       = NULL;
	m_pDispBuffer   = NULL;
	if (caption == _T("Start"))
	{
		ret = m_Camera.OnStartAcquisition();
		if (ret)
			WriteLog(_T("Succesed to start stream"));
		else
			WriteLog(_T("Failed to start stream"));
		m_bThWork = true;
		m_pDisplayThread_Cam = AfxBeginThread(DisplayThread_Cam, this);
		SetDlgItemText(IDC_BTN_START, _T("Stop"));
		GetDlgItem(IDC_EDIT_WIDTH)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_HEIGHT)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_OFFSET_X)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_OFFSET_Y)->EnableWindow(FALSE);
		GetDlgItem(IDC_CBTN_CROSS_HAIR)->EnableWindow(TRUE);
	}
	else if (caption == _T("Stop"))
	{
		if (m_pDisplayThread_Cam != NULL)
		{
			m_bThWork = false;
			DWORD dwResult = 0;
			dwResult = WaitForSingleObject(m_pDisplayThread_Cam->m_hThread, 5000);
		}
		ret = m_Camera.OnStopAcquisition();
		if (ret)
			WriteLog(_T("Successed to stop stream"));
		else
			WriteLog(_T("Failed to stop stream"));
		m_bThWork = false;
		SetDlgItemText(IDC_BTN_START, _T("Start"));
		GetDlgItem(IDC_EDIT_WIDTH)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_HEIGHT)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_OFFSET_X)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_OFFSET_Y)->EnableWindow(TRUE);
		GetDlgItem(IDC_CBTN_CROSS_HAIR)->EnableWindow(FALSE);
	}
}


void CEasyAccessToolDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

}

void CEasyAccessToolDlg::GetParameters()
{
	CString strTmp;
	double dTmp;
	int nTmp;
	bool ret = false;

	ret = m_Camera.GetExposureAuto(&strTmp);
	if (ret)
	{
		WriteLog(_T("Successed to read Exposure Auto"));
		if (strTmp == _T("Off"))
		{
			m_cBoxExposureAuto.SetCurSel(0);
			GetDlgItem(IDC_EDIT_EXPOSURE_TIME)->EnableWindow(TRUE);
			m_cBoxExposureMode.EnableWindow(TRUE);
		}
		else if (strTmp == _T("Once"))
			m_cBoxExposureAuto.SetCurSel(1);
		else if (strTmp == _T("Continuous"))
		{
			m_cBoxExposureAuto.SetCurSel(2);
			GetDlgItem(IDC_EDIT_EXPOSURE_TIME)->EnableWindow(FALSE);
			m_cBoxExposureMode.EnableWindow(FALSE);
		}
	}
	else
		WriteLog(_T("Failed to read Exposure Auto"));

	ret = m_Camera.GetGainAuto(&strTmp);
	if (ret)
	{
		WriteLog(_T("Successed to read Gain Auto"));
		if (strTmp == _T("Off"))
		{
			m_cBoxGainAuto.SetCurSel(0);
			GetDlgItem(IDC_EDIT_GAIN)->EnableWindow(TRUE);
		}
		else if (strTmp == _T("Once"))
			m_cBoxGainAuto.SetCurSel(1);
		else if (strTmp == _T("Continuous"))
		{
			m_cBoxGainAuto.SetCurSel(2);
			GetDlgItem(IDC_EDIT_GAIN)->EnableWindow(FALSE);
		}
	}
	else
		WriteLog(_T("Failed to read Gain Auto"));

	ret = m_Camera.GetTargetGreyAuto(&strTmp);
	if (ret)
	{
		WriteLog(_T("Successed to read Target Grey Auto"));
		if (strTmp == _T("Off"))
			m_cBoxTargetGreyAuto.SetCurSel(0);
		else if (strTmp == _T("Continuous"))
			m_cBoxTargetGreyAuto.SetCurSel(1);
		m_cBoxTargetGreyAuto.EnableWindow(TRUE);
	}
	else
	{
		WriteLog(_T("Failed to read Target Grey Auto"));
		m_cBoxTargetGreyAuto.EnableWindow(FALSE);
	}

	ret = m_Camera.GetWhiteBalanceAuto(&strTmp);
	if (ret)
	{
		WriteLog(_T("Successed to read White Balance Auto"));
		if (strTmp == _T("Off"))
			m_cBoxWhiteBalanceAuto.SetCurSel(0);
		else if (strTmp == _T("Once"))
			m_cBoxWhiteBalanceAuto.SetCurSel(1);
		else if (strTmp == _T("Continuous"))
			m_cBoxWhiteBalanceAuto.SetCurSel(2);
	}
	else
	{
		WriteLog(_T("Failed to read WhiteBalance Auto"));
		m_cBoxWhiteBalanceAuto.EnableWindow(FALSE);
	}

	ret = m_Camera.GetExposureCompensationAuto(&strTmp);
	if (ret)
	{
		WriteLog(_T("Successed to read ExposureCompensation Auto"));
		if (strTmp == _T("Off"))
			m_cBoxExposureCompensationAuto.SetCurSel(0);
		else if (strTmp == _T("Once"))
			m_cBoxExposureCompensationAuto.SetCurSel(1);
		else if (strTmp == _T("Continuous"))
			m_cBoxExposureCompensationAuto.SetCurSel(2);
	}
	bool bTmp;
	ret = m_Camera.GetSharpnessEnable(&bTmp);
	if (ret)
	{
		WriteLog(_T("Successed to read Sharpness Enable"));
		if (bTmp)
			CheckDlgButton(IDC_CBTN_SHARPNESS, true);
		else
			CheckDlgButton(IDC_CBTN_SHARPNESS, false);
	}
	else
	{
		WriteLog(_T("Failed to read Sharpness Enable"));
		GetDlgItem(IDC_CBTN_SHARPNESS)->EnableWindow(false);
	}

	ret = m_Camera.GetExposureMode(&strTmp);
	if (ret)
	{
		WriteLog(_T("Successed to read Exposure Mode"));
		if (strTmp == _T("Timed"))
			m_cBoxExposureMode.SetCurSel(0);
		else if (strTmp = _T("TriggerWidth"))
			m_cBoxExposureMode.SetCurSel(1);
	}
	else
		WriteLog(_T("Failed to read Exposure Mode"));

	ret = m_Camera.GetExposureTime(&dTmp);
	if (ret)
	{
		WriteLog(_T("Successed to read Exposure Time"));
		SetDlgItemInt(IDC_EDIT_EXPOSURE_TIME, (int)dTmp);
	}
	else
		WriteLog(_T("Failed to read Exposure Time"));

	ret = m_Camera.GetGain(&dTmp);
	if (ret)
	{
		WriteLog(_T("Successed to read Gain"));
		CString tmp;
		tmp.Format(_T("%.1f"), dTmp);
		SetDlgItemText(IDC_EDIT_GAIN, tmp);
	}
	else
		WriteLog(_T("Failed to read Gain"));

	ret = m_Camera.GetTriggerMode(&strTmp);
	if (ret)
	{
		WriteLog(_T("Successed to read Trigger Mode"));
		if (strTmp == _T("Off"))
		{
			m_cBoxTriggerMode.SetCurSel(0);
			m_cBoxTriggerSource.EnableWindow(FALSE);
		}
		else if (strTmp == _T("On"))
		{
			m_cBoxTriggerMode.SetCurSel(1);
			m_cBoxTriggerSource.EnableWindow(TRUE);
		}

	}
	else
		WriteLog(_T("Failed to read Trigger Mode"));

	ret = m_Camera.GetTriggerSource(&strTmp);
	if (ret)
	{
		WriteLog(_T("Successed to read TriggerSource"));
		if (strTmp == _T("Line0"))
			m_cBoxTriggerSource.SetCurSel(0);
		else if (strTmp == _T("Software"))
			m_cBoxTriggerSource.SetCurSel(1);
	}
	else
		WriteLog(_T("Failed to read Trigger Source"));

	ret = m_Camera.GetWidth(&nTmp);
	if (ret)
	{
		WriteLog(_T("Successed to read Width"));
		SetDlgItemInt(IDC_EDIT_WIDTH, nTmp);
	}
	else
		WriteLog(_T("Failed to read Width"));

	ret = m_Camera.GetHeight(&nTmp);
	if (ret)
	{
		WriteLog(_T("Successed to read Height"));
		SetDlgItemInt(IDC_EDIT_HEIGHT, nTmp);
	}
	else
		WriteLog(_T("Failed to read Height"));

	ret = m_Camera.GetOffsetX(&nTmp);
	if (ret)
	{
		WriteLog(_T("Successed to read Offset X"));
		SetDlgItemInt(IDC_EDIT_OFFSET_X, nTmp);
	}
	else
		WriteLog(_T("Failed to read Offset X"));

	ret = m_Camera.GetOffsetY(&nTmp);
	if (ret)
	{
		WriteLog(_T("Successed to read Offset Y"));
		SetDlgItemInt(IDC_EDIT_OFFSET_Y, nTmp);
	}
	else
		WriteLog(_T("Failed to read Offset Y"));
}

void CEasyAccessToolDlg::SetParameters()
{
	CString strTmp;
	bool ret;
	int nTmp;
	
	bool WindowEnable = m_cBoxExposureAuto.IsWindowEnabled();
	if (WindowEnable)
	{
		nTmp = m_cBoxExposureAuto.GetCurSel();
		m_cBoxExposureAuto.GetLBText(nTmp, strTmp);
		ret = m_Camera.SetExposureAuto(strTmp);
		if (ret)
			WriteLog(_T("Successed to write Exposure Auto"));
		else
			WriteLog(_T("Failed to write Exposure Auto"));
	}

	WindowEnable = m_cBoxGainAuto.IsWindowEnabled();
	if (WindowEnable)
	{
		nTmp = m_cBoxGainAuto.GetCurSel();
		m_cBoxGainAuto.GetLBText(nTmp, strTmp);
		ret = m_Camera.SetGainAuto(strTmp);
		if (ret)
			WriteLog(_T("Successed to write Gain Auto"));
		else
			WriteLog(_T("Failed to write Gain Auto"));
	}

	WindowEnable = m_cBoxTargetGreyAuto.IsWindowEnabled();
	if (WindowEnable)
	{
		nTmp = m_cBoxTargetGreyAuto.GetCurSel();
		m_cBoxExposureAuto.GetLBText(nTmp, strTmp);
		ret = m_Camera.SetTargetGreyAuto(strTmp);
		if (ret)
			WriteLog(_T("Successed to write Target Grey Value Auto"));
		else
			WriteLog(_T("Failed to write Target Grey Value Auto"));
	}

	WindowEnable = m_cBoxWhiteBalanceAuto.IsWindowEnabled();
	if (WindowEnable)
	{
		nTmp = m_cBoxWhiteBalanceAuto.GetCurSel();
		m_cBoxWhiteBalanceAuto.GetLBText(nTmp, strTmp);
		ret = m_Camera.SetWhiteBalanceAuto(strTmp);
		if (ret)
			WriteLog(_T("Successed to write White Balance Auto"));
		else
			WriteLog(_T("Failed to write White Balance Auto"));
	}

	WindowEnable = GetDlgItem(IDC_CBTN_SHARPNESS)->IsWindowEnabled();
	if (WindowEnable)
	{
		bool bTmp = IsDlgButtonChecked(IDC_CBTN_SHARPNESS);
		ret = m_Camera.SetSharpnessEnable(bTmp);
		if (ret)
			WriteLog(_T("Successed to write Sharpness Enable"));
		else
			WriteLog(_T("Failed to write Sharpness Enable"));
	}

	WindowEnable = m_cBoxExposureCompensationAuto.IsWindowEnabled();
	if (WindowEnable)
	{
		nTmp = m_cBoxExposureCompensationAuto.GetCurSel();
		m_cBoxExposureAuto.GetLBText(nTmp, strTmp);
		ret = m_Camera.SetExposureCompensationAuto(strTmp);
		if (ret)
			WriteLog(_T("Successed to write Exposure Compensation Auto"));
		else
			WriteLog(_T("Failed to write Exposure Compensation Auto"));
	}

	WindowEnable = m_cBoxExposureMode.IsWindowEnabled();
	if (WindowEnable)
	{
		nTmp = m_cBoxExposureMode.GetCurSel();
		m_cBoxExposureMode.GetLBText(nTmp, strTmp);
		strTmp.Remove(_T(' '));
		ret = m_Camera.SetExposureMode(strTmp);
		if (ret)
			WriteLog(_T("Successed to write Exposure Mode"));
		else
			WriteLog(_T("Failed to write Exposure Mode"));
	}

	WindowEnable = GetDlgItem(IDC_EDIT_EXPOSURE_TIME)->IsWindowEnabled();
	if (WindowEnable)
	{
		nTmp = GetDlgItemInt(IDC_EDIT_EXPOSURE_TIME);
		ret = m_Camera.SetExposureTime((double)nTmp);
		if (ret)
			WriteLog(_T("Successed to write Exposure Time"));
		else
			WriteLog(_T("Failed to write Exposure Time"));
	}

	WindowEnable = GetDlgItem(IDC_EDIT_GAIN)->IsWindowEnabled();
	if (WindowEnable)
	{
		GetDlgItemText(IDC_EDIT_GAIN, strTmp);
		//nTmp = GetDlgItemInt(IDC_EDIT_GAIN);
		double dTmp = atof(strTmp);
		ret = m_Camera.SetGain(dTmp);
		if (ret)
			WriteLog(_T("Successed to write Gain"));
		else
			WriteLog(_T("Failed to write Gain"));
	}

	WindowEnable = m_cBoxTriggerMode.IsWindowEnabled();
	if (WindowEnable)
	{
		nTmp = m_cBoxTriggerMode.GetCurSel();
		m_cBoxTriggerMode.GetLBText(nTmp, strTmp);
		strTmp.Remove(_T(' '));
		ret = m_Camera.SetTriggerMode(strTmp);
		if (ret)
			WriteLog(_T("Successed to write Trigger Mode"));
		else
			WriteLog(_T("Failed to write Trigger Mode"));
	}

	WindowEnable = m_cBoxTriggerSource.IsWindowEnabled();
	if (WindowEnable)
	{
		nTmp = m_cBoxTriggerSource.GetCurSel();
		m_cBoxTriggerMode.GetLBText(nTmp, strTmp);
		strTmp.Remove(_T(' '));
		ret = m_Camera.SetTriggerSource(strTmp);
		if (ret)
			WriteLog(_T("Successed to write Trigger Source"));
		else
			WriteLog(_T("Failed to write Trigger Source"));
	}

	WindowEnable = GetDlgItem(IDC_EDIT_WIDTH)->IsWindowEnabled();
	if (WindowEnable)
	{
		nTmp = GetDlgItemInt(IDC_EDIT_WIDTH);
		ret = m_Camera.SetWidth(nTmp);
		if (ret)
			WriteLog(_T("Successed to write Width"));
		else
			WriteLog(_T("Failed to write Width"));
	}

	WindowEnable = GetDlgItem(IDC_EDIT_HEIGHT)->IsWindowEnabled();
	if (WindowEnable)
	{
		nTmp = GetDlgItemInt(IDC_EDIT_HEIGHT);
		ret = m_Camera.SetHeight(nTmp);
		if (ret)
			WriteLog(_T("Successed to write Height"));
		else
			WriteLog(_T("Failed to write Height"));
	}

	WindowEnable = GetDlgItem(IDC_EDIT_OFFSET_X)->IsWindowEnabled();
	if (WindowEnable)
	{
		nTmp = GetDlgItemInt(IDC_EDIT_OFFSET_X);
		ret = m_Camera.SetOffsetX(nTmp);
		if (ret)
			WriteLog(_T("Successed to write Offset X"));
		else
			WriteLog(_T("Failed to write Offset X"));
	}

	WindowEnable = GetDlgItem(IDC_EDIT_OFFSET_Y)->IsWindowEnabled();
	if (WindowEnable)
	{
		nTmp = GetDlgItemInt(IDC_EDIT_OFFSET_Y);
		ret = m_Camera.SetOffsetY(nTmp);
		if (ret)
			WriteLog(_T("Successed to write Offset Y"));
		else
			WriteLog(_T("Failed to write Offset Y"));
	}	
}

void CEasyAccessToolDlg::WriteLog(IN CString message)
{
	m_lBoxLog.AddString(message);
	m_lBoxLog.SetCurSel(m_lBoxLog.GetCount() - 1);
}

void CEasyAccessToolDlg::OnBnClickedBtnWrite()
{
	SetParameters();
	m_Camera.SaveUserSet();
}


void CEasyAccessToolDlg::OnBnClickedBtnRead()
{
	GetParameters();
}


void CEasyAccessToolDlg::OnBnClickedCbtnCrossHair()
{
	m_bCrossHair = IsDlgButtonChecked(IDC_CBTN_CROSS_HAIR);
}


BOOL CEasyAccessToolDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			bool ret;
			int nTmp;
			if (pMsg->hwnd == GetDlgItem(IDC_EDIT_EXPOSURE_TIME)->m_hWnd)
			{
				nTmp = GetDlgItemInt(IDC_EDIT_EXPOSURE_TIME);
				ret = m_Camera.SetExposureTime((double)nTmp);
				if (ret)
					WriteLog(_T("Successed to write Exposure Time"));
				else
					WriteLog(_T("Failed to write Exposure Time"));
				return ret;
			}
			else if (pMsg->hwnd == GetDlgItem(IDC_EDIT_GAIN)->m_hWnd)
			{
				CString strTmp;
				GetDlgItemText(IDC_EDIT_GAIN, strTmp);
				//nTmp = GetDlgItemInt(IDC_EDIT_GAIN);
				double dTmp = atof(strTmp);
				ret = m_Camera.SetGain(dTmp);
				if (ret)
					WriteLog(_T("Successed to write Gain"));
				else
					WriteLog(_T("Failed to write Gain"));
				return ret;
			}
			else if (pMsg->hwnd == GetDlgItem(IDC_EDIT_WIDTH)->m_hWnd)
			{
				nTmp = GetDlgItemInt(IDC_EDIT_WIDTH);
				ret = m_Camera.SetWidth(nTmp);
				if (ret)
					WriteLog(_T("Successed to write Width"));
				else
					WriteLog(_T("Failed to write Width"));

				ret = m_Camera.ExchangeResolution();
				if (ret)
				{
					WriteLog(_T("Succssed to change resolution"));
					if (m_pBitmapInfo != NULL)
					{
						delete m_pBitmapInfo;
						m_pBitmapInfo = NULL;
					}
					m_nWidth = m_Camera.GetWidth();
					m_nHeight = m_Camera.GetHeight();
					OnCreateBitmapInfo(m_nWidth, m_nHeight, m_nBpp);
				}
				else
					WriteLog(_T("Failed to change resolution"));
				return ret;
			}
			else if (pMsg->hwnd == GetDlgItem(IDC_EDIT_HEIGHT)->m_hWnd)
			{
				nTmp = GetDlgItemInt(IDC_EDIT_HEIGHT);
				ret = m_Camera.SetHeight(nTmp);
				if (ret)
					WriteLog(_T("Successed to write Height"));
				else
					WriteLog(_T("Failed to write Height"));

				ret = m_Camera.ExchangeResolution();
				if (ret)
				{
					WriteLog(_T("Succssed to change resolution"));
					if (m_pBitmapInfo != NULL)
					{
						delete m_pBitmapInfo;
						m_pBitmapInfo = NULL;
					}
					m_nWidth = m_Camera.GetWidth();
					m_nHeight = m_Camera.GetHeight();
					OnCreateBitmapInfo(m_nWidth, m_nHeight, m_nBpp);
				}
				else
					WriteLog(_T("Failed to change resolution"));
				return ret;
			}
			else if (pMsg->hwnd == GetDlgItem(IDC_EDIT_OFFSET_X)->m_hWnd)
			{
				nTmp = GetDlgItemInt(IDC_EDIT_OFFSET_X);
				ret = m_Camera.SetOffsetX(nTmp);
				if (ret)
					WriteLog(_T("Successed to write Offset X"));
				else
					WriteLog(_T("Failed to write Offset X"));
				return ret;
			}
			else if (pMsg->hwnd ==GetDlgItem(IDC_EDIT_OFFSET_Y)->m_hWnd)
			{
				nTmp = GetDlgItemInt(IDC_EDIT_OFFSET_Y);
				ret = m_Camera.SetOffsetY(nTmp);
				if (ret)
					WriteLog(_T("Successed to write Offset Y"));
				else
					WriteLog(_T("Failed to write Offset Y"));
				return ret;
			}
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CEasyAccessToolDlg::OnCbnSelchangeComboAutoExposure()
{
	int nTmp;
	CString strTmp;
	bool ret;
	nTmp = m_cBoxExposureAuto.GetCurSel();
	m_cBoxExposureAuto.GetLBText(nTmp, strTmp);
	ret = m_Camera.SetExposureAuto(strTmp);
	if (ret)
		WriteLog(_T("Successed to write Exposure Auto"));
	else
		WriteLog(_T("Failed to write Exposure Auto"));

	if (strTmp == _T("Off"))
	{
		m_cBoxExposureAuto.SetCurSel(0);
		GetDlgItem(IDC_EDIT_EXPOSURE_TIME)->EnableWindow(TRUE);
		m_cBoxExposureMode.EnableWindow(TRUE);
	}
	else if (strTmp == _T("Once"))
		m_cBoxExposureAuto.SetCurSel(1);
	else if (strTmp == _T("Continuous"))
	{
		m_cBoxExposureAuto.SetCurSel(2);
		GetDlgItem(IDC_EDIT_EXPOSURE_TIME)->EnableWindow(FALSE);
		m_cBoxExposureMode.EnableWindow(FALSE);
	}
}


void CEasyAccessToolDlg::OnCbnSelchangeComboAutoGain()
{
	int nTmp;
	CString strTmp;
	bool ret;
	nTmp = m_cBoxGainAuto.GetCurSel();
	m_cBoxGainAuto.GetLBText(nTmp, strTmp);
	ret = m_Camera.SetGainAuto(strTmp);
	if (ret)
		WriteLog(_T("Successed to write Gain Auto"));
	else
		WriteLog(_T("Failed to write Gain Auto"));

	if (strTmp == _T("Off"))
	{
		m_cBoxGainAuto.SetCurSel(0);
		GetDlgItem(IDC_EDIT_GAIN)->EnableWindow(TRUE);
	}
	else if (strTmp == _T("Once"))
		m_cBoxGainAuto.SetCurSel(1);
	else if (strTmp == _T("Continuous"))
	{
		m_cBoxGainAuto.SetCurSel(2);
		GetDlgItem(IDC_EDIT_GAIN)->EnableWindow(FALSE);
	}
}


void CEasyAccessToolDlg::OnCbnSelchangeComboAutoTargetGrey()
{
	int nTmp;
	CString strTmp;
	bool ret;
	nTmp = m_cBoxTargetGreyAuto.GetCurSel();
	m_cBoxTargetGreyAuto.GetLBText(nTmp, strTmp);
	ret = m_Camera.SetTargetGreyAuto(strTmp);
	if (ret)
		WriteLog(_T("Successed to write Target Grey Value Auto"));
	else
		WriteLog(_T("Failed to write Target Grey Value Auto"));
}


void CEasyAccessToolDlg::OnCbnSelchangeComboAutoWhiteBalance()
{
	int nTmp;
	CString strTmp;
	bool ret;
	nTmp = m_cBoxWhiteBalanceAuto.GetCurSel();
	m_cBoxWhiteBalanceAuto.GetLBText(nTmp, strTmp);
	ret = m_Camera.SetWhiteBalanceAuto(strTmp);
	if (ret)
		WriteLog(_T("Successed to write White Balance Auto"));
	else
		WriteLog(_T("Failed to write White Balance Auto"));
}


void CEasyAccessToolDlg::OnCbnSelchangeComboExposureMode()
{
	int nTmp;
	CString strTmp;
	bool ret;
	nTmp = m_cBoxExposureMode.GetCurSel();
	m_cBoxExposureMode.GetLBText(nTmp, strTmp);
	strTmp.Remove(_T(' '));
	ret = m_Camera.SetExposureMode(strTmp);
	if (ret)
		WriteLog(_T("Successed to write Exposure Mode"));
	else
		WriteLog(_T("Failed to write Exposure Mode"));
}


void CEasyAccessToolDlg::OnCbnSelchangeComboTriggerMode()
{
	int nTmp;
	CString strTmp;
	bool ret;
	nTmp = m_cBoxTriggerMode.GetCurSel();
	m_cBoxTriggerMode.GetLBText(nTmp, strTmp);
	strTmp.Remove(_T(' '));
	ret = m_Camera.SetTriggerMode(strTmp);
	if (ret)
		WriteLog(_T("Successed to write Trigger Mode"));
	else
		WriteLog(_T("Failed to write Trigger Mode"));

	if (strTmp == _T("Off"))
	{
		m_cBoxTriggerSource.EnableWindow(FALSE);
	}
	else
	{
		m_cBoxTriggerSource.EnableWindow(TRUE);
		ret = m_Camera.GetTriggerSource(&strTmp);
		if (ret)
		{
			WriteLog(_T("Successed to read TriggerSource"));
			if (strTmp == _T("Line0"))
				m_cBoxTriggerSource.SetCurSel(0);
			else if (strTmp == _T("Software"))
				m_cBoxTriggerSource.SetCurSel(1);
		}
		else
			WriteLog(_T("Failed to read Trigger Source"));
	}
}


void CEasyAccessToolDlg::OnCbnSelchangeComboTriggerSource()
{
	int nTmp;
	CString strTmp;
	bool ret;
	nTmp = m_cBoxTriggerSource.GetCurSel();
	m_cBoxTriggerMode.GetLBText(nTmp, strTmp);
	strTmp.Remove(_T(' '));
	ret = m_Camera.SetTriggerSource(strTmp);
	if (ret)
		WriteLog(_T("Successed to write Trigger Source"));
	else
		WriteLog(_T("Failed to write Trigger Source"));
}


void CEasyAccessToolDlg::OnBnClickedBtnIniLoad()
{
	CFileDialog fileDlg(TRUE, _T("*.INI"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("All fileds(*.*)|*.*|"), this);

	if (fileDlg.DoModal() == IDOK)
	{
		CString path = fileDlg.GetPathName();
		CString strTmp;
		double dTmp;
		int nTmp;
		bool ret = false;
		ret = CINI::Load(path, _T("AutoAlgorithm"), _T("Exposure"), strTmp);
		if (ret)
		{
			WriteLog(_T("Successed to load Exposure Auto from INI"));
			if (strTmp == _T("Off"))
			{
				m_cBoxExposureAuto.SetCurSel(0);
				GetDlgItem(IDC_EDIT_EXPOSURE_TIME)->EnableWindow(TRUE);
				m_cBoxExposureMode.EnableWindow(TRUE);
			}
			else if (strTmp == _T("Once"))
				m_cBoxExposureAuto.SetCurSel(1);
			else if (strTmp == _T("Continuous"))
			{
				m_cBoxExposureAuto.SetCurSel(2);
				GetDlgItem(IDC_EDIT_EXPOSURE_TIME)->EnableWindow(FALSE);
				m_cBoxExposureMode.EnableWindow(FALSE);
			}
		}
		else
			WriteLog(_T("FAILED to load Exposure Auto from INI"));

		ret = CINI::Load(path, _T("AutoAlgorithm"), _T("Exposure Compensation"), strTmp);
		if (ret)
		{
			WriteLog(_T("Successed to load Exposure Compensation Auto from INI"));
			if (strTmp == _T("Off"))
			{
				m_cBoxExposureCompensationAuto.SetCurSel(0);
			}
			else if (strTmp == _T("Once"))
				m_cBoxExposureCompensationAuto.SetCurSel(1);
			else if (strTmp == _T("Continuous"))
			{
				m_cBoxExposureCompensationAuto.SetCurSel(2);
			}
		}
		else
			WriteLog(_T("FAILED to load Exposure Compensation Auto from INI"));

		ret = CINI::Load(path, _T("AutoAlgorithm"), _T("Gain"), strTmp);
		if (ret)
		{
			WriteLog(_T("Successed to load Gain Auto from INI"));
			if (strTmp == _T("Off"))
			{
				m_cBoxGainAuto.SetCurSel(0);
				GetDlgItem(IDC_EDIT_GAIN)->EnableWindow(TRUE);
			}
			else if (strTmp == _T("Once"))
				m_cBoxGainAuto.SetCurSel(1);
			else if (strTmp == _T("Continuous"))
			{
				m_cBoxGainAuto.SetCurSel(2);
				GetDlgItem(IDC_EDIT_GAIN)->EnableWindow(FALSE);
			}
		}
		else
			WriteLog(_T("FAILED to load Gain Auto from INI"));

		ret = CINI::Load(path, _T("AutoAlgorithm"), _T("Target Grey Value Auto"), strTmp);
		if (ret)
		{
			WriteLog(_T("Successed to load Target Grey Auto from INI"));
			if (strTmp == _T("Off"))
				m_cBoxTargetGreyAuto.SetCurSel(0);
			else if (strTmp == _T("Continuous"))
				m_cBoxTargetGreyAuto.SetCurSel(1);
		}
		else
			WriteLog(_T("FAILED to load Target Grey Auto from INI"));

		ret = CINI::Load(path, _T("AutoAlgorithm"), _T("White Balance Auto"), strTmp);
		if (ret)
		{
			WriteLog(_T("Successed to load White Balance Auto from INI"));
			if (strTmp == _T("Off"))
				m_cBoxWhiteBalanceAuto.SetCurSel(0);
			else if (strTmp == _T("Once"))
				m_cBoxWhiteBalanceAuto.SetCurSel(1);
			else if (strTmp == _T("Continuous"))
				m_cBoxWhiteBalanceAuto.SetCurSel(2);
		}
		else
			WriteLog(_T("Failed to load White Balance Auto from INI"));

		bool bTmp;
		ret = CINI::Load(path, _T("Parameters"), _T("Sharpness Enable"), bTmp);
		if (ret)
		{
			WriteLog(_T("Successed to load Sharpness Enable from INI"));
			CheckDlgButton(IDC_CBTN_SHARPNESS, bTmp);
		}
		else
			WriteLog(_T("Failed to load Sharpness Enable from INI"));

		ret = CINI::Load(path, _T("Parameters"), _T("Exposure Mode"), strTmp);
		if (ret)
		{
			WriteLog(_T("Successed to load Exposure Mode from INI"));
			if (strTmp == _T("Timed"))
			{
				CINI::Load(path, _T("AutoAlgorithm"), _T("Exposure"), strTmp);
				if(strTmp == _T("Off"))
				{
					m_cBoxExposureMode.SetCurSel(0);
					GetDlgItem(IDC_EDIT_EXPOSURE_TIME)->EnableWindow(TRUE);
				}
				else if (strTmp == _T("Trigger Width"))
				{
					m_cBoxExposureMode.SetCurSel(1);
					GetDlgItem(IDC_EDIT_EXPOSURE_TIME)->EnableWindow(FALSE);
				}
			}
		}

		ret = CINI::Load(path, _T("Parameters"), _T("Exposure Time"), nTmp);
		if (ret)
		{
			WriteLog(_T("Successed to load Exposure Time from INI"));
			SetDlgItemInt(IDC_EDIT_EXPOSURE_TIME, nTmp);
		}
		else
			WriteLog(_T("Failed to load Exposure Time from INI"));

		ret = CINI::Load(path, _T("Parameters"), _T("Gain"), dTmp);
		if (ret)
		{
			WriteLog(_T("Successed to load Gain from INI"));
			strTmp.Format(_T("%.1f"), dTmp);
			SetDlgItemText(IDC_EDIT_GAIN, strTmp);
		}
		else
			WriteLog(_T("Failed to load Gain from INI"));

		ret = CINI::Load(path, _T("Parameters"), _T("Trigger Mode"), strTmp);
		if (ret)
		{
			WriteLog(_T("Successed to load Trigger Mode from INI"));
			if (strTmp == _T("Off"))
			{
				m_cBoxTriggerMode.SetCurSel(0);
				m_cBoxTriggerSource.EnableWindow(FALSE);
			}
			else if (strTmp == _T("On"))
			{
				m_cBoxTriggerMode.SetCurSel(1);
				m_cBoxTriggerSource.EnableWindow(TRUE);
			}
		}
		else
			WriteLog(_T("Failed to load Trigger Mode from INI"));

		ret = CINI::Load(path, _T("Parameters"), _T("Trigger Source"), strTmp);
		if (ret)
		{
			WriteLog(_T("Successed to load Trigger Source from INI"));
			if (strTmp == _T("Line0"))
				m_cBoxTriggerSource.SetCurSel(0);
			else
				m_cBoxTriggerSource.SetCurSel(1);
		}
		else
			WriteLog(_T("Failed to load Trigger Mode from INI"));

		ret = CINI::Load(path, _T("Parameters"), _T("Width"), nTmp);
		if (ret)
		{
			WriteLog(_T("Successed to load Width from INI"));
			SetDlgItemInt(IDC_EDIT_WIDTH, nTmp);
		}
		else
			WriteLog(_T("Failed to load Width from INI"));

		ret = CINI::Load(path, _T("Parameters"), _T("Height"), nTmp);
		if (ret)
		{
			WriteLog(_T("Successed to load Height from INI"));
			SetDlgItemInt(IDC_EDIT_HEIGHT, nTmp);
		}
		else
			WriteLog(_T("Failed to load Height from INI"));

		ret = CINI::Load(path, _T("Parameters"), _T("Offset X"), nTmp);
		if (ret)
		{
			WriteLog(_T("Successed to load Offset X from INI"));
			SetDlgItemInt(IDC_EDIT_OFFSET_X, nTmp);
		}
		else
			WriteLog(_T("Failed to load Offset X from INI"));

		ret = CINI::Load(path, _T("Parameters"), _T("Offset Y"), nTmp);
		if (ret)
		{
			WriteLog(_T("Successed to load Offset Y from INI"));
			SetDlgItemInt(IDC_EDIT_OFFSET_Y, nTmp);
		}
		else
			WriteLog(_T("Failed to load Offset Y from INI"));

		//SetParameters();
	}
}


void CEasyAccessToolDlg::OnBnClickedBtnIniSave()
{
	CFileDialog fileDlg(TRUE, _T("*.ini"), NULL, OFN_OVERWRITEPROMPT, _T("All fileds(*.*)|*.*|"), this);

	if (fileDlg.DoModal() == IDOK)
	{
		CString path = fileDlg.GetPathName();
		CString strTmp;
		double dTmp;
		int nTmp;
		bool ret = false;

// 		CStdioFile file;
// 		if (file.Open(path, CFile::modeReadWrite ) == FALSE)
// 		{
// 			WriteLog(_T("Failed to Create INI File"));
// 			return;
// 		}

		nTmp = m_cBoxExposureAuto.GetCurSel();
		if (nTmp != -1)
		{
			m_cBoxExposureAuto.GetLBText(nTmp, strTmp);
			ret = CINI::Save(path, _T("AutoAlgorithm"), _T("Exposure"), strTmp);
			if (ret)
				WriteLog(_T("Successed to save Exposure Auto to INI"));
			else
				WriteLog(_T("Failed to save Exposure Auto to INI"));
		}

		nTmp = m_cBoxExposureCompensationAuto.GetCurSel();
		if (nTmp != -1)
		{
			m_cBoxExposureCompensationAuto.GetLBText(nTmp, strTmp);
			ret = CINI::Save(path, _T("AutoAlgorithm"), _T("Exposure Compensation"), strTmp);
			if (ret)
				WriteLog(_T("Successed to save Exposure Auto to INI"));
			else
				WriteLog(_T("Failed to save Exposure Auto to INI"));
		}


		nTmp = m_cBoxGainAuto.GetCurSel();
		if (nTmp != -1)
		{
			m_cBoxGainAuto.GetLBText(nTmp, strTmp);
			ret = CINI::Save(path, _T("AutoAlgorithm"), _T("Gain"), strTmp);
			if (ret)
				WriteLog(_T("Successed to save Gain Auto to INI"));
			else
				WriteLog(_T("Failed to save Gain Auto to INI"));
		}

		nTmp = m_cBoxTargetGreyAuto.GetCurSel();
		if (nTmp != -1)
		{
			m_cBoxTargetGreyAuto.GetLBText(nTmp, strTmp);
			ret = CINI::Save(path, _T("AutoAlgorithm"), _T("Target Grey Value Auto"), strTmp);
			if (ret)
				WriteLog(_T("Successed to save Target Grey Auto to INI"));
			else
				WriteLog(_T("Failed to save Target Grey Auto to INI"));
		}

		nTmp = m_cBoxWhiteBalanceAuto.GetCurSel();
		if (nTmp != -1)
		{
			m_cBoxWhiteBalanceAuto.GetLBText(nTmp, strTmp);
			ret = CINI::Save(path, _T("AutoAlgorithm"), _T("White Balance Auto"), strTmp);
			if (ret)
				WriteLog(_T("Successed to save White Balance to INI"));
			else
				WriteLog(_T("Failed to save White Balance to INI"));
		}
		
		bool bTmp = GetDlgItem(IDC_CBTN_SHARPNESS)->IsWindowEnabled();
		if (bTmp)
		{
			bool Tmp = IsDlgButtonChecked(IDC_CBTN_SHARPNESS);
			if (Tmp)
				ret = CINI::Save(path, _T("Parameters"), _T("Sharpness Enable"), Tmp);
			if (ret)
				WriteLog(_T("Successed to save Shapness Enable to INI"));
			else
				WriteLog(_T("Failed to save Sharpness Enable to INI"));
		}
		
		

		nTmp = m_cBoxExposureMode.GetCurSel();
		if (nTmp != -1)
		{
			m_cBoxExposureMode.GetLBText(nTmp, strTmp);
			ret = CINI::Save(path, _T("Parameters"), _T("Exposure Mode"), strTmp);
			if (ret)
				WriteLog(_T("Successed to save Exposure Mode to INI"));
			else
				WriteLog(_T("Failed to save Exposure Mode to INI"));
		}

		nTmp = GetDlgItemInt(IDC_EDIT_EXPOSURE_TIME);
		if (nTmp != -1)
		{
			ret = CINI::Save(path, _T("Parameters"), _T("Exposure Time"), nTmp);
			if (ret)
				WriteLog(_T("Successed to save Exposure Time to INI"));
			else
				WriteLog(_T("Failed to save Exposure Time to INI"));
		}

		GetDlgItemText(IDC_EDIT_GAIN, strTmp);
		ret = CINI::Save(path, _T("Parameters"), _T("Gain"), strTmp);
		if (ret)
			WriteLog(_T("Successed to save Gain to INI"));
		else
			WriteLog(_T("Failed to save Gain to INI"));

		nTmp = m_cBoxTriggerMode.GetCurSel();
		m_cBoxTriggerMode.GetLBText(nTmp, strTmp);
		ret = CINI::Save(path, _T("Parameters"), _T("Trigger Mode"), strTmp);
		if (ret)
			WriteLog(_T("Successed to save Trigger Mode to INI"));
		else
			WriteLog(_T("Failed to save Trigger Mode to INI"));

		nTmp = m_cBoxTriggerSource.GetCurSel();
		m_cBoxTriggerSource.GetLBText(nTmp, strTmp);
		ret = CINI::Save(path, _T("Parameters"), _T("Trigger Source"), strTmp);
		if (ret)
			WriteLog(_T("Succcessed to save Trigger Source to INI"));
		else
			WriteLog(_T("Failed to save Trigger Source to INI"));

		nTmp = GetDlgItemInt(IDC_EDIT_WIDTH);
		ret = CINI::Save(path, _T("Parameters"), _T("Width"), nTmp);
		if (ret)
			WriteLog(_T("Successed to save Width to INI"));
		else
			WriteLog(_T("Failed to save Width to INI"));

		nTmp = GetDlgItemInt(IDC_EDIT_HEIGHT);
		ret = CINI::Save(path, _T("Parameters"), _T("Height"), nTmp);
		if (ret)
			WriteLog(_T("Successed to save Height to INI"));
		else
			WriteLog(_T("Failed to save Height to INI"));

		nTmp = GetDlgItemInt(IDC_EDIT_OFFSET_X);
		nTmp = CINI::Save(path, _T("Parameters"), _T("Offset X"), nTmp);
		if (ret)
			WriteLog(_T("Successed to save Offset X to INI"));
		else
			WriteLog(_T("Failed to save Offset X to INI"));

		nTmp = GetDlgItemInt(IDC_EDIT_OFFSET_Y);
		nTmp = CINI::Save(path, _T("Parameters"), _T("Offset Y"), nTmp);
		if (ret)
			WriteLog(_T("Successed to save Offset Y to INI"));
		else
			WriteLog(_T("Failed to save Offset Y to INI"));
	}
}


void CEasyAccessToolDlg::OnBnClickedCbtnSharpness()
{
	bool bTmp = IsDlgButtonChecked(IDC_CBTN_SHARPNESS);

	m_Camera.SetSharpnessEnable(bTmp);
}


void CEasyAccessToolDlg::OnCbnSelchangeComboAutoExposureCompensation()
{
	int nTmp;
	CString strTmp;
	bool ret;
	nTmp = m_cBoxExposureCompensationAuto.GetCurSel();
	m_cBoxWhiteBalanceAuto.GetLBText(nTmp, strTmp);
	ret = m_Camera.SetExposureCompensationAuto(strTmp);
	if (ret)
		WriteLog(_T("Successed to write Exposure Compensation Auto"));
	else
		WriteLog(_T("Failed to write Exposure Compensation Auto"));
}


void CEasyAccessToolDlg::OnBnClickedBtnHide()
{
	m_fnSetShowFlag( FALSE );
}
#endif
