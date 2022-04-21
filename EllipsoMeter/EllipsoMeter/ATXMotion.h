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
	//   DIR_CCW    00h     반시계방향
	//   DIR_CW     01h     시계방향
	//[HommingSignal]
	//   PosEndLimit  00h   +Elm(End limit) +방향 리미트 센서 신호
	//   NegEndLimit  01h   -Elm(End limit) -방향 리미트 센서 신호
	//   HomeSensor   04h   IN0(ORG)  원점 센서 신호

	DWORD   dwHommingZphas;
	//! [dwZphas]
	//!    DISABLE      00h   Z상 검출 사용하지 않음
	//!    + 방향       01h   Z상 검출 +방향사용
	//!    - 방향       02h   Z상 검출 -방향사용

	float fHommingvelocity[2];
	//[0] 1차 원점센서 검출 후 재 검색할 때 사용될 구동속도
	//[1] 마지막 원점 검출시 사용될 구동속도 (Z상 검출 사용시 속도, 원점센서 정밀 검출 속도)

	BOOL bUseSensorFlag[MotorSensor::MotorSensorCnt]; // 0:+ / 1:- / 2:H

	float fSafePosition; //하드웨어적으로 박을 위험 없는 위치
	float fDangerzonePos[MAX_MOTOR_NUM][2]; //하드웨어적으로 박을 위험 위치 [0]~[1] 사이
	unsigned __int16 b_int64CrushAxis; //하드웨어적으로 박을 위험이 있는 축 선택

	float fNozzlePosGapX;
	float fNozzlePosGapY;

	void SavePara();
	void LoadPara();

	BOOL bHommingEndFlag;

	float m_fSpeedFlag;

	double		m_dOldPos;
	double		m_dOldCmdPos;				// OnTimer에서 사용할 이전 Command Position			
	double		m_dOldActPos;				// OnTimer에서 사용할 이전 Actual Position	
	double		m_dOldCmdVel;				// OnTimer에서 사용할 이전 Command Velocity

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
