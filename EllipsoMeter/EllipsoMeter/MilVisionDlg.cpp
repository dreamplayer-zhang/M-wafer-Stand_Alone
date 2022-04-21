
// MilVisionDlg.cpp : 구현 파일
//
#pragma once

#include "stdafx.h"
#if MIL_USE
#include "MilVision.h"
#include "MilVisionDlg.h"

#include "ModelData.h"
#include "Task.h"

#include "CamJAI.h"
#include "AJIN_CtlClass.h"

#include "WorkRun.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMilVisionDlg 대화 상자


CMilVisionDlg* pMILAlignDlg;

CMilVisionDlg::CMilVisionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMilVisionDlg::IDD, pParent)
	, m_iComboPattNo(0)
	, m_dEditImgResol(0)
	, m_bRegModFlag(FALSE)
	, m_iDegCalStep(0)
{
	m_hMutexOneInstance = NULL;
// 	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_iDispNum = DEF_DISP_SET;
	bLClick = FALSE;
	bDragPt = FALSE;

	rcMark.left = (DEF_IMAGE_X - DEF_MARK_SIZE_X) >> 1;
	rcMark.top = (DEF_IMAGE_Y - DEF_MARK_SIZE_Y) >> 1;
	rcMark.right = (DEF_IMAGE_X + DEF_MARK_SIZE_X) >> 1;
	rcMark.bottom = (DEF_IMAGE_Y + DEF_MARK_SIZE_Y) >> 1;

	this->Create(IDD_MILVISION_DIALOG);
}

CMilVisionDlg::~CMilVisionDlg()
{

}

void CMilVisionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RESULT, m_ctrlListResult);
	DDX_Control(pDX, IDC_COMBO_PATT, m_ctrlComboPattNo);
	DDX_Control(pDX, IDC_CHECK_DISP_CENTER, m_ctrlChkDispCenter);
	DDX_CBIndex(pDX, IDC_COMBO_PATT, m_iComboPattNo);
	DDX_Control(pDX, IDC_LIST_PARA, m_ctrlListPara);
	DDX_Control(pDX, IDC_SCROLLBAR_PARA, m_ctrlScrollVal);
	DDX_Text(pDX, IDC_EDIT_IMG_RESOL, m_dEditImgResol);
	DDV_MinMaxDouble(pDX, m_dEditImgResol, 0.0, 1.0);
	DDX_Control(pDX, IDC_EDIT_IMG_RESOL, m_ctrlEditImgResol);
}

BEGIN_MESSAGE_MAP(CMilVisionDlg, CDialog)
	ON_WM_PAINT()
// 	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_PATT_REGI, &CMilVisionDlg::OnBnClickedButtonPattRegi)
	ON_BN_CLICKED(IDC_BUTTON_PATT_FIND, &CMilVisionDlg::OnBnClickedButtonPattFind)
	ON_COMMAND(ID__IMAGELOAD, &CMilVisionDlg::PopupImageLoad)
	ON_COMMAND(ID__IMAGESAVE, &CMilVisionDlg::PopupImageSave)
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_CHECK_DISP_CENTER, &CMilVisionDlg::OnBnClickedCheckDispCenter)
	ON_CBN_SELCHANGE(IDC_COMBO_PATT, &CMilVisionDlg::OnCbnSelchangeComboPatt)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CMilVisionDlg::OnBnClickedButtonSave)
	ON_WM_HSCROLL()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_PARA, &CMilVisionDlg::OnNMDblclkListPara)
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_CAL, &CMilVisionDlg::OnBnClickedButtonCal)
	ON_BN_CLICKED(IDC_BUTTON_POINT_CALI, &CMilVisionDlg::OnBnClickedButtonPointCali)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_FIND_EDGE_PNT, &CMilVisionDlg::OnBnClickedButtonFindEdgePnt)
// 	ON_BN_CLICKED(IDC_BUTTON_CUR_IMG_APLLY, &CMilVisionDlg::OnBnClickedButtonCurImgAplly)
	ON_BN_CLICKED(IDC_BUTTON_PATTERN_LOAD, &CMilVisionDlg::OnBnClickedButtonPatternLoad)
	ON_BN_CLICKED(IDOK, &CMilVisionDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_DEGREE_CAL, &CMilVisionDlg::OnBnClickedButtonDegreeCal)
	ON_BN_CLICKED(IDC_BUTTON_REFLASH_IMG, &CMilVisionDlg::OnBnClickedButtonReflashImg)
END_MESSAGE_MAP()


// CMilVisionDlg 메시지 처리기

BOOL CMilVisionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

// 	theApp.SetViewPointer(this);

// 	m_hMutexOneInstance = ::CreateMutex(NULL, TRUE, _T("PatternMatchVision"));
// 	if(::GetLastError() == ERROR_ALREADY_EXISTS)
// 	{
// 		AfxMessageBox("PatternMatchVision 모듈이 이미 실행중입니다.");
// 		if ( m_hMutexOneInstance != NULL )
// 		{
// 			::ReleaseMutex(m_hMutexOneInstance);
// 			::CloseHandle(m_hMutexOneInstance);
// 			m_hMutexOneInstance = NULL;
// 		}
// 		exit(1);
// 		return FALSE;
// 	}

	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

	Initial_ObjPtr();

	initDisplay();

	MbufCopy(pInsp[DEF_DISP_SET]->MilBuf_Insp_Image, pDisplay[DEF_DISP_SET]->MilBuf_Set_View);//m_ImageInsp 버프에 영상넣어두면 다이얼로그 뜰때 표시

	int i;
	CString strTemp;
	for (i = 0; i < DEF_MAX_MARK_NUM; i++)
	{
		strTemp.Format(_T("패턴 No.%d"), i + 1);
		m_ctrlComboPattNo.AddString(strTemp);
	}

	m_ctrlComboPattNo.SetCurSel(m_iComboPattNo);
	pInsp[m_iDispNum]->ModLoad(m_iComboPattNo);
	MmodDraw(pInsp[m_iDispNum]->Milmodgra, pInsp[m_iDispNum]->MilImageMod[m_iComboPattNo], pDisplay[m_iDispNum]->MilOvr[DEF_OVR_MARK].MilOverlayImage, M_DRAW_IMAGE + M_DRAW_EDGES + M_DRAW_POSITION, M_DEFAULT, M_DEFAULT);

	SetListParaData();

	SetTimer(TIMER_SET_PATT_MASTER, 300, NULL);
	SetTimer(TIMER_SET_MOTOR_ALIGN, 300, NULL);
	SetTimer(TIMER_GET_PATT_CENTER, 300, NULL);
	SetTimer(TIMER_GET_TRANS_COORDINATE, 300, NULL);

	m_ctrlChkDispCenter.SetCheck(TRUE);

	this->ShowWindow(SW_HIDE);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMilVisionDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
// 		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
// HCURSOR CMilVisionDlg::OnQueryDragIcon()
// {
// 	return static_cast<HCURSOR>(m_hIcon);
// }

void CMilVisionDlg::Initial_ObjPtr()
{
	int i = 0;

	pModelData = (CModelData*)new CModelData;
	pModelData_Backup = (CModelData*)new CModelData;
	pModelData->ListLoad();
	pModelData->Load();
	pModelData->iModNumber = 1;//무조건 1개 강제 지정
	memcpy(pModelData_Backup, pModelData, sizeof(CModelData));

	pTask = (CTask *)new CTask;

	pVision = (CVision *)new CVision;
	pDisplay[DEF_DISP_SET] = (CDisplay *)new CDisplay(pVision->MilSystem, 1, DEF_IMAGE_X, DEF_IMAGE_Y, DEF_ZOOM, DEF_ZOOM, DEF_DISP_SET);
	pInsp[DEF_DISP_SET] = (CMilInspFunc *)new CMilInspFunc;
}

void CMilVisionDlg::initDisplay()
{
	int i = 0;
	int j = 0;

	CWnd* m_pWnd[MAX_OVR_CNT];

	m_pWnd[DEF_OVR_SET] = (CWnd*)GetDlgItem(IDC_DISP_SET);
	m_pWnd[DEF_OVR_MARK] = (CWnd*)GetDlgItem(IDC_DISP_MARK);

	MIL_DOUBLE sX, sY;

	j = DEF_DISP_SET;
	for (i = 0; i < MAX_OVR_CNT; i++)
	{
		if (pDisplay[j] == nullptr)
			continue;

		if (m_pWnd[i] == NULL) continue;

		MdispAlloc(pVision->MilSystem, M_DEFAULT, _T("M_DEFAULT"), M_DEFAULT, &pDisplay[j]->MilOvr[i].MilDisplay);

		switch (i)
		{
		case DEF_OVR_SET:
			MdispSelectWindow(pDisplay[j]->MilOvr[i].MilDisplay, pDisplay[j]->MilBuf_Set_View, m_pWnd[i]->m_hWnd);

			sX = (MIL_DOUBLE)MbufInquire(pDisplay[j]->MilBuf_Set_View, M_SIZE_X, M_NULL)/DEF_ZOOM;//2464
			sY = (MIL_DOUBLE)MbufInquire(pDisplay[j]->MilBuf_Set_View, M_SIZE_Y, M_NULL)/DEF_ZOOM;//2464
			break;
		case DEF_OVR_MARK:
			MdispSelectWindow(pDisplay[j]->MilOvr[i].MilDisplay, pDisplay[j]->MilBuf_Mark_View, m_pWnd[i]->m_hWnd);
			sX = (MIL_DOUBLE)DEF_MARK_SIZE_X/DEF_ZOOM;
			sY = (MIL_DOUBLE)DEF_MARK_SIZE_Y/DEF_ZOOM;
			break;
		default:
			return;
		}
		MdispZoom(pDisplay[j]->MilOvr[i].MilDisplay, 1.0/DEF_ZOOM, 1.0/DEF_ZOOM);
		pDisplay[j]->OverlayEnableCam(i);


		m_pWnd[i]->GetWindowRect(&pDisplay[j]->MilOvr[i].rectPic);
		ScreenToClient(pDisplay[j]->MilOvr[i].rectPic);

		pDisplay[j]->MilOvr[i].rectPic.right	= pDisplay[j]->MilOvr[i].rectPic.left + (LONG)sX;
		pDisplay[j]->MilOvr[i].rectPic.bottom	= pDisplay[j]->MilOvr[i].rectPic.top + (LONG)sY;

		m_pWnd[i]->SetWindowPos(NULL, 0, 0, pDisplay[j]->MilOvr[i].rectPic.Width(), pDisplay[j]->MilOvr[i].rectPic.Height(), SWP_NOMOVE);
	}

}

