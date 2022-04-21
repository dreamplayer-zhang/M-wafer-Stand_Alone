
// stdafx.cpp : ǥ�� ���� ���ϸ� ��� �ִ� �ҽ� �����Դϴ�.
// EllipsoMeter.pch�� �̸� �����ϵ� ����� �˴ϴ�.
// stdafx.obj���� �̸� �����ϵ� ���� ������ ���Ե˴ϴ�.

#include "stdafx.h"


BOOL JoyInit(HWND hwnd)
{
	JOYINFOEX  joyinfo;
	UINT       nValid = 0;

	// �ý��ۿ� � ���̽�ƽ�� �޷ȴ��� üũ�Ѵ�.
	// ������ 0�� �ֹǷ� FALSE����
	if (joyGetNumDevs() == 0)
		return FALSE;

	memset(&joyinfo, 0, sizeof(JOYINFOEX));
	joyinfo.dwSize = sizeof(JOYINFOEX);
	joyinfo.dwFlags = JOY_RETURNBUTTONS | JOY_RETURNX |
		JOY_RETURNY;

	// 1�� ���̽�ƽ�� ����Ǿ��� �˾ƺ���.
	if (joyGetPosEx(JOYSTICKID1, &joyinfo) != JOYERR_UNPLUGGED)
		nValid = JOYSTICKID1; // �����Ǿ��ٸ� 1�� ���̽�ƽ�̶�� ���
							  // 2�� ���̽�ƽ�� ����Ǿ��� �˾ƺ���.
	else if (joyGetPosEx(JOYSTICKID2, &joyinfo) != JOYERR_UNPLUGGED)
		nValid = JOYSTICKID2;
	else
		return FALSE;  // �̵� ���� �ƴϸ� ����

	// �ش�� ���̽�ƽ ID�� ���̽�ƽ�� ������ �Ÿ��� ����
	// ������ �Ÿ����� �������� �̼��� �����ӿ��� �����Ѵ�.
	// ������ ���� �־��ָ� �� �ʹ� Ŀ�� �� �ȿ�����. ^^; Ǫ��������~
	joySetThreshold(nValid, 100);
	// ���̽�ƽ�� �޽����� ���� ������ �ڵ� ����
	joySetCapture(hwnd, nValid, 100, TRUE);

	return TRUE;
}

void Delay(double msec)
{
	// Precious MilliSeconds Delay(msec); Time Function....
	LARGE_INTEGER frequency, count1, count2;
	__int64 timeInMilliSeconds;
	msec *= 10.0;

	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
	QueryPerformanceCounter((LARGE_INTEGER*)&count1);
	while (1)
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&count2);
		timeInMilliSeconds = ((count2.QuadPart - count1.QuadPart) * 10 * CLOCKS_PER_SEC) / frequency.QuadPart;
		if (timeInMilliSeconds >= msec) break;
	}
}

//************************************
// Method:    CalculateEnergy
// FullName:  CInspFunc::CalculateEnergy
// Access:    public 
// Returns:   long
// Qualifier:
// Parameter: unsigned char * Buf_TargetImg	:	�̹��� �迭����
// Parameter: int imgSizeX					:	�̹���Xũ��
// Parameter: int imgSizeY					:	�̹���Yũ��
// Parameter: int sx						:	�˻翵�� �»�� x��ǥ
// Parameter: int sy						:	�˻翵�� �»�� y��ǥ
// Parameter: int ex						:	�˻翵�� ���ϴ� x��ǥ
// Parameter: int ey						:	�˻翵�� ���ϴ� y��ǥ
// Parameter: int maskno					:	mask �˰��� ���� �Ϲ������� Sobel���
//************************************
long CalculateEnergy(unsigned char* Buf_TargetImg, int imgSizeX, int imgSizeY, int sx, int sy, int ex, int ey, int maskno, int nStep)
{
	unsigned int mask[3][3];
	long	energy = 0L;
	double	tmpx = 0L;
	double  tmpy = 0L;
	int energyCnt = 0;
	memset(mask, 0x00, sizeof(mask));

	for (int y = sy; y <= ey; y += nStep)
	{
		for (int x = sx; x <= ex; x += nStep)
		{
			if ((x >= (imgSizeX - 15)) || (x <= 15) || (y >= (imgSizeY - 15)) || (y <= 15)) continue;

			mask[0][0] = Buf_TargetImg[((long)(x - 1) + (long)((long)imgSizeX*(long)(y - 1)))];
			mask[0][1] = Buf_TargetImg[((long)(x + 0) + (long)((long)imgSizeX*(long)(y - 1)))];
			mask[0][2] = Buf_TargetImg[((long)(x + 1) + (long)((long)imgSizeX*(long)(y - 1)))];
			mask[1][0] = Buf_TargetImg[((long)(x - 1) + (long)((long)imgSizeX*(long)(y + 0)))];
			mask[1][1] = Buf_TargetImg[((long)(x + 0) + (long)((long)imgSizeX*(long)(y + 0)))];
			mask[1][2] = Buf_TargetImg[((long)(x + 1) + (long)((long)imgSizeX*(long)(y + 0)))];
			mask[2][0] = Buf_TargetImg[((long)(x - 1) + (long)((long)imgSizeX*(long)(y + 1)))];
			mask[2][1] = Buf_TargetImg[((long)(x + 0) + (long)((long)imgSizeX*(long)(y + 1)))];
			mask[2][2] = Buf_TargetImg[((long)(x + 1) + (long)((long)imgSizeX*(long)(y + 1)))];

			if (maskno == 0)
			{	// Sobel mask				
				tmpx = (double)mask[0][0] * (-1.) + (double)mask[0][2] * (1.) +
					(double)mask[1][0] * (-2.) + (double)mask[1][2] * (2.) +
					(double)mask[2][0] * (-1.) + (double)mask[2][2] * (1.);

				tmpy = (double)mask[0][0] * (-1.) + (double)mask[0][1] * (-2.) + (double)mask[0][2] * (-1.) +
					(double)mask[2][0] * (1.) + (double)mask[2][1] * (2.) + (double)mask[2][2] * (1.);
				energy += (long)sqrt(tmpx*tmpx + tmpy*tmpy);
			}
			else if (maskno == 1)
			{	// Laplacian mask			
				tmpx = (double)mask[1][1] * 4.0 - ((double)mask[0][1] + (double)mask[1][0] +
					(double)mask[1][2] + (double)mask[2][1]);
				energy += (long)(tmpx*tmpx);
			}
			else if (maskno == 2)
			{	// SML mask		
				tmpx = (double)mask[1][0] * (-1.0) + (double)mask[1][1] * (2.0) + (double)mask[1][2] * (-1.0);
				tmpy = (double)mask[0][1] * (-1.0) + (double)mask[1][1] * (2.0) + (double)mask[2][1] * (-1.0);
				energy += (long)(tmpy*tmpy);//+tmpy*tmpy);
			}

			energyCnt++;
		}
	}

	energy = (long)sqrt(fabs((double)energy));
	return energy;
}

