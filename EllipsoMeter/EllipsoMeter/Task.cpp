#include "stdafx.h"
#include "Task.h"

CTask*              pTask;

CTask::CTask()
{
	fAlign_X = 0.0;
	fAlign_Y = 0.0;
	fAlign_T = 0.0;


	// �Լ����ۿ�û�ÿ� 1, ���� �Ϸ�ÿ� 0
	iSetMasterPatt = 0;
	iSetMotorAlign = 0;
	iGetPattAlignCenter = 0;
	iGetTransCoordinate = 0;
	// �Լ����ۿ�û�ÿ� 1, ���� �Ϸ�ÿ� 0, ����ã�� ���и� 2

	iGroupNum = 0;

	ZeroMemory(ImageBuff, sizeof(ImageBuff));
	PatternNum = 0;

	ZeroMemory(XPixArray, sizeof(XPixArray));
	ZeroMemory(YPixArray, sizeof(YPixArray));
	ZeroMemory(XWorldArray, sizeof(XWorldArray));
	ZeroMemory(YWorldArray, sizeof(YWorldArray));
	NumPoint = 0;

	inX = 0.0;
	inY = 0.0;
	OutX = 0.0;
	OutY = 0.0;
	tansformType = 0;

	ErrorCode = 0;
	ZeroMemory(msgVision, sizeof(msgVision));
}


CTask::~CTask()
{
}
