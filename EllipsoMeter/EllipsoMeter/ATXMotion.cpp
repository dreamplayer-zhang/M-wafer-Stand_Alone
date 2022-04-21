// ATXMotion.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "ATXMotion.h"

// CATXMotion
// #ifdef _DEBUG
// #define new DEBUG_NEW
// #undef THIS_FILE
// static char THIS_FILE[] = __FILE__;
// #endif

CATXMotion* pMotion;
CATXAxis* pAxis[MAX_MOTOR_NUM];

CATXAxis::CATXAxis(int iNum, int iMode /*= 0*/)
	: MotionMode(FALSE)
{
	AxisNo = iNum;
	MotionMode = iMode;

	fPitch = fResol = fAccel = fVelocity = fDeccel = 100.f;
	bHommingEndFlag = FALSE;
	lHommingDirection = DIR_CCW;
	dwHommingSignal = PosEndLimit;

	fHommingvelocity[0] = fHommingvelocity[1] = 10.f;

	fSafePosition = 0.f; //�ϵ���������� ���� ���� ���� ��ġ [0]~[1] ����

	memset(&fDangerzonePos, 0, sizeof(float)*MAX_MOTOR_NUM * 2);
	//fDangerzonePos[0]=fDangerzonePos[1]=0.f; //�ϵ���������� ���� ���� ���� ��ġ [0]~[1] ����

	b_int64CrushAxis = 0x00;

	fNozzlePosGapX = 0;
	fNozzlePosGapY = 0;

	m_fSpeedFlag = 1.0f;

	bUseSensorFlag[0] = true;
	bUseSensorFlag[1] = bUseSensorFlag[2] = false;

	fSoftPosiLimit = 380001;
	fSoftNegaLimit = -380001;

	m_dOffsetPos = 0.0;
}

CATXAxis::~CATXAxis()
{
// 	SavePara();
}






CATXMotion::CATXMotion()
{
	MotionMode = 1;

	GetCurrentDirectory(MAX_PATH, curDirName); // Current File Path...
	MakeDirectory();

	m_bEmergency = FALSE;

	for (int i=0; i<MAX_MOTOR_NUM; i++)
		pAxis[i] = (CATXAxis*)new CATXAxis(i, TRUE);
}

CATXMotion::~CATXMotion()
{
	for (int i = MAX_MOTOR_NUM-1; i >= 0; i--)
	{
		delete pAxis[i];
		pAxis[i] = nullptr;
	}
}

void CATXMotion::MakeDirectory()
{
	TCHAR tmpStr[MAX_PATH];

	sprintf_s(tmpStr,sizeof(TCHAR)*MAX_PATH, _T("%s\\Data\\MotionPara\\"), curDirName);
	CreateDirectory(tmpStr, NULL);

// 	sprintf_s(tmpStr, sizeof(TCHAR)*MAX_Buf, _T("%s\\Data\\MotionPara\\%04d"), curDirName, 603);
// 	CreateDirectory(tmpStr, NULL);
// 	sprintf_s(tmpStr, sizeof(TCHAR)*MAX_Buf, _T("%s\\Data\\MotionPara\\%04d"), curDirName, 1005);
// 	CreateDirectory(tmpStr, NULL);
// 	sprintf_s(tmpStr, sizeof(TCHAR)*MAX_Buf, _T("%s\\Data\\MotionPara\\%04d"), curDirName, 1608);
// 	CreateDirectory(tmpStr, NULL);
// 	sprintf_s(tmpStr, sizeof(TCHAR)*MAX_Buf, _T("%s\\Data\\MotionPara\\%04d"), curDirName, 2012);
// 	CreateDirectory(tmpStr, NULL);
// 	sprintf_s(tmpStr, sizeof(TCHAR)*MAX_Buf, _T("%s\\Data\\MotionPara\\%04d"), curDirName, 3216);
// 	CreateDirectory(tmpStr, NULL);
// 	sprintf_s(tmpStr, sizeof(TCHAR)*MAX_Buf, _T("%s\\Data\\MotionPara\\%04d"), curDirName, 3225);
// 	CreateDirectory(tmpStr, NULL);

}

BOOL CATXMotion::InitMotionSystem()
{
	if(!MotionMode)	
		return true;

// 	//++ AXL(AjineXtek Library)�� ��밡���ϰ� �ϰ� ������ ������� �ʱ�ȭ�մϴ�.
// 	if ((AxlOpen(7) != AXT_RT_SUCCESS)){
// 		AfxMessageBox(_T("AxlOpen Fail..."));
// 		return FALSE;
// 	}


	char szFilePath[MAX_Buf];
	sprintf_s(szFilePath, "%s%s%s%s", m_Util.m_fnGetCurrentDrive().GetString(), DEFAULT_PATH, DEFAULT_ENV_PATH, DEFAULT_MOT_FILE);
	if (LoadMotFile(szFilePath) != AXT_RT_SUCCESS)
		return FALSE;


	//������ ���� ������ ���� ����
	int axis=0;
	DWORD dwRetrun;
	for(axis=0; axis<MAX_MOTOR_NUM; axis++)
	{
		dwRetrun = AxmMotSetAccelUnit(axis, UNIT_SEC2);
		if(dwRetrun!= AXT_RT_SUCCESS)
		{
			AfxMessageBox(_T("Accl;Unit Fail..."));
			return FALSE;
		}
	}

	return TRUE;
}

DWORD CATXMotion::LoadMotFile(LPSTR szMotFile)
{
	if (!MotionMode)
		return 0;

	DWORD MotPara[40];
	memset(MotPara, 0x00, sizeof(DWORD) * 40);
	// �� [CAUTION] �Ʒ��� �ٸ� Mot����(��� ��������)�� ����� ��� ��θ� �����Ͻʽÿ�.

	DWORD dwRet = 0;
	//++ ������ Mot������ ��������� ��Ǻ����� ���������� �ϰ����� �����մϴ�.
	dwRet = AxmMotLoadParaAll(szMotFile);
	if (dwRet != AXT_RT_SUCCESS)
	{
		AfxMessageBox("Mot File Not Found.");
	}

	return dwRet;
}

