#include "stdafx.h"

#include "ModelData.h"


CModelData* pModelData;
CModelData* pModelData_Backup;

CModelList::CModelList(void)
{
// 	GetCurrentDirectory(MAX_PATH, curDirName); // Current File Path...
	sprintf_s(curDirName, sizeof(curDirName), _T("%s%s"), m_Util.m_fnGetCurrentDrive().GetString(), DEFAULT_PATH);
	m_Util.CreateDir(curDirName);
	MakeDirectory();

	ModelCnt = 1;
	// 	modelListName->Empty();
	sprintf_s(modelListName[0], sizeof(modelListName[0]), _T("DEFAULT"));
}
CModelList::~CModelList(void)
{
}

void CModelList::MakeDirectory(void)
{
	TCHAR tmpStr[MAX_PATH];

	sprintf_s(tmpStr, sizeof(tmpStr), _T("%sModel\\"), curDirName);
	CreateDirectory(tmpStr, NULL);
}

void CModelList::ListSave()
{
	int i = 0, j = 0, k = 0;

	CString strINIPath;
	CString strSection, strKey, strValue;

	FILE *out;
	TCHAR tmpStr[CHAR_BUFF];

	sprintf_s(tmpStr, sizeof(tmpStr), (_T("%sModel\\ModelList.ini")), curDirName);

	if (fopen_s(&out, tmpStr, _T("w")) != 0)
	{
		TECmsgOut( L"Failure to save model List !!!", RED_1, 1 );
	}
	else fclose(out);

	strINIPath.Format(tmpStr);

	strSection = _T("Current_Model");

	strKey.Format(_T("Model_Name"));
	strValue.Format(_T("%s"), modelName);
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	sorting_ModelList();

	strSection = _T("ModelList");

	strKey.Format(_T("ModelCount"));
	strValue.Format(_T("%d"), ModelCnt);
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);
	for (i = 0; i < ModelCnt; i++)
	{
		strKey.Format(_T("%d"), i);
		strValue.Format(_T("%s"), modelListName[i]);
		WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

// 		sprintf_s(tmpStr, sizeof(tmpStr), _T("%sModel\\%s"), curDirName, modelListName[i]);
// 		m_Util.CreateDir(tmpStr);
	}
}

void CModelList::ListLoad()
{
	int i = 0, j = 0, k = 0;

	CString strINIPath;
	CString strSection, strKey, strValue;

	TCHAR szBuf[CHAR_BUFF] = { 0, };

	FILE *in;
	TCHAR tmpStr[CHAR_BUFF];
	sprintf_s(tmpStr, sizeof(tmpStr), (_T("%sModel\\ModelList.ini")), curDirName);

	if (fopen_s(&in, tmpStr, _T("r")) != 0)
	{
		ListSave();
	}
	else fclose(in);

	strINIPath.Format(tmpStr);


	strSection = _T("Current_Model");

	strKey.Format(_T("Model_Name"));
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, CHAR_BUFF, strINIPath);
	strValue.Format(_T("%s"), szBuf);
	sprintf_s(modelName, sizeof(modelName), strValue);

	strSection = _T("ModelList");

	strKey.Format(_T("ModelCount"));
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, CHAR_BUFF, strINIPath);
	strValue.Format(_T("%s"), szBuf);
	ModelCnt = atoi(strValue);
	for (i = 0; i < ModelCnt; i++)
	{
		strKey.Format(_T("%d"), i);
		GetPrivateProfileString(strSection, strKey, _T(""), szBuf, CHAR_BUFF, strINIPath);
		strValue.Format(_T("%s"), szBuf);
		sprintf_s(modelListName[i], sizeof(modelListName[i]), strValue);
	}
}

void CModelList::sorting_ModelList(int ASCflag /*= 1*/)
{
	//ASCflag == 0	내림차순[5,4,3,2,1]
	//ASCflag == 1  오름차순[1,2,3,4,5]
	int i = 0, j = 0;
	CString  tmpStr = _T("");

	if (ASCflag == 1)
	{//오름차순
		for (j = 0; j < (ModelCnt - 1); j++)
		{
			for (i = 0; i<(ModelCnt - 1); i++)
			{
				if (strcmp(modelListName[i], modelListName[i + 1]) > 0)//i+1번 str이 큼
				{
					tmpStr = modelListName[i + 1];
					memcpy(modelListName[i + 1], modelListName[i], sizeof(TCHAR)*CHAR_BUFF);
					memcpy(modelListName[i], tmpStr, sizeof(TCHAR)*CHAR_BUFF);
				}
			}
		}
	}
}


