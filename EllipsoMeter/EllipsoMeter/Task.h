#pragma once

#define VISION_NAME										_T("VISION_ALIGN")
#define MAX_ALIGN_POINT									100
#define DEF_IMAGE_X										2000		//영상 원본 가로 사이즈
#define DEF_IMAGE_Y										2000		//영상 원본 세로 사이즈

class CTask
{
public:
	CTask();
	~CTask();

	CDPoint cdTargetPos;
	double fAlign_X;
	double fAlign_Y;
	double fAlign_T;


	// 함수동작요청시에 1, 리턴 완료시에 0
	int iSetMasterPatt;
	int	iSetMotorAlign;
	int	iGetPattAlignCenter;
	int	iGetTransCoordinate;
	// 함수동작요청시에 1, 리턴 완료시에 0, 패턴찾기 실패면 2

	int iGroupNum;

	unsigned char ImageBuff[MAX_ALIGN_POINT][DEF_IMAGE_X*DEF_IMAGE_Y];// [얼라인이외의경우 0번버퍼 사용] [카메라 영상사이즈 고정]
	int PatternNum;

	double XPixArray[MAX_ALIGN_POINT];
	double YPixArray[MAX_ALIGN_POINT];
	double XWorldArray[MAX_ALIGN_POINT];
	double YWorldArray[MAX_ALIGN_POINT];
	int NumPoint;

	CDPoint AlignCenter; //CDPoint Class는 Stdafx.h CommonStruct.h 인클루드 윗줄에 있어요.

	double inX;
	double inY;
	double OutX;
	double OutY;
	int tansformType;

	int ErrorCode;
	TCHAR msgVision[512];

};

extern CTask*              pTask;