bool desc(pair<double, double> a, pair<double, double> b)
{
	return a.first > b.first;
}

CString m_fnGetCurrentDrive()
{
	TCHAR szCurDir[MAX_PATH + 1] = _T("");
	::GetCurrentDirectory(MAX_PATH, szCurDir);

	szCurDir[3] = NULL;
	return szCurDir;
}

void CountingImgFile(LPCTSTR directory, int bdNo, LPCTSTR FileFormat, int *pCnt, int mod)
{
	//directory\\FilenameFormat-board.tif
	// mod>>	0:�Ϲ�  1:RGB	2:ĸ��
	CFileFind finder;
	CString tmpString;
	int Cnt = 0;
	BOOL bWorking = false;

	do
	{
		Cnt++;

		if (mod == 1)
		{
			tmpString.Format(_T("%s\\%03d-%02d[R].%s"), directory, Cnt, bdNo + 1, FileFormat);
		}
		else if (mod == 2)
		{
			tmpString.Format(_T("%s\\%03d-%02d_Capture.%s"), directory, Cnt, bdNo + 1, FileFormat);
		}
		else
		{
			tmpString.Format(_T("%s\\%03d-%02d.%s"), directory, Cnt, bdNo + 1, FileFormat);
			// 			tmpString.Format(_T("%s\\%03d.%s"), directory, Cnt, FileFormat);
		}
		bWorking = finder.FindFile(tmpString);
	} while (bWorking);

	*pCnt = Cnt;

	finder.Close();
}

BOOL inCircle(int in_x, int in_y, int CircleCenterX, int CircleCenterY, float radius)
{
	return ((in_x - CircleCenterX)*(in_x - CircleCenterX) + (in_y - CircleCenterY)*(in_y - CircleCenterY) <= radius * radius) ? TRUE : FALSE;  //TRUE:����//FALSE:����
}
BOOL inCircle(double in_x, double in_y, double CircleCenterX, double CircleCenterY, double radius)
{
	return ((in_x - CircleCenterX)*(in_x - CircleCenterX) + (in_y - CircleCenterY)*(in_y - CircleCenterY) <= radius * radius) ? TRUE : FALSE;  //TRUE:����//FALSE:����
}

int ChildWindowsCounting(HWND parentHwnd)
{
	HWND tmpChildHwnd;
	int iCtrlCount = 0;

	tmpChildHwnd = ::GetWindow(parentHwnd, GW_CHILD);

	while (tmpChildHwnd)
	{
		iCtrlCount++;
		tmpChildHwnd = ::GetWindow(tmpChildHwnd, GW_HWNDNEXT);
	}
	iCtrlCount++;

	return iCtrlCount;
}

