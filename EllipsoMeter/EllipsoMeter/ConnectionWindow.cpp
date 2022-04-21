// ConnectionWindow.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#if SPINNAKER_CAMERA
#include "FlirCamera.h"
#include "ConnectionWindow.h"
#include "resource.h"
#include "afxdialogex.h"


// ConnectionWindow 대화 상자입니다.

IMPLEMENT_DYNAMIC(ConnectionWindow, CDialogEx)

ConnectionWindow::ConnectionWindow(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CONNECTIONWINDOW, pParent)
{
	//this->Create( IDD_CONNECTIONWINDOW );
}

ConnectionWindow::~ConnectionWindow()
{
}

void ConnectionWindow::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LTC_DEVICE, m_LtcDevice);
	DDX_Control(pDX, IDC_IP_PERSISTANT, m_PersistantIP);
}


BEGIN_MESSAGE_MAP(ConnectionWindow, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_REFRESH, &ConnectionWindow::OnBnClickedBtnRefresh)
	ON_BN_CLICKED(IDC_BTN_FORCEIP, &ConnectionWindow::OnBnClickedBtnForceip)
	ON_BN_CLICKED(IDC_BTN_CONNECT, &ConnectionWindow::OnBnClickedBtnConnect)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_PERSISTNAT, &ConnectionWindow::OnBnClickedBtnPersistnat)
END_MESSAGE_MAP()


// ConnectionWindow 메시지 처리기입니다.

