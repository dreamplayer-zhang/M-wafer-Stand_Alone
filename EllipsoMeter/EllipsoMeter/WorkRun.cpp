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
		TECmsgOut(L"Vision AutoFocusing �۾� ���Դϴ�...", YELLOW, 1000);
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
	TECmsgOut(L"Manual ������ �����մϴ�.", LIGHTGREEN, 500);

	if (!pWorkRun->SqncManualMeasure())
	{
		TECmsgOut(L"Manual ������ �Ϸ����� ���߽��ϴ�.", LIGHTMAGENTA, 1);
	}
	else
		TECmsgOut(L"Manual ������ �Ϸ��߽��ϴ�.", LIGHTBLUE, 1000);

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
		TECmsgOut(L"�����Ҹ�Ÿ ������¸� Ȯ�����ּ���.", LIGHTMAGENTA, 1);
		pWorkRun->SqncCloseMeasure();
		pWorkRun->m_bThrdFlag[TF_AUTORUN] = FALSE;
		return;
	}

	t_ELIPSO_RESULT_INFO stcResultAlign;

	if (!pWorkRun->SqncCorrectAlign(stcResultAlign))
	{
		TECmsgOut(L"""��ġ����""�� �Ϸ����� ���߽��ϴ�.", LIGHTMAGENTA, 1);
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
		TECmsgOut(L"�����Ҹ�Ÿ ������¸� Ȯ�����ּ���.", LIGHTMAGENTA, 1);
		pWorkRun->SqncCloseMeasure();
		pWorkRun->m_bThrdFlag[TF_AUTORUN] = FALSE;
		return;
	}

	if (!pWorkRun->SqncCorrectAlign(stcResultAlign))
	{
		TECmsgOut(L"""��ġ����""�� �Ϸ����� ���߽��ϴ�.", LIGHTMAGENTA, 1);
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
		TECmsgOut(L"""����""�� �Ϸ����� ���߽��ϴ�.", LIGHTMAGENTA, 1);
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
		// ��� ������ ���
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

	//����Ʈ ��� ��ǥ ���� Ȯ��
	int iListCnt = pView->m_pTask->stCurrModel.nSRData;
	int iCurCnt = 0;

	enum//���� ���� �ٲٷ��� �Ʒ� ������ �ٲܰ�
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
			if (bFnRes == FALSE)//�˻�����
			{
#if _DEBUG
				AfxMessageBox(tmpMsg, MB_ICONHAND);
#endif

				if (TECmsgOut(L"���� ���� �۾��� ���� �Ͻðڽ��ϱ�?", LIGHTMAGENTA, 2))
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
			if (pView->m_pAuto->m_bPause)//�Ͻ�����
				break;

			if (pWorkRun->m_bJustNowFinish)
			{
				if (iSqncCur >= SQNC_INSERT_START_SIG)
				{
					iSqncCur = SQNC_BASE;
					iSqncNum = SQNC_BASE;
				}
			}

			if (iSqncCur == MAX_SQNC_CNT)//������ ��������
			{
				pWorkRun->m_bThrdFlag[TF_AUTORUN] = bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//�����ϰ� ����
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
					if (TECmsgOut(L"������ ������� �Ϸ����� ���߽��ϴ�. �׷��� ����Ͻðڽ��ϱ�?", LIGHTMAGENTA, 2))
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
					TECmsgOut(L"���� ������� �Ϸ����� ���߽��ϴ�. �˻縦 �����մϴ�.", LIGHTRED, 1);
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
		TECmsgOut(L"���� �۾� �Ϸ�", LIGHTBLUE, 1);
	}

	EnableDlgItem(pView->m_pModel, TRUE);//����ư Ȱ��
// 	EnableDlgItem(pView->m_pAuto, TRUE);//����ư Ȱ��
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

	enum//���� ���� �ٲٷ��� �Ʒ� ������ �ٲܰ�
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
			if (bFnRes == FALSE)//�˻�����
			{
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//�Ͻ�����
				break;

			if (iSqncCur == MAX_SQNC_CNT)//������ ��������
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//�����ϰ� ����
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
				iSqncCur = MAX_SQNC_CNT-1;//���������
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

	if (m_bAutoMode)//�ڵ�
	{
		tmpSupplyPos.x = pView->m_pTask->stCommon.dLoadingX;
		tmpSupplyPos.y = pView->m_pTask->stCommon.dLoadingY;
	}
	else//����
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


	enum//���� ���� �ٲٷ��� �Ʒ� ������ �ٲܰ�
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
			if (bFnRes == FALSE)//�˻�����
			{
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//�Ͻ�����
				break;

			if (iSqncCur == MAX_SQNC_CNT)//������ ��������
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//�����ϰ� ����
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
		TECmsgOut(L"�������� ������ġ �̵� �Ϸ� ", YELLOW, 200, 17, BLACK);

	return bFnRes;
}

BOOL CWorkRun::SqncStandbySupply()
{
	CTimeInterval CheckTime;
	BOOL bSqncFlag = TRUE;
	BOOL bFnRes = TRUE;
	char tmpMsg[MAX_Buf] = { "Error_SqncStandbySupply" };

	double dWaferLineMotorPos[2];

	enum//���� ���� �ٲٷ��� �Ʒ� ������ �ٲܰ�
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
			if (bFnRes == FALSE)//�˻�����
			{
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//�Ͻ�����
				break;

			if (iSqncCur == MAX_SQNC_CNT)//������ ��������
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//�����ϰ� ����
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
				if(!TECmsgOut(L"Stage Vacuum ���� ����-TimeOut. �����ϰ� ��� �����Ͻðڽ��ϱ�?", LIGHTRED, 2))
				{
					TECmsgOut(L"Stage Vacuum ���¸� Ȯ���ϰ� ����� �ּ���.", LIGHTMAGENTA, 1);
					CheckTime.SetTimeStart();
					break;
				}
// 				bFnRes = FALSE;
// 				iSqncCur = MAX_SQNC_CNT;//������ ����
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
			if (CheckTime.GetTimeInterval() < 500)//500ms�̻� ���(�ڵ彦��ũ)
				break;
			//IN4 OFF ���
			if (pView->m_pIO->m_pTask->stElipsoIO.stIn.EFEM_Finish)
				break;

			if (m_bAutoMode)//�ڵ�
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
					TECmsgOut(L"Stage�� ������ ��ġ�� �̵����� ���߽��ϴ�. X,Y Stage ���¸� Ȯ���ϰ� �ٽ� ������ �ּ���.", LIGHTMAGENTA, 1);
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
		TECmsgOut(L"�������� ���� �غ�(����/��ȣ) �Ϸ� ", YELLOW, 200, 17, BLACK);

	return bFnRes;
}

BOOL CWorkRun::SqncInputStartSignal()
{
	CTimeInterval CheckTime;
	BOOL bSqncFlag = TRUE;
	BOOL bFnRes = TRUE;
	char tmpMsg[MAX_Buf] = { "Error_SqncInputStartSignal" };
	CString strSlotNum;

	enum//���� ���� �ٲٷ��� �Ʒ� ������ �ٲܰ�
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
			if (bFnRes == FALSE )//�˻�����
			{
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//�Ͻ�����
				break;

			if (iSqncCur == MAX_SQNC_CNT)//������ ��������
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//�����ϰ� ����
			break;

		case SQNC_CHECK_INPUT:
			if (pWorkRun->m_bJustNowFinish)
			{
				pWorkRun->m_bJustNowFinish = FALSE;
			}

			if (m_bAutoMode)//�ڵ�
			{
				if (!pView->m_pIO->m_pTask->stElipsoIO.stIn.EFEM_Finish
					|| pView->m_pIO->m_pTask->stElipsoIO.stIn.EFEM_Run)
					break;
			}
			else//����
			{
// 				TECmsgOut(L"�����۸� ���������� ���� �ø� �� ""Ȯ��""�� ������ �����մϴ�. ", LIGHTMAGENTA, 1);
				if (!TECmsgOut(L"�����۸� ���������� ���� �ø� �� [Yes]�� ������ �����մϴ�. [No]�� ������ �����մϴ�", LIGHTMAGENTA, 2))
					bFnRes = FALSE;
			}

			iSqncNum = SQNC_BASE;
			break;

		case SQNC_CHECK_SOCKET:
			if (m_bAutoMode)//�ڵ�
			{
// 				pView->m_pAuto->m_ctrlEditWaferNum.GetWindowTextA(strSlotNum);
// 				pView->m_pAuto->m_stUser.iSlotNo = atoi(strSlotNum.GetString());
				//������� Recv Ȯ��
				//-ī��Ʈ ����/��
				//-������ ��ȣ
				//x�κ�����ʿ� �����Ϸ�
			}
			else//����
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
		TECmsgOut(L"�۾����� ��ȣ Ȯ�� �Ϸ� ", YELLOW, 200, 17, BLACK);

	return bFnRes;
}

BOOL CWorkRun::SqncDetectWafer()
{
	CTimeInterval CheckTime;
	CDPoint tmpSupplyPos;
	BOOL bSqncFlag = TRUE;
	BOOL bFnRes = TRUE;
	char tmpMsg[MAX_Buf] = { "Error_SqncDetectWafer" };

	enum//���� ���� �ٲٷ��� �Ʒ� ������ �ٲܰ�
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
			if (bFnRes == FALSE)//�˻�����
			{
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//�Ͻ�����
				break;

			if (pWorkRun->m_bJustNowFinish)
			{
				bSqncFlag = FALSE;
				break;
			}

			if (iSqncCur == MAX_SQNC_CNT)//������ ��������
			{
// 				ThrdAF();
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//�����ϰ� ����
			break;

		case SQNC_DETECT_WAFER_SENSOR:
			if (pView->m_pIO->m_pTask->stElipsoIO.stIn.Wafer_Detect_Sensor)
			{
				//OUT6 ON
				pView->m_pIO->m_fnSetOutputSignal(MEASURE_READY, FALSE);
				//OUT7 OFF
				pView->m_pIO->m_fnSetOutputSignal(MEASURE_BUSY, FALSE);

				TECmsgOut(L"�������� ���� �����۸� ����� �ø��� [Yes]�� �����ּ���. ", LIGHTMAGENTA, 1);
				break;
			}
			//OUT6 OFF
			pView->m_pIO->m_fnSetOutputSignal(MEASURE_READY, FALSE);
			//OUT7 ON
			pView->m_pIO->m_fnSetOutputSignal(MEASURE_BUSY, TRUE);

			if (pView->m_pMap->m_nWaperSize == 0)
			{
				iSqncCur = MAX_SQNC_CNT;//������ ����
			}
			else if (pView->m_pMap->m_nWaperSize > WAPER_12)
			{
				bFnRes = FALSE;
				AfxMessageBox("Wafer Size ���� ����.");
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
			if (CheckTime.GetTimeInterval() < 500)//500ms�̻� ���
				break;

			if (CheckTime.GetTimeInterval() > 8000)//TimeOut
			{
// 				AfxMessageBox("Stage Vacuum ���� ����-TimeOut");
				if (!TECmsgOut(L"Stage Vacuum ���� ����-TimeOut 2. �����ϰ� ��� �����Ͻðڽ��ϱ�?", LIGHTRED, 2))
				{
					TECmsgOut(L"Stage Vacuum ���¸� Ȯ���ϰ� ����� �ּ���.", LIGHTMAGENTA, 1);
					CheckTime.SetTimeStart();
					break;
// 					bFnRes = FALSE;
// 					iSqncCur = MAX_SQNC_CNT;//������ ����
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

			//���߽� �̵�.
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
		TECmsgOut(L"������ ���� �ν� Ȯ��.", LIGHTBLUE, 2000);

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

	enum//���� ���� �ٲٷ��� �Ʒ� ������ �ٲܰ�
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
			if (bFnRes == FALSE)//�˻�����
			{
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//�Ͻ�����
				break;

			if (pWorkRun->m_bJustNowFinish)
			{
				bSqncFlag = FALSE;
				break;
			}

			if (iSqncCur == MAX_SQNC_CNT)//������ ��������
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//�����ϰ� ����
			break;

			//AF
		case SQNC_AF:
			ThrdAF(pView->m_pAuto->m_ctrlChkUseAF.GetCheck());
			iSqncNum = SQNC_BASE;
			break;

			//11�� �����˻�
		case SQNC_EDGE_1:
			bFnRes = SqncSearchEdge(0);
			iSqncNum = SQNC_BASE;
			break;

		// 9�� �����˻�
		case SQNC_EDGE_2:
			bFnRes = SqncSearchEdge(1);
			iSqncNum = SQNC_BASE;
			break;

		// 7�� �����˻�
		case SQNC_EDGE_3:
			bFnRes = SqncSearchEdge(2);
			iSqncNum = SQNC_BASE;
			break;

		//3�� ���߽� ���
		case SQNC_3P_CALC_CIRCLE_CENTER:
			bFnRes = pMakeObj->CalculateCircle3(m_dPntForCircle, dCalcCenter, &dCalcRadius);
			iSqncNum = SQNC_BASE;
			break;

		//���߽� �̵�.
		case SQNC_MOVE_CIRCLE_CENTER:
			pAxis[X_AXIS]->m_dOffsetPos = dCalcCenter[0];
			pAxis[Y_AXIS]->m_dOffsetPos = dCalcCenter[1];

			bFnRes = pAxis[X_AXIS]->MoveAxis(0, 0, TRUE);
			if (bFnRes)
				bFnRes = pAxis[Y_AXIS]->MoveAxis(0, 0, TRUE);
			iSqncNum = SQNC_BASE;
			break;

		//x-y�� ������ǥ 0��set
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
		TECmsgOut(L"������ ����� �Ϸ�", YELLOW, 200, 17, BLACK);

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
	//11��
	dPntLimit[0].x = (-1.0)*dMM*cos(PI/6);	//pView->m_pTask->stCommon.dXMLimit;
	dPntLimit[0].y = (-1.0)*dMM*sin(PI/6);	//pView->m_pTask->stCommon.dYMLimit;
	//6��
	dPntLimit[1].x = (1.0)*dMM*cos(PI/2);		//0.0;
	dPntLimit[1].y = (1.0)*dMM*sin(PI/2);		//pView->m_pTask->stCommon.dYPLimit;
	//2��
	dPntLimit[2].x = (1.0)*dMM*cos(PI/6);		//pView->m_pTask->stCommon.dXPLimit;
	dPntLimit[2].y = (-1.0)*dMM*sin(PI/6);	//pView->m_pTask->stCommon.dYMLimit;


	pView->m_pModel->m_fnGetInchToMM(arrWaferInch[pView->m_pAuto->m_nWaperSize] - 1, dMM, 0.0);
	CDPoint dPntStart[3];
	//11��
	dPntStart[0].x = (-1.0)*dMM*cos(PI/6); //;
	dPntStart[0].y = (-1.0)*dMM*sin(PI/6);	//pView->m_pTask->stCommon.dYMLimit;
	//6��
	dPntStart[1].x = (1.0)*dMM*cos(PI/2);		//0.0;
	dPntStart[1].y = (1.0)*dMM*sin(PI/2);		//pView->m_pTask->stCommon.dYPLimit;
	//2��
	dPntStart[2].x = (1.0)*dMM*cos(PI/6);		//pView->m_pTask->stCommon.dXPLimit;
	dPntStart[2].y = (-1.0)*dMM*sin(PI/6);	//pView->m_pTask->stCommon.dYMLimit;
	

	enum//���� ���� �ٲٷ��� �Ʒ� ������ �ٲܰ�
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
			if (bFnRes == FALSE)//�˻�����
			{
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//�Ͻ�����
				break;

			if (iSqncCur == MAX_SQNC_CNT)//������ ��������
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

			iSqncNum = ++iSqncCur;//�����ϰ� ����
			break;

		//���߽� �̵�.
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

		//������ġ �̵�
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

		//ȹ���̹����� ������ ���� �����˻� 1/2�̻��̸� ����
		case SQNC_INDEX_BLACK:
			pMILAlignDlg->m_fnBringImage(pCamJAI->m_CnvImageInfo.pImageBuffer, pCamJAI->m_CnvImageInfo.iImageSize);

			bFindWaferLine = pMILAlignDlg->FindWaferLine();
			if (!bFindWaferLine)//�����˻� �Լ�
			{
				if (CheckTime.GetTimeInterval(2) > 60)
				{
					TECmsgOut(L"������ ���˻� TimeOut (60sec). �˻縦 �����մϴ�. ������ ���ð��� Ȯ�����ּ���. ", LIGHTMAGENTA, 1);
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

		//�밢�� �˻��� ���� ����ǥ ȹ��
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

				//�̹�����������
				m_dPntForCircle[iSearchDirection][0] -= (DEF_IMAGE_X >> 1);
				m_dPntForCircle[iSearchDirection][1] -= (DEF_IMAGE_Y >> 1);
				//��ǥ��������
				m_dPntForCircle[iSearchDirection][0] *= 0.001;
				m_dPntForCircle[iSearchDirection][1] *= 0.001;
				//������Ͱ����
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
		TECmsgOut(L"������ ����ν� �Ϸ� ", YELLOW, 200, 17, BLACK);
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

	enum//���� ���� �ٲٷ��� �Ʒ� ������ �ٲܰ�
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
			if (bFnRes == FALSE)//�˻�����
			{
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//�Ͻ�����
				break;

			if (pWorkRun->m_bJustNowFinish)
			{
				bSqncFlag = FALSE;
				break;
			}

			if (iSqncCur == MAX_SQNC_CNT)//������ ��������
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//�����ϰ� ����
			break;


			//���� ���� ���� ��ǥ - ������ ����@
			//�����üũ
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
			bFnRes = pWorkRun->SqncTargetReg();//�������� �ε��� �ʿ�

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
		TECmsgOut(L"���� ����� �Ϸ� ", YELLOW, 200, 17, BLACK);
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
		//���� Ŀ����ġRow Ȯ��
		iCurRow = pView->m_pTask->stResult.nSRData+1;// pView->m_pAuto->m_clResult.GetRow();
		pView->m_pAuto->m_clResult.SetRow(iCurRow);
		
		//RESULT_TARGET_NO SetText();
		pView->m_pAuto->m_clResult.SetCol(RESULT_TARGET_NO);
		strNAME.Format("Manual_%01d", iCurRow);
		pView->m_pAuto->m_clResult.SetText(strNAME);

		//RESULT_X1_X2 ���� ����X��ǥ SetText();
		pView->m_pAuto->m_clResult.SetCol(RESULT_X1_X2);
		pAxis[X_AXIS]->StatusGetPos(&dPntTargetPos.x);
		strX.Format("%.3f", dPntTargetPos.x);
		pView->m_pAuto->m_clResult.SetText(strX);

		//RESULT_Y1_Y2 ���� ����Y��ǥ SetText();
		pView->m_pAuto->m_clResult.SetCol(RESULT_Y1_Y2);
		pAxis[Y_AXIS]->StatusGetPos(&dPntTargetPos.y);
		strY.Format("%.3f", dPntTargetPos.y);
		pView->m_pAuto->m_clResult.SetText(strY);

		//double = ���� ����Z��ǥ
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

	//����Ʈ ��� ��ǥ ���� Ȯ��
	int iListCnt = pView->m_pTask->stResult.nSRData;
	int iCurCnt = 0;

	int arrWaferInch[4] = { 4,6,8,12 };
	double dMM = 0.0;

	enum//���� ���� �ٲٷ��� �Ʒ� ������ �ٲܰ�
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
			if (bFnRes == FALSE)//�˻�����
			{
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//�Ͻ�����
				break;

			if (iSqncCur == MAX_SQNC_CNT)//������ ��������
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//�����ϰ� ����
			break;

			//*����Ʈ ������� ��ġ�� �о����
			//*��ġ�̵� �ǽ�
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
				TECmsgOut(L"""��ġ����""�� �Ϸ����� ���߽��ϴ�.", LIGHTMAGENTA, 3000);
// 				iSqncCur = SQNC_LIST_REPEAT - 1;
// 				bFnRes = TRUE;
			}
			iSqncNum = SQNC_BASE;
			break;

			//*���� �ǽ�	
		case SQNC_MEASURE:
			bFnRes = SqncMeasure(stcResultAlign);
			if (!bFnRes)
			{
				TECmsgOut(L"""����""�� �Ϸ����� ���߽��ϴ�.", LIGHTMAGENTA, 3000);
// 				iSqncCur = SQNC_LIST_REPEAT - 1;
// 				bFnRes = TRUE;
			}
			iSqncNum = SQNC_BASE;
			break;

		case SQNC_CLOSE_MEASURE:
			bFnRes = pWorkRun->SqncCloseMeasure();
			iSqncNum = SQNC_BASE;
			break;

			//����Ʈ ������������� ǥ��
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
			// ��� ������ ���
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

			//*����Ʈ ������ŭ �ݺ�
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
		TECmsgOut(L"���� ��ġ ���� �Ϸ� ", YELLOW, 200, 17, BLACK);
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

	enum//���� ���� �ٲٷ��� �Ʒ� ������ �ٲܰ�
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
			if (bFnRes == FALSE)//�˻�����
			{
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//�Ͻ�����
				break;

			if (pWorkRun->m_bJustNowFinish)
			{
				bSqncFlag = FALSE;
				break;
			}

			if (iSqncCur == MAX_SQNC_CNT)//������ ��������
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//�����ϰ� ����
			break;

			//*����Ʈ ������� ��ġ�� �о����
			//*��ġ�̵� �ǽ�

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
					TECmsgOut(L"Recipe Load�� �Ϸ����� ���߽��ϴ�. ������¸� Ȯ���ϼ���.", LIGHTMAGENTA, 1);
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
		TECmsgOut(L"EllipsoMeter RecipeLoad�Ϸ� ", YELLOW, 200, 17, BLACK);

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

	enum//���� ���� �ٲٷ��� �Ʒ� ������ �ٲܰ�
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
			if (bFnRes == FALSE)//�˻�����
			{
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//�Ͻ�����
				break;

			if (pWorkRun->m_bJustNowFinish)
			{
				bSqncFlag = FALSE;
				break;
			}

			if (iSqncCur == MAX_SQNC_CNT)//������ ��������
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//�����ϰ� ����
			break;

			//*����Ʈ ������� ��ġ�� �о����
			//*��ġ�̵� �ǽ�

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
					TECmsgOut(L"�����Ҹ�Ÿ �����غ� �Ϸ����� ���߽��ϴ�. ������¸� Ȯ���ϼ���.", LIGHTMAGENTA, 1);
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
		TECmsgOut(L"EllipsoMeter �غ�Ϸ� ", YELLOW, 200, 17, BLACK);

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

	enum//���� ���� �ٲٷ��� �Ʒ� ������ �ٲܰ�
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
			if (bFnRes == FALSE)//�˻�����
			{
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//�Ͻ�����
				break;

			if (pWorkRun->m_bJustNowFinish)
			{
				bSqncFlag = FALSE;
				break;
			}

			if (iSqncCur == MAX_SQNC_CNT)//������ ��������
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//�����ϰ� ����
			break;

			//*����Ʈ ������� ��ġ�� �о����
			//*��ġ�̵� �ǽ�

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
// 					TECmsgOut(L"�����Ҹ�Ÿ ���͸� Close���� ���߽��ϴ�. ������¸� Ȯ���ϼ���.", LIGHTMAGENTA, 1);
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
		TECmsgOut(L"EllipsoMeter �غ�Ϸ� ", YELLOW, 200, 17, BLACK);

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

	//����Ʈ ��� ��ǥ ���� Ȯ��
	int iListCnt = pView->m_pTask->stResult.nSRData;
	int iCurCnt = 0;

	int arrWaferInch[4] = { 4,6,8,12 };
	double dMM = 0.0;

	enum//���� ���� �ٲٷ��� �Ʒ� ������ �ٲܰ�
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
			if (bFnRes == FALSE)//�˻�����
			{
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//�Ͻ�����
				break;

			if (pWorkRun->m_bJustNowFinish)
			{
				bSqncFlag = FALSE;
				break;
			}

			if (iSqncCur == MAX_SQNC_CNT)//������ ��������
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//�����ϰ� ����
			break;

			//*����Ʈ ������� ��ġ�� �о����
			//*��ġ�̵� �ǽ�
		case SQNC_MOVE_XY:
			CheckTime.SetTimeStart();
			CheckTactTime.SetTimeStart();
			dPntTargetPos.x = pQI->m_pTask->stData.m_dDataX[iCurCnt];
			dPntTargetPos.y = pQI->m_pTask->stData.m_dDataY[iCurCnt];

			// 			pQI->m_pTask->stResult.nWaperSize

			//�̵�����Ʈ�� ������ �ۿ� ������...
			pView->m_pModel->m_fnGetInchToMM(arrWaferInch[pView->m_pAuto->m_nWaperSize], dMM, pView->m_pAuto->m_dWaperExclusion);
			if (!inCircle(dPntTargetPos.x, dPntTargetPos.y, 0.0, 0.0, dMM))
			{
				if (TECmsgOut(L"������ġ�� �����۹����� ������ϴ�. �ش���ġ ������ �ǳʶٰڽ��ϱ�?", LIGHTMAGENTA, 2))
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
				TECmsgOut(L"������ġ""�̵�""�� �Ϸ����� ���߽��ϴ�. 3�� �� ������ġ�� �����մϴ�", LIGHTMAGENTA, 3000);
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
				TECmsgOut(L"""��ġ����""�� �Ϸ����� ���߽��ϴ�. 3�� �� ������ġ�� �����մϴ�", LIGHTMAGENTA, 3000);
				iSqncCur = SQNC_LIST_REPEAT - 1;
				bFnRes = TRUE;
			}
			iSqncNum = SQNC_BASE;
			break;

			//*���� �ǽ�	
		case SQNC_MEASURE:
			sprintf_s(pView->m_pTask->stResult.szInspTackTime, sizeof(char) * 256, "MoveZ: %.1f s", CheckTime.GetTimeInterval(2));
			OutputDebugString(pView->m_pTask->stResult.szInspTackTime);
			CheckTime.SetTimeStart();
			bFnRes = SqncMeasure(stcResultAlign);
			if (!bFnRes)
			{
				TECmsgOut(L"""����""�� �Ϸ����� ���߽��ϴ�. 3�� �� ������ġ�� �����մϴ�", LIGHTMAGENTA, 3000);
				iSqncCur = SQNC_LIST_REPEAT - 1;
				bFnRes = TRUE;
			}
			iSqncNum = SQNC_BASE;
			break;

			//����Ʈ ������������� ǥ��
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
			// ��� ������ ���
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

			//*����Ʈ ������ŭ �ݺ�
		case SQNC_LIST_REPEAT:
			if (iListCnt - 1 <= iCurCnt)
			{
				iSqncCur = MAX_SQNC_CNT;
				iSqncNum = SQNC_BASE;
				break;
			}

// 			pView->m_pManual->m_pElipso->m_bReadyDev = FALSE;//ù���Ŀ��� ��Ӹ��

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
		TECmsgOut(L"��� ���� ��ġ ���� �Ϸ� ", YELLOW, 200, 17, BLACK);

	return bFnRes;
}


BOOL CWorkRun::SqncTargetMove(CDPoint dPntRequestPos)
{
	CTimeInterval CheckTime;
	BOOL bSqncFlag = TRUE;
	BOOL bFnRes = TRUE;
	char tmpMsg[MAX_Buf] = { "Error_SqncTargetMove" };

	enum//���� ���� �ٲٷ��� �Ʒ� ������ �ٲܰ�
	{
		SQNC_BASE = 0,

		//Z�� ������ġ �̵�
		SQNC_MOVE_SAFE_Z,
		//X,Y�� ��û�� ������ġ �̵�
		SQNC_MOVE_XY,
		//AF �ǽ�
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
			if (bFnRes == FALSE)//�˻�����
			{
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//�Ͻ�����
				break;

			if (iSqncCur == MAX_SQNC_CNT)//������ ��������
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//�����ϰ� ����
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
		TECmsgOut(L"������ġ �̵� �Ϸ� ", YELLOW, 200, 17, BLACK);
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

	enum//���� ���� �ٲٷ��� �Ʒ� ������ �ٲܰ�
	{
		SQNC_BASE = 0,

		//������ ����� �ǽ�
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
			TECmsgOut(L"��ġ���� �� �Դϴ�", YELLOW, 500, 20, BLACK, 0);
			CheckTimeMsg.SetTimeStart();
		}

		if (bFnRes && CheckTime.GetTimeInterval() > 60000)
		{
			TECmsgOut(L"Align �ð�(60sec) �ʰ��Դϴ�.", LIGHTRED, 1);
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
			if (bFnRes == FALSE)//�˻�����
			{
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//�Ͻ�����
				break;

			if (iSqncCur == MAX_SQNC_CNT)//������ ��������
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//�����ϰ� ����
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
					if (TECmsgOut(L"�����Ҹ�Ÿ ""���о����""�� �õ��Ͻðڽ��ϱ�?", LIGHTMAGENTA, 2))//�Ϸ����� ���߽��ϴ�. �ٽ�
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
					iSqncCur = SQNC_BASE;//��� �ȳ����� ó������ �ٽ�
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
				iSqncCur = SQNC_BASE;//ó������ �ٽ�
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
		TECmsgOut(L"��ġ���� �Ϸ� ", YELLOW, 200, 17, BLACK);
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

	enum//���� ���� �ٲٷ��� �Ʒ� ������ �ٲܰ�
	{
		SQNC_BASE = 0,

// 		//������ ����� �ǽ�
// 		SQNC_GET_ALIGN_Z,
// 		SQNC_CHECK_ALIGN_Z,
// 		SQNC_MOVE_ALIGN_Z,
		//����
		SQNC_MEASURE,
		//������� DATA�Է�
		SQNC_DATA,
		//���÷���	
		SQNC_DISPLAY,

		MAX_SQNC_CNT
	};
	int iSqncNum = SQNC_BASE;
	int iSqncCur = SQNC_BASE;

	if (m_strRecipeName.IsEmpty())
	{
		TECmsgOut(L"�˸��� ������ Receipe�� �����ϼ���.", LIGHTMAGENTA, 1);
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
			TECmsgOut(L"�������Դϴ�", LIGHTCYAN, 500, 20, BLACK, 0);
			CheckTimeMsg.SetTimeStart();
		}
		if (bFnRes && CheckTime.GetTimeInterval() > 60000)
		{
			TECmsgOut(L"�����ð�(60sec) �ʰ��Դϴ�.", LIGHTRED, 1);
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
			if (bFnRes == FALSE)//�˻�����
			{
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//�Ͻ�����
				break;

			if (iSqncCur == MAX_SQNC_CNT)//������ ��������
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//�����ϰ� ����
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
					if (TECmsgOut(L"�����Ҹ�Ÿ ""����""�� �Ϸ����� ���߽��ϴ�. �ٽ� �õ��Ͻðڽ��ϱ�?", LIGHTMAGENTA, 2))
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
		TECmsgOut(L"���� �Ϸ� ", YELLOW, 200, 17, BLACK);
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

	enum//���� ���� �ٲٷ��� �Ʒ� ������ �ٲܰ�
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
			if (bFnRes == FALSE)//�˻�����
			{
				TECmsgOut(L"Stop Pattern Reg", LIGHTMAGENTA, 1500);
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//�Ͻ�����
				break;

			if (iSqncCur == MAX_SQNC_CNT)//������ ��������
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//�����ϰ� ����
			break;


			//��â �� - �𵨰��ù�ư Ȱ��
		case SQNC_SET_MODEL_DLG:
			pView->m_fnSetSwitch(SW_MODEL);

			EnableDlgItem(pView->m_pModel, FALSE);

			if (m_bThrdFlag[TF_CASSETTE_START] == FALSE)
				iSqncCur = SQNC_PATT_FIND;

			iSqncNum = SQNC_BASE;
			break;


			//ī�޶� ON
			//Auto��Ŀ��
		case SQNC_MODEL_DLG:
			EnableDlgItem(pView->m_pModel, FALSE, ArrExcepID_1, 9);
			pView->m_pModel->m_fnSetRadioModelType(MODEL_TYPE_PATTERN);
			m_bSqncInterrupt[SQITRT_SELECT_MODEL] = FALSE;

			pView->m_pModel->m_ctrlListMeasureResult.ResetContent();
			pView->m_pModel->m_ctrlListMeasureResult.AddString(_T("STEP 1. ���ϵ���� �𵨸��� ����(or �߰�) ���ּ���."));

			iSqncNum = SQNC_BASE;
			break;


			//������ �����Է�â Ȱ��
			//���ϵ�� ��ưȰ��
		case SQNC_MODEL_SELECT:
			if (m_bThrdFlag[TF_CASSETTE_START])
			{
				if (!m_bSqncInterrupt[SQITRT_SELECT_MODEL])// �𵨼���
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

			pView->m_pModel->m_ctrlListMeasureResult.AddString(_T("STEP 2. ������ '��������'Ȯ�� �� '����� ���ϵ��'�� ���� �� �ּ���."));

			iSqncNum = SQNC_BASE;
			break;


			//�������� ��� �𵨸� ���� �߰�@
			//�н���ư ���� - ������������ �ε�
			//���� �� �������� ����Ȯ�� - ������ ���� �� ���� ��û  - utilclass - Existence(Ǯ���)
			//������ ����ã�� ��ưȰ��
		case SQNC_PATTERN_REG_DLG:
			if (!m_bSqncInterrupt[SQITRT_REG_PATTERN])
				break;
			m_bSqncInterrupt[SQITRT_REG_PATTERN] = FALSE;

			pView->m_pModel->GetDlgItem(IDC_BTN_FIND_PATT)->EnableWindow(TRUE);//����¡ ����

			m_bSqncInterrupt[SQITRT_FIND_PATTERN] = FALSE;

			pView->m_pModel->m_ctrlListMeasureResult.AddString(_T("STEP 3. ����� ������ ȭ�鿡 ǥ�õǰ� �����,  "));
			pView->m_pModel->m_ctrlListMeasureResult.AddString(_T("           '����ã��'�� ���� ���ּ���."));

			iSqncNum = SQNC_BASE;
			break;


			//ȭ��󿡼� ���� ã���� ������ġ�� 
			//sheet 1�࿡ 0��/0�� ������ǥ ��� - ������ġ�߰� ��ư �ڵ� ����
			//���ϰ������� Ȱ��
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
		TECmsgOut(L"���� ��� �Ϸ�", LIGHTBLUE, 1000);
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

	enum//���� ���� �ٲٷ��� �Ʒ� ������ �ٲܰ�
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
			if (bFnRes == FALSE)//�˻�����
			{
				TECmsgOut(L"Stop SubPoint Setting", LIGHTMAGENTA, 1500);
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//�Ͻ�����
				break;

			if (iSqncCur == MAX_SQNC_CNT)//������ ��������
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//�����ϰ� ����
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

			//Check Sub �ڵ�//Align��ư ��Ȱ��
			//���� : �̸��տ� Sub_
			//�ҷ����� : Sub_ �ƴϸ� �ȵ�.
		case SQNC_SUB_MODEL_SELECT:
// 			pView->m_pModel->m_ctrlCheckSetSubPoint.SetCheck(TRUE);
			pView->m_pModel->m_fnSetRadioModelType(MODEL_TYPE_SUB);

			EnableDlgItem(pView->m_pModel, FALSE, ArrExcepID_1, 9);
			m_bSqncInterrupt[SQITRT_SELECT_MODEL] = FALSE;

			pView->m_pModel->m_ctrlListMeasureResult.AddString(_T("STEP 5. ����Sub��ġ�� ����� �𵨸��� ����(or �߰�) ���ּ���.  "));

			iSqncNum = SQNC_BASE;
			break;


			//��Ͻ� ������ǥ�� �� �����ǥ�� �Է�
			//�Ϸ�
		case SQNC_SUB_POSITION_REG:
			if (m_bThrdFlag[TF_CASSETTE_START])
			{
				if (!m_bSqncInterrupt[SQITRT_SELECT_MODEL])// �𵨼���
					break;
			}
			else
			{
				pView->m_pModel->m_fnModule_ModelLoad(m_strSubName);
			}
			m_bSqncInterrupt[SQITRT_SELECT_MODEL] = FALSE;

			m_bSqncInterrupt[SQITRT_REG_POSITION] = FALSE;
			TECmsgOut(L"Cam ȭ���� ���鼭 ������ ���ϴ� ��ġ���� ��ǥ�� ���� ������ּ���.", LIGHTCYAN, 1, 17, BLACK);

			EnableDlgItem(pView->m_pModel, FALSE, ArrExcepID_4, 2);
			pView->m_pModel->GetDlgItem(IDC_FPSPREAD_SR_INFO)->EnableWindow(TRUE);//�̹�ľƴϸ� �ȵ�....
			pView->m_pStageMap->m_fnCheckCamera(TRUE);

			pView->m_pModel->m_ctrlListMeasureResult.AddString(_T("STEP 6. ���� ȭ�鿡 ǥ�õ� ��ũ�� ����(0,0)���� �ϴ�  "));
			pView->m_pModel->m_ctrlListMeasureResult.AddString(_T("           ���� Sub����Ʈ�� ����ϼ���."));
			pView->m_pModel->m_ctrlListMeasureResult.AddString(_T("           * '���̽�ƽ'���� �̵� �� '��ǥ �б�'"));
			pView->m_pModel->m_ctrlListMeasureResult.AddString(_T("           * ��� �Ϸ� ��, '����'���ּ���."));

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
		TECmsgOut(L"Sub����Ʈ ��� �Ϸ�", LIGHTBLUE, 1000);
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

	enum//���� ���� �ٲٷ��� �Ʒ� ������ �ٲܰ�
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
		//���� - �ش���ǥ ���������
		//���� - ���׿��� ����ã�� ������ġ �������

		SQNC_MAPPING,
		//�ڵ��˻�â���� �̵�
		SQNC_CALC_POS,
		//Target + Sub ��ǥ��
		//����� �ڵ��˻�â sheet�� �ѷ���

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
	pView->m_pModel->GetDlgItem(IDC_FPSPREAD_SR_INFO)->EnableWindow(TRUE);//�̹�ľƴϸ� �ȵ�....

	pView->m_pModel->m_ctrlListMeasureResult.AddString(_T("STEP 4. 'ALIGN �������'�Է� �� ������ '��������'Ȯ�� ��,"));
	pView->m_pModel->m_ctrlListMeasureResult.AddString(_T("           '���� ALIGN��ũ ��ǥ �ڵ����'�� ���� �� �ּ���."));

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
			if (bFnRes == FALSE)//�˻�����
			{
				TECmsgOut(L"Stop Pattern Setting", LIGHTMAGENTA, 1500);
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//�Ͻ�����
				break;

			if (iSqncCur == MAX_SQNC_CNT)//������ ��������
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//�����ϰ� ����
			break;


			//[�ڵ���ϰ��]��ư �� ���--------------
			//����ũ��/���� �����Է��� ���Ŭ��
			//���� ������ġ MapView�� ǥ��(����) (���ϰ������-����)
			//���������ʺ�draw�� ���
			//������ġ �ڵ� ����(��������) - ������ġ ���� 䨹������� 4����Ʈ - �ܰ����� ��ĭ ��
			//sheet 2~5�࿡ ��ǥ ���
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
			m_bSqncInterrupt[SQITRT_REG_POSITION] = FALSE;//�ռ���ưŬ������ ���� ���� �ʱ�ȭ

			iSqncNum = SQNC_BASE;
			break;


// 			//���� : �̸��տ� Taget_@
// 			//�ҷ����� : Taget_ �ƴϸ� �ȵ�.@
// 			//������ �����Է�â Ȱ��
// 			//���ϵ�� ��ưȰ��
// 		case SQNC_POSITION_REG:
// 			EnableDlgItem(pView->m_pModel, FALSE, ArrExcepID_5, 10);
// // 			pView->m_pModel->m_fnCheckAlignUse(TRUE);
// // 			pView->m_pModel->m_ctrlCheckSetSubPoint.SetCheck(FALSE);
// 			pView->m_pModel->m_fnSetRadioModelType(MODEL_TYPE_TARGET);
// 
// 			pView->m_pModel->GetDlgItem(IDC_FPSPREAD_SR_INFO)->EnableWindow(TRUE);//�̹�ľƴϸ� �ȵ�....
// 
// 			pView->m_pModel->m_ctrlListMeasureResult.AddString(_T("STEP 5. ������ Ÿ�� ���� ����(or �߰�) �� �ּ���."));
// 
// 			iSqncNum = SQNC_BASE;
// 			break;


			//XXX-sheet Ȱ��
			//XXX-6����� ����� ��ϰ���
			//XXX-��ü ���� ���(������ġ ���� ��ε��)
			//�Ϸ� - SubPoint ����Ȯ��
		case SQNC_POSITION_CALC:
// 			if (!m_bSqncInterrupt[SQITRT_REG_TAGET_POSITION])
// 				break;
			m_bSqncInterrupt[SQITRT_REG_POSITION] = FALSE;
			pView->m_pStageMap->m_fnCheckCamera(TRUE);

			m_bSqncInterrupt[SQITRT_FIND_PATTERN_FAIL] = TRUE;

			iSqncNum = SQNC_BASE;
			break;


		case SQNC_MOVE_ALIGN_POS:
// 			if (!pView->m_pModel->m_bSetInsertSubPosFinish)// ������
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


			//���� - ���׿��� ����ã�� ������ġ �������
			//���� - �ش���ǥ ���������
		case SQNC_PATT_CONFIRM_ALIGN_POS:
			if (pTask->ErrorCode)//����
			{
				//���л����̹Ƿ� Ȯ���ش޶�� �޼���(�ߺ�����)
				if (m_bSqncInterrupt[SQITRT_FIND_PATTERN_FAIL])
				{
					m_bSqncInterrupt[SQITRT_FIND_PATTERN_FAIL] = FALSE;
					iSqncNum = iSqncCur = SQNC_MOVE_ALIGN_POS;
					break;
				}
// 				else
// 				{
// 					TECmsgOut(L"������ ã�� ���߽��ϴ�.", LIGHTMAGENTA, 1);
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


			//�ڵ��˻�â���� �̵�
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
				//���������
				TECmsgOut(L"Mapping������ �Ϸ����� ���߽��ϴ�. �ش������ �˻縦 �����մϴ�.", LIGHTMAGENTA, 5000);
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


			//���õ� Target�� ���� - Data����ü
			//���õ� Sub�� ���� - Data����ü
			//Target + Sub ��ǥ��
			//����� �ڵ��˻�â sheet�� �ѷ���
		case SQNC_CALC_POS:
// 			EnableDlgItem(pView->m_pModel, TRUE);//����ư Ȱ��

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
		TECmsgOut(L"����� �Ϸ�", LIGHTBLUE, 1000);
	}
	EnableDlgItem(pView->m_pModel, TRUE);//����ư Ȱ��

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

	enum//���� ���� �ٲٷ��� �Ʒ� ������ �ٲܰ�
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

	pView->m_pModel->m_ctrlListMeasureResult.AddString(_T("STEP 7. ����Target���� ��ǥ(��/��)�� ����� �𵨸���"));
	pView->m_pModel->m_ctrlListMeasureResult.AddString(_T("           ����(or �߰�) ���ּ���."));

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
			if (bFnRes == FALSE)//�˻�����
			{
				TECmsgOut(L"Stop Pattern Setting", LIGHTMAGENTA, 1500);
				bSqncFlag = FALSE;
				break;
			}
			if (pView->m_pAuto->m_bPause)//�Ͻ�����
				break;

			if (iSqncCur == MAX_SQNC_CNT)//������ ��������
			{
				bSqncFlag = FALSE;
				break;
			}

			if (pAxis[X_AXIS]->CheckInMotion()
				|| pAxis[Y_AXIS]->CheckInMotion()
				|| pAxis[Z_AXIS]->CheckInMotion())
				break;

			iSqncNum = ++iSqncCur;//�����ϰ� ����
			break;


			//���� : �̸��տ� Taget_@
			//�ҷ����� : Taget_ �ƴϸ� �ȵ�.@
			//������ �����Է�â Ȱ��
			//���ϵ�� ��ưȰ��
		case SQNC_MODEL_SELECT:
			if (m_bThrdFlag[TF_CASSETTE_START])
			{
				if (!m_bSqncInterrupt[SQITRT_SELECT_MODEL])// �𵨼���
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

			pView->m_pModel->GetDlgItem(IDC_FPSPREAD_SR_INFO)->EnableWindow(TRUE);//�̹�ľƴϸ� �ȵ�....
// 			pView->m_pModel->m_clSR.EnableWindow(TRUE);

			pView->m_pModel->m_ctrlListMeasureResult.AddString(_T("STEP 8. ����Target���� ��ǥ(��/��)�� ����ϼ���."));
			pView->m_pModel->m_ctrlListMeasureResult.AddString(_T("           * '������ġ�߰�' �Ŀ�, ��/�� ���� �Է��ϼ���."));
			pView->m_pModel->m_ctrlListMeasureResult.AddString(_T("           * ��� �Ϸ� ��, '����'���ּ���."));

			iSqncNum = SQNC_BASE;
			break;


			//XXX-sheet Ȱ��
			//XXX-6����� ����� ��ϰ���
			//XXX-��ü ���� ���(������ġ ���� ��ε��)
			//�Ϸ� - SubPoint ����Ȯ��
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
		TECmsgOut(L"Target����Ʈ ��� �Ϸ�", LIGHTBLUE, 1000);
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
// 	enum//���� ���� �ٲٷ��� �Ʒ� ������ �ٲܰ�
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
// 		//���� - �ش���ǥ ���������
// 		//���� - ���׿��� ����ã�� ������ġ �������
// 
// 		SQNC_MAPPING,
// 		//�ڵ��˻�â���� �̵�
// 		SQNC_CALC_POS,
// 		//Target + Sub ��ǥ��
// 		//����� �ڵ��˻�â sheet�� �ѷ���
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
// 			if (bFnRes == FALSE)//�˻�����
// 			{
// 				TECmsgOut(L"Stop Pattern Setting", LIGHTMAGENTA, 1500);
// 				bSqncFlag = FALSE;
// 				break;
// 			}
// 			if (pView->m_pAuto->m_bPause)//�Ͻ�����
// 				break;
// 
// 			if (iSqncCur == MAX_SQNC_CNT)//������ ��������
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
// 			iSqncNum = ++iSqncCur;//�����ϰ� ����
// 			break;
// 
// 
// 			//���� ���� ���� ��ǥ - ������ ����@
// 			//��â �� - �𵨰��ù�ư Ȱ��
// 			//�����üũ
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
// 			//ī�޶� ON
// 			//Auto��Ŀ��
// 		case SQNC_MODEL_DLG:
// 			pView->m_pStageMap->m_fnCheckCamera(TRUE);
// 			ThrdAF();
// 
// 			iSqncNum = SQNC_BASE;
// 			break;
// 
// 
// 			//���� : �̸��տ� Taget_@
// 			//�ҷ����� : Taget_ �ƴϸ� �ȵ�.@
// 			//������ �����Է�â Ȱ��
// 			//���ϵ�� ��ưȰ��
// 		case SQNC_MODEL_SELECT:
// 			if (!m_bSqncInterrupt[SQITRT_SELECT_MODEL])// �𵨼���
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
// 			//�������� ��� �𵨸� ���� �߰�@
// 			//�н���ư ���� - ������������ �ε�
// 			//���� �� �������� ����Ȯ�� - ������ ���� �� ���� ��û  - utilclass - Existence(Ǯ���)
// 			//������ ����ã�� ��ưȰ��
// 		case SQNC_PATTERN_REG_DLG:
// 			if (!m_bSqncInterrupt[SQITRT_REG_PATTERN])
// 				break;
// 			m_bSqncInterrupt[SQITRT_REG_PATTERN] = FALSE;
// 
// 			pView->m_pModel->GetDlgItem(IDC_BTN_FIND_PATT)->EnableWindow(TRUE);//����¡ ����
// 
// 			m_bSqncInterrupt[SQITRT_FIND_PATTERN] = FALSE;
// 			iSqncNum = SQNC_BASE;
// 			break;
// 
// 
// 			//ȭ��󿡼� ���� ã���� ������ġ�� 
// 			//sheet 1�࿡ 0��/0�� ������ǥ ��� - ������ġ�߰� ��ư �ڵ� ����
// 			//���ϰ������� Ȱ��
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
// 			//�ڵ���ϰ���ư �� ���--------------
// 			//����ũ��/���� �����Է��� ���Ŭ��
// 			//���� ������ġ MapView�� ǥ��(����) (���ϰ������-����)
// 			//���������ʺ�draw�� ���
// 			//������ġ �ڵ� ����(��������) - ������ġ ���� 䨹������� 4����Ʈ - �ܰ����� ��ĭ ��
// 			//sheet 2~5�࿡ ��ǥ ���
// 		case SQNC_PATT_INFO:
// 			if (!m_bSqncInterrupt[SQITRT_AUTO_CAL_PATTERN_INFO])
// 				break;
// 			m_bSqncInterrupt[SQITRT_AUTO_CAL_PATTERN_INFO] = FALSE;
// 			m_bSqncInterrupt[SQITRT_REG_TAGET_POSITION] = FALSE;//�ռ���ưŬ������ ���� ���� �ʱ�ȭ
// 
// 			EnableDlgItem(pView->m_pModel, FALSE, ArrExcepID_4, 5);
// 			iSqncNum = SQNC_BASE;
// 			break;
// 
// 
// 			//XXX-sheet Ȱ��
// 			//XXX-6����� ����� ��ϰ���
// 			//XXX-��ü ���� ���(������ġ ���� ��ε��)
// 			//�Ϸ� - SubPoint ����Ȯ��
// 		case SQNC_POSITION_REG:
// 			if (!m_bSqncInterrupt[SQITRT_REG_TAGET_POSITION])
// 				break;
// 			m_bSqncInterrupt[SQITRT_REG_TAGET_POSITION] = FALSE;
// 
// 			if (!TECmsgOut(L"SubPoint ����� �Ͻðڽ��ϱ�?", LIGHTMAGENTA, 2))
// 			{
// 				iSqncNum = SQNC_MOVE_ALIGN_POS;//�� ��������
// 				break;
// 			}
// 
// 			iSqncNum = SQNC_BASE;
// 			break;
// 
// 
// 			//Check Sub �ڵ�//Align��ư ��Ȱ��
// 			//���� : �̸��տ� Sub_
// 			//�ҷ����� : Sub_ �ƴϸ� �ȵ�.
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
// 			//��Ͻ� ������ǥ�� �� �����ǥ�� �Է�
// 			//�Ϸ�
// 		case SQNC_SUB_POSITION_REG:
// 			if (!m_bSqncInterrupt[SQITRT_SELECT_MODEL])// �𵨼���
// 				break;
// 			m_bSqncInterrupt[SQITRT_SELECT_MODEL] = FALSE;
// 
// 			pView->m_pModel->m_clSR.EnableWindow(TRUE);
// 
// 			break;
// 
// 
// 		case SQNC_MOVE_ALIGN_POS:
// 			if (!pView->m_pModel->m_bSetInsertSubPosFinish)// ������
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
// 			//���� - ���׿��� ����ã�� ������ġ �������
// 			//���� - �ش���ǥ ���������
// 		case SQNC_PATT_CONFIRM_ALIGN_POS:
// 			if (pTask->ErrorCode)//����
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
// 			//�ڵ��˻�â���� �̵�
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
// 				if (TECmsgOut(L"Mapping������ �Ϸ����� ���߽��ϴ�. ���� Align Mark ����� �ٽ� �Ͻðڽ��ϱ�?", LIGHTMAGENTA, 2))
// 				{
// 					iSqncNum = SQNC_MOVE_ALIGN_POS;
// 					break;
// 				}
// 				else
// 				{
// 					//���������?
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
// 			//���õ� Target�� ���� - Data����ü
// 			//���õ� Sub�� ���� - Data����ü
// 			//Target + Sub ��ǥ��
// 			//����� �ڵ��˻�â sheet�� �ѷ���
// 		case SQNC_CALC_POS:
// 			EnableDlgItem(pView->m_pModel, TRUE);//����ư Ȱ��
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
// 		TECmsgOut(L"���ϼ��� �Ϸ�", LIGHTBLUE, 1000);
// 	}
// 
// 	return bFnRes;
// }


#endif