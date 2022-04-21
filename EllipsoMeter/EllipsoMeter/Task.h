#pragma once

#define VISION_NAME										_T("VISION_ALIGN")
#define MAX_ALIGN_POINT									100
#define DEF_IMAGE_X										2000		//���� ���� ���� ������
#define DEF_IMAGE_Y										2000		//���� ���� ���� ������

class CTask
{
public:
	CTask();
	~CTask();

	CDPoint cdTargetPos;
	double fAlign_X;
	double fAlign_Y;
	double fAlign_T;


	// �Լ����ۿ�û�ÿ� 1, ���� �Ϸ�ÿ� 0
	int iSetMasterPatt;
	int	iSetMotorAlign;
	int	iGetPattAlignCenter;
	int	iGetTransCoordinate;
	// �Լ����ۿ�û�ÿ� 1, ���� �Ϸ�ÿ� 0, ����ã�� ���и� 2

	int iGroupNum;

	unsigned char ImageBuff[MAX_ALIGN_POINT][DEF_IMAGE_X*DEF_IMAGE_Y];// [������̿��ǰ�� 0������ ���] [ī�޶� ��������� ����]
	int PatternNum;

	double XPixArray[MAX_ALIGN_POINT];
	double YPixArray[MAX_ALIGN_POINT];
	double XWorldArray[MAX_ALIGN_POINT];
	double YWorldArray[MAX_ALIGN_POINT];
	int NumPoint;

	CDPoint AlignCenter; //CDPoint Class�� Stdafx.h CommonStruct.h ��Ŭ��� ���ٿ� �־��.

	double inX;
	double inY;
	double OutX;
	double OutY;
	int tansformType;

	int ErrorCode;
	TCHAR msgVision[512];

};

extern CTask*              pTask;
