// Model.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EllipsoMeter.h"
#include "Model.h"
#include "afxdialogex.h"
#include "ModelManage.h"
#include "WorkRun.h"
#include "CamJAI.h"
#include "MilVision.h"
#include "MilVisionDlg.h"
#include "ModelData.h"
#include "Task.h"


// CModel 대화 상자입니다.

IMPLEMENT_DYNAMIC(CModel, CDialogEx)

CModel::CModel(CWnd* pParent /*=NULL*/)
	: CDialogEx(CModel::IDD, pParent)
	, m_iRadioModelType(FALSE)
{
	m_bShow = FALSE;

	this->Create(IDD_MODEL);

	m_pTaskHandle       = NULL;
	m_strLastModel		= _T("");

	m_bDisplay          = false;
	m_bLive             = false;
	m_bProcess          = false;

	m_hRcpMeasureThread	= NULL;
	m_bRcpMeasureEnd	= FALSE;
	m_nRcpMeasureStep   = STEP_HALT;

	m_hRcpMoveThread	= NULL;
	m_bRcpMoveEnd	    = FALSE;
	m_nRcpMoveStep      = STEP_HALT;

	m_nResultKind       = 0;
	m_clDisp.SetCurSel(0);
	m_dWaperThickness   = 1.0;
	m_dWaperExclusion   = 0.0;

	m_pModelMap         = NULL;
	m_bSetAlignModel = FALSE;
	m_bSetInsertSubPosFinish = FALSE;
	m_bPatternFinded = FALSE;
}

CModel::~CModel()
{
}

void CModel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FPSPREAD_SR_INFO, m_clSR);
	DDX_Control(pDX, IDC_LST_MODEL, m_clModel);
	DDX_Control(pDX, IDC_CBO_INCH, m_clWaperSize);
	DDX_Control(pDX, IDC_FPSPREAD_VALUE, m_clInsp);
	DDX_Control(pDX, IDC_CBO_DANI, m_clDisp);
	DDX_Control(pDX, IDC_CBO_THICKNESS, m_clThickness);
	DDX_Control(pDX, IDC_EDT_PATT_SIZE_X, m_ctrlEditPattSizeX);
	DDX_Control(pDX, IDC_EDT_PATT_SIZE_Y, m_ctrlEditPattSizeY);
	DDX_Control(pDX, IDC_EDT_PATT_GAB_X, m_ctrlEditPattGabX);
	DDX_Control(pDX, IDC_EDT_PATT_GAB_Y, m_ctrlEditPattGabY);
	DDX_Control(pDX, IDC_EDT_PATT_POS_X, m_ctrlEditPattPosX);
	DDX_Control(pDX, IDC_EDT_PATT_POS_Y, m_ctrlEditPattPosY);
	DDX_Control(pDX, IDC_CHECK_SUBPOINT, m_ctrlCheckSetSubPoint);
	DDX_Control(pDX, IDC_CHK_ALIGN_USE, m_ctrlCheckAlignUse);
	DDX_Control(pDX, IDC_LIST_MEASURE_RESULT, m_ctrlListMeasureResult);
	DDX_Control(pDX, IDC_RADIO_MODEL_TYPE1, m_ctrlRadioModelType);
	DDX_Radio(pDX, IDC_RADIO_MODEL_TYPE1, m_iRadioModelType);
}


BEGIN_MESSAGE_MAP(CModel, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_DEL_ALL, &CModel::OnBnClickedBtnDelAll)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CModel::OnBnClickedBtnSave)
	ON_LBN_DBLCLK(IDC_LST_MODEL, &CModel::OnLbnDblclkLstModel)
	ON_BN_CLICKED(IDC_BTN_ADD, &CModel::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_DEL, &CModel::OnBnClickedBtnDel)
	ON_BN_CLICKED(IDC_BTN_COPY, &CModel::OnBnClickedBtnCopy)
	ON_BN_CLICKED(IDC_BTN_RENAME, &CModel::OnBnClickedBtnRename)
	ON_BN_CLICKED(IDC_BTN_ADD_POINT, &CModel::OnBnClickedBtnAddPoint)
	ON_BN_CLICKED(IDC_BTN_RCP_INSP, &CModel::OnBnClickedBtnRcpInsp)
	ON_BN_CLICKED(IDC_CHK_PREVIEW, &CModel::OnBnClickedChkPreview)
	ON_BN_CLICKED(IDC_CHK_USE_SEL, &CModel::OnBnClickedChkUseSel)
	ON_BN_CLICKED(IDC_BTN_MAP_ADD, &CModel::OnBnClickedBtnMapAdd)
	ON_CBN_SELCHANGE(IDC_CBO_INCH, &CModel::OnCbnSelchangeCboInch)
	ON_BN_CLICKED(IDC_BTN_AF, &CModel::OnBnClickedBtnAf)
	ON_BN_CLICKED(IDC_BTN_SET_PATT, &CModel::OnBnClickedBtnSetPatt)
	ON_BN_CLICKED(IDC_BTN_FIND_PATT, &CModel::OnBnClickedBtnFindPatt)
	ON_BN_CLICKED(IDC_BTN_POINT, &CModel::OnBnClickedBtnPoint)
	ON_EN_CHANGE(IDC_EDT_THICKNESS, &CModel::OnEnChangeEdtThickness)
	ON_EN_CHANGE(IDC_EDT_EXCLUSION, &CModel::OnEnChangeEdtExclusion)

	ON_BN_CLICKED(IDC_BTN_ALIGN_MAPPING, &CModel::OnBnClickedBtnAlignMapping)
	ON_BN_CLICKED(IDC_BUTTON_GET_POS, &CModel::OnBnClickedButtonGetPos)
	ON_BN_CLICKED(IDC_BTN_ALIGN_POS_AUTO_SET, &CModel::OnBnClickedBtnAlignPosAutoSet)
	ON_BN_CLICKED(IDC_BTN_CALC_POS, &CModel::OnBnClickedBtnCalcPos)
	ON_LBN_SELCHANGE(IDC_LIST_MEASURE_RESULT, &CModel::OnLbnSelchangeListMeasureResult)
	ON_BN_CLICKED(IDC_CHK_ALIGN_USE, &CModel::OnBnClickedChkAlignUse)
	ON_BN_CLICKED(IDC_CHECK_SUBPOINT, &CModel::OnBnClickedCheckSubpoint)
	ON_BN_CLICKED(IDC_BTN_AF2, &CModel::OnBnClickedBtnAf2)
	ON_BN_CLICKED(IDC_BUTTON3, &CModel::OnBnClickedButton3)
//	ON_LBN_SELCHANGE(IDC_LST_MODEL, &CModel::OnLbnSelchangeLstModel)
ON_BN_CLICKED(IDC_BTN_INPUT_ROWCOL_AUTO_CALC, &CModel::OnBnClickedBtnInputRowcolAutoCalc)
// ON_BN_CLICKED(IDC_RADIO_MODEL_TYPE1, &CModel::OnBnClickedRadioModelType1)
ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_MODEL_TYPE1, IDC_RADIO_MODEL_TYPE4, &CModel::OnBnClickedRadioModelType)
ON_LBN_SELCHANGE(IDC_LST_MODEL, &CModel::OnLbnSelchangeLstModel)
END_MESSAGE_MAP()

BOOL CModel::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (( nID   == 2 ) &&
		( nCode == -1 ))
	{
		m_fnSetShowFlag( FALSE );
	}
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CModel::m_fnSetShowFlag( BOOL bShow )
{
	m_bShow = bShow;
	if ( m_bShow )
	{
		this->ShowWindow( SW_SHOW );
		GetDlgItem(IDC_BUTTON_GET_POS)->EnableWindow(m_ctrlCheckSetSubPoint.GetCheck());
	}
	else
	{
		this->ShowWindow( SW_HIDE );
	}
}

BOOL CModel::PreTranslateMessage(MSG* pMsg) 
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
// Log 파일을 저장한다.
//---------------------------------------------------------------------------------------------------------
void CModel::m_fnLogWrite( CString strMsg )
{
	return;
	CString strCnt("");
	int     nCnt		= 0;
	int     nTotal		= 0;

	memset( m_szData, 0x00, sizeof( m_szData ));

	sprintf_s(m_szData, "%s", strMsg.GetString());
	if ( m_pTask == NULL ) return;
	if ( m_pTask->stCommon.bLogSave )
	{
		m_Log.DEBUG_LOG( m_szData );
	}

}

//---------------------------------------------------------------------------------------------------------
// 시작에 필요한 작업을 한다.
//---------------------------------------------------------------------------------------------------------
void CModel::m_fnSetInit( ) 
{
	CString s("");
	s.Format( "MODEL 제어 메인창");
	this->SetWindowText(s);

	this->MoveWindow( 20, 100, 1160, 860, TRUE );

	m_Log.m_fnSetLogPath( "Model", "Model" );

	m_pTaskHandle = new CTaskHandle( this, SHARED_NAME, 
									1, 
									sizeof( SHEET_RESISTANCE_Format ) + 1,
									0,
									FALSE );

	m_pTask					= ( SHEET_RESISTANCE_Format* )m_pTaskHandle->m_pFilePointer( );

	m_clSR.SetMaxRows(MAX_DATA);

	CStringArray strFile;
	CString strPath("");
	CString strDirve("");
	CString strData("");

	strDirve = m_Util.m_fnGetCurrentDrive();
	strPath.Format( "%s%s%s", strDirve,
							  DEFAULT_PATH,
							  DEFAULT_MODEL_PATH );
	m_clModel.ResetContent();
	m_Util.m_fnGetFileList( strPath, "*.xml", strFile );
	for( int i = 0; i < strFile.GetSize(); i++ )
	{
		strData = strFile.GetAt(i);
		m_clModel.AddString( strData.Mid( 0, strData.GetLength() - 4) );
	}

	GetDlgItem( IDC_EDT_THICKNESS )->SetWindowText( "1" );
	GetDlgItem( IDC_EDT_EXCLUSION )->SetWindowText( "5.000" );

	m_pModelMap = new CModelMapDLG();
	m_pModelMap->m_fnSetInit();
	m_pModelMap->m_fnSetShowFlag( FALSE );

	m_pModelMap->m_clMethod.SetCurSel(0);
	m_clWaperSize.SetCurSel(0);
	m_clThickness.SetCurSel(0);
	int nFind = m_clModel.FindString( -1, m_pTask->stLastModel.szLastModel );
	if ( nFind != -1 )
	{
		m_clModel.SetCurSel( nFind );
		m_fnGetModel_Load( m_pTask->stLastModel.szLastModel );
		m_fnSetModel();
		m_strLastModel.Format( "%s", m_pTask->stLastModel.szLastModel );
	}

	for (int i = 0; i < MAX_DATA; i++)
	{
		m_clSR.SetRow(i + 1);
		m_clSR.SetRowHidden(TRUE);
	}



	m_fnSetResultDisplay( );
	((CButton*)GetDlgItem( IDC_CHK_PREVIEW ))->SetCheck(TRUE);
}

void CModel::m_fnSetParam( CString strUserID, CString strPwdID ) 
{
	CString strMsg("");
	strMsg.Format( "Login ID : %s", strUserID );
	if ( m_pTask == NULL ) return;
	if ( m_pTask->stCommon.bLogSave )
	{
		m_Log.DEBUG_LOG( strMsg.GetBuffer(strMsg.GetLength()) );
	}
}
//---------------------------------------------------------------------------------------------------------
// 종료에 필요한 작업을 한다.
//---------------------------------------------------------------------------------------------------------
void CModel::m_fnSetEnd() 
{
	if ( m_pModelMap != NULL )
	{
		m_pModelMap->m_fnSetEnd();
		delete m_pModelMap;
		m_pModelMap = NULL;
	}
	if ( m_hRcpMeasureThread != NULL )
	{
		m_bRcpMeasureEnd			= FALSE;
		SuspendThread( m_hRcpMeasureThread );
		CloseHandle(m_hRcpMeasureThread);
		m_hRcpMeasureThread	    = NULL;
	}

	if ( m_hRcpMoveThread != NULL )
	{
		m_bRcpMoveEnd			= FALSE;
		SuspendThread( m_hRcpMoveThread );
		CloseHandle(m_hRcpMoveThread);
		m_hRcpMoveThread	    = NULL;
	}

	if ( m_pTaskHandle != NULL )
	{
		m_pTaskHandle->m_fnSetEnd();
		delete m_pTaskHandle;
		m_pTaskHandle = NULL;
	}
}

BEGIN_EVENTSINK_MAP(CModel, CDialogEx)
	ON_EVENT(CModel, IDC_FPSPREAD_SR_INFO, 3, CModel::ButtonClickedFpspreadSRInfo, VTS_I4 VTS_I4 VTS_I2)
	ON_EVENT(CModel, IDC_FPSPREAD_SR_INFO, 4, CModel::ChangeFpspreadSrInfo, VTS_I4 VTS_I4)
END_EVENTSINK_MAP()

double CModel::m_fnGetAlignSafetyZ( double dThickness )
{
	return m_pTask->stCommon.dZReadySafety -  (dThickness / 1000);
}

double CModel::m_fnGetMeasureSafetyZ( double dThickness )
{
	return m_pTask->stCommon.dZMSafety -  (dThickness / 1000);
}

double CModel::m_fnGetAlignZ( double dThickness )
{
	return m_pTask->stCommon.dZAlign -  (dThickness/1000);
}

double CModel::m_fnGetMeasureZ( double dThickness )
{
	return m_pTask->stCommon.dZMesaureOffset - (dThickness / 1000);
}

