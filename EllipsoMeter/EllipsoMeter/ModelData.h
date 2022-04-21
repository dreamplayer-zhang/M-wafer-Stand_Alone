#pragma once
#include "EzIni.h"

class CModelList
{
public:
	CModelList(void);
	~CModelList(void);

	TCHAR curDirName[MAX_PATH];

	int ModelCnt;
	TCHAR modelListName[CHAR_BUFF][CHAR_BUFF];//CHAR_BUFF=�𵨰���
	TCHAR modelName[CHAR_BUFF];//CHAR_BUFF=���̸����ڱ���
	TCHAR old_modelName[CHAR_BUFF];//CHAR_BUFF=���̸����ڱ���

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

	CDPoint cdMarkSize[DEF_MAX_MARK_NUM];			//�� ī�޶� ��ũ ������
	CDPoint cdMarkCenterPos[DEF_MAX_MARK_NUM];		//�� ī�޶� ��ũ�� ���� ��
	int		iMatchValue;								//�� ī�޶� ���� �˻�� ��Ī��
	int		iModRectMode; //0:M_Image 1:M_Rect
	int		iAvg_MinValue;		//���͸� �� �ּ� ��Ⱚ
	int		iAvg_CompareValue;	//OK ���� ��Ⱚ




	void initData(void);
	void Save();
	void Load();
};

extern CModelData* pModelData;
extern CModelData* pModelData_Backup;
