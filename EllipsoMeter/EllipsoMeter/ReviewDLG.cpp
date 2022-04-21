// ReviewDLG.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EllipsoMeter.h"
#include "ReviewDLG.h"
#include "afxdialogex.h"
#include "WorkRun.h"


// CReviewDLG 대화 상자입니다.

IMPLEMENT_DYNAMIC(CReviewDLG, CDialogEx)

CReviewDLG::CReviewDLG(CWnd* pParent /*=NULL*/)
	: CDialogEx(CReviewDLG::IDD, pParent)
{
	this->Create( IDD_REVIEW_DLG );
	m_bShow        = FALSE;
	m_strModel     = _T("");
	m_nMeasureItem = 1;
	m_nSelLayer    = 0;

	m_hExcelWriteThread = NULL;
}

CReviewDLG::~CReviewDLG()
{
}

void CReviewDLG::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FPSPREAD_INFO, m_clReview);
	DDX_Control(pDX, IDC_FPSPREAD_RESULT, m_clReviewResult);
	DDX_Control(pDX, IDC_MEASURE_ITEM, m_clMeasureItem);
	DDX_Control(pDX, IDC_EDIT_WAFER_NUM, m_ctrlEditWaferNum);
	DDX_Control(pDX, IDC_CBO_LAYER, m_clLayer);
}


BEGIN_MESSAGE_MAP(CReviewDLG, CDialogEx)
	ON_MESSAGE(UM_REPORT_READY_REVIEW, m_fnSetReportReadyReview)//차트만들고 UM_REPORT 콜
	ON_MESSAGE(UM_REPORT_REVIEW, m_fnSetReportReview)
	ON_BN_CLICKED(IDC_BTN_USER_INFO, &CReviewDLG::OnBnClickedBtnUserInfo)
	ON_BN_CLICKED(IDC_BTN_XML_PATH, &CReviewDLG::OnBnClickedBtnXmlPath)
	ON_BN_CLICKED(IDC_BTN_REPORT, &CReviewDLG::OnBnClickedBtnReport)
	ON_CBN_SELCHANGE(IDC_MEASURE_ITEM, &CReviewDLG::OnCbnSelchangeMeasureItem)
	ON_BN_CLICKED(IDC_BTN_RESULT, &CReviewDLG::OnBnClickedBtnResult)
	ON_CBN_SELCHANGE(IDC_CBO_LAYER, &CReviewDLG::OnCbnSelchangeCboLayer)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

//---------------------------------------------------------------------------------------------
// 외부 참조용 Function
//---------------------------------------------------------------------------------------------
BOOL CReviewDLG::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (( nID   == 2 ) &&
		( nCode == -1 ))
	{
		m_fnSetShowFlag( FALSE );
	}
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CReviewDLG::m_fnSetShowFlag( BOOL bShow )
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

BOOL CReviewDLG::PreTranslateMessage(MSG* pMsg) 
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
void CReviewDLG::m_fnSetInit( ) 
{
	CString s("");
	s.Format( "ReView 메인창" );
	this->SetWindowText(s);

	this->MoveWindow( 20, 100, 1050, 860, TRUE );
	m_pTaskHandle = new CTaskHandle( this, SHARED_NAME, 
										1, 
										sizeof( SHEET_RESISTANCE_Format ) + 1,
										0,
										FALSE );

	m_pTask					= ( SHEET_RESISTANCE_Format* )m_pTaskHandle->m_pFilePointer( );
	for (int i = 0; i < 7; i++)
	{
		m_clMeasureItem.AddString(strMeasureItem[i]);
	}
	m_clMeasureItem.SetCurSel(1);

	m_fWaferNumFont.CreateFont(50, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, 0, 0, 0, 0, 0, _T("굴림체"));
	m_ctrlEditWaferNum.SetFont(&m_fWaferNumFont);
	m_ctrlEditWaferNum.SetWindowTextA("0");
	m_clLayer.SetCurSel(0);

}

//---------------------------------------------------------------------------------------------------------
// 종료에 필요한 작업을 한다.
//---------------------------------------------------------------------------------------------------------
void CReviewDLG::m_fnSetEnd() 
{
	if ( m_pTaskHandle != NULL )
	{
		m_pTaskHandle->m_fnSetEnd();
		delete m_pTaskHandle;
		m_pTaskHandle = NULL;
	}
}


void CReviewDLG::OnBnClickedBtnUserInfo()
{
	theApp.m_fnGetViewPointer()->m_pUserReview->m_fnSetShowFlag( !theApp.m_fnGetViewPointer()->m_pUserReview->m_bShow );
}


void CReviewDLG::OnBnClickedBtnXmlPath()
{
	CString strExcute("");
	CString strExt("");
	int  nFind  = -1;
	char szFilters[] = "XML files (*.XML)|*.xml|All Files (*.*)|*.*||";

	CFileDialog dlg ( TRUE, 
						NULL,
						m_pTask->stCommon.szXmlResultPath, //경로
						OFN_HIDEREADONLY,
						szFilters);

	dlg.m_ofn.lpstrTitle = "Select XML file";

	if( IDOK == dlg.DoModal() ) 
	{
		strExcute = dlg.GetPathName();
		if ( !strExcute.IsEmpty() )
		{
			nFind = strExcute.ReverseFind('.');
			if ( nFind != -1 )
			{
				strExt = strExcute.Right( strExcute.GetLength() - nFind );
				strExt.MakeUpper();
				if ( strExt == ".XML" )
				{
					GetDlgItem( IDC_EDT_XML_PATH )->SetWindowText( strExcute );
					m_fnGetXMLResult( strExcute );

					theApp.m_fnGetViewPointer()->m_pReviewMap->m_nWaperSize = m_pTask->stReview.nWaperSize;
					theApp.m_fnGetViewPointer()->m_pReviewMap->m_dExclusion = m_pTask->stReview.stUser.dExclusion;
					theApp.m_fnGetViewPointer()->m_fnSetChartView(SW_MAP);
				}
			}
		}
	}
}