void CModel::OnBnClickedBtnSave()
{
#if MIL_USE
	CString strModel("");
	CString strData("");
	CString strMsg("");

	int nNo;

	switch (m_iRadioModelType)
	{
	case MODEL_TYPE_GENERAL://일반
		nNo = m_clModel.GetCurSel();
		if (nNo == -1)
		{
			TECmsgOut(L"MODEL 명을 선택후 저장하여 주십시요.", GREEN_1, 1);
			return;
		}
		if (m_strLastModel != strModel)
		{
			strMsg.Format("최종 Load된 MODEL명(%s)과 현재 저장하려는 Model명(%s)이 상이합니다. 그대로 진행하시겠습니까?", m_strLastModel, strModel);
			if (!TECmsgOut(m_Util.CStringTobstr(strMsg), GREEN_1, 2))
				return;
		}
		m_clModel.GetText(nNo, strModel);
		break;
	case MODEL_TYPE_PATTERN://패턴
		strModel = pWorkRun->m_strPatternName;
		break;
	case MODEL_TYPE_TARGET://타겟
		strModel = pWorkRun->m_strTargetName;
		break;
	case MODEL_TYPE_SUB://서브
		strModel = pWorkRun->m_strSubName;
		break;
	default:
		break;
	}

	strMsg.Format( "%s MODEL 을  저장 하시겠습니까?", strModel );
	if(!TECmsgOut(m_Util.CStringTobstr(strMsg), GREEN_1, 2))
		return;

	m_fnModule_GetCurItemVal();
	m_fnSetModel_Save(strModel);

	theApp.m_fnGetViewPointer()->m_pAuto->m_fnGetModelList();

	switch (m_iRadioModelType)
	{
	case MODEL_TYPE_GENERAL://일반
		break;
	case MODEL_TYPE_PATTERN://패턴
		break;
	case MODEL_TYPE_TARGET://타겟
		if (!TECmsgOut(L"Target_Point 등록을 마치겠습니까?", LIGHTMAGENTA, 2))
			return;
		break;
	case MODEL_TYPE_SUB://서브
		if (!TECmsgOut(L"Sub_Point 등록을 마치겠습니까?", LIGHTMAGENTA, 2))
			return;
		break;
	default:
		break;
	}
	pWorkRun->m_bSqncInterrupt[SQITRT_REG_POSITION] = TRUE;


// 	if (m_ctrlCheckSetSubPoint.GetCheck())
// 	if (m_iRadioModelType == MODEL_TYPE_SUB)
// 	{
// 		if(TECmsgOut(L"Sub_Point 등록을 마치겠습니까?", LIGHTMAGENTA, 2))
// 			m_bSetInsertSubPosFinish = TRUE;
// 	}
// 
// 	//if (theApp.m_fnGetViewPointer()->m_pAuto->m_bPattAlignMode)
// 	if(m_iRadioModelType == MODEL_TYPE_PATTERN || m_iRadioModelType == MODEL_TYPE_TARGET)
// 		pWorkRun->m_bSqncInterrupt[SQITRT_REG_TAGET_POSITION] = TRUE;

#endif
}

 void CModel::ButtonClickedFpspreadSRInfo(long Col, long Row, short ButtonDown)
{
	CString strData("");
	double  dwPosition  = 0.0L;
	double  dwPositionX = 0.0L;
	double  dwPositionY = 0.0L;
	double  dwPositionZ = 0.0L;
	int nLen = 0;
	int i = 0;

	int tmpRow = 0;
	m_clSR.SetRow(Row);
	switch (Col)
	{
	case MEASURE_DELETE:	// 삭제
		for (i=0; i<4; i++)
		{
			m_clSR.SetCol(MEASURE_X+i);
			nLen += m_clSR.GetText().GetLength();
		}
		if (nLen > 0)
		{
			m_clSR.SetCol(MEASURE_COL);	// Colum
			m_clSR.SetText("");
			m_clSR.SetCol(MEASURE_ROW);	// Row
			m_clSR.SetText("");
			m_clSR.SetCol(MEASURE_X);	// X
			m_clSR.SetText("");
			m_clSR.SetCol(MEASURE_Y);	// Y
			m_clSR.SetText("");
		}
		else
		{
			tmpRow = Row - 1;
			while (tmpRow <= m_pTask->stModel.nSRData)
			{
				memcpy(m_pTask->stModel.stSRData[tmpRow].szName, m_pTask->stModel.stSRData[tmpRow + 1].szName, sizeof(m_pTask->stModel.stSRData[tmpRow].szName));
				m_pTask->stModel.stSRData[tmpRow].dColum = m_pTask->stModel.stSRData[tmpRow + 1].dColum;
				m_pTask->stModel.stSRData[tmpRow].iColum = m_pTask->stModel.stSRData[tmpRow + 1].iColum;
				m_pTask->stModel.stSRData[tmpRow].dRow = m_pTask->stModel.stSRData[tmpRow + 1].dRow;
				m_pTask->stModel.stSRData[tmpRow].iRow = m_pTask->stModel.stSRData[tmpRow + 1].iRow;
				m_pTask->stModel.stSRData[tmpRow].dwXPosition = m_pTask->stModel.stSRData[tmpRow + 1].dwXPosition;
				m_pTask->stModel.stSRData[tmpRow].dwYPosition = m_pTask->stModel.stSRData[tmpRow + 1].dwYPosition;

				Row = tmpRow + 1;
				m_clSR.SetRow(Row);

				m_clSR.SetCol(MEASURE_NAME);
				m_clSR.SetText(m_pTask->stModel.stSRData[Row].szName);
				m_clSR.SetCol(MEASURE_COL);	// Colum
				if (m_pTask->stModel.stSRData[Row].dColum != 0.0)
					strData.Format("%.3f", m_pTask->stModel.stSRData[Row].dColum);
				else
					strData.Format("%d", m_pTask->stModel.stSRData[Row].iColum);
				m_clSR.SetText(strData);
				m_clSR.SetCol(MEASURE_ROW);	// Row
				if (m_pTask->stModel.stSRData[Row].dRow != 0.0)
					strData.Format("%.3f", m_pTask->stModel.stSRData[Row].dRow);
				else
					strData.Format("%d", m_pTask->stModel.stSRData[Row].iRow);
				m_clSR.SetText(strData);
				m_clSR.SetCol(MEASURE_X);	// X
				strData.Format("%7.3f", m_pTask->stModel.stSRData[Row].dwXPosition);
				m_clSR.SetText(strData);
				m_clSR.SetCol(MEASURE_Y);	// Y
				strData.Format("%7.3f", m_pTask->stModel.stSRData[Row].dwYPosition);
				m_clSR.SetText(strData);

				tmpRow++;
			}
			m_clSR.SetRow(m_pTask->stModel.nSRData);
			m_clSR.SetRowHidden(TRUE);

			m_pTask->stModel.nSRData--;
		}
		break;
	case MEASURE_READ:	// 현재좌표읽기
	{
#if AJINTEX_BOARD
		m_clSR.SetCol(MEASURE_NAME);		// NAME
		strData.Format("%01d", Row);
		m_clSR.SetText(strData);

// 			if (m_ctrlCheckSetSubPoint.GetCheck())
			if (m_iRadioModelType == MODEL_TYPE_SUB)
			{
				CString strVal;
				double dStX, dStY;
				m_ctrlEditPattPosX.GetWindowTextA(strVal);
				dStX = atof(strVal);
				m_ctrlEditPattPosY.GetWindowTextA(strVal);
				dStY = atof(strVal);
				
				theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos(X_AXIS, dwPositionX);
				theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos(Y_AXIS, dwPositionY);

				CString strCalPath;
				strCalPath.Format(_T("%sModel\\%s\\"), pModelData->curDirName, pWorkRun->m_strPatternName.GetString());
				pInsp[DEF_DISP_SET]->LoadCal(strCalPath);

				pTask->inX = dwPositionX;
				pTask->inY = dwPositionY;
				GetTransformCoordinate(pTask->iGroupNum, pTask->inX, pTask->inY, &pTask->OutX, &pTask->OutY, 1);


				m_clSR.SetCol(MEASURE_COL);	// col
				strData.Format("%7.3f", pTask->OutX);
				m_clSR.SetText(strData);

				m_clSR.SetCol(MEASURE_ROW);	// col
				strData.Format("%7.3f", pTask->OutY);
				m_clSR.SetText(strData);

				m_clSR.SetCol(MEASURE_X);	// X
				strData.Format("%7.3f", dwPositionX - dStX);
				m_clSR.SetText(strData);

				m_clSR.SetCol(MEASURE_Y);	// Y
				strData.Format("%7.3f", dwPositionY - dStY);
				m_clSR.SetText(strData);

			}
			else
			{
				if (theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos(X_AXIS, dwPosition) == RC_NORMAL)
				{
					m_clSR.SetCol(MEASURE_X);	// X
					strData.Format("%7.3f", dwPosition);
					m_clSR.SetText(strData);
				}

				if (theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos(Y_AXIS, dwPosition) == RC_NORMAL)
				{
					m_clSR.SetCol(MEASURE_Y);	// Y
					strData.Format("%7.3f", dwPosition);
					m_clSR.SetText(strData);
				}
			}
#endif
		}
		break;
	case MEASURE_MOVE:	// 좌표이동
		{
			m_clSR.SetCol( MEASURE_X );	// X
			strData = m_clSR.GetText();
			dwPositionX = atof( strData );

			m_clSR.SetCol( MEASURE_Y );	// Y
			strData = m_clSR.GetText();
			dwPositionY = atof( strData );

			if ( theApp.m_fnGetViewPointer()->m_pModel->m_fnGetMeasureSafetyZ( theApp.m_fnGetViewPointer()->m_pTask->stModel.dWaperThickness ) > theApp.m_fnGetViewPointer()->m_pTask->stCommon.dZLimit )
			{
				TECmsgOut( L"Z축 Limit 위치보다 Z측정 안정위치가 낮아서 Stage와 충돌할수 있어서 이동불가 합니다.", RED_1, 1 );
				return;
			}

			if ( m_hRcpMoveThread != NULL )
			{
				SuspendThread( m_hRcpMoveThread );
				CloseHandle(m_hRcpMoveThread);
				m_hRcpMoveThread	    = NULL;
				m_bRcpMoveEnd           = FALSE;
			}

			m_dMoveX = dwPositionX;
			m_dMoveY = dwPositionY;
			unsigned long	ThreadID;
			m_bRcpMoveEnd    = TRUE;

			m_hRcpMoveThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RcpMoveThread, this, NULL, &ThreadID);
			m_nRcpMoveStep   = 10;
		}
		break;
	}
}

//------------------------------------------------------------------------
// 장비 구동에 필요한 폴더를 생성한다.
//------------------------------------------------------------------------
BOOL CModel::m_fnSetMakeFolder()
{
	CString strDirve("");

	// 현재 사용중인 드라이브 정보를 설정한다.
	strDirve = m_Util.m_fnGetCurrentDrive();

	// 환경파일 폴더를 생성한다. 
	char szFullPath[MAX_PATH + 1];
	memset( szFullPath, 0x00, sizeof( szFullPath ));

	sprintf_s(szFullPath, "%s%s%s", strDirve.GetString(),
		DEFAULT_PATH,
		DEFAULT_MODEL_PATH );
	szFullPath[MAX_PATH] = NULL;
	m_Util.m_fnSetCreateDir( szFullPath );

	return TRUE;
}

//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
CString CModel::m_fnGetModelFileFullPath( CString strModelName )
{
	char szFullPath[MAX_PATH + 1];
	memset( szFullPath, 0x00, sizeof( szFullPath ));

	sprintf_s(szFullPath, "%s%s%s%s", m_Util.m_fnGetCurrentDrive().GetString(),
									 DEFAULT_PATH,
									 DEFAULT_MODEL_PATH,
									 strModelName.GetString());
	szFullPath[MAX_PATH] = NULL;

	return szFullPath;
}

//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
CString CModel::m_fnGetStandardModelFileFullPath( )
{
	char szFullPath[MAX_PATH + 1];
	memset( szFullPath, 0x00, sizeof( szFullPath ));

	sprintf_s(szFullPath, "%s%s%s%s", m_Util.m_fnGetCurrentDrive().GetString(),
									DEFAULT_PATH,
									DEFAULT_ENV_PATH,
									DEFAULT_MODEL_FILE );
	szFullPath[MAX_PATH] = NULL;

	return szFullPath;
}

//------------------------------------------------------------------------
// 설정 환경파일을 저장한다.
//------------------------------------------------------------------------
BOOL CModel::m_fnSetModel_Save( CString strModelName )
{
	if (m_pTask == NULL) return FALSE;

	CString strWork("");
	CString strData("");
	CString strFullPath("");
	CString strBackup("");

	CStringArray strLst;
	CXmlElement *pElement     = NULL;
	CXmlElement *pRootElement = NULL;

	strFullPath		 =	m_fnGetModelFileFullPath( strModelName );

	strBackup = strFullPath;
	strBackup += "\\";
	m_Util.m_fnSetCreateDir(strBackup);
 	strBackup.Format("%s%s.XML.", strBackup, strModelName);
	strBackup       +=   m_Util.m_fnGetCurrentTime1();

	strFullPath += ".XML";
// 	strFullPath.Format("%s.XML", strFullPath);

	::rename( strFullPath, strBackup );

	theApp.m_fnGetViewPointer()->m_fnLogWrite( "MODEL Data 저장" );

	CXmlDocument* pXML = new CXmlDocument();
	pXML->Load( m_fnGetStandardModelFileFullPath() );

	pXML->GetTagToString( MODEL , strData );
	pXML->Parse( strData );
	pRootElement = pXML->GetRootElement();
	if ( pRootElement == NULL )
	{
		delete pXML;
		pXML = NULL;
		return FALSE;
	}

	strLst.RemoveAll();
	strLst.Add(ELEMENT);
	pElement = pXML->FindElement( pRootElement, strLst );
	if ( pElement != NULL )
	{
		if ( m_pTask->stModel.bFlat )
		{
			pXML->DataSet( pElement, WAPER_TYPE, STR_YES );
		}
		else
		{
			pXML->DataSet( pElement, WAPER_TYPE, STR_NO );
		}
		if (( m_pTask->stModel.nWaperSize >= WAPER_4 ) &&
			( m_pTask->stModel.nWaperSize <= WAPER_12 ))
		{
			strWork.Format( "%01d", m_pTask->stModel.nWaperSize );
		}
		else
		{
			strWork.Format( "%01d", WAPER_4 );
		}
		pXML->DataSet( pElement, WAPER_SIZE, strWork );

// 		strWork.Format( "%01d", (int)(m_pTask->stModel.dWaperThickness) );
// 		pXML->DataSet( pElement, WAPER_THICKNESS, strWork );

		strWork.Format( "%01d", m_pTask->stModel.nOutputDisp );
		pXML->DataSet( pElement, OUTPUT_DISP, strWork );
		strWork.Format("%.3f", m_pTask->stModel.dWaperThickness);
		pXML->DataSet(pElement, WAPER_THICKNESS, strWork);
		strWork.Format("%.3f", m_pTask->stModel.dEdgeExclusion);
		pXML->DataSet(pElement, EDGE_EXCLUSION, strWork);
			
		strWork.Format("%.3f", m_pTask->stModel.dPattPosX);
		pXML->DataSet(pElement, PATTERN_BASE_POSITION_X, strWork);
		strWork.Format("%.3f", m_pTask->stModel.dPattPosY);
		pXML->DataSet(pElement, PATTERN_BASE_POSITION_Y, strWork);

		strWork.Format("%.3f", m_pTask->stModel.dPattSizeX);
		pXML->DataSet(pElement, PATTERN_SIZE_X, strWork);
		strWork.Format("%.3f", m_pTask->stModel.dPattSizeY);
		pXML->DataSet(pElement, PATTERN_SIZE_Y, strWork);

		strWork.Format("%.3f", m_pTask->stModel.dPattGabX);
		pXML->DataSet(pElement, PATTERN_GAB_X, strWork);
		strWork.Format("%.3f", m_pTask->stModel.dPattGabY);
		pXML->DataSet(pElement, PATTERN_GAB_Y, strWork);
	}

	strLst.RemoveAll();
	strLst.Add(BODY);
	strLst.Add(SR_LIST);
	pElement = pXML->FindElement( pRootElement, strLst );
	if ( pElement != NULL )
	{
		for ( int i = 0; i < m_pTask->stModel.nSRData; i++ )
		{
			strWork.Format( "%s", m_pTask->stModel.stSRData[i].szName );
			strWork.Trim();
			if ( !strWork.IsEmpty() )
			{
				CXmlElement* pElement1 = pXML->AddElement( pElement, ITEM );
				pXML->AddElement( pElement1, SR_NAME, m_pTask->stModel.stSRData[i].szName );
				strWork.Format("%d", m_pTask->stModel.stSRData[i].iColum);
				pXML->AddElement(pElement1, SR_COL, strWork);
				strWork.Format("%d", m_pTask->stModel.stSRData[i].iRow);
				pXML->AddElement(pElement1, SR_ROW, strWork);
				strWork.Format("%.3f", m_pTask->stModel.stSRData[i].dColum);
				pXML->AddElement(pElement1, SR_COL_DOUBLE, strWork);
				strWork.Format("%.3f", m_pTask->stModel.stSRData[i].dRow);
				pXML->AddElement(pElement1, SR_ROW_DOUBLE, strWork);
				strWork.Format("%7.3f", m_pTask->stModel.stSRData[i].dwXPosition);
				pXML->AddElement(pElement1, SR_X, strWork);
				strWork.Format("%7.3f", m_pTask->stModel.stSRData[i].dwYPosition);
				pXML->AddElement(pElement1, SR_Y, strWork);
			}
		}
	}

	pXML->Store( strFullPath );

	delete pXML;
	pXML = NULL;

	int nNo = m_clModel.GetCurSel();
	if ( nNo != -1 )
	{
		CString strModel("");
		CString strCurrModel("");
		m_clModel.GetText( nNo, strModel );
		strModel.Trim();
		strCurrModel.Format( "%s", m_pTask->stLastModel.szLastModel );
		strCurrModel.Trim();
		if ( strCurrModel == strModel )
		{
			int nResult = TECmsgOut( L"현재작업중인 MODEL에 수정내용을 반영합니까?", GREEN_1, 2 );
			if(nResult == 1)
			{
				memcpy( &m_pTask->stCurrModel, &m_pTask->stModel, sizeof( t_MODEL_INFO ));
				theApp.m_fnGetViewPointer()->m_pAuto->OnCbnSelchangeCboModelList();
			}
		}
	}
	theApp.m_fnGetViewPointer()->m_fnLogWrite( "MODEL Data 저장 완료" );

	return TRUE;
}

