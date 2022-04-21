#pragma once
#include "stdafx.h"
#if MIL_USE
#include "WorkRun.h"
#include "EllipsoMeter.h"
#include "ATXMotion.h"
#include "CamJAI.h"
#include "UtilityClass.h"
// #include "OpenCV_Lib.h"
#include "MilVision.h"
#include "MilVisionDlg.h"
#include "ATXMotion.h"
#include "MakeObj.h"
#include "CommThread.h"
#include "Task.h"
#include "ModelData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CWorkRun* pWorkRun;

CEllipsoMeterDlg* pView;

void ThrdAF(BOOL bVisionAF)
{
#if JAI_CAMERA
	if (pWorkRun->m_bThrdFlag[TF_AUTOFOCUS] == TRUE)
	{
		TECmsgOut(L"Vision AutoFocusing 작업 중입니다...", YELLOW, 1000);
		return;
	}
	pWorkRun->m_bThrdFlag[TF_AUTOFOCUS] = TRUE;

	pWorkRun->SqncVisionAF(bVisionAF);

	pWorkRun->m_bThrdFlag[TF_AUTOFOCUS] = FALSE;



#endif
}


void ThrdManualMeasure()
{
	if (pWorkRun->m_bThrdFlag[TF_AUTORUN])
		return;

	pWorkRun->m_bThrdFlag[TF_AUTORUN] = TRUE;
	TECmsgOut(L"Manual 측정을 시작합니다.", LIGHTGREEN, 500);

	if (!pWorkRun->SqncManualMeasure())
	{
		TECmsgOut(L"Manual 측정을 완료하지 못했습니다.", LIGHTMAGENTA, 1);
	}
	else
		TECmsgOut(L"Manual 측정을 완료했습니다.", LIGHTBLUE, 1000);

	theApp.m_fnGetViewPointer()->m_pAuto->m_strWaferTimeEnd = theApp.m_fnGetViewPointer()->m_pAuto->m_tiAWafer.GetStrTime(theApp.m_fnGetViewPointer()->m_pAuto->m_tiAWafer.SetTimeEnd());
	sprintf_s(theApp.m_fnGetViewPointer()->m_pAuto->m_stUser.szTimeEnd, "%s", theApp.m_fnGetViewPointer()->m_pAuto->m_strWaferTimeEnd.GetString());
	sprintf_s(theApp.m_fnGetViewPointer()->m_pAuto->m_stUser.szTimeSpend, "%.3f", theApp.m_fnGetViewPointer()->m_pAuto->m_tiAWafer.GetTimeInterval(2, FALSE));

	pWorkRun->m_bThrdFlag[TF_AUTORUN] = FALSE;
}

void ThrdLoadRecipe()
{
	pView->m_pManual->m_pElipso->m_strRunRecipe = pWorkRun->m_strRecipeName;

	pWorkRun->SqncRecipeLoad();
	return;
}