void CReviewDLG::OnBnClickedBtnReport()
{
	SendMessage( UM_REPORT_READY_REVIEW, 0, 0 );
}

//------------------------------------------------------------------------
// 파일의 경로를 읽어온다.
//------------------------------------------------------------------------
CString CReviewDLG::m_fnGetResultFileFullPath( )
{
	CString strMemo("");
	char szFullPath[MAX_PATH + 1];
	memset( szFullPath, 0x00, sizeof( szFullPath ));

	sprintf_s( szFullPath, "%s%s", m_Util.m_fnGetCurrentDrive().GetString(), DEFAULT_RESULT_PATH );
	szFullPath[MAX_PATH] = NULL;
	m_Util.m_fnSetCreateDir( szFullPath );

	szFullPath[MAX_PATH] = NULL;
	return szFullPath;
}

BOOL CReviewDLG::m_fnGetXMLResult( CString strXMLFile )
{
	CString strWork("");
	CString strData("");
	CString strFullPath("");
	CStringArray 	strUSE;
	CStringArray 	strNAME;
	CStringArray	strCol;
	CStringArray	strRow;
	CStringArray	strSub;
	CStringArray 	strX;
	CStringArray 	strY;
	CStringArray 	strZ;
	CStringArray 	strMSE[MAX_LAYER];
	CStringArray 	strRoughness[MAX_LAYER];
	CStringArray 	strThickness[MAX_LAYER];
	CStringArray 	strN550[MAX_LAYER];
	CStringArray 	strK550[MAX_LAYER];
	CStringArray 	strN632[MAX_LAYER];
	CStringArray 	strK632[MAX_LAYER];
	int nNo			= 0;

	CStringArray strLst;
	CXmlElement *pElement     = NULL;
	CXmlElement *pRootElement = NULL;

	strXMLFile.Replace( "SGL","XML");
	strFullPath.Format( "%s", strXMLFile ); 

	strWork = "Result Data XML 읽기 시작";
	theApp.m_fnGetViewPointer()->m_fnLogWrite( strWork );

	m_fnSRSheetClear();

	CXmlDocument* pXML = new CXmlDocument();
	pXML->Load( strFullPath );

	pXML->GetTagToString( MODEL , strData );
	pXML->Parse( strData );
	pRootElement = pXML->GetRootElement();
	if ( pRootElement == NULL )
	{
		delete pXML;
		pXML = NULL;
		theApp.m_fnGetViewPointer()->m_fnLogWrite( "pRootElement is Null" );
		return FALSE;
	}

	strLst.RemoveAll();
	strLst.Add(ELEMENT);
	pElement = pXML->FindElement( pRootElement, strLst );
	if ( pElement != NULL )
	{

		pXML->FindData( pElement, ALIGN_USE, strWork );
		m_pTask->stReview.bUseAlign = FALSE;
		if ( strWork == STR_YES )
		{
			m_pTask->stReview.bUseAlign = TRUE;
		}
		pXML->FindData( pElement, WAPER_TYPE, strWork );
		m_pTask->stReview.bFlat = FALSE;
		if ( strWork == STR_YES )
		{
			m_pTask->stReview.bFlat = TRUE;
		}
		pXML->FindData( pElement, CHART_INCLUDE, strWork );
		m_pTask->stReview.bChartReport = FALSE;
		if ( strWork == STR_YES )
		{
			m_pTask->stReview.bChartReport = TRUE;
		}
		pXML->FindData( pElement, WAPER_THICKNESS, strWork );
		m_pTask->stReview.dWaperThickness = atof( strWork );
		pXML->FindData( pElement, OUTPUT_DISP, strWork );
		m_pTask->stReview.nOutputDisp = atoi( strWork );
		pXML->FindData( pElement, ALIGN_Z_SAFETY_AXIS, strWork );
		m_pTask->stReview.dAlignSafetyZ = atof( strWork );

		pXML->FindData( pElement, SR_Z_SAFETY_AXIS, strWork );
		m_pTask->stReview.dSRSafetyZ = atof( strWork );

		pXML->FindData( pElement, SR_SPEED, strWork );
		m_pTask->stReview.dSRSpeed = atof( strWork );

		pXML->FindData( pElement, ALIGN_Z, strWork );
		m_pTask->stReview.dAlignZ = atof( strWork );

		pXML->FindData( pElement, ALIGN_X, strWork );
		m_pTask->stReview.dAlignX = atof( strWork );
		pXML->FindData( pElement, ALIGN_Y, strWork );
		m_pTask->stReview.dAlignY = atof( strWork );
		pXML->FindData( pElement, WAPER_SIZE, strWork );
		nNo = atoi( strWork );
		m_pTask->stReview.nWaperSize = nNo;
		pXML->FindData( pElement, SR_Z, strWork );
		m_pTask->stReview.dSRZ = atof( strWork );

		pXML->FindData( pElement, LIGHT_VALUE, strWork );
		m_pTask->stReview.nLight = atoi( strWork );

		pXML->FindData( pElement, AUTO_START_X, strWork );
		m_pTask->stReview.dAutoX = atof(strWork);
		pXML->FindData( pElement, AUTO_START_Y, strWork );
		m_pTask->stReview.dAutoY = atof(strWork);
		pXML->FindData( pElement, AUTO_DISTANCE_X, strWork );
		m_pTask->stReview.dAutoDistanceX = atof(strWork);
		pXML->FindData( pElement, AUTO_DISTANCE_Y, strWork );
		m_pTask->stReview.dAutoDistanceY = atof(strWork);
		pXML->FindData( pElement, AUTO_STEP_X, strWork );
		m_pTask->stReview.nAutoStepX = atoi(strWork);
		pXML->FindData( pElement, AUTO_STEP_Y, strWork );
		m_pTask->stReview.nAutoStepY = atoi(strWork);
		pXML->FindData( pElement, AUTO_METHOD, strWork );
		m_pTask->stReview.nAutoMethod = atoi(strWork);
	}

	strLst.RemoveAll();
	strLst.Add(INSPECT_RESULT);
	pElement = pXML->FindElement( pRootElement, strLst );
	if ( pElement != NULL )
	{
		pXML->FindData( pElement, INSP_MODEL, strWork );
		sprintf_s(m_pTask->stReview.szInspModel, "%s", strWork.GetString());
		m_strModel = strWork;
		pXML->FindData( pElement, INSP_DATE_TIME, strWork );
		sprintf_s( m_pTask->stReview.szInspDateTime, "%s", strWork.GetString());
		pXML->FindData( pElement, INSP_DATE, strWork );
		sprintf_s( m_pTask->stReview.szInspDate, "%s", strWork.GetString());
		pXML->FindData( pElement, TACT_TIME, strWork ); 
		sprintf_s( m_pTask->stReview.szInspTackTime, "%s", strWork.GetString());
		pXML->FindData(pElement, LOT_START_TIME, strWork);
		sprintf_s(m_pTask->stReview.szLotStartTime, "%s", strWork.GetString());
	}

	strLst.RemoveAll();
	strLst.Add(USER_INPUT);
	pElement = pXML->FindElement( pRootElement, strLst );
	if ( pElement != NULL )
	{
		pXML->FindData(pElement, LOTID, strData);
		sprintf_s(m_stUser.szLotID, "%s", strData.GetString());
		pXML->FindData(pElement, SAMPLE_SIZE, strData);
		m_stUser.dSampleSize = atof(strData);
		pXML->FindData(pElement, THICKNESS, strData);
		m_stUser.dThickness = atof(strData);
		pXML->FindData(pElement, EXCLUSION, strData);
		m_stUser.dExclusion = atof(strData);
		pXML->FindData(pElement, RECIPE, strData);
		sprintf_s(m_stUser.szRecipeFileName, "%s", strData.GetString());
		pXML->FindData(pElement, INSERTMODE, strData);
		sprintf_s(m_stUser.szInsertMode, "%s", strData.GetString());
		pXML->FindData(pElement, WAFERALIGNMODE, strData);
		sprintf_s(m_stUser.szWaferAlignMode, "%s", strData.GetString());
		pXML->FindData(pElement, VISIONAFMODE, strData);
		sprintf_s(m_stUser.szVisionAFMode, "%s", strData.GetString());
		pXML->FindData(pElement, USER_DATE, strData);
		sprintf_s(m_stUser.szDate, "%s", strData.GetString());
		pXML->FindData(pElement, SLOT_NO, strData);
		m_stUser.iSlotNo = atoi(strData);
		pXML->FindData(pElement, OPID, strData);
		sprintf_s(m_stUser.szOperatorID, "%s", strData.GetString());
		pXML->FindData(pElement, TIMESTART, strData);
		sprintf_s(m_stUser.szTimeStart, "%s", strData.GetString());
		pXML->FindData(pElement, TIMEEND, strData);
		sprintf_s(m_stUser.szTimeEnd, "%s", strData.GetString());
		pXML->FindData(pElement, TIMESPEND, strData);
		sprintf_s(m_stUser.szTimeSpend, "%s", strData.GetString());
		pXML->FindData(pElement, DATA_FILE, strData);
		sprintf_s(m_stUser.szDataFile, "%s", strData.GetString());
		pXML->FindData(pElement, DESCRIPTION, strData);
		sprintf_s(m_stUser.szDescription, "%s", strData.GetString());
		pXML->FindData(pElement, MEASUREAREATYPE, strData);
		m_stUser.iMeasureMode = atoi(strData); 

// 		pXML->FindData( pElement, LOTID, strWork );
// 		sprintf_s(m_stUser.szLotID, "%s", strWork.GetString());
// 		pXML->FindData( pElement, SAMPLE_SIZE, strWork );
// 		m_stUser.dSampleSize = atof( strWork );
// 		pXML->FindData( pElement, THICKNESS, strWork );
// 		m_stUser.dThickness = atof( strWork );
// 		pXML->FindData( pElement, OPID, strWork );
// 		sprintf_s( m_stUser.szOperatorID, "%s", strWork.GetString());
// 		pXML->FindData( pElement, DESCRIPTION, strWork );
// 		sprintf_s( m_stUser.szDescription, "%s", strWork.GetString());
// 		pXML->FindData( pElement, USER_FLAT, strWork );
// 		m_stUser.dFlat = atof( strWork );
// 		pXML->FindData( pElement, METERIAL, strWork );
// 		sprintf_s( m_stUser.szMaterial, "%s", strWork.GetString());
// 		pXML->FindData( pElement, EXCLUSION, strWork );
// 		m_stUser.dExclusion = atof( strWork );
// 		pXML->FindData( pElement, THIN_METERIAL, strWork );
// 		sprintf_s( m_stUser.szThinfilmmaterial, "%s", strWork.GetString());
// 		pXML->FindData( pElement, THIN_THICKNESS, strWork );
// 		m_stUser.dThinfilmthickness = atof( strWork );
// 		pXML->FindData( pElement, USER_DATE, strWork );
// 		sprintf_s( m_stUser.szDate, "%s", strWork.GetString());
// 		pXML->FindData( pElement, USER_TIME, strWork );
// 		sprintf_s( m_stUser.szTime, "%s", strWork.GetString());
	}

	theApp.m_fnGetViewPointer()->m_pUserReview->m_fnSetInputData( m_stUser );

	memcpy(&m_pTask->stReview.stUser, &m_stUser, sizeof(t_USER_INFO));


	strLst.RemoveAll();
	strLst.Add(BODY);
	strLst.Add(SR_TOTAL_CNT);
	pElement = pXML->FindElement(pRootElement, strLst);
	if (pElement != NULL)
	{
		pXML->FindData(pElement, DATA_CNT, strWork);
		m_pTask->stReview.nSRData = atoi( strWork );
		pXML->FindData(pElement, SLOT_NO, strWork);
		m_pTask->stReview.nSlotNo = atoi(strWork);
		pXML->FindData(pElement, LAYER_CNT, strWork);
		m_pTask->stReview.nLayer = atoi(strWork);
	}
		
	strLst.RemoveAll();
	strLst.Add(BODY);
	strLst.Add(SR_LIST);
	pElement = pXML->FindElement( pRootElement, strLst );
	if ( pElement != NULL )
	{
		strNAME.RemoveAll();
		strCol.RemoveAll();
		strRow.RemoveAll();
		strSub.RemoveAll();
		strX.RemoveAll();
		strY.RemoveAll();
		strZ.RemoveAll();
		pXML->FindDataList(pElement, strNAME, strCol, strRow, strSub, strX, strY, strZ);
		m_pTask->stReview.nSRData = strNAME.GetSize();
		for (int i = 0; i < strNAME.GetSize(); i++)
		{
			sprintf_s(m_pTask->stReview.stSRData[i].szName, "%s", strNAME.GetAt(i).GetString());
			m_pTask->stReview.stSRData[i].iColum = atoi(strCol.GetAt(i));
			m_pTask->stReview.stSRData[i].iRow = atoi(strRow.GetAt(i));
			m_pTask->stReview.stSRData[i].iSub = atoi(strSub.GetAt(i));
			m_pTask->stReview.stSRData[i].dwXPosition = atof(strX.GetAt(i));
			m_pTask->stReview.stSRData[i].dwYPosition = atof(strY.GetAt(i));
			m_pTask->stReview.stSRData[i].dwZPosition = atof(strZ.GetAt(i));
			m_pTask->stReviewData.m_dDataX[i] = m_pTask->stReview.stSRData[i].dwXPosition;
			m_pTask->stReviewData.m_dDataY[i] = m_pTask->stReview.stSRData[i].dwYPosition;
			m_pTask->stReviewData.m_dDataH[i] = m_pTask->stReview.stSRData[i].dwZPosition;
			m_pTask->stReviewData.m_dDataL[i] = i;
			m_pTask->stReviewData.m_nDataCnt = i + 1;
		}
	}
	m_pTask->stReview.nSRData = m_pTask->stReviewData.m_nDataCnt;

	strLst.RemoveAll();
	strLst.Add(BODY);
	strLst.Add(SR_DATA);
	pElement = pXML->FindElement(pRootElement, strLst);
	if (pElement != NULL)
	{
		t_ITEM_INFO stItem;
		pXML->FindItemDataList(pElement, m_pTask->stReview.nLayer, stItem );
		for (int i = 0; i < m_pTask->stReview.nSRData; i++)
		{
			for (int j = 0; j < m_pTask->stReview.nLayer; j++)
			{
				for (int k = 0; k < MAX_ITEM; k++)
				{
					m_pTask->stReview.stSRData[i].dwResult[j][k] = stItem.dItem[i][j][k];
					m_pTask->stReviewData.m_dDataZ[i][j][k] = stItem.dItem[i][j][k];
				}
			}
		}
	}
	m_pTask->stReviewData.m_nCurrCnt = 0;

	delete pXML;
	pXML = NULL;

	strWork = "Result XML Data 읽기 완료";
	theApp.m_fnGetViewPointer()->m_fnLogWrite( strWork );
	
	GetDlgItem( IDC_EDT_LOTID )->SetWindowText( m_pTask->stReview.stUser.szLotID );
	strWork.Format("%01d", m_pTask->stReview.nSlotNo);
	m_ctrlEditWaferNum.SetWindowTextA(strWork);


	for (int i = 0; i < m_pTask->stReview.nSRData; i++)
	{
		m_clReviewResult.SetRow(i + 1);
		m_clReviewResult.SetCol(REVIEW_TARGET_NO);
		strWork.Format("%01d", atoi(m_pTask->stReview.stSRData[i].szName));
		m_clReviewResult.SetText(strWork);
		m_clReviewResult.SetCol(REVIEW_ROW);
		strWork.Format("%01d", m_pTask->stReview.stSRData[i].iRow);
		m_clReviewResult.SetText(strWork);
		m_clReviewResult.SetCol(REVIEW_COL);
		strWork.Format("%01d", m_pTask->stReview.stSRData[i].iColum);
		m_clReviewResult.SetText(strWork);
		m_clReviewResult.SetCol(REVIEW_SUB);
		strWork.Format("%01d", m_pTask->stReview.stSRData[i].iSub);
		m_clReviewResult.SetText(strWork);
		m_clReviewResult.SetCol(REVIEW_X1_X2);
		strWork.Format("%7.3f", m_pTask->stReview.stSRData[i].dwXPosition);
		m_clReviewResult.SetText(strWork);
		m_clReviewResult.SetCol(REVIEW_Y1_Y2);
		strWork.Format("%7.3f", m_pTask->stReview.stSRData[i].dwYPosition);
		m_clReviewResult.SetText(strWork);
		m_clReviewResult.SetCol(REVIEW_MSE);
		strWork.Format("%.3f", m_pTask->stReview.stSRData[i].dwResult[0][0]);
		m_clReviewResult.SetText(strWork);
		m_clReviewResult.SetCol(REVIEW_TOTAL_THICKNESS);
		strWork.Format("%.3f", m_pTask->stReview.stSRData[i].dwResult[0][1]);
		m_clReviewResult.SetText(strWork);
		for (int j = 0; j < m_pTask->stReview.nLayer; j++)
		{
			m_clReviewResult.SetCol(REVIEW_LAYER_1 + j);
			strWork.Format("%.3f", m_pTask->stReview.stSRData[i].dwResult[j][m_nMeasureItem]);
			m_clReviewResult.SetText(strWork);
		}
	}

	m_fnSetReCalculate();

	return TRUE;
}

