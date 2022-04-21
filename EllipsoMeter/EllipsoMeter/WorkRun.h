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

	BOOL m_bAutoMode;//투입모드
// 	BOOL m_bAutoMode2;//측정모드1
	BOOL m_bZzeroMove;
	BOOL m_bWaferAlign;
	BOOL m_bCompleteWaferInput;
	CString m_strRecipeName;

	BOOL m_bPattInfo;
	BOOL m_bTargetInfo;
	BOOL m_bTargetPattRegComplete;

	//[배열수][x,y,z]
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

	//투입위치이동 - 자동/수동
	BOOL SqncMoveSupplyPosition();
	//투입대기
	BOOL SqncStandbySupply();
	//시작신호 - 자동/수동
	BOOL SqncInputStartSignal();
	//웨이퍼 감지
	BOOL SqncDetectWafer();

	//얼라인-웨이퍼
	BOOL SqncAlignWafer();
	//중심부터 지정방향으로 엣지검색
	BOOL SqncSearchEdge(int iSearchDirection);

	//얼라인-패턴(별도 쓰레드 구현필요)
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

	//자동 이동-측정
	BOOL SqncAutoMoveMeasure();
	
	//측정위치이동
	BOOL SqncTargetMove(CDPoint dPntRequestPos);
	BOOL SqncCorrectAlign(t_ELIPSO_RESULT_INFO& stcResultAlign);
	//측정
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
//패턴 세팅 스레드
void ThrdPattSet();

//검사 스레드
void ThrdRunning();

void ThrdPattTest(int iStep);

void ThrdWaferAlignTest();
#endif