BOOL ConnectionWindow::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetWindowText(_T("FAINSTEC Easy Access Tool FLIR Camera Connection Window"));;

	GetDlgItem(IDC_BTN_PERSISTNAT)->EnableWindow(FALSE);
	m_nSelectPos = -1;

	m_LtcDevice.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_LtcDevice.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_LtcDevice.InsertColumn(0, _T("Interface Name"), NULL, 130);
	m_LtcDevice.InsertColumn(1, _T("Model"), NULL, 180);
	m_LtcDevice.InsertColumn(2, _T("Interface"), NULL, 80);
	m_LtcDevice.InsertColumn(3, _T("Serial Number"), NULL, 100);
	m_LtcDevice.InsertColumn(4, _T("Device Status"), NULL, 100);
	m_LtcDevice.InsertColumn(5, _T("IPAddress"), NULL, 130);
	m_LtcDevice.InsertColumn(6, _T("Control"), NULL, 130);

	OnBnClickedBtnRefresh();

	SetTimer(100, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
void ConnectionWindow::OnInitial()
{
	/*CDialogEx::OnInitDialog();

	SetWindowText(_T("FAINSTEC Easy Access Tool FLIR Camera Connection Window"));;

	GetDlgItem(IDC_BTN_PERSISTNAT)->EnableWindow(FALSE);
	m_nSelectPos = -1;

	m_LtcDevice.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_LtcDevice.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_LtcDevice.InsertColumn(0, _T("Interface Name"), NULL, 130);
	m_LtcDevice.InsertColumn(1, _T("Model"), NULL, 180);
	m_LtcDevice.InsertColumn(2, _T("Interface"), NULL, 80);
	m_LtcDevice.InsertColumn(3, _T("Serial Number"), NULL, 100);
	m_LtcDevice.InsertColumn(4, _T("Device Status"), NULL, 100);
	m_LtcDevice.InsertColumn(5, _T("IPAddress"), NULL, 130);
	m_LtcDevice.InsertColumn(6, _T("Control"), NULL, 130);

	OnBnClickedBtnRefresh();

	SetTimer(100, 100, NULL);*/
}

void ConnectionWindow::OnBnClickedBtnRefresh()
{
	::SetCursor(::LoadCursor(NULL, IDC_WAIT));
	m_LtcDevice.DeleteAllItems();

	CEasyAccessToolDlg *pMain = (CEasyAccessToolDlg*)GetParent();

	pMain->m_Camera.OnUpdateList();
	pMain->m_Camera.OnSearchCameraOnSystem();

	int nDevice;
	pMain->m_Camera.GetCameraCount(&nDevice);

	for (int n = 0; n < nDevice; n++)
	{
		m_LtcDevice.InsertItem(n, pMain->m_Camera.GetInterfaceDisplayName(n));
		m_LtcDevice.SetItem(n, 1, LVIF_TEXT, pMain->m_Camera.GetModelName(n), 0, 0, 0, NULL);
		m_LtcDevice.SetItem(n, 2, LVIF_TEXT, pMain->m_Camera.GetInterfaceType(n), 0, 0, 0, NULL);
		m_LtcDevice.SetItem(n, 3, LVIF_TEXT, pMain->m_Camera.GetSerialNumber(n), 0, 0, 0, NULL);
		m_LtcDevice.SetItem(n, 4, LVIF_TEXT, pMain->m_Camera.GetAccessStatus(n), 0, 0, 0, NULL);
		m_LtcDevice.SetItem(n, 5, LVIF_TEXT, pMain->m_Camera.GetIPAddress(n), 0, 0, 0, NULL);
		m_LtcDevice.SetItem(n, 6, LVIF_TEXT, pMain->m_Camera.GetGEVCCP(n), 0, 0, 0, NULL);
	}

	::SetCursor(::LoadCursor(NULL, IDC_ARROW));
	pMain->WriteLog(_T("Execute Refresh"));
}

void ConnectionWindow::OnBnClickedBtnForceip()
{
	::SetCursor(::LoadCursor(NULL, IDC_WAIT));
	CEasyAccessToolDlg *pMain = (CEasyAccessToolDlg*)GetParent();
	pMain->m_Camera.ExecuteForceIP();
	Sleep(1000);
	OnBnClickedBtnRefresh();
	::SetCursor(::LoadCursor(NULL, IDC_ARROW));
	pMain->WriteLog(_T("Execute Force IP"));
}




void ConnectionWindow::OnBnClickedBtnConnect()
{
	::SetCursor(::LoadCursor(NULL, IDC_WAIT));
	CEasyAccessToolDlg *pMain = (CEasyAccessToolDlg*)GetParent();

	int nPos = 0;


	POSITION pos = NULL;
	pos = m_LtcDevice.GetFirstSelectedItemPosition();

	if (pos != NULL)
		nPos = m_LtcDevice.GetNextSelectedItem(pos);

	if (pMain->m_Camera.OnConnectByIndex(nPos))
		::SendMessage(this->m_hWnd, WM_CLOSE, NULL, NULL);
	else
		MessageBox(_T("FAILED to connection seleted camera"), _T("Connection Error"));

	::SetCursor(::LoadCursor(NULL, IDC_ARROW));
}


void ConnectionWindow::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 100)
	{
		POSITION pos = NULL;
		pos = m_LtcDevice.GetFirstSelectedItemPosition();
		if (pos != NULL)
		{
			int nPos;
			nPos = m_LtcDevice.GetNextSelectedItem(pos);
			if (m_LtcDevice.GetItemText(nPos, 2) == _T("GEV") && m_LtcDevice.GetItemText(nPos, 4) == _T("ReadWrite"))
			{
				m_nSelectPos = nPos;
				GetDlgItem(IDC_BTN_PERSISTNAT)->EnableWindow(TRUE);
			}
			else
			{
				GetDlgItem(IDC_BTN_PERSISTNAT)->EnableWindow(FALSE);
			}
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}


void ConnectionWindow::OnDestroy()
{
	CDialogEx::OnDestroy();

	KillTimer(100);
}


void ConnectionWindow::OnBnClickedBtnPersistnat()
{
	::SetCursor(::LoadCursor(NULL, IDC_WAIT));
	CEasyAccessToolDlg *pMain = (CEasyAccessToolDlg*)GetParent();

	BYTE A, B, C, D;
	m_PersistantIP.GetAddress(A, B, C, D);
	CString tmp;
	tmp.Format(_T("%d.%d.%d.%d"), A, B, C, D);
	pMain->m_Camera.OnConnectByIndex(m_nSelectPos);
	pMain->m_Camera.SetPersistentIP(tmp);
	Sleep(15000);
	::SetCursor(::LoadCursor(NULL, IDC_ARROW));
	pMain->WriteLog(_T("Execute Persistant IP"));
}


BOOL ConnectionWindow::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			if (GetDlgItem(IDC_IP_PERSISTANT)->IsChild(GetFocus()))
			{
				OnBnClickedBtnPersistnat();
				return true;
			}
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
#endif