void CReviewDLG::m_fnSRSheetClear(BOOL bAllClear /*= TRUE */)
{
	for (int i = 0; i < MAX_DATA; i++)
	{
		m_clReviewResult.SetRow(i + 1);
		if (bAllClear)
		{
			m_clReviewResult.SetCol(REVIEW_TARGET_NO);
			m_clReviewResult.SetText("");
			m_clReviewResult.SetCol(REVIEW_COL);
			m_clReviewResult.SetText("");
			m_clReviewResult.SetCol(REVIEW_ROW);
			m_clReviewResult.SetText("");
			m_clReviewResult.SetCol(REVIEW_SUB);
			m_clReviewResult.SetText("");
			m_clReviewResult.SetCol(REVIEW_X1_X2);
			m_clReviewResult.SetText("");
			m_clReviewResult.SetCol(REVIEW_Y1_Y2);
			m_clReviewResult.SetText("");
		}
		m_clReviewResult.SetCol(REVIEW_MSE);
		m_clReviewResult.SetText("");
		m_clReviewResult.SetCol(REVIEW_TOTAL_THICKNESS);
		m_clReviewResult.SetText("");
		m_clReviewResult.SetCol(REVIEW_LAYER_1);
		m_clReviewResult.SetText("");
		m_clReviewResult.SetCol(REVIEW_LAYER_2);
		m_clReviewResult.SetText("");
		m_clReviewResult.SetCol(REVIEW_LAYER_3);
		m_clReviewResult.SetText("");
		m_clReviewResult.SetCol(REVIEW_LAYER_4);
		m_clReviewResult.SetText("");
		m_clReviewResult.SetCol(REVIEW_LAYER_5);
		m_clReviewResult.SetText("");
		m_clReviewResult.SetCol(REVIEW_LAYER_6);
		m_clReviewResult.SetText("");
		m_clReviewResult.SetCol(REVIEW_LAYER_7);
		m_clReviewResult.SetText("");
		m_clReviewResult.SetCol(REVIEW_LAYER_8);
		m_clReviewResult.SetText("");
		m_clReviewResult.SetCol(REVIEW_LAYER_9);
		m_clReviewResult.SetText("");
		m_clReviewResult.SetCol(REVIEW_LAYER_10);
		m_clReviewResult.SetText("");
	}
	m_pTask->stReview.nSRData = 0;
	memset(m_pTask->stReview.stSRData, 0x00, sizeof(m_pTask->stReview.stSRData));
	memset(&m_pTask->stReviewData, 0x00, sizeof(t_RESULT_INFO));
}