void CMilVisionDlg::OnDestroy()
{
	CDialog::OnDestroy();

	KillTimer(TIMER_SET_PATT_MASTER);
	KillTimer(TIMER_SET_MOTOR_ALIGN);
	KillTimer(TIMER_GET_PATT_CENTER);
	KillTimer(TIMER_GET_TRANS_COORDINATE);

// 	//Display free
// 	for (int j = MAX_DISP_CNT - 1; j >= 0; j--)
// 	{
// 		for (int i = MAX_OVR_CNT - 1; i >= 0; i--)
// 		{
// 
// 			if (pDisplay[j]->MilOvr[i].MilDisplay != M_NULL)
// 			{
// 				MdispSelectWindow(pDisplay[j]->MilOvr[i].MilDisplay, M_NULL, M_NULL);
// 				MdispFree(pDisplay[j]->MilOvr[i].MilDisplay);
// 				pDisplay[j]->MilOvr[i].MilDisplay = M_NULL;
// 			}
// 		}
// 	}

	Close_Object();

	if ( m_hMutexOneInstance != NULL )
	{
		::ReleaseMutex(m_hMutexOneInstance);
		::CloseHandle(m_hMutexOneInstance);
		m_hMutexOneInstance = NULL;
	}
}

void CMilVisionDlg::Close_Object()
{
	int i = 0;

	for (i = MAX_DISP_CNT - 1; i >= 0; i--)
	{
		if (pInsp[i] != NULL)
		{
			delete pInsp[i];
			pInsp[i] = NULL;
		}
	}

	for (i = MAX_DISP_CNT - 1; i >= 0; i--)
	{
		if (pDisplay[i] != NULL)
		{
			delete pDisplay[i];
			pDisplay[i] = NULL;
		}
	}

	if (pVision != NULL)
	{
		delete pVision;
		pVision = NULL;
	}



	if (pTask != NULL)
	{
		delete pTask;
		pTask = NULL;
	}

	if (pModelData_Backup != NULL)
	{
		delete pModelData_Backup;
		pModelData_Backup = NULL;
	}

	if (pModelData != NULL)
	{
		delete pModelData;
		pModelData = NULL;
	}
}

void CMilVisionDlg::putListData(TCHAR *tmpStr)
{
	if (m_ctrlListResult.GetCount() > DEF_MARK_SIZE_X)
		m_ctrlListResult.ResetContent();
	m_ctrlListResult.InsertString(-1, tmpStr); // 0.05ms

	m_ctrlListResult.SetCurSel(m_ctrlListResult.GetCount() - 1); //0.5ms
}

void CMilVisionDlg::OnBnClickedButtonPattRegi()
{
	TCHAR tmpStr[CHAR_BUFF];

	pDisplay[m_iDispNum]->addBoxList(rcMark, M_COLOR_MAGENTA, DEF_OVR_SET);
	rcMark.left -= 1;
	rcMark.top -= 1;
	rcMark.right += 1;
	rcMark.bottom += 1;
	pDisplay[m_iDispNum]->addBoxList(rcMark, M_COLOR_MAGENTA, DEF_OVR_SET);
	rcMark.left += 2;
	rcMark.top += 2;
	rcMark.right -= 2;
	rcMark.bottom -= 2;
	pDisplay[m_iDispNum]->addBoxList(rcMark, M_COLOR_MAGENTA, DEF_OVR_SET);
	rcMark.left -= 1;
	rcMark.top -= 1;
	rcMark.right += 1;
	rcMark.bottom += 1;

	pDisplay[m_iDispNum]->drawOverlay(DEF_OVR_SET);


	pInsp[m_iDispNum]->ptrMarkStart = rcMark.TopLeft();
	pInsp[m_iDispNum]->ptrMarkSize.x = rcMark.Width();
	pInsp[m_iDispNum]->ptrMarkSize.y = rcMark.Height();

	if (pInsp[m_iDispNum]->ptrMarkSize.x > DEF_MARK_SIZE_X || pInsp[m_iDispNum]->ptrMarkSize.y > DEF_MARK_SIZE_Y)
	{
		sprintf_s(tmpStr, sizeof(tmpStr), _T("Over to Maximum Mark Size.(%dX%d)"), DEF_MARK_SIZE_X, DEF_MARK_SIZE_Y);
		AfxMessageBox(tmpStr);
		return;
	}

	int niMarkNo = 0;// m_iComboPattNo;
	MIL_DOUBLE Mcolor = M_COLOR_RED;
	// 	Mcolor = m_ctrlChkMarkdrawBW.GetCheck() ? M_COLOR_WHITE : M_COLOR_BLACK;
	pInsp[m_iDispNum]->ModRegist(niMarkNo, Mcolor);

	//마크 이미지 버퍼에 Draw
	pDisplay[m_iDispNum]->OverlayClear(DEF_OVR_MARK);
	MbufClear(pDisplay[m_iDispNum]->MilBuf_Mark_View, 0);
	MbufCopyClip(pDisplay[m_iDispNum]->MilBuf_Origin_Image, pDisplay[m_iDispNum]->MilBuf_Mark_View, -pInsp[m_iDispNum]->ptrMarkStart.x, -pInsp[m_iDispNum]->ptrMarkStart.y);
	MmodDraw(pInsp[m_iDispNum]->Milmodgra, pInsp[m_iDispNum]->MilImageMod[niMarkNo], pDisplay[m_iDispNum]->MilOvr[DEF_OVR_MARK].MilOverlayImage, /*M_DRAW_IMAGE +*/ M_DRAW_EDGES /*+ M_DRAW_POSITION*/, M_DEFAULT, M_DEFAULT);
	MgraColor(pInsp[m_iDispNum]->Milmodgra, M_COLOR_YELLOW);
	MmodDraw(pInsp[m_iDispNum]->Milmodgra, pInsp[m_iDispNum]->MilImageMod[niMarkNo], pDisplay[m_iDispNum]->MilOvr[DEF_OVR_MARK].MilOverlayImage, /*M_DRAW_IMAGE +*/ /*M_DRAW_EDGES + */M_DRAW_POSITION, M_DEFAULT, M_DEFAULT);
	
// 	MIL_DOUBLE Xscale = (MIL_DOUBLE)(1.0 / (double)DEF_ZOOM);
// 	MimResize(pDisplay[m_iDispNum]->MilBuf_Mark_View, pDisplay[m_iDispNum]->MilBuf_Set_View, Xscale, Xscale, M_DEFAULT);

	sprintf_s(tmpStr, sizeof(tmpStr), _T("Mark regist Complete"));
	putListData(tmpStr);

	GetDlgItem(IDOK)->EnableWindow(TRUE);
// 	m_bRegModFlag = TRUE;
	pWorkRun->m_bSqncInterrupt[SQITRT_REG_PATTERN] = TRUE;
}