//------------------------------------------------------------------------
// 설정 환경파일을 로드한다.
//------------------------------------------------------------------------
BOOL CModel::m_fnGetModel_Load( CString strModelName )
{
	CString			strWork("");
	CString			strFullPath("");
	CString			strCurrPath("");

	CStringArray strNAME;
	CStringArray strCol;
	CStringArray strRow;
	CStringArray strColDouble;
	CStringArray strRowDouble;
	CStringArray strSub;
	CStringArray strX;
	CStringArray strY;
	CStringArray strZ;
	CStringArray strMSE;
	CStringArray strRoughness;
	CStringArray strThickness;
	CStringArray strN550;
	CStringArray strK550;
	CStringArray strN632;
	CStringArray strK632;

	CStringArray strLst;
	CString      strData("");
	int          nNo = 0;

	CXmlElement *pElement     = NULL;
	CXmlElement *pRootElement = NULL;

	strFullPath = m_fnGetModelFileFullPath( strModelName );
	strFullPath                +=   ".XML";

	m_bBaseSearch = false;

	theApp.m_fnGetViewPointer()->m_fnLogWrite( "MODEL Data 읽기" );

	CString strAtoI;

	CXmlDocument* pXML = new CXmlDocument();
	pXML->Load( strFullPath );

	pXML->GetTagToString( MODEL , strData );
	pXML->Parse( strData );
	pRootElement = pXML->GetRootElement();
	if ( pRootElement == NULL )
	{
		delete pXML;
		pXML = NULL;
		return FALSE;
	}

	strLst.RemoveAll();
	strLst.Add(ELEMENT);
	pElement = pXML->FindElement( pRootElement, strLst );
	if ( pElement != NULL )
	{

		pXML->FindData( pElement, WAPER_TYPE, strWork );
		m_pTask->stModel.bFlat = FALSE;
		if ( strWork == STR_YES )
		{
			m_pTask->stModel.bFlat = TRUE;
		}
		pXML->FindData( pElement, WAPER_SIZE, strWork );
		nNo = atoi( strWork );
		if (( nNo >= WAPER_4 ) &&
			( nNo <= WAPER_12))
		{
			m_pTask->stModel.nWaperSize = nNo;
		}
		else
		{
			m_pTask->stModel.nWaperSize = WAPER_4;
		}

		pXML->FindData(pElement, EDGE_EXCLUSION, strWork);
		m_pTask->stModel.dEdgeExclusion = (double)(atof(strWork));

		pXML->FindData(pElement, WAPER_THICKNESS, strWork);
		m_pTask->stModel.dWaperThickness = (double)(atof(strWork));

		pXML->FindData( pElement, OUTPUT_DISP, strWork );
		m_pTask->stModel.nOutputDisp = atoi(strWork);

// 		m_iRadioModelType = m_pTask->stModel.iModelType;

		if(m_iRadioModelType == MODEL_TYPE_PATTERN)
		{
			pXML->FindData(pElement, PATTERN_BASE_POSITION_X, strWork);
			m_pTask->stModel.dPattPosX = (double)(atof(strWork));
			pXML->FindData(pElement, PATTERN_BASE_POSITION_Y, strWork);
			m_pTask->stModel.dPattPosY = (double)(atof(strWork));

			pXML->FindData(pElement, PATTERN_SIZE_X, strWork);
			m_pTask->stModel.dPattSizeX = (double)(atof(strWork));
			pXML->FindData(pElement, PATTERN_SIZE_Y, strWork);
			m_pTask->stModel.dPattSizeY = (double)(atof(strWork));

			pXML->FindData(pElement, PATTERN_GAB_X, strWork);
			m_pTask->stModel.dPattGabX = (double)(atof(strWork));
			pXML->FindData(pElement, PATTERN_GAB_Y, strWork);
			m_pTask->stModel.dPattGabY = (double)(atof(strWork));
		}
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
		strColDouble.RemoveAll();
		strRowDouble.RemoveAll();
		strSub.RemoveAll();
		strX.RemoveAll();
		strY.RemoveAll();
		strZ.RemoveAll();
		pXML->FindDataList( pElement, strNAME, strCol, strRow, strColDouble, strRowDouble, strSub, strX, strY, strZ);
		m_pTask->stModel.nSRData = strNAME.GetCount();
		for ( int i = 0; i < m_pTask->stModel.nSRData; i++ )
		{
			sprintf_s(m_pTask->stModel.stSRData[i].szName, "%s", strNAME.GetAt(i).GetString());

			if (strCol.GetCount() != 0)
			{
				strAtoI.Format("%s", strCol.GetAt(i));
				m_pTask->stModel.stSRData[i].iColum = atoi(strAtoI);
			}
			if (strRow.GetCount() != 0)
			{
				strAtoI.Format("%s", strRow.GetAt(i));
				m_pTask->stModel.stSRData[i].iRow = atoi(strAtoI);
			}
			if (strColDouble.GetCount() != 0)
			{
				strAtoI.Format("%s", strColDouble.GetAt(i));
				m_pTask->stModel.stSRData[i].dColum = atof(strAtoI);
			}
			if (strRowDouble.GetCount() != 0)
			{
				strAtoI.Format("%s", strRowDouble.GetAt(i));
				m_pTask->stModel.stSRData[i].dRow = atof(strAtoI);
			}
			strAtoI.Format("%s", strX.GetAt(i));
			m_pTask->stModel.stSRData[i].dwXPosition = atof(strAtoI);
			strAtoI.Format("%s", strY.GetAt(i));
			m_pTask->stModel.stSRData[i].dwYPosition = atof(strAtoI);
		}
	}

	delete pXML;
	pXML = NULL;

	m_strLastModel = strModelName;

	theApp.m_fnGetViewPointer()->m_fnLogWrite( "MODEL Data 읽기 완료" );

	return TRUE;
}

void CModel::OnLbnDblclkLstModel()
{
#if MIL_USE
	CString strData("");
	int nNo = m_clModel.GetCurSel();
	if ( nNo == -1 )
	{
		return;
	}

	CString strModel( "" );
	CString strFullPath( "" );
	m_clModel.GetText( nNo, strModel );

	strData.Format("%s MODEL 을 Load하시겠습니까?", strModel);
	int nResult = TECmsgOut(m_Util.CStringTobstr(strData), GREEN_1, 2);
	if (nResult != 1)
		return;

	switch (m_iRadioModelType)
	{
	case MODEL_TYPE_GENERAL://일반
		break;
	case MODEL_TYPE_PATTERN://패턴
		if (strModel.GetLength() <= 8 || strModel.Right(8) != "_Pattern")
		{
			TECmsgOut(L"모델명에 _Pattern 이 붙은 모델만 Load할 수 있습니다.", LIGHTMAGENTA, 1);
			m_clModel.SetCurSel(-1);
			return;
		}
		pWorkRun->m_strPatternName = strModel;
		break;
	case MODEL_TYPE_TARGET://타겟
		if (strModel.GetLength() <= 7 || strModel.Right(7) != "_Target")
		{
			TECmsgOut(L"모델명에 _Tatget 이 붙은 모델만 Load할 수 있습니다.", LIGHTMAGENTA, 1);
			m_clModel.SetCurSel(-1);
			return;
		}
		pWorkRun->m_strTargetName = strModel;
		break;
	case MODEL_TYPE_SUB://서브
		if (strModel.GetLength() <= 4 || strModel.Right(4) != "_Sub")
		{
			TECmsgOut(L"모델명에 _Sub 가 붙은 모델만 Load할 수 있습니다.", LIGHTMAGENTA, 1);
			m_clModel.SetCurSel(-1);
			return;
		}
		pWorkRun->m_strSubName = strModel;
		break;
	default:
		break;
	}

	if(nResult == 1)
	{
		m_fnModule_ModelLoad(strModel);
		pWorkRun->m_bSqncInterrupt[SQITRT_SELECT_MODEL] = TRUE;
	}
#endif
}

void CModel::m_fnModule_ModelLoad(CString &strModel)
{
	m_fnGetModel_Load(strModel);
	m_fnSetModel();
	m_strLastModel = strModel;
	sprintf_s(pModelData->modelName, sizeof(pModelData->modelName), "%s", strModel.GetString());
}

void CModel::m_fnSetModel()
{
	CString strData("");

	((CButton*)GetDlgItem( IDC_CHK_FLAT ))->SetCheck( m_pTask->stModel.bFlat );

	m_clWaperSize.SetCurSel( m_pTask->stModel.nWaperSize );
	if ( theApp.m_fnGetViewPointer()->m_pStageMap != NULL )
	{
		theApp.m_fnGetViewPointer()->m_pStageMap->m_nWaperSize = m_pTask->stModel.nWaperSize;
	}
	m_clDisp.SetCurSel(m_pTask->stModel.nOutputDisp);
	strData.Format("%7.3f", m_pTask->stModel.dWaperThickness);
	GetDlgItem(IDC_EDT_THICKNESS)->SetWindowText(strData);

	strData.Format("%7.3f", m_pTask->stModel.dEdgeExclusion);
	GetDlgItem(IDC_EDT_EXCLUSION)->SetWindowText(strData);

	if (m_iRadioModelType == MODEL_TYPE_PATTERN)
	{
		strData.Format("%.3f", m_pTask->stModel.dPattPosX);
		GetDlgItem(IDC_EDT_PATT_POS_X)->SetWindowText(strData);
		strData.Format("%.3f", m_pTask->stModel.dPattPosY);
		GetDlgItem(IDC_EDT_PATT_POS_Y)->SetWindowText(strData);

		strData.Format("%.3f", m_pTask->stModel.dPattSizeX);
		GetDlgItem(IDC_EDT_PATT_SIZE_X)->SetWindowText(strData);
		strData.Format("%.3f", m_pTask->stModel.dPattSizeY);
		GetDlgItem(IDC_EDT_PATT_SIZE_Y)->SetWindowText(strData);

		strData.Format("%.3f", m_pTask->stModel.dPattGabX);
		GetDlgItem(IDC_EDT_PATT_GAB_X)->SetWindowText(strData);
		strData.Format("%.3f", m_pTask->stModel.dPattGabY);
		GetDlgItem(IDC_EDT_PATT_GAB_Y)->SetWindowText(strData);
	}

	if ( m_pTask->stModel.nSRData > MAX_DATA )
	{
		m_pTask->stModel.nSRData = MAX_DATA;
	}

	int i = 0;
	for( i = 0; i < MAX_DATA; i++ )
	{
		m_clSR.SetRow( i + 1 );
		m_clSR.SetCol( MEASURE_NAME );
		m_clSR.SetText( "" );
		m_clSR.SetCol(MEASURE_COL);
		m_clSR.SetText("");
		m_clSR.SetCol(MEASURE_ROW);
		m_clSR.SetText("");
		m_clSR.SetCol(MEASURE_X);
		m_clSR.SetText("");
		m_clSR.SetCol( MEASURE_Y );
		m_clSR.SetText( "" );
	}

// 	for( i = 0; i < m_pTask->stModel.nSRData; i++ )
// 	{
// 		m_clSR.SetRow( i + 1 );
// 		//m_clSR.SetRowHidden(TRUE);
// 
// 		m_clSR.SetCol( MEASURE_NAME );
// 		m_clSR.SetText( m_pTask->stModel.stSRData[i].szName );
// 
// 		m_clSR.SetCol(MEASURE_COL);
// 		if(m_pTask->stModel.stSRData[i].dColum != 0.0)
// 			strData.Format("%.3f", m_pTask->stModel.stSRData[i].dColum);
// 		else
// 			strData.Format("%d", m_pTask->stModel.stSRData[i].iColum);
// 
// 		m_clSR.SetText(strData);
// 
// 		m_clSR.SetCol(MEASURE_ROW);
// 		if (m_pTask->stModel.stSRData[i].dRow != 0.0)
// 			strData.Format("%.3f", m_pTask->stModel.stSRData[i].dRow);
// 		else
// 			strData.Format("%d", m_pTask->stModel.stSRData[i].iRow);
// 		m_clSR.SetText(strData);
// 
// 		m_clSR.SetCol( MEASURE_X );
// 		strData.Format( "%7.3f", m_pTask->stModel.stSRData[i].dwXPosition );
// 		m_clSR.SetText(strData);
// 
// 		m_clSR.SetCol( MEASURE_Y );
// 		strData.Format( "%7.3f", m_pTask->stModel.stSRData[i].dwYPosition );
// 		m_clSR.SetText( strData );
// 	}

	for (i = 0; i < MAX_DATA; i++)
	{
		m_clSR.SetRow(i + 1);

		if (i < m_pTask->stModel.nSRData)
		{
			m_clSR.SetRowHidden(FALSE);
		}
		else
		{
			m_clSR.SetRowHidden(TRUE);
			continue;
		}		

		m_clSR.SetCol(MEASURE_NAME);
		m_clSR.SetText(m_pTask->stModel.stSRData[i].szName);

		m_clSR.SetCol(MEASURE_COL);
		if (m_pTask->stModel.stSRData[i].dColum != 0.0)
			strData.Format("%.3f", m_pTask->stModel.stSRData[i].dColum);
		else
			strData.Format("%d", m_pTask->stModel.stSRData[i].iColum);

		m_clSR.SetText(strData);

		m_clSR.SetCol(MEASURE_ROW);
		if (m_pTask->stModel.stSRData[i].dRow != 0.0)
			strData.Format("%.3f", m_pTask->stModel.stSRData[i].dRow);
		else
			strData.Format("%d", m_pTask->stModel.stSRData[i].iRow);
		m_clSR.SetText(strData);

		m_clSR.SetCol(MEASURE_X);
		strData.Format("%7.3f", m_pTask->stModel.stSRData[i].dwXPosition);
		m_clSR.SetText(strData);

		m_clSR.SetCol(MEASURE_Y);
		strData.Format("%7.3f", m_pTask->stModel.stSRData[i].dwYPosition);
		m_clSR.SetText(strData);
	}


// 	m_ctrlCheckAlignUse.SetCheck(m_pTask->stModel.bUseAlign);
}