DWORD CATXMotion::StopAxisAll(bool bEStopFlag/*=false*/)
{
	if(!MotionMode)	
		return 0;

	DWORD dwRtn = 0;
// 	m_bEmergency = bEStopFlag;
	for (int i = 0; i < MAX_AXIS; i++)
	{
		if (bEStopFlag)
			dwRtn = AxmMoveEStop(i);
		else
			dwRtn = AxmMoveSStop(i);

		if (dwRtn != AXT_RT_SUCCESS)
		{
			return dwRtn;
		}
// 		dwRtn = AxmSignalServoAlarmReset(i, TRUE);
// 		if (dwRtn != AXT_RT_SUCCESS)
// 		{
// 			return dwRtn;
// 		}
// 		Delay(200);
// 		dwRtn = AxmSignalServoAlarmReset(i, FALSE);
// 		if (dwRtn != AXT_RT_SUCCESS)
// 		{
// 			return dwRtn;
// 		}
	}

	return 0;
}

inline void CATXMotion::checkMessage()
{
	MSG msg;
	//	PeekMessage(&msg,::GetActiveWindow(),NULL,NULL,PM_REMOVE);
	PeekMessage(&msg,NULL,NULL,NULL,PM_REMOVE);
	DispatchMessage(&msg);
}

void CATXMotion::CloseMotion(void)
{
	if (!MotionMode)
		return;
	//++ AXL ����� �����մϴ�.
	AxlClose();
}

void CATXAxis::SavePara()
{
	int i=0,j=0,k=0;

	CString strINIPath;
	CString strSection, strKey, strValue;

	FILE *out;
	TCHAR tmpStr[MAX_Buf];

	sprintf_s(tmpStr, sizeof(TCHAR)*MAX_PATH, (_T("%s\\Data\\MotionPara\\MotorPara_%dAxis.ini")), curDirName, AxisNo);

	if (fopen_s(&out, tmpStr, _T("w")) != 0)
	{
		AfxMessageBox(_T("Failure to save motor parameter !!!"));
	}
	else fclose(out);

	strINIPath.Format(tmpStr);

	strSection.Format(_T("Axis_%02d"),AxisNo);

	strKey = _T("AxisName");
	strValue.Format(_T("%s"),AxisName);
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("LeadPitch");
	strValue.Format(_T("%f"),fPitch);
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("MotorResol");
	strValue.Format(_T("%f"),fResol);
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("ACC");
	strValue.Format(_T("%f"),fAccel);
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("Vel");
	strValue.Format(_T("%f"),fVelocity);
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("DCC");
	strValue.Format(_T("%f"),fDeccel);
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("MicroPerPulse");
	strValue.Format(_T("%f"),fMicroPerPuls);
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("SoftPosiLimit");
	strValue.Format(_T("%f"),fSoftPosiLimit);
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("SoftNegaLimit");
	strValue.Format(_T("%f"), fSoftNegaLimit);
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("ContactLimit");
	strValue.Format(_T("%f"), fContactLimit);
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("UseSensorPosi");
	strValue.Format(_T("%d"), bUseSensorFlag[MotorSensor::UseLimitPosi]);
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("UseSensorNega");
	strValue.Format(_T("%d"), bUseSensorFlag[MotorSensor::UseLimitNega]);
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("UseSensorHome");
	strValue.Format(_T("%d"), bUseSensorFlag[MotorSensor::UseHome]);
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("HommingDirection");
	strValue.Format(_T("%d"), lHommingDirection);
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("HommingSignal");
	strValue.Format(_T("%d"), dwHommingSignal);
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("HommingZPhase");
	strValue.Format(_T("%d"), dwHommingZphas);
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("Hommingvelocity_2nd");
	strValue.Format(_T("%f"), fHommingvelocity[0]);
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("Hommingvelocity_Last");
	strValue.Format(_T("%f"), fHommingvelocity[1]);
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("fSafePosition");
	strValue.Format(_T("%f"), fSafePosition);
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	for (i=0; i<MAX_MOTOR_NUM; i++)
	{
		strKey.Format(_T("fDangerzonePos_%d_StartPos"), i);
		strValue.Format(_T("%f"), fDangerzonePos[i][0]);
		WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

		strKey.Format(_T("fDangerzonePos_%d_EndPos"), i);
		strValue.Format(_T("%f"), fDangerzonePos[i][1]);
		WritePrivateProfileString(strSection, strKey, strValue, strINIPath);
	}
	
	strKey = _T("b_int64CrushAxis");
	strValue.Format(_T("%d"), b_int64CrushAxis);
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("NozzleXPosGapFromFirst");
	strValue.Format(_T("%f"), fNozzlePosGapX);
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

	strKey = _T("NozzleYPosGapFromFirst");
	strValue.Format(_T("%f"), fNozzlePosGapY);
	WritePrivateProfileString(strSection, strKey, strValue, strINIPath);

}