void CReviewDLG::m_fnSetReCalculate()
{
	float  fStd[MAX_LAYER][MAX_ITEM];
	memset(fStd, 0x00, sizeof(fStd));
	memset(m_pTask->stReviewData.m_dMax, 0x00, sizeof(m_pTask->stReviewData.m_dMax));
	memset(m_pTask->stReviewData.m_dMin, 0x00, sizeof(m_pTask->stReviewData.m_dMin));
	memset(m_pTask->stReviewData.m_dAvg, 0x00, sizeof(m_pTask->stReviewData.m_dAvg));
	memset(m_pTask->stReviewData.m_dStd, 0x00, sizeof(m_pTask->stReviewData.m_dStd));
	memset(m_pTask->stReviewData.m_dMax_Min, 0x00, sizeof(m_pTask->stReviewData.m_dMax_Min));
	memset(m_pTask->stReviewData.m_dUniformity, 0x00, sizeof(m_pTask->stReviewData.m_dUniformity));
	memset(m_pTask->stReviewData.m_fTot, 0x00, sizeof(m_pTask->stReviewData.m_fTot));
	for (int i = 0; i < m_pTask->stReview.nSRData; i++)
	{
		for (int j = 0; j < MAX_LAYER; j++)
		{
			for (int k = 0; k < MAX_ITEM; k++)
			{
				if (i == 0)
				{
					m_pTask->stReviewData.m_dMax[j][k] = m_pTask->stReview.stSRData[i].dwResult[j][k];
					m_pTask->stReviewData.m_dMin[j][k] = m_pTask->stReview.stSRData[i].dwResult[j][k];
					m_pTask->stReviewData.m_dAvg[j][k] = m_pTask->stReview.stSRData[i].dwResult[j][k];
					m_pTask->stReviewData.m_dStd[j][k] = 0.0;
					m_pTask->stReviewData.m_dMax_Min[j][k] = 0.0;
					m_pTask->stReviewData.m_dUniformity[j][k] = 0.0;
					m_pTask->stReviewData.m_fTot[j][k] = 0.0f;
					m_pTask->stReviewData.m_iZeroCnt[j][k] = 0;
				}

				if (m_pTask->stReview.stSRData[i].dwResult[j][k] > 0)
				{
					if (m_pTask->stReviewData.m_dMax[j][k] < m_pTask->stReview.stSRData[i].dwResult[j][k])
					{
						m_pTask->stReviewData.m_dMax[j][k] = m_pTask->stReview.stSRData[i].dwResult[j][k];
					}
					if (m_pTask->stReviewData.m_dMin[j][k] > m_pTask->stReview.stSRData[i].dwResult[j][k])
					{
						m_pTask->stReviewData.m_dMin[j][k] = m_pTask->stReview.stSRData[i].dwResult[j][k];
					}
					m_pTask->stReviewData.m_dMax_Min[j][k] = abs(m_pTask->stReviewData.m_dMax[j][k] - m_pTask->stReviewData.m_dMin[j][k]);

					m_pTask->stReviewData.m_fTot[j][k] += (float)m_pTask->stReview.stSRData[i].dwResult[j][k];
					m_pTask->stReviewData.m_dAvg[j][k] = (double)(m_pTask->stReviewData.m_fTot[j][k] / (double)(i + 1 - m_pTask->stReviewData.m_iZeroCnt[j][k]));
				} 
				else
				{
					m_pTask->stReviewData.m_iZeroCnt[j][k]++;
				}
			}
		}
	}

	for (int j = 0; j < MAX_LAYER; j++)
	{
		for (int k = 0; k < MAX_ITEM; k++)
		{

			for (int i = 0; i < m_pTask->stReview.nSRData; i++)
			{
				if (m_pTask->stReview.stSRData[i].dwResult[j][k] > 0)
					fStd[j][k] += (float)pow(m_pTask->stReview.stSRData[i].dwResult[j][k] - m_pTask->stReviewData.m_dAvg[j][k], 2);
			}

			m_pTask->stReviewData.m_dStd[j][k] = sqrt((double)(fStd[j][k] / (double)(m_pTask->stReview.nSRData - m_pTask->stReviewData.m_iZeroCnt[j][k])));
			m_pTask->stReviewData.m_dUniformity[j][k] = (m_pTask->stReviewData.m_dStd[j][k] / m_pTask->stReviewData.m_dAvg[j][k]);
			m_pTask->stReviewData.m_dContourMax[j][k] = m_pTask->stReviewData.m_dMax[j][k];
			m_pTask->stReviewData.m_dContourMin[j][k] = m_pTask->stReviewData.m_dMin[j][k];
			m_pTask->stReviewData.m_dContourStep[j][k] = m_pTask->stReviewData.m_dMax_Min[j][k] / 10.;
		}
	}

	m_fnSetInspStatus(m_pTask->stReviewData.m_dMax[m_nSelLayer][m_nMeasureItem],
									m_pTask->stReviewData.m_dMin[m_nSelLayer][m_nMeasureItem],
									m_pTask->stReviewData.m_dAvg[m_nSelLayer][m_nMeasureItem],
									m_pTask->stReviewData.m_dStd[m_nSelLayer][m_nMeasureItem],
									m_pTask->stReviewData.m_dMax_Min[m_nSelLayer][m_nMeasureItem],
									m_pTask->stReviewData.m_dUniformity[m_nSelLayer][m_nMeasureItem]);

}