void CModel::OnBnClickedBtnAdd()
{
#if MIL_USE
	CString strData("");
	CModelManage dlg;
	dlg.m_fnInit( 1, "" );
	if ( dlg.DoModal() == IDOK )
	{
		strData = dlg.m_strMODEL;
		strData.Trim();
		strData.Replace( ' ', '_' );
		if ( !strData.IsEmpty() )
		{
			int nNo = m_clModel.FindString( 0, strData );
			if ( nNo == -1 )
			{
			
				switch (m_iRadioModelType)
				{
				case MODEL_TYPE_GENERAL://일반
					break;
				case MODEL_TYPE_PATTERN://패턴
					if (!(strData.GetLength() > 8 && strData.Left(8) == "Pattern_"))
					{
						strData.Insert(0, "Pattern_");
					}
					pWorkRun->m_strPatternName = strData;
					break;
				case MODEL_TYPE_TARGET://타겟
					if (!(strData.GetLength() > 7 && strData.Left(7) == "Target_"))
					{
						strData.Insert(0, "Target_");
					}
					pWorkRun->m_strTargetName = strData;
					break;
				case MODEL_TYPE_SUB://서브
					if (!(strData.GetLength() > 4 && strData.Left(4) == "Sub_"))
					{
						strData.Insert(0, "Sub_");
					}
					pWorkRun->m_strSubName = strData;
					break;
				default:
					break;
				}
				sprintf_s(pModelData->modelName, sizeof(pModelData->modelName), "%s", strData.GetString());
				m_clModel.AddString(strData);
				theApp.m_fnGetViewPointer()->m_pAuto->m_fnGetModelList();
				pWorkRun->m_bSqncInterrupt[SQITRT_SELECT_MODEL] = TRUE;
			}
			else
			{
				strData = "이미 존재하는 Model입니다. 신규모델로 등록할수 없습니다.";
				theApp.m_fnGetViewPointer()->m_fnLogWrite( strData );
				TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 1 );
			}
		}
	}
#endif
}


void CModel::OnBnClickedBtnDel()
{
	CString strDel("");
	CString strData("");
	int nNo = m_clModel.GetCurSel();
	if ( nNo == -1 )
	{
		TECmsgOut( L"삭제 대상 모델을 선택해 주십시요.", GREEN_1, 1 );
		return;
	}
	m_clModel.GetText( nNo, strDel );
	strData.Format( "%s MODEL 을 삭제해도 되겠습니까?", strDel );
	int nResult = TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 2 );
	if(nResult == 1)
	{
		strData.Format( "%s", m_pTask->stLastModel.szLastModel );
		if ( strDel != strData )
		{
			m_clModel.DeleteString( nNo );
			strDel += ".XML";
			::DeleteFile( m_fnGetModelFileFullPath( strDel ) );
			theApp.m_fnGetViewPointer()->m_pAuto->m_fnGetModelList();
		}
		else
		{
			strData = "사용중인 Model입니다. 삭제할수 없습니다.";
			theApp.m_fnGetViewPointer()->m_fnLogWrite( strData );
			TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 1 );
		}
	}
}

void CModel::OnBnClickedBtnCopy()
{
	CString strData("");
	CString strData1("");
	CString strCopy("");
	int nNo = m_clModel.GetCurSel();
	if ( nNo == -1 )
	{
		TECmsgOut( L"복사 대상 MODEL 을 선택해 주십시요.", GREEN_1, 1 );
		return;
	}
	m_clModel.GetText( nNo, strCopy );

	CModelManage dlg;

	dlg.m_fnInit( 2, strCopy );
	if ( dlg.DoModal() == IDOK )
	{
		strData1 = dlg.m_strNewMODEL;
		strData = dlg.m_strMODEL;
		strData.Trim();
		strData1.Trim();
		if ( strData == strData1 )
		{
			strData = "복사원본과 복사대상의 MODEL명이 동일합니다. 복사대상명을 다른이름으로 입력해 주세요.";
			theApp.m_fnGetViewPointer()->m_fnLogWrite( strData );
			return;
		}
		int nNo = m_clModel.FindString( 0, strData1 );
		if ( nNo != -1) 
		{
			strData = "복사대상 Model 이 존재합니다. 복사대상명을 다른이름으로 입력해 주세요.";
			theApp.m_fnGetViewPointer()->m_fnLogWrite( strData );
			TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 1 );
			return;
		}
		if (( !strData.IsEmpty() ) &&
			( !strData1.IsEmpty() ))
		{
			strData1.Replace( ' ', '_' );
			m_clModel.AddString( strData1 );
			strData += ".XML";
			strData1 += ".XML";
			::CopyFile( m_fnGetModelFileFullPath( strData ),  m_fnGetModelFileFullPath( strData1 ), false );
			theApp.m_fnGetViewPointer()->m_pAuto->m_fnGetModelList();
		}
	}
}


void CModel::OnBnClickedBtnRename()
{
	CString strData("");
	CString strData1("");
	CString strModify("");
	int nNo = m_clModel.GetCurSel();
	if ( nNo == -1 )
	{
		TECmsgOut( L"변경 대상 MODEL 을 선택해 주십시요.", GREEN_1, 1 );
		return;
	}
	m_clModel.GetText( nNo, strModify );

	strData.Format( "%s", m_pTask->stLastModel.szLastModel );
	if ( strModify == strData ) 
	{
		strData = "변경대상 Model 이 사용중입니다. 변경불가합니다.";
		theApp.m_fnGetViewPointer()->m_fnLogWrite( strData );
		TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 1 );
		return;
	}

	CModelManage dlg;

	dlg.m_fnInit( 2, strModify );
	if ( dlg.DoModal() == IDOK )
	{
		strData1 = dlg.m_strNewMODEL;
		strData = dlg.m_strMODEL;
		strData.Trim();
		strData1.Trim();
		if (( !strData.IsEmpty() ) &&
			( !strData1.IsEmpty() ))
		{
			m_clModel.DeleteString( m_clModel.FindString( -1, strData ));
			strData1.Replace( ' ', '_' );
			m_clModel.AddString( strData1 );
			strData += ".XML";
			strData1 += ".XML";
			::rename( m_fnGetModelFileFullPath( strData ),  m_fnGetModelFileFullPath( strData1 ) );
			theApp.m_fnGetViewPointer()->m_pAuto->m_fnGetModelList();
		}
	}
}

//------------------------------------------------------------------------
// 설정 환경파일을 로드한다.
//------------------------------------------------------------------------
BOOL CModel::m_fnGetCurrModel_Load( CString strModelName )
{
	CString			strWork("");
	CString			strFullPath("");
	CString			strCurrPath("");

	CStringArray strNAME;
	CStringArray strCol;
	CStringArray strRow;
	CStringArray strColDouble;
	CStringArray strRowDouble;
	CStringArray strSub;
	CStringArray strX;
	CStringArray strY;
	CStringArray strZ;
	CStringArray strMSE;
	CStringArray strRoughness;
	CStringArray strThickness;
	CStringArray strN550;
	CStringArray strK550;
	CStringArray strN632;
	CStringArray strK632;

	CStringArray strLst;
	CString      strData("");

	CXmlElement *pElement     = NULL;
	CXmlElement *pRootElement = NULL;

	strFullPath = m_fnGetModelFileFullPath( strModelName );
	strFullPath                +=   ".XML";

	if ( m_Util.Existence(strFullPath) != FM_FILE )
	{
		return FALSE;
	}

	CXmlDocument* pXML = new CXmlDocument();
	pXML->Load( strFullPath );

	pXML->GetTagToString( MODEL , strData );
	pXML->Parse( strData );
	pRootElement = pXML->GetRootElement();
	if ( pRootElement == NULL )
	{
		delete pXML;
		pXML = NULL;
		return FALSE;
	}

	strLst.RemoveAll();
	strLst.Add(ELEMENT);
	pElement = pXML->FindElement( pRootElement, strLst );
	if ( pElement != NULL )
	{
		pXML->FindData( pElement, WAPER_TYPE, strWork );
		m_pTask->stCurrModel.bFlat = FALSE;
		if ( strWork == STR_YES )
		{
			m_pTask->stCurrModel.bFlat = TRUE;
		}

		pXML->FindData( pElement, WAPER_SIZE, strWork );
		m_pTask->stCurrModel.nWaperSize = atoi( strWork );
		if (( m_pTask->stCurrModel.nWaperSize < WAPER_4 ) ||
			( m_pTask->stCurrModel.nWaperSize > WAPER_12 ))
		{
			m_pTask->stCurrModel.nWaperSize = WAPER_4;
		}

		pXML->FindData( pElement, OUTPUT_DISP, strWork );
		m_pTask->stCurrModel.nOutputDisp = atoi(strWork);
		pXML->FindData(pElement, WAPER_THICKNESS, strWork);
		m_pTask->stCurrModel.dWaperThickness = (double)(atof(strWork));
		pXML->FindData(pElement, EDGE_EXCLUSION, strWork);
		m_pTask->stCurrModel.dEdgeExclusion = (double)(atof(strWork));
	}

	CString strAtoI;
	strLst.RemoveAll();
	strLst.Add(BODY);
	strLst.Add(SR_LIST);
	pElement = pXML->FindElement( pRootElement, strLst );
	if ( pElement != NULL )
	{
		strNAME.RemoveAll();
		strCol.RemoveAll();
		strRow.RemoveAll();
		strColDouble.RemoveAll();
		strRowDouble.RemoveAll();
		strSub.RemoveAll();
		strX.RemoveAll();
		strY.RemoveAll();
		strZ.RemoveAll();
		pXML->FindDataList(pElement, strNAME, strCol, strRow, strColDouble, strRowDouble, strSub, strX, strY, strZ);
		m_pTask->stCurrModel.nSRData = strNAME.GetSize();
		for ( int i = 0; i < strNAME.GetSize(); i++ )
		{
			sprintf_s(m_pTask->stCurrModel.stSRData[i].szName, "%s", strNAME.GetAt(i).GetString());

			if (strCol.GetCount() != 0)
			{
				strAtoI.Format("%s", strCol.GetAt(i));
				m_pTask->stCurrModel.stSRData[i].iColum = atoi(strAtoI);
			}
			if (strRow.GetCount() != 0)
			{
				strAtoI.Format("%s", strRow.GetAt(i));
				m_pTask->stCurrModel.stSRData[i].iRow = atoi(strAtoI);
			}
			if (strColDouble.GetCount() != 0)
			{
				strAtoI.Format("%s", strColDouble.GetAt(i));
				m_pTask->stCurrModel.stSRData[i].dColum = atof(strAtoI);
			}
			if (strRowDouble.GetCount() != 0)
			{
				strAtoI.Format("%s", strRowDouble.GetAt(i));
				m_pTask->stCurrModel.stSRData[i].dRow = atof(strAtoI);
			}
			strAtoI.Format("%s", strX.GetAt(i));
			m_pTask->stCurrModel.stSRData[i].dwXPosition = atof(strAtoI);
			strAtoI.Format("%s", strY.GetAt(i));
			m_pTask->stCurrModel.stSRData[i].dwYPosition = atof(strAtoI);

		}
	}

	delete pXML;
	pXML = NULL;

	return TRUE;
}