void CMilVisionDlg::OnBnClickedButtonPattFind()
{
	pDisplay[m_iDispNum]->OverlayClear(DEF_OVR_SET);

	TCHAR tmpStr[CHAR_BUFF];

	int niMarkNo = 0;//m_iComboPattNo;
	sprintf_s(tmpStr, sizeof(tmpStr), _T("Model Finder Start.%d"), niMarkNo + 1);
	putListData(tmpStr);

	// 	MmodPreprocess(pInsp[iDispNum]->MilImageMod[niMarkNo], M_DEFAULT);
	pInsp[m_iDispNum]->ModFind(pInsp[m_iDispNum]->MilBuf_Insp_Image, tmpStr, niMarkNo);
	putListData(tmpStr);

	if(pInsp[m_iDispNum]->nCount > 0)
	{
		CRect rcFindMark;
		rcFindMark.left		= (LONG)pTask->cdTargetPos.x - (rcMark.Width()/2);
		rcFindMark.right	= (LONG)pTask->cdTargetPos.x + (rcMark.Width()/2);
		rcFindMark.top		= (LONG)pTask->cdTargetPos.y - (rcMark.Height()/2);
		rcFindMark.bottom	= (LONG)pTask->cdTargetPos.y + (rcMark.Height()/2);

		pDisplay[m_iDispNum]->addBoxList(rcFindMark, M_COLOR_MAGENTA, DEF_OVR_SET);
		pDisplay[m_iDispNum]->addCrossList(rcFindMark.CenterPoint().x, rcFindMark.CenterPoint().y, 30, M_COLOR_MAGENTA, DEF_OVR_SET);
		rcFindMark.left -= 1;
		rcFindMark.top -= 1;
		rcFindMark.right += 1;
		rcFindMark.bottom += 1;
		pDisplay[m_iDispNum]->addBoxList(rcFindMark, M_COLOR_MAGENTA, DEF_OVR_SET);
		pDisplay[m_iDispNum]->addCrossList(rcFindMark.CenterPoint().x, rcFindMark.CenterPoint().y, 30, M_COLOR_MAGENTA, DEF_OVR_SET);
		rcFindMark.left += 2;
		rcFindMark.top += 2;
		rcFindMark.right -= 2;
		rcFindMark.bottom -= 2;
		pDisplay[m_iDispNum]->addBoxList(rcFindMark, M_COLOR_MAGENTA, DEF_OVR_SET);
		pDisplay[m_iDispNum]->addCrossList(rcFindMark.CenterPoint().x, rcFindMark.CenterPoint().y, 30, M_COLOR_MAGENTA, DEF_OVR_SET);
		rcFindMark.left -= 1;
		rcFindMark.top -= 1;
		rcFindMark.right += 1;
		rcFindMark.bottom += 1;

		pDisplay[m_iDispNum]->drawOverlay(DEF_OVR_SET);

		MgraColor(pInsp[m_iDispNum]->MilmodResultgra, M_COLOR_BLUE);
		MmodDraw(pInsp[m_iDispNum]->MilmodResultgra, pInsp[m_iDispNum]->MilResultMod[niMarkNo], pDisplay[m_iDispNum]->MilOvr[DEF_OVR_SET].MilOverlayImage, M_DRAW_EDGES /*+ M_DRAW_POSITION*/, M_DEFAULT, M_DEFAULT);
		MgraColor(pInsp[m_iDispNum]->MilmodResultgra, M_COLOR_YELLOW);
		MmodDraw(pInsp[m_iDispNum]->MilmodResultgra, pInsp[m_iDispNum]->MilResultMod[niMarkNo], pDisplay[m_iDispNum]->MilOvr[DEF_OVR_SET].MilOverlayImage, /*M_DRAW_EDGES + */M_DRAW_POSITION, M_DEFAULT, M_DEFAULT);
	}
}

void CMilVisionDlg::PopupImageLoad()
{
	pDisplay[m_iDispNum]->ImageLoad_Browser();

	MIL_DOUBLE sX1 = (MIL_DOUBLE)MbufInquire(pDisplay[m_iDispNum]->MilBuf_Origin_Image, M_SIZE_X, M_NULL);//2464
	MIL_DOUBLE sX2 = (MIL_DOUBLE)MbufInquire(pDisplay[m_iDispNum]->MilBuf_Set_View, M_SIZE_X, M_NULL);
	MIL_DOUBLE SizeRate = sX2/sX1;

	MimResize(pDisplay[m_iDispNum]->MilBuf_Origin_Image, pDisplay[m_iDispNum]->MilBuf_Set_View, SizeRate, SizeRate, M_DEFAULT);
	MbufCopy(pDisplay[m_iDispNum]->MilBuf_Origin_Image, pInsp[m_iDispNum]->MilBuf_Insp_Image);
// 	pInsp[m_iDispNum]->MilImageCheck(pDisplay[m_iDispNum]->MilBuf_Origin_Image, pDisplay[m_iDispNum]->MilBuf_Set_View, _T(""));
}

void CMilVisionDlg::PopupImageSave()
{
	pDisplay[m_iDispNum]->ImageSave(m_iDispNum);
}

void CMilVisionDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	CRect rect;
	GetWindowRect(&rect);

	if (pDisplay[m_iDispNum]->CheckInDisp(point))	//영역 안에 클릭했을 경우
	{
		CMenu	m_sub_menu;			// Stamp Operation Menu
		m_sub_menu.LoadMenu(IDR_MENU1);
		m_sub_menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x + rect.left, point.y + rect.top + 20, this, NULL);
	}

	CDialog::OnRButtonUp(nFlags, point);
}

void CMilVisionDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	CPoint CvtPoint;

	int iOvrNum = pDisplay[m_iDispNum]->WhichInDisp(point);

	switch (iOvrNum)
	{
	case DEF_OVR_SET:
		if (bLClick)
		{
			pntDrag[1] = pDisplay[m_iDispNum]->ConvertPnt_Click(point, iOvrNum);
			pntDrag[1] = pDisplay[m_iDispNum]->ConvertPnt(pntDrag[1], FALSE);//영상좌표로 저장

			if (abs(pntDrag[0].x - pntDrag[1].x) > 10
				|| abs(pntDrag[0].y - pntDrag[1].y) > 10)
			{
				bDragPt = TRUE;
			}
		}

		if (bDragPt)
		{

			if (pDisplay[m_iDispNum]->CheckInBox(pntDrag[1], rcMark))	//영역 안에 클릭했을 경우
			{
				rectDraw.left		= rcMark.left	+ (pntDrag[1].x - pntDrag[0].x);
				rectDraw.top		= rcMark.top	+ (pntDrag[1].y - pntDrag[0].y);
				rectDraw.right		= rcMark.right	+ (pntDrag[1].x - pntDrag[0].x);
				rectDraw.bottom		= rcMark.bottom	+ (pntDrag[1].y - pntDrag[0].y);

				pDisplay[m_iDispNum]->addBoxList(rectDraw, M_COLOR_MAGENTA, DEF_OVR_SET);
				rectDraw.left -= 1;
				rectDraw.top -= 1;
				rectDraw.right += 1;
				rectDraw.bottom += 1;
				pDisplay[m_iDispNum]->addBoxList(rectDraw, M_COLOR_MAGENTA, DEF_OVR_SET);
				rectDraw.left += 2;
				rectDraw.top += 2;
				rectDraw.right -= 2;
				rectDraw.bottom -= 2;
				pDisplay[m_iDispNum]->addBoxList(rectDraw, M_COLOR_MAGENTA, DEF_OVR_SET);
				rectDraw.left -= 1;
				rectDraw.top -= 1;
				rectDraw.right += 1;
				rectDraw.bottom += 1;

				CPoint cdStart, cdEnd;
				//세로줄
				cdStart.x = cdEnd.x = rectDraw.CenterPoint().x;
				cdStart.y = rectDraw.top;
				cdEnd.y = rectDraw.bottom;
				pDisplay[m_iDispNum]->addLineList(cdStart, cdEnd, M_COLOR_MAGENTA, DEF_OVR_SET);
				cdStart.x = cdEnd.x = rectDraw.CenterPoint().x - 1;
				pDisplay[m_iDispNum]->addLineList(cdStart, cdEnd, M_COLOR_MAGENTA, DEF_OVR_SET);
				cdStart.x = cdEnd.x = rectDraw.CenterPoint().x + 1;
				pDisplay[m_iDispNum]->addLineList(cdStart, cdEnd, M_COLOR_MAGENTA, DEF_OVR_SET);

				//가로줄
				cdStart.y = cdEnd.y = rectDraw.CenterPoint().y;
				cdStart.x = rectDraw.left;
				cdEnd.x = rectDraw.right;
				pDisplay[m_iDispNum]->addLineList(cdStart, cdEnd, M_COLOR_MAGENTA, DEF_OVR_SET);
				cdStart.y = cdEnd.y = rectDraw.CenterPoint().y - 1;
				pDisplay[m_iDispNum]->addLineList(cdStart, cdEnd, M_COLOR_MAGENTA, DEF_OVR_SET);
				cdStart.y = cdEnd.y = rectDraw.CenterPoint().y + 1;
				pDisplay[m_iDispNum]->addLineList(cdStart, cdEnd, M_COLOR_MAGENTA, DEF_OVR_SET);

				pDisplay[m_iDispNum]->drawOverlay(iOvrNum);
			}
		}
// 		else
// 		{
// 			CvtPoint = pDisplay[m_iDispNum]->ConvertPnt_Click(point);
// 			CvtPoint = pDisplay[m_iDispNum]->ConvertPnt(CvtPoint);
// 			if (pDisplay[m_iDispNum]->CheckInBox(CvtPoint, rcMark))	//영역 안에 클릭했을 경우
// 				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));
// 			else
// 				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
// 
// 
// // 			if (m_ctrlChkZoom.GetCheck())
// 			{
// 				CvtPoint = pDisplay[m_iDispNum]->ConvertPnt_Click(point);
// 				CvtPoint = pDisplay[m_iDispNum]->ConvertPnt(CvtPoint);
// 				CvtPoint.x = CvtPoint.x - (LONG)(DEF_MARK_SIZE_X / DEF_ZOOM);
// 				CvtPoint.y = CvtPoint.y - (LONG)(DEF_MARK_SIZE_X / DEF_ZOOM);
// 
// 				MbufCopyColor2d(pDisplay[m_iDispNum]->MilBuf_Origin_Image, pDisplay[m_iDispNum]->MilBuf_Mark_View
// 					, M_ALL_BANDS, CvtPoint.x, CvtPoint.y,
// 					M_ALL_BANDS, 0, 0, DEF_ZOOM_AREA_SIZE_X, DEF_ZOOM_AREA_SIZE_Y);
// 				MbufCopy(pDisplay[m_iDispNum]->MilBuf_PartZoom_View, pDisplay[DEF_DISP_TOP_SET]->MilBuf_PartZoom_View);
// 
// 				CPoint nsl, nel;
// 				nsl.x = nel.x = (LONG)(DEF_ZOOM_AREA_SIZE_X / 2.0);
// 				nsl.y = 0;
// 				nel.y = DEF_ZOOM_AREA_SIZE_Y;
// 				pDisplay[m_iDispNum]->addLineList(nsl, nel, LIGHTMAGENTA, DEF_OVR_ZOOM);
// 				nsl.y = nel.y = (LONG)(DEF_ZOOM_AREA_SIZE_Y / 2.0);
// 				nsl.x = 0;
// 				nel.x = DEF_ZOOM_AREA_SIZE_X;
// 				pDisplay[m_iDispNum]->addLineList(nsl, nel, LIGHTMAGENTA, DEF_OVR_ZOOM);
// 				// 		pDisplay[m_iDispNum]->addCrossList(DEF_ZOOM_AREA_SIZE_X / 2, DEF_ZOOM_AREA_SIZE_Y / 2, 10, LIGHTMAGENTA, DEF_OVR_ZOOM);
// 				pDisplay[DEF_DISP_TOP_SET]->OverlayClear(DEF_OVR_ZOOM);
// 				pDisplay[m_iDispNum]->drawOverlay(DEF_OVR_ZOOM);
// 			}
// 		}
		break;
	case DEF_OVR_MARK:
		if (bLClick)
		{
			int iPenSize = pModelData->iModDntSize;
			CRect rcDrag_DntCareMask;
			CvtPoint = pDisplay[m_iDispNum]->ConvertPnt_Click(point, iOvrNum);
			CvtPoint = pDisplay[m_iDispNum]->ConvertPnt(CvtPoint, FALSE);//영상좌표로 저장
			rcDrag_DntCareMask.left = CvtPoint.x - (iPenSize >> 1);// -iPenSize;
			rcDrag_DntCareMask.right = CvtPoint.x + (iPenSize >> 1);
			rcDrag_DntCareMask.top = CvtPoint.y - (iPenSize >> 1);// -iPenSize;
			rcDrag_DntCareMask.bottom = CvtPoint.y + (iPenSize >> 1);
			pDisplay[m_iDispNum]->addBoxList(rcDrag_DntCareMask, LIGHTMAGENTA, iOvrNum);
			MgraColor(M_DEFAULT, WHITE);
			MgraRectFill(M_DEFAULT, pInsp[m_iDispNum]->MilBuf_Mask, rcDrag_DntCareMask.left, rcDrag_DntCareMask.top, rcDrag_DntCareMask.right, rcDrag_DntCareMask.bottom);
			MgraRectFill(M_DEFAULT, pDisplay[m_iDispNum]->MilOvr[iOvrNum].MilOverlayImage, rcDrag_DntCareMask.left, rcDrag_DntCareMask.top, rcDrag_DntCareMask.right, rcDrag_DntCareMask.bottom);
// 			pDisplay[m_iDispNum]->OverlayClear(iOvrNum);
// 			pDisplay[m_iDispNum]->drawOverlay(iOvrNum);

			bDragPt = TRUE;
		}
		break;
	default:
		break;
	}



	CDialog::OnMouseMove(nFlags, point);
}