void ThrdCorrectAlign()
{
	if (pWorkRun->m_bThrdFlag[TF_AUTORUN])
		return;

	pWorkRun->m_bThrdFlag[TF_AUTORUN] = TRUE;

	if (!pWorkRun->SqncReadyMeasure(FALSE))
	{
		TECmsgOut(L"엘립소메타 연결상태를 확인해주세요.", LIGHTMAGENTA, 1);
		pWorkRun->SqncCloseMeasure();
		pWorkRun->m_bThrdFlag[TF_AUTORUN] = FALSE;
		return;
	}

	t_ELIPSO_RESULT_INFO stcResultAlign;

	if (!pWorkRun->SqncCorrectAlign(stcResultAlign))
	{
		TECmsgOut(L"""위치정렬""을 완료하지 못했습니다.", LIGHTMAGENTA, 1);
	}

	pWorkRun->SqncCloseMeasure();
	pWorkRun->m_bThrdFlag[TF_AUTORUN] = FALSE;
	return;
}


void ThrdReMeasure(int nRow, BOOL* bRes)
{
	if (pWorkRun->m_bThrdFlag[TF_AUTORUN])
		return;

	pWorkRun->m_bThrdFlag[TF_AUTORUN] = TRUE;

	t_ELIPSO_RESULT_INFO stcResultAlign;

	ThrdAF(pView->m_pAuto->m_ctrlChkUseAF.GetCheck());

	if (!pWorkRun->SqncReadyMeasure(FALSE))
	{
		TECmsgOut(L"엘립소메타 연결상태를 확인해주세요.", LIGHTMAGENTA, 1);
		pWorkRun->SqncCloseMeasure();
		pWorkRun->m_bThrdFlag[TF_AUTORUN] = FALSE;
		return;
	}

	if (!pWorkRun->SqncCorrectAlign(stcResultAlign))
	{
		TECmsgOut(L"""위치정렬""을 완료하지 못했습니다.", LIGHTMAGENTA, 1);
		pWorkRun->m_bThrdFlag[TF_AUTORUN] = FALSE;
		return;
	}

	CString strData("");
	pView->m_pModel->GetDlgItem(IDC_EDT_THICKNESS)->GetWindowText(strData);
	pView->m_pTask->stModel.dWaperThickness = atof(strData);
	pView->m_pModel->GetDlgItem(IDC_EDT_EXCLUSION)->GetWindowText(strData);
	pView->m_pTask->stModel.dEdgeExclusion = atof(strData);

	t_ELIPSO_RESULT_INFO stcResultMeasure;
	
	*bRes = pWorkRun->SqncMeasure(stcResultMeasure);
	if (!(*bRes))
	{
		TECmsgOut(L"""측정""을 완료하지 못했습니다.", LIGHTMAGENTA, 1);
	}
	else
	{
		float fStd[MAX_LAYER + 1][MAX_ITEM + 1];
		CString strWork;
		int nSEQ = nRow - 1;
		for (int i = 0; i < stcResultMeasure.nLayerCnt; i++)
		{
			pView->m_pAuto->m_clResult.SetRow(nSEQ + 1);

			pView->m_pAuto->m_clResult.SetCol(RESULT_MSE);
			strWork.Format("%.3f", stcResultMeasure.dMSE);
			pView->m_pAuto->m_clResult.SetText(strWork);
			pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][RES_MSE] = stcResultMeasure.dMSE;
			pView->m_pTask->stData.m_dDataZ[nSEQ][i][RES_MSE] = stcResultMeasure.dMSE;

			pView->m_pAuto->m_clResult.SetCol(RESULT_TOTAL_THICKNESS);
			strWork.Format("%.3f", stcResultMeasure.dTotalThickness);
			pView->m_pAuto->m_clResult.SetText(strWork);
			pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][RES_TOTAL_THICKNESS] = stcResultMeasure.dTotalThickness;
			pView->m_pTask->stData.m_dDataZ[nSEQ][i][RES_TOTAL_THICKNESS] = stcResultMeasure.dTotalThickness;

			for (int i = 0; i < stcResultMeasure.nLayerCnt; i++)
			{
				pView->m_pAuto->m_clResult.SetCol(RESULT_LAYER_1 + i);
				strWork.Format("%s", pView->m_pAuto->m_fnGetSelectResultData(stcResultMeasure, pView->m_pAuto->m_nMeasureItem-2, i + 1));
				pView->m_pAuto->m_clResult.SetText(strWork);
			}

			pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][RES_THICKNESS] = stcResultMeasure.stLayer[i].dThickness;
			pView->m_pTask->stData.m_dDataZ[nSEQ][i][RES_THICKNESS] = stcResultMeasure.stLayer[i].dThickness;

			pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][RES_N_550] = stcResultMeasure.stLayer[i].dN_550;
			pView->m_pTask->stData.m_dDataZ[nSEQ][i][RES_N_550] = stcResultMeasure.stLayer[i].dN_550;

			pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][RES_K_550] = stcResultMeasure.stLayer[i].dK_550;
			pView->m_pTask->stData.m_dDataZ[nSEQ][i][RES_K_550] = stcResultMeasure.stLayer[i].dK_550;

			pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][RES_N_632] = stcResultMeasure.stLayer[i].dN_632;
			pView->m_pTask->stData.m_dDataZ[nSEQ][i][RES_N_632] = stcResultMeasure.stLayer[i].dN_632;

			pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][RES_K_632] = stcResultMeasure.stLayer[i].dK_632;
			pView->m_pTask->stData.m_dDataZ[nSEQ][i][RES_K_632] = stcResultMeasure.stLayer[i].dK_632;
		}
		pView->m_pTask->stResult.nLayer = stcResultMeasure.nLayerCnt;
		// 통계 데이터 계산
		for (int n = 0; n < pView->m_pAuto->m_pTask->stResult.nSRData; n++)
		{
			for (int i = 0; i < MAX_LAYER; i++)
			{
				for (int j = 0; j < MAX_ITEM; j++)
				{
					if (n == 0)
					{
						pView->m_pTask->stData.m_dMax[i][j] = pView->m_pTask->stResult.stSRData[n].dwResult[i][j];
						pView->m_pTask->stData.m_dMin[i][j] = pView->m_pTask->stResult.stSRData[n].dwResult[i][j];
						pView->m_pTask->stData.m_dAvg[i][j] = pView->m_pTask->stResult.stSRData[n].dwResult[i][j];
						fStd[i][j] = 0.0f;
						pView->m_pTask->stData.m_fTot[i][j] = 0.0f;
						pView->m_pTask->stData.m_iZeroCnt[i][j] = 0;
					}

					if (pView->m_pTask->stResult.stSRData[n].dwResult[i][j] > 0)
					{
						if (pView->m_pTask->stData.m_dMax[i][j] < pView->m_pTask->stResult.stSRData[n].dwResult[i][j])
						{
							pView->m_pTask->stData.m_dMax[i][j] = pView->m_pTask->stResult.stSRData[n].dwResult[i][j];
						}
						if (pView->m_pTask->stData.m_dMin[i][j] > pView->m_pTask->stResult.stSRData[n].dwResult[i][j])
						{
							pView->m_pTask->stData.m_dMin[i][j] = pView->m_pTask->stResult.stSRData[n].dwResult[i][j];
						}
						pView->m_pTask->stData.m_dMax_Min[i][j] = abs(pView->m_pTask->stData.m_dMax[i][j] - pView->m_pTask->stData.m_dMin[i][j]);

						pView->m_pTask->stData.m_fTot[i][j] += (float)pView->m_pTask->stResult.stSRData[n].dwResult[i][j];
						pView->m_pTask->stData.m_dAvg[i][j] = (double)(pView->m_pTask->stData.m_fTot[i][j] / (float)(n + 1 - pView->m_pTask->stData.m_iZeroCnt[i][j]));
					} 
					else
					{
						pView->m_pTask->stData.m_iZeroCnt[i][j]++;
					}
				}
			}
		}

		for (int i = 0; i < MAX_LAYER; i++)
		{
			for (int j = 0; j < MAX_ITEM; j++)
			{
				for (int n = 0; n < pView->m_pAuto->m_pTask->stResult.nSRData; n++)
				{
					if (pView->m_pTask->stResult.stSRData[n].dwResult[i][j] > 0)
						fStd[i][j] += (float)pow(pView->m_pTask->stResult.stSRData[n].dwResult[i][j] - pView->m_pTask->stData.m_dAvg[i][j], 2);
				}

				pView->m_pTask->stData.m_dStd[i][j] = sqrt((double)(fStd[i][j] / (pView->m_pAuto->m_pTask->stResult.nSRData - pView->m_pTask->stData.m_iZeroCnt[i][j])));
				pView->m_pTask->stData.m_dUniformity[i][j] = (pView->m_pTask->stData.m_dStd[i][j] / pView->m_pTask->stData.m_dAvg[i][j]);
				pView->m_pTask->stData.m_dContourMax[i][j] = pView->m_pTask->stData.m_dMax[i][j];
				pView->m_pTask->stData.m_dContourMin[i][j] = pView->m_pTask->stData.m_dMin[i][j];
				pView->m_pTask->stData.m_dContourStep[i][j] = pView->m_pTask->stData.m_dMax_Min[i][j] / 10.;
			}
		}

		pView->m_pAuto->m_fnSetInspStatus(pView->m_pTask->stData.m_dMax[pView->m_pAuto->m_nSelLayer][pView->m_pAuto->m_nMeasureItem],
			pView->m_pTask->stData.m_dMin[pView->m_pAuto->m_nSelLayer][pView->m_pAuto->m_nMeasureItem],
			pView->m_pTask->stData.m_dAvg[pView->m_pAuto->m_nSelLayer][pView->m_pAuto->m_nMeasureItem],
			pView->m_pTask->stData.m_dStd[pView->m_pAuto->m_nSelLayer][pView->m_pAuto->m_nMeasureItem],
			pView->m_pTask->stData.m_dMax_Min[pView->m_pAuto->m_nSelLayer][pView->m_pAuto->m_nMeasureItem],
			pView->m_pTask->stData.m_dUniformity[pView->m_pAuto->m_nSelLayer][pView->m_pAuto->m_nMeasureItem]);

		pView->m_pAuto->m_fnSetView();
	}

	pWorkRun->SqncCloseMeasure();
	pWorkRun->m_bThrdFlag[TF_AUTORUN] = FALSE;
	return;
}

void ThrdWaitMilProcess(BOOL bCheckVal)
{
	int i = 0;
	while (bCheckVal)
	{
		Sleep(200);
	}
}


void ThrdPattSet()
{


	return;
}



void ThrdRunning()
{
	if (pWorkRun->m_bThrdFlag[TF_AUTORUN] == TRUE)
		return;

	pWorkRun->m_bThrdFlag[TF_AUTORUN] = TRUE;


// 	AfxMessageBox(_T("Start AutoRun"), MB_ICONASTERISK);
	TECmsgOut(L"Start AutoRun", LIGHTBLUE, 1500);

	CTimeInterval CheckTime;
	CTimeInterval CheckTactTime;
	BOOL bSqncFlag = TRUE;
	BOOL bFnRes = TRUE;
	char tmpMsg[MAX_Buf] = { "Error_RunningStop" };

	CDPoint dPntTargetPos;
	double dCurPosZ = 0.0;

	t_USER_INFO stUser;
	CString aaa = "Test";

	BOOL bJustFinshFlag = FALSE;
	pWorkRun->m_bJustNowFinish = FALSE;
	BOOL bPattMappingFail = FALSE;

	//리스트 등록 좌표 개수 확인
	int iListCnt = pView->m_pTask->stCurrModel.nSRData;
	int iCurCnt = 0;

	enum//실행 순서 바꾸려면 아래 순서를 바꿀것
	{
		SQNC_BASE = 0,

		SQNC_MOVE_SUPPLY_POS,
		SQNC_STANDBY_SUPPLY,

		SQNC_INSERT_START_SIG,
		SQNC_DETECT_WAFER,

		SQNC_ALIGN_WAFER,
		SQNC_ALIGN_PATT,
		SQNC_AF,

		SQNC_READY_MEASURE,
		SQNC_AUTO_MOVE_MEASURE,
		SQNC_CLOSE_MEASURE,

		MAX_SQNC_CNT
	};
	int iSqncNum = SQNC_BASE;
	int iSqncCur = SQNC_BASE;

	while (bSqncFlag)
	{
		Sleep(100);
// 		if (!pWorkRun->m_bFinishedCassette && pWorkRun->m_bJustNowFinish)
// 		{
// 			if (iSqncCur > SQNC_INPUT_START_SIG)
// 			{
// 				iSqncCur = SQNC_BASE;
// 				iSqncNum = SQNC_BASE;
// 			}
// // 			bJustFinshFlag = pWorkRun->m_bJustNowFinish;
// // 			pWorkRun->m_bJustNowFinish = FALSE;
// 		}
// 		else
// 		{
// 			if (!bJustFinshFlag && !pWorkRun->m_bThrdFlag[TF_AUTORUN])
// 			{
// 				iSqncCur = MAX_SQNC_CNT;
// 				iSqncNum = SQNC_BASE;
// 			}
// 		}

		if (!pWorkRun->m_bThrdFlag[TF_AUTORUN])
		{
			iSqncCur = MAX_SQNC_CNT;
			iSqncNum = SQNC_BASE;
		}

		switch (iSqncNum)
		{
		case SQNC_BASE:
			if (bFnRes == FALSE)//검사중지
			{
#if _DEBUG
				AfxMessageBox(tmpMsg, MB_ICONHAND);
#endif

				if (TECmsgOut(L"정말 측정 작업을 종료 하시겠습니까?", LIGHTMAGENTA, 2))
				{
					pWorkRun->m_bThrdFlag[TF_AUTORUN] = bSqncFlag = FALSE;
				}
				else
				{
					iSqncCur--;
					bFnRes = TRUE;
				}
				break;
			}
			if (pView->m_pAuto->m_bPause)//일시정지
				break;

			if (pWorkRun->m_bJustNowFinish)
			{
				if (iSqncCur >= SQNC_INSERT_START_SIG)
				{
					iSqncCur = SQNC_BASE;
					iSqncNum = SQNC_BASE;
				}
			}

			if (iSqncCur == MAX_SQNC_CNT)//시퀀스 정상종료
			{
				pWorkRun->m_bThrdFlag[TF_AUTORUN] = bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//증가하고 대입
			break;

		case SQNC_MOVE_SUPPLY_POS:
			bFnRes = pWorkRun->SqncMoveSupplyPosition();
			iSqncNum = SQNC_BASE;
			break;

		case SQNC_STANDBY_SUPPLY:
			bFnRes = pWorkRun->SqncStandbySupply();
			iSqncNum = SQNC_BASE;
			break;

		case SQNC_INSERT_START_SIG:
			bFnRes = pWorkRun->SqncInputStartSignal();
			iSqncNum = SQNC_BASE;
			break;

		case SQNC_DETECT_WAFER:
			bFnRes = pWorkRun->SqncDetectWafer();
			if (bFnRes)
			{
				pAxis[X_AXIS]->m_dOffsetPos = 0.0;
				pAxis[Y_AXIS]->m_dOffsetPos = 0.0;
				pAxis[Z_AXIS]->m_dOffsetPos = 0.0;

				sprintf_s(pView->m_pTask->stResult.szInspTackTime, sizeof(char) * 256, "0.0 s");
				pView->m_pAuto->m_fnSetView();
				CheckTactTime.SetTimeStart();
			}
			iSqncNum = SQNC_BASE;
			break;

		case SQNC_ALIGN_WAFER:
			if (pWorkRun->m_bWaferAlign)
			{
				bFnRes = pWorkRun->SqncAlignWafer();
				if (!bFnRes)
				{
					if (TECmsgOut(L"웨이퍼 얼라인을 완료하지 못했습니다. 그래도 계속하시겠습니까?", LIGHTMAGENTA, 2))
					{
						bFnRes = TRUE;
					}
				}
			}
			iSqncNum = SQNC_BASE;
			break;

		case SQNC_ALIGN_PATT:
			if(pView->m_pAuto->m_bPattAlignMode)
			{
				if (!pWorkRun->SqncAlignPatt())
				{
					TECmsgOut(L"패턴 얼라인을 완료하지 못했습니다. 검사를 종료합니다.", LIGHTRED, 1);
					bPattMappingFail = TRUE;
				}
			}

			iSqncNum = SQNC_BASE;
			break;

			//AF
		case SQNC_AF:
			if(!pWorkRun->m_bWaferAlign && !pView->m_pAuto->m_bPattAlignMode)
				ThrdAF(pView->m_pAuto->m_ctrlChkUseAF.GetCheck());
			iSqncNum = SQNC_BASE;
			break;

		case SQNC_READY_MEASURE:
			bFnRes = pWorkRun->SqncReadyMeasure(TRUE);
			iSqncNum = SQNC_BASE;
			break;

		case SQNC_AUTO_MOVE_MEASURE:
			sprintf_s(pView->m_pTask->stResult.szInspTackTime, sizeof(char) * 256, "%.1f s", CheckTactTime.GetTimeInterval(2));
			if (!bPattMappingFail)
			{
				pView->m_pAuto->m_fnSetView();

				bFnRes = pWorkRun->SqncAutoMoveMeasure();

				sprintf_s(pView->m_pTask->stResult.szInspTackTime, sizeof(char) * 256, "%.1f s", CheckTactTime.GetTimeInterval(2));
				pView->m_pAuto->m_strWaferTimeEnd = pView->m_pAuto->m_tiAWafer.GetStrTime(pView->m_pAuto->m_tiAWafer.SetTimeEnd());
				sprintf_s(pView->m_pAuto->m_stUser.szTimeEnd, "%s", pView->m_pAuto->m_strWaferTimeEnd.GetString());
				sprintf_s(pView->m_pAuto->m_stUser.szTimeSpend, "%.3f", pView->m_pAuto->m_tiAWafer.GetTimeInterval(2, FALSE));
				if (bFnRes)
				{

					pView->m_pAuto->m_bFirstInsp = TRUE;
					pView->m_pAuto->m_fnSetUserInput(&stUser);
					memcpy(&pView->m_pAuto->m_pTask->stResult.stUser, &pView->m_pAuto->m_stUser, sizeof(t_USER_INFO));
					pView->m_pAuto->m_fnSetXMLResult(pView->m_pAuto->m_pTask->stLastModel.szLastModel, pView->m_pAuto->m_pTask->stResult, pView->m_pAuto->m_pTask->stData, SW_AUTO);
				}
			}
			else
			{
				pView->m_pAuto->m_fnSetUserInput(&stUser);
				memcpy(&pView->m_pAuto->m_pTask->stResult.stUser, &pView->m_pAuto->m_stUser, sizeof(t_USER_INFO));
				pView->m_pAuto->m_fnSetXMLResult(pView->m_pAuto->m_pTask->stLastModel.szLastModel, pView->m_pAuto->m_pTask->stResult, pView->m_pAuto->m_pTask->stData, SW_AUTO);
			}
			pView->m_pAuto->m_fnSetView();
			iSqncNum = SQNC_BASE;
			break;

		case SQNC_CLOSE_MEASURE:
			bFnRes = pWorkRun->SqncCloseMeasure();
			
			iSqncCur = SQNC_BASE;
			iSqncNum = SQNC_BASE;
			break;

		case MAX_SQNC_CNT:
			pWorkRun->m_bThrdFlag[TF_AUTORUN] = bSqncFlag = FALSE;
			break;
		}
	}

	//OUT6 ON
	pView->m_pIO->m_fnSetOutputSignal(MEASURE_READY, FALSE);
	//OUT7 OFF
	pView->m_pIO->m_fnSetOutputSignal(MEASURE_BUSY, FALSE);

// 	if (!bFnRes)
// 	{
// 		AfxMessageBox(tmpMsg, MB_ICONHAND);
// 	}
// 	else
	{
	// 	AfxMessageBox(_T("End AutoRun"), MB_ICONASTERISK);
		TECmsgOut(L"측정 작업 완료", LIGHTBLUE, 1);
	}

	EnableDlgItem(pView->m_pModel, TRUE);//모든버튼 활성
// 	EnableDlgItem(pView->m_pAuto, TRUE);//모든버튼 활성
	pView->m_pAuto->m_fnSetBtnEnable(1);
	pView->m_pAuto->SendMessage(UM_RUN_BTN_STATUS, SW_INSP_END);
	return;
}


void ThrdPattTest(int iStep)
{
	pWorkRun->m_bThrdFlag[TF_AUTORUN] = TRUE;

	switch (iStep)
	{
	case 0:
		pWorkRun->SqncPattReg();
		break;
	case 1:
		pWorkRun->SqncSubReg();
		break;
	case 2:
		pWorkRun->SqncAlignMapping();
		break;
	case 3:
		pWorkRun->SqncTargetReg();
		break;
	default:
		break;
	}
	EnableDlgItem(pView->m_pModel, TRUE);

	pWorkRun->m_bThrdFlag[TF_AUTORUN] = FALSE;
}

void ThrdWaferAlignTest()
{
	pWorkRun->m_bThrdFlag[TF_AUTORUN] = TRUE;
	pWorkRun->SqncAlignWafer();
	pWorkRun->m_bThrdFlag[TF_AUTORUN] = FALSE;
}

CWorkRun::CWorkRun()
{
	pView = theApp.m_fnGetViewPointer();

	memset(m_bThrdFlag, 0x00, sizeof(BOOL)*MAX_TF_CNT);
	memset(m_dPntForCircle, 0x00, sizeof(m_dPntForCircle));
	memset(m_bSqncInterrupt, 0x00, sizeof(BOOL)*MAX_SQITRT_CNT);

	m_bAutoMode = TRUE;
	m_bZzeroMove = FALSE;
// 	m_bAutoMode2 = TRUE;
	m_bWaferAlign = TRUE;
	m_bCompleteWaferInput = FALSE;
	m_bPattInfo = FALSE;
	m_bTargetInfo = FALSE;
	m_bTargetPattRegComplete = FALSE;

	pMakeObj = (CMakeObject*) new CMakeObject();

	m_bFinishedCassette = FALSE;
	m_bJustNowFinish = FALSE;

	m_bInitManualData = TRUE;
}

CWorkRun::~CWorkRun()
{
	delete pMakeObj;
}

BOOL CWorkRun::SqncVisionAF(BOOL bVisionAF)
{
	CTimeInterval CheckTime;

	BOOL bSqncFlag = TRUE;
	BOOL bFnRes = TRUE;
	char tmpMsg[MAX_Buf] = { "Error_SqncMoveSupplyPosition" };

	BOOL bCamDlg = TRUE;

	enum//실행 순서 바꾸려면 아래 순서를 바꿀것
	{
		SQNC_BASE = 0,
		SQNC_MOVE_Z_INIT,
		SQNC_READY_CAM,
		SQNC_MOVE_Z_SCAN,
		SQNC_MOVE_Z_INIT2,
		SQNC_MOVE_Z_FINEFOCUS,

		MAX_SQNC_CNT
	};
	int iSqncNum = SQNC_BASE;
	int iSqncCur = SQNC_BASE;



	int iAxisNo = Z_AXIS;
	double dMovePos;

	int iFocusCnt = 0;

	int iFocuseVal = 0;
	double dFocusePos = 0.0;


	vector<pair<int, double> > vFocuse;
	

	TECmsgOut(m_Util.CStringTobstr(_T("Start AutoFocus")), LIGHTGREEN, 500);

	while (bSqncFlag)
	{
		Sleep(30);
		if (!pWorkRun->m_bThrdFlag[TF_AUTOFOCUS])
		{
			iSqncCur = MAX_SQNC_CNT;
			iSqncNum = SQNC_BASE;
		}

		switch (iSqncNum)
		{
		case SQNC_BASE:
			if (bFnRes == FALSE)//검사중지
			{
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//일시정지
				break;

			if (iSqncCur == MAX_SQNC_CNT)//시퀀스 정상종료
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//증가하고 대입
			break;

		case SQNC_MOVE_Z_INIT:
			pAxis[iAxisNo]->m_fSpeedFlag = 1.f;
			pAxis[iAxisNo]->fVelocity = (float)pView->m_pTask->stCommon.dInspZSpeed;

			if (bVisionAF)
			{
				dMovePos = pView->m_pModel->m_fnGetAlignZ(pView->m_pTask->stResult.dWaperThickness) + 0.3;
			}
			else
			{
				dMovePos = pView->m_pModel->m_fnGetAlignZ(pView->m_pTask->stResult.dWaperThickness);
				iSqncCur = MAX_SQNC_CNT-1;//종료시퀀스
			}

			pAxis[iAxisNo]->MoveAxis(dMovePos, 0, TRUE);
			iSqncNum = SQNC_BASE;
			break;

		case SQNC_READY_CAM:
			LightControl(0, 1, pView->m_pTask->stCommon.nInspLight);
			if (!pCamJAI->m_bUseUI && !pCamJAI->m_bStreamStarted)
			{
				pCamJAI->SendMessage(WM_COMMAND, IDC_START, 0);
				bCamDlg = FALSE;
			}
			iSqncNum = SQNC_BASE;
			break;

		case SQNC_MOVE_Z_SCAN:
			pAxis[iAxisNo]->fVelocity = (float)pView->m_pTask->stCommon.dSpeedSlowZ*0.7f;

			dMovePos = pView->m_pModel->m_fnGetAlignZ(pView->m_pTask->stResult.dWaperThickness) - 1.7;
			pAxis[iAxisNo]->MoveAxis(dMovePos, 0, TRUE);

			while (pAxis[iAxisNo]->CheckInMotion())
			{
				pAxis[iAxisNo]->StatusGetPos(&dFocusePos);
				iFocuseVal = (int)pCamJAI->m_lResultFocus;

				vFocuse.push_back(pair<int, double>(iFocuseVal, dFocusePos));
				iFocusCnt++;
			}
			sort(vFocuse.begin(), vFocuse.end(), desc);

			// strTemp.Format("FocusVal/Z_POS : %d/%.3f : %d/%.3f : %d/%.3f)"
			// 	, vFocuse[0].first, vFocuse[0].second
			// 	, vFocuse[1].first, vFocuse[1].second
			// 	, vFocuse[2].first, vFocuse[2].second
			// );
			// TECmsgOut(m_Util.CStringTobstr(strTemp), LIGHTGRAY, 1);

			iSqncNum = SQNC_BASE;
			break;

		case SQNC_MOVE_Z_INIT2:
			if (vFocuse.size() <= 1)
			{
				TECmsgOut(L"Vision AutoFocus Failed", LIGHTMAGENTA, 1);
				bFnRes = FALSE;
			}
			else
			{
				pAxis[iAxisNo]->fVelocity = (float)pView->m_pTask->stCommon.dInspZSpeed;

				dMovePos = pView->m_pModel->m_fnGetAlignZ(pView->m_pTask->stResult.dWaperThickness) + 0.3;
				pAxis[iAxisNo]->MoveAxis(dMovePos, 0, TRUE);
			}
			iSqncNum = SQNC_BASE;
			break;

		case SQNC_MOVE_Z_FINEFOCUS:
			pAxis[iAxisNo]->fVelocity = (float)pView->m_pTask->stCommon.dInspZSpeed;
			pAxis[iAxisNo]->MoveAxis(vFocuse[0].second, 0, TRUE);

			if(!bCamDlg)
				pCamJAI->SendMessage(WM_COMMAND, IDC_STOP, 0);

			iSqncNum = SQNC_BASE;
			break;

		case MAX_SQNC_CNT:
			bSqncFlag = FALSE;
			break;
		}

		//Image Buffer Alloc
		// 		MIL_ID saveImg;
		// 		M_Alloc8(pCamJAI->m_CnvImageInfo.iSizeX, pCamJAI->m_CnvImageInfo.iSizeY, saveImg);
		// 		MbufPut(saveImg, pCamJAI->m_CnvImageInfo.pImageBuffer);
		// 		MbufExport(_T("D:\\EllipsoMeter\\FineFocusImg.bmp"), M_BMP, saveImg);
		// 		MbufFree(saveImg);

		// 		if (!pCamJAI->m_bUseUI)
		// 			pCamJAI->SendMessage(WM_COMMAND, IDC_STOP, 0);
		// 		if(pWorkRun->m_bThrdFlag[TF_AUTORUN])
		// 			LightControl(0, 1, 0);
	}


	if (!bFnRes)
	{
#if _DEBUG
		AfxMessageBox(tmpMsg, MB_ICONHAND);
#endif
	}
	else
		TECmsgOut(L"End AutoFocus", LIGHTBLUE, 500);

	return bFnRes;

}

BOOL CWorkRun::SqncMoveSupplyPosition()
{
	CDPoint tmpSupplyPos;
	BOOL bSqncFlag = TRUE;
	BOOL bFnRes = TRUE;
	char tmpMsg[MAX_Buf] = { "Error_SqncMoveSupplyPosition" };

	if (m_bAutoMode)//자동
	{
		tmpSupplyPos.x = pView->m_pTask->stCommon.dLoadingX;
		tmpSupplyPos.y = pView->m_pTask->stCommon.dLoadingY;
	}
	else//수동
	{
		tmpSupplyPos.x = pView->m_pTask->stCommon.dLoadingX2;
		tmpSupplyPos.y = pView->m_pTask->stCommon.dLoadingY2;
	}

	switch (pView->m_pAuto->m_nWaperSize)
	{
	case WAPER_4:
		pView->m_pIO->m_fnSetOutputSignal(WAFER_0B01, FALSE);
		pView->m_pIO->m_fnSetOutputSignal(WAFER_0B10, FALSE);
		break;
	case WAPER_6:
		pView->m_pIO->m_fnSetOutputSignal(WAFER_0B01, TRUE);
		pView->m_pIO->m_fnSetOutputSignal(WAFER_0B10, FALSE);
		break;
	case WAPER_8:
		pView->m_pIO->m_fnSetOutputSignal(WAFER_0B01, FALSE);
		pView->m_pIO->m_fnSetOutputSignal(WAFER_0B10, TRUE);
		break;
	case WAPER_12:
		pView->m_pIO->m_fnSetOutputSignal(WAFER_0B01, TRUE);
		pView->m_pIO->m_fnSetOutputSignal(WAFER_0B10, TRUE);
		break;
	}


	enum//실행 순서 바꾸려면 아래 순서를 바꿀것
	{
		SQNC_BASE = 0,
		SQNC_MOVE_Z,
		SQNC_MOVE_XY,

		MAX_SQNC_CNT
	};
	int iSqncNum = SQNC_BASE;
	int iSqncCur = SQNC_BASE;

	while (bSqncFlag)
	{
		Sleep(100);
		if (!pWorkRun->m_bThrdFlag[TF_AUTORUN])
		{
			iSqncCur = MAX_SQNC_CNT;
			iSqncNum = SQNC_BASE;
		}

		switch (iSqncNum)
		{
		case SQNC_BASE:
			if (bFnRes == FALSE)//검사중지
			{
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//일시정지
				break;

			if (iSqncCur == MAX_SQNC_CNT)//시퀀스 정상종료
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//증가하고 대입
			break;

		case SQNC_MOVE_Z:
			pAxis[X_AXIS]->fVelocity = (float)pView->m_pTask->stCommon.dInspXYSpeed;
			pAxis[Y_AXIS]->fVelocity = (float)pView->m_pTask->stCommon.dInspXYSpeed;
			if (pView->m_pAuto->m_nWaperSize == WAPER_4)
			{
				pAxis[X_AXIS]->fVelocity *= 0.06f;
				pAxis[Y_AXIS]->fVelocity *= 0.06f;
			}

			pAxis[Z_AXIS]->fVelocity = (float)pView->m_pTask->stCommon.dInspZSpeed;

			bFnRes = pAxis[Z_AXIS]->MoveAxis(pView->m_pTask->stCommon.dZReadySafety, 0, TRUE);

			iSqncNum = SQNC_BASE;
			break;

		case SQNC_MOVE_XY:
// 			if (pView->m_pIO->m_pTask->stElipsoIO.stIn.EFEM_Run)
// 				break;

			bFnRes = pAxis[X_AXIS]->MoveAxis(tmpSupplyPos.x, 0, TRUE, FALSE);
			if(bFnRes)
				bFnRes = pAxis[Y_AXIS]->MoveAxis(tmpSupplyPos.y, 0, TRUE, FALSE);

			iSqncNum = SQNC_BASE;
			break;

		case MAX_SQNC_CNT:
			bSqncFlag = FALSE;
			break;
		}
	}

	if (!bFnRes)
	{
#if _DEBUG
		AfxMessageBox(tmpMsg, MB_ICONHAND);
#endif
	}
	else
		TECmsgOut(L"스테이지 투입위치 이동 완료 ", YELLOW, 200, 17, BLACK);

	return bFnRes;
}

BOOL CWorkRun::SqncStandbySupply()
{
	CTimeInterval CheckTime;
	BOOL bSqncFlag = TRUE;
	BOOL bFnRes = TRUE;
	char tmpMsg[MAX_Buf] = { "Error_SqncStandbySupply" };

	double dWaferLineMotorPos[2];

	enum//실행 순서 바꾸려면 아래 순서를 바꿀것
	{
		SQNC_BASE = 0,
		SQNC_VACUUM_OFF,
		SQNC_VACUUM_TIMEOUT,
		SQNC_SIGNAL_READY,

		MAX_SQNC_CNT
	};
	int iSqncNum = SQNC_BASE;
	int iSqncCur = SQNC_BASE;

	while (bSqncFlag)
	{
		Sleep(100);
		if (!pWorkRun->m_bThrdFlag[TF_AUTORUN])
		{
			iSqncCur = MAX_SQNC_CNT;
			iSqncNum = SQNC_BASE;
		}

		switch (iSqncNum)
		{
		case SQNC_BASE:
			if (bFnRes == FALSE)//검사중지
			{
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//일시정지
				break;

			if (iSqncCur == MAX_SQNC_CNT)//시퀀스 정상종료
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//증가하고 대입
			break;

		case SQNC_VACUUM_OFF:
			pView->m_pTask->stElipsoIO.stOut.Vacuum_On_06_Inch = FALSE;
			pView->m_pTask->stElipsoIO.stOut.Vacuum_On_08_Inch = FALSE;
			pView->m_pTask->stElipsoIO.stOut.Vacuum_On_12_Inch = FALSE;
			pView->m_pAjin->m_fnSetAxdoWriteOutportBit(ELIPSO_IO_OUTPUT_1, VACUUM_ON_06_INCH, pView->m_pTask->stElipsoIO.stOut.Vacuum_On_06_Inch);
			pView->m_pAjin->m_fnSetAxdoWriteOutportBit(ELIPSO_IO_OUTPUT_1, VACUUM_ON_08_INCH, pView->m_pTask->stElipsoIO.stOut.Vacuum_On_08_Inch);
			pView->m_pAjin->m_fnSetAxdoWriteOutportBit(ELIPSO_IO_OUTPUT_1, VACUUM_ON_12_INCH, pView->m_pTask->stElipsoIO.stOut.Vacuum_On_12_Inch);
			pView->SendMessageA(UM_VAC_STATUS, 0, 0);
			CheckTime.SetTimeStart();

			iSqncNum = SQNC_BASE;
			break;

		case SQNC_VACUUM_TIMEOUT:
			if (CheckTime.GetTimeInterval() > 8000)
			{
				if(!TECmsgOut(L"Stage Vacuum 해제 실패-TimeOut. 무시하고 계속 진행하시겠습니까?", LIGHTRED, 2))
				{
					TECmsgOut(L"Stage Vacuum 상태를 확인하고 계속해 주세요.", LIGHTMAGENTA, 1);
					CheckTime.SetTimeStart();
					break;
				}
// 				bFnRes = FALSE;
// 				iSqncCur = MAX_SQNC_CNT;//시퀀스 종료
			}
			else
			{
				if (
					pView->m_pIO->m_pTask->stElipsoIO.stIn.Wafer_Vacuum_Sensor_06_Inch
					|| pView->m_pIO->m_pTask->stElipsoIO.stIn.Wafer_Vacuum_Sensor_08_Inch
					|| pView->m_pIO->m_pTask->stElipsoIO.stIn.Wafer_Vacuum_Sensor_12_Inch
					)
					break;
			}

			iSqncNum = SQNC_BASE;
			break;

		case SQNC_SIGNAL_READY:
			if (CheckTime.GetTimeInterval() < 500)//500ms이상 대기(핸드쉐이크)
				break;
			//IN4 OFF 대기
			if (pView->m_pIO->m_pTask->stElipsoIO.stIn.EFEM_Finish)
				break;

			if (m_bAutoMode)//자동
			{
				pAxis[X_AXIS]->StatusGetPos(&dWaferLineMotorPos[0]);
				pAxis[Y_AXIS]->StatusGetPos(&dWaferLineMotorPos[1]);
				if (fabs(dWaferLineMotorPos[0]+ pAxis[X_AXIS]->m_dOffsetPos - pView->m_pTask->stCommon.dLoadingX) < 1.0
					&& fabs(dWaferLineMotorPos[1]+ pAxis[Y_AXIS]->m_dOffsetPos - pView->m_pTask->stCommon.dLoadingY) < 1.0)
				{
					//OUT6 ON
					pView->m_pIO->m_fnSetOutputSignal(MEASURE_READY, TRUE);
					//OUT7 OFF
					pView->m_pIO->m_fnSetOutputSignal(MEASURE_BUSY, FALSE);
				}
				else
				{
					TECmsgOut(L"Stage가 지정된 위치로 이동하지 못했습니다. X,Y Stage 상태를 확인하고 다시 시작해 주세요.", LIGHTMAGENTA, 1);
					bFnRes = FALSE;
				}
			}
			else
			{
				//OUT6 ON
				pView->m_pIO->m_fnSetOutputSignal(MEASURE_READY, FALSE);
				//OUT7 OFF
				pView->m_pIO->m_fnSetOutputSignal(MEASURE_BUSY, FALSE);
			}

			iSqncNum = SQNC_BASE;
			break;

		case MAX_SQNC_CNT:
			bSqncFlag = FALSE;
			break;
		}
	}

	if (!bFnRes)
	{
#if _DEBUG
		AfxMessageBox(tmpMsg, MB_ICONHAND);
#endif
	}
	else
		TECmsgOut(L"스테이지 투입 준비(진공/신호) 완료 ", YELLOW, 200, 17, BLACK);

	return bFnRes;
}

BOOL CWorkRun::SqncInputStartSignal()
{
	CTimeInterval CheckTime;
	BOOL bSqncFlag = TRUE;
	BOOL bFnRes = TRUE;
	char tmpMsg[MAX_Buf] = { "Error_SqncInputStartSignal" };
	CString strSlotNum;

	enum//실행 순서 바꾸려면 아래 순서를 바꿀것
	{
		SQNC_BASE = 0,
		SQNC_CHECK_INPUT,
		SQNC_CHECK_SOCKET,
		SQNC_SIGNAL_BUSY,

		MAX_SQNC_CNT
	};
	int iSqncNum = SQNC_BASE;
	int iSqncCur = SQNC_BASE;


	switch (pView->m_pAuto->m_nWaperSize)
	{
	case WAPER_4:
		pView->m_pIO->m_fnSetOutputSignal(WAFER_0B01, FALSE);
		pView->m_pIO->m_fnSetOutputSignal(WAFER_0B10, FALSE);
		break;
	case WAPER_6:
		pView->m_pIO->m_fnSetOutputSignal(WAFER_0B01, TRUE);
		pView->m_pIO->m_fnSetOutputSignal(WAFER_0B10, FALSE);
		break;
	case WAPER_8:
		pView->m_pIO->m_fnSetOutputSignal(WAFER_0B01, FALSE);
		pView->m_pIO->m_fnSetOutputSignal(WAFER_0B10, TRUE);
		break;
	case WAPER_12:
		pView->m_pIO->m_fnSetOutputSignal(WAFER_0B01, TRUE);
		pView->m_pIO->m_fnSetOutputSignal(WAFER_0B10, TRUE);
		break;
	default:
		pView->m_pIO->m_fnSetOutputSignal(WAFER_0B01, FALSE);
		pView->m_pIO->m_fnSetOutputSignal(WAFER_0B10, FALSE);
		break;
	}

	while (bSqncFlag)
	{
		Sleep(100);

		if (m_bFinishedCassette)
		{
			m_bFinishedCassette = FALSE;
			pWorkRun->m_bThrdFlag[TF_AUTORUN] = FALSE;
		}

		if (!pWorkRun->m_bThrdFlag[TF_AUTORUN])
		{
			iSqncCur = MAX_SQNC_CNT;
			iSqncNum = SQNC_BASE;
		}



		switch (iSqncNum)
		{
		case SQNC_BASE:
			if (bFnRes == FALSE )//검사중지
			{
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//일시정지
				break;

			if (iSqncCur == MAX_SQNC_CNT)//시퀀스 정상종료
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//증가하고 대입
			break;

		case SQNC_CHECK_INPUT:
			if (pWorkRun->m_bJustNowFinish)
			{
				pWorkRun->m_bJustNowFinish = FALSE;
			}

			if (m_bAutoMode)//자동
			{
				if (!pView->m_pIO->m_pTask->stElipsoIO.stIn.EFEM_Finish
					|| pView->m_pIO->m_pTask->stElipsoIO.stIn.EFEM_Run)
					break;
			}
			else//수동
			{
// 				TECmsgOut(L"웨이퍼를 스테이지에 직접 올린 후 ""확인""을 누르면 시작합니다. ", LIGHTMAGENTA, 1);
				if (!TECmsgOut(L"웨이퍼를 스테이지에 직접 올린 후 [Yes]을 누르면 시작합니다. [No]를 누르면 종료합니다", LIGHTMAGENTA, 2))
					bFnRes = FALSE;
			}

			iSqncNum = SQNC_BASE;
			break;

		case SQNC_CHECK_SOCKET:
			if (m_bAutoMode)//자동
			{
// 				pView->m_pAuto->m_ctrlEditWaferNum.GetWindowTextA(strSlotNum);
// 				pView->m_pAuto->m_stUser.iSlotNo = atoi(strSlotNum.GetString());
				//소켓통신 Recv 확인
				//-카세트 시작/끝
				//-웨이퍼 번호
				//x로봇통신쪽에 구현완료
			}
			else//수동
			{
				pView->m_pAuto->m_strInspDate = m_Util.m_fnGetDate();
				sprintf_s(pView->m_pAuto->m_stUser.szDate, "%s", pView->m_pAuto->m_strInspDate.GetString());
				pView->m_pAuto->m_strLotStartTime = m_Util.m_fnGetTime24();
				m_bThrdFlag[TF_CASSETTE_START] = TRUE;
			}
			theApp.m_fnGetViewPointer()->m_pAuto->m_strWaferTimeStart = theApp.m_fnGetViewPointer()->m_pAuto->m_tiAWafer.GetStrTime(theApp.m_fnGetViewPointer()->m_pAuto->m_tiAWafer.SetTimeStart());
			sprintf_s(theApp.m_fnGetViewPointer()->m_pAuto->m_stUser.szTimeStart, "%s", theApp.m_fnGetViewPointer()->m_pAuto->m_strWaferTimeStart.GetString());

			switch (pView->m_pAuto->m_nWaperSize)
			{
			case WAPER_4:
				pView->m_pIO->m_fnSetOutputSignal(WAFER_0B01, FALSE);
				pView->m_pIO->m_fnSetOutputSignal(WAFER_0B10, FALSE);
				break;
			case WAPER_6:
				pView->m_pIO->m_fnSetOutputSignal(WAFER_0B01, TRUE);
				pView->m_pIO->m_fnSetOutputSignal(WAFER_0B10, FALSE);
				break;
			case WAPER_8:
				pView->m_pIO->m_fnSetOutputSignal(WAFER_0B01, FALSE);
				pView->m_pIO->m_fnSetOutputSignal(WAFER_0B10, TRUE);
				break;
			case WAPER_12:
				pView->m_pIO->m_fnSetOutputSignal(WAFER_0B01, TRUE);
				pView->m_pIO->m_fnSetOutputSignal(WAFER_0B10, TRUE);
				break;
			default:
				pView->m_pIO->m_fnSetOutputSignal(WAFER_0B01, FALSE);
				pView->m_pIO->m_fnSetOutputSignal(WAFER_0B10, FALSE);
				break;
			}

			iSqncNum = SQNC_BASE;
			break;

		case SQNC_SIGNAL_BUSY:
			//OUT6 OFF
			pView->m_pIO->m_fnSetOutputSignal(MEASURE_READY, FALSE);
			//OUT7 ON
			pView->m_pIO->m_fnSetOutputSignal(MEASURE_BUSY, TRUE);

			iSqncNum = SQNC_BASE;
			break;

		case MAX_SQNC_CNT:
			bSqncFlag = FALSE;
			break;
		}
	}

	if (!bFnRes)
	{
#if _DEBUG
		AfxMessageBox(tmpMsg, MB_ICONHAND);
#endif
	}
	else
		TECmsgOut(L"작업시작 신호 확인 완료 ", YELLOW, 200, 17, BLACK);

	return bFnRes;
}

BOOL CWorkRun::SqncDetectWafer()
{
	CTimeInterval CheckTime;
	CDPoint tmpSupplyPos;
	BOOL bSqncFlag = TRUE;
	BOOL bFnRes = TRUE;
	char tmpMsg[MAX_Buf] = { "Error_SqncDetectWafer" };

	enum//실행 순서 바꾸려면 아래 순서를 바꿀것
	{
		SQNC_BASE = 0,
		SQNC_DETECT_WAFER_SENSOR,
		SQNC_VACUUM_ON,
		SQNC_CHECK_VACUUM_ON,
		SQNC_MOVE_CIRCLE_CENTER,

		MAX_SQNC_CNT
	};
	int iSqncNum = SQNC_BASE;
	int iSqncCur = SQNC_BASE;

	int iVacuumOn[3] = { VACUUM_ON_06_INCH , VACUUM_ON_08_INCH , VACUUM_ON_12_INCH };

	while (bSqncFlag)
	{
		Sleep(100);
		if (!pWorkRun->m_bThrdFlag[TF_AUTORUN])
		{
			iSqncCur = MAX_SQNC_CNT;
			iSqncNum = SQNC_BASE;
		}

		switch (iSqncNum)
		{
		case SQNC_BASE:
			if (bFnRes == FALSE)//검사중지
			{
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//일시정지
				break;

			if (pWorkRun->m_bJustNowFinish)
			{
				bSqncFlag = FALSE;
				break;
			}

			if (iSqncCur == MAX_SQNC_CNT)//시퀀스 정상종료
			{
// 				ThrdAF();
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//증가하고 대입
			break;

		case SQNC_DETECT_WAFER_SENSOR:
			if (pView->m_pIO->m_pTask->stElipsoIO.stIn.Wafer_Detect_Sensor)
			{
				//OUT6 ON
				pView->m_pIO->m_fnSetOutputSignal(MEASURE_READY, FALSE);
				//OUT7 OFF
				pView->m_pIO->m_fnSetOutputSignal(MEASURE_BUSY, FALSE);

				TECmsgOut(L"스테이지 위에 웨이퍼를 정가운데 올리고 [Yes]를 눌러주세요. ", LIGHTMAGENTA, 1);
				break;
			}
			//OUT6 OFF
			pView->m_pIO->m_fnSetOutputSignal(MEASURE_READY, FALSE);
			//OUT7 ON
			pView->m_pIO->m_fnSetOutputSignal(MEASURE_BUSY, TRUE);

			if (pView->m_pMap->m_nWaperSize == 0)
			{
				iSqncCur = MAX_SQNC_CNT;//시퀀스 종료
			}
			else if (pView->m_pMap->m_nWaperSize > WAPER_12)
			{
				bFnRes = FALSE;
				AfxMessageBox("Wafer Size 세팅 에러.");
			}


			iSqncNum = SQNC_BASE;
			break;

		case SQNC_VACUUM_ON:
			//Vacuum On
			pView->m_pIO->m_fnSetOutputSignal(iVacuumOn[pView->m_pMap->m_nWaperSize - 1], TRUE);
			pView->SendMessageA(UM_VAC_STATUS, 1, 0);

			CheckTime.SetTimeStart();

			iSqncNum = SQNC_BASE;
			break;

		case SQNC_CHECK_VACUUM_ON:
			if (CheckTime.GetTimeInterval() < 500)//500ms이상 대기
				break;

			if (CheckTime.GetTimeInterval() > 8000)//TimeOut
			{
// 				AfxMessageBox("Stage Vacuum 동작 실패-TimeOut");
				if (!TECmsgOut(L"Stage Vacuum 동작 실패-TimeOut 2. 무시하고 계속 진행하시겠습니까?", LIGHTRED, 2))
				{
					TECmsgOut(L"Stage Vacuum 상태를 확인하고 계속해 주세요.", LIGHTMAGENTA, 1);
					CheckTime.SetTimeStart();
					break;
// 					bFnRes = FALSE;
// 					iSqncCur = MAX_SQNC_CNT;//시퀀스 종료
				}
			}
			else
			{
				if(pView->m_pAuto->m_nWaperSize != WAPER_4)
				{
					if (
						!pView->m_pIO->m_pTask->stElipsoIO.stIn.Wafer_Vacuum_Sensor_06_Inch
						&& !pView->m_pIO->m_pTask->stElipsoIO.stIn.Wafer_Vacuum_Sensor_08_Inch
						&& !pView->m_pIO->m_pTask->stElipsoIO.stIn.Wafer_Vacuum_Sensor_12_Inch
						)
						break;
				}
			}
			iSqncNum = SQNC_BASE;
			break;

			//원중심 이동.
		case SQNC_MOVE_CIRCLE_CENTER:
			bFnRes = pAxis[X_AXIS]->MoveAxis(0, 0, TRUE);
			if (bFnRes)
				bFnRes = pAxis[Y_AXIS]->MoveAxis(0, 0, TRUE);
			iSqncNum = SQNC_BASE;
			break;

		case MAX_SQNC_CNT:
// 			ThrdAF();

			bSqncFlag = FALSE;
			break;
		}
	}

	if (!bFnRes)
	{
#if _DEBUG
		AfxMessageBox(tmpMsg, MB_ICONHAND);
#endif
	}
	else
		TECmsgOut(L"웨이퍼 투입 인식 확인.", LIGHTBLUE, 2000);

	return bFnRes;
}

BOOL CWorkRun::SqncAlignWafer()
{
	CTimeInterval CheckTime;
	BOOL bSqncFlag = TRUE;
	BOOL bFnRes = TRUE;
	char tmpMsg[MAX_Buf] = { "Error_SqncAlignWafer" };

	double dCalcCenter[3];
	double dCalcRadius;

	pAxis[X_AXIS]->m_dOffsetPos = 0.0;
	pAxis[Y_AXIS]->m_dOffsetPos = 0.0;
	pAxis[Z_AXIS]->m_dOffsetPos = 0.0;

	pView->m_pStageMap->m_fnSetRunningCamera(TRUE);

	enum//실행 순서 바꾸려면 아래 순서를 바꿀것
	{
		SQNC_BASE = 0,

		SQNC_AF,
		SQNC_EDGE_1,
		SQNC_EDGE_2,
		SQNC_EDGE_3,
		SQNC_3P_CALC_CIRCLE_CENTER,
		SQNC_MOVE_CIRCLE_CENTER,
		SQNC_ZEROSET_XY,

		MAX_SQNC_CNT
	};
	int iSqncNum = SQNC_BASE;
	int iSqncCur = SQNC_BASE;

	while (bSqncFlag)
	{
		Sleep(100);
		if (!pWorkRun->m_bThrdFlag[TF_AUTORUN])
		{
			iSqncCur = MAX_SQNC_CNT;
			iSqncNum = SQNC_BASE;
		}

		switch (iSqncNum)
		{
		case SQNC_BASE:
			if (bFnRes == FALSE)//검사중지
			{
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//일시정지
				break;

			if (pWorkRun->m_bJustNowFinish)
			{
				bSqncFlag = FALSE;
				break;
			}

			if (iSqncCur == MAX_SQNC_CNT)//시퀀스 정상종료
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//증가하고 대입
			break;

			//AF
		case SQNC_AF:
			ThrdAF(pView->m_pAuto->m_ctrlChkUseAF.GetCheck());
			iSqncNum = SQNC_BASE;
			break;

			//11시 엣지검색
		case SQNC_EDGE_1:
			bFnRes = SqncSearchEdge(0);
			iSqncNum = SQNC_BASE;
			break;

		// 9시 엣지검색
		case SQNC_EDGE_2:
			bFnRes = SqncSearchEdge(1);
			iSqncNum = SQNC_BASE;
			break;

		// 7시 엣지검색
		case SQNC_EDGE_3:
			bFnRes = SqncSearchEdge(2);
			iSqncNum = SQNC_BASE;
			break;

		//3점 원중심 계산
		case SQNC_3P_CALC_CIRCLE_CENTER:
			bFnRes = pMakeObj->CalculateCircle3(m_dPntForCircle, dCalcCenter, &dCalcRadius);
			iSqncNum = SQNC_BASE;
			break;

		//원중심 이동.
		case SQNC_MOVE_CIRCLE_CENTER:
			pAxis[X_AXIS]->m_dOffsetPos = dCalcCenter[0];
			pAxis[Y_AXIS]->m_dOffsetPos = dCalcCenter[1];

			bFnRes = pAxis[X_AXIS]->MoveAxis(0, 0, TRUE);
			if (bFnRes)
				bFnRes = pAxis[Y_AXIS]->MoveAxis(0, 0, TRUE);
			iSqncNum = SQNC_BASE;
			break;

		//x-y축 현재좌표 0점set
		case SQNC_ZEROSET_XY:
// 			bFnRes = pAxis[X_AXIS]->ZeroSet();
// 			if (bFnRes)
// 				bFnRes = pAxis[Y_AXIS]->ZeroSet();

			if (!bFnRes)
				sprintf_s(tmpMsg, sizeof(tmpMsg), "%s-ZeroSet", tmpMsg);
			iSqncNum = SQNC_BASE;
			break;

		case MAX_SQNC_CNT:
			bSqncFlag = FALSE;
			break;
		}
	}

	if (!bFnRes)
	{
#if _DEBUG
		AfxMessageBox(tmpMsg, MB_ICONHAND);
#endif
	}
	else
		TECmsgOut(L"웨이퍼 얼라인 완료", YELLOW, 200, 17, BLACK);

	pView->m_pStageMap->m_fnSetRunningCamera(FALSE);

	return bFnRes;
}

BOOL CWorkRun::SqncSearchEdge(int iSearchDirection)
{
#if JAI_CAMERA
	CTimeInterval CheckTime;
	BOOL bSqncFlag = TRUE;
	BOOL bFnRes = TRUE;
	char tmpMsg[MAX_Buf] = { "Error_SqncSearchEdge" };

	BOOL bFindWaferLine = FALSE;
	double dWaferLineMotorPos[2];

	int arrWaferInch[4] = { 4,6,8,12 };
	double dMM = 0.0;

	pView->m_pAuto->m_nWaperSize = pView->m_pAuto->m_clWaperSize.GetCurSel();

	pView->m_pModel->m_fnGetInchToMM(arrWaferInch[pView->m_pAuto->m_nWaperSize] + 1, dMM, 0.0);
	CDPoint dPntLimit[3];
	//11시
	dPntLimit[0].x = (-1.0)*dMM*cos(PI/6);	//pView->m_pTask->stCommon.dXMLimit;
	dPntLimit[0].y = (-1.0)*dMM*sin(PI/6);	//pView->m_pTask->stCommon.dYMLimit;
	//6시
	dPntLimit[1].x = (1.0)*dMM*cos(PI/2);		//0.0;
	dPntLimit[1].y = (1.0)*dMM*sin(PI/2);		//pView->m_pTask->stCommon.dYPLimit;
	//2시
	dPntLimit[2].x = (1.0)*dMM*cos(PI/6);		//pView->m_pTask->stCommon.dXPLimit;
	dPntLimit[2].y = (-1.0)*dMM*sin(PI/6);	//pView->m_pTask->stCommon.dYMLimit;


	pView->m_pModel->m_fnGetInchToMM(arrWaferInch[pView->m_pAuto->m_nWaperSize] - 1, dMM, 0.0);
	CDPoint dPntStart[3];
	//11시
	dPntStart[0].x = (-1.0)*dMM*cos(PI/6); //;
	dPntStart[0].y = (-1.0)*dMM*sin(PI/6);	//pView->m_pTask->stCommon.dYMLimit;
	//6시
	dPntStart[1].x = (1.0)*dMM*cos(PI/2);		//0.0;
	dPntStart[1].y = (1.0)*dMM*sin(PI/2);		//pView->m_pTask->stCommon.dYPLimit;
	//2시
	dPntStart[2].x = (1.0)*dMM*cos(PI/6);		//pView->m_pTask->stCommon.dXPLimit;
	dPntStart[2].y = (-1.0)*dMM*sin(PI/6);	//pView->m_pTask->stCommon.dYMLimit;
	

	enum//실행 순서 바꾸려면 아래 순서를 바꿀것
	{
		SQNC_BASE = 0,

		SQNC_MOVE_CIRCLE_CENTER,
		SQNC_MOVE,
		SQNC_INDEX_BLACK,
		SQNC_AF2,
		SQNC_SEARCH_EDGE_PNT,

		MAX_SQNC_CNT
	};
	int iSqncNum = SQNC_BASE;
	int iSqncCur = SQNC_BASE;

// 	LightControl(0, 1, pView->m_pTask->stCommon.nInspLight);
// 	pCamJAI->SendMessage(WM_COMMAND, IDC_START, 0);
	pView->m_pStageMap->m_fnSetRunningCamera(TRUE);

	while (bSqncFlag)
	{
		Sleep(30);
		if (!pWorkRun->m_bThrdFlag[TF_AUTORUN])
		{
			iSqncCur = MAX_SQNC_CNT;
			iSqncNum = SQNC_BASE;
		}

		switch (iSqncNum)
		{
		case SQNC_BASE:
			if (bFnRes == FALSE)//검사중지
			{
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//일시정지
				break;

			if (iSqncCur == MAX_SQNC_CNT)//시퀀스 정상종료
			{
				bSqncFlag = FALSE;
				break;
			}

			if(iSqncCur != SQNC_MOVE
				&& iSqncCur != SQNC_INDEX_BLACK)
			{
				if (pAxis[X_AXIS]->CheckInMotion()
					|| pAxis[Y_AXIS]->CheckInMotion()
					|| pAxis[Z_AXIS]->CheckInMotion())
					break;
			}

			iSqncNum = ++iSqncCur;//증가하고 대입
			break;

		//원중심 이동.
		case SQNC_MOVE_CIRCLE_CENTER:
			pAxis[X_AXIS]->fVelocity = (float)pView->m_pTask->stCommon.dInspXYSpeed*1.f;
			pAxis[Y_AXIS]->fVelocity = (float)pView->m_pTask->stCommon.dInspXYSpeed*1.f;
			if (pView->m_pAuto->m_nWaperSize == WAPER_4)
			{
				pAxis[X_AXIS]->fVelocity *= 0.06f;
				pAxis[Y_AXIS]->fVelocity *= 0.06f;
			}

			bFnRes = pAxis[X_AXIS]->MoveAxis(dPntStart[iSearchDirection].x, 0, TRUE);
			if (bFnRes)
				bFnRes = pAxis[Y_AXIS]->MoveAxis(dPntStart[iSearchDirection].y, 0, TRUE);

			iSqncNum = SQNC_BASE;
			break;

		//지정위치 이동
		case SQNC_MOVE:
// 			pView->m_pStageMap->m_fnSetRunningCamera(TRUE);

// 			pMILAlignDlg->m_fnTranferCamBuff2PattBuff(pView->m_pAuto->m_bPattAlignMode);


// 			pAxis[X_AXIS]->fVelocity = (float)pView->m_pTask->stCommon.dInspXYSpeed*0.1f;
// 			pAxis[Y_AXIS]->fVelocity = (float)pView->m_pTask->stCommon.dInspXYSpeed*0.1f;
			pAxis[X_AXIS]->fVelocity = 5.f;
			pAxis[Y_AXIS]->fVelocity = 5.f;
			bFnRes = pAxis[X_AXIS]->MoveAxis(dPntLimit[iSearchDirection].x, 0, TRUE);
			if (bFnRes)
				bFnRes = pAxis[Y_AXIS]->MoveAxis(dPntLimit[iSearchDirection].y, 0, TRUE);
			CheckTime.SetTimeStart();
			iSqncNum = SQNC_BASE;
			break;

		//획득이미지로 검은색 영역 비율검사 1/2이상이면 정지
		case SQNC_INDEX_BLACK:
			pMILAlignDlg->m_fnBringImage(pCamJAI->m_CnvImageInfo.pImageBuffer, pCamJAI->m_CnvImageInfo.iImageSize);

			bFindWaferLine = pMILAlignDlg->FindWaferLine();
			if (!bFindWaferLine)//비율검사 함수
			{
				if (CheckTime.GetTimeInterval(2) > 60)
				{
					TECmsgOut(L"웨이퍼 경계검색 TimeOut (60sec). 검사를 종료합니다. 웨이퍼 세팅값을 확인해주세요. ", LIGHTMAGENTA, 1);
					CheckTime.SetTimeStart();
					bFnRes = FALSE;
					iSqncNum = SQNC_BASE;
				}
				break;
			}

			if(!pAxis[X_AXIS]->StopAxis() || !pAxis[Y_AXIS]->StopAxis())
				bFnRes = FALSE;

			if (!bFnRes)
				sprintf_s(tmpMsg, sizeof(tmpMsg), "%s-StopAxis", tmpMsg);

			iSqncNum = SQNC_BASE;
			break;

			//AF
		case SQNC_AF2:
// 			pView->m_pStageMap->m_fnSetRunningCamera(FALSE);
			ThrdAF(pView->m_pAuto->m_ctrlChkUseAF.GetCheck());
			pMILAlignDlg->m_fnBringImage(pCamJAI->m_CnvImageInfo.pImageBuffer, pCamJAI->m_CnvImageInfo.iImageSize);
			iSqncNum = SQNC_BASE;
			break;

		//대각선 검색후 엣지 점좌표 획득
		case SQNC_SEARCH_EDGE_PNT:
// 			pView->m_pStageMap->m_fnSetRunningCamera(TRUE);
// 			pTask->PatternNum = 0;
			pMILAlignDlg->m_fnBringImage(pCamJAI->m_CnvImageInfo.pImageBuffer, pCamJAI->m_CnvImageInfo.iImageSize);

			memset(m_dPntForCircle[iSearchDirection], 0x00, sizeof(m_dPntForCircle[iSearchDirection]));

			bFnRes = pMILAlignDlg->FindWaferEdge(m_dPntForCircle[iSearchDirection]);

			if (!bFnRes)
				sprintf_s(tmpMsg, sizeof(tmpMsg), "%s-FindWaferEdge", tmpMsg);
			else
			{
				pAxis[X_AXIS]->StatusGetPos(&dWaferLineMotorPos[0]);
				pAxis[Y_AXIS]->StatusGetPos(&dWaferLineMotorPos[1]);

				//이미지원점보정
				m_dPntForCircle[iSearchDirection][0] -= (DEF_IMAGE_X >> 1);
				m_dPntForCircle[iSearchDirection][1] -= (DEF_IMAGE_Y >> 1);
				//좌표단위보정
				m_dPntForCircle[iSearchDirection][0] *= 0.001;
				m_dPntForCircle[iSearchDirection][1] *= 0.001;
				//현재모터값계산
				m_dPntForCircle[iSearchDirection][0] += (dWaferLineMotorPos[0]);
				m_dPntForCircle[iSearchDirection][1] += (dWaferLineMotorPos[1]);
			}

// 			pView->m_pStageMap->m_fnSetRunningCamera(FALSE);
			pAxis[X_AXIS]->fVelocity = (float)pView->m_pTask->stCommon.dInspXYSpeed*1.f;
			pAxis[Y_AXIS]->fVelocity = (float)pView->m_pTask->stCommon.dInspXYSpeed*1.f;
			if (pView->m_pAuto->m_nWaperSize == WAPER_4)
			{
				pAxis[X_AXIS]->fVelocity *= 0.06f;
				pAxis[Y_AXIS]->fVelocity *= 0.06f;
			}

			iSqncNum = SQNC_BASE;
			break;

		case MAX_SQNC_CNT:
			bSqncFlag = FALSE;
			break;
		}
	}

	if (!bFnRes)
	{
		sprintf_s(tmpMsg, sizeof(tmpMsg), "%s[%d]", tmpMsg, iSearchDirection);
#if _DEBUG
		AfxMessageBox(tmpMsg, MB_ICONHAND);
#endif
	}
	else
		TECmsgOut(L"웨이퍼 경계인식 완료 ", YELLOW, 200, 17, BLACK);
	return bFnRes;
#else
return FALSE;
#endif
}

BOOL CWorkRun::SqncAlignPatt()
{
	CTimeInterval CheckTime;
	BOOL bSqncFlag = TRUE;
	BOOL bFnRes = TRUE;
	char tmpMsg[MAX_Buf] = { "Error_SqncAlignPatt" };

	CDPoint dPntTargetPos;

	enum//실행 순서 바꾸려면 아래 순서를 바꿀것
	{
		SQNC_BASE = 0,

		SQNC_MOVE_FIRST_POS,

		SQNC_MARK_REG,
		SQNC_ALIGN_MAPPING,
		SQNC_SUB_PNT_REG,
		SQNC_TARGET_PNT_REG,

		MAX_SQNC_CNT
	};
	int iSqncNum = SQNC_BASE;
	int iSqncCur = SQNC_BASE;

	while (bSqncFlag)
	{
		Sleep(100);
		if (!pWorkRun->m_bThrdFlag[TF_AUTORUN])
		{
			iSqncCur = MAX_SQNC_CNT;
			iSqncNum = SQNC_BASE;
		}

		switch (iSqncNum)
		{
		case SQNC_BASE:
			if (bFnRes == FALSE)//검사중지
			{
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//일시정지
				break;

			if (pWorkRun->m_bJustNowFinish)
			{
				bSqncFlag = FALSE;
				break;
			}

			if (iSqncCur == MAX_SQNC_CNT)//시퀀스 정상종료
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//증가하고 대입
			break;


			//현재 모델의 기준 좌표 - 없으면 원점@
			//얼라인체크
		case SQNC_MOVE_FIRST_POS:
			dPntTargetPos.x = pView->m_pModel->m_pTask->stModel.dPattPosX;
			dPntTargetPos.y = pView->m_pModel->m_pTask->stModel.dPattPosY;
			dPntTargetPos.x = dPntTargetPos.x < -200 ? -200.0 : dPntTargetPos.x > 200 ? 200 : dPntTargetPos.x;
			dPntTargetPos.y = dPntTargetPos.y < -200 ? -200.0 : dPntTargetPos.y > 200 ? 200 : dPntTargetPos.y;
			pAxis[X_AXIS]->fVelocity = (float)pView->m_pModel->m_pTask->stCommon.dInspXYSpeed;
			pAxis[Y_AXIS]->fVelocity = (float)pView->m_pModel->m_pTask->stCommon.dInspXYSpeed;

			bFnRes = SqncTargetMove(dPntTargetPos);

			pView->m_pTask->stResult.dWaperThickness = pView->m_pModel->m_dWaperThickness;
			pView->m_pTask->stResult.dEdgeExclusion = pView->m_pModel->m_dWaperExclusion;
			ThrdAF(pView->m_pAuto->m_ctrlChkUseAF.GetCheck());

			iSqncNum = SQNC_BASE;
			break;


		case SQNC_MARK_REG:
			pWorkRun->m_bThrdFlag[TF_MODELSET] = TRUE;
// 			if (m_bThrdFlag[TF_CASSETTE_START])
				bFnRes = pWorkRun->SqncPattReg();

			iSqncNum = SQNC_BASE;
			break;


		case SQNC_ALIGN_MAPPING:
			bFnRes = pWorkRun->SqncAlignMapping();
			pWorkRun->m_bThrdFlag[TF_MODELSET] = FALSE;
			iSqncNum = SQNC_BASE;
			break;


		case SQNC_SUB_PNT_REG:
			if (m_bThrdFlag[TF_CASSETTE_START])
				bFnRes = pWorkRun->SqncSubReg();
			iSqncNum = SQNC_BASE;
			break;


		case SQNC_TARGET_PNT_REG:
			bFnRes = pWorkRun->SqncTargetReg();//보정파일 로딩시 필요

			iSqncNum = SQNC_BASE;
			break;


		case MAX_SQNC_CNT:
			bSqncFlag = FALSE;
			break;
		}
	}

	if (!bFnRes)
	{
#if _DEBUG
		AfxMessageBox(tmpMsg, MB_ICONHAND);
#endif
	}
	else
	{
		TECmsgOut(L"패턴 얼라인 완료 ", YELLOW, 200, 17, BLACK);
		m_bThrdFlag[TF_CASSETTE_START] = FALSE;
	}
	pWorkRun->m_bThrdFlag[TF_MODELSET] = FALSE;
	pView->m_pModel->m_fnSetRadioModelType(MODEL_TYPE_GENERAL);

	return bFnRes;

}

BOOL CWorkRun::SqncManualMeasure()
{
	CTimeInterval CheckTime;
	CTimeInterval CheckTactTime;
	BOOL bSqncFlag = TRUE;
	BOOL bFnRes = TRUE;
	char tmpMsg[MAX_Buf] = { "" };
	int  nSEQ = 0;
	CString strWork("");
	float fStd[MAX_LAYER+1][MAX_ITEM+1];
	memset( fStd, 0x00, sizeof(fStd));

	CDPoint dPntTargetPos;
	double dCurPosZ=0.0;

	t_ELIPSO_RESULT_INFO stcResultAlign;

	CString strX("");
	CString strY("");
	CString strNAME("");

// 	pView->m_pAuto->m_fnSRSheetClear(FALSE);

// 	for (int i = 0; i < MAX_DATA; i++)

	int iCurRow = 0;
	{
		//현재 커서위치Row 확인
		iCurRow = pView->m_pTask->stResult.nSRData+1;// pView->m_pAuto->m_clResult.GetRow();
		pView->m_pAuto->m_clResult.SetRow(iCurRow);
		
		//RESULT_TARGET_NO SetText();
		pView->m_pAuto->m_clResult.SetCol(RESULT_TARGET_NO);
		strNAME.Format("Manual_%01d", iCurRow);
		pView->m_pAuto->m_clResult.SetText(strNAME);

		//RESULT_X1_X2 현재 모터X좌표 SetText();
		pView->m_pAuto->m_clResult.SetCol(RESULT_X1_X2);
		pAxis[X_AXIS]->StatusGetPos(&dPntTargetPos.x);
		strX.Format("%.3f", dPntTargetPos.x);
		pView->m_pAuto->m_clResult.SetText(strX);

		//RESULT_Y1_Y2 현재 모터Y좌표 SetText();
		pView->m_pAuto->m_clResult.SetCol(RESULT_Y1_Y2);
		pAxis[Y_AXIS]->StatusGetPos(&dPntTargetPos.y);
		strY.Format("%.3f", dPntTargetPos.y);
		pView->m_pAuto->m_clResult.SetText(strY);

		//double = 현재 모터Z좌표
		pAxis[Z_AXIS]->StatusGetPos(&dCurPosZ);

		strX.Trim();
		strY.Trim();
		strNAME.Trim();

		if ((!strX.IsEmpty()) &&
			(!strY.IsEmpty()) &&
			(!strNAME.IsEmpty()))
		{
			pView->m_pTask->stData.m_dDataL[pView->m_pTask->stData.m_nDataCnt] = pView->m_pTask->stData.m_nDataCnt;
			sprintf_s(pView->m_pTask->stResult.stSRData[pView->m_pTask->stData.m_nDataCnt].szName, "%s", strNAME.GetString() );

			pView->m_pTask->stResult.stSRData[pView->m_pTask->stData.m_nDataCnt].dwXPosition = atof(strX);
			pView->m_pTask->stData.m_dDataX[pView->m_pTask->stData.m_nDataCnt] = atof(strX);

			pView->m_pTask->stResult.stSRData[pView->m_pTask->stData.m_nDataCnt].dwYPosition = atof(strY);
			pView->m_pTask->stData.m_dDataY[pView->m_pTask->stData.m_nDataCnt] = atof(strY);

			pView->m_pTask->stResult.stSRData[pView->m_pTask->stData.m_nDataCnt].dwZPosition = dCurPosZ;
			pView->m_pTask->stData.m_dDataH[pView->m_pTask->stData.m_nDataCnt] = pView->m_pTask->stResult.stSRData[pView->m_pTask->stData.m_nDataCnt].dwZPosition;

			pView->m_pTask->stData.m_nDataCnt++;
		}
	}
	pView->m_pTask->stResult.nSRData = pView->m_pTask->stData.m_nDataCnt;
	pView->m_pTask->stResult.dSRZ = theApp.m_fnGetViewPointer()->m_pModel->m_fnGetMeasureZ(pView->m_pAuto->m_dWaperThickness);
	pView->m_pTask->stResult.dSRSafetyZ = theApp.m_fnGetViewPointer()->m_pModel->m_fnGetMeasureSafetyZ(pView->m_pAuto->m_dWaperThickness);
	pView->m_pTask->stResult.dAlignZ = theApp.m_fnGetViewPointer()->m_pModel->m_fnGetAlignZ(pView->m_pAuto->m_dWaperThickness);
	pView->m_pTask->stResult.dAlignSafetyZ = theApp.m_fnGetViewPointer()->m_pModel->m_fnGetAlignSafetyZ(pView->m_pAuto->m_dWaperThickness);
	pView->m_pTask->stResult.dAlignX = pView->m_pTask->stCommon.dAlignX[pView->m_pAuto->m_bWaperType][pView->m_pAuto->m_nWaperSize];
	pView->m_pTask->stResult.dAlignY = pView->m_pTask->stCommon.dAlignY[pView->m_pAuto->m_bWaperType][pView->m_pAuto->m_nWaperSize];
	pView->m_pTask->stResult.dSRSpeed = pView->m_pTask->stCommon.dInspXYSpeed;

	if (pView->m_pAuto->m_pTask->stData.m_nDataCnt == 0)
	{
		bSqncFlag = FALSE;
		bFnRes = FALSE;
		return bFnRes;
	}
	CAutoInspectorDlg *pQI = (CAutoInspectorDlg *)pView->m_pAuto;
	int     nLoopCnt = 0;

	//리스트 등록 좌표 개수 확인
	int iListCnt = pView->m_pTask->stResult.nSRData;
	int iCurCnt = 0;

	int arrWaferInch[4] = { 4,6,8,12 };
	double dMM = 0.0;

	enum//실행 순서 바꾸려면 아래 순서를 바꿀것
	{
		SQNC_BASE = 0,

		SQNC_LIGHT_OFF,
		SQNC_READY_MEASURE,
		SQNC_MOVE_Z_OFFSET,
		SQNC_MEASURE,
		SQNC_CLOSE_MEASURE,
		SQNC_RESULT_DISPLAY,
		SQNC_LIST_REPEAT,

		MAX_SQNC_CNT
	};
	int iSqncNum = SQNC_BASE;
	int iSqncCur = SQNC_BASE;

	while (bSqncFlag)
	{
		Sleep(300);
		if (!pWorkRun->m_bThrdFlag[TF_AUTORUN])
		{
			iSqncCur = MAX_SQNC_CNT;
			iSqncNum = SQNC_BASE;
		}

		switch (iSqncNum)
		{
		case SQNC_BASE:
			if (bFnRes == FALSE)//검사중지
			{
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//일시정지
				break;

			if (iSqncCur == MAX_SQNC_CNT)//시퀀스 정상종료
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//증가하고 대입
			break;

			//*리스트 순서대로 위치값 읽어오기
			//*위치이동 실시
		case SQNC_LIGHT_OFF:
			CheckTactTime.SetTimeStart();
			// 			LightControl(0, 1, 0);

			iSqncNum = SQNC_BASE;
			break;

		case SQNC_READY_MEASURE:
			bFnRes = pWorkRun->SqncReadyMeasure(m_bInitManualData);
			m_bInitManualData = FALSE;
			iSqncNum = SQNC_BASE;
			break;

		case SQNC_MOVE_Z_OFFSET:
			bFnRes = SqncCorrectAlign(stcResultAlign);
			if (!bFnRes)
			{
				TECmsgOut(L"""위치정렬""을 완료하지 못했습니다.", LIGHTMAGENTA, 3000);
// 				iSqncCur = SQNC_LIST_REPEAT - 1;
// 				bFnRes = TRUE;
			}
			iSqncNum = SQNC_BASE;
			break;

			//*측정 실시	
		case SQNC_MEASURE:
			bFnRes = SqncMeasure(stcResultAlign);
			if (!bFnRes)
			{
				TECmsgOut(L"""측정""을 완료하지 못했습니다.", LIGHTMAGENTA, 3000);
// 				iSqncCur = SQNC_LIST_REPEAT - 1;
// 				bFnRes = TRUE;
			}
			iSqncNum = SQNC_BASE;
			break;

		case SQNC_CLOSE_MEASURE:
			bFnRes = pWorkRun->SqncCloseMeasure();
			iSqncNum = SQNC_BASE;
			break;

			//리스트 측정결과데이터 표시
		case SQNC_RESULT_DISPLAY:
			nSEQ = iListCnt-1;
			pView->m_pTask->stResult.nLayer = stcResultAlign.nLayerCnt;
			for (int i = 0; i < stcResultAlign.nLayerCnt; i++)
			{
				pView->m_pAuto->m_clResult.SetRow(nSEQ + 1);

				pView->m_pAuto->m_clResult.SetCol(RESULT_MSE);
				strWork.Format( "%.3f", stcResultAlign.dMSE );
				pView->m_pAuto->m_clResult.SetText(strWork);
				pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][RES_MSE] = stcResultAlign.dMSE;
				pView->m_pTask->stData.m_dDataZ[nSEQ][i][RES_MSE] = stcResultAlign.dMSE;

				pView->m_pAuto->m_clResult.SetCol(RESULT_TOTAL_THICKNESS);
				strWork.Format("%.3f", stcResultAlign.dTotalThickness);
				pView->m_pAuto->m_clResult.SetText(strWork);
				pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][RES_TOTAL_THICKNESS] = stcResultAlign.dTotalThickness;
				pView->m_pTask->stData.m_dDataZ[nSEQ][i][RES_TOTAL_THICKNESS] = stcResultAlign.dTotalThickness;

				for (int i = 0; i < MAX_LAYER; i++)
				{
					pView->m_pAuto->m_clResult.SetCol(RESULT_LAYER_1 + i);
					strWork.Format("%s", pView->m_pAuto->m_fnGetSelectResultData(stcResultAlign, pView->m_pAuto->m_nMeasureItem-2, i + 1));
					pView->m_pAuto->m_clResult.SetText(strWork);
				}

				pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][RES_THICKNESS] = stcResultAlign.stLayer[i].dThickness;
				pView->m_pTask->stData.m_dDataZ[nSEQ][i][RES_THICKNESS] = stcResultAlign.stLayer[i].dThickness;

				pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][RES_N_550] = stcResultAlign.stLayer[i].dN_550;
				pView->m_pTask->stData.m_dDataZ[nSEQ][i][RES_N_550] = stcResultAlign.stLayer[i].dN_550;

				pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][RES_K_550] = stcResultAlign.stLayer[i].dK_550;
				pView->m_pTask->stData.m_dDataZ[nSEQ][i][RES_K_550] = stcResultAlign.stLayer[i].dK_550;

				pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][RES_N_632] = stcResultAlign.stLayer[i].dN_632;
				pView->m_pTask->stData.m_dDataZ[nSEQ][i][RES_N_632] = stcResultAlign.stLayer[i].dN_632;

				pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][RES_K_632] = stcResultAlign.stLayer[i].dK_632;
				pView->m_pTask->stData.m_dDataZ[nSEQ][i][RES_K_632] = stcResultAlign.stLayer[i].dK_632;
			}
			pView->m_pTask->stResult.nLayer = stcResultAlign.nLayerCnt;
			// 통계 데이터 계산
			for (int i = 0; i < MAX_LAYER; i++)
			{
				for (int j = 0; j < MAX_ITEM; j++)
				{
					if (nSEQ == 0)
					{
						pView->m_pTask->stData.m_dMax[i][j] = pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][j];
						pView->m_pTask->stData.m_dMin[i][j] = pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][j];
						pView->m_pTask->stData.m_dAvg[i][j] = pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][j];
						pView->m_pTask->stData.m_fTot[i][j] = 0.0f;
						pView->m_pTask->stData.m_iZeroCnt[i][j] = 0;
					}

					if (pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][j] > 0)
					{
						if (pView->m_pTask->stData.m_dMax[i][j] < pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][j])
						{
							pView->m_pTask->stData.m_dMax[i][j] = pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][j];
						}
						if (pView->m_pTask->stData.m_dMin[i][j] > pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][j])
						{
							pView->m_pTask->stData.m_dMin[i][j] = pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][j];
						}
						pView->m_pTask->stData.m_dMax_Min[i][j]     = abs(pView->m_pTask->stData.m_dMax[i][j] - pView->m_pTask->stData.m_dMin[i][j]);

						pView->m_pTask->stData.m_fTot[i][j] += (float)pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][j];
						pView->m_pTask->stData.m_dAvg[i][j] = (double)(pView->m_pTask->stData.m_fTot[i][j] / (float)(nSEQ + 1 - pView->m_pTask->stData.m_iZeroCnt[i][j]));

					} 
					else
					{
						pView->m_pTask->stData.m_iZeroCnt[i][j]++;
					}
				}
			}

			for (int i = 0; i < MAX_LAYER; i++)
			{
				for (int j = 0; j < MAX_ITEM; j++)
				{
					fStd[i][j] = 0.0f;
					for (int n = 0; n < nSEQ + 1; n++)
						fStd[i][j] += (float)pow(pView->m_pTask->stResult.stSRData[n].dwResult[i][j] - pView->m_pTask->stData.m_dAvg[i][j], 2);

					pView->m_pTask->stData.m_dStd[i][j] = sqrt((double)(fStd[i][j] / (nSEQ + 1 - pView->m_pTask->stData.m_iZeroCnt[i][j])));
					pView->m_pTask->stData.m_dUniformity[i][j] = (pView->m_pTask->stData.m_dStd[i][j] / pView->m_pTask->stData.m_dAvg[i][j]);
					pView->m_pTask->stData.m_dContourMax[i][j] = pView->m_pTask->stData.m_dMax[i][j];
					pView->m_pTask->stData.m_dContourMin[i][j] = pView->m_pTask->stData.m_dMin[i][j];
					pView->m_pTask->stData.m_dContourStep[i][j] = pView->m_pTask->stData.m_dMax_Min[i][j] / 10.;
				}
			}

			sprintf_s(pView->m_pTask->stResult.szInspTackTime, sizeof(char) * 256, "%.1f s", CheckTactTime.GetTimeInterval(2));
			pView->m_pAuto->m_fnSetView();

			iSqncNum = SQNC_BASE;
			break;

			//*리스트 개수만큼 반복
		case SQNC_LIST_REPEAT:
// 			if (iListCnt-1 <= iCurCnt)
			{
				iSqncCur = MAX_SQNC_CNT;
				iSqncNum = SQNC_BASE;
// 				break;
			}
// 			iCurCnt++;
// 			iSqncCur = SQNC_BASE;
// 			iSqncNum = SQNC_BASE;
			break;

		case MAX_SQNC_CNT:
			bSqncFlag = FALSE;
			break;
		}
	}

	if (!bFnRes)
	{
#if _DEBUG
		AfxMessageBox(tmpMsg, MB_ICONHAND);
#endif
	}
	else
	{
		TECmsgOut(L"지정 위치 측정 완료 ", YELLOW, 200, 17, BLACK);
		pView->m_pAuto->m_clResult.SetRow(iCurRow + 1);
	}

	return bFnRes;
}

// 	theApp.m_fnGetViewPointer()->m_pManual->m_pElipso->m_fnSetReadyEllipso();
// 
BOOL CWorkRun::SqncRecipeLoad()
{
	CTimeInterval CheckTime;
	CTimeInterval CheckTactTime;
	BOOL bSqncFlag = TRUE;
	BOOL bFnRes = TRUE;
	char tmpMsg[MAX_Buf] = { "" };

	CString strX("");
	CString strY("");
	CString strNAME("");
	int i = 0;

	t_ELIPSO_RESULT_INFO stcResultLoad;

	enum//실행 순서 바꾸려면 아래 순서를 바꿀것
	{
		SQNC_BASE = 0,

		SQNC_LOAD_RECIPE,
		SQNC_CHECK_LOAD,

		MAX_SQNC_CNT
	};
	int iSqncNum = SQNC_BASE;
	int iSqncCur = SQNC_BASE;

	while (bSqncFlag)
	{
		Sleep(30);

		switch (iSqncNum)
		{
		case SQNC_BASE:
			if (bFnRes == FALSE)//검사중지
			{
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//일시정지
				break;

			if (pWorkRun->m_bJustNowFinish)
			{
				bSqncFlag = FALSE;
				break;
			}

			if (iSqncCur == MAX_SQNC_CNT)//시퀀스 정상종료
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//증가하고 대입
			break;

			//*리스트 순서대로 위치값 읽어오기
			//*위치이동 실시

		case SQNC_LOAD_RECIPE:
			pView->m_pManual->m_pElipso->m_fnSetLoadRecipe();
			iSqncNum = SQNC_BASE;
			break;

		case SQNC_CHECK_LOAD:
			pView->m_pManual->m_pElipso->m_fnGetResult(stcResultLoad);
			if (stcResultLoad.bComplete
				|| !pView->m_pManual->m_pElipso->m_bResultEnd)
			{
				if (stcResultLoad.bError)
				{
					TECmsgOut(L"Recipe Load를 완료하지 못했습니다. 연결상태를 확인하세요.", LIGHTMAGENTA, 1);
					bFnRes = FALSE;
				}
			}

			iSqncNum = SQNC_BASE;
			break;

		case MAX_SQNC_CNT:
			bSqncFlag = FALSE;
			break;
		}
	}

	if (!bFnRes)
	{
#if _DEBUG
		AfxMessageBox(tmpMsg, MB_ICONHAND);
#endif
	}
	else
		TECmsgOut(L"EllipsoMeter RecipeLoad완료 ", YELLOW, 200, 17, BLACK);

	return bFnRes;
}


BOOL CWorkRun::SqncReadyMeasure(BOOL bInitData)
{
	CTimeInterval CheckTime;
	CTimeInterval CheckTactTime;
	BOOL bSqncFlag = TRUE;
	BOOL bFnRes = TRUE;
	char tmpMsg[MAX_Buf] = { "" };

	CString strX("");
	CString strY("");
	CString strNAME("");
	int i = 0;

	t_ELIPSO_RESULT_INFO stcResultReady;

	enum//실행 순서 바꾸려면 아래 순서를 바꿀것
	{
		SQNC_BASE = 0,

		SQNC_INIT_DATA,
		SQNC_READY_ELLIPSO,
		SQNC_CHECK_READY,

		MAX_SQNC_CNT
	};
	int iSqncNum = SQNC_BASE;
	int iSqncCur = SQNC_BASE;

	while (bSqncFlag)
	{
		Sleep(300);
		if (!pWorkRun->m_bThrdFlag[TF_AUTORUN])
		{
			iSqncCur = MAX_SQNC_CNT;
			iSqncNum = SQNC_BASE;
		}

		switch (iSqncNum)
		{
		case SQNC_BASE:
			if (bFnRes == FALSE)//검사중지
			{
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//일시정지
				break;

			if (pWorkRun->m_bJustNowFinish)
			{
				bSqncFlag = FALSE;
				break;
			}

			if (iSqncCur == MAX_SQNC_CNT)//시퀀스 정상종료
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//증가하고 대입
			break;

			//*리스트 순서대로 위치값 읽어오기
			//*위치이동 실시

		case SQNC_INIT_DATA:
			if(!bInitData)
			{
				iSqncNum = SQNC_BASE;
				break;
			}

			memset(&pView->m_pTask->stData, 0x00, sizeof(t_RESULT_INFO));
			pView->m_pAuto->m_fnSRSheetClear(FALSE);

			for (i = 0; i < MAX_DATA; i++)
			{
				pView->m_pAuto->m_clResult.SetRow(i + 1);

				pView->m_pAuto->m_clResult.SetCol(RESULT_TARGET_NO);
				strNAME = pView->m_pAuto->m_clResult.GetText();

				pView->m_pAuto->m_clResult.SetCol(RESULT_X1_X2);
				strX = pView->m_pAuto->m_clResult.GetText();

				pView->m_pAuto->m_clResult.SetCol(RESULT_Y1_Y2);
				strY = pView->m_pAuto->m_clResult.GetText();

				strNAME.Trim();
				strX.Trim();
				strY.Trim();
				strNAME.Trim();
				if ((!strX.IsEmpty()) &&
					(!strY.IsEmpty()) &&
					(!strNAME.IsEmpty()))
				{
					pView->m_pTask->stData.m_dDataL[pView->m_pTask->stData.m_nDataCnt] = pView->m_pTask->stData.m_nDataCnt;
					sprintf_s(pView->m_pTask->stResult.stSRData[pView->m_pTask->stData.m_nDataCnt].szName, "%s", strNAME.GetString());

					pView->m_pTask->stResult.stSRData[pView->m_pTask->stData.m_nDataCnt].dwXPosition = atof(strX);
					pView->m_pTask->stData.m_dDataX[pView->m_pTask->stData.m_nDataCnt] = atof(strX);

					pView->m_pTask->stResult.stSRData[pView->m_pTask->stData.m_nDataCnt].dwYPosition = atof(strY);
					pView->m_pTask->stData.m_dDataY[pView->m_pTask->stData.m_nDataCnt] = atof(strY);

					pView->m_pTask->stData.m_nDataCnt++;
				}
			}
			pView->m_pTask->stResult.nSRData = pView->m_pTask->stData.m_nDataCnt;
			pView->m_pTask->stResult.dSRZ = theApp.m_fnGetViewPointer()->m_pModel->m_fnGetMeasureZ(pView->m_pAuto->m_dWaperThickness);
			pView->m_pTask->stResult.dSRSafetyZ = theApp.m_fnGetViewPointer()->m_pModel->m_fnGetMeasureSafetyZ(pView->m_pAuto->m_dWaperThickness);
			pView->m_pTask->stResult.dAlignZ = theApp.m_fnGetViewPointer()->m_pModel->m_fnGetAlignZ(pView->m_pAuto->m_dWaperThickness);
			pView->m_pTask->stResult.dAlignSafetyZ = theApp.m_fnGetViewPointer()->m_pModel->m_fnGetAlignSafetyZ(pView->m_pAuto->m_dWaperThickness);
			pView->m_pTask->stResult.dAlignX = pView->m_pTask->stCommon.dAlignX[pView->m_pAuto->m_bWaperType][pView->m_pAuto->m_nWaperSize];
			pView->m_pTask->stResult.dAlignY = pView->m_pTask->stCommon.dAlignY[pView->m_pAuto->m_bWaperType][pView->m_pAuto->m_nWaperSize];
			pView->m_pTask->stResult.dSRSpeed = pView->m_pTask->stCommon.dInspXYSpeed;

			iSqncNum = SQNC_BASE;
			break;

		case SQNC_READY_ELLIPSO:
			pView->m_pManual->m_pElipso->m_fnSetReadyEllipso();
			iSqncNum = SQNC_BASE;
			break;

		case SQNC_CHECK_READY:
			pView->m_pManual->m_pElipso->m_fnGetResult(stcResultReady);
			if (stcResultReady.bComplete
				|| !pView->m_pManual->m_pElipso->m_bResultEnd)
			{
				if (stcResultReady.bError)
				{
					TECmsgOut(L"엘립소메타 측정준비를 완료하지 못했습니다. 연결상태를 확인하세요.", LIGHTMAGENTA, 1);
					bFnRes = FALSE;
				}
			}
			else
				break;

			iSqncNum = SQNC_BASE;
			break;

		case MAX_SQNC_CNT:
			bSqncFlag = FALSE;
			break;
		}
	}

	if (!bFnRes)
	{
#if _DEBUG
		AfxMessageBox(tmpMsg, MB_ICONHAND);
#endif
	}
	else
		TECmsgOut(L"EllipsoMeter 준비완료 ", YELLOW, 200, 17, BLACK);

	return bFnRes;
}

BOOL CWorkRun::SqncCloseMeasure()
{
	CTimeInterval CheckTime;
	CTimeInterval CheckTactTime;
	BOOL bSqncFlag = TRUE;
	BOOL bFnRes = TRUE;
	char tmpMsg[MAX_Buf] = { "" };

	CString strX("");
	CString strY("");
	CString strNAME("");
	int i = 0;

	t_ELIPSO_RESULT_INFO stcResultClose;

	enum//실행 순서 바꾸려면 아래 순서를 바꿀것
	{
		SQNC_BASE = 0,

		SQNC_CLOSE_ELLIPSO,
		SQNC_CHECK_CLOSE,

		MAX_SQNC_CNT
	};
	int iSqncNum = SQNC_BASE;
	int iSqncCur = SQNC_BASE;

	while (bSqncFlag)
	{
		Sleep(300);
		if (!pWorkRun->m_bThrdFlag[TF_AUTORUN])
		{
			iSqncCur = MAX_SQNC_CNT;
			iSqncNum = SQNC_BASE;
		}

		switch (iSqncNum)
		{
		case SQNC_BASE:
			if (bFnRes == FALSE)//검사중지
			{
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//일시정지
				break;

			if (pWorkRun->m_bJustNowFinish)
			{
				bSqncFlag = FALSE;
				break;
			}

			if (iSqncCur == MAX_SQNC_CNT)//시퀀스 정상종료
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//증가하고 대입
			break;

			//*리스트 순서대로 위치값 읽어오기
			//*위치이동 실시

		case SQNC_CLOSE_ELLIPSO:
			pView->m_pManual->m_pElipso->m_fnSetSystemLampShutterClosed();
			iSqncNum = SQNC_BASE;
			break;

		case SQNC_CHECK_CLOSE:
// 			pView->m_pManual->m_pElipso->m_fnGetResult(stcResultClose);
// 			if (stcResultClose.bComplete
// 				|| !pView->m_pManual->m_pElipso->m_bResultEnd)
// 			{
// 				if (stcResultClose.bError)
// 				{
// 					TECmsgOut(L"엘립소메타 셔터를 Close하지 못했습니다. 연결상태를 확인하세요.", LIGHTMAGENTA, 1);
// 					bFnRes = FALSE;
// 				}
// 			}
			iSqncNum = SQNC_BASE;
			break;

		case MAX_SQNC_CNT:
			bSqncFlag = FALSE;
			break;
		}
	}

	if (!bFnRes)
	{
#if _DEBUG
		AfxMessageBox(tmpMsg, MB_ICONHAND);
#endif
	}
	else
		TECmsgOut(L"EllipsoMeter 준비완료 ", YELLOW, 200, 17, BLACK);

	return bFnRes;
}

BOOL CWorkRun::SqncAutoMoveMeasure()
{
	CTimeInterval CheckTime;
	CTimeInterval CheckTactTime;
	BOOL bSqncFlag = TRUE;
	BOOL bFnRes = TRUE;
	char tmpMsg[MAX_Buf] = { "" };

	int  nSEQ = 0;
	CString strWork("");
	float fStd[MAX_LAYER + 1][MAX_ITEM + 1];
	memset(fStd, 0x00, sizeof(fStd));

	CDPoint dPntTargetPos;
	double dCurPosZ = 0.0;

	t_ELIPSO_RESULT_INFO stcResultAlign;

	if (pView->m_pAuto->m_pTask->stData.m_nDataCnt == 0)
	{
		bSqncFlag = FALSE;
		bFnRes = FALSE;
		return bFnRes;
	}
	CAutoInspectorDlg *pQI = (CAutoInspectorDlg *)pView->m_pAuto;
	int     nLoopCnt = 0;

	//리스트 등록 좌표 개수 확인
	int iListCnt = pView->m_pTask->stResult.nSRData;
	int iCurCnt = 0;

	int arrWaferInch[4] = { 4,6,8,12 };
	double dMM = 0.0;

	enum//실행 순서 바꾸려면 아래 순서를 바꿀것
	{
		SQNC_BASE = 0,

		SQNC_MOVE_XY,
		SQNC_MOVE_Z_OFFSET,
		SQNC_MEASURE,
		SQNC_RESULT_DISPLAY,
		SQNC_LIST_REPEAT,

		MAX_SQNC_CNT
	};
	int iSqncNum = SQNC_BASE;
	int iSqncCur = SQNC_BASE;

	while (bSqncFlag)
	{
		Sleep(30);
		if (!pWorkRun->m_bThrdFlag[TF_AUTORUN])
		{
			iSqncCur = MAX_SQNC_CNT;
			iSqncNum = SQNC_BASE;
		}

		switch (iSqncNum)
		{
		case SQNC_BASE:
			if (bFnRes == FALSE)//검사중지
			{
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//일시정지
				break;

			if (pWorkRun->m_bJustNowFinish)
			{
				bSqncFlag = FALSE;
				break;
			}

			if (iSqncCur == MAX_SQNC_CNT)//시퀀스 정상종료
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//증가하고 대입
			break;

			//*리스트 순서대로 위치값 읽어오기
			//*위치이동 실시
		case SQNC_MOVE_XY:
			CheckTime.SetTimeStart();
			CheckTactTime.SetTimeStart();
			dPntTargetPos.x = pQI->m_pTask->stData.m_dDataX[iCurCnt];
			dPntTargetPos.y = pQI->m_pTask->stData.m_dDataY[iCurCnt];

			// 			pQI->m_pTask->stResult.nWaperSize

			//이동포인트가 웨이퍼 밖에 있을때...
			pView->m_pModel->m_fnGetInchToMM(arrWaferInch[pView->m_pAuto->m_nWaperSize], dMM, pView->m_pAuto->m_dWaperExclusion);
			if (!inCircle(dPntTargetPos.x, dPntTargetPos.y, 0.0, 0.0, dMM))
			{
				if (TECmsgOut(L"측정위치가 웨이퍼범위를 벗어났습니다. 해당위치 측정을 건너뛰겠습니까?", LIGHTMAGENTA, 2))
				{
					iSqncCur = SQNC_LIST_REPEAT - 1;
					iSqncNum = SQNC_BASE;
					break;
				}
			}

			pAxis[X_AXIS]->fVelocity = (float)pQI->m_pTask->stCommon.dInspXYSpeed;
			pAxis[Y_AXIS]->fVelocity = (float)pQI->m_pTask->stCommon.dInspXYSpeed;
			if (pView->m_pAuto->m_nWaperSize == WAPER_4)
			{
				pAxis[X_AXIS]->fVelocity *= 0.06f;
				pAxis[Y_AXIS]->fVelocity *= 0.06f;
			}

			//			dwMoveZ = theApp.m_fnGetViewPointer()->m_pModel->m_fnGetAlignZ(pQI->m_dWaperThickness);
			// 			dwSpeedZ = pQI->m_pTask->stCommon.dSpeedHighZ;

// 			LightControl(0, 1, 0);
			bFnRes = SqncTargetMove(dPntTargetPos);
			if (!bFnRes)
			{
				TECmsgOut(L"측정위치""이동""을 완료하지 못했습니다. 3초 후 다음위치로 진행합니다", LIGHTMAGENTA, 3000);
				iSqncCur = SQNC_LIST_REPEAT - 1;
			}

			iSqncNum = SQNC_BASE;
			break;

		case SQNC_MOVE_Z_OFFSET:
			sprintf_s(pView->m_pTask->stResult.szInspTackTime, sizeof(char) * 256, "MoveXY: %.1f s", CheckTime.GetTimeInterval(2));
			OutputDebugString(pView->m_pTask->stResult.szInspTackTime);
			CheckTime.SetTimeStart();
			bFnRes = SqncCorrectAlign(stcResultAlign);
			if (!bFnRes)
			{
				TECmsgOut(L"""위치정렬""을 완료하지 못했습니다. 3초 후 다음위치로 진행합니다", LIGHTMAGENTA, 3000);
				iSqncCur = SQNC_LIST_REPEAT - 1;
				bFnRes = TRUE;
			}
			iSqncNum = SQNC_BASE;
			break;

			//*측정 실시	
		case SQNC_MEASURE:
			sprintf_s(pView->m_pTask->stResult.szInspTackTime, sizeof(char) * 256, "MoveZ: %.1f s", CheckTime.GetTimeInterval(2));
			OutputDebugString(pView->m_pTask->stResult.szInspTackTime);
			CheckTime.SetTimeStart();
			bFnRes = SqncMeasure(stcResultAlign);
			if (!bFnRes)
			{
				TECmsgOut(L"""측정""을 완료하지 못했습니다. 3초 후 다음위치로 진행합니다", LIGHTMAGENTA, 3000);
				iSqncCur = SQNC_LIST_REPEAT - 1;
				bFnRes = TRUE;
			}
			iSqncNum = SQNC_BASE;
			break;

			//리스트 측정결과데이터 표시
		case SQNC_RESULT_DISPLAY:
			sprintf_s(pView->m_pTask->stResult.szInspTackTime, sizeof(char) * 256, "Measure: %.1f s", CheckTime.GetTimeInterval(2));
			OutputDebugString(pView->m_pTask->stResult.szInspTackTime);
			CheckTime.SetTimeStart();
			nSEQ = iCurCnt;

			pAxis[Z_AXIS]->StatusGetPos(&dCurPosZ);
			pView->m_pTask->stResult.stSRData[nSEQ].dwZPosition = dCurPosZ;
			pView->m_pTask->stData.m_dDataH[nSEQ] = pView->m_pTask->stResult.stSRData[nSEQ].dwZPosition;
			
			pView->m_pTask->stResult.nLayer = stcResultAlign.nLayerCnt;
			for (int i = 0; i < stcResultAlign.nLayerCnt; i++)
			{
				pView->m_pAuto->m_clResult.SetRow(nSEQ + 1);

				pView->m_pAuto->m_clResult.SetCol(RESULT_MSE);
				strWork.Format("%.3f", stcResultAlign.dMSE);
				pView->m_pAuto->m_clResult.SetText(strWork);
				pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][RES_MSE] = stcResultAlign.dMSE;
				pView->m_pTask->stData.m_dDataZ[nSEQ][i][RES_MSE] = stcResultAlign.dMSE;

				pView->m_pAuto->m_clResult.SetCol(RESULT_TOTAL_THICKNESS);
				strWork.Format("%.3f", stcResultAlign.dTotalThickness);
				pView->m_pAuto->m_clResult.SetText(strWork);
				pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][RES_TOTAL_THICKNESS] = stcResultAlign.dTotalThickness;
				pView->m_pTask->stData.m_dDataZ[nSEQ][i][RES_TOTAL_THICKNESS] = stcResultAlign.dTotalThickness;

				for (int j = 0; j < MAX_LAYER; j++)
				{
					pView->m_pAuto->m_clResult.SetCol(RESULT_LAYER_1 + j);
					strWork.Format("%s", pView->m_pAuto->m_fnGetSelectResultData(stcResultAlign, pView->m_pAuto->m_nMeasureItem-2, j + 1));
					pView->m_pAuto->m_clResult.SetText(strWork);
				}

				pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][RES_THICKNESS] = stcResultAlign.stLayer[i].dThickness;
				pView->m_pTask->stData.m_dDataZ[nSEQ][i][RES_THICKNESS] = stcResultAlign.stLayer[i].dThickness;

				pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][RES_N_550] = stcResultAlign.stLayer[i].dN_550;
				pView->m_pTask->stData.m_dDataZ[nSEQ][i][RES_N_550] = stcResultAlign.stLayer[i].dN_550;

				pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][RES_K_550] = stcResultAlign.stLayer[i].dK_550;
				pView->m_pTask->stData.m_dDataZ[nSEQ][i][RES_K_550] = stcResultAlign.stLayer[i].dK_550;

				pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][RES_N_632] = stcResultAlign.stLayer[i].dN_632;
				pView->m_pTask->stData.m_dDataZ[nSEQ][i][RES_N_632] = stcResultAlign.stLayer[i].dN_632;

				pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][RES_K_632] = stcResultAlign.stLayer[i].dK_632;
				pView->m_pTask->stData.m_dDataZ[nSEQ][i][RES_K_632] = stcResultAlign.stLayer[i].dK_632;
			}
			pView->m_pTask->stResult.nLayer = stcResultAlign.nLayerCnt;
			// 통계 데이터 계산
			for (int i = 0; i < MAX_LAYER; i++)
			{
				for (int j = 0; j < MAX_ITEM; j++)
				{
					if (nSEQ == 0)
					{
						pView->m_pTask->stData.m_dMax[i][j] = pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][j];
						pView->m_pTask->stData.m_dMin[i][j] = pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][j];
						pView->m_pTask->stData.m_dAvg[i][j] = pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][j];
						pView->m_pTask->stData.m_fTot[i][j] = 0.0f;
						pView->m_pTask->stData.m_iZeroCnt[i][j] = 0;
					}

					if (pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][j]>0)
					{
						if (pView->m_pTask->stData.m_dMax[i][j] < pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][j])
						{
							pView->m_pTask->stData.m_dMax[i][j] = pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][j];
						}
						if (pView->m_pTask->stData.m_dMin[i][j] > pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][j])
						{
							pView->m_pTask->stData.m_dMin[i][j] = pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][j];
						}
						pView->m_pTask->stData.m_dMax_Min[i][j] = abs(pView->m_pTask->stData.m_dMax[i][j] - pView->m_pTask->stData.m_dMin[i][j]);

						pView->m_pTask->stData.m_fTot[i][j] += (float)pView->m_pTask->stResult.stSRData[nSEQ].dwResult[i][j];
						pView->m_pTask->stData.m_dAvg[i][j] = (double)(pView->m_pTask->stData.m_fTot[i][j] / (float)(nSEQ + 1 - pView->m_pTask->stData.m_iZeroCnt[i][j]));

					} 
					else
					{
						pView->m_pTask->stData.m_iZeroCnt[i][j]++;
					}
				}
			}

			for (int i = 0; i < MAX_LAYER; i++)
			{
				for (int j = 0; j < MAX_ITEM; j++)
				{
					fStd[i][j] = 0.0f;
					for(int n=0; n < nSEQ + 1; n++)
						fStd[i][j] += (float)pow(pView->m_pTask->stResult.stSRData[n].dwResult[i][j] - pView->m_pTask->stData.m_dAvg[i][j], 2);

					pView->m_pTask->stData.m_dStd[i][j] = sqrt((double)(fStd[i][j] / (nSEQ + 1 - pView->m_pTask->stData.m_iZeroCnt[i][j])));

					pView->m_pTask->stData.m_dUniformity[i][j] = (pView->m_pTask->stData.m_dStd[i][j] / pView->m_pTask->stData.m_dAvg[i][j]);
					pView->m_pTask->stData.m_dContourMax[i][j] = pView->m_pTask->stData.m_dMax[i][j];
					pView->m_pTask->stData.m_dContourMin[i][j] = pView->m_pTask->stData.m_dMin[i][j];
					pView->m_pTask->stData.m_dContourStep[i][j] = pView->m_pTask->stData.m_dMax_Min[i][j] / 10.;
				}
			}

			sprintf_s(pView->m_pTask->stResult.szInspTackTime, sizeof(char) * 256, "Display: %.1f s", CheckTime.GetTimeInterval(2));
			OutputDebugString(pView->m_pTask->stResult.szInspTackTime);
			sprintf_s(pView->m_pTask->stResult.szInspTackTime, sizeof(char) * 256, "%.1f s", CheckTactTime.GetTimeInterval(2));
			pView->m_pAuto->m_fnSetView();

			iSqncNum = SQNC_BASE;
			break;

			//*리스트 개수만큼 반복
		case SQNC_LIST_REPEAT:
			if (iListCnt - 1 <= iCurCnt)
			{
				iSqncCur = MAX_SQNC_CNT;
				iSqncNum = SQNC_BASE;
				break;
			}

// 			pView->m_pManual->m_pElipso->m_bReadyDev = FALSE;//첫장후에는 고속모드

			iCurCnt++;
			iSqncCur = SQNC_BASE;
			iSqncNum = SQNC_BASE;
			break;

		case MAX_SQNC_CNT:
			bSqncFlag = FALSE;
			break;
		}
	}

	sprintf_s(pView->m_pTask->stResult.szInspTackTime, sizeof(char) * 256, "0.0 s");

	if (!bFnRes)
	{
#if _DEBUG
		AfxMessageBox(tmpMsg, MB_ICONHAND);
#endif
	}
	else
		TECmsgOut(L"모든 지정 위치 측정 완료 ", YELLOW, 200, 17, BLACK);

	return bFnRes;
}


BOOL CWorkRun::SqncTargetMove(CDPoint dPntRequestPos)
{
	CTimeInterval CheckTime;
	BOOL bSqncFlag = TRUE;
	BOOL bFnRes = TRUE;
	char tmpMsg[MAX_Buf] = { "Error_SqncTargetMove" };

	enum//실행 순서 바꾸려면 아래 순서를 바꿀것
	{
		SQNC_BASE = 0,

		//Z축 안전위치 이동
		SQNC_MOVE_SAFE_Z,
		//X,Y축 요청한 측정위치 이동
		SQNC_MOVE_XY,
		//AF 실시
		SQNC_AF,

		MAX_SQNC_CNT
	};
	int iSqncNum = SQNC_BASE;
	int iSqncCur = SQNC_BASE;

	while (bSqncFlag)
	{
		Sleep(100);
		if (!pWorkRun->m_bThrdFlag[TF_AUTORUN])
		{
			iSqncCur = MAX_SQNC_CNT;
			iSqncNum = SQNC_BASE;
		}

		switch (iSqncNum)
		{
		case SQNC_BASE:
			if (bFnRes == FALSE)//검사중지
			{
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//일시정지
				break;

			if (iSqncCur == MAX_SQNC_CNT)//시퀀스 정상종료
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//증가하고 대입
			break;

		case SQNC_MOVE_SAFE_Z:
			if (m_bZzeroMove)
			{
				pAxis[Z_AXIS]->fVelocity = (float)pView->m_pTask->stCommon.dInspZSpeed;// *10.f;
				bFnRes = pAxis[Z_AXIS]->MoveAxis(pView->m_pTask->stCommon.dZMSafety, 0, TRUE);
			}

			iSqncNum = SQNC_BASE;
			break;

		case SQNC_MOVE_XY:
			pAxis[X_AXIS]->fVelocity = (float)pView->m_pTask->stCommon.dInspXYSpeed;
			pAxis[Y_AXIS]->fVelocity = (float)pView->m_pTask->stCommon.dInspXYSpeed;
			if (pView->m_pAuto->m_nWaperSize == WAPER_4)
			{
				pAxis[X_AXIS]->fVelocity *= 0.06f;
				pAxis[Y_AXIS]->fVelocity *= 0.06f;
			}
			bFnRes = pAxis[X_AXIS]->MoveAxis(dPntRequestPos.x, 0, TRUE);
			if (bFnRes)
				bFnRes = pAxis[Y_AXIS]->MoveAxis(dPntRequestPos.y, 0, TRUE);

			iSqncNum = SQNC_BASE;
			break;

		case SQNC_AF:
			if (m_bZzeroMove)
				ThrdAF(pView->m_pAuto->m_ctrlChkUseAF.GetCheck());

			iSqncNum = SQNC_BASE;
			break;

		case MAX_SQNC_CNT:
			bSqncFlag = FALSE;
			break;
		}
	}

#if _DEBUG
	if (!bFnRes)
	{
		AfxMessageBox(tmpMsg, MB_ICONHAND);
	}
	else
		TECmsgOut(L"측정위치 이동 완료 ", YELLOW, 200, 17, BLACK);
#endif

	return bFnRes;


}


BOOL CWorkRun::SqncCorrectAlign(t_ELIPSO_RESULT_INFO& stcResultAlign)
{
	CTimeInterval CheckTime;
	CTimeInterval CheckTimeAlign;
	CTimeInterval CheckTimeMsg;
	BOOL bSqncFlag = TRUE;
	BOOL bFnRes = TRUE;
	char tmpMsg[MAX_Buf] = { "Error_SqncCorrectAlign" };

	double dMovePos;

	enum//실행 순서 바꾸려면 아래 순서를 바꿀것
	{
		SQNC_BASE = 0,

		//엘립소 얼라인 실시
		SQNC_GET_ALIGN_Z,
		SQNC_CHECK_ALIGN_Z,
		SQNC_MOVE_ALIGN_Z,

		MAX_SQNC_CNT
	};
	int iSqncNum = SQNC_BASE;
	int iSqncCur = SQNC_BASE;

	int ArrCount = 0;
	int i = 0;

	CheckTime.SetTimeStart();
	CheckTimeMsg.SetTimeStart();
	while (bSqncFlag)
	{
		if ((iSqncCur == SQNC_GET_ALIGN_Z
			|| iSqncCur == SQNC_CHECK_ALIGN_Z
			|| iSqncCur == SQNC_MOVE_ALIGN_Z)
			&& CheckTimeMsg.GetTimeInterval() > 3000)
		{
			TECmsgOut(L"위치정렬 중 입니다", YELLOW, 500, 20, BLACK, 0);
			CheckTimeMsg.SetTimeStart();
		}

		if (bFnRes && CheckTime.GetTimeInterval() > 60000)
		{
			TECmsgOut(L"Align 시간(60sec) 초과입니다.", LIGHTRED, 1);
			CheckTime.SetTimeStart();
			sprintf_s(tmpMsg, sizeof(tmpMsg), "%s-TimeOut Measure-%d", tmpMsg, iSqncCur);
			bFnRes = FALSE;
			iSqncNum = SQNC_BASE;
		}

// 		Sleep(200);
		if (!pWorkRun->m_bThrdFlag[TF_AUTORUN])
		{
			iSqncCur = MAX_SQNC_CNT;
			iSqncNum = SQNC_BASE;
		}

		switch (iSqncNum)
		{
		case SQNC_BASE:
			if (bFnRes == FALSE)//검사중지
			{
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//일시정지
				break;

			if (iSqncCur == MAX_SQNC_CNT)//시퀀스 정상종료
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//증가하고 대입
			break;

		case SQNC_GET_ALIGN_Z:
			pView->m_pManual->m_pElipso->m_fnGetAlignZ();
			CheckTimeAlign.SetTimeStart();
			iSqncNum = SQNC_BASE;
			break;

		case SQNC_CHECK_ALIGN_Z:
			pView->m_pManual->m_pElipso->m_fnGetResult(stcResultAlign);
			if (stcResultAlign.bComplete
				|| !pView->m_pManual->m_pElipso->m_bResultEnd)
			{
				if (stcResultAlign.bError)
				{
					m_bThrdFlag[TF_AUTORUN] = FALSE;
					if (TECmsgOut(L"엘립소메타 ""정밀얼라인""을 시도하시겠습니까?", LIGHTMAGENTA, 2))//완료하지 못했습니다. 다시
					{
						memset(&stcResultAlign, 0x00, sizeof(t_ELIPSO_RESULT_INFO));

						pAxis[Z_AXIS]->fVelocity = (float)pView->m_pTask->stCommon.dInspZSpeed;
						dMovePos = pView->m_pModel->m_fnGetAlignZ(pView->m_pAuto->m_dWaperThickness);
						pAxis[Z_AXIS]->MoveAxis(dMovePos, 0, TRUE);

						CheckTime.SetTimeStart();
						iSqncCur = SQNC_GET_ALIGN_Z - 1;
					}
					else
						bFnRes = FALSE;
					m_bThrdFlag[TF_AUTORUN] = TRUE;
				}
			}
			else
			{
				if (CheckTimeAlign.GetTimeInterval() > 8000)
					iSqncCur = SQNC_BASE;//결과 안나오면 처음부터 다시
				else
					break;
			}
			iSqncNum = SQNC_BASE;
			break;

		case SQNC_MOVE_ALIGN_Z:
			if (fabs(pView->m_pManual->m_pElipso->m_dAlignZ) >= 0.0025)
			{
// 				pAxis[Z_AXIS]->fVelocity = (float)pView->m_pTask->stCommon.dSpeedSlowZ;
				pAxis[Z_AXIS]->fVelocity = (float)pView->m_pTask->stCommon.dInspZSpeed;
				bFnRes = pAxis[Z_AXIS]->MoveAxis(pView->m_pManual->m_pElipso->m_dAlignZ*(-1.0), 1, TRUE);

				memset(&stcResultAlign, 0x00, sizeof(t_ELIPSO_RESULT_INFO));
				iSqncCur = SQNC_BASE;//처음부터 다시
			}

			iSqncNum = SQNC_BASE;
			break;

		case MAX_SQNC_CNT:
			bSqncFlag = FALSE;
			break;
		}
	}

#if _DEBUG
	if (!bFnRes)
	{
		AfxMessageBox(tmpMsg, MB_ICONHAND);
	}
	else
		TECmsgOut(L"위치정렬 완료 ", YELLOW, 200, 17, BLACK);
#endif

	return bFnRes;
}


BOOL CWorkRun::SqncMeasure(t_ELIPSO_RESULT_INFO& stcResultMeasure)
{
	CTimeInterval CheckTime;
	CTimeInterval CheckTimeMsg;
	CTimeInterval CheckTimeIF;
	BOOL bSqncFlag = TRUE;
	BOOL bFnRes = TRUE;
	char tmpMsg[MAX_Buf] = { "Error_SqncMeasure" };

	enum//실행 순서 바꾸려면 아래 순서를 바꿀것
	{
		SQNC_BASE = 0,

// 		//엘립소 얼라인 실시
// 		SQNC_GET_ALIGN_Z,
// 		SQNC_CHECK_ALIGN_Z,
// 		SQNC_MOVE_ALIGN_Z,
		//측정
		SQNC_MEASURE,
		//결과변수 DATA입력
		SQNC_DATA,
		//디스플레이	
		SQNC_DISPLAY,

		MAX_SQNC_CNT
	};
	int iSqncNum = SQNC_BASE;
	int iSqncCur = SQNC_BASE;

	if (m_strRecipeName.IsEmpty())
	{
		TECmsgOut(L"알맞은 측정용 Receipe를 선택하세요.", LIGHTMAGENTA, 1);
// 		m_strRecipeName = _T("201110-AIM.recipe");
		bFnRes = FALSE;
		bSqncFlag = FALSE;
	}
	int ArrCount = 0;
	int i = 0;

	CString strRecipeName = m_strRecipeName;
	CheckTime.SetTimeStart();
	CheckTimeMsg.SetTimeStart();
	while (bSqncFlag)
	{
		if ((iSqncCur == SQNC_MEASURE
			|| iSqncCur == SQNC_DATA
			|| iSqncCur == SQNC_DISPLAY)
			&& CheckTimeMsg.GetTimeInterval() > 3000)
		{
			TECmsgOut(L"측정중입니다", LIGHTCYAN, 500, 20, BLACK, 0);
			CheckTimeMsg.SetTimeStart();
		}
		if (bFnRes && CheckTime.GetTimeInterval() > 60000)
		{
			TECmsgOut(L"측정시간(60sec) 초과입니다.", LIGHTRED, 1);
			CheckTime.SetTimeStart();
			sprintf_s(tmpMsg, sizeof(tmpMsg), "%s-TimeOut Measure-%d", tmpMsg, iSqncCur);
			bFnRes = FALSE;
			iSqncNum = SQNC_BASE;
		}

// 		Sleep(200);
		if (!pWorkRun->m_bThrdFlag[TF_AUTORUN])
		{
			iSqncCur = MAX_SQNC_CNT;
			iSqncNum = SQNC_BASE;
		}

		switch (iSqncNum)
		{
		case SQNC_BASE:
			if (bFnRes == FALSE)//검사중지
			{
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//일시정지
				break;

			if (iSqncCur == MAX_SQNC_CNT)//시퀀스 정상종료
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//증가하고 대입
			break;

		case SQNC_MEASURE:
			CheckTimeIF.SetTimeStart();
			pView->m_pManual->m_pElipso->m_fnSetMeasureStart(strRecipeName);
			CheckTime.SetTimeStart();

			iSqncNum = SQNC_BASE;
			break;


		case SQNC_DATA:
			pView->m_pManual->m_pElipso->m_fnGetResult(stcResultMeasure);
			if (stcResultMeasure.bComplete)
			{
				sprintf_s(pView->m_pTask->stResult.szInspTackTime, sizeof(char) * 256, "GetResult_Measure: %.1f s", CheckTimeIF.GetTimeInterval(2));
				OutputDebugString(pView->m_pTask->stResult.szInspTackTime);
				CheckTimeIF.SetTimeStart();
				if (stcResultMeasure.bError)
				{
					if (TECmsgOut(L"엘립소메타 ""측정""을 완료하지 못했습니다. 다시 시도하시겠습니까?", LIGHTMAGENTA, 2))
					{
						CheckTime.SetTimeStart();
						iSqncCur = SQNC_MEASURE - 1;
					}
					else
						bFnRes = FALSE;
				}
			}
			else
				break;

			iSqncNum = SQNC_BASE;
			break;

		case SQNC_DISPLAY:
			ArrCount = pView->m_pManual->m_pElipso->m_strResult.GetCount();
			pView->m_pModel->m_ctrlListMeasureResult.ResetContent();
			for (i=0;i<ArrCount;i++)
			{
				pView->m_pModel->m_ctrlListMeasureResult.InsertString(-1, pView->m_pManual->m_pElipso->m_strResult.GetAt(i));
			}
//			pView->m_pModel->GetDlgItem(IDC_LIST_MEASURE_RESULT)->SetWindowText(pView->m_pManual->m_pElipso->m_strAnalysisData);
// 			pView->m_pContour->m_fnSetChart();
			sprintf_s(pView->m_pTask->stResult.szInspTackTime, sizeof(char) * 256, "Display_Measure: %.1f s", CheckTimeIF.GetTimeInterval(2));
			OutputDebugString(pView->m_pTask->stResult.szInspTackTime);
			iSqncNum = SQNC_BASE;
			break;

		case MAX_SQNC_CNT:
			bSqncFlag = FALSE;
			break;
		}
	}

#if _DEBUG
	if (!bFnRes)
	{
		AfxMessageBox(tmpMsg, MB_ICONHAND);
	}
	else
		TECmsgOut(L"측정 완료 ", YELLOW, 200, 17, BLACK);
#endif

	return bFnRes;
}



BOOL CWorkRun::SqncPattReg()
{
	CTimeInterval CheckTime;
	BOOL bSqncFlag = TRUE;
	BOOL bFnRes = TRUE;
	char tmpMsg[MAX_Buf] = { "Error_SqncPattReg" };

	CDPoint dPntTargetPos;
	double dCurPosZ = 0.0;

	int iAlignPattNo = 0;
	CString strVal;

	enum//실행 순서 바꾸려면 아래 순서를 바꿀것
	{
		SQNC_BASE = 0,

		SQNC_SET_MODEL_DLG,
		SQNC_MODEL_DLG,
		SQNC_MODEL_SELECT,

		SQNC_PATTERN_REG_DLG,
		SQNC_PATT_FIND,

		MAX_SQNC_CNT
	};
	int iSqncNum = SQNC_BASE;
	int iSqncCur = SQNC_BASE;

	int ArrExcepID_1[9] = {
		IDC_LST_MODEL,
		IDC_BTN_ADD,
		IDC_BTN_RENAME,
		IDC_BTN_COPY,
		IDC_BTN_SAVE,
		IDC_RADIO_MODEL_TYPE1,
		IDC_RADIO_MODEL_TYPE2,
		IDC_RADIO_MODEL_TYPE3,
		IDC_RADIO_MODEL_TYPE4
	};
	int ArrExcepID_2[4] = {
		IDC_CBO_INCH,
		IDC_EDT_THICKNESS,
		IDC_EDT_EXCLUSION,
		IDC_BTN_SET_PATT
	};
	int ArrExcepID_3[5] = {
		IDC_EDT_PATT_SIZE_X,
		IDC_EDT_PATT_SIZE_Y,
		IDC_EDT_PATT_GAB_X,
		IDC_EDT_PATT_GAB_Y,
		IDC_BTN_ALIGN_POS_AUTO_SET
	};

	pView->m_pStageMap->m_fnCheckCamera(TRUE);

	while (bSqncFlag)
	{
		Sleep(100);
		if (!pWorkRun->m_bThrdFlag[TF_AUTORUN])
		{
			iSqncCur = MAX_SQNC_CNT;
			iSqncNum = SQNC_BASE;
		}

		switch (iSqncNum)
		{
		case SQNC_BASE:
			if (bFnRes == FALSE)//검사중지
			{
				TECmsgOut(L"Stop Pattern Reg", LIGHTMAGENTA, 1500);
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//일시정지
				break;

			if (iSqncCur == MAX_SQNC_CNT)//시퀀스 정상종료
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//증가하고 대입
			break;


			//모델창 뷰 - 모델관련버튼 활성
		case SQNC_SET_MODEL_DLG:
			pView->m_fnSetSwitch(SW_MODEL);

			EnableDlgItem(pView->m_pModel, FALSE);

			if (m_bThrdFlag[TF_CASSETTE_START] == FALSE)
				iSqncCur = SQNC_PATT_FIND;

			iSqncNum = SQNC_BASE;
			break;


			//카메라 ON
			//Auto포커싱
		case SQNC_MODEL_DLG:
			EnableDlgItem(pView->m_pModel, FALSE, ArrExcepID_1, 9);
			pView->m_pModel->m_fnSetRadioModelType(MODEL_TYPE_PATTERN);
			m_bSqncInterrupt[SQITRT_SELECT_MODEL] = FALSE;

			pView->m_pModel->m_ctrlListMeasureResult.ResetContent();
			pView->m_pModel->m_ctrlListMeasureResult.AddString(_T("STEP 1. 패턴등록할 모델명을 선택(or 추가) 해주세요."));

			iSqncNum = SQNC_BASE;
			break;


			//웨이퍼 정보입력창 활성
			//패턴등록 버튼활성
		case SQNC_MODEL_SELECT:
			if (m_bThrdFlag[TF_CASSETTE_START])
			{
				if (!m_bSqncInterrupt[SQITRT_SELECT_MODEL])// 모델선택
					break;
			}
			else
			{
				pView->m_pModel->m_fnModule_ModelLoad(m_strPatternName);
			}
			m_bSqncInterrupt[SQITRT_SELECT_MODEL] = FALSE;

			EnableDlgItem(pView->m_pModel, FALSE, ArrExcepID_2, 4);
			m_bSqncInterrupt[SQITRT_REG_PATTERN] = FALSE;
// 			pView->m_pModel->m_fnCheckAlignUse(TRUE);
			pView->m_pModel->m_fnSetRadioModelType(MODEL_TYPE_PATTERN);

			pView->m_pModel->m_ctrlListMeasureResult.AddString(_T("STEP 2. 웨이퍼 '측정정보'확인 및 '얼라인 패턴등록'을 수행 해 주세요."));

			iSqncNum = SQNC_BASE;
			break;


			//패턴저장 경로 모델명 폴더 추가@
			//패스버튼 생성 - 기존패턴정보 로딩
			//닫을 때 패턴파일 유무확인 - 없으면 생성 및 저장 요청  - utilclass - Existence(풀경로)
			//닫히면 패턴찾기 버튼활성
		case SQNC_PATTERN_REG_DLG:
			if (!m_bSqncInterrupt[SQITRT_REG_PATTERN])
				break;
			m_bSqncInterrupt[SQITRT_REG_PATTERN] = FALSE;

			pView->m_pModel->GetDlgItem(IDC_BTN_FIND_PATT)->EnableWindow(TRUE);//프리징 위험

			m_bSqncInterrupt[SQITRT_FIND_PATTERN] = FALSE;

			pView->m_pModel->m_ctrlListMeasureResult.AddString(_T("STEP 3. 등록한 패턴이 화면에 표시되게 맞춘뒤,  "));
			pView->m_pModel->m_ctrlListMeasureResult.AddString(_T("           '패턴찾기'를 수행 해주세요."));

			iSqncNum = SQNC_BASE;
			break;


			//화면상에서 패턴 찾으면 현재위치로 
			//sheet 1행에 0행/0열 기준좌표 등록 - 측정위치추가 버튼 코드 참고
			//패턴간격정보 활성
		case SQNC_PATT_FIND:
			if (m_bThrdFlag[TF_CASSETTE_START])
			{
				if (!m_bSqncInterrupt[SQITRT_FIND_PATTERN])
					break;
			}
			else
			{
				pView->m_pModel->m_fnModule_FindPatt();
			}

			m_bSqncInterrupt[SQITRT_FIND_PATTERN] = FALSE;
			pView->m_pModel->m_fnPosInfoInputSheet(1, 0, 0);
// 			EnableDlgItem(pView->m_pModel, FALSE, ArrExcepID_3, 5);
// 			pView->m_pStageMap->m_fnCheckCamera(FALSE);
			iSqncNum = SQNC_BASE;
			break;

			
		case MAX_SQNC_CNT:
			bSqncFlag = FALSE;
			break;
		}
	}

	if (!bFnRes)
	{
#if _DEBUG
		AfxMessageBox(tmpMsg, MB_ICONHAND);
#endif
	}
	else
	{
		TECmsgOut(L"패턴 등록 완료", LIGHTBLUE, 1000);
	}

	return bFnRes;
}

BOOL CWorkRun::SqncSubReg()
{
	CTimeInterval CheckTime;
	BOOL bSqncFlag = TRUE;
	BOOL bFnRes = TRUE;
	char tmpMsg[MAX_Buf] = { "Error_SqncSubReg" };

	CDPoint dPntTargetPos;
	double dCurPosZ = 0.0;

	int iAlignPattNo = 0;
	CString strVal;

	enum//실행 순서 바꾸려면 아래 순서를 바꿀것
	{
		SQNC_BASE = 0,

		SQNC_MOVE_BASE_PNT,
		SQNC_SUB_MODEL_SELECT,
		SQNC_SUB_POSITION_REG,

		MAX_SQNC_CNT
	};
	int iSqncNum = SQNC_BASE;
	int iSqncCur = SQNC_BASE;

	int ArrExcepID_1[9] = {
		IDC_LST_MODEL,
		IDC_BTN_ADD,
		IDC_BTN_RENAME,
		IDC_BTN_COPY,
		IDC_BTN_SAVE,
		IDC_RADIO_MODEL_TYPE1,
		IDC_RADIO_MODEL_TYPE2,
		IDC_RADIO_MODEL_TYPE3,
		IDC_RADIO_MODEL_TYPE4
	};
	int ArrExcepID_4[2] = {
		IDC_FPSPREAD_SR_INFO,
		IDC_BTN_SAVE
	};

	while (bSqncFlag)
	{
		Sleep(100);
		if (!pWorkRun->m_bThrdFlag[TF_AUTORUN])
		{
			iSqncCur = MAX_SQNC_CNT;
			iSqncNum = SQNC_BASE;
		}

		switch (iSqncNum)
		{
		case SQNC_BASE:
			if (bFnRes == FALSE)//검사중지
			{
				TECmsgOut(L"Stop SubPoint Setting", LIGHTMAGENTA, 1500);
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//일시정지
				break;

			if (iSqncCur == MAX_SQNC_CNT)//시퀀스 정상종료
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//증가하고 대입
			break;


		case SQNC_MOVE_BASE_PNT:
			dPntTargetPos.x = pView->m_pModel->m_pTask->stModel.dPattPosX;
			dPntTargetPos.y = pView->m_pModel->m_pTask->stModel.dPattPosY;
// 			dPntTargetPos.x = dPntTargetPos.x < -200 ? -200.0 : dPntTargetPos.x > 200 ? 200 : dPntTargetPos.x;
// 			dPntTargetPos.y = dPntTargetPos.y < -200 ? -200.0 : dPntTargetPos.y > 200 ? 200 : dPntTargetPos.y;
			pAxis[X_AXIS]->fVelocity = (float)pView->m_pModel->m_pTask->stCommon.dInspXYSpeed;
			pAxis[Y_AXIS]->fVelocity = (float)pView->m_pModel->m_pTask->stCommon.dInspXYSpeed;

			bFnRes = SqncTargetMove(dPntTargetPos);

			iSqncNum = SQNC_BASE;
			break;

			//Check Sub 자동//Align버튼 비활성
			//생성 : 이름앞에 Sub_
			//불러오기 : Sub_ 아니면 안됨.
		case SQNC_SUB_MODEL_SELECT:
// 			pView->m_pModel->m_ctrlCheckSetSubPoint.SetCheck(TRUE);
			pView->m_pModel->m_fnSetRadioModelType(MODEL_TYPE_SUB);

			EnableDlgItem(pView->m_pModel, FALSE, ArrExcepID_1, 9);
			m_bSqncInterrupt[SQITRT_SELECT_MODEL] = FALSE;

			pView->m_pModel->m_ctrlListMeasureResult.AddString(_T("STEP 5. 측정Sub위치를 등록할 모델명을 선택(or 추가) 해주세요.  "));

			iSqncNum = SQNC_BASE;
			break;


			//등록시 기준좌표를 뺀 상대좌표로 입력
			//완료
		case SQNC_SUB_POSITION_REG:
			if (m_bThrdFlag[TF_CASSETTE_START])
			{
				if (!m_bSqncInterrupt[SQITRT_SELECT_MODEL])// 모델선택
					break;
			}
			else
			{
				pView->m_pModel->m_fnModule_ModelLoad(m_strSubName);
			}
			m_bSqncInterrupt[SQITRT_SELECT_MODEL] = FALSE;

			m_bSqncInterrupt[SQITRT_REG_POSITION] = FALSE;
			TECmsgOut(L"Cam 화면을 보면서 측정을 원하는 위치에서 좌표를 각각 등록해주세요.", LIGHTCYAN, 1, 17, BLACK);

			EnableDlgItem(pView->m_pModel, FALSE, ArrExcepID_4, 2);
			pView->m_pModel->GetDlgItem(IDC_FPSPREAD_SR_INFO)->EnableWindow(TRUE);//이방식아니면 안됨....
			pView->m_pStageMap->m_fnCheckCamera(TRUE);

			pView->m_pModel->m_ctrlListMeasureResult.AddString(_T("STEP 6. 현재 화면에 표시된 마크를 기준(0,0)으로 하는  "));
			pView->m_pModel->m_ctrlListMeasureResult.AddString(_T("           측정 Sub포인트를 등록하세요."));
			pView->m_pModel->m_ctrlListMeasureResult.AddString(_T("           * '조이스틱'으로 이동 후 '좌표 읽기'"));
			pView->m_pModel->m_ctrlListMeasureResult.AddString(_T("           * 등록 완료 후, '저장'해주세요."));

			iSqncNum = SQNC_BASE;
			break;


		case MAX_SQNC_CNT:
			if (!m_bSqncInterrupt[SQITRT_REG_POSITION])
				break;
			m_bSqncInterrupt[SQITRT_REG_POSITION] = FALSE;

			bSqncFlag = FALSE;
			break;
		}
	}

	if (!bFnRes)
	{
#if _DEBUG
		AfxMessageBox(tmpMsg, MB_ICONHAND);
#endif
	}
	else
	{
		TECmsgOut(L"Sub포인트 등록 완료", LIGHTBLUE, 1000);
	}

	return bFnRes;
}

BOOL CWorkRun::SqncAlignMapping()
{
	CTimeInterval CheckTime;
	BOOL bSqncFlag = TRUE;
	BOOL bFnRes = TRUE;
	char tmpMsg[MAX_Buf] = { "Error_SqncAlignMapping" };

	CDPoint dPntTargetPos;
	double dCurPosZ = 0.0;

	int iAlignPattNo = 0;
	CString strVal;
	CDPoint dTargetPos;
	CString strCalPath;

	enum//실행 순서 바꾸려면 아래 순서를 바꿀것
	{
		SQNC_BASE = 0,

		SQNC_PATT_INFO,

// 		SQNC_POSITION_REG,
		SQNC_POSITION_CALC,

		SQNC_MOVE_ALIGN_POS,
		SQNC_PATT_FIND_ALIGN_POS,
		SQNC_MOVE_ALIGN_POS_2,
		SQNC_PATT_FIND_ALIGN_POS_2,
		SQNC_PATT_CONFIRM_ALIGN_POS,
		//성공 - 해당좌표 기준점등록
		//실패 - 조그운전 패턴찾고 기준위치 수동등록

		SQNC_MAPPING,
		//자동검사창으로 이동
		SQNC_CALC_POS,
		//Target + Sub 좌표로
		//계산후 자동검사창 sheet에 뿌려줌

		MAX_SQNC_CNT
	};
	int iSqncNum = SQNC_BASE;
	int iSqncCur = SQNC_BASE;

	int ArrExcepID_1[9] = {
		IDC_LST_MODEL,
		IDC_BTN_ADD,
		IDC_BTN_RENAME,
		IDC_BTN_COPY,
		IDC_BTN_SAVE,
		IDC_RADIO_MODEL_TYPE1,
		IDC_RADIO_MODEL_TYPE2,
		IDC_RADIO_MODEL_TYPE3,
		IDC_RADIO_MODEL_TYPE4
	};
	int ArrExcepID_2[4] = {
		IDC_CBO_INCH,
		IDC_EDT_THICKNESS,
		IDC_EDT_EXCLUSION,
		IDC_BTN_SET_PATT
	};
	int ArrExcepID_3[8] = {
		IDC_CBO_INCH,
		IDC_EDT_THICKNESS,
		IDC_EDT_EXCLUSION,
		IDC_EDT_PATT_SIZE_X,
		IDC_EDT_PATT_SIZE_Y,
		IDC_EDT_PATT_GAB_X,
		IDC_EDT_PATT_GAB_Y,
		IDC_BTN_ALIGN_POS_AUTO_SET
	};
	int ArrExcepID_5[10] = {
		IDC_LST_MODEL,
		IDC_BTN_ADD,
		IDC_BTN_RENAME,
		IDC_BTN_COPY,
		IDC_BTN_SAVE,
		IDC_RADIO_MODEL_TYPE1,
		IDC_RADIO_MODEL_TYPE2,
		IDC_RADIO_MODEL_TYPE3,
		IDC_RADIO_MODEL_TYPE4,
		IDC_BTN_ADD_POINT
// 		IDC_EDT_PATT_SIZE_X,
// 		IDC_EDT_PATT_SIZE_Y,
// 		IDC_EDT_PATT_GAB_X,
// 		IDC_EDT_PATT_GAB_Y,
// 		IDC_CBO_INCH,
// 		IDC_EDT_THICKNESS,
// 		IDC_EDT_EXCLUSION
	};


	EnableDlgItem(pView->m_pModel, FALSE, ArrExcepID_3, 8);
	m_bSqncInterrupt[SQITRT_AUTO_CAL_PATTERN_INFO] = FALSE;
	m_bSqncInterrupt[SQITRT_REG_POSITION] = FALSE;
	pView->m_pModel->GetDlgItem(IDC_FPSPREAD_SR_INFO)->EnableWindow(TRUE);//이방식아니면 안됨....

	pView->m_pModel->m_ctrlListMeasureResult.AddString(_T("STEP 4. 'ALIGN 계산정보'입력 및 웨이퍼 '측정정보'확인 후,"));
	pView->m_pModel->m_ctrlListMeasureResult.AddString(_T("           '기준 ALIGN마크 좌표 자동계산'을 수행 해 주세요."));

	while (bSqncFlag)
	{
		Sleep(100);
		if (!pWorkRun->m_bThrdFlag[TF_AUTORUN])
		{
			iSqncCur = MAX_SQNC_CNT;
			iSqncNum = SQNC_BASE;
		}

		switch (iSqncNum)
		{
		case SQNC_BASE:
			if (bFnRes == FALSE)//검사중지
			{
				TECmsgOut(L"Stop Pattern Setting", LIGHTMAGENTA, 1500);
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//일시정지
				break;

			if (iSqncCur == MAX_SQNC_CNT)//시퀀스 정상종료
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//증가하고 대입
			break;


			//[자동등록계산]버튼 내 기능--------------
			//패턴크기/간격 정보입력후 등록클릭
			//예상 패턴위치 MapView에 표시(남색) (패턴각도고려-보류)
			//스테이지맵뷰draw에 기록
			//지정위치 자동 선정(오랜지색) - 현재위치 기준 十방향으로 4포인트 - 외각에서 한칸 안
			//sheet 2~5행에 좌표 등록
		case SQNC_PATT_INFO:
			if (m_bThrdFlag[TF_CASSETTE_START])
			{ 
				if (!m_bSqncInterrupt[SQITRT_AUTO_CAL_PATTERN_INFO])
					break;
			}
			else
			{ 
				pView->m_pModel->m_fnModule_CalPattInfo();
			}
// 			pView->m_pStageMap->m_fnCheckCamera(TRUE);
			m_bSqncInterrupt[SQITRT_AUTO_CAL_PATTERN_INFO] = FALSE;
			m_bSqncInterrupt[SQITRT_REG_POSITION] = FALSE;//앞선버튼클릭으로 인한 변수 초기화

			iSqncNum = SQNC_BASE;
			break;


// 			//생성 : 이름앞에 Taget_@
// 			//불러오기 : Taget_ 아니면 안됨.@
// 			//웨이퍼 정보입력창 활성
// 			//패턴등록 버튼활성
// 		case SQNC_POSITION_REG:
// 			EnableDlgItem(pView->m_pModel, FALSE, ArrExcepID_5, 10);
// // 			pView->m_pModel->m_fnCheckAlignUse(TRUE);
// // 			pView->m_pModel->m_ctrlCheckSetSubPoint.SetCheck(FALSE);
// 			pView->m_pModel->m_fnSetRadioModelType(MODEL_TYPE_TARGET);
// 
// 			pView->m_pModel->GetDlgItem(IDC_FPSPREAD_SR_INFO)->EnableWindow(TRUE);//이방식아니면 안됨....
// 
// 			pView->m_pModel->m_ctrlListMeasureResult.AddString(_T("STEP 5. 측정할 타겟 모델을 선택(or 추가) 해 주세요."));
// 
// 			iSqncNum = SQNC_BASE;
// 			break;


			//XXX-sheet 활성
			//XXX-6행부터 사용자 등록가능
			//XXX-전체 열행 등록(예상위치 열행 모두등록)
			//완료 - SubPoint 생성확인
		case SQNC_POSITION_CALC:
// 			if (!m_bSqncInterrupt[SQITRT_REG_TAGET_POSITION])
// 				break;
			m_bSqncInterrupt[SQITRT_REG_POSITION] = FALSE;
			pView->m_pStageMap->m_fnCheckCamera(TRUE);

			m_bSqncInterrupt[SQITRT_FIND_PATTERN_FAIL] = TRUE;

			iSqncNum = SQNC_BASE;
			break;


		case SQNC_MOVE_ALIGN_POS:
// 			if (!pView->m_pModel->m_bSetInsertSubPosFinish)// 모델저장
// 				break;
// 			pView->m_pModel->m_bSetInsertSubPosFinish = FALSE;

// 		{//test
// 			pTask->AlignCenter.x = pView->m_pStageMap->m_dPntAlign[iAlignPattNo].x;
// 			pTask->AlignCenter.y = pView->m_pStageMap->m_dPntAlign[iAlignPattNo].y;
// 			iSqncCur = SQNC_PATT_FIND_ALIGN_POS;
// 			iSqncNum = SQNC_BASE;
// 			pTask->ErrorCode = FALSE;
// 			break;
// 		}

			pAxis[X_AXIS]->fVelocity = (float)pView->m_pTask->stCommon.dInspXYSpeed*1.f;
			pAxis[Y_AXIS]->fVelocity = (float)pView->m_pTask->stCommon.dInspXYSpeed*1.f;

			bFnRes = pAxis[X_AXIS]->MoveAxis(pView->m_pStageMap->m_dPntAlign[iAlignPattNo].x, 0, TRUE);
			if (bFnRes)
				bFnRes = pAxis[Y_AXIS]->MoveAxis(pView->m_pStageMap->m_dPntAlign[iAlignPattNo].y, 0, TRUE);

			iSqncNum = SQNC_BASE;
			break;


		case SQNC_PATT_FIND_ALIGN_POS:
// 			ThrdAF();
			m_bSqncInterrupt[SQITRT_FIND_PATTERN] = FALSE;

			pMILAlignDlg->m_fnBringImage(pCamJAI->m_CnvImageInfo.pImageBuffer, pCamJAI->m_CnvImageInfo.iImageSize);

			pTask->AlignCenter = GetPattAlignCenter(pTask->ImageBuff[pTask->PatternNum], pTask->PatternNum);
			pTask->ErrorCode = pInsp[DEF_DISP_SET]->ErrorCode;
			iSqncNum = SQNC_BASE;
			break;



		case SQNC_MOVE_ALIGN_POS_2:
			pAxis[X_AXIS]->StatusGetPos(&dTargetPos.x);
			pAxis[Y_AXIS]->StatusGetPos(&dTargetPos.y);
			dTargetPos.x += pTask->AlignCenter.x;
			dTargetPos.y += pTask->AlignCenter.y;

			bFnRes = pAxis[X_AXIS]->MoveAxis(dTargetPos.x, 0, TRUE);
			if (bFnRes)
				bFnRes = pAxis[Y_AXIS]->MoveAxis(dTargetPos.y, 0, TRUE);

			iSqncNum = SQNC_BASE;
			break;


		case SQNC_PATT_FIND_ALIGN_POS_2:
// 			ThrdAF();
			pMILAlignDlg->m_fnBringImage(pCamJAI->m_CnvImageInfo.pImageBuffer, pCamJAI->m_CnvImageInfo.iImageSize);

			pTask->AlignCenter = GetPattAlignCenter(pTask->ImageBuff[pTask->PatternNum], pTask->PatternNum);
			pTask->ErrorCode = pInsp[DEF_DISP_SET]->ErrorCode;
			iSqncNum = SQNC_BASE;
			break;


			//실패 - 조그운전 패턴찾고 기준위치 수동등록
			//성공 - 해당좌표 기준점등록
		case SQNC_PATT_CONFIRM_ALIGN_POS:
			if (pTask->ErrorCode)//실패
			{
				//실패상태이므로 확인해달라고 메세지(중복방지)
				if (m_bSqncInterrupt[SQITRT_FIND_PATTERN_FAIL])
				{
					m_bSqncInterrupt[SQITRT_FIND_PATTERN_FAIL] = FALSE;
					iSqncNum = iSqncCur = SQNC_MOVE_ALIGN_POS;
					break;
				}
// 				else
// 				{
// 					TECmsgOut(L"패턴을 찾지 못했습니다.", LIGHTMAGENTA, 1);
// 					bFnRes = FALSE;
// 					iSqncNum = SQNC_BASE;
// 				}
			}

			pAxis[X_AXIS]->StatusGetPos(&dTargetPos.x);
			pAxis[Y_AXIS]->StatusGetPos(&dTargetPos.y);
			dTargetPos.x += pTask->AlignCenter.x;
			dTargetPos.y += pTask->AlignCenter.y;

			pTask->XWorldArray[iAlignPattNo] = dTargetPos.x;
			pTask->YWorldArray[iAlignPattNo] = dTargetPos.y;

			iAlignPattNo++;
			if (iAlignPattNo < 4)
			{
				iSqncNum = iSqncCur = SQNC_MOVE_ALIGN_POS;
				break;
			}
			else
			{
				iSqncNum = SQNC_BASE;
				break;
			}

			iSqncNum = SQNC_BASE;
			break;


			//자동검사창으로 이동
		case SQNC_MAPPING:
			pTask->NumPoint = 4;
			for (int i = 0; i < pTask->NumPoint; i++)
			{
				pView->m_pModel->m_clSR.SetRow(i + 2);

				pView->m_pModel->m_clSR.SetCol(MEASURE_COL);
				strVal = pView->m_pModel->m_clSR.GetText();
				pTask->XPixArray[i] = atof(strVal);

				pView->m_pModel->m_clSR.SetCol(MEASURE_ROW);
				strVal = pView->m_pModel->m_clSR.GetText();
				pTask->YPixArray[i] = atof(strVal);
			}
			pTask->ErrorCode = pInsp[DEF_DISP_SET]->ErrorCode = 0;
			pTask->iGroupNum = 0;

			SetMotorAlign(pTask->iGroupNum, pTask->XPixArray, pTask->YPixArray, pTask->XWorldArray, pTask->YWorldArray, pTask->ImageBuff, pTask->NumPoint);
			memcpy(pTask->msgVision, pInsp[DEF_DISP_SET]->msgVision, sizeof(TCHAR)*512);
			pTask->ErrorCode = pInsp[DEF_DISP_SET]->ErrorCode;
			if (pTask->ErrorCode)
			{
				//종료시퀀스
				TECmsgOut(L"Mapping과정을 완료하지 못했습니다. 해당웨이퍼 검사를 종료합니다.", LIGHTMAGENTA, 5000);
				bFnRes = FALSE;
				pWorkRun->m_bThrdFlag[TF_AUTORUN] = FALSE;
				break;
			}
			else
			{
				strCalPath.Format(_T("%sModel\\%s\\"), pModelData->curDirName, m_strPatternName.GetString());
				pInsp[DEF_DISP_SET]->SaveCal(strCalPath);
				pView->m_pAuto->m_pTask->stModel.nSRData = 5;
				for (int i = 0; i < pView->m_pAuto->m_pTask->stModel.nSRData; i++)
				{
					pView->m_pModel->m_clSR.SetRow(i + 1);

					pView->m_pModel->m_clSR.SetCol(MEASURE_NAME);
					strVal = pView->m_pModel->m_clSR.GetText();
					if(strVal == "")
					{
						pView->m_pAuto->m_pTask->stModel.nSRData = i;
						break;
					}

					pTask->inX = (double)pView->m_pAuto->m_pTask->stModel.stSRData[i].iColum;
					pTask->inY = (double)pView->m_pAuto->m_pTask->stModel.stSRData[i].iRow;
					GetTransformCoordinate(pTask->iGroupNum, pTask->inX, pTask->inY, &pTask->OutX, &pTask->OutY);
					pView->m_pAuto->m_pTask->stModel.stSRData[i].dwXPosition = pTask->OutX;
					pView->m_pAuto->m_pTask->stModel.stSRData[i].dwYPosition = pTask->OutY;

// 					pView->m_pModel->m_clSR.SetCol(MEASURE_X);
// 					strVal.Format()
// 					pView->m_pModel->m_clSR.SetText();
// 					pTask->XPixArray[i] = atof(strVal);
// 
// 					pView->m_pModel->m_clSR.SetCol(MEASURE_Y);
// 					strVal = pView->m_pModel->m_clSR.GetText();
// 					pTask->YPixArray[i] = atof(strVal);

				}
// 				pView->m_pModel->m_fnSetModel();

				TECmsgOut(L"Align Mapping Complete", LIGHTGREEN, 1000);
			}

			iSqncNum = SQNC_BASE;
			break;


			//선택된 Target모델 네임 - Data구조체
			//선택된 Sub모델 네임 - Data구조체
			//Target + Sub 좌표로
			//계산후 자동검사창 sheet에 뿌려줌
		case SQNC_CALC_POS:
// 			EnableDlgItem(pView->m_pModel, TRUE);//모든버튼 활성

			iSqncNum = SQNC_BASE;
			break;


		case MAX_SQNC_CNT:
			bSqncFlag = FALSE;
			break;
		}
	}

	if (!bFnRes)
	{
#if _DEBUG
		AfxMessageBox(tmpMsg, MB_ICONHAND);
#endif
	}
	else
	{
		TECmsgOut(L"얼라인 완료", LIGHTBLUE, 1000);
	}
	EnableDlgItem(pView->m_pModel, TRUE);//모든버튼 활성

	return bFnRes;
}

BOOL CWorkRun::SqncTargetReg()
{
	CTimeInterval CheckTime;
	BOOL bSqncFlag = TRUE;
	BOOL bFnRes = TRUE;
	char tmpMsg[MAX_Buf] = { "Error_SqncTargetReg" };

	CDPoint dPntTargetPos;
	double dCurPosZ = 0.0;

	int iAlignPattNo = 0;
	CString strVal;

	enum//실행 순서 바꾸려면 아래 순서를 바꿀것
	{
		SQNC_BASE = 0,

		SQNC_MODEL_SELECT,

		SQNC_POSITION_REG,

		MAX_SQNC_CNT
	};
	int iSqncNum = SQNC_BASE;
	int iSqncCur = SQNC_BASE;

	int ArrExcepID_1[9] = {
		IDC_LST_MODEL,
		IDC_BTN_ADD,
		IDC_BTN_RENAME,
		IDC_BTN_COPY,
		IDC_BTN_SAVE,
		IDC_RADIO_MODEL_TYPE1,
		IDC_RADIO_MODEL_TYPE2,
		IDC_RADIO_MODEL_TYPE3,
		IDC_RADIO_MODEL_TYPE4
	};
	int ArrExcepID_5[17] = {
		IDC_LST_MODEL,
		IDC_BTN_ADD,
		IDC_BTN_RENAME,
		IDC_BTN_COPY,
		IDC_BTN_SAVE,
		IDC_BTN_ADD_POINT,
		IDC_EDT_PATT_SIZE_X,
		IDC_EDT_PATT_SIZE_Y,
		IDC_EDT_PATT_GAB_X,
		IDC_EDT_PATT_GAB_Y,
		IDC_CBO_INCH,
		IDC_EDT_THICKNESS,
		IDC_EDT_EXCLUSION,
		IDC_RADIO_MODEL_TYPE1,
		IDC_RADIO_MODEL_TYPE2,
		IDC_RADIO_MODEL_TYPE3,
		IDC_RADIO_MODEL_TYPE4
	};

	EnableDlgItem(pView->m_pModel, FALSE, ArrExcepID_1, 9);
	m_bSqncInterrupt[SQITRT_SELECT_MODEL] = FALSE;
	m_bSqncInterrupt[SQITRT_REG_POSITION] = FALSE;
	pView->m_pModel->m_fnSetRadioModelType(MODEL_TYPE_TARGET);

	pView->m_pModel->m_ctrlListMeasureResult.AddString(_T("STEP 7. 측정Target패턴 좌표(열/행)를 등록할 모델명을"));
	pView->m_pModel->m_ctrlListMeasureResult.AddString(_T("           선택(or 추가) 해주세요."));

	int nNo = -1;

	while (bSqncFlag)
	{
		Sleep(100);
		if (!pWorkRun->m_bThrdFlag[TF_AUTORUN])
		{
			iSqncCur = MAX_SQNC_CNT;
			iSqncNum = SQNC_BASE;
		}

		switch (iSqncNum)
		{
		case SQNC_BASE:
			if (bFnRes == FALSE)//검사중지
			{
				TECmsgOut(L"Stop Pattern Setting", LIGHTMAGENTA, 1500);
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//일시정지
				break;

			if (iSqncCur == MAX_SQNC_CNT)//시퀀스 정상종료
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//증가하고 대입
			break;


			//생성 : 이름앞에 Taget_@
			//불러오기 : Taget_ 아니면 안됨.@
			//웨이퍼 정보입력창 활성
			//패턴등록 버튼활성
		case SQNC_MODEL_SELECT:
			if (m_bThrdFlag[TF_CASSETTE_START])
			{
				if (!m_bSqncInterrupt[SQITRT_SELECT_MODEL])// 모델선택
					break;
			}
			else
			{
				pView->m_pModel->m_fnModule_ModelLoad(m_strTargetName);
			}
			m_bSqncInterrupt[SQITRT_SELECT_MODEL] = FALSE;

			EnableDlgItem(pView->m_pModel, FALSE, ArrExcepID_5, 17);
// 			pView->m_pModel->m_fnCheckAlignUse(TRUE);
// 			pView->m_pModel->m_ctrlCheckSetSubPoint.SetCheck(FALSE);
			pView->m_pModel->m_fnSetRadioModelType(MODEL_TYPE_TARGET);

			pView->m_pModel->GetDlgItem(IDC_FPSPREAD_SR_INFO)->EnableWindow(TRUE);//이방식아니면 안됨....
// 			pView->m_pModel->m_clSR.EnableWindow(TRUE);

			pView->m_pModel->m_ctrlListMeasureResult.AddString(_T("STEP 8. 측정Target패턴 좌표(열/행)를 등록하세요."));
			pView->m_pModel->m_ctrlListMeasureResult.AddString(_T("           * '측정위치추가' 후에, 열/행 값을 입력하세요."));
			pView->m_pModel->m_ctrlListMeasureResult.AddString(_T("           * 등록 완료 후, '저장'해주세요."));

			iSqncNum = SQNC_BASE;
			break;


			//XXX-sheet 활성
			//XXX-6행부터 사용자 등록가능
			//XXX-전체 열행 등록(예상위치 열행 모두등록)
			//완료 - SubPoint 생성확인
		case SQNC_POSITION_REG:
			if (m_bThrdFlag[TF_CASSETTE_START])
			{
				if (!m_bSqncInterrupt[SQITRT_REG_POSITION])
					break;
			}
			m_bSqncInterrupt[SQITRT_REG_POSITION] = FALSE;

// 			pView->m_pModel->m_fnGetCurrModel_Load(pWorkRun->m_strSubName);
// 			memcpy(&pView->m_pAuto->m_pTask->stSub, &pView->m_pAuto->m_pTask->stCurrModel, sizeof(t_MODEL_INFO));
// 			pView->m_pModel->m_fnGetCurrModel_Load(pWorkRun->m_strTargetName);
// 			memcpy(&pView->m_pAuto->m_pTask->stTarget, &pView->m_pAuto->m_pTask->stCurrModel, sizeof(t_MODEL_INFO));
// 			memcpy(&pView->m_pAuto->m_pTask->stModel, &pView->m_pAuto->m_pTask->stCurrModel, sizeof(t_MODEL_INFO));
// 
// 			pView->m_pAuto->m_fnSetTargetSubCalcPoint();

			pView->m_pModel->m_fnGetCurrModel_Load(pWorkRun->m_strSubName);
			memcpy(&pView->m_pAuto->m_pTask->stSub, &pView->m_pAuto->m_pTask->stCurrModel, sizeof(t_MODEL_INFO));
			pView->m_pModel->m_fnGetCurrModel_Load(pWorkRun->m_strTargetName);
			memcpy(&pView->m_pAuto->m_pTask->stTarget, &pView->m_pAuto->m_pTask->stCurrModel, sizeof(t_MODEL_INFO));

			pView->m_pAuto->m_fnSetTargetSubCalcPoint();

			pView->m_pAuto->InsertSheet(pView->m_pAuto->m_pTask->stResult.nSRData);

			pView->m_pAuto->m_clModelList2.SetWindowTextA(pWorkRun->m_strSubName);
			nNo = pView->m_pAuto->m_clModelList.FindString(0, pWorkRun->m_strTargetName);
			if (nNo != -1)
				pView->m_pAuto->m_clModelList.SetCurSel(nNo);

			pView->m_fnSetSwitch(SW_AUTO);

			iSqncNum = SQNC_BASE;
			break;
			
		case MAX_SQNC_CNT:
			bSqncFlag = FALSE;
			break;
		}
	}

	if (!bFnRes)
	{
#if _DEBUG
		AfxMessageBox(tmpMsg, MB_ICONHAND);
#endif
	}
	else
	{
		TECmsgOut(L"Target포인트 등록 완료", LIGHTBLUE, 1000);
	}

	return bFnRes;
}

// BOOL CWorkRun::SqncAlign_Proto()
// {
// 	CTimeInterval CheckTime;
// 	BOOL bSqncFlag = TRUE;
// 	BOOL bFnRes = TRUE;
// 	char tmpMsg[MAX_Buf] = { "Error_SqncPattReg" };
// 
// 	CDPoint dPntTargetPos;
// 	double dCurPosZ = 0.0;
// 
// 	int iAlignPattNo = 0;
// 	CString strVal;
// 
// 	enum//실행 순서 바꾸려면 아래 순서를 바꿀것
// 	{
// 		SQNC_BASE = 0,
// 		SQNC_MOVE_FIRST_POS,
// 		SQNC_MODEL_DLG,
// 		SQNC_MODEL_SELECT,
// 
// 		SQNC_PATTERN_REG_DLG,
// 		SQNC_PATT_FIND,
// 
// 		SQNC_PATT_INFO,
// 		SQNC_POSITION_REG,
// 
// 		SQNC_SUB_MODEL_SELECT,
// 		SQNC_SUB_POSITION_REG,
// 
// 		SQNC_MOVE_ALIGN_POS,
// 		SQNC_PATT_FIND_ALIGN_POS,
// 		SQNC_PATT_CONFIRM_ALIGN_POS,
// 		//성공 - 해당좌표 기준점등록
// 		//실패 - 조그운전 패턴찾고 기준위치 수동등록
// 
// 		SQNC_MAPPING,
// 		//자동검사창으로 이동
// 		SQNC_CALC_POS,
// 		//Target + Sub 좌표로
// 		//계산후 자동검사창 sheet에 뿌려줌
// 
// 		MAX_SQNC_CNT
// 	};
// 	int iSqncNum = SQNC_BASE;
// 	int iSqncCur = SQNC_BASE;
// 
// 	int ArrExcepID_1[9] = {
// 		IDC_LST_MODEL,
// 		IDC_BTN_ADD,
// 		IDC_BTN_RENAME,
// 		IDC_BTN_COPY,
// 		IDC_BTN_SAVE,
// 		IDC_RADIO_MODEL_TYPE1,
// 		IDC_RADIO_MODEL_TYPE2,
// 		IDC_RADIO_MODEL_TYPE3,
// 		IDC_RADIO_MODEL_TYPE4
// 	};
// 	int ArrExcepID_2[4] = {
// 		IDC_CBO_INCH,
// 		IDC_EDT_THICKNESS,
// 		IDC_EDT_EXCLUSION,
// 		IDC_BTN_SET_PATT
// 	};
// 	int ArrExcepID_3[5] = {
// 		IDC_EDT_PATT_SIZE_X,
// 		IDC_EDT_PATT_SIZE_Y,
// 		IDC_EDT_PATT_GAB_X,
// 		IDC_EDT_PATT_GAB_Y,
// 		IDC_BTN_ALIGN_POS_AUTO_SET
// 	};
// 	int ArrExcepID_4[3] = {
// 		IDC_FPSPREAD_SR_INFO,
// 		IDC_BTN_SAVE,
// 		IDC_BTN_INPUT_ROWCOL_AUTO_CALC
// 	};
// 
// 	while (bSqncFlag)
// 	{
// 		Sleep(100);
// 		if (!pWorkRun->m_bThrdFlag[TF_AUTORUN])
// 		{
// 			iSqncCur = MAX_SQNC_CNT;
// 			iSqncNum = SQNC_BASE;
// 		}
// 
// 		switch (iSqncNum)
// 		{
// 		case SQNC_BASE:
// 			if (bFnRes == FALSE)//검사중지
// 			{
// 				TECmsgOut(L"Stop Pattern Setting", LIGHTMAGENTA, 1500);
// 				bSqncFlag = FALSE;
// 				break;
// 			}
// 			if (pView->m_pAuto->m_bPause)//일시정지
// 				break;
// 
// 			if (iSqncCur == MAX_SQNC_CNT)//시퀀스 정상종료
// 			{
// 				bSqncFlag = FALSE;
// 				break;
// 			}
// 
// 			if (pAxis[X_AXIS]->CheckInMotion()
// 				|| pAxis[Y_AXIS]->CheckInMotion()
// 				|| pAxis[Z_AXIS]->CheckInMotion())
// 				break;
// 
// 			iSqncNum = ++iSqncCur;//증가하고 대입
// 			break;
// 
// 
// 			//현재 모델의 기준 좌표 - 없으면 원점@
// 			//모델창 뷰 - 모델관련버튼 활성
// 			//얼라인체크
// 		case SQNC_MOVE_FIRST_POS:
// 			dPntTargetPos.x = pView->m_pModel->m_pTask->stModel.dPattPosX;
// 			dPntTargetPos.y = pView->m_pModel->m_pTask->stModel.dPattPosY;
// 			dPntTargetPos.x = dPntTargetPos.x < -200 ? -200.0 : dPntTargetPos.x > 200 ? 200 : dPntTargetPos.x;
// 			dPntTargetPos.y = dPntTargetPos.y < -200 ? -200.0 : dPntTargetPos.y > 200 ? 200 : dPntTargetPos.y;
// 			pAxis[X_AXIS]->fVelocity = (float)pView->m_pModel->m_pTask->stCommon.dInspXYSpeed;
// 			pAxis[Y_AXIS]->fVelocity = (float)pView->m_pModel->m_pTask->stCommon.dInspXYSpeed;
// 
// 			bFnRes = SqncTargetMove(dPntTargetPos);
// 
// 			pView->m_fnSetSwitch(SW_MODEL);
// 			pView->m_fnSetChangeBtn(SW_MODEL);
// 
// 			EnableDlgItem(pView->m_pModel, FALSE, ArrExcepID_1, 9);
// 			m_bSqncInterrupt[SQITRT_SELECT_MODEL] = FALSE;
// 
// 			iSqncNum = SQNC_BASE;
// 			break;
// 
// 
// 			//카메라 ON
// 			//Auto포커싱
// 		case SQNC_MODEL_DLG:
// 			pView->m_pStageMap->m_fnCheckCamera(TRUE);
// 			ThrdAF();
// 
// 			iSqncNum = SQNC_BASE;
// 			break;
// 
// 
// 			//생성 : 이름앞에 Taget_@
// 			//불러오기 : Taget_ 아니면 안됨.@
// 			//웨이퍼 정보입력창 활성
// 			//패턴등록 버튼활성
// 		case SQNC_MODEL_SELECT:
// 			if (!m_bSqncInterrupt[SQITRT_SELECT_MODEL])// 모델선택
// 				break;
// 			m_bSqncInterrupt[SQITRT_SELECT_MODEL] = FALSE;
// 
// 			EnableDlgItem(pView->m_pModel, FALSE, ArrExcepID_2, 4);
// 			m_bSqncInterrupt[SQITRT_REG_PATTERN] = FALSE;
// // 			pView->m_pModel->m_fnCheckAlignUse(TRUE);
// 			pView->m_pModel->m_fnSetRadioModelType(MODEL_TYPE_TARGET);
// 
// 			iSqncNum = SQNC_BASE;
// 			break;
// 
// 
// 			//패턴저장 경로 모델명 폴더 추가@
// 			//패스버튼 생성 - 기존패턴정보 로딩
// 			//닫을 때 패턴파일 유무확인 - 없으면 생성 및 저장 요청  - utilclass - Existence(풀경로)
// 			//닫히면 패턴찾기 버튼활성
// 		case SQNC_PATTERN_REG_DLG:
// 			if (!m_bSqncInterrupt[SQITRT_REG_PATTERN])
// 				break;
// 			m_bSqncInterrupt[SQITRT_REG_PATTERN] = FALSE;
// 
// 			pView->m_pModel->GetDlgItem(IDC_BTN_FIND_PATT)->EnableWindow(TRUE);//프리징 위험
// 
// 			m_bSqncInterrupt[SQITRT_FIND_PATTERN] = FALSE;
// 			iSqncNum = SQNC_BASE;
// 			break;
// 
// 
// 			//화면상에서 패턴 찾으면 현재위치로 
// 			//sheet 1행에 0행/0열 기준좌표 등록 - 측정위치추가 버튼 코드 참고
// 			//패턴간격정보 활성
// 		case SQNC_PATT_FIND:
// 			if (!m_bSqncInterrupt[SQITRT_FIND_PATTERN])
// 				break;
// 			m_bSqncInterrupt[SQITRT_FIND_PATTERN] = FALSE;
// 			pView->m_pModel->m_fnPosInfoInputSheet(1, 0, 0);
// 			EnableDlgItem(pView->m_pModel, FALSE, ArrExcepID_3, 5);
// 			m_bSqncInterrupt[SQITRT_AUTO_CAL_PATTERN_INFO] = FALSE;
// 			iSqncNum = SQNC_BASE;
// 			break;
// 
// 
// 			//자동등록계산버튼 내 기능--------------
// 			//패턴크기/간격 정보입력후 등록클릭
// 			//예상 패턴위치 MapView에 표시(남색) (패턴각도고려-보류)
// 			//스테이지맵뷰draw에 기록
// 			//지정위치 자동 선정(오랜지색) - 현재위치 기준 十방향으로 4포인트 - 외각에서 한칸 안
// 			//sheet 2~5행에 좌표 등록
// 		case SQNC_PATT_INFO:
// 			if (!m_bSqncInterrupt[SQITRT_AUTO_CAL_PATTERN_INFO])
// 				break;
// 			m_bSqncInterrupt[SQITRT_AUTO_CAL_PATTERN_INFO] = FALSE;
// 			m_bSqncInterrupt[SQITRT_REG_TAGET_POSITION] = FALSE;//앞선버튼클릭으로 인한 변수 초기화
// 
// 			EnableDlgItem(pView->m_pModel, FALSE, ArrExcepID_4, 5);
// 			iSqncNum = SQNC_BASE;
// 			break;
// 
// 
// 			//XXX-sheet 활성
// 			//XXX-6행부터 사용자 등록가능
// 			//XXX-전체 열행 등록(예상위치 열행 모두등록)
// 			//완료 - SubPoint 생성확인
// 		case SQNC_POSITION_REG:
// 			if (!m_bSqncInterrupt[SQITRT_REG_TAGET_POSITION])
// 				break;
// 			m_bSqncInterrupt[SQITRT_REG_TAGET_POSITION] = FALSE;
// 
// 			if (!TECmsgOut(L"SubPoint 등록을 하시겠습니까?", LIGHTMAGENTA, 2))
// 			{
// 				iSqncNum = SQNC_MOVE_ALIGN_POS;//갈 방향지정
// 				break;
// 			}
// 
// 			iSqncNum = SQNC_BASE;
// 			break;
// 
// 
// 			//Check Sub 자동//Align버튼 비활성
// 			//생성 : 이름앞에 Sub_
// 			//불러오기 : Sub_ 아니면 안됨.
// 		case SQNC_SUB_MODEL_SELECT:
// // 			pView->m_pModel->m_ctrlCheckSetSubPoint.SetCheck(TRUE);
// 			pView->m_pModel->m_fnSetRadioModelType(MODEL_TYPE_SUB);
// 
// 			EnableDlgItem(pView->m_pModel, FALSE, ArrExcepID_1, 9);
// 			m_bSqncInterrupt[SQITRT_SELECT_MODEL] = FALSE;
// 
// 			iSqncNum = SQNC_BASE;
// 			break;
// 
// 
// 			//등록시 기준좌표를 뺀 상대좌표로 입력
// 			//완료
// 		case SQNC_SUB_POSITION_REG:
// 			if (!m_bSqncInterrupt[SQITRT_SELECT_MODEL])// 모델선택
// 				break;
// 			m_bSqncInterrupt[SQITRT_SELECT_MODEL] = FALSE;
// 
// 			pView->m_pModel->m_clSR.EnableWindow(TRUE);
// 
// 			break;
// 
// 
// 		case SQNC_MOVE_ALIGN_POS:
// 			if (!pView->m_pModel->m_bSetInsertSubPosFinish)// 모델저장
// 				break;
// 			pView->m_pModel->m_bSetInsertSubPosFinish = FALSE;
// 
// 			bFnRes = pAxis[X_AXIS]->MoveAxis(pView->m_pStageMap->m_dPntAlign[iAlignPattNo].x, 0, TRUE);
// 			if (bFnRes)
// 				bFnRes = pAxis[Y_AXIS]->MoveAxis(pView->m_pStageMap->m_dPntAlign[iAlignPattNo].y, 0, TRUE);
// 
// 			iSqncNum = SQNC_BASE;
// 			break;
// 
// 
// 		case SQNC_PATT_FIND_ALIGN_POS:
// 			pView->m_pStageMap->m_fnCheckCamera(TRUE);
// 			ThrdAF();
// 			m_bSqncInterrupt[SQITRT_FIND_PATTERN] = FALSE;
// 
// 			pTask->PatternNum = 0;
// 			// 			memcpy(pTask->ImageBuff[pTask->PatternNum], pCamJAI->m_CnvImageInfo.pImageBuffer, sizeof(unsigned char)*DEF_IMAGE_X*DEF_IMAGE_Y);
// 			pMILAlignDlg->m_fnTranferCamBuff2PattBuff(pView->m_pAuto->m_bPattAlignMode);
// 
// 			pTask->AlignCenter = GetPattAlignCenter(pTask->ImageBuff[pTask->PatternNum], pTask->PatternNum);
// 			pTask->ErrorCode = pInsp[DEF_DISP_SET]->ErrorCode;
// 			iSqncNum = SQNC_BASE;
// 			break;
// 
// 
// 			//실패 - 조그운전 패턴찾고 기준위치 수동등록
// 			//성공 - 해당좌표 기준점등록
// 		case SQNC_PATT_CONFIRM_ALIGN_POS:
// 			if (pTask->ErrorCode)//실패
// 			{
// 				if (!m_bSqncInterrupt[SQITRT_FIND_PATTERN])
// 					break;
// 				else
// 				{
// 					iSqncNum = SQNC_MOVE_ALIGN_POS;
// 					break;
// 				}
// 			}
// 
// 			pTask->XWorldArray[iAlignPattNo] = pTask->AlignCenter.x;
// 			pTask->YWorldArray[iAlignPattNo] = pTask->AlignCenter.y;
// 
// 			iAlignPattNo++;
// 			if (iAlignPattNo < 4)
// 			{
// 				iSqncNum = SQNC_MOVE_ALIGN_POS;
// 				break;
// 			}
// 			else
// 			{
// 				iSqncNum = SQNC_BASE;
// 				break;
// 			}
// 
// 			iSqncNum = SQNC_BASE;
// 			break;
// 
// 
// 			//자동검사창으로 이동
// 		case SQNC_MAPPING:
// 			pTask->NumPoint = 4;
// 			for (int i = 0; i < pTask->NumPoint; i++)
// 			{
// 				pView->m_pModel->m_clSR.SetRow(i + 2);
// 
// 				pView->m_pModel->m_clSR.SetCol(MEASURE_COL);
// 				strVal = pView->m_pModel->m_clSR.GetText();
// 				pTask->XPixArray[i] = atof(strVal);
// 
// 				pView->m_pModel->m_clSR.SetCol(MEASURE_ROW);
// 				strVal = pView->m_pModel->m_clSR.GetText();
// 				pTask->YPixArray[i] = atof(strVal);
// 			}
// 			pTask->ErrorCode = pInsp[DEF_DISP_SET]->ErrorCode = 0;
// 			pTask->iGroupNum = 0;
// 
// 			SetMotorAlign(pTask->iGroupNum, pTask->XPixArray, pTask->YPixArray, pTask->XWorldArray, pTask->YWorldArray, pTask->ImageBuff, pTask->NumPoint);
// 			memcpy(pTask->msgVision, pInsp[DEF_DISP_SET]->msgVision, sizeof(pTask->msgVision));
// 			pTask->ErrorCode = pInsp[DEF_DISP_SET]->ErrorCode;
// 			if (pTask->ErrorCode)
// 			{
// 				if (TECmsgOut(L"Mapping과정을 완료하지 못했습니다. 기준 Align Mark 등록을 다시 하시겠습니까?", LIGHTMAGENTA, 2))
// 				{
// 					iSqncNum = SQNC_MOVE_ALIGN_POS;
// 					break;
// 				}
// 				else
// 				{
// 					//종료시퀀스?
// 				}
// 			}
// 			else
// 			{
// 				TECmsgOut(L"Align Mapping Complete", LIGHTGREEN, 1000);
// 			}
// 
// 			pView->m_fnSetSwitch(SW_AUTO);
// 			pView->m_fnSetChangeBtn(SW_AUTO);
// 
// 			iSqncNum = SQNC_BASE;
// 			break;
// 
// 
// 			//선택된 Target모델 네임 - Data구조체
// 			//선택된 Sub모델 네임 - Data구조체
// 			//Target + Sub 좌표로
// 			//계산후 자동검사창 sheet에 뿌려줌
// 		case SQNC_CALC_POS:
// 			EnableDlgItem(pView->m_pModel, TRUE);//모든버튼 활성
// 
// 			iSqncNum = SQNC_BASE;
// 			break;
// 
// 
// 		case MAX_SQNC_CNT:
// 			bSqncFlag = FALSE;
// 			break;
// 		}
// 	}
// 
// 	if (!bFnRes)
// 	{
// #if _DEBUG
// 		AfxMessageBox(tmpMsg, MB_ICONHAND);
// #endif
// 	}
// 	else
// 	{
// 		TECmsgOut(L"패턴세팅 완료", LIGHTBLUE, 1000);
// 	}
// 
// 	return bFnRes;
// }


#endif