UINT RcpMeasureThread(LPVOID pFuncData)
{
#if MIL_USE
	pWorkRun->m_bThrdFlag[TF_AUTORUN] = TRUE;

	if (!pWorkRun->SqncReadyMeasure(FALSE))
	{
		TECmsgOut(L"엘립소메타 연결상태를 확인해주세요.", LIGHTMAGENTA, 1);
		pWorkRun->SqncCloseMeasure();
		pWorkRun->m_bThrdFlag[TF_AUTORUN] = FALSE;
		return 0;
	}

	t_ELIPSO_RESULT_INFO stcResultAlign;
	pWorkRun->SqncMeasure(stcResultAlign);

	pWorkRun->SqncCloseMeasure();
	pWorkRun->m_bThrdFlag[TF_AUTORUN] = FALSE;
#endif


	CModel		*pQI = (CModel *)pFuncData;
	pQI->m_bProcess = FALSE;
	if (pQI->m_hWnd == NULL) return 0;

	if (pQI->m_hRcpMeasureThread != NULL)
	{
		SuspendThread(pQI->m_hRcpMeasureThread);
		CloseHandle(pQI->m_hRcpMeasureThread);
		pQI->m_hRcpMeasureThread = NULL;
	}
	return 0;


	DWORD   dwBusyStatus = 0;
	DWORD   dwRtnCode    = 0;
	double  dMove        = 0.0;
	double  dSpeed       = 0.0;
	int     nWaitTime    = 0;
	CString strData("");
	while ( pQI->m_bRcpMeasureEnd )
	{
		if (!pQI->m_pTask->stElipsoIO.stIn.EMS_1
			|| !pQI->m_pTask->stElipsoIO.stIn.EMS_2
			|| !pQI->m_pTask->stElipsoIO.stIn.EMS_3
			|| !pQI->m_pTask->stElipsoIO.stIn.EMS_4
			)
		{
			pQI->m_nRcpMeasureStep    = STEP_HALT;
			pQI->m_bRcpMeasureEnd = FALSE;
			theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmMoveAllStop();
			continue;
		}
		if ( pQI->m_hWnd != NULL )
		{
#if AJINTEX_BOARD
			switch ( pQI->m_nRcpMeasureStep )
			{
				case 10:
					{
						dwRtnCode = theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmStatusReadInMotion( Z_AXIS, dwBusyStatus );
						if ( dwBusyStatus == 0 )
						{
							dMove  = pQI->m_fnGetMeasureSafetyZ( pQI->m_pTask->stModel.dWaperThickness );
							dSpeed = pQI->m_pTask->stCommon.dSpeedHighZ;
							theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmMotSetAbsRelMode( Z_AXIS, POS_ABS_MODE );
							theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmMoveStartPos( Z_AXIS, dMove, dSpeed, dSpeed * 2, dSpeed * 2 );
							theApp.m_fnGetViewPointer()->m_fnLogWrite( "Model : 수동측정 안전위치 이동시작" );
							pQI->m_nRcpMeasureStep = 20;
						}
					}
					break;
				case 20:
					{
						dwRtnCode = theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmStatusReadInMotion( Z_AXIS, dwBusyStatus );
						if ( dwBusyStatus == 0 )
						{
							theApp.m_fnGetViewPointer()->m_fnLogWrite( "Model : 수동측정 안전위치 이동완료" );
							dMove  = pQI->m_fnGetMeasureZ( pQI->m_pTask->stModel.dWaperThickness );
							dSpeed = pQI->m_pTask->stCommon.dInspZSpeed;
							theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmMotSetAbsRelMode( Z_AXIS, POS_ABS_MODE );
							theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmMoveStartPos( Z_AXIS, dMove, dSpeed, dSpeed * 2, dSpeed * 2 );
							theApp.m_fnGetViewPointer()->m_fnLogWrite( "Model : 수동측정 측정위치 이동시작" );
							pQI->m_nRcpMeasureStep = 30;
						}
					}
					break;
				case 30:
					{
						dwRtnCode = theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmStatusReadInMotion( Z_AXIS, dwBusyStatus );
						if ( dwBusyStatus == 0 )
						{
							nWaitTime              = 0;
							theApp.m_fnGetViewPointer()->m_fnLogWrite( "Model : 수동측정 측정위치 이동완료" );
							pQI->m_nRcpMeasureStep = 40;
						}
					}
					break;
				case 40:
					{
						if ( nWaitTime >= 200 )
						{
							theApp.m_fnGetViewPointer()->m_fnLogWrite( "Model : 수동측정 측정 Start" );
							pQI->m_nRcpMeasureStep = 50;
						}
						else
						{
							nWaitTime += AUTO_TIME;
						}
					}
					break;
				case 50:
					{
						pQI->m_nRcpMeasureStep = 60;
					}
					break;
				case 60:
					{
						dwRtnCode = theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmStatusReadInMotion( Z_AXIS, dwBusyStatus );
						if ( dwBusyStatus == 0 )
						{
							dMove  = 0;
							dSpeed = pQI->m_pTask->stCommon.dSpeedHighZ;
							theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmMotSetAbsRelMode( Z_AXIS, POS_ABS_MODE );
							theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmMoveStartPos( Z_AXIS, dMove, dSpeed, dSpeed * 2, dSpeed * 2 );
							theApp.m_fnGetViewPointer()->m_fnLogWrite( "Model : 수동측정 원점위치 이동시작" );
							pQI->m_nRcpMeasureStep = 70;
						}
					}
					break;
				case 70:
					{
						dwRtnCode = theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmStatusReadInMotion( Z_AXIS, dwBusyStatus );
						if ( dwBusyStatus == 0 )
						{
							theApp.m_fnGetViewPointer()->m_fnLogWrite( "Model : 수동측정 원점위치 이동완료" );
							nWaitTime              = 0;
							pQI->m_nRcpMeasureStep = STEP_HALT;
							pQI->m_bRcpMeasureEnd = FALSE;
							theApp.m_fnGetViewPointer()->m_fnLogWrite( strData );
							TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 1 );
							pQI->m_bProcess = FALSE;
						}
					}
					break;
			}
#endif
			Sleep( AUTO_TIME );
		}
	}

	if ( pQI->m_hWnd == NULL ) return 0;

	if ( pQI->m_hRcpMeasureThread != NULL )
	{
		SuspendThread(pQI->m_hRcpMeasureThread);
		CloseHandle(pQI->m_hRcpMeasureThread);
		pQI->m_hRcpMeasureThread	= NULL;
	}
	return 0;
}

UINT RcpMoveThread(LPVOID pFuncData)
{
	CModel		*pQI = (CModel *)pFuncData;
	DWORD   dwBusyStatus  = 0;
	DWORD   dwRtnCode     = 0;
	DWORD   dwBusyStatus1 = 0;
	DWORD   dwRtnCode1    = 0;
	DWORD   dwBusyStatus2 = 0;
	DWORD   dwRtnCode2    = 0;
	double  dMove         = 0.0;
	double  dSpeed        = 0.0;
	CString strData("");
	while ( pQI->m_bRcpMoveEnd )
	{
		if (!pQI->m_pTask->stElipsoIO.stIn.EMS_1
			|| !pQI->m_pTask->stElipsoIO.stIn.EMS_2
			|| !pQI->m_pTask->stElipsoIO.stIn.EMS_3
			|| !pQI->m_pTask->stElipsoIO.stIn.EMS_4
			)
		{
			pQI->m_nRcpMoveStep    = STEP_HALT;
			pQI->m_bRcpMoveEnd     = FALSE;
			theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmMoveAllStop();
			continue;
		}
		if ( pQI->m_hWnd != NULL )
		{
#if AJINTEX_BOARD
			switch ( pQI->m_nRcpMoveStep )
			{
			case 10:
				{
					dwRtnCode = theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmStatusReadInMotion( Z_AXIS, dwBusyStatus );
					if ( dwBusyStatus == 0 )
					{
						dMove  = pQI->m_fnGetMeasureSafetyZ( pQI->m_pTask->stModel.dWaperThickness );
						dSpeed = pQI->m_pTask->stCommon.dSpeedHighZ;
						theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmMotSetAbsRelMode( Z_AXIS, POS_ABS_MODE );
// 						theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmMoveStartPos( Z_AXIS, dMove, dSpeed, dSpeed * 2, dSpeed * 2 );
						theApp.m_fnGetViewPointer()->m_fnLogWrite( "Model : 안전위치 이동시작" );
						pQI->m_nRcpMoveStep = 20;
					}
				}
				break;
			case 20:
				{
					dwRtnCode = theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmStatusReadInMotion( Z_AXIS, dwBusyStatus );
					if ( dwBusyStatus == 0 )
					{
						theApp.m_fnGetViewPointer()->m_fnLogWrite( "Model : 안전위치 이동완료" );
						dSpeed = pQI->m_pTask->stCommon.dSpeedHigh;
						theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmMotSetAbsRelMode( X_AXIS, POS_ABS_MODE );
						theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmMotSetAbsRelMode( Y_AXIS, POS_ABS_MODE );
						theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmMoveStartPos( X_AXIS, pQI->m_dMoveX, dSpeed, dSpeed * 2, dSpeed * 2 );
						theApp.m_fnGetViewPointer( )->m_pAjin->m_fnSetAxmMoveStartPos( Y_AXIS, pQI->m_dMoveY, dSpeed, dSpeed * 2, dSpeed * 2 );
						theApp.m_fnGetViewPointer()->m_fnLogWrite( "Model : 위치 이동시작" );
						pQI->m_nRcpMoveStep = 30;
					}
				}
				break;
			case 30:
				{
					dwRtnCode1 = theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmStatusReadInMotion( X_AXIS, dwBusyStatus1 );
					dwRtnCode2 = theApp.m_fnGetViewPointer( )->m_pAjin->m_fnGetAxmStatusReadInMotion( Y_AXIS, dwBusyStatus2 );
					if (( dwBusyStatus1 == 0 ) &&
					    ( dwBusyStatus2 == 0 ))
					{
						theApp.m_fnGetViewPointer()->m_fnLogWrite( "Model : 위치 이동완료" );
						pQI->m_nRcpMoveStep = STEP_HALT;
						pQI->m_bRcpMoveEnd = FALSE;

					}
				}
				break;
			}
#endif
			Sleep( AUTO_TIME );
		}
	}

	if ( pQI->m_hWnd == NULL ) return 0;

	if ( pQI->m_hRcpMoveThread != NULL )
	{
		SuspendThread(pQI->m_hRcpMoveThread);
		CloseHandle(pQI->m_hRcpMoveThread);
		pQI->m_hRcpMoveThread	= NULL;
	}
	return 0;
}

void CModel::OnBnClickedBtnAddPoint()
{
	int nRow = 0;
	CString strName("");
	CString strMsg("");
	CString strCol("0");
	CString strRow("0");
	CString strX("0.000");
	CString strY("0.000");
	CString strZ("0.000");

	double dwPositionX = 0L;
	double dwPositionY = 0L;
	double dwPositionZ = 0L;

	nRow = m_pTask->stModel.nSRData + 1;
	m_clSR.SetRow(nRow);
	m_clSR.SetRowHidden(FALSE);
	m_clSR.SetCol( MEASURE_NAME );
	strName.Format("%01d", nRow );
	m_clSR.SetText( strName );
// 	if (m_ctrlCheckAlignUse)
	if (m_iRadioModelType == MODEL_TYPE_TARGET)
	{
		m_clSR.SetCol(MEASURE_COL);
		m_clSR.SetText(strCol);
		m_clSR.SetCol(MEASURE_ROW);
		m_clSR.SetText(strRow);
	}
	else
	{
#if AJINTEX_BOARD
		CString strData("");
		if (theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos(X_AXIS, dwPositionX) == RC_NORMAL)
		{
			strX.Format("%7.3f", dwPositionX);
		}
		if (theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos(Y_AXIS, dwPositionY) == RC_NORMAL)
		{
			strY.Format("%7.3f", dwPositionY);
		}
#endif
		m_clSR.SetCol(MEASURE_X);
		m_clSR.SetText(strX);
		m_clSR.SetCol(MEASURE_Y);
		m_clSR.SetText(strY);
	}
 	m_pTask->stModel.stSRData[m_pTask->stModel.nSRData].dwXPosition = dwPositionX;
 	m_pTask->stModel.stSRData[m_pTask->stModel.nSRData].dwYPosition = dwPositionY;
/*
	m_pTask->stModel.stSRData[m_pTask->stModel.nSRData].dwResult[VAL_R]  = 0.0L;
	m_pTask->stModel.stSRData[m_pTask->stModel.nSRData].dwResult[VAL_S]  = 0.0L;
	m_pTask->stModel.stSRData[m_pTask->stModel.nSRData].dwResult[VAL_V]  = 0.0L;
	m_pTask->stModel.stSRData[m_pTask->stModel.nSRData].dwResult[VAL_C]  = 0.0L;
*/	strName.Format( "%01d", m_pTask->stModel.nSRData + 1 );
	sprintf_s(m_pTask->stModel.stSRData[m_pTask->stModel.nSRData].szName, "%s", strName.GetString());
 	m_pTask->stModel.nSRData++;
	return;
}

void CModel::OnBnClickedBtnRcpInsp()
{
	if ( m_bProcess ) return;

	if ( m_hRcpMeasureThread != NULL )
	{
		m_bRcpMeasureEnd			= FALSE;
		SuspendThread( m_hRcpMeasureThread );
		CloseHandle(m_hRcpMeasureThread);
		m_hRcpMeasureThread	    = NULL;
	}

	m_bProcess          = TRUE;

	CString strData("");
	GetDlgItem( IDC_EDT_THICKNESS )->GetWindowText( strData );
	m_pTask->stModel.dWaperThickness = atof( strData );
	GetDlgItem(IDC_EDT_EXCLUSION)->GetWindowText(strData);
	m_pTask->stModel.dEdgeExclusion = atof(strData);

	unsigned long	ThreadID;
	m_bRcpMeasureEnd    = TRUE;

	m_hRcpMeasureThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RcpMeasureThread, this, NULL, &ThreadID);
	m_nRcpMeasureStep   = 10;
}

void CModel::m_fnSetResultDisplay( )
{
	int nDani[MAX_ITEM];
	double dResult[MAX_ITEM];
	memset( nDani, 0x00, sizeof(nDani));
	memset( dResult, 0x00, sizeof(dResult));

	CString strResult("");
	m_clInsp.SetRow( 1 );
	m_clInsp.SetCol( 3 );
/*
	if (( nDani[VAL_R] < 0 ) ||
		( nDani[VAL_R] > 5 ))
	{
		nDani[VAL_R] = 0;
	}
	if (( nDani[VAL_S] < 0 ) ||
		( nDani[VAL_S] > 5 ))
	{
		nDani[VAL_S] = 0;
	}
	if (( nDani[VAL_V] < 0 ) ||
		( nDani[VAL_V] > 5 ))
	{
		nDani[VAL_V] = 0;
	}
	if (( nDani[VAL_C] < 0 ) ||
		( nDani[VAL_C] > 5 ))
	{
		nDani[VAL_C] = 0;
	}
	strResult.Format( "%.2f %s", dResult[VAL_R], strDani_n[nDani[VAL_R]]);
	m_clInsp.SetText( strResult );
	m_clInsp.SetCol( 5 );
	strResult.Format( "%.2f %s", dResult[VAL_S], strDani_k[nDani[VAL_S]]);
	m_clInsp.SetText( strResult );
	m_clInsp.SetCol( 7 );
	strResult.Format( "%.2f %s", dResult[VAL_V], strDaniThickness[nDani[VAL_V]]);
	m_clInsp.SetText( strResult );
	m_clInsp.SetCol( 9 );
	strResult.Format( "%.2f %s", dResult[VAL_C], strDaniC[nDani[VAL_C]]);
	m_clInsp.SetText( strResult );
*/
}

void CModel::OnBnClickedBtnDelAll()
{
	CString strData("");
	strData.Format( "정말로 모든 데이터를 삭제하시겠습니까?" );
	int nResult = TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 2 );
	if ( nResult == 1 )
	{
		m_fnSRSheetClear();
 		m_pTask->stModel.nSRData = 0;
 		memset( m_pTask->stModel.stSRData, 0x00, sizeof(t_DATA));
	}
}