void CMilVisionDlg::OnBnClickedCheckDispCenter()
{
// 	rcMark.left = (DEF_IMAGE_X - DEF_MARK_SIZE_X)>>1;
// 	rcMark.top = (DEF_IMAGE_Y - DEF_MARK_SIZE_Y)>>1;
// 	rcMark.right = (DEF_IMAGE_X + DEF_MARK_SIZE_X)>>1;
// 	rcMark.bottom = (DEF_IMAGE_Y + DEF_MARK_SIZE_Y)>>1;

	CPoint cdStart, cdEnd;

	if (m_ctrlChkDispCenter.GetCheck())
	{
		//세로줄
		cdStart.x = cdEnd.x = (LONG)DEF_IMAGE_X>>1;
		cdStart.y = 0;
		cdEnd.y = (LONG)DEF_IMAGE_Y;
		pDisplay[m_iDispNum]->addLineList(cdStart, cdEnd, M_COLOR_MAGENTA, DEF_OVR_SET);
		cdStart.x = cdEnd.x = (LONG)(DEF_IMAGE_X>>1)-1;
		pDisplay[m_iDispNum]->addLineList(cdStart, cdEnd, M_COLOR_MAGENTA, DEF_OVR_SET);
		cdStart.x = cdEnd.x = (LONG)(DEF_IMAGE_X>>1)+1;
		pDisplay[m_iDispNum]->addLineList(cdStart, cdEnd, M_COLOR_MAGENTA, DEF_OVR_SET);

		//가로줄
		cdStart.y = cdEnd.y = (LONG)DEF_IMAGE_Y>>1;
		cdStart.x = 0;
		cdEnd.x = (LONG)DEF_IMAGE_X;
		pDisplay[m_iDispNum]->addLineList(cdStart, cdEnd, M_COLOR_MAGENTA, DEF_OVR_SET);
		cdStart.y = cdEnd.y = (LONG)(DEF_IMAGE_Y>>1)-1;
		pDisplay[m_iDispNum]->addLineList(cdStart, cdEnd, M_COLOR_MAGENTA, DEF_OVR_SET);
		cdStart.y = cdEnd.y = (LONG)(DEF_IMAGE_Y>>1)+1;
		pDisplay[m_iDispNum]->addLineList(cdStart, cdEnd, M_COLOR_MAGENTA, DEF_OVR_SET);

		//그리기
		pDisplay[m_iDispNum]->drawOverlay(DEF_OVR_SET);
	}
	else
		pDisplay[m_iDispNum]->OverlayClear(DEF_OVR_SET);
}

void CMilVisionDlg::OnCbnSelchangeComboPatt()
{
	m_iComboPattNo = m_ctrlComboPattNo.GetCurSel();

	pInsp[m_iDispNum]->ModLoad(m_iComboPattNo);

	MbufClear(pDisplay[m_iDispNum]->MilBuf_Mark_View, 0);
	pDisplay[m_iDispNum]->OverlayClear(DEF_OVR_MARK);
	MmodDraw(pInsp[m_iDispNum]->Milmodgra, pInsp[m_iDispNum]->MilImageMod[m_iComboPattNo], pDisplay[m_iDispNum]->MilOvr[DEF_OVR_MARK].MilOverlayImage, M_DRAW_IMAGE + M_DRAW_EDGES + M_DRAW_POSITION, M_DEFAULT, M_DEFAULT);
}

void CMilVisionDlg::SetListParaData()
{
	CRect tmpRect;
	m_ctrlListPara.GetWindowRect(tmpRect);

	// 리스트 초기화 
	m_ctrlListPara.DeleteAllItems();
	m_ctrlListPara.DeleteColumn(1);
	m_ctrlListPara.DeleteColumn(0);

	// 리스트 스타일 설정
	m_ctrlListPara.SetExtendedStyle(LVS_EX_GRIDLINES);

	// 타이틀 삽입
	m_ctrlListPara.InsertColumn(0, _T("파라미터 이름"), LVCFMT_LEFT, (int)(0.6*tmpRect.Width()), -1);
	m_ctrlListPara.InsertColumn(1, _T("설정값"), LVCFMT_RIGHT, (int)(0.38*tmpRect.Width()), -1);

	stcListItem item[Max_Mil_Para_Cnt];


	int i=0;
	for (i=0; i<Max_Mil_Para_Cnt; i++)
	{
		switch(i)
		{
		case Para_iModNumber:
			sprintf_s(item[i].name, sizeof(item[i].name), _T("검출 패턴수"));
			item[i].iVal = pModelData->iModNumber;	
			item[i].ValType = 0;
			break;
		case Para_iModSmoothness:
			sprintf_s(item[i].name, sizeof(item[i].name), _T("Smoothness"));
			item[i].iVal = pModelData->iModSmoothness;	
			item[i].ValType = 0;
			break;
		case Para_iModAcceptance:
			sprintf_s(item[i].name, sizeof(item[i].name), _T("매칭율"));
			item[i].iVal = pModelData->iModAcceptance;	
			item[i].ValType = 0;
			break;
		case Para_dModScaleMax:
			sprintf_s(item[i].name, sizeof(item[i].name), _T("최대크기비"));
			item[i].dVal = pModelData->dModScaleMax;	
			item[i].ValType = 1;
			break;
		case Para_dModScaleMin:
			sprintf_s(item[i].name, sizeof(item[i].name), _T("최소크기비"));
			item[i].dVal = pModelData->dModScaleMin;	
			item[i].ValType = 1;
			break;
		case Para_dModAngleDeltaNeg:
			sprintf_s(item[i].name, sizeof(item[i].name), _T("회전허용치(-)"));
			item[i].dVal = pModelData->dModAngleDeltaNeg;	
			item[i].ValType = 1;
			break;
		case Para_dModAngleDeltaPos:
			sprintf_s(item[i].name, sizeof(item[i].name), _T("회전허용치(+)"));
			item[i].dVal = pModelData->dModAngleDeltaPos;	
			item[i].ValType = 1;
			break;
		case Para_dModReferenceAngle:
			sprintf_s(item[i].name, sizeof(item[i].name), _T("각도 보정"));
			item[i].dVal = pModelData->dModReferenceAngle;	
			item[i].ValType = 1;
			break;
		case Para_iModTimeout:
			sprintf_s(item[i].name, sizeof(item[i].name), _T("타임아웃"));
			item[i].iVal = pModelData->iModTimeout;
			item[i].ValType = 0;
			break;
		case Para_iModDntSize:
			sprintf_s(item[i].name, sizeof(item[i].name), _T("제외영역표시크기"));
			item[i].iVal = pModelData->iModDntSize;
			item[i].ValType = 0;
			break;
		}

		AddParaItem(i, 1, item[i]);
	}

	m_dEditImgResol = pModelData->dImgResol;
	UpdateData(FALSE);

}

