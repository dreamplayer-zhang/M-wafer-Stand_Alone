// USerInputDLG.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EllipsoMeter.h"
#include "USerInputDLG.h"
#include "afxdialogex.h"


// CUSerInputDLG 대화 상자입니다.

IMPLEMENT_DYNAMIC(CUSerInputDLG, CDialogEx)

CUSerInputDLG::CUSerInputDLG(CWnd* pParent /*=NULL*/)
	: CDialogEx(CUSerInputDLG::IDD, pParent)
{
	this->Create( IDD_USER_INPUT_DLG );
	m_bShow = FALSE;
}

CUSerInputDLG::~CUSerInputDLG()
{
}

void CUSerInputDLG::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDT_DATE, m_ctrlEditDate);
	DDX_Control(pDX, IDC_EDT_TIME, m_ctrlEditTime);
	DDX_Control(pDX, IDC_FPSPREAD1, m_ctrlUserInfo);
}


BEGIN_MESSAGE_MAP(CUSerInputDLG, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_HIDE, &CUSerInputDLG::OnBnClickedBtnHide)
	ON_EN_CHANGE(IDC_EDT_TIME, &CUSerInputDLG::OnEnChangeEdtTime)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------------------------
// 외부 참조용 Function
//---------------------------------------------------------------------------------------------
BOOL CUSerInputDLG::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (( nID   == 2 ) &&
		( nCode == -1 ))
	{
		m_fnSetShowFlag( FALSE );
	}
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CUSerInputDLG::m_fnSetShowFlag( BOOL bShow )
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