void CModel::OnBnClickedChkPreview()
{
	BOOL bPreView     = ((CButton*)GetDlgItem( IDC_CHK_PREVIEW ))->GetCheck();
	theApp.m_fnGetViewPointer()->m_pStageMap->m_bPreView = bPreView;
}

void CModel::OnBnClickedChkUseSel()
{
	BOOL bAllSelect =  ((CButton*)GetDlgItem( IDC_CHK_USE_SEL ))->GetCheck();
	for(int i = 0; i < m_pTask->stModel.nSRData; i++ )
	{
		m_clSR.SetCol(1);
		m_clSR.SetRow(i + 1);
		m_clSR.SetValue("0");
		if ( bAllSelect )
		{
			m_clSR.SetValue("1");
		}
	}
}


void CModel::OnBnClickedBtnMapAdd()
{
	if ( m_pModelMap == NULL )
	{
		m_pModelMap = new CModelMapDLG();
	}
	m_pModelMap->m_fnSetInit();
	m_pModelMap->m_fnSetShowFlag(!m_pModelMap->m_bShow );
}


void CModel::OnCbnSelchangeCboInch()
{
	theApp.m_fnGetViewPointer()->m_pStageMap->m_nWaperSize = m_clWaperSize.GetCurSel();
}


void CModel::OnBnClickedBtnAf()
{
	if (!theApp.m_fnGetViewPointer()->m_pStageMap->m_bCamera)
	{
		((CButton*)theApp.m_fnGetViewPointer()->m_pStageMap->GetDlgItem(IDC_CHK_CAMERA))->SetCheck(TRUE);
		theApp.m_fnGetViewPointer()->m_pStageMap->OnBnClickedChkCamera();
	}
#if MIL_USE
	m_pTask->stResult.dWaperThickness = m_dWaperThickness;
	m_pTask->stResult.dEdgeExclusion = m_dWaperExclusion;

	thread thrdStartAuto = thread(ThrdAF, TRUE);
	thrdStartAuto.detach();
#endif
	return;
}


void CModel::OnBnClickedBtnSetPatt()
{
#if MIL_USE
// 	theApp.m_fnGetViewPointer()->m_pStageMap->m_fnCheckCamera(TRUE);

	if (pCamJAI != NULL)
	{
		pCamJAI->m_fnSetShowFlag(TRUE);
	}
	
	pMILAlignDlg->m_fnTranferCamBuff2PattBuff(theApp.m_fnGetViewPointer()->m_pAuto->m_bPattAlignMode);
#endif
}


// void CModel::m_fnTranferCamBuff2PattBuff(BOOL bAutoPattAlignMode /*= FALSE*/)`
// {
// #if JAI_CAMERA
// 	pCamJAI->m_bLockedGrabBuf = TRUE;
// 	theApp.m_fnGetViewPointer()->m_pStageMap->m_fnCheckCamera(TRUE);
// 
// 	if(bAutoPattAlignMode)
// 		pMILAlignDlg->GetDlgItem(IDOK)->EnableWindow(FALSE);
// 	pMILAlignDlg->ShowWindow(SW_SHOW);
// 	pMILAlignDlg->m_fnBringImage(pCamJAI->m_CnvImageInfo.pImageBuffer, pCamJAI->m_CnvImageInfo.iImageSize);
// 	pCamJAI->m_bLockedGrabBuf = FALSE;
// #endif
// }

void CModel::OnBnClickedBtnFindPatt()
{
	m_fnModule_FindPatt();

	pWorkRun->m_bSqncInterrupt[SQITRT_FIND_PATTERN] = TRUE;
}


void CModel::m_fnModule_FindPatt()
{
#if JAI_CAMERA
	theApp.m_fnGetViewPointer()->m_pStageMap->m_fnCheckCamera(TRUE);

	pTask->PatternNum = 0;

	// 	memcpy(pTask->ImageBuff[pTask->PatternNum], pCamJAI->m_CnvImageInfo.pImageBuffer, sizeof(unsigned char)*DEF_IMAGE_X*DEF_IMAGE_Y);
	// 	pMILAlignDlg->m_fnTranferCamBuff2PattBuff(theApp.m_fnGetViewPointer()->m_pAuto->m_bPattAlignMode);
	pMILAlignDlg->m_fnBringImage(pCamJAI->m_CnvImageInfo.pImageBuffer, pCamJAI->m_CnvImageInfo.iImageSize);

	pTask->AlignCenter = GetPattAlignCenter(pTask->ImageBuff[pTask->PatternNum], pTask->PatternNum);
	// 		pAxis[X_AXIS]->MoveAxis(pTask->AlignCenter.x, 1, TRUE, NULL);
	// 		pAxis[Y_AXIS]->MoveAxis(pTask->AlignCenter.y, 1, TRUE, NULL);

	CDPoint dTargetPos;
	pAxis[X_AXIS]->StatusGetPos(&dTargetPos.x);
	pAxis[Y_AXIS]->StatusGetPos(&dTargetPos.y);
	dTargetPos.x += pTask->AlignCenter.x;
	dTargetPos.y += pTask->AlignCenter.y;
	pAxis[X_AXIS]->MoveAxis(dTargetPos.x, 0, TRUE);
	pAxis[Y_AXIS]->MoveAxis(dTargetPos.y, 0, TRUE);

	while (pAxis[X_AXIS]->CheckInMotion() || pAxis[Y_AXIS]->CheckInMotion())
	{
		Sleep(200);
	}
	Sleep(100);

	// 	memcpy(pTask->ImageBuff[pTask->PatternNum], pCamJAI->m_CnvImageInfo.pImageBuffer, sizeof(unsigned char)*DEF_IMAGE_X*DEF_IMAGE_Y);
	// 	pMILAlignDlg->m_fnTranferCamBuff2PattBuff(theApp.m_fnGetViewPointer()->m_pAuto->m_bPattAlignMode);
	pMILAlignDlg->m_fnBringImage(pCamJAI->m_CnvImageInfo.pImageBuffer, pCamJAI->m_CnvImageInfo.iImageSize);

	pTask->AlignCenter = GetPattAlignCenter(pTask->ImageBuff[pTask->PatternNum], pTask->PatternNum);
	// 		pAxis[X_AXIS]->MoveAxis(pTask->AlignCenter.x, 1, TRUE, NULL);
	// 		pAxis[Y_AXIS]->MoveAxis(pTask->AlignCenter.y, 1, TRUE, NULL);

	pAxis[X_AXIS]->StatusGetPos(&dTargetPos.x);
	pAxis[Y_AXIS]->StatusGetPos(&dTargetPos.y);
	dTargetPos.x += pTask->AlignCenter.x;
	dTargetPos.y += pTask->AlignCenter.y;
	pAxis[X_AXIS]->MoveAxis(dTargetPos.x, 0, TRUE);
	pAxis[Y_AXIS]->MoveAxis(dTargetPos.y, 0, TRUE);
	while (pAxis[X_AXIS]->CheckInMotion() || pAxis[Y_AXIS]->CheckInMotion())
	{
		Sleep(200);
	}
#endif
}

void CModel::m_fnSRSheetClear(  )
{
	for( int i = 0; i < MAX_DATA; i++ )
	{
		m_clSR.SetRow( i + 1 );
		m_clSR.SetCol( MEASURE_NAME );
		m_clSR.SetText( "" );
		m_clSR.SetCol(MEASURE_COL);
		m_clSR.SetText("");
		m_clSR.SetCol(MEASURE_ROW);
		m_clSR.SetText("");
		m_clSR.SetCol(MEASURE_X);
		m_clSR.SetText("");
		m_clSR.SetCol(MEASURE_Y);
		m_clSR.SetText("");
	}
}

int CModel::m_fnGetWaperSize()
{
	return m_clWaperSize.GetCurSel();
}

void CModel::m_fnGetInchToMM( int nInch, double &dMM, double dExclusion )
{
	dMM = (( ((double)nInch/ 2.) * INCH ) ) - dExclusion;
}

void CModel::m_fnSetWaperDivision(t_WAPER_INFO &stWaper)
{
	CString strData("");
	CString strNAME("");
	int     nCnt = 0;
	double  dStep = 0.0;
	BOOL    bEnd = TRUE;
	double  dRound = 0.0;

	if ((stWaper.dR < 0.0) ||
		(stWaper.dR > 360.0))
	{
		if (stWaper.bMsgDisp)
		{
			strData.Format("각도 설정범위는 0 ~ 359 입니다.");
			TECmsgOut(m_Util.CStringTobstr(strData), GREEN_1, 1);
		}
		return;
	}

	if (!stWaper.bCw)
	{
		dStep = 360.;
	}

	double  dX = 0.0;
	double  dY = 0.0;
	stWaper.nTotal = 0;
	if (stWaper.bZeroInclude)
	{
		stWaper.dX[stWaper.nTotal] = dX;
		stWaper.dY[stWaper.nTotal] = dY;
		stWaper.nTotal++;
	}

	if (stWaper.bOnlyZero)
	{
		stWaper.bOK = TRUE;
		return;
	}

	while (bEnd)
	{
		if (stWaper.bCw)
		{
			if (dStep >= 360.)
			{
				bEnd = FALSE;
				stWaper.bOK = TRUE;
				continue;
			}
		}
		else
		{
			if (dStep <= 0.)
			{
				bEnd = FALSE;
				stWaper.bOK = TRUE;
				continue;
			}
		}

		dX = stWaper.dMM * cos(dStep * (PI / 180.));
		dY = stWaper.dMM * sin(dStep * (PI / 180.));

		dRound = floor(1000. * (dX + 0.0005)) / 1000.;
		dX = dRound;
		if (dX == -0.000)	dX = 0.0;

		dRound = floor(1000. * (dY + 0.0005)) / 1000.;
		dY = dRound;
		if (dY == -0.000)	dY = 0.0;

		stWaper.dX[stWaper.nTotal] = dX;
		stWaper.dY[stWaper.nTotal] = dY;

		if (stWaper.bCw)
		{
			dStep += stWaper.dR;
		}
		else
		{
			dStep -= stWaper.dR;
		}
		stWaper.nTotal++;
	}
}

void CModel::m_fnSetWaper( t_WAPER_INFO &stWaper )
{
	CString strData("");
	CString strNAME("");
	double  dX     = 0.0;
	double  dY     = 0.0;
	int     nCnt   = 0;
	double  dStep  = 0.0;
	BOOL    bEnd   = TRUE;
	double  dRound = 0.0;

// 	if (( stWaper.dMM < 0 ) ||
// 		( stWaper.dMM > XY_MEASURE_LIMIT ))
// 	{
// 		if ( stWaper.bMsgDisp )
// 		{
// 			strData.Format( "반지름 설정범위는 %.3f ~ %.3f 입니다.", 0., XY_MEASURE_LIMIT );
// 			TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 1 );
// 		}
// 		return;
// 	}
	if (( stWaper.dR < 0.0 ) ||
		( stWaper.dR > 360.0 ))
	{
		if ( stWaper.bMsgDisp )
		{
			strData.Format( "각도 설정범위는 0 ~ 359 입니다." );
			TECmsgOut( m_Util.CStringTobstr(strData), GREEN_1, 1 );
		}
		return;
	}

	stWaper.nTotal = 0;
	if ( !stWaper.bCw )
	{
		dStep = 360.;
	}

	while ( bEnd )
	{	
		if ( stWaper.bZeroInclude )
		{
			if ( stWaper.bCw )
			{
				if ( dStep > 360. )
				{
					bEnd = FALSE;
					stWaper.bOK = TRUE;
					continue;
				}
			}
			else
			{
				if ( dStep < 0. )
				{
					bEnd = FALSE;
					stWaper.bOK = TRUE;
					continue;
				}
			}
			stWaper.dX[stWaper.nTotal] = dX;
			stWaper.dY[stWaper.nTotal] = dY;
			dX = stWaper.dMM * cos( dStep * ( PI / 180. ));
			dY = stWaper.dMM * sin( dStep * ( PI / 180. ));
			dRound = floor( 1000. * ( dX + 0.0005 )) / 1000.;
			dX = dRound;
			if ( dX == -0.000 ) dX = 0.0;
			dRound = floor( 1000. * ( dY + 0.0005 )) / 1000.;
			dY = dRound;
			if ( dY == -0.000 ) dY = 0.0;
			if ( stWaper.bCw )
			{
				dStep += stWaper.dR;
			}
			else
			{
				dStep -= stWaper.dR;
			}
			stWaper.nTotal++;
		}
		else
		{
			if ( stWaper.bCw )
			{
				if ( dStep >= 360. )
				{
					bEnd = FALSE;
					stWaper.bOK = TRUE;
					continue;
				}
			}
			else
			{
				if ( dStep <= 0. )
				{
					bEnd = FALSE;
					stWaper.bOK = TRUE;
					continue;
				}
			}
			dX = stWaper.dMM * cos( dStep * ( PI / 180. ));
			dY = stWaper.dMM * sin( dStep * ( PI / 180. ));
			dRound = floor( 1000. * ( dX + 0.0005 )) / 1000.;
			dX = dRound;
			if ( dX == -0.000 ) dX = 0.0;
			dRound = floor( 1000. * ( dY + 0.0005 )) / 1000.;
			dY = dRound;
			if ( dY == -0.000 ) dY = 0.0;
			stWaper.dX[stWaper.nTotal] = dX;
			stWaper.dY[stWaper.nTotal] = dY;
			if ( stWaper.bCw )
			{
				dStep += stWaper.dR;
			}
			else
			{
				dStep -= stWaper.dR;
			}
			stWaper.nTotal++;
		}
	}
}

void CModel::OnBnClickedBtnPoint()
{
	if ( theApp.m_fnGetViewPointer()->m_pPoint == NULL )
	{
		theApp.m_fnGetViewPointer()->m_pPoint = new CPointDLG();
		theApp.m_fnGetViewPointer()->m_pPoint->m_fnSetInit();
	}
	theApp.m_fnGetViewPointer()->m_pPoint->m_fnSetShowFlag( !theApp.m_fnGetViewPointer()->m_pPoint->m_bShow );
}


void CModel::OnEnChangeEdtThickness()
{
	CString strData("");
	double dThickness = 0.0;
	GetDlgItem( IDC_EDT_THICKNESS )->GetWindowText( strData );
	dThickness = atof( strData );
	if (( dThickness > 5000.0 )
// 		||( dThickness < 0.0 )
		)
	{
		TECmsgOut( L"두께를 확인해 주세요.( ~ 5000)", GREEN_1, 1 );
		GetDlgItem( IDC_EDT_THICKNESS )->SetWindowText( "1000" );
		return;
	}
	m_dWaperThickness = dThickness;

}