void CATXAxis::LoadPara()
{
	int i=0,j=0,k=0;

	CString strINIPath;
	CString strSection, strKey, strValue;

	TCHAR szBuf[MAX_Buf]={0,};

	FILE *in;
	TCHAR tmpStr[MAX_Buf];
	sprintf_s(tmpStr, sizeof(TCHAR)*MAX_Buf, (_T("%s\\Data\\MotionPara\\MotorPara_%dAxis.ini")), curDirName, AxisNo);

	if (fopen_s(&in, tmpStr, _T("r")) != 0)
	{
		SavePara();
	}
	else fclose(in);

	strINIPath.Format(tmpStr);

	strSection.Format(_T("Axis_%02d"),AxisNo);

	strKey = _T("AxisName");
	GetPrivateProfileString(strSection, strKey, _T(""), AxisName, MAX_Buf, strINIPath);

	strKey = _T("LeadPitch");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_Buf, strINIPath);
	strValue.Format(_T("%s"),szBuf);
	fPitch = (float)_wtof(strValue);

	strKey = _T("MotorResol");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_Buf, strINIPath);
	strValue.Format(_T("%s"),szBuf);
	fResol = (float)_wtof(strValue);

	strKey = _T("ACC");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_Buf, strINIPath);
	strValue.Format(_T("%s"),szBuf);
	fAccel = (float)_wtof(strValue);

	strKey = _T("Vel");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_Buf, strINIPath);
	strValue.Format(_T("%s"),szBuf);
	fVelocity = (float)_wtof(strValue);

	strKey = _T("DCC");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_Buf, strINIPath);
	strValue.Format(_T("%s"),szBuf);
	fDeccel = (float)_wtof(strValue);

	strKey = _T("MicroPerPulse");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_Buf, strINIPath);
	strValue.Format(_T("%s"),szBuf);
	fMicroPerPuls = (float)_wtof(strValue);

	strKey = _T("SoftPosiLimit");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_Buf, strINIPath);
	strValue.Format(_T("%s"),szBuf);
	fSoftPosiLimit = (float)_wtof(strValue);

	strKey = _T("SoftNegaLimit");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_Buf, strINIPath);
	strValue.Format(_T("%s"),szBuf);
	fSoftNegaLimit = (float)_wtof(strValue);

	strKey = _T("ContactLimit");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_Buf, strINIPath);
	strValue.Format(_T("%s"),szBuf);
	fContactLimit = (float)_wtof(strValue);

	strKey = _T("UseSensorPosi");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_Buf, strINIPath);
	strValue.Format(_T("%s"),szBuf);
	bUseSensorFlag[0] = _wtoi(strValue);

	strKey = _T("UseSensorNega");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_Buf, strINIPath);
	strValue.Format(_T("%s"),szBuf);
	bUseSensorFlag[1] = _wtoi(strValue);

	strKey = _T("UseSensorHome");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_Buf, strINIPath);
	strValue.Format(_T("%s"),szBuf);
	bUseSensorFlag[2] = _wtoi(strValue);

	strKey = _T("HommingDirection");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_Buf, strINIPath);
	strValue.Format(_T("%s"),szBuf);
	lHommingDirection = _wtol(strValue);

	strKey = _T("HommingSignal");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_Buf, strINIPath);
	strValue.Format(_T("%s"),szBuf);
	dwHommingSignal = _wtol(strValue);

	strKey = _T("HommingZPhase");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_Buf, strINIPath);
	strValue.Format(_T("%s"),szBuf);
	dwHommingZphas = _wtol(strValue);

	strKey = _T("Hommingvelocity_2nd");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_Buf, strINIPath);
	strValue.Format(_T("%s"),szBuf);
	fHommingvelocity[0] = (float)_wtof(strValue);

	strKey = _T("Hommingvelocity_Last");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_Buf, strINIPath);
	strValue.Format(_T("%s"),szBuf);
	fHommingvelocity[1] = (float)_wtof(strValue);

	strKey = _T("fSafePosition");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_Buf, strINIPath);
	strValue.Format(_T("%s"),szBuf);
	fSafePosition = (float)_wtof(strValue);

	for (i=0; i<MAX_MOTOR_NUM; i++)
	{
		strKey.Format(_T("fDangerzonePos_%d_StartPos"), i);
		GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_Buf, strINIPath);
		strValue.Format(_T("%s"),szBuf);
		fDangerzonePos[i][0] = (float)_wtof(strValue);

		strKey.Format(_T("fDangerzonePos_%d_EndPos"), i);
		GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_Buf, strINIPath);
		strValue.Format(_T("%s"),szBuf);
		fDangerzonePos[i][1] = (float)_wtof(strValue);
	}

	strKey = _T("b_int64CrushAxis");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_Buf, strINIPath);
	strValue.Format(_T("%s"),szBuf);
	b_int64CrushAxis = (unsigned __int16)_wtol(strValue);

	strKey = _T("NozzleXPosGapFromFirst");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_Buf, strINIPath);
	strValue.Format(_T("%s"),szBuf);
	fNozzlePosGapX = (float)_wtof(strValue);

	strKey = _T("NozzleYPosGapFromFirst");
	GetPrivateProfileString(strSection, strKey, _T(""), szBuf, MAX_Buf, strINIPath);
	strValue.Format(_T("%s"),szBuf);
	fNozzlePosGapY = (float)_wtof(strValue);
}

void CATXAxis::ServoOn( DWORD dwOnOff )
{
	if(!MotionMode)	
		return;

// 	DWORD level;
// 	AxmSignalIsServoOn(AxisNo, &level);

	//++ ���� ���� Servo On/Off ��ȣ�� ����մϴ�.
	AxmSignalServoOn(AxisNo, dwOnOff);		
}

DWORD CATXAxis::AlarmReset()
{
	DWORD dwRtn = 0;
	DWORD dwUse = 0;

	dwRtn = AxmSignalReadServoAlarm(AxisNo, &dwUse);
	if (dwRtn != AXT_RT_SUCCESS)
		return dwRtn;

	if (dwUse != 0)
	{
		dwRtn = AxmSignalServoAlarmReset(AxisNo, ENABLE);
		if (dwRtn != AXT_RT_SUCCESS)
			return dwRtn;

		Delay(300);

		dwRtn = AxmSignalServoAlarmReset(AxisNo, DISABLE);
		if (dwRtn != AXT_RT_SUCCESS)
			return dwRtn;
	}

	return 0;
}

