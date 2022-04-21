#pragma once

#if UNICODE
#else
#define _wtof	atof
#define _wtoi	atoi
#define _wtol	atol
#endif



// #include "AJIN_CtlClass.h"
#include "AXL.h"
#include "AXM.h"
#include "AXDev.h"
#include "AXHS.h"
#include "AXD.h"

#pragma comment(lib, "AxL.lib")

#define MAX_MOTOR_NUM	4
#define MAX_Buf			128


#define ABS 0
#define INC 1

#define MOTOR_STATUS_CH_CNT		7

// CATXMotion
namespace MotorSensor
{
	enum 
	{
		UseLimitPosi = 0,
		UseLimitNega,
		UseHome,
		UseZPhase,
		MotorSensorCnt
	};
}

enum
{
	ITR_SAFE = 0,
	ITR_CAUTION,
	ITR_DANGER
};




class CATXMotion
{
public:
	CATXMotion();
	virtual ~CATXMotion();

	BOOL m_bEmergency;

	TCHAR curDirName[MAX_PATH];
	void MakeDirectory();

	BOOL MotionMode;
	BOOL InitMotionSystem();
	DWORD LoadMotFile(LPSTR szMotFile);

	DWORD StopAxisAll(bool bEStopFlag=false);
	inline void checkMessage();

	void CloseMotion(void);
};




class CATXAxis
{
public:
	CATXAxis(int iNum, int iMode = 0);
	~CATXAxis();

	TCHAR curDirName[MAX_PATH];

public:
	int AxisNo;
	BOOL MotionMode;
	TCHAR AxisName[MAX_Buf];

	float fPitch;
	float fResol;
	float fAccel;
	float fVelocity;
	float fDeccel;
	float fMicroPerPuls;
	float fSoftPosiLimit;
	float fSoftNegaLimit;
	float fContactLimit;

	long    lHommingDirection;
	DWORD   dwHommingSignal;
	//[HommingDirection] 
	//   DIR_CCW    00h     �ݽð����
	//   DIR_CW     01h     �ð����
	//[HommingSignal]
	//   PosEndLimit  00h   +Elm(End limit) +���� ����Ʈ ���� ��ȣ
	//   NegEndLimit  01h   -Elm(End limit) -���� ����Ʈ ���� ��ȣ
	//   HomeSensor   04h   IN0(ORG)  ���� ���� ��ȣ

	DWORD   dwHommingZphas;
	//! [dwZphas]
	//!    DISABLE      00h   Z�� ���� ������� ����
	//!    + ����       01h   Z�� ���� +������
	//!    - ����       02h   Z�� ���� -������

	float fHommingvelocity[2];
	//[0] 1�� �������� ���� �� �� �˻��� �� ���� �����ӵ�
	//[1] ������ ���� ����� ���� �����ӵ� (Z�� ���� ���� �ӵ�, �������� ���� ���� �ӵ�)

	BOOL bUseSensorFlag[MotorSensor::MotorSensorCnt]; // 0:+ / 1:- / 2:H

	float fSafePosition; //�ϵ���������� ���� ���� ���� ��ġ
	float fDangerzonePos[MAX_MOTOR_NUM][2]; //�ϵ���������� ���� ���� ��ġ [0]~[1] ����
	unsigned __int16 b_int64CrushAxis; //�ϵ���������� ���� ������ �ִ� �� ����

	float fNozzlePosGapX;
	float fNozzlePosGapY;

	void SavePara();
	void LoadPara();

	BOOL bHommingEndFlag;

	float m_fSpeedFlag;

	double		m_dOldPos;
	double		m_dOldCmdPos;				// OnTimer���� ����� ���� Command Position			
	double		m_dOldActPos;				// OnTimer���� ����� ���� Actual Position	
	double		m_dOldCmdVel;				// OnTimer���� ����� ���� Command Velocity

	double		m_dOffsetPos;

public:
	bool AmpEnable();
	void ServoOn(DWORD dwOnOff);
	void CheckStatus(DWORD* dwStatusMech, DWORD* dwStatusStop, DWORD* dwStatusDrive);
	void MonitoringGetPos(double* dCmdPos, double* dActPos, double* dCmdVel, BOOL bEncoder = TRUE);
	DWORD CheckAmpFault();
	DWORD ZeroSet();
	DWORD AlarmReset();
	BOOL SetSoftLimit(float PosiLimit, float NegaLimit);

	int Start_Home_Search(TCHAR* szLogMsg, BOOL CheckFlag = FALSE);
	DWORD GetResult_Home_Search(TCHAR* sResult);
	bool CheckReadyMotor();

	DWORD StopAxis(bool Estopflag = false);
	bool MoveAxis(double pos, int IncFlag, BOOL bEncorder, BOOL bOffsetCalib = TRUE);
	bool MoveAxisVel(double vel, LPSTR message);
	bool SetFlyingMode(long lMapNo, long AxisCnt, long AxisNo[], int PosCnt, double Pos[][2]);
	bool CircleMidPntMove();
	DWORD CheckInMotion();

	DWORD StatusGetPos(double* dCurPos, BOOL bModeCmd = FALSE);
	void StatusGetPosError(double* dpError);
	int CheckPosInterrupt(double Pos, float SafePos, float DangerPosSt, float DangerPosEd);
	bool CheckMotorPosition();
};



extern 	CATXMotion* pMotion;
extern	CATXAxis* pAxis[MAX_MOTOR_NUM];