void CModel::OnEnChangeEdtExclusion()
{
	CString strData("");
	double dExclusion = 0.0;
	GetDlgItem( IDC_EDT_EXCLUSION )->GetWindowText( strData );
	dExclusion = atof( strData );
	if (( dExclusion > 20.0 ) ||
		( dExclusion < 0.0 ))
	{
		m_dWaperExclusion = 0.0;
		TECmsgOut( L"Edge Exclusion을 확인해 주세요.(0.0 ~ 20.0)", GREEN_1, 1 );
		GetDlgItem(IDC_EDT_EXCLUSION)->SetWindowText( "0.000" );
		theApp.m_fnGetViewPointer()->m_pStageMap->m_dExclusion = m_dWaperExclusion;
		return;
	}
	m_dWaperExclusion = dExclusion;
	theApp.m_fnGetViewPointer()->m_pStageMap->m_dExclusion = m_dWaperExclusion;
}

BOOL CModel::m_fnGetCheckAxisData(double dStartX, double dStartY, double dStepX, double dStepY, int nCountX, int nCountY )
{
	double dPositionX	= 0.0L;
	double dPositionY	= 0.0L;
	double dPLimitX     = 0.0L;
	double dPLimitY     = 0.0L;
	double dNLimitX     = 0.0L;
	double dNLimitY     = 0.0L;

	dPLimitX = STAGE_SIZE_X;
	dNLimitX = -STAGE_SIZE_X;
	dPLimitY = STAGE_SIZE_Y;
	dNLimitY = -STAGE_SIZE_X;

	for( int j = 0; j < nCountY; j++ )
	{
		dPositionY = dStartY + ( dStepY * j );
		for( int k = 0; k < nCountX; k++ )
		{
			dPositionX = dStartX + ( dStepX * k );
			if (( dPositionX < dNLimitX ) || ( dPositionX > dPLimitX ))
			{
				return FALSE;
			}
		}
		if (( dPositionY < dNLimitY ) || ( dPositionY > dPLimitY ))
		{
			return FALSE;
		}
	}
	return TRUE;
}


void CModel::OnBnClickedButtonGetPos()
{
#if AJINTEX_BOARD
	double dwPosition = 0L;
	CString strData("");
	if (theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos(X_AXIS, dwPosition) == RC_NORMAL)
	{
		strData.Format("%7.3f", dwPosition);
		GetDlgItem(IDC_EDT_PATT_POS_X)->SetWindowText(strData);
	}
	if (theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos(Y_AXIS, dwPosition) == RC_NORMAL)
	{
		strData.Format("%7.3f", dwPosition);
		GetDlgItem(IDC_EDT_PATT_POS_Y)->SetWindowText(strData);
	}
#endif
}


void CModel::OnBnClickedBtnAlignPosAutoSet()
{
#if !_DEBUG
	if (m_pTask->stLogin.nLevel == UNKNOWN_USER)
	{
		return;
	}
#endif
#if MIL_USE

// 	SendMessage(WM_COMMAND, IDC_BTN_SAVE, 0);	
	m_fnModule_GetCurItemVal();
	if(!pWorkRun->m_strPatternName.IsEmpty())
		m_fnSetModel_Save(pWorkRun->m_strPatternName);

	m_fnModule_CalPattInfo();

	return;// AlignPosAutoSet_Test4Point();
#endif
}

void CModel::m_fnModule_CalPattInfo()
{
	CString strVal;
	double dStX, dPattX, dGabX;
	double dStY, dPattY, dGabY;
	m_ctrlEditPattPosX.GetWindowTextA(strVal);
	dStX = atof(strVal);
	m_ctrlEditPattPosY.GetWindowTextA(strVal);
	dStY = atof(strVal);

	m_ctrlEditPattSizeX.GetWindowTextA(strVal);
	dPattX = atof(strVal);
	m_ctrlEditPattSizeY.GetWindowTextA(strVal);
	dPattY = atof(strVal);

	m_ctrlEditPattGabX.GetWindowTextA(strVal);
	dGabX = atof(strVal);
	m_ctrlEditPattGabY.GetWindowTextA(strVal);
	dGabY = atof(strVal);

	if (dPattX <= 0
		|| dPattY <= 0
		)
	{
		TECmsgOut(((L"ALIGN 계산정보를 입력해주세요.")), LIGHTRED, 1);
		return;
	}

	//12인치 기준 가능한 모든점 구함
	//현재 세팅된 웨이퍼 사이즈 안의 점만 배열정리
	//스테이지맵뷰 draw 회색
	//2,0 / 0,2 / -2,0 / 0,-2
	//스테이지맵뷰 draw 파랑

	int iMaxDivisionX = (int)(200.0 / (dPattX + dGabX)) * 2; //한 변당 최대 포인트 개수
	int iMaxDivisionY = (int)(200.0 / (dPattY + dGabY)) * 2; //한 변당 최대 포인트 개수

	CDPoint dPntTemp;

	CDPoint* dPntInCircle;
	dPntInCircle = new CDPoint[iMaxDivisionX * iMaxDivisionY];
	int iPointCount = 0;

	int arrWaferInch[4] = { 4,6,8,12 };
	int CurWaferSizeSel = m_clWaperSize.GetCurSel();
	double dMM;
	theApp.m_fnGetViewPointer()->m_pModel->m_fnGetInchToMM(arrWaferInch[CurWaferSizeSel], dMM, m_dWaperExclusion);

	int iMinX = 99999;
	int iMaxX = 0;
	int iMinY = 99999;
	int iMaxY = 0;

	for (int j = 0; j < iMaxDivisionY; j++)
	{
		for (int i = 0; i < iMaxDivisionX; i++)
		{
			dPntTemp.x = dStX + (dPattX + dGabX) * (i - (iMaxDivisionX >> 1));
			dPntTemp.y = dStY + (dPattY + dGabY) * (j - (iMaxDivisionY >> 1));

			if (!inCircle(dPntTemp.x, dPntTemp.y, 0.0, 0.0, dMM))
				continue;

			iMinX = iMinX < i ? iMinX : i;
			iMaxX = iMaxX > i ? iMaxX : i;
			iMinY = iMinY < j ? iMinY : j;
			iMaxY = iMaxY > j ? iMaxY : j;

			dPntInCircle[iPointCount].x = dPntTemp.x;
			dPntInCircle[iPointCount].y = dPntTemp.y;
			iPointCount++;
		}
	}

	for (int i = 0; i < iPointCount; i++)
	{
		m_pTask->stModel.stSRData[i].dwXPosition = dPntInCircle[i].x;
		m_pTask->stModel.stSRData[i].dwYPosition = dPntInCircle[i].y;
	}
	m_pTask->stModel.nSRData = iPointCount;

	delete[] dPntInCircle;
	theApp.m_fnGetViewPointer()->m_pStageMap->m_iPntAlignCnt = 0;

	if (iPointCount <= 0)
		return;


	int iOffset = 1;
	int iAlignArrX[4] = { (iMaxX + iMinX) >> 1,	iMinX + iOffset,			iMaxX - iOffset,			(iMaxX + iMinX) >> 1 };
	int iAlignArrY[4] = { iMinY + iOffset,		(iMaxY + iMinY) >> 1,	(iMaxY + iMinY) >> 1,	iMaxY - iOffset };
	CString strWork;

	double dTempX1, dTempY1;
	double dTempX2, dTempY2;

	for (int i = 0; i < 4; i++)
	{
		// 		theApp.m_fnGetViewPointer()->m_pStageMap->m_dPntAlign[i].x = dStX + (dPattX + dGabX) * (iAlignArrX[i] - (iMaxDivisionX >> 1));// * cos(pTask->fAlign_T*PI / 180.0);
		// 		theApp.m_fnGetViewPointer()->m_pStageMap->m_dPntAlign[i].y = dStY + (dPattY + dGabY) * (iAlignArrY[i] - (iMaxDivisionY >> 1));// * sin(pTask->fAlign_T*PI / 180.0);
		dTempX1 = (dPattX + dGabX) * (iAlignArrX[i] - (iMaxDivisionX >> 1));
		dTempY1 = (dPattY + dGabY) * (iAlignArrY[i] - (iMaxDivisionY >> 1));

		dTempX2 = (dTempX1* cos(pTask->fAlign_T*PI / (-180.0))) - (dTempY1* sin(pTask->fAlign_T*PI / (-180.0)));
		dTempY2 = (dTempX1* sin(pTask->fAlign_T*PI / (-180.0))) + (dTempY1* cos(pTask->fAlign_T*PI / (-180.0)));

		theApp.m_fnGetViewPointer()->m_pStageMap->m_dPntAlign[i].x = dStX + dTempX2;
		theApp.m_fnGetViewPointer()->m_pStageMap->m_dPntAlign[i].y = dStY + dTempY2;

		m_clSR.SetRow(i + 2);

		m_clSR.SetCol(MEASURE_NAME);
		strWork.Format("%01d", i + 2);
		m_clSR.SetText(strWork);

		m_clSR.SetCol(MEASURE_COL);
		strWork.Format("%01d", iAlignArrX[i] - ((iMaxX + iMinX) >> 1));
		m_clSR.SetText(strWork);

		m_clSR.SetCol(MEASURE_ROW);
		strWork.Format("%01d", iAlignArrY[i] - ((iMaxY + iMinY) >> 1));
		m_clSR.SetText(strWork);

		m_clSR.SetCol(MEASURE_X);
		strWork.Format("%7.3f", theApp.m_fnGetViewPointer()->m_pStageMap->m_dPntAlign[i].x);
		m_clSR.SetText(strWork);

		m_clSR.SetCol(MEASURE_Y);
		strWork.Format("%7.3f", theApp.m_fnGetViewPointer()->m_pStageMap->m_dPntAlign[i].y);
		m_clSR.SetText(strWork);
	}

	theApp.m_fnGetViewPointer()->m_pStageMap->m_dPntAlign[4].x = dStX;
	theApp.m_fnGetViewPointer()->m_pStageMap->m_dPntAlign[4].y = dStY;

	m_clSR.SetRow(1);

	m_clSR.SetCol(MEASURE_NAME);
	strWork.Format("%01d", 1);
	m_clSR.SetText(strWork);

	m_clSR.SetCol(MEASURE_COL);
	strWork.Format("%01d", 0);
	m_clSR.SetText(strWork);

	m_clSR.SetCol(MEASURE_ROW);
	strWork.Format("%01d", 0);
	m_clSR.SetText(strWork);

	m_clSR.SetCol(MEASURE_X);
	strWork.Format("%7.3f", dStX);
	m_clSR.SetText(strWork);

	m_clSR.SetCol(MEASURE_Y);
	strWork.Format("%7.3f", dStY);
	m_clSR.SetText(strWork);


	theApp.m_fnGetViewPointer()->m_pStageMap->m_iPntAlignCnt = 5;

	pWorkRun->m_bSqncInterrupt[SQITRT_AUTO_CAL_PATTERN_INFO] = TRUE;
}

void CModel::m_fnModule_GetCurItemVal()
{
	if (m_pTask == NULL) return;

	CString strWork("");

	m_pTask->stModel.bFlat = ((CButton*)GetDlgItem(IDC_CHK_FLAT))->GetCheck();

	m_pTask->stModel.nWaperSize = m_clWaperSize.GetCurSel();

	m_pTask->stModel.nOutputDisp = m_clDisp.GetCurSel();

	GetDlgItem(IDC_EDT_THICKNESS)->GetWindowText(strWork);
	m_pTask->stModel.dWaperThickness = atof(strWork);
	GetDlgItem(IDC_EDT_EXCLUSION)->GetWindowText(strWork);
	m_pTask->stModel.dEdgeExclusion = atof(strWork);

	// 	m_pTask->stModel.iModelType = m_iRadioModelType;

	GetDlgItem(IDC_EDT_PATT_POS_X)->GetWindowText(strWork);
	m_pTask->stModel.dPattPosX = atof(strWork);
	GetDlgItem(IDC_EDT_PATT_POS_Y)->GetWindowText(strWork);
	m_pTask->stModel.dPattPosY = atof(strWork);
	GetDlgItem(IDC_EDT_PATT_SIZE_X)->GetWindowText(strWork);
	m_pTask->stModel.dPattSizeX = atof(strWork);
	GetDlgItem(IDC_EDT_PATT_SIZE_Y)->GetWindowText(strWork);
	m_pTask->stModel.dPattSizeY = atof(strWork);
	GetDlgItem(IDC_EDT_PATT_GAB_X)->GetWindowText(strWork);
	m_pTask->stModel.dPattGabX = atof(strWork);
	GetDlgItem(IDC_EDT_PATT_GAB_Y)->GetWindowText(strWork);
	m_pTask->stModel.dPattGabY = atof(strWork);



	CString strData("");

	int i = 0;
	int nCnt = 0;
	int nGroup = 0;
	for (i = 0; i < MAX_DATA; i++)
	{
		m_clSR.SetRow(i + 1);
		m_clSR.SetCol(MEASURE_NAME);
		strData = m_clSR.GetText();
		strData.Trim();
		strData.Trim();
		if (!strData.IsEmpty())
		{
			sprintf_s(m_pTask->stModel.stSRData[nCnt].szName, "%s", strData.GetString());
			m_clSR.SetCol(MEASURE_COL);
			strData = m_clSR.GetText();
			m_pTask->stModel.stSRData[nCnt].iColum = atoi(strData);
			m_pTask->stModel.stSRData[nCnt].dColum = atof(strData);
			m_clSR.SetCol(MEASURE_ROW);
			strData = m_clSR.GetText();
			m_pTask->stModel.stSRData[nCnt].iRow = atoi(strData);
			m_pTask->stModel.stSRData[nCnt].dRow = atof(strData);
			m_clSR.SetCol(MEASURE_X);
			strData = m_clSR.GetText();
			m_pTask->stModel.stSRData[nCnt].dwXPosition = atof(strData);
			m_clSR.SetCol(MEASURE_Y);
			strData = m_clSR.GetText();
			m_pTask->stModel.stSRData[nCnt].dwYPosition = atof(strData);
			nCnt++;
		}
	}
	m_pTask->stModel.nSRData = nCnt;
}