BOOL CUSerInputDLG::PreTranslateMessage(MSG* pMsg) 
{
	// ENTER KEY or ESC KEY CANCEL
	if (( pMsg->wParam == 13 ) || 
		( pMsg->wParam == 27 ))
	{
		return FALSE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

//---------------------------------------------------------------------------------------------------------
// 시작에 필요한 작업을 한다.
//---------------------------------------------------------------------------------------------------------
void CUSerInputDLG::m_fnSetInit( ) 
{
	CString s("");
	s.Format( "사용자 입력 메인창" );
	this->SetWindowText(s);

	m_pTaskHandle = new CTaskHandle( this, SHARED_NAME, 
											1, 
											sizeof( SHEET_RESISTANCE_Format ) + 1,
											0,
											FALSE );

	m_pTask					= ( SHEET_RESISTANCE_Format* )m_pTaskHandle->m_pFilePointer( );

}

//---------------------------------------------------------------------------------------------------------
// 종료에 필요한 작업을 한다.
//---------------------------------------------------------------------------------------------------------
void CUSerInputDLG::m_fnSetEnd() 
{
	if ( m_pTaskHandle != NULL )
	{
		m_pTask->stElipsoIO.bConnect = FALSE;
		m_pTaskHandle->m_fnSetEnd();
		delete m_pTaskHandle;
		m_pTaskHandle = NULL;
	}
}

void CUSerInputDLG::m_fnSetInputData( t_USER_INFO stUser ) 
{
	CString strData("");

	m_ctrlUserInfo.SetRow(1);
	//LOT ID
	m_ctrlUserInfo.SetCol(2);
	strData.Format( "%s", stUser.szLotID );
	m_ctrlUserInfo.SetText(strData);


	m_ctrlUserInfo.SetRow(2);
	//WaferSize
	m_ctrlUserInfo.SetCol(2);
	strData.Format( "%7.3f", stUser.dSampleSize );
	m_ctrlUserInfo.SetText(strData);

	//Thickness
	m_ctrlUserInfo.SetCol(4);
	strData.Format( "%7.3f", stUser.dThickness );
	m_ctrlUserInfo.SetText(strData);

	//Exclusion
	m_ctrlUserInfo.SetCol(6);
	strData.Format("%7.3f", stUser.dExclusion);
	m_ctrlUserInfo.SetText(strData);


	m_ctrlUserInfo.SetRow(3);
	//측정Recipe
	m_ctrlUserInfo.SetCol(2);
	strData.Format("%s", stUser.szRecipeFileName);
	m_ctrlUserInfo.SetText(strData);


	m_ctrlUserInfo.SetRow(4);
	//자동/수동 투입
	m_ctrlUserInfo.SetCol(2);
	strData.Format("%s", stUser.szInsertMode);
	m_ctrlUserInfo.SetText(strData);

	//웨이퍼얼라인
	m_ctrlUserInfo.SetCol(4);
	strData.Format("%s", stUser.szWaferAlignMode);
	m_ctrlUserInfo.SetText(strData);

	//Vision AF
	m_ctrlUserInfo.SetCol(6);
	strData.Format("%s", stUser.szVisionAFMode);
	m_ctrlUserInfo.SetText(strData);


	m_ctrlUserInfo.SetRow(5);
	//검사일자
	m_ctrlUserInfo.SetCol(2);
	strData.Format("%s", stUser.szDate);
	m_ctrlUserInfo.SetText(strData);

	//Slot No.
	m_ctrlUserInfo.SetCol(4);
	strData.Format("%d", stUser.iSlotNo);
	m_ctrlUserInfo.SetText(strData);

	//Op. ID
	m_ctrlUserInfo.SetCol(6);
	strData.Format("%s", stUser.szOperatorID);
	m_ctrlUserInfo.SetText(strData);


	m_ctrlUserInfo.SetRow(6);
	//검사시작시각
	m_ctrlUserInfo.SetCol(2);
	strData.Format("%s", stUser.szTimeStart);
	m_ctrlUserInfo.SetText(strData);

	//검사종료시각
	m_ctrlUserInfo.SetCol(4);
	strData.Format("%s", stUser.szTimeEnd);
	m_ctrlUserInfo.SetText(strData);

	//검사시간
	m_ctrlUserInfo.SetCol(6);
	strData.Format("%s", stUser.szTimeSpend);
	m_ctrlUserInfo.SetText(strData);


	m_ctrlUserInfo.SetRow(7);
	//파일경로
	m_ctrlUserInfo.SetCol(2);
	strData.Format("%s", stUser.szDataFile);
	m_ctrlUserInfo.SetText(strData);


	m_ctrlUserInfo.SetRow(8);
	//Description
	m_ctrlUserInfo.SetCol(2);
	strData.Format("%s", stUser.szDescription);
	m_ctrlUserInfo.SetText(strData);

// 	GetDlgItem(IDC_EDT_LOTID)->SetWindowText(strData);
// 	GetDlgItem(IDC_EDT_SIZE)->SetWindowText(strData);
// 	GetDlgItem(IDC_EDT_THICKNESS)->SetWindowText(strData);
// 	GetDlgItem(IDC_EDT_EXCLUSION)->SetWindowText(strData);
// 	GetDlgItem(IDC_EDT_OPID)->SetWindowText(strData);
// 	GetDlgItem(IDC_EDT_DESCRIPTION)->SetWindowText(strData);
// 
// 	strData.Format("%s", stUser.szDate);
// 	GetDlgItem(IDC_EDT_DATE)->SetWindowText(strData);
// 
// 	strData.Format("%s", stUser.szTime);
// 	GetDlgItem(IDC_EDT_TIME)->SetWindowText(strData);
// 
// 	strData.Format( "%s", stUser.szMatResistivity );
// 	GetDlgItem( IDC_EDT_MAT_RESISTIVITY )->SetWindowText( strData );
// 
// 	strData.Format( "%7.3f", stUser.dProbeSpace );
// 	GetDlgItem( IDC_EDT_PROBE_SPACE )->SetWindowText( strData );
// 
// 	strData.Format( "%7.3f", stUser.dTMeasure );
// 	GetDlgItem( IDC_EDT_TMEASURE )->SetWindowText( strData );
// 
// 	strData.Format( "%s", stUser.szMMode );
// 	GetDlgItem( IDC_EDT_MMODE )->SetWindowText( strData );
// 
// 
// 	strData.Format( "%s", stUser.szAnalysis );
// 	GetDlgItem( IDC_EDT_ANALYSIS )->SetWindowText( strData );
// 
// 
// 	strData.Format( "%7.3f", stUser.dFlat );
// 	GetDlgItem( IDC_EDT_FLAT )->SetWindowText( strData );
// 
// 	strData.Format( "%s", stUser.szMaterial );
// 	GetDlgItem( IDC_EDT_METERIAL )->SetWindowText( strData );
// 
// 	strData.Format( "%7.3f", stUser.dCorrectionF );
// 	GetDlgItem( IDC_EDT_CORRECTIONF )->SetWindowText( strData );
// 
// 	strData.Format( "%7.3f", stUser.dTCoefficient );
// 	GetDlgItem( IDC_EDT_TCOEFFICENT )->SetWindowText( strData );
// 
// 	strData.Format( "%7.3f", stUser.dTReference );
// 	GetDlgItem( IDC_EDT_TREFERENCE )->SetWindowText( strData );
// 
// 	strData.Format( "%s", stUser.szDataFile );
// 	GetDlgItem( IDC_EDT_DATA_FILE )->SetWindowText( strData );
// 
// 	strData.Format( "%7.3f", stUser.dSigmaMax );
// 	GetDlgItem( IDC_EDT_SIGMA_MAX )->SetWindowText( strData );
// 
// 
// 	strData.Format( "%s", stUser.szThinfilmmaterial );
// 	GetDlgItem( IDC_EDT_THIN_METERIAL )->SetWindowText( strData );
// 
// 	strData.Format( "%7.3f", stUser.dThinfilmthickness );
// 	GetDlgItem( IDC_EDT_THIN_THICKNESS )->SetWindowText( strData );
// 
// 	strData.Format( "%7.3f", stUser.dSigmaMin );
// 	GetDlgItem( IDC_EDT_SIGMA_MIN )->SetWindowText( strData );


}

void CUSerInputDLG::m_fnGetInputData( t_USER_INFO &stUser ) 
{
	CString strData("");

	m_ctrlUserInfo.SetRow(1);
	//LOT ID
	m_ctrlUserInfo.SetCol(2);
	strData = m_ctrlUserInfo.GetText();
	sprintf_s(stUser.szLotID, "%s", strData.GetString());


	m_ctrlUserInfo.SetRow(2);
	//WaferSize
	m_ctrlUserInfo.SetCol(2);
	strData = m_ctrlUserInfo.GetText();
	stUser.dSampleSize = atof(strData);

	//Thickness
	m_ctrlUserInfo.SetCol(4);
	strData = m_ctrlUserInfo.GetText();
	stUser.dThickness = atof(strData);

	//Exclusion
	m_ctrlUserInfo.SetCol(6);
	strData = m_ctrlUserInfo.GetText();
	stUser.dExclusion = atof(strData);


	m_ctrlUserInfo.SetRow(3);
	//측정Recipe
	m_ctrlUserInfo.SetCol(2);
	strData = m_ctrlUserInfo.GetText();
	sprintf_s(stUser.szRecipeFileName, "%s", strData.GetString());


	m_ctrlUserInfo.SetRow(4);
	//자동/수동 투입
	m_ctrlUserInfo.SetCol(2);
	strData = m_ctrlUserInfo.GetText();
	sprintf_s(stUser.szInsertMode, "%s", strData.GetString());

	//웨이퍼얼라인
	m_ctrlUserInfo.SetCol(4);
	strData = m_ctrlUserInfo.GetText();
	sprintf_s(stUser.szWaferAlignMode, "%s", strData.GetString());

	//Vision AF
	m_ctrlUserInfo.SetCol(6);
	strData = m_ctrlUserInfo.GetText();
	sprintf_s(stUser.szVisionAFMode, "%s", strData.GetString());


	m_ctrlUserInfo.SetRow(5);
	//검사일자
	m_ctrlUserInfo.SetCol(2);
	strData = m_ctrlUserInfo.GetText();
	sprintf_s(stUser.szDate, "%s", strData.GetString());

	//Slot No.
	m_ctrlUserInfo.SetCol(4);
	strData = m_ctrlUserInfo.GetText();
	stUser.iSlotNo = atoi(strData);

	//Op. ID
	m_ctrlUserInfo.SetCol(6);
	strData = m_ctrlUserInfo.GetText();
	sprintf_s(stUser.szOperatorID, "%s", strData.GetString());


	m_ctrlUserInfo.SetRow(6);
	//검사시작시각
	m_ctrlUserInfo.SetCol(2);
	strData = m_ctrlUserInfo.GetText();
	sprintf_s(stUser.szTimeStart, "%s", strData.GetString());

	//검사종료시각
	m_ctrlUserInfo.SetCol(4);
	strData = m_ctrlUserInfo.GetText();
	sprintf_s(stUser.szTimeEnd, "%s", strData.GetString());

	//검사시간
	m_ctrlUserInfo.SetCol(6);
	strData = m_ctrlUserInfo.GetText();
	sprintf_s(stUser.szTimeSpend, "%s", strData.GetString());


	m_ctrlUserInfo.SetRow(7);
	//파일경로
	m_ctrlUserInfo.SetCol(2);
	strData = m_ctrlUserInfo.GetText();
	sprintf_s(stUser.szDataFile, "%s", strData.GetString());


	m_ctrlUserInfo.SetRow(8);
	//Description
	m_ctrlUserInfo.SetCol(2);
	strData = m_ctrlUserInfo.GetText();
	sprintf_s(stUser.szDescription, "%s", strData.GetString());

	if (theApp.m_fnGetViewPointer()->m_nPage == SW_AUTO)
		memcpy(&theApp.m_fnGetViewPointer()->m_pAuto->m_stUser, &stUser, sizeof(t_USER_INFO));

// 	GetDlgItem( IDC_EDT_LOTID )->GetWindowText( strData );
// 	sprintf_s(stUser.szLotID, "%s", strData.GetString());
// 
// 	GetDlgItem( IDC_EDT_SIZE )->GetWindowText( strData );
// 	stUser.dSampleSize = atof( strData );
// 
// 	GetDlgItem( IDC_EDT_THICKNESS )->GetWindowText( strData );
// 	stUser.dThickness = atof( strData );
// 
// 	GetDlgItem( IDC_EDT_MAT_RESISTIVITY )->GetWindowText( strData );
// 	sprintf_s( stUser.szMatResistivity, "%s", strData.GetString());
// 
// 	GetDlgItem( IDC_EDT_PROBE_SPACE )->GetWindowText( strData );
// 	stUser.dProbeSpace =  atof( strData );
// 
// 	GetDlgItem( IDC_EDT_TMEASURE )->GetWindowText( strData );
// 	stUser.dTMeasure =  atof( strData );
// 
// 	GetDlgItem( IDC_EDT_MMODE )->GetWindowText( strData );
// 	sprintf_s( stUser.szMMode, "%s", strData.GetString());
// 
// 	GetDlgItem( IDC_EDT_OPID )->GetWindowText( strData );
// 	sprintf_s( stUser.szOperatorID, "%s", strData.GetString());
// 
// 	GetDlgItem( IDC_EDT_ANALYSIS )->GetWindowText( strData );
// 	sprintf_s( stUser.szAnalysis, "%s", strData.GetString());
// 
// 	GetDlgItem( IDC_EDT_DESCRIPTION )->GetWindowText( strData );
// 	sprintf_s( stUser.szDescription, "%s", strData.GetString());
// 
// 	GetDlgItem( IDC_EDT_FLAT )->GetWindowText( strData );
// 	stUser.dFlat = atof( strData );
// 
// 	GetDlgItem( IDC_EDT_METERIAL )->GetWindowText( strData );
// 	sprintf_s( stUser.szMaterial, "%s",strData.GetString());
// 
// 	GetDlgItem( IDC_EDT_CORRECTIONF )->GetWindowText( strData );
// 	stUser.dCorrectionF = atof( strData );
// 
// 	GetDlgItem( IDC_EDT_TCOEFFICENT )->GetWindowText( strData );
// 	stUser.dTCoefficient = atof( strData );
// 
// 	GetDlgItem( IDC_EDT_TREFERENCE )->GetWindowText( strData );
// 	stUser.dTReference = atof( strData );
// 
// 	GetDlgItem( IDC_EDT_DATA_FILE )->GetWindowText( strData );
// 	sprintf_s( stUser.szDataFile, "%s", strData.GetString());
// 
// 	GetDlgItem( IDC_EDT_SIGMA_MAX )->GetWindowText( strData );
// 	stUser.dSigmaMax = atof( strData );
// 
// 	GetDlgItem( IDC_EDT_EXCLUSION )->GetWindowText( strData );
// 	stUser.dExclusion = atof( strData );
// 
// 	GetDlgItem( IDC_EDT_THIN_METERIAL )->GetWindowText( strData );
// 	sprintf_s( stUser.szThinfilmmaterial, "%s", strData.GetString());
// 
// 	GetDlgItem( IDC_EDT_THIN_THICKNESS )->GetWindowText( strData );
// 	stUser.dThinfilmthickness = atof( strData );
// 
// 	GetDlgItem( IDC_EDT_SIGMA_MIN )->GetWindowText( strData );
// 	stUser.dSigmaMin = atof( strData );
// 
// 	GetDlgItem( IDC_EDT_DATE )->GetWindowText( strData );
// 	sprintf_s( stUser.szDate, "%s", strData.GetString());
// 
// 	GetDlgItem( IDC_EDT_TIME )->GetWindowText( strData );
// 	sprintf_s( stUser.szTime, "%s", strData.GetString());
}





void CUSerInputDLG::OnBnClickedBtnHide()
{
	t_USER_INFO stUser;

	//LOT ID
	m_ctrlUserInfo.SetRow(1);
	m_ctrlUserInfo.SetCol(2);

	CString strData("");
	strData = m_ctrlUserInfo.GetText();

	// Space Charactor Del
	int			nIndex = 0;
	while ((nIndex = strData.Find(STR_SPACE)) >= 0)
	{
		strData = strData.Left(nIndex) + STR_UNDERLINE + strData.Right(strData.GetLength() - nIndex - 1);
	}
	m_ctrlUserInfo.SetText(strData);


	m_fnGetInputData(stUser);

	theApp.m_fnGetViewPointer()->m_pAuto->GetDlgItem(IDC_EDT_LOTID)->SetWindowText(strData);

	m_fnSetShowFlag( FALSE );
}


void CUSerInputDLG::OnEnChangeEdtTime()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