void CReviewDLG::m_fnSetView()
{
	CString strResult("");
	int iMitem = m_nMeasureItem;
	if (iMitem < 2)
	{
		iMitem = 2;
	}

	for (int i = 0; i < m_pTask->stReviewData.m_nDataCnt; i++)
	{
		m_clReviewResult.SetRow(i + 1);
		m_clReviewResult.SetCol(REVIEW_MSE);
		strResult.Format("%.3f", m_pTask->stReview.stSRData[i].dwResult[0][0]);
		m_clReviewResult.SetText(strResult);
		m_clReviewResult.SetCol(REVIEW_TOTAL_THICKNESS);
		strResult.Format("%.3f", m_pTask->stReview.stSRData[i].dwResult[0][1]);
		m_clReview.SetText(strResult);
		for (int j = 0; j < MAX_LAYER; j++)
		{
			m_clReviewResult.SetCol(REVIEW_LAYER_1 + j);
			strResult.Format("%.3f", m_pTask->stReview.stSRData[i].dwResult[j][iMitem]);
			m_clReviewResult.SetText(strResult);
		}
	}

	m_fnSetInspStatus(m_pTask->stReviewData.m_dMax[m_nSelLayer][m_nMeasureItem],
							m_pTask->stReviewData.m_dMin[m_nSelLayer][m_nMeasureItem],
							m_pTask->stReviewData.m_dAvg[m_nSelLayer][m_nMeasureItem],
							m_pTask->stReviewData.m_dStd[m_nSelLayer][m_nMeasureItem],
							m_pTask->stReviewData.m_dMax_Min[m_nSelLayer][m_nMeasureItem],
							m_pTask->stReviewData.m_dUniformity[m_nSelLayer][m_nMeasureItem]);


	theApp.m_fnGetViewPointer()->PostMessage(UM_CHART_VIEW, theApp.m_fnGetViewPointer()->m_nChart, 0);
}