void CMilVisionDlg::GetListParaData()
{
	UpdateData(TRUE);
	pModelData->dImgResol = m_dEditImgResol;

	stcListItem item[Max_Mil_Para_Cnt];

	int count = m_ctrlListPara.GetItemCount();

	int i;
	for(i = 0; i < count; i++)
	{
		item[i] = ReadParaItem(i, 1);
	}

	pModelData->iModSmoothness		= item[Para_iModNumber			].iVal;
	pModelData->iModSmoothness		= item[Para_iModSmoothness		].iVal;
	pModelData->iModAcceptance		= item[Para_iModAcceptance		].iVal;
	pModelData->dModScaleMax		= item[Para_dModScaleMax		].dVal;
	pModelData->dModScaleMin		= item[Para_dModScaleMin		].dVal;
	pModelData->dModAngleDeltaNeg	= item[Para_dModAngleDeltaNeg	].dVal;
	pModelData->dModAngleDeltaPos	= item[Para_dModAngleDeltaPos	].dVal;
	pModelData->dModReferenceAngle	= item[Para_dModReferenceAngle	].dVal;
	pModelData->iModTimeout			= item[Para_iModTimeout			].iVal;
	pModelData->iModDntSize			= item[Para_iModDntSize			].iVal;
}

void CMilVisionDlg::AddParaItem(int raw, int cal, stcListItem ParaData)
{
	m_ctrlListPara.InsertItem(raw, ParaData.name);

	CString StrVal;
	if (ParaData.ValType == 0)//int
	{
		StrVal.Format(_T("%d"), ParaData.iVal);
	} 
	else
	{
		StrVal.Format(_T("%.2f"), ParaData.dVal);
	}

	m_ctrlListPara.SetItem(raw, cal, LVIF_TEXT, StrVal, 0, 0, 0, NULL );
}

CMilVisionDlg::stcListItem CMilVisionDlg::ReadParaItem( int raw, int cal )
{
	stcListItem	tmpItem;

	CString StrTemp;

	StrTemp = m_ctrlListPara.GetItemText(raw, cal);
	tmpItem.iVal = atoi(StrTemp);
	tmpItem.dVal = atof(StrTemp);

	return tmpItem;
}


void CMilVisionDlg::OnBnClickedButtonSave()
{
	GetListParaData();
	pModelData->iModNumber = 1;//무조건 1개 강제 지정
	pModelData->Save();
}

void CMilVisionDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if(nSBCode==SB_THUMBTRACK || nSBCode==SB_ENDSCROLL)  return;

	stcScroll scrollItem;

	scrollItem.ValType = -1; //0:int / 1: double
	scrollItem.iMin = 0;
	scrollItem.iMax = 0;
	scrollItem.iLine = 0;
	scrollItem.iPage = 0;
	scrollItem.iPos = 0;
	scrollItem.dMin = 0.0;
	scrollItem.dMax = 0.0;
	scrollItem.dLine = 0.0;
	scrollItem.dPage = 0.0;
	scrollItem.dPos = 0.0;


	int count = m_ctrlListPara.GetItemCount();
	int index = -1;

	int i;
	for(i = 0; i < count; i++)
	{
		// 개별 항목의 현재 상태가 선택되어진 상태일 경우
		if(LVIS_SELECTED == m_ctrlListPara.GetItemState(i, LVIS_SELECTED))
		{
			switch(i)
			{
			case Para_iModNumber:
				scrollItem.ValType = 0;
				scrollItem.iMin = 1;
				scrollItem.iMax = 1;
				scrollItem.iLine = 0;
				scrollItem.iPage = 0;
				scrollItem.iPos	= atoi(m_ctrlListPara.GetItemText(i,1));
				break;
			case Para_iModSmoothness:
				scrollItem.ValType = 0;
				scrollItem.iMin = 0;
				scrollItem.iMax = 100;
				scrollItem.iLine = 2;
				scrollItem.iPage = 10;
				scrollItem.iPos	= atoi(m_ctrlListPara.GetItemText(i,1));
				break;
			case Para_iModAcceptance:
				scrollItem.ValType = 0;
				scrollItem.iMin = 0;
				scrollItem.iMax = 100;
				scrollItem.iLine = 1;
				scrollItem.iPage = 10;
				scrollItem.iPos	= atoi(m_ctrlListPara.GetItemText(i,1));
				break;
			case Para_dModScaleMax:
				scrollItem.ValType = 1;
				scrollItem.dMin = 0.5;
				scrollItem.dMax = 2.0;
				scrollItem.dLine = 0.1;
				scrollItem.dPage = 0.2;
				scrollItem.dPos	= atof(m_ctrlListPara.GetItemText(i,1));
				break;
			case Para_dModScaleMin:
				scrollItem.ValType = 1;
				scrollItem.dMin = 0.5;
				scrollItem.dMax = 2.0;
				scrollItem.dLine = 0.1;
				scrollItem.dPage = 0.2;
				scrollItem.dPos	= atof(m_ctrlListPara.GetItemText(i,1));
				break;
			case Para_dModAngleDeltaNeg:
				scrollItem.ValType = 1;
				scrollItem.dMin = 0.0;
				scrollItem.dMax = 180.0;
				scrollItem.dLine = 1.0;
				scrollItem.dPage = 5.0;
				scrollItem.dPos	= atof(m_ctrlListPara.GetItemText(i,1));
				break;
			case Para_dModAngleDeltaPos:
				scrollItem.ValType = 1;
				scrollItem.dMin = 0.0;
				scrollItem.dMax = 180.0;
				scrollItem.dLine = 1.0;
				scrollItem.dPage = 5.0;
				scrollItem.dPos	= atof(m_ctrlListPara.GetItemText(i,1));
				break;
			case Para_dModReferenceAngle:
				scrollItem.ValType = 1;
				scrollItem.dMin = -90.0;
				scrollItem.dMax = 90.0;
				scrollItem.dLine = 0.1;
				scrollItem.dPage = 1.0;
				scrollItem.dPos	= atof(m_ctrlListPara.GetItemText(i,1));
				break;
			case Para_iModTimeout:
				scrollItem.ValType = 0;
				scrollItem.iMin = 1;
				scrollItem.iMax = 10000;
				scrollItem.iLine = 100;
				scrollItem.iPage = 500;
				scrollItem.iPos = atoi(m_ctrlListPara.GetItemText(i, 1));
				break;
			case Para_iModDntSize:
				scrollItem.ValType = 0;
				scrollItem.iMin = 5;
				scrollItem.iMax = 500;
				scrollItem.iLine = 5;
				scrollItem.iPage = 10;
				scrollItem.iPos = atoi(m_ctrlListPara.GetItemText(i, 1));
				break;
			}

			index = i;
			break;
		}
		else
		{
			continue;
		}
	}

	if(index == -1)
		return;


	CString StrVal;
	CDoubleScrollBar* pDoubleScrollBar;
	if (scrollItem.ValType)
	{//double형 스크롤바
		pDoubleScrollBar = (CDoubleScrollBar*)pScrollBar;

		pDoubleScrollBar->SetScrollRange(scrollItem.dMin, scrollItem.dMax);

		if(nSBCode==SB_LINELEFT)    scrollItem.dPos -= scrollItem.dLine;
		else if(nSBCode==SB_LINERIGHT)   scrollItem.dPos += scrollItem.dLine;
		else if(nSBCode==SB_PAGELEFT)   scrollItem.dPos -= scrollItem.dPage;
		else if(nSBCode==SB_PAGERIGHT)   scrollItem.dPos += scrollItem.dPage;
		else if(nSBCode == SB_THUMBPOSITION)
		{
			scrollItem.dPos = pDoubleScrollBar->GetTrackPos();
			pDoubleScrollBar->SetScrollPos(scrollItem.dPos);
		}

		scrollItem.dPos = scrollItem.dPos < scrollItem.dMin ? scrollItem.dMin : scrollItem.dPos;
		scrollItem.dPos = scrollItem.dPos > scrollItem.dMax ? scrollItem.dMax : scrollItem.dPos;

		pDoubleScrollBar->SetScrollPos(scrollItem.dPos);

		StrVal.Format(_T("%.2f"), scrollItem.dPos);
		m_ctrlListPara.SetItem(index, 1, LVIF_TEXT, StrVal, 0, 0, 0, NULL );
	} 
	else
	{//int형 스크롤바
		pScrollBar->SetScrollRange(scrollItem.iMin, scrollItem.iMax);

		if(nSBCode==SB_LINELEFT)    scrollItem.iPos -= scrollItem.iLine;
		else if(nSBCode==SB_LINERIGHT)   scrollItem.iPos += scrollItem.iLine;
		else if(nSBCode==SB_PAGELEFT)   scrollItem.iPos -= scrollItem.iPage;
		else if(nSBCode==SB_PAGERIGHT)   scrollItem.iPos += scrollItem.iPage;
		else if(nSBCode == SB_THUMBPOSITION) scrollItem.iPos = nPos;

		scrollItem.iPos = scrollItem.iPos < scrollItem.iMin ? scrollItem.iMin : scrollItem.iPos;
		scrollItem.iPos = scrollItem.iPos > scrollItem.iMax ? scrollItem.iMax : scrollItem.iPos;

		pScrollBar->SetScrollPos(scrollItem.iPos);

		StrVal.Format(_T("%d"), scrollItem.iPos);
		m_ctrlListPara.SetItem(index, 1, LVIF_TEXT, StrVal, 0, 0, 0, NULL );
	}


	GetListParaData();

	UpdateData(false);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CMilVisionDlg::OnNMDblclkListPara(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	stcScroll scrollItem;

	scrollItem.ValType = -1; //0:int / 1: double
	scrollItem.iMin = 0;
	scrollItem.iMax = 0;
	scrollItem.iLine = 0;
	scrollItem.iPage = 0;
	scrollItem.iPos = 0;
	scrollItem.dMin = 0.0;
	scrollItem.dMax = 0.0;
	scrollItem.dLine = 0.0;
	scrollItem.dPage = 0.0;
	scrollItem.dPos = 0.0;


	int count = m_ctrlListPara.GetItemCount();
	int index = 0;

	int i;
	for(i = 0; i < count; i++)
	{
		// 개별 항목의 현재 상태가 선택되어진 상태일 경우
		if(LVIS_SELECTED == m_ctrlListPara.GetItemState(i, LVIS_SELECTED))
		{
			switch(i)
			{
			case Para_iModNumber:
				scrollItem.ValType = 0;
				scrollItem.iMin = 1;
				scrollItem.iMax = 1;
				scrollItem.iLine = 0;
				scrollItem.iPage = 0;
				scrollItem.iPos	= atoi(m_ctrlListPara.GetItemText(i,1));
				break;
			case Para_iModSmoothness:
				scrollItem.ValType = 0;
				scrollItem.iMin = 0;
				scrollItem.iMax = 100;
				scrollItem.iLine = 2;
				scrollItem.iPage = 10;
				scrollItem.iPos	= atoi(m_ctrlListPara.GetItemText(i,1));
				break;
			case Para_iModAcceptance:
				scrollItem.ValType = 0;
				scrollItem.iMin = 0;
				scrollItem.iMax = 100;
				scrollItem.iLine = 1;
				scrollItem.iPage = 10;
				scrollItem.iPos	= atoi(m_ctrlListPara.GetItemText(i,1));
				break;
			case Para_dModScaleMax:
				scrollItem.ValType = 1;
				scrollItem.dMin = 0.5;
				scrollItem.dMax = 2.0;
				scrollItem.dLine = 0.1;
				scrollItem.dPage = 0.2;
				scrollItem.dPos	= atof(m_ctrlListPara.GetItemText(i,1));
				break;
			case Para_dModScaleMin:
				scrollItem.ValType = 1;
				scrollItem.dMin = 0.5;
				scrollItem.dMax = 2.0;
				scrollItem.dLine = 0.1;
				scrollItem.dPage = 0.2;
				scrollItem.dPos	= atof(m_ctrlListPara.GetItemText(i,1));
				break;
			case Para_dModAngleDeltaNeg:
				scrollItem.ValType = 1;
				scrollItem.dMin = 0.0;
				scrollItem.dMax = 180.0;
				scrollItem.dLine = 1.0;
				scrollItem.dPage = 5.0;
				scrollItem.dPos	= atof(m_ctrlListPara.GetItemText(i,1));
				break;
			case Para_dModAngleDeltaPos:
				scrollItem.ValType = 1;
				scrollItem.dMin = 0.0;
				scrollItem.dMax = 180.0;
				scrollItem.dLine = 1.0;
				scrollItem.dPage = 5.0;
				scrollItem.dPos	= atof(m_ctrlListPara.GetItemText(i,1));
				break;
			case Para_dModReferenceAngle:
				scrollItem.ValType = 1;
				scrollItem.dMin = -90.0;
				scrollItem.dMax = 90.0;
				scrollItem.dLine = 0.1;
				scrollItem.dPage = 1.0;
				scrollItem.dPos	= atof(m_ctrlListPara.GetItemText(i,1));
				break;
			case Para_iModTimeout:
				scrollItem.ValType = 0;
				scrollItem.iMin = 1;
				scrollItem.iMax = 10000;
				scrollItem.iLine = 100;
				scrollItem.iPage = 500;
				scrollItem.iPos = atoi(m_ctrlListPara.GetItemText(i, 1));
				break;
			case Para_iModDntSize:
				scrollItem.ValType = 0;
				scrollItem.iMin = 5;
				scrollItem.iMax = 500;
				scrollItem.iLine = 5;
				scrollItem.iPage = 10;
				scrollItem.iPos = atoi(m_ctrlListPara.GetItemText(i, 1));
				break;
			}

			index = i;
			break;
		}
	}

	CString StrVal;
	CDoubleScrollBar* pDoubleScrollBar;
	CScrollBar* pScrollBar;
	if (scrollItem.ValType)
	{//double형 스크롤바
		pDoubleScrollBar = (CDoubleScrollBar*)&m_ctrlScrollVal;

		pDoubleScrollBar->SetScrollRange(scrollItem.dMin, scrollItem.dMax);
		pDoubleScrollBar->SetScrollPos(scrollItem.dPos);
	} 
	else
	{//int형 스크롤바
		pScrollBar = (CScrollBar*)&m_ctrlScrollVal;
		pScrollBar->SetScrollRange(scrollItem.iMin, scrollItem.iMax);
		pScrollBar->SetScrollPos(scrollItem.iPos);
	}
	*pResult = 0;
}