void CATXAxis::CheckStatus( DWORD* dwStatusMech, DWORD* dwStatusStop, DWORD* dwStatusDrive )
{
	if(!MotionMode)	
		return;
// 	EnterCriticalSection(&mot_cs[0]);

	DWORD	dwRetCode, dwStatus = 0;
	(*dwStatusMech) = (*dwStatusStop) = (*dwStatusDrive) = 0;

	//++ ���� ���� Mechanical Signal Data(���� ������� ��ȣ����)�� Ȯ���մϴ�.
	dwRetCode = AxmStatusReadMechanical(AxisNo, &dwStatus);
	if(dwRetCode == AXT_RT_SUCCESS)
	{
		(*dwStatusMech) = dwStatus;
	}

	//++ ���� ���� End Status(���� ����)�� Ȯ���մϴ�.
	dwRetCode = AxmStatusReadStop(AxisNo, &dwStatus);
	if(dwRetCode == AXT_RT_SUCCESS)
	{
		(*dwStatusStop) = dwStatus;
	}

	//++ ���� ���� Drive Status(����� ����)�� Ȯ���մϴ�.
	dwRetCode = AxmStatusReadMotion(AxisNo, &dwStatus);
	if(dwRetCode == AXT_RT_SUCCESS)
	{
		(*dwStatusDrive) = dwStatus;
	}

// 	LeaveCriticalSection(&mot_cs[0]);
}

DWORD CATXAxis::CheckInMotion()
{
	if(!MotionMode)	
		return true;

	DWORD Status;

	DWORD dwRet = AxmStatusReadInMotion(AxisNo, &Status);
	if ( dwRet != AXT_RT_SUCCESS )
		return false;

	return Status;
}

DWORD CATXAxis::CheckAmpFault()
{
	if(!MotionMode)	
		return false;
	DWORD dwRet = 0;
	DWORD use;

	dwRet = AxmSignalGetServoAlarm(AxisNo, &use);
	if ( dwRet != AXT_RT_SUCCESS )
	{		
		return dwRet;
	}
	if ( use == UNUSED )
	{
		//! ��� ����		
		return use;
	}

	DWORD upStatus;
	// ���� �˶� ��ȣ Ȯ��
	dwRet = AxmSignalReadServoAlarm(AxisNo, &upStatus);
	if ( dwRet != AXT_RT_SUCCESS )
	{
		return dwRet;
	}

	return upStatus;
}

DWORD CATXAxis::StatusGetPos( double* dCurPos, BOOL bModeCmd /*= FALSE*/)
{
	if(!MotionMode)	
	{
		(*dCurPos) = 0.0;
		return 0;
	}

	DWORD dwRet = 0;
	if (bModeCmd)
	{
		//++ ������ ���� ����(Command)��ġ�� ��ȯ�մϴ�.
		dwRet = AxmStatusGetCmdPos(AxisNo, dCurPos);
		(*dCurPos) = *dCurPos/(fResol/fPitch)/1000;//mm����ȯ��
	}
	else
	{
		//++ ������ ���� ����(Feedback)��ġ�� ��ȯ�մϴ�.
		dwRet = AxmStatusGetActPos(AxisNo, dCurPos);
		(*dCurPos) = *dCurPos/(fResol/fPitch)/1000;//mm����ȯ��
	}

	*dCurPos -= m_dOffsetPos;

	if (AxisNo == Z_AXIS)
		*dCurPos /= 10;

	return dwRet;
}

bool CATXAxis::AmpEnable( )
{
	if(!MotionMode)	
		return false;
	DWORD level = ENABLE;
	DWORD dwRtn = 0;

	dwRtn = AxmSignalIsServoOn(AxisNo, &level);
	if (dwRtn != AXT_RT_SUCCESS)
		return false;
	if ( level == ENABLE )					// �̹� ���� ON �����̸� �ٷ� return;
		return true;

	dwRtn = AxmSignalServoOn(AxisNo, ENABLE);
	if (dwRtn != AXT_RT_SUCCESS)
		return false;

	dwRtn = AxmSignalIsServoOn(AxisNo, &level);
	if (dwRtn != AXT_RT_SUCCESS)
		return false;
	if ( level != ENABLE )
		return false;

	return true;
}

DWORD CATXAxis::StopAxis(bool Estopflag /*= false*/)
{
	DWORD dwRtn = 0;

	if(!MotionMode)	
		return 0;

	if (Estopflag)
	{
		dwRtn = AxmMoveEStop(AxisNo);
	} 
	else
	{
		dwRtn = AxmMoveSStop(AxisNo);
	}

	return dwRtn == 0 ? TRUE: FALSE;
}