void CReviewDLG::m_fnSetInspStatus( double dMax, double dMin, double dAvg, double dStd, double dMax_Min, double dUniformity)
{
	CString strData("");


	m_clReview.SetRow( 1 );

	m_clReview.SetCol(2);
	m_clReview.SetText( m_pTask->stReview.szInspTackTime );

	strData.Format( "%7.3f", dMax );
	m_clReview.SetCol( 4 );
	m_clReview.SetText( strData );

	strData.Format( "%7.3f", dMin );
	m_clReview.SetCol( 6 );
	m_clReview.SetText( strData );

	strData.Format( "%7.3f", dMax_Min );
	m_clReview.SetCol( 8 );
	m_clReview.SetText( strData );


	m_clReview.SetRow( 2 );

	strData.Format( "%7.3f", dAvg );
	m_clReview.SetCol( 2 );
	m_clReview.SetText( strData );

	strData.Format( "%7.3f", dUniformity );
	m_clReview.SetCol( 4 );
	m_clReview.SetText( strData );

	strData.Format("%7.3f", dStd);
	m_clReview.SetCol(6);
	m_clReview.SetText(strData);
}



BEGIN_EVENTSINK_MAP(CReviewDLG, CDialogEx)
	ON_EVENT(CReviewDLG, IDC_FPSPREAD_RESULT, 5, CReviewDLG::ClickFpspreadResult, VTS_I4 VTS_I4)
