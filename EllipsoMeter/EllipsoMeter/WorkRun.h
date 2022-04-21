#pragma once

#if MIL_USE
enum THRD_FLAG
{
	TF_AUTORUN = 0,
	TF_AUTOFOCUS,
	TF_ELLIPSORUN,
	TF_MODELSET,
	TF_MAKE_REPORT,
	TF_CASSETTE_START,

	MAX_TF_CNT
};

enum THRD_AUTORUN_STEP
{
	THRD_READY = 0,
	THRD_REQ_SIG,
	THRD_INPUT_READY,
	THRD_MOVE_POSITION,
	THRD_MEASURE,

	MAX_THRD_AUTORUN_CNT
};

enum SQNC_INTERRUPT
{
	SQITRT_SELECT_MODEL = 0,
	SQITRT_REG_PATTERN,
	SQITRT_FIND_PATTERN,
	SQITRT_FIND_PATTERN_FAIL,
	SQITRT_REG_POSITION,
	SQITRT_AUTO_CAL_PATTERN_INFO,

	MAX_SQITRT_CNT
};

class CWorkRun
{
public:
	CWorkRun();
	~CWorkRun();

	BOOL m_bThrdFlag[MAX_TF_CNT];

	BOOL m_bAutoMode;//���Ը��
// 	BOOL m_bAutoMode2;//�������1
	BOOL m_bZzeroMove;
	BOOL m_bWaferAlign;
	BOOL m_bCompleteWaferInput;
	CString m_strRecipeName;

	BOOL m_bPattInfo;
	BOOL m_bTargetInfo;
	BOOL m_bTargetPattRegComplete;

	//[�迭��][x,y,z]
	double m_dPntForCircle[3][3];

	CString m_strTargetName;
	CString m_strSubName;
	CString	m_strPatternName;

	BOOL m_bFinishedCassette;
	BOOL m_bJustNowFinish;

	BOOL m_bInitManualData;

	BOOL m_bSqncInterrupt[MAX_SQITRT_CNT];


public:
	BOOL SqncVisionAF(BOOL bVisionAF);

	//������ġ�̵� - �ڵ�/����
	BOOL SqncMoveSupplyPosition();
	//���Դ��
	BOOL SqncStandbySupply();
	//���۽�ȣ - �ڵ�/����
	BOOL SqncInputStartSignal();
	//������ ����
	BOOL SqncDetectWafer();

	//�����-������
	BOOL SqncAlignWafer();
	//�߽ɺ��� ������������ �����˻�
	BOOL SqncSearchEdge(int iSearchDirection);

	//�����-����(���� ������ �����ʿ�)
	BOOL SqncAlignPatt();
	BOOL SqncManualMeasure();
	BOOL SqncRecipeLoad();
	BOOL SqncReadyMeasure(BOOL bInitData);
	BOOL SqncCloseMeasure();
	BOOL SqncPattReg();
	BOOL SqncSubReg();
	BOOL SqncAlignMapping();
	BOOL SqncTargetReg();
	BOOL SqncAlign_Proto();

	//�ڵ� �̵�-����
	BOOL SqncAutoMoveMeasure();
	
	//������ġ�̵�
	BOOL SqncTargetMove(CDPoint dPntRequestPos);
	BOOL SqncCorrectAlign(t_ELIPSO_RESULT_INFO& stcResultAlign);
	//����
	BOOL SqncMeasure(t_ELIPSO_RESULT_INFO& stcResultAlign);

};

extern CWorkRun* pWorkRun;


// void ThrdRunningTest();
void ThrdAF(BOOL bVisionAF);
void ThrdManualMeasure();
void ThrdLoadRecipe();
void ThrdCorrectAlign();
void ThrdReMeasure(int nRow, BOOL* bRes);
void ThrdWaitMilProcess(BOOL bCheckVal);
//���� ���� ������
void ThrdPattSet();

//�˻� ������
void ThrdRunning();

void ThrdPattTest(int iStep);

void ThrdWaferAlignTest();
#endif