// bool CATXAxis::MoveAxis(double pos, int IncFlag, BOOL bEncorder, LPSTR message, int msgSize /*= 128*/)
bool CATXAxis::MoveAxis(double pos, int IncFlag, BOOL bEncorder, BOOL bOffsetCalib /*= TRUE*/)
{
	if(!MotionMode)	
	{
		return false;
	}

	if (pMotion->m_bEmergency)
		return false;
// 	EnterCriticalSection(&mot_cs[2]);

// 	if(!bHommingEndFlag)	
// 	{
// 		wsprintf(message, _T("No Motor Homming!!!"));
// 		if(TECmsgOut(_T("Start Motor Home?"), LIGHTBLUE, 2)==TRUE)
// 			Start_Home_Search(message);
// 		return false;
// 	}
	AxmMotSetAbsRelMode(AxisNo, 0); //�����ǥ�� ������ �����ϰ�� ��ġ ��߳�(�����غ��� ����) ==> �׻� ������ǥ��� �̵�

	if (CheckInMotion())
	{
		return false;
	}

	double  dCmdPos, szdCurPos;

	if (IncFlag == 0)
	{//������ǥ�϶�
		if (bOffsetCalib)//����������
			pos += m_dOffsetPos;
	}

	pos = pos * 1000;//mm���� ȯ��

	if (bEncorder==FALSE)
	{//���ڴ� ���� ��
		AxmStatusGetCmdPos(AxisNo, &szdCurPos);//Ŀ�����ġ��
		if (AxisNo == Z_AXIS)
			szdCurPos /= 10;
	} 
	else
	{
		AxmStatusGetCmdPos(AxisNo, &dCmdPos);//Ŀ�����ġ��
		AxmStatusGetActPos(AxisNo, &szdCurPos);//���ڴ���ġ��
		if (AxisNo == Z_AXIS)
		{
			dCmdPos /= 10;
			szdCurPos /= 10;
		}

		pos += (dCmdPos-szdCurPos)/(fResol/fPitch);
	}


	if(IncFlag==0)
	{	// ���� ��ǥ
		if(fSoftPosiLimit*1000<pos)
		{
// 			if(message != NULL)
// 				sprintf_s(message, msgSize, _T("Software Limit[Plus]"));
// 			LeaveCriticalSection(&mot_cs[2]);
			return false;
		}
		if(fSoftNegaLimit*1000>pos)
		{
// 			if (message != NULL)
// 				sprintf_s(message, msgSize, _T("Software Limit[Minus]"));
// 			LeaveCriticalSection(&mot_cs[2]);
			return false;
		}
	}
	else
	{	// ��� ��ǥ +����
		if(pos>0.0)
		{
			if(fSoftPosiLimit*1000<szdCurPos/(fResol/fPitch)+pos)
			{
// 				if (message != NULL)
// 					sprintf_s(message, msgSize, _T("Software Limit[Plus]"));
// 				LeaveCriticalSection(&mot_cs[2]);
				return false;
			}
		}
		else
		{
			if(fSoftNegaLimit*1000>szdCurPos/(fResol/fPitch)+pos)
			{
// 				if (message != NULL)
// 					sprintf_s(message, msgSize, _T("Software Limit[Minus]"));
// 				LeaveCriticalSection(&mot_cs[2]);
				return false;
			}
		}
	}


	DWORD	dwRetCode;
	double	dMovePos, dMoveVel, dMoveAcc, dMoveDec;

	if (IncFlag == 0)
	{//������ǥ
		dMovePos = (double)pos*(fResol/fPitch);
	}
	else
	{//�����ǥ
		dMovePos = (double)pos*(fResol/fPitch) + szdCurPos;
	}

	if (AxisNo == Z_AXIS)
	{
		fAccel = fDeccel = fVelocity * 2.f;
	}
	else
	{
		fAccel = fDeccel = fVelocity * 3.f;
	}


	dMoveVel = fVelocity*1000;
	dMoveAcc = fAccel*1000;
 	dMoveDec = fDeccel*1000;
	//dMoveDec = fAccel;

	dMoveVel *= m_fSpeedFlag;
	dMoveAcc *= m_fSpeedFlag; 
	dMoveDec *= m_fSpeedFlag; 

	if (AxisNo == Z_AXIS)
	{
		dMovePos *= 10;
		dMoveVel *= 10;
		dMoveAcc *= 10;
		dMoveDec *= 10;
	}
	else
	{
		if (dMoveVel < 50000)
		{
			dMoveAcc = dMoveDec = 50000.0f * 3.f;
		}
	}

	//++ ������ ���� ������ �Ÿ�(�Ǵ� ��ġ)/�ӵ�/���ӵ�/���ӵ��� ��Ǳ����ϰ� ��� ���Ῡ�ο� ������� �Լ��� �������ɴϴ�.
	dwRetCode = AxmMoveStartPos(AxisNo, dMovePos, dMoveVel, dMoveAcc, dMoveDec);
	if(dwRetCode != AXT_RT_SUCCESS)
	{
// 		if (message != NULL)
// 			sprintf_s(message, msgSize, _T("AxmMoveStartPos return error[Code:%04d]"), dwRetCode);
		return false;
	}
	
	return true;
}

bool CATXAxis::MoveAxisVel(double vel, LPSTR message)
{
	if (pMotion->m_bEmergency)
		return false;

	DWORD	dwRetCode;
	double	dMoveVel = 0.0, dMoveAcc = 0.0, dMoveDec = 0.0;

	dMoveAcc = fAccel * 1000;
	dMoveDec = fDeccel * 1000;
	//dMoveDec = fAccel;
	dMoveVel = vel * 1000;

	if (AxisNo == Z_AXIS)
	{
		dMoveVel *= 10;
		dMoveAcc *= 10;
		dMoveDec *= 10;
	}


// 	if (CheckInMotion())
// 	{
// 		if(dMoveVel >= 0)
// 			AxmOverrideSetMaxVel(AxisNo, 150000);
// 		else
// 			AxmOverrideSetMaxVel(AxisNo, -150000);
// 
// 		dwRetCode = AxmOverrideVel(AxisNo, dMoveVel);
// 	}
// 	else
	{
		dwRetCode = AxmMoveVel(AxisNo, dMoveVel, dMoveAcc, dMoveDec);
	}


	if (dwRetCode != AXT_RT_SUCCESS)
	{
		if (message != NULL)
// 			sprintf_s(message, sizeof(message), _T("AxmMoveVel return error[Code:%04d]"), dwRetCode);
			sprintf_s(message, sizeof(message), _T("[%04d]"), dwRetCode);
		return false;
	}

	return true;
}