END_EVENTSINK_MAP()


void CReviewDLG::ClickFpspreadResult(long Col, long Row)
{
	m_pTask->stReviewData.m_nCurrCnt = Row - 1;
}


void CReviewDLG::OnCbnSelchangeMeasureItem()
{
	int nNo = m_clMeasureItem.GetCurSel();
	if (nNo == -1)
	{
		nNo = 1;
		m_clMeasureItem.SetCurSel(nNo);
	}
	m_nMeasureItem = nNo;
	m_fnSetView();
}


void CReviewDLG::OnBnClickedBtnResult()
{
#if !_DEBUG
	if (m_pTask->stLogin.nLevel == UNKNOWN_USER)
	{
		return;
	}
#endif
	ShellExecute(NULL, _T("open"), _T("explorer.exe"), m_pTask->stCommon.szExcelResultPath, NULL, SW_SHOW);
}

void threadChartView(int iKind)
{
	if (iKind < SW_MAP || iKind > SW_3D)
		iKind = SW_MAP;
	theApp.m_fnGetViewPointer()->SendMessage(UM_CHART_VIEW, iKind);

	return;
}

UINT ExcelWriteThreadReview(LPVOID pFuncData)
{
	CReviewDLG		        *pQI = (CReviewDLG *)pFuncData;
	int                     nLayer = 0;
	int                     nMeasureItem = 0;
	int                     nLevel = 0;
	int                     nChart = 0;
	CString                 strAxisImage("");
	CString                 str2DImage("");
	CString                 str3DImage("");
	BOOL                    bProcess = FALSE;

	CEllipsoMeterDlg& nView = *theApp.m_fnGetViewPointer();//참조

	thread thrdChartView;


	//기존 선택 저장
	nMeasureItem = nView.m_pReview->m_nMeasureItem;	//아이템
	nLayer = nView.m_pReview->m_nSelLayer;			//레이어
	nChart = nView.m_nChart;						//차트종류

	nView.m_pReview->m_nSelLayer = 0;
	pQI->m_nMeasureItem = 0;
	nView.m_nChart = SW_MAP;

	if (!pQI->m_pTask->stCommon.bChartReport)
		pQI->m_nExcelWriteStep = 200;
	else
		TECmsgOut(L"Excel 레포트를 위한 차트생성 작업을 시작합니다.", GREEN_1, 100);

	while (pQI->m_bExcelWriteEnd)
	{
		if (pQI->m_hWnd == NULL) continue;
		if (bProcess) continue;
		bProcess = TRUE;
		Sleep(5);

		switch (pQI->m_nExcelWriteStep)
		{
		case 10:
			if (nView.m_pReview->m_nMeasureItem == MAX_ITEM)
			{
				nView.m_pReview->m_nSelLayer++;
				nView.m_pReview->m_nMeasureItem = 0;
			}
			if (nView.m_pReview->m_nSelLayer == MAX_LAYER)
			{
				nView.m_nChart++;
				nView.m_pReview->m_nSelLayer = 0;
				nView.m_pReview->m_nMeasureItem = 0;
			}
			pQI->m_nExcelWriteStep = 30;

			if (nView.m_nChart == 3)
				pQI->m_nExcelWriteStep = 200;

			break;

		case 30:
			nView.PostMessage(UM_CHART_VIEW, nView.m_nChart);
// 			thrdChartView = thread(threadChartView, nView.m_nChart);
// 			thrdChartView.detach();

			pQI->m_nExcelWriteStep = 40 + (nView.m_nChart * 20);
			break;


		case 40:
			nView.m_pReviewMap->m_bView = TRUE;
			nView.m_pReviewMap->m_bSave = TRUE;
			nView.m_pReviewMap->SendMessage(UM_MAP_DRAW, 0, 0);
			pQI->m_nExcelWriteStep = 50;
			break;
		case 50:
			if (!nView.m_pReviewMap->m_bSave)
			{
				strAxisImage.Format("AXIS_%01d_%01d.bmp", nView.m_pReview->m_nSelLayer + 1, nView.m_pReview->m_nMeasureItem + 1);
				nView.m_pReviewMap->m_fnSetContourChartToImage(nView.m_pAuto->m_fnGetTempPath() + strAxisImage);
				nView.m_pReview->m_nMeasureItem++;
				pQI->m_nExcelWriteStep = 10;
			}
			break;


		case 60:
			nView.m_pContour->m_bSave = TRUE;
			pQI->m_nExcelWriteStep = 70;
			break;
		case 70:
			if (!nView.m_pContour->m_bSave)
			{
				str2DImage.Format("2D_%01d_%01d.bmp", nView.m_pReview->m_nSelLayer + 1, nView.m_pReview->m_nMeasureItem + 1);
				nView.m_pContour->m_fnSetContourChartToImage(FALSE, nView.m_pAuto->m_fnGetTempPath() + str2DImage);
				nView.m_pReview->m_nMeasureItem++;
				pQI->m_nExcelWriteStep = 10;
			}
			break;


		case 80:
			nView.m_pContour->m_bSave = TRUE;
			pQI->m_nExcelWriteStep = 90;
			break;
		case 90:
			if (!nView.m_pContour->m_bSave)
			{
				str3DImage.Format("3D_%01d_%01d.bmp", nView.m_pReview->m_nSelLayer + 1, nView.m_pReview->m_nMeasureItem + 1);
				nView.m_pContour->m_fnSetContourChartToImage(TRUE, nView.m_pAuto->m_fnGetTempPath() + str3DImage);
				nView.m_pReview->m_nMeasureItem++;
				pQI->m_nExcelWriteStep = 10;
			}
			break;


		case 200:
			nView.m_pMap->m_bView = FALSE;
			nView.m_pReview->m_nMeasureItem = nMeasureItem;
			nView.m_pReview->m_nSelLayer = nLayer;
			nView.PostMessage(UM_CHART_VIEW, nChart);

			nView.m_pReviewMap->m_bView = FALSE;
			pQI->PostMessageA(UM_REPORT_REVIEW, 0, 0);
			pQI->m_nExcelWriteStep = 300;
			break;
		case 300:
			pQI->m_nExcelWriteStep = -1;
			pQI->m_bExcelWriteEnd = FALSE;
			TECmsgOut(L"Excel 레포트를 위한 차트생성 작업을 종료합니다.", GREEN_1, 100);
			break;
		}

		
		bProcess = FALSE;
	}
	if (pQI->m_hWnd == NULL) return 0;

	if (pQI->m_hExcelWriteThread != NULL)
	{
		SuspendThread(pQI->m_hExcelWriteThread);
		CloseHandle(pQI->m_hExcelWriteThread);
		pQI->m_hExcelWriteThread = NULL;
	}
	return 0;
}