void CMilVisionDlg::OnLButtonUp(UINT nFlags, CPoint point)
{

	if (pDisplay[m_iDispNum]->CheckInDisp(point, DEF_OVR_SET))
	{
		if(bLClick && bDragPt)
			rcMark = rectDraw;

		if (!m_ctrlChkDispCenter.GetCheck())
		{
			CPoint CurPoint;
			CurPoint = pDisplay[m_iDispNum]->ConvertPnt_Click(point, DEF_OVR_SET);
			CurPoint = pDisplay[m_iDispNum]->ConvertPnt(CurPoint, FALSE);

			int iLimitDisp = 5;

			rcMark.left		= CurPoint.x - (DEF_MARK_SIZE_X>>1);
			rcMark.top		= CurPoint.y - (DEF_MARK_SIZE_Y >>1);
			rcMark.right	= CurPoint.x + (DEF_MARK_SIZE_X>>1);
			rcMark.bottom	= CurPoint.y + (DEF_MARK_SIZE_Y >>1);

			if (rcMark.left	< iLimitDisp)
			{
				rcMark.left		= iLimitDisp;
				rcMark.right	= CurPoint.x + (CurPoint.x-iLimitDisp);
			}
			if (rcMark.right > DEF_IMAGE_X - iLimitDisp)
			{
				rcMark.right	= DEF_IMAGE_X - iLimitDisp;
				rcMark.left		= CurPoint.x - ((DEF_IMAGE_X - iLimitDisp)-CurPoint.x);
			}

			if (rcMark.top	< iLimitDisp)
			{
				rcMark.top		= iLimitDisp;
				rcMark.bottom	= CurPoint.y + (CurPoint.y-iLimitDisp);
			}
			if (rcMark.bottom > DEF_IMAGE_Y - iLimitDisp)
			{
				rcMark.bottom	= DEF_IMAGE_Y - iLimitDisp;
				rcMark.top		= CurPoint.y - ((DEF_IMAGE_Y - iLimitDisp)-CurPoint.y);
			}
		}
	}

	bLClick = FALSE;
	bDragPt = FALSE;

	
	CDialog::OnLButtonUp(nFlags, point);
}