CModelData::CModelData(void)
	:iDiffTh(0)
	,iDiffPixel(0)
	,iVal(0)
	,iValDir(0)
	,iScandir(0)
	,dSigmaSTD(0.0)
	, iNochKind(0)
{
	initData();
}


void CModelData::initData(void)
{
	int i, j, k;
	i = j = k = 0;

	sprintf_s(modelName, sizeof(modelName), _T(""));

	for (i = 0; i < DEF_MAX_MARK_NUM; i++)
	{
		cdMarkCenterPos[i].x = 0;
		cdMarkCenterPos[i].y = 0;
		cdMarkSize[i].x = 0;
		cdMarkSize[i].y = 0;
	}

	iMatchValue = 60;
	iModNumber = 1;
	iModSmoothness = 80;
	iModAcceptance = 75;
	dModScaleMax = 1.1;
	dModScaleMin = 0.9;
	dModAngleDeltaNeg = 180.0;
	dModAngleDeltaPos = 180.0;
	dModReferenceAngle = 0.0;
	iModTimeout = 500;
	iModRectMode = 0;
	iModRectHor = 0;
	iModRectVer = 0;

	iModDntSize = 50;

	dImgResol = 0.002156;

}

CModelData::~CModelData(void)
{
}

void CModelData::Save()
{
	int i, j, k;
	i = j = k =0;

	FILE *out;
	TCHAR tmpStr[CHAR_BUFF];
// 	sprintf_s(tmpStr, sizeof(tmpStr), (_T("%sModel\\%s\\ModelData.ini")), curDirName, modelName);
	sprintf_s(tmpStr, sizeof(tmpStr), (_T("%sModel\\ModelData.ini")), curDirName);

	if (fopen_s(&out, tmpStr, _T("w")) != 0)
	{
		TECmsgOut( L"Failure to save model Data !!!", RED_1, 1 );
	}
	else fclose(out);


// 	EZIni ini(m_Util.FormatStr((_T("%sModel\\%s\\ModelData.ini")), curDirName, modelName));
	EZIni ini(m_Util.FormatStr((_T("%sModel\\ModelData.ini")), curDirName));

	//ex>	ini["ALIGN"][FormatStr("CD %d X POS", i+1)] = m_nCDCountX[i];
	ini["SEARCH_EDGE"]["iDiffTh"]		= iDiffTh	;
	ini["SEARCH_EDGE"]["iDiffPixel"]	= iDiffPixel;
	ini["SEARCH_EDGE"]["iVal"]			= iVal		;
	ini["SEARCH_EDGE"]["iValDir"]		= iValDir	;
	ini["SEARCH_EDGE"]["iScandir"]		= iScandir	;
	ini["SEARCH_EDGE"]["fSigmaSTD"]		= dSigmaSTD	;
	ini["SEARCH_EDGE"]["iNochKind"]		= iNochKind	;

	CString strSection, strKey;

	strSection = _T("INSPECTION_MARK");
	for (i = 0; i < DEF_MAX_MARK_NUM; i++)
	{
		strKey.Format(_T("MARK_%d_CENTER_X"), i);
		ini[strSection][strKey] = cdMarkCenterPos[i].x;
		strKey.Format(_T("MARK_%d_CENTER_Y"), i);
		ini[strSection][strKey] = cdMarkCenterPos[i].y;

		strKey.Format(_T("MARK_%d_SIZE_X"), i);
		ini[strSection][strKey] = cdMarkSize[i].x;
		strKey.Format(_T("MARK_%d_SIZE_Y"), i);
		ini[strSection][strKey] = cdMarkSize[i].y;
	}
	ini[strSection]["MATCH_VALUE"]					= iMatchValue;
	ini[strSection]["NUMBER"]						= iModNumber;
	ini[strSection]["SMOOTHNESS"]					= iModSmoothness;
	ini[strSection]["ACCEPTANCE"]					= iModAcceptance;
	ini[strSection]["SCALE_MAX"]					= dModScaleMax;
	ini[strSection]["SCALE_MIN"]					= dModScaleMin;
	ini[strSection]["AngleDeltaNega"]				= dModAngleDeltaNeg;
	ini[strSection]["AngleDeltaPosi"]				= dModAngleDeltaPos;
	ini[strSection]["ReferenceAngle"]				= dModReferenceAngle;
	ini[strSection]["ModTimeout"]					= iModTimeout;
	ini[strSection]["Mod RectMode(0:Img 1:Rect)"]	= iModRectMode;
	ini[strSection]["Mod RectMode Hor"]				= iModRectHor;
	ini[strSection]["Mod RectMode Ver"]				= iModRectVer;
	ini[strSection]["Mod Dont Size"]				= iModDntSize;

	strSection = _T("AVG_VALUE");
	ini[strSection]["AVG_MIN_VALUE"]				= iAvg_MinValue;
	ini[strSection]["AVG_COMPARE_VALUE"]			= iAvg_CompareValue;

	strSection = _T("IMG_RESOL");
	ini[strSection]["dImgResol"]					= dImgResol;
}