void ChildWindows(HWND parentHwnd, int CtrlCount, HWND* childHwnd)
{
	HWND	tmpChildHwnd;
	CWnd*	pTmpCwnd;
	int iTmpCount = 0;

	memset(childHwnd, 0x00, sizeof(HWND)*CtrlCount);

	tmpChildHwnd = ::GetWindow(parentHwnd, GW_CHILD);
	// 	pTmpCwnd = CWnd::FromHandle(tmpChildHwnd);
	childHwnd[iTmpCount] = tmpChildHwnd;

	while (tmpChildHwnd)
	{
		iTmpCount++;
		if (iTmpCount >= CtrlCount)
		{
			pTmpCwnd = CWnd::FromHandle(tmpChildHwnd);
			break;
		}
		tmpChildHwnd = ::GetWindow(tmpChildHwnd, GW_HWNDNEXT);
		childHwnd[iTmpCount] = tmpChildHwnd;
	}

	return;
}

void EnableDlgItem(CWnd* pDlg, BOOL bEnable, int* ArrExcepID /*= nullptr*/, int ArrExcepCnt /*= 0*/)
{
	int iTmpCtrlID = 0;
	int tmpCtrlCnt;

	HWND* pTmpHwnd;
	CWnd* pTmpCtrlWnd;

	tmpCtrlCnt = ChildWindowsCounting(pDlg->m_hWnd);
	pTmpHwnd = new HWND[tmpCtrlCnt];
	if (!pTmpHwnd)
	{
		delete[] pTmpHwnd;
		return;
	}
	ASSERT(pTmpHwnd);
	ChildWindows(pDlg->m_hWnd, tmpCtrlCnt, pTmpHwnd);

	BOOL bAplly = TRUE;
	for (int i = 0; i < tmpCtrlCnt; i++)
	{
		bAplly = TRUE;
		if (pTmpHwnd[i] == nullptr)
			continue;
		pTmpCtrlWnd = CWnd::FromHandle(pTmpHwnd[i]);
		iTmpCtrlID = pTmpCtrlWnd->GetDlgCtrlID();

		for (int j = 0; j < ArrExcepCnt; j++)
		{
			if (iTmpCtrlID == ArrExcepID[j])
				bAplly = FALSE;
		}

		if (bAplly)
			pDlg->GetDlgItem(iTmpCtrlID)->EnableWindow(bEnable);
		else
			pDlg->GetDlgItem(iTmpCtrlID)->EnableWindow(!bEnable);
	}

	delete[] pTmpHwnd;
}

CTimeInterval::CTimeInterval() // ǥ�� �������Դϴ�.
{
	memset(ChkTime, 0x00, sizeof(SYSTEMTIME) * 2);
}

CTimeInterval::~CTimeInterval()
{

}

SYSTEMTIME CTimeInterval::SetTimeStart()
{
	GetLocalTime(&ChkTime[0]);
	return ChkTime[0];
}

SYSTEMTIME CTimeInterval::SetTimeEnd()
{
	GetLocalTime(&ChkTime[1]);
	return ChkTime[1];
}

CString CTimeInterval::GetStrTime(SYSTEMTIME ntime)
{
	CString strTime;
	int subHour;
	if (ntime.wHour >= 12)
	{
		subHour = ntime.wHour == 12 ? 0 : 12;
		strTime.Format("%s %02d:%02d:%02d", "����", ntime.wHour - subHour, ntime.wMinute, ntime.wSecond);
	}
	else
	{
		subHour = ntime.wHour == 0 ? 12 : 0;
		strTime.Format("%s %02d:%02d:%02d", "����", ntime.wHour + subHour, ntime.wMinute, ntime.wSecond);
	}

	return strTime;
}

double CTimeInterval::GetTimeInterval(int Unit /*= 1*/, BOOL AutoEndCheck /*= TRUE*/)
{
	if (AutoEndCheck)
		GetLocalTime(&ChkTime[1]);

	__int64 dTm1, dTm2;

	// 		ULONGLONG *ullVal1, *ullVal2;
	// 		ULONGLONG ullDif;
	double TimeInterval;
	SystemTimeToFileTime(&ChkTime[0], (LPFILETIME)&dTm1);
	SystemTimeToFileTime(&ChkTime[1], (LPFILETIME)&dTm2);

	// 		ullVal1 = (ULONGLONG *)&dTm1;
	// 		ullVal2 = (ULONGLONG *)&dTm2;

	// 		ullDif = *ullVal2 - *ullVal1;  //100 ���� ������ ����

	__int64 dTDif;
	dTDif = dTm2 - dTm1;

	switch (Unit)
	{
	case 0:
		TimeInterval = double(dTDif);	//100�����ʴ�����
		break;
	case 1:
		TimeInterval = (double)dTDif / 10000.0;			//�и��ʴ�����
		break;
	case 2:
		TimeInterval = double(dTDif / 10000) / 1000;	//�ʴ�����
		break;
	default:
		TimeInterval = double(dTDif / 10000);			//�и��ʴ�����
		break;
	}

	dItvTime = TimeInterval;
	return TimeInterval;

	//��ó: https://3dmpengines.tistory.com/1051 [3DMP]
}