int CATXAxis::Start_Home_Search(   TCHAR* szLogMsg, BOOL CheckFlag /*= FALSE*/ )
{	
	// homming ����
	// 1. Table ��Ÿ���� ������ ����
	// 2. 1,2,6,7,8 home
	// 3. 4 home
	// 4. 4 �߰� ��ġ �̵�
	// 5. 3,5 home
	// 6. 5 �߰� ��ġ �̵�

	if(!MotionMode)	
		return 1;
	bHommingEndFlag = FALSE;

// 	int inpositionCnt;
	DWORD dwReturn = 0;

	long    lHmDir = 1;
	DWORD   lHmsig =  4;
	DWORD   dwZphas = 0;
	double  dwHClrTim = 2000.0;
	double  dwHOffset = 0.0;

	//! ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//! �����˻� ��� �������� �˻� �������, �������� ����� ��ȣ,���ڴ� Z�� ���� ���ε��� ���� �Ѵ�. 
	//! [lHmDir] 
	//!    DIR_CCW    00h     �ݽð����
	//!    DIR_CW     01h     �ð����
	//! [lHmsig]
	//!    PosEndLimit  00h   +Elm(End limit) +���� ����Ʈ ���� ��ȣ
	//!    NegEndLimit  01h   -Elm(End limit) -���� ����Ʈ ���� ��ȣ
	//!    HomeSensor   04h   IN0(ORG)  ���� ���� ��ȣ
	//! [dwZphas]
	//!    DISABLE      00h   Z�� ���� ������� ����
	//!    + ����       01h   Z�� ���� +������
	//!    - ����       02h   Z�� ���� -������
	lHmDir = lHommingDirection;			//! �����˻��� �ʱ⿡ ������ ���⼳��
	lHmsig = dwHommingSignal;		//! ���� �˻��� ��� �� ��ȣ����

	dwZphas = dwHommingZphas;			//! 1�� �����˻� �Ϸ� �� ���ڴ� Z�� ���� ���� ����
	dwHClrTim = 2000.0;			//! �����˻� �Ϸ� �� ������ġ�� Encoder��ġ�� Clear�ϱ� ���� ����ϴ� �ð����� [mSec����]
	dwHOffset = 0.0;			//! �����˻��� �Ϸ�� �� �ⱸ �������� �̵� �� ���� �缳�� �� ��ġ

	dwReturn = AxmHomeSetMethod(AxisNo, lHmDir, lHmsig, dwZphas, dwHClrTim, dwHOffset);
	if ( dwReturn != AXT_RT_SUCCESS )
	{
		return -3;
	}

	double dVelFirst = 0.;
	double dVelSecond = 0.;
	double dVelThird = 0.;
	double dvelLast = 0.;
	double dAccFirst = 0.;
	double dAccSecond = 0.;

	//! ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//! ������ ������ �����ϰ� �˻��ϱ� ���� ���� �ܰ��� �������� �����Ѵ�. 
	//! �̶� �� ���ܿ� ��� �� �ӵ��� �����Ѵ�. 
	//! �� �ӵ����� �������� ���� �����˻� �ð���, �����˻� ���е��� �����ȴ�. 
	//! �� ���� �� �ӵ����� ������ �ٲ㰡�鼭 �� ���� �����˻� �ӵ��� �����ϸ� �ȴ�.

	//! (�ӵ��� ������ Unit/pulse�� 1/1�� �Ѱ�쿡 PPS[Pulses/sec]) 
	dVelFirst	= fVelocity*1000/2.0;				//! ���������� �����Ǿ� �������� �� �����˻������������ ������ �ӵ�
	dVelSecond	= fVelocity*1000/4.0;				//! ���������� �����Ǿ� ���� �� �����˻� ���� �ݴ� �������� ���� �� �ӵ�
	dVelThird	= fHommingvelocity[0]*1000;			//! 1�� �������� ���� �� �� �˻��� �� ���� �����ӵ�
	dvelLast	= fHommingvelocity[1]*1000;			//! ������ ���� ����� ���� �����ӵ� (Z�� ���� ���� �ӵ�, �������� ���� ���� �ӵ�)

	//! (���ӵ��� ������ Unit/pulse�� 1/1�� �Ѱ�쿡 PPS[Pulses/sec^2])
	dAccFirst = fAccel*1000;				//! ���������� �����Ǿ� �������� �� �����˻� ���� �������� ������ �� ���� ����/���ӵ�
	dAccSecond = fAccel*1000*2;				//! ���������� �����Ǿ� ���� �� �����˻� ���� �ݴ� �������� ������ �� ���� ����/���ӵ�

// 	if (AxisNo == AXIS_PICK_Theta)
// 	{
// 		dVelFirst	= fVelocity > 10 ? 10*1000/2.0 : fVelocity*1000/2.0; 
// 		dVelSecond	= fVelocity > 10 ? 10*1000/4.0 : fVelocity*1000/4.0; 
// 	}

	dwReturn = AxmHomeSetVel(AxisNo, dVelFirst, dVelSecond, dVelThird, dvelLast, dAccFirst, dAccSecond);
	if ( dwReturn != AXT_RT_SUCCESS )
	{
		return -4;
	}

	//! ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//! �����˻� ���� �Լ��� ��� �ϸ� ���̺귯�� ���ο��� �ش� ���� �����˻��� ���������� ���� �� �� �ڵ� ���� �ȴ�.
	//! �����˻��Լ��� �����ϸ� ���ο��� �ڵ����� ���ο� �����带 ����� �����˻��� �����Ѵ�. 
	//! [���� ����] �������� �ݴ������ ����Ʈ ������ ���͵� ��������� ������ ACTIVE���������� �����Ѵ�.
	//!            ���� �˻��� ���۵Ǿ� ��������� ����Ʈ ������ ������ ����Ʈ ������ �����Ǿ��ٰ� �����ϰ� �����ܰ�� ����ȴ�.

	dwReturn = AxmHomeSetStart(AxisNo);
	if (dwReturn != AXT_RT_SUCCESS )
	{
		GetResult_Home_Search(szLogMsg);
// 		TECmsgOut(m_Util.ConverCtoWC(szLogMsg), LIGHTRED, 1);
		AfxMessageBox(szLogMsg, MB_ICONERROR);
		return -5;
	}

	dwReturn = 0;

	while(CheckFlag)
	{
		Sleep(0);
		dwReturn = GetResult_Home_Search(szLogMsg);
		if (dwReturn == HOME_SUCCESS)
		{
			bHommingEndFlag = TRUE;
			break;
		}
		else if (dwReturn != HOME_SEARCHING)
		{
// 			TECmsgOut(m_Util.ConverCtoWC(szLogMsg), LIGHTRED, 1);
			AfxMessageBox(szLogMsg, MB_ICONERROR);
			return -5;
		}
	}

	return 1;
}