void CMilVisionDlg::OnTimer(UINT_PTR nIDEvent)
{
	int i=0;

	switch(nIDEvent)
	{
	case TIMER_SET_PATT_MASTER://패턴등록
		if (pTask->iSetMasterPatt)
		{
			KillTimer(TIMER_SET_PATT_MASTER);

			for (i=0; i<pTask->NumPoint; i++)
			{
				//디스플레이원할때 코드//
				MbufPut(pDisplay[DEF_DISP_SET]->MilBuf_Origin_Image, pTask->ImageBuff[i]);
				MbufCopy(pDisplay[DEF_DISP_SET]->MilBuf_Origin_Image, pInsp[DEF_DISP_SET]->MilBuf_Insp_Image);
				MIL_DOUBLE sX1 = (MIL_DOUBLE)MbufInquire(pInsp[DEF_DISP_SET]->MilBuf_Insp_Image, M_SIZE_X, M_NULL);//2464
				MIL_DOUBLE sX2 = (MIL_DOUBLE)MbufInquire(pDisplay[m_iDispNum]->MilBuf_Set_View, M_SIZE_X, M_NULL);
				MIL_DOUBLE SizeRate = sX2/sX1;
				MimResize(pInsp[DEF_DISP_SET]->MilBuf_Insp_Image, pDisplay[m_iDispNum]->MilBuf_Set_View, SizeRate, SizeRate, M_DEFAULT);
				//디스플레이원할때 코드//

				SetMasterPattern(pTask->ImageBuff[i], i);
				MbufClear(pDisplay[m_iDispNum]->MilBuf_Mark_View, 0);
				pDisplay[m_iDispNum]->OverlayClear(DEF_OVR_MARK);
				MmodDraw(pInsp[m_iDispNum]->Milmodgra, pInsp[m_iDispNum]->MilImageMod[i], pDisplay[m_iDispNum]->MilOvr[DEF_OVR_MARK].MilOverlayImage, M_DRAW_IMAGE + M_DRAW_EDGES + M_DRAW_POSITION, M_DEFAULT, M_DEFAULT);
			}

			pTask->iSetMasterPatt = 0;
			SetTimer(TIMER_SET_PATT_MASTER,300,NULL);
		}
		break;

	case TIMER_SET_PATT_DETAIL:
		break;

	case TIMER_SET_MOTOR_ALIGN://좌표계 등록
		if (pTask->iSetMotorAlign)
		{
			KillTimer(TIMER_SET_MOTOR_ALIGN);
			pTask->ErrorCode = 0;

			SetMotorAlign(pTask->iGroupNum, pTask->XPixArray, pTask->YPixArray, pTask->XWorldArray, pTask->YWorldArray, pTask->ImageBuff, pTask->NumPoint);
			memcpy(pTask->msgVision, pInsp[DEF_DISP_SET]->msgVision, sizeof(pTask->msgVision));
			pTask->ErrorCode = pInsp[DEF_DISP_SET]->ErrorCode;

			pTask->iSetMotorAlign = 0;
			SetTimer(TIMER_SET_MOTOR_ALIGN, 300,NULL);
		}
		break;

	case TIMER_GET_PATT_CENTER://패턴중심이동
		if (pTask->iGetPattAlignCenter)
		{
			KillTimer(TIMER_GET_PATT_CENTER);
			pTask->ErrorCode = 0;

			//디스플레이원할때 코드//
			MbufPut(pDisplay[DEF_DISP_SET]->MilBuf_Origin_Image, pTask->ImageBuff[pTask->PatternNum]);
			MbufCopy(pDisplay[DEF_DISP_SET]->MilBuf_Origin_Image, pInsp[DEF_DISP_SET]->MilBuf_Insp_Image);
			MIL_DOUBLE sX1 = (MIL_DOUBLE)MbufInquire(pInsp[DEF_DISP_SET]->MilBuf_Insp_Image, M_SIZE_X, M_NULL);//2464
			MIL_DOUBLE sX2 = (MIL_DOUBLE)MbufInquire(pDisplay[m_iDispNum]->MilBuf_Set_View, M_SIZE_X, M_NULL);
			MIL_DOUBLE SizeRate = sX2/sX1;
			MimResize(pInsp[DEF_DISP_SET]->MilBuf_Insp_Image, pDisplay[m_iDispNum]->MilBuf_Set_View, SizeRate, SizeRate, M_DEFAULT);
			//디스플레이원할때 코드//

			pTask->AlignCenter = GetPattAlignCenter(pTask->ImageBuff[pTask->PatternNum], pTask->PatternNum);
			pTask->ErrorCode = pInsp[DEF_DISP_SET]->ErrorCode;

			pTask->iGetPattAlignCenter = 0;
			SetTimer(TIMER_GET_PATT_CENTER, 300,NULL);
		}
		break;

	case TIMER_GET_TRANS_COORDINATE://좌표 변환
		if (pTask->iGetTransCoordinate)
		{
			KillTimer(TIMER_GET_TRANS_COORDINATE);

			//디스플레이원할때 코드//
			MbufPut(pDisplay[DEF_DISP_SET]->MilBuf_Origin_Image, pTask->ImageBuff[pTask->PatternNum]);
			MbufCopy(pDisplay[DEF_DISP_SET]->MilBuf_Origin_Image, pInsp[DEF_DISP_SET]->MilBuf_Insp_Image);
			MIL_DOUBLE sX1 = (MIL_DOUBLE)MbufInquire(pInsp[DEF_DISP_SET]->MilBuf_Insp_Image, M_SIZE_X, M_NULL);//2464
			MIL_DOUBLE sX2 = (MIL_DOUBLE)MbufInquire(pDisplay[m_iDispNum]->MilBuf_Set_View, M_SIZE_X, M_NULL);
			MIL_DOUBLE SizeRate = sX2/sX1;
			MimResize(pInsp[DEF_DISP_SET]->MilBuf_Insp_Image, pDisplay[m_iDispNum]->MilBuf_Set_View, SizeRate, SizeRate, M_DEFAULT);
			//디스플레이원할때 코드//

			GetTransformCoordinate(pTask->iGroupNum, pTask->inX, pTask->inY, &pTask->OutX, &pTask->OutY, pTask->tansformType);

			pTask->iGetTransCoordinate = 0;
			SetTimer(TIMER_GET_TRANS_COORDINATE, 300,NULL);
		}
		break;
	}

	CDialog::OnTimer(nIDEvent);
}


void CMilVisionDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	SendMessage(WM_COMMAND, IDC_CHECK_DISP_CENTER, 0);

	this->SetWindowPos(NULL, 20, 100, 1160, 860, NULL);// SWP_NOSIZE);

	if (bShow)
	{
		MbufClear(pInsp[m_iDispNum]->MilBuf_Mask, 0x00);
		rcMark.left = (DEF_IMAGE_X - DEF_MARK_SIZE_X) >> 1;
		rcMark.top = (DEF_IMAGE_Y - DEF_MARK_SIZE_Y) >> 1;
		rcMark.right = (DEF_IMAGE_X + DEF_MARK_SIZE_X) >> 1;
		rcMark.bottom = (DEF_IMAGE_Y + DEF_MARK_SIZE_Y) >> 1;

// 		EnableDlgItem(theApp.m_fnGetViewPointer(), FALSE);//모든버튼 활성
	}
	else
	{
		int a = 0;
// 		EnableDlgItem(theApp.m_fnGetViewPointer(), TRUE);//모든버튼 활성
	}

}


void CMilVisionDlg::OnBnClickedButtonCal()
{
	int GroupNum=0;
	const int NumPoint = 6;
	double XPixArray[NumPoint];
	double YPixArray[NumPoint];
	double XWorArray[NumPoint];
	double YWorArray[NumPoint];

	XPixArray[0]=1;
	YPixArray[0]=1;
	XWorArray[0]=380.315;
	YWorArray[0]=32.436;

	XPixArray[1]=1;
	YPixArray[1]=6;
	XWorArray[1]=382.234642;
	YWorArray[1]=161.124;

	XPixArray[2]=9;
	YPixArray[2]=1;
	XWorArray[2]=560.243652;
	YWorArray[2]=31.246;

	XPixArray[3]=9;
	YPixArray[3]=6;
	XWorArray[3]=562.135;
	YWorArray[3]=160.135;

	XPixArray[4]=4;
	YPixArray[4]=1;
	XWorArray[4]=470;
	YWorArray[4]=30;

	XPixArray[5]=4;
	YPixArray[5]=6;
	XWorArray[5]=470;
	YWorArray[5]=160;


	pInsp[DEF_DISP_SET]->Calibration(GroupNum, XPixArray, YPixArray, XWorArray, YWorArray, NumPoint);

	putListData(pInsp[DEF_DISP_SET]->msgVision);
}


void CMilVisionDlg::OnBnClickedButtonPointCali()
{
	int GroupNum=0;
	const int tansformType = 0;
	double x=5;
	double y=1;
	double resX;
	double resY;

	for(x=1; x<=9; x+=1)
	{
		for(y=1; y<=6; y+=1)
		{
			pInsp[DEF_DISP_SET]->GetCamTransformCoordinate(GroupNum, x,y,&resX,&resY, tansformType);

			sprintf_s(pInsp[DEF_DISP_SET]->msgVision, _T("(%d,%d) = %.3f,%.3f"),(int)x,(int)y, resX, resY);
			putListData(pInsp[DEF_DISP_SET]->msgVision);
		}
	}



}


void CMilVisionDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	bLClick = TRUE;

	int iOvrNum = pDisplay[m_iDispNum]->WhichInDisp(point);

	switch (iOvrNum)
	{
	case DEF_OVR_SET:
		pntDrag[0] = pDisplay[m_iDispNum]->ConvertPnt_Click(point, iOvrNum);
		pntDrag[0] = pDisplay[m_iDispNum]->ConvertPnt(pntDrag[0], FALSE);//영상좌표로 저장
		break;
	case DEF_OVR_MARK:
		break;
	}


	CDialog::OnLButtonDown(nFlags, point);
}

BOOL CMilVisionDlg::FindWaferLine()
{
	memset(&pInsp[m_iDispNum]->sp, 0x00, sizeof(pInsp[m_iDispNum]->sp));
	pInsp[m_iDispNum]->BlobBlack(pInsp[m_iDispNum]->MilBuf_Insp_Image);

	if (pInsp[m_iDispNum]->sp.sizeX[0] > DEF_IMAGE_X*0.25
		|| pInsp[m_iDispNum]->sp.sizeY[0] > DEF_IMAGE_Y*0.25)
	{
		if(pInsp[m_iDispNum]->sp.pixelCnt[0] > pInsp[m_iDispNum]->sp.sizeX[0] * pInsp[m_iDispNum]->sp.sizeY[0] * 0.25)
			return TRUE;
	}

	return FALSE;
}

BOOL CMilVisionDlg::FindWaferEdge(double dPntEdge[3])
{
	if (!FindWaferLine())
		return FALSE;
	return pInsp[m_iDispNum]->SearchEdge(pInsp[m_iDispNum]->sp.center[0][0], pInsp[m_iDispNum]->sp.center[0][1], dPntEdge);
}