void CModel::AlignPosAutoSet_Test4Point()
{
	CString strVal;
	double dStX, dPattX, dGabX;
	double dStY, dPattY, dGabY;
	m_ctrlEditPattPosX.GetWindowTextA(strVal);
	dStX = atof(strVal);
	m_ctrlEditPattPosY.GetWindowTextA(strVal);
	dStY = atof(strVal);

	m_ctrlEditPattSizeX.GetWindowTextA(strVal);
	dPattX = atof(strVal);
	m_ctrlEditPattSizeY.GetWindowTextA(strVal);
	dPattY = atof(strVal);

	m_ctrlEditPattGabX.GetWindowTextA(strVal);
	dGabX = atof(strVal);
	m_ctrlEditPattGabY.GetWindowTextA(strVal);
	dGabY = atof(strVal);

	if (dPattX <= 0
		|| dPattY <= 0
		)
	{
		TECmsgOut(((L"ALIGN 계산정보를 입력해주세요.")), LIGHTRED, 1);
		return;
	}

	for (int i = 0; i < pTask->NumPoint; i++)
	{
		m_clSR.SetRow(i + 1);

		m_clSR.SetCol(MEASURE_COL);
		strVal = m_clSR.GetText();
		pTask->XPixArray[i] = atof(strVal);

		pTask->XWorldArray[i] = dStX + (dPattX + dGabX)*(pTask->XPixArray[i] - 1);

		m_clSR.SetCol(MEASURE_X);
		m_clSR.SetText(m_Util.FormatStr(_T("%.3f"), pTask->XWorldArray[i]));


		m_clSR.SetCol(MEASURE_ROW);
		strVal = m_clSR.GetText();
		pTask->YPixArray[i] = atof(strVal);

		pTask->YWorldArray[i] = dStY + (dPattY + dGabY)*(pTask->YPixArray[i] - 1);

		m_clSR.SetCol(MEASURE_Y);
		m_clSR.SetText(m_Util.FormatStr(_T("%.3f"), pTask->YWorldArray[i]));
	}
}

void CModel::OnBnClickedBtnAlignMapping()
{
#if !_DEBUG
	if (m_pTask->stLogin.nLevel == UNKNOWN_USER)
	{
		return;
	}
#endif

	CString strVal;
	pTask->NumPoint = 4;// pTask->NumPoint > 4 ? 4 : pTask->NumPoint;
	for (int i = 0; i < pTask->NumPoint; i++)
	{
		m_clSR.SetRow(i + 1);

		m_clSR.SetCol(MEASURE_COL);
		strVal = m_clSR.GetText();
		pTask->XPixArray[i] = atof(strVal);

		m_clSR.SetCol(MEASURE_ROW);
		strVal = m_clSR.GetText();
		pTask->YPixArray[i] = atof(strVal);

		m_clSR.SetCol(MEASURE_X);
		strVal = m_clSR.GetText();
		pTask->XWorldArray[i] = atof(strVal);

		m_clSR.SetCol(MEASURE_Y);
		strVal = m_clSR.GetText();
		pTask->YWorldArray[i] = atof(strVal);
	}


// 	unsigned long	ThreadID;
#if JAI_CAMERA
	for (int i = 0; i < pTask->NumPoint; i++)
	{
		//Move
		pAxis[X_AXIS]->MoveAxis(pTask->XWorldArray[i], 0, TRUE);
		pAxis[Y_AXIS]->MoveAxis(pTask->YWorldArray[i], 0, TRUE);
		while (pAxis[X_AXIS]->CheckInMotion()
			|| pAxis[Y_AXIS]->CheckInMotion())
		{
			Sleep(500);
		}

		//AF
		if (!theApp.m_fnGetViewPointer()->m_pStageMap->m_ctrlCheckCamera.GetCheck())
		{
			((CButton*)theApp.m_fnGetViewPointer()->m_pStageMap->GetDlgItem(IDC_CHK_CAMERA))->SetCheck(TRUE);
			theApp.m_fnGetViewPointer()->m_pStageMap->OnBnClickedChkCamera();
		}
// 		thread thrdStartAuto = thread(ThrdAF);
// 		thrdStartAuto.join();
#if MIL_USE
		ThrdAF(TRUE);
#endif
		//Centering
// 		pTask->PatternNum = 0;
// 
// 		memcpy(pTask->ImageBuff[pTask->PatternNum], pCamJAI->m_CnvImageInfo.pImageBuffer, sizeof(unsigned char)*DEF_IMAGE_X*DEF_IMAGE_Y);
// 		pTask->AlignCenter = GetPattAlignCenter(pTask->ImageBuff[pTask->PatternNum], pTask->PatternNum);
// 		pAxis[X_AXIS]->MoveAxis(pTask->AlignCenter.x, 1, TRUE, NULL);
// 		pAxis[Y_AXIS]->MoveAxis(pTask->AlignCenter.y, 1, TRUE, NULL);
// 
// 		while (pAxis[X_AXIS]->CheckInMotion() || pAxis[Y_AXIS]->CheckInMotion())
// 		{
// 			Sleep(500);
// 		}

		//ImgGrab
		ZeroMemory(pTask->ImageBuff[i], sizeof(unsigned char)*DEF_IMAGE_X*DEF_IMAGE_Y);
// 		memcpy(pTask->ImageBuff[i], pCamJAI->m_CnvImageInfo.pImageBuffer
// 			, sizeof(unsigned char)*pCamJAI->m_CnvImageInfo.iSizeX*pCamJAI->m_CnvImageInfo.iSizeY);
		pTask->PatternNum = i;
		pMILAlignDlg->m_fnTranferCamBuff2PattBuff(theApp.m_fnGetViewPointer()->m_pAuto->m_bPattAlignMode);


		MIL_ID saveImg;
		M_Alloc8(pCamJAI->m_CnvImageInfo.iSizeX, pCamJAI->m_CnvImageInfo.iSizeY, saveImg);
		MbufPut(saveImg, pTask->ImageBuff[i]);// pCamJAI->m_CnvImageInfo.pImageBuffer);
// 		MbufGet(saveImg, pTask->ImageBuff[i]);
		CString strFilePath;
		strFilePath.Format(_T("D:\\EllipsoMeter\\Image\\AlignMappingImg_%02d.bmp"), i + 1);
		MbufExport(strFilePath.GetString(), M_BMP, saveImg);
		MbufFree(saveImg);
	}
	((CButton*)theApp.m_fnGetViewPointer()->m_pStageMap->GetDlgItem(IDC_CHK_CAMERA))->SetCheck(FALSE);
	theApp.m_fnGetViewPointer()->m_pStageMap->OnBnClickedChkCamera();

	pTask->ErrorCode = pInsp[DEF_DISP_SET]->ErrorCode = 0;
	pTask->iGroupNum = 0;	

	SetMotorAlign(pTask->iGroupNum, pTask->XPixArray, pTask->YPixArray, pTask->XWorldArray, pTask->YWorldArray, pTask->ImageBuff, pTask->NumPoint);
	memcpy(pTask->msgVision, pInsp[DEF_DISP_SET]->msgVision, sizeof(pTask->msgVision));
	pTask->ErrorCode = pInsp[DEF_DISP_SET]->ErrorCode;
#endif
	TECmsgOut(L"Align Mapping Complete", LIGHTGREEN, 1);
}




void CModel::OnBnClickedBtnCalcPos()
{
#if !_DEBUG
	if (m_pTask->stLogin.nLevel == UNKNOWN_USER)
	{
		return;
	}
#endif
#if MIL_USE
	pTask->iGroupNum = 0;
	pTask->tansformType = 0;

	CString strVal;
	for (int i = 0; i < MAX_DATA; i++)
	{
		m_clSR.SetRow(i + 1);
		m_clSR.SetCol(MEASURE_COL);
		if (m_clSR.GetText().GetLength() < 1)
		{
			break;
		}

		strVal = m_clSR.GetText();
		pTask->inX = atof(strVal);

		m_clSR.SetCol(MEASURE_ROW);
		strVal = m_clSR.GetText();
		pTask->inY = atof(strVal);

		GetTransformCoordinate(pTask->iGroupNum, pTask->inX, pTask->inY, &pTask->OutX, &pTask->OutY, pTask->tansformType);

		m_clSR.SetCol(MEASURE_X);
		m_clSR.SetText(m_Util.FormatStr(_T("%.3f"), pTask->OutX));

		m_clSR.SetCol(MEASURE_Y);
		m_clSR.SetText(m_Util.FormatStr(_T("%.3f"), pTask->OutY));
	}
#endif



}


void CModel::OnLbnSelchangeListMeasureResult()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CModel::OnBnClickedChkAlignUse()
{
	if (m_ctrlCheckAlignUse.GetCheck())
	{
		m_ctrlCheckSetSubPoint.EnableWindow(TRUE);
	}
	else
	{
		m_ctrlCheckSetSubPoint.EnableWindow(FALSE);
	}
}


void CModel::OnBnClickedCheckSubpoint()
{
	if(!m_ctrlCheckSetSubPoint.GetCheck())
		m_bSetInsertSubPosFinish = FALSE;
}


void CModel::OnBnClickedBtnAf2()
{
#if MIL_USE
	thread thrdCorrectAlign = thread(ThrdCorrectAlign);
	thrdCorrectAlign.detach();
#endif
}


void CModel::OnBnClickedButton3()
{
}

void CModel::m_fnCheckAlignUse(BOOL bCheck)
{
	m_ctrlCheckAlignUse.SetCheck(bCheck);
}

void CModel::m_fnPosInfoInputSheet(int iStep, int iCol, int iRaw)
{
	CString strName("");
	CString strMsg("");
	CString strCol("0");
	CString strRow("0");
	CString strX("0.000");
	CString strY("0.000");
	CString strZ("0.000");

	double dwPositionX = 0L;
	double dwPositionY = 0L;
	double dwPositionZ = 0L;


// 	//스탭별 계산식
// 	switch (iStep)
// 	{
// 	case 1:
// 		break;
// 	case 2:
// 		break;
// 	case 3:
// 		break;
// 	case 4:
// 		break;
// 	case 5:
// 		break;
// 	default:
// 		return;
// 	}
// 
	m_clSR.SetRow(iStep);

	strName.Format("%01d", iStep);
	strCol.Format("%d", iCol);
	strRow.Format("%d", iRaw);
#if AJINTEX_BOARD
	CString strData("");
	if (theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos(X_AXIS, dwPositionX) == RC_NORMAL)
	{
		strX.Format("%7.3f", dwPositionX);
	}
	if (theApp.m_fnGetViewPointer()->m_pAjin->m_fnAxmStatusGetActPos(Y_AXIS, dwPositionY) == RC_NORMAL)
	{
		strY.Format("%7.3f", dwPositionY);
	}
#endif

	if (iStep == 1)
	{
		GetDlgItem(IDC_EDT_PATT_POS_X)->SetWindowText(strX);
		GetDlgItem(IDC_EDT_PATT_POS_Y)->SetWindowText(strY);
	}

	m_clSR.SetCol(MEASURE_NAME);
	m_clSR.SetText(strName);
	m_clSR.SetCol(MEASURE_COL);
	m_clSR.SetText(strCol);
	m_clSR.SetCol(MEASURE_ROW);
	m_clSR.SetText(strRow);
	m_clSR.SetCol(MEASURE_X);
	m_clSR.SetText(strX);
	m_clSR.SetCol(MEASURE_Y);
	m_clSR.SetText(strY);

// 	UpdateData(FALSE);

	return;
}

//void CModel::OnLbnSelchangeLstModel()
//{
//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//}


void CModel::OnBnClickedBtnInputRowcolAutoCalc()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}




void CModel::OnBnClickedRadioModelType(UINT wParam)
{
	UpdateData(TRUE);

	CString tmpStr;
	tmpStr.Format("%d/%d", m_iRadioModelType, wParam);
	OutputDebugString(tmpStr);

	switch (wParam)
	{
	case IDC_RADIO_MODEL_TYPE1:
		break;
	case IDC_RADIO_MODEL_TYPE2:
		break;
	case IDC_RADIO_MODEL_TYPE3:
		break;
	case IDC_RADIO_MODEL_TYPE4:
		break;
	}
}

void CModel::m_fnSetRadioModelType(int iCheckNum)
{
	m_iRadioModelType = iCheckNum;
//	UpdateData(FALSE);


	CButton* tmpBtn;
	for (int i=0; i<4; i++)
	{
		tmpBtn = (CButton*)GetDlgItem(IDC_RADIO_MODEL_TYPE1 + i);
		tmpBtn->SetCheck(FALSE);
	}
	tmpBtn = (CButton*)GetDlgItem(IDC_RADIO_MODEL_TYPE1 + iCheckNum);
	tmpBtn->SetCheck(TRUE);
}


void CModel::ChangeFpspreadSrInfo(long Col, long Row)
{
	CString strDataCol;
	CString strDataRow;

	if(m_iRadioModelType == MODEL_TYPE_TARGET)
	{
		if (Col == 2 || Col == 3)
		{
			m_clSR.SetRow(Row);

			m_clSR.SetCol(2);
			strDataCol = m_clSR.GetText();
			if (strDataCol.IsEmpty())
				return;

			m_clSR.SetCol(3);
			strDataRow = m_clSR.GetText();
			if (strDataRow.IsEmpty())
				return;

			pTask->inX = (double)atoi(strDataCol);
			pTask->inY = (double)atoi(strDataRow);
			GetTransformCoordinate(pTask->iGroupNum, pTask->inX, pTask->inY, &pTask->OutX, &pTask->OutY);

			m_clSR.SetCol(4);
			strDataCol.Format("%7.3f", pTask->OutX);
			m_clSR.SetText(strDataCol);

			m_clSR.SetCol(5);
			strDataRow.Format("%7.3f", pTask->OutY);
			m_clSR.SetText(strDataRow);

			m_pTask->stModel.stSRData[Row - 1].iColum = (int)pTask->inX;
			m_pTask->stModel.stSRData[Row - 1].iRow = (int)pTask->inY;
			m_pTask->stModel.stSRData[Row - 1].dwXPosition = pTask->OutX;
			m_pTask->stModel.stSRData[Row - 1].dwYPosition = pTask->OutY;
		}
	}
}


void CModel::OnLbnSelchangeLstModel()
{
	int nNo = m_clModel.GetCurSel();
	if (nNo == -1)
		return;

	CString strModel("");
	m_clModel.GetText(nNo, strModel);


	switch (m_iRadioModelType)
	{
	case MODEL_TYPE_GENERAL://일반
		break;
	case MODEL_TYPE_PATTERN://패턴
		if (strModel.GetLength() <= 8 || strModel.Right(8) != "_Pattern")
		{
			TECmsgOut(L"모델명에 _Pattern 이 붙은 모델만 선택 할 수 있습니다.", LIGHTMAGENTA, 1000);
			m_clModel.SetCurSel(-1);
			return;
		}
		break;
	case MODEL_TYPE_TARGET://타겟
		if (strModel.GetLength() <= 7 || strModel.Right(7) != "_Target")
		{
			TECmsgOut(L"모델명에 _Tatget 이 붙은 모델만 선택 할 수 있습니다.", LIGHTMAGENTA, 1000);
			m_clModel.SetCurSel(-1);
			return;
		}
		break;
	case MODEL_TYPE_SUB://서브
		if (strModel.GetLength() <= 4 || strModel.Right(4) != "_Sub")
		{
			TECmsgOut(L"모델명에 _Sub 가 붙은 모델만 선택 할 수 있습니다.", LIGHTMAGENTA, 1000);
			m_clModel.SetCurSel(-1);
			return;
		}
		break;
	default:
		break;
	}

}