DWORD CATXAxis::GetResult_Home_Search(  TCHAR* sResult)
{
	if(!MotionMode)	
		return true;

	DWORD uHomeResult;
	bool bRet = false;

	DWORD level = ENABLE;
	AxmSignalIsServoOn(AxisNo, &level);
	if ( level != ENABLE )
	{
		sprintf_s(sResult, sizeof(sResult), _T("Check ServoOn Signal."));
		return -1;
	}

	AxmHomeGetResult(AxisNo, &uHomeResult);


	switch(uHomeResult)
	{
	case HOME_SUCCESS:
		{
			sprintf_s(sResult, sizeof(sResult), _T("Ȩ �Ϸ�"));
			bRet = true;
		}
		break;
	case HOME_SEARCHING:
		{
			sprintf_s(sResult, sizeof(sResult), _T("Ȩ �˻� ��"));
		}
		break;
	case HOME_ERR_GNT_RANGE:
		{
			sprintf_s(sResult, sizeof(sResult), _T("Ȩ �˻� ���� ��Ż"));
		}
		break;
	case HOME_ERR_USER_BREAK:
		{
			sprintf_s(sResult, sizeof(sResult), _T("Ȩ �˻� �� ����ڰ� ���� ��� ����"));
		}
		break;
	case HOME_ERR_VELOCITY:
		{
			sprintf_s(sResult, sizeof(sResult), _T("Ȩ �˻��� �ӵ� ���� �� �� 0���� �۰ų� ���� ���� ����"));
		}
		break;
	case HOME_ERR_AMP_FAULT:
		{
			sprintf_s(sResult, sizeof(sResult), _T("������ �˶��� �߻�"));
		}
		break;
	case HOME_ERR_NEG_LIMIT:
		{
			sprintf_s(sResult, sizeof(sResult), _T("(+)�������� Ȩ ���� �˻� �� (-)����Ʈ ���� ����"));
		}
		break;
	case HOME_ERR_POS_LIMIT:
		{
			sprintf_s(sResult, sizeof(sResult), _T("(-)�������� Ȩ ���� �˻� �� (+)����Ʈ ���� ����"));
		}
		break;
	case HOME_ERR_NOT_DETECT:
		{
			sprintf_s(sResult, sizeof(sResult), _T("Ȩ ���� ���� �Ұ�"));
		}
		break;
	case HOME_ERR_UNKNOWN:
		{
			sprintf_s(sResult, sizeof(sResult), _T("�˼� ���� ä��(��)��ȣ(0 ~ 16)�� Ȩ �˻� ����"));
		}
		break;
	default:
		{
			sprintf_s(sResult, sizeof(sResult), _T("Error"));
		}
		break;
	}//! switch(uHomeResult)

	return uHomeResult;//bRet;
}

DWORD CATXAxis::ZeroSet()
{
	if(!MotionMode)	
		return 1;

	return AxmStatusSetPosMatch(AxisNo, 0.0) == 0? TRUE: FALSE;
// 	bHommingEndFlag = FALSE;
}

void CATXAxis::StatusGetPosError(  double* dpError)
{
	double	dValue = 0;

	AxmStatusReadPosError(AxisNo, &dValue);
	(*dpError) = dValue;
	(*dpError) = dValue/(fResol/fPitch)/1000;
}


int CATXAxis::CheckPosInterrupt(double Pos, float SafePos, float DangerPosSt, float DangerPosEd)
{
	if (Pos > (SafePos-0.f) && Pos < (SafePos+0.f))
	{
		return ITR_SAFE;
	}
	else if (Pos > DangerPosSt || Pos < DangerPosEd)
	{
		return ITR_DANGER;
	}

	return ITR_CAUTION;
}

bool CATXAxis::CheckReadyMotor()
{
// 	EnterCriticalSection(&mot_cs[4]);

	TCHAR tmpStr[MAX_Buf];
	if(!bHommingEndFlag)
	{
		sprintf_s(tmpStr, sizeof(TCHAR)*MAX_Buf, _T("AXIS%02d Did Not Homming!"), AxisNo);
		// 			putListData(&pViewThis->m_ListBox_Report, tmpStr);
// 		LeaveCriticalSection(&mot_cs[4]);
		return false;
	}
	if(CheckInMotion())
	{
		sprintf_s(tmpStr, sizeof(TCHAR)*MAX_Buf, _T("AXIS%02d Moving Now!"), AxisNo);
		// 			putListData(&pViewThis->m_ListBox_Report, tmpStr);
// 		LeaveCriticalSection(&mot_cs);
		return false;
	}
// 	LeaveCriticalSection(&mot_cs[4]);
	return true;
}

bool CATXAxis::CheckMotorPosition()
{
// 	EnterCriticalSection(&mot_cs[5]);

// 	TCHAR tmpStr[MAX_Buf];
// 	double  dCmdPos, dActPos, dCmdVel;

// 	StatusGetPos(&dCmdPos, &dActPos, &dCmdVel);
// 
// // 	if( fabs((float)(dCmdPos - fSafePosition)) > 500 )
// // 	{
// // 		sprintf_s(tmpStr, sizeof(TCHAR)*MAX_Buf, _T("Axis %d : Out of SafeZone!"), AxisNo);
// // 		// 		putListData(&pViewThis->m_ListBox_Report, tmpStr);
// //		LeaveCriticalSection(&mot_cs);
// // 		return false;
// // 	}
// 	if( (dCmdPos > fDangerzonePos[0]) && (dCmdPos < fDangerzonePos[1]) )
// 	{
// 		sprintf_s(tmpStr, sizeof(TCHAR)*MAX_Buf, _T("Axis %d : In DangerZone!(Current Position:%.03f"), AxisNo+1, (float)dCmdPos);
// 		TECmsgOut(tmpStr, LIGHTRED, 1);
// // 		LeaveCriticalSection(&mot_cs[5]);
// 		return false;
// 	}
// // 	LeaveCriticalSection(&mot_cs[5]);
	return true;
}


