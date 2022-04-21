#pragma once
#include "EzIni.h"

class CModelList
{
public:
	CModelList(void);
	~CModelList(void);

	TCHAR curDirName[MAX_PATH];

	int ModelCnt;
	TCHAR modelListName[CHAR_BUFF][CHAR_BUFF];//CHAR_BUFF=모델개수
	TCHAR modelName[CHAR_BUFF];//CHAR_BUFF=모델이름문자길이
	TCHAR old_modelName[CHAR_BUFF];//CHAR_BUFF=모델이름문자길이

	void MakeDirectory(void);

	void ListSave();
	void ListLoad();

	void sorting_ModelList(int ASCflag = 1);

};

class CModelData : public CModelList
{
public:
	CModelData(void);
	~CModelData(void);

	int		iDiffTh;
	int		iDiffPixel;
	int		iVal;
	int		iValDir;
	int		iScandir;
	double	dSigmaSTD;
	int		iNochKind;


	//PattAlign Cam Para
	int		iModNumber;
	int		iModSmoothness;
	int		iModAcceptance;
	double	dModScaleMax;
	double	dModScaleMin;
	double	dModAngleDeltaNeg;
	double	dModAngleDeltaPos;
	double	dModReferenceAngle;
	int		iModTimeout;

	int		iModRectHor;
	int		iModRectVer;
	int		iModDntSize;

	double	dImgResol;

	CDPoint cdMarkSize[DEF_MAX_MARK_NUM];			//각 카메라 마크 사이즈
	CDPoint cdMarkCenterPos[DEF_MAX_MARK_NUM];		//각 카메라 마크의 센터 값
	int		iMatchValue;								//각 카메라 영상 검사시 매칭율
	int		iModRectMode; //0:M_Image 1:M_Rect
	int		iAvg_MinValue;		//필터링 할 최소 밝기값
	int		iAvg_CompareValue;	//OK 기준 밝기값




	void initData(void);
	void Save();
	void Load();
};

extern CModelData* pModelData;
extern CModelData* pModelData_Backup;