void CModelData::Load()
{
	int i, j, k;
	i = j = k = 0;

	FILE *in;
	TCHAR tmpStr[CHAR_BUFF];
// 	sprintf_s(tmpStr, sizeof(tmpStr), (_T("%sModel\\%s\\ModelData.ini")), curDirName, modelName);
	sprintf_s(tmpStr, sizeof(tmpStr), (_T("%sModel\\ModelData.ini")), curDirName);

	if (fopen_s(&in, tmpStr, _T("r")) != 0)
	{
		Save();
	}
	else fclose(in);


// 	EZIni ini(m_Util.FormatStr((_T("%sModel\\%s\\ModelData.ini")), curDirName, modelName));
	EZIni ini(m_Util.FormatStr((_T("%sModel\\ModelData.ini")), curDirName));

	//ex>	m_nCDCountX[i] = ini["ALIGN"][FormatStr("CD %d X POS", i+1)] ;
	iDiffTh		= ini["SEARCH_EDGE"]["iDiffTh"]		;
	iDiffPixel  = ini["SEARCH_EDGE"]["iDiffPixel"]	;
	iVal		= ini["SEARCH_EDGE"]["iVal"]		;
	iValDir		= ini["SEARCH_EDGE"]["iValDir"]		;
	iScandir	= ini["SEARCH_EDGE"]["iScandir"]	;
	dSigmaSTD	= ini["SEARCH_EDGE"]["fSigmaSTD"]	;
	iNochKind	= ini["SEARCH_EDGE"]["iNochKind"]	;

	CString strSection, strKey;

	strSection = _T("INSPECTION_MARK");
	for (i = 0; i < DEF_MAX_MARK_NUM; i++)
	{
		strKey.Format(_T("MARK_%d_CENTER_X"), i);
		cdMarkCenterPos[i].x = ini[strSection][strKey];
		strKey.Format(_T("MARK_%d_CENTER_Y"), i);
		cdMarkCenterPos[i].y = ini[strSection][strKey];

		strKey.Format(_T("MARK_%d_SIZE_X"), i);
		cdMarkSize[i].x = ini[strSection][strKey];
		strKey.Format(_T("MARK_%d_SIZE_Y"), i);
		cdMarkSize[i].y = ini[strSection][strKey];
	}
	iMatchValue			= ini[strSection]["MATCH_VALUE"]				;
	iModNumber			= ini[strSection]["NUMBER"]						;
	iModSmoothness		= ini[strSection]["SMOOTHNESS"]					;
	iModAcceptance		= ini[strSection]["ACCEPTANCE"]					;
	dModScaleMax		= ini[strSection]["SCALE_MAX"]					;
	dModScaleMin		= ini[strSection]["SCALE_MIN"]					;
	dModAngleDeltaNeg	= ini[strSection]["AngleDeltaNega"]				;
	dModAngleDeltaPos	= ini[strSection]["AngleDeltaPosi"]				;
	dModReferenceAngle	= ini[strSection]["ReferenceAngle"]				;
	iModTimeout			= ini[strSection]["ModTimeout"]					;
	iModRectMode		= ini[strSection]["Mod RectMode(0:Img 1:Rect)"]	;
	iModRectHor			= ini[strSection]["Mod RectMode Hor"]			;
	iModRectVer			= ini[strSection]["Mod RectMode Ver"]			;
	iModDntSize			= ini[strSection]["Mod Dont Size"]				;

	strSection = _T("AVG_VALUE");
	iAvg_MinValue		= ini[strSection]["AVG_MIN_VALUE"]				;
	iAvg_CompareValue	= ini[strSection]["AVG_COMPARE_VALUE"]			;

	strSection = _T("IMG_RESOL");
	dImgResol			= ini[strSection]["dImgResol"]					;

}