LRESULT CReviewDLG::m_fnSetReportReview(WPARAM nNo, LPARAM lParam)
{
#if MIL_USE
	pWorkRun->m_bThrdFlag[TF_MAKE_REPORT] = TRUE;

	t_MODEL_INFO  stReport;
	t_RESULT_INFO stData;
	CString strModel("");
	if (m_pTask->stReview.nSRData == 0) 
	{
		pWorkRun->m_bThrdFlag[TF_MAKE_REPORT] = FALSE;
		return 1;
	}

	strModel.Format("%s", theApp.m_fnGetViewPointer()->m_pReview->m_strModel);
	strModel.Trim();
	if (strModel.IsEmpty())
	{
		strModel = "Empty";
	}
	memcpy(&stReport, &m_pTask->stReview, sizeof(t_MODEL_INFO));

	memcpy(&stData, &m_pTask->stReviewData, sizeof(t_RESULT_INFO));

	theApp.m_fnGetViewPointer()->m_pAuto->m_fnSetXMLResult(strModel, stReport, stData, SW_REVIEW );

	memcpy(&theApp.m_fnGetViewPointer()->m_stModel, &stReport, sizeof(t_MODEL_INFO));
	theApp.m_fnGetViewPointer()->m_fnSetXLSResult(SW_REVIEW);

	pWorkRun->m_bThrdFlag[TF_MAKE_REPORT] = FALSE;
#endif
	return 0;
}

LRESULT CReviewDLG::m_fnSetReportReadyReview(WPARAM nNo, LPARAM lParam)
{
	if (m_hExcelWriteThread != NULL)
	{
		m_bExcelWriteEnd = FALSE;
		SuspendThread(m_hExcelWriteThread);
		CloseHandle(m_hExcelWriteThread);
		m_hExcelWriteThread = NULL;
	}

	unsigned long	ThreadID;
	m_bExcelWriteEnd = TRUE;
	m_hExcelWriteThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ExcelWriteThreadReview, this, NULL, &ThreadID);
	m_nExcelWriteStep = 10;

	return 0;
}


void CReviewDLG::OnCbnSelchangeCboLayer()
{
// 	if (theApp.m_fnGetViewPointer()->m_nPage == SW_REVIEW)
	{
		m_nSelLayer = m_clLayer.GetCurSel();
		theApp.m_fnGetViewPointer()->m_pContour->m_clLayer.SetCurSel(m_nSelLayer);
		m_fnSetView();
	}
}


void CReviewDLG::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if(bShow)
	{
		m_nSelLayer = m_clLayer.GetCurSel();
		theApp.m_fnGetViewPointer()->m_pContour->m_clLayer.SetCurSel(m_nSelLayer);
		m_fnSetView();
	}
}