void CMilVisionDlg::OnBnClickedButtonFindEdgePnt()
{
	pInsp[m_iDispNum]->BlobBlack(pInsp[m_iDispNum]->MilBuf_Insp_Image);

	CRect rcBlob;
	rcBlob.left		= pInsp[m_iDispNum]->sp.minX[0];
	rcBlob.top		= pInsp[m_iDispNum]->sp.minY[0];
	rcBlob.right	= pInsp[m_iDispNum]->sp.maxX[0];
	rcBlob.bottom	= pInsp[m_iDispNum]->sp.maxY[0];
	pDisplay[m_iDispNum]->addBoxList(rcBlob, M_COLOR_RED, DEF_OVR_SET);
	rcBlob.left -= 1;
	rcBlob.top -= 1;
	rcBlob.right += 1;
	rcBlob.bottom += 1;
	pDisplay[m_iDispNum]->addBoxList(rcBlob, M_COLOR_RED, DEF_OVR_SET);
	rcBlob.left += 2;
	rcBlob.top += 2;
	rcBlob.right -= 2;
	rcBlob.bottom -= 2;
	pDisplay[m_iDispNum]->addBoxList(rcBlob, M_COLOR_RED, DEF_OVR_SET);

	CPoint pntBlob;
	pntBlob.x = pInsp[m_iDispNum]->sp.center[0][0];
	pntBlob.y = pInsp[m_iDispNum]->sp.center[0][1];
	pDisplay[m_iDispNum]->addCrossList(pntBlob, 50, M_COLOR_RED, DEF_OVR_SET);
	pntBlob.x += 1;
	pntBlob.y += 1;
	pDisplay[m_iDispNum]->addCrossList(pntBlob, 50, M_COLOR_RED, DEF_OVR_SET);
	pntBlob.x -= 2;
	pntBlob.y -= 2;
	pDisplay[m_iDispNum]->addCrossList(pntBlob, 50, M_COLOR_RED, DEF_OVR_SET);

	double dPntEdge[3];
	pInsp[m_iDispNum]->SearchEdge(pInsp[m_iDispNum]->sp.center[0][0], pInsp[m_iDispNum]->sp.center[0][1], dPntEdge);

	CPoint pntEdge;
	pntEdge.x = (LONG)dPntEdge[0];
	pntEdge.y = (LONG)dPntEdge[1];
	pDisplay[m_iDispNum]->addCrossList(pntEdge, 100, M_COLOR_BLUE, DEF_OVR_SET);
	pntEdge.x += 1;
	pntEdge.y += 1;
	pDisplay[m_iDispNum]->addCrossList(pntEdge, 100, M_COLOR_BLUE, DEF_OVR_SET);
	pntEdge.x -= 2;
	pntEdge.y -= 2;
	pDisplay[m_iDispNum]->addCrossList(pntEdge, 100, M_COLOR_BLUE, DEF_OVR_SET);

	pDisplay[m_iDispNum]->drawOverlay(DEF_OVR_SET);
}


// void CMilVisionDlg::OnBnClickedButtonCurImgAplly()
// {
// 	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
// }

void CMilVisionDlg::m_fnBringImage(void* InputImgBuff, int iImgBuffSize)
{
	CTimeInterval tiTimeOut;

	tiTimeOut.SetTimeStart();
	while (!pCamJAI->m_bGrabBufCpyConfirm)
	{
		Sleep(50);
		if(tiTimeOut.GetTimeInterval() > 1500)
			break;
	}

	pCamJAI->m_bLockedGrabBuf = TRUE;
	Sleep(100);

	if (iImgBuffSize != sizeof(unsigned char)*DEF_IMAGE_X*DEF_IMAGE_Y)
	{
		pCamJAI->m_bLockedGrabBuf = FALSE;
		return;
	}

	pTask->NumPoint = 1;
	pTask->PatternNum = 0;

// 	int iSizeTemp = sizeof(pTask->ImageBuff[pTask->PatternNum]);
// 	int iSizeTemp2 = sizeof(&InputImgBuff);
// 	unsigned char* tmpImgBuf = new unsigned char[DEF_IMAGE_X*DEF_IMAGE_Y];
// 	memset(tmpImgBuf, 0x00, sizeof(unsigned char)*DEF_IMAGE_X*DEF_IMAGE_Y);
// 	memcpy(tmpImgBuf, InputImgBuff, sizeof(InputImgBuff));
// 	delete[] tmpImgBuf;


	memset(pTask->ImageBuff[pTask->PatternNum], 0x00, sizeof(unsigned char)*DEF_IMAGE_X*DEF_IMAGE_Y);
	memcpy(pTask->ImageBuff[pTask->PatternNum], InputImgBuff, sizeof(unsigned char)*DEF_IMAGE_X*DEF_IMAGE_Y);
	pCamJAI->m_bLockedGrabBuf = FALSE;

	MbufPut(pDisplay[DEF_DISP_SET]->MilBuf_Origin_Image, pTask->ImageBuff[pTask->PatternNum]);
	MbufCopy(pDisplay[DEF_DISP_SET]->MilBuf_Origin_Image, pInsp[DEF_DISP_SET]->MilBuf_Insp_Image);
	MIL_DOUBLE sX1 = (MIL_DOUBLE)MbufInquire(pInsp[DEF_DISP_SET]->MilBuf_Insp_Image, M_SIZE_X, M_NULL);
	MIL_DOUBLE sX2 = (MIL_DOUBLE)MbufInquire(pDisplay[DEF_DISP_SET]->MilBuf_Set_View, M_SIZE_X, M_NULL);
	MIL_DOUBLE SizeRate = sX2 / sX1;
	MimResize(pInsp[DEF_DISP_SET]->MilBuf_Insp_Image, pDisplay[DEF_DISP_SET]->MilBuf_Set_View, SizeRate, SizeRate, M_DEFAULT);
}


void CMilVisionDlg::OnBnClickedButtonPatternLoad()
{
	pInsp[m_iDispNum]->ModLoad(m_iComboPattNo);
	MmodDraw(pInsp[m_iDispNum]->Milmodgra, pInsp[m_iDispNum]->MilImageMod[m_iComboPattNo], pDisplay[m_iDispNum]->MilOvr[DEF_OVR_MARK].MilOverlayImage, M_DRAW_IMAGE + M_DRAW_EDGES + M_DRAW_POSITION, M_DEFAULT, M_DEFAULT);

	GetDlgItem(IDOK)->EnableWindow(TRUE);
// 	m_bRegModFlag = TRUE;
	pWorkRun->m_bSqncInterrupt[SQITRT_REG_PATTERN] = TRUE;
}


void CMilVisionDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialog::OnOK();
}

void CMilVisionDlg::m_fnTranferCamBuff2PattBuff(BOOL bAutoPattAlignMode /*= FALSE*/)
{
#if JAI_CAMERA
	// 	if(theApp.m_fnGetViewPointer()->m_pAuto->m_bPattAlignMode)
// 	if (bAutoPattAlignMode)
// 		pMILAlignDlg->GetDlgItem(IDOK)->EnableWindow(FALSE);
	pMILAlignDlg->ShowWindow(SW_SHOW);
#endif
}
#endif


void CMilVisionDlg::OnBnClickedButtonDegreeCal()
{
	CString strName;

	pMILAlignDlg->m_fnBringImage(pCamJAI->m_CnvImageInfo.pImageBuffer, pCamJAI->m_CnvImageInfo.iImageSize);
	pTask->AlignCenter = GetPattAlignCenter(pTask->ImageBuff[pTask->PatternNum], pTask->PatternNum);
	if (pInsp[DEF_DISP_SET]->nCount != 1)
	{
		TECmsgOut(L"패턴을 재등록 하고 다시 시작하세요.", LIGHTMAGENTA, 1);

		m_iDegCalStep = 0;

		strName.Format("각도보정");
		GetDlgItem(IDC_BUTTON_DEGREE_CAL)->GetWindowText(strName);

		UpdateData(FALSE);
		return;
	}

	strName.Format("각도보정 Step(%d/2)", m_iDegCalStep+1);
	GetDlgItem(IDC_BUTTON_DEGREE_CAL)->GetWindowText(strName);

	pAxis[X_AXIS]->StatusGetPos(&m_dDegCalPos[m_iDegCalStep].x);
	pAxis[Y_AXIS]->StatusGetPos(&m_dDegCalPos[m_iDegCalStep].y);
	m_dDegCalPos[m_iDegCalStep].x += pTask->AlignCenter.x;
	m_dDegCalPos[m_iDegCalStep].y += pTask->AlignCenter.y;

	m_iDegCalStep++;

	if (m_iDegCalStep >= 2)
	{
		if (fabs(m_dDegCalPos[1].y - m_dDegCalPos[0].y) < 0.01 || fabs(m_dDegCalPos[1].x - m_dDegCalPos[0].x) < 0.01)
		{
			TECmsgOut(L"다른 위치의 패턴을 찍어주세요.", LIGHTMAGENTA, 1);

			m_iDegCalStep = 1;
			return;
		}

		pModelData->dModReferenceAngle = 
			atan((m_dDegCalPos[1].y - m_dDegCalPos[0].y) / (m_dDegCalPos[1].x - m_dDegCalPos[0].x)) * -180.0 / PI;
		if (fabs(pModelData->dModReferenceAngle) > 45.0)
		{
			if (pModelData->dModReferenceAngle > 0.0)
				pModelData->dModReferenceAngle = pModelData->dModReferenceAngle - 90.0;
			else
				pModelData->dModReferenceAngle = pModelData->dModReferenceAngle + 90.0;
		}

		SetListParaData();

		m_iDegCalStep = 0;

		strName.Format("각도보정");
		GetDlgItem(IDC_BUTTON_DEGREE_CAL)->GetWindowText(strName);

		TECmsgOut(L"각도 보정이 완료되었습니다. 저장하고 패턴등록을 다시 진행하세요", LIGHTBLUE, 1);
	}

	UpdateData(FALSE);
	return;
}


void CMilVisionDlg::OnBnClickedButtonReflashImg()
{
	pMILAlignDlg->m_fnBringImage(pCamJAI->m_CnvImageInfo.pImageBuffer, pCamJAI->m_CnvImageInfo.iImageSize);
}