BOOL CATXAxis::SetSoftLimit( float PosiLimit, float NegaLimit )
{//long lAxisNo, DWORD uUse, DWORD uStopMode, DWORD uSelection, double dPositivePos, double dNegativePos
 //							0:E-stop 1:S-stop   0:ComPos 1:ActPos
	DWORD dwReturn;

	////mm����ȯ��
	PosiLimit = PosiLimit*1000*(fResol/fPitch);
	NegaLimit = NegaLimit*1000*(fResol/fPitch);

	if (AxisNo == Z_AXIS)
	{
		PosiLimit *= 10;
		NegaLimit *= 10;
	}

	dwReturn = AxmSignalSetSoftLimit(AxisNo, ENABLE, 0, 0, PosiLimit, NegaLimit);

	if (dwReturn != AXT_RT_SUCCESS)
	{
		return false;
	}
	return true;
}

bool CATXAxis::CircleMidPntMove()
{
// 	double  dCmdPos, dActPos, dCmdVel;
// 
// 	long    lAxis[2];
// 	double  dMidPos[2];   
// 	double  dEndPos[2];
// 	long    lPosSize = 2;
// 	long  lCoordinate = 0;
// 
// 	double  dVelocity = 200;
// 	double  dAccel = 400;
// 	dVelocity	*= 1000;
// 	dAccel		*= 1000;
// 
// 	lAxis[0] = AXIS_PICK_X;
// 	lAxis[1] = AXIS_PICK_Y;
// 
// 	dMidPos[0] = pWork->SeqPos[SEQ_ALIGN_POINT].AixsPos[lAxis[0]]	*1000*(pMotion[lAxis[0]]->fResol/pMotion[lAxis[0]]->fPitch);
// 	dMidPos[1] = pWork->SeqPos[SEQ_ALIGN_POINT].AixsPos[lAxis[1]]	*1000*(pMotion[lAxis[1]]->fResol/pMotion[lAxis[1]]->fPitch);		
// 
// 	dEndPos[0] = pWork->SeqPos[SEQ_CDF_PLACE].AixsPos[lAxis[0]]	*1000*(pMotion[lAxis[0]]->fResol/pMotion[lAxis[0]]->fPitch);
// 	dEndPos[1] = pWork->SeqPos[SEQ_CDF_PLACE].AixsPos[lAxis[1]]	*1000*(pMotion[lAxis[1]]->fResol/pMotion[lAxis[1]]->fPitch);
// 
// 	AxmContiWriteClear(lCoordinate);
// 	AxmContiSetAxisMap(lCoordinate, lPosSize, lAxis);
// 	AxmContiSetAbsRelMode(lCoordinate, 0);
// 
// 	AxmCirclePointMove(lCoordinate, lAxis, dMidPos, dEndPos, dVelocity, dAccel, dAccel, 0);
// 
// 	while(1)
// 	{
// 		pMotion[AXIS_PICK_X]->StatusGetPos(&dCmdPos, &dActPos, &dCmdVel);
// 
// 		if ((float)dActPos >= pWork->SeqPos[SEQ_ALIGN_POINT].AixsPos[lAxis[0]])
// 		{
// 			MdigGrab(pVision->MilDigitizer[DEF_ALIGN_CAM], pVision->MilBuf_A_Cam_Grab_Image);
// 			pVision->Trigout(DEF_ALIGN_CAM);
// 			MdigGrabWait(pVision->MilDigitizer[DEF_ALIGN_CAM], M_GRAB_END);
// 			MbufCopy(pVision->MilBuf_A_Cam_Grab_Image, pVision->MilBuf_A_Cam_Insp_Image);
// 
// 			MbufExport(_T("C:\\Test\\FlyAlign.tif"), M_TIFF, pVision->MilBuf_A_Cam_Insp_Image);
// 			break;
// 		}
// 	}
	return true;
}

bool CATXAxis::SetFlyingMode( long lMapNo, long AxisCnt, long AxisNo[], int PosCnt, double Pos[][2] )
{
	int i;
	long  lCoordinate	= lMapNo;
	long  lPosSize		= AxisCnt;

	double  dVelocity	=0;
	double  dAccel		=0;
	double  dDeccel		=0;

	AxmContiWriteClear(lCoordinate);
	AxmContiSetAxisMap(lCoordinate, lPosSize, AxisNo);
	AxmContiSetAbsRelMode(lCoordinate, 0);

	AxmContiBeginNode(lCoordinate); 

	for (i=0; i<(PosCnt-1); i++)
	{
		AxmCirclePointMove(lCoordinate, AxisNo, Pos[i], Pos[i+1], dVelocity, dAccel, dDeccel, DIR_CCW);
	}

	AxmContiEndNode(lCoordinate);
// 	AxmContiSetAbsRelMode(lCoordinate, 0);
// 	AxmContiStart(lCoordinate, 0, 0);

	return true;

}

void CATXAxis::MonitoringGetPos(double* dCmdPos, double* dActPos, double* dCmdVel, BOOL bEncoder/*=TRUE*/)
{
	if(!MotionMode)	
	{
		(*dCmdPos) = 0.0;//+AxisNo*0.783;
		(*dActPos) = 0.0;//+AxisNo*1.341;
		(*dCmdVel) = 0.0;
		return;
	}

	//++ ������ ���� ����(Command)��ġ�� ��ȯ�մϴ�.
	AxmStatusGetCmdPos(AxisNo, dCmdPos);
	(*dCmdPos) = *dCmdPos/(fResol/fPitch)/1000;//mm����ȯ��

	if (AxisNo == Z_AXIS)
		*dCmdPos /= 10;


	//++ ������ ���� ����(Feedback)��ġ�� ��ȯ�մϴ�.
	AxmStatusGetActPos(AxisNo, dActPos);
	(*dActPos) = *dActPos/(fResol/fPitch)/1000;//mm����ȯ��
	if (AxisNo == Z_AXIS)
		*dActPos /= 10;

	if (!bEncoder)
	{
		*dActPos = *dCmdPos;
	}

	//++ ������ ���� ���� �ӵ��� ��ȯ�մϴ�.
	AxmStatusReadVel(AxisNo, dCmdVel);
	(*dCmdVel) = *dCmdVel/(fResol/fPitch)/1000;//mm����ȯ��
	if (AxisNo == Z_AXIS)
		*dCmdVel /= 10;

}
