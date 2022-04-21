#include "stdafx.h"
#include "CommThread.h"

HWND hCommWnd;


CCommThread *pCommTh[SR_CNT];

int LightControl(int portNo, int chNo, int iLightVal)
{
	CString StrSend = _T("");

	if (portNo < 0 || chNo < 1)
		return FALSE;


#ifdef LVS
	StrSend.Format(_T(":1%d%04d%c%c"), chNo, iLightVal, CR, LF);
	//2020년 이전
	//StrSend.Format(_T("L%d%03d%c%c"), chNo, iLightVal, CR, LF); //노말모드컨트롤러
	//StrSend.Format(_T("S%d%04d%c%c"), chNo, iLightVal, CR, LF); //스트로브컨트롤러

#else

#ifdef VIT
	pCommTh[portNo]->csLightWrite[0].Lock();
	StrSend.Format(_T("C%d%03d%c%c"), chNo, iLightVal, CR, LF);

#else //HSP(백두)
	// 	int i;
	// 	for (i = 0x00; i <= 0xaa; i++)
	// 	{
	// 		if (i > 0x2a && i < 0x80) 
	// 			i = 0x80;
	// 		StrSend.Format(_T("%02x%02x%02x"), 0x5b, i, 0x00);
	// 		pCommTh[portNo]->nMakeHexData(StrSend);
	// 	}
	// 	return 0;

	int edge;
	if (chNo < 9)
		edge = 0x5b * 0x10000 + 0x28 * 0x100 + 0x00;
	else
		edge = 0x5b * 0x10000 + 0xa8 * 0x100 + 0x00;

	if		(chNo == 1) chNo = 0x00;//= 0x80;//
	else if (chNo == 2) chNo = 0x02;//= 0x82;//
	else if (chNo == 3) chNo = 0x04;//= 0x04;//
	else if (chNo == 4) chNo = 0x06;//= 0x84;//
	else if (chNo == 5) chNo = 0x08;//= 0x86;//
	else if (chNo == 6) chNo = 0x0a;//= 0x0a;//
	else if (chNo == 7) chNo = 0x0c;//= 0x88;//
	else if (chNo == 8) chNo = 0x0e;//= 0x8a;//
	else if (chNo == 9) chNo = 0x80;//= 0x8c;//

									//iLightVal : 0~99
	iLightVal /= 100;
	iLightVal = iLightVal > 99 ? 99 : iLightVal;

	StrSend.Format(_T("%02x%02x%02x%02x%02x%02x%06x")// % 02x % 02x % 02x % 02x % 02x % 02x")
		, 0x5b, chNo, 0x08, 0x5b, chNo + 0x01, iLightVal
		, edge
		// 		, 0x5b, chNo + 0x10, 0x04, 0x5b, chNo + 0x11, 5
		);
	// 	StrSend.Format(_T("%02x%02x%02x%02x%02x%02x"), 0x5b, chNo, 0x08, 0x5b, chNo + 0x01, iLightVal);


	int bufPos = 0;
	bufPos = pCommTh[portNo]->nMakeHexData(StrSend);
	pCommTh[portNo]->csLightWrite[0].Unlock();
	return bufPos;

#endif //_VIT

#endif //_LVS
	return pCommTh[portNo]->nMakeAsciiData(StrSend);

}

void CQueue::Clear()
{
	m_iHead = m_iTail =0;
	memset(buff,0,Q_BUFF_SIZE);
}
CQueue::CQueue()
{
	Clear();
}
int CQueue::GetSize()
{
	return (m_iHead - m_iTail + Q_BUFF_SIZE) % Q_BUFF_SIZE;
}
BOOL CQueue::PutByte(BYTE b)
{
	if(GetSize() == (Q_BUFF_SIZE-1)) return FALSE;
	buff[m_iHead++] =b;
	m_iHead %= Q_BUFF_SIZE;
	return TRUE;
}
BOOL CQueue::GetByte(BYTE *pb)
{
	if(GetSize() == 0) return FALSE;
	*pb = buff[m_iTail++];
	m_iTail %= Q_BUFF_SIZE;
	return TRUE;
}
//--- 클래스 생성자
CCommThread::CCommThread(int Port)
{
	m_iPortNum = Port;
	//--> 초기는 당연히..포트가 열리지 않은 상태여야겠죠?
	m_bConnected = FALSE;

	m_hThreadWatchComm = NULL;

	m_hComm = NULL;
}

CCommThread::~CCommThread()
{

}


// 포트 sPortName을 dwBaud 속도로 연다.
// ThreadWatchComm 함수에서 포트에 무언가 읽혔을 때 MainWnd에 알리기
// 위해 WM_COMM_READ메시지를 보낼때 같이 보낼 wPortID값을 전달 받는다.
BOOL CCommThread::OpenPort(CString strPortName, 
					   DWORD dwBaud, BYTE byData, BYTE byStop, BYTE byParity )
{

	// Local 변수.
		COMMTIMEOUTS	timeouts;
		DCB				dcb;
// 		DWORD			dwThreadID;
		
		// overlapped structure 변수 초기화.
		m_osRead.Offset = 0;
		m_osRead.OffsetHigh = 0;
		//--> Read 이벤트 생성에 실패..
		if ( !(m_osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL)) ) 	
		{
			return FALSE;
		}
	

		m_osWrite.Offset = 0;
		m_osWrite.OffsetHigh = 0;
		//--> Write 이벤트 생성에 실패..
		if (! (m_osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL)))
		{
			return FALSE;
		}
		
		//--> 포트명 저장..
		m_sPortName = strPortName;
	
		//--> 실제적인...RS 232 포트 열기..
		m_hComm = CreateFile( m_sPortName, 
							  GENERIC_READ | GENERIC_WRITE, 0, NULL,
							  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 
							  NULL);

	
		//--> 포트 열기에 실해하면..
		if (m_hComm == (HANDLE) -1)
		{
			strPortName += L" Port Open Failed";
// 			AfxMessageBox(strPortName);
			TCHAR tmpStr[512];
			sprintf_s(tmpStr, _countof(tmpStr), _T("%s"), strPortName.GetBuffer());
			TECmsgOut(m_Util.CStringTobstr(tmpStr), LIGHTMAGENTA, 1000);
			return FALSE;
		}
	

	//===== 포트 상태 설정. =====

	// EV_RXCHAR event 설정...데이터가 들어오면.. 수신 이벤트가 발생하게끔..
	SetCommMask( m_hComm, EV_RXCHAR);	

	// InQueue, OutQueue 크기 설정.
	SetupComm( m_hComm, Q_BUFF_SIZE, Q_BUFF_SIZE);	

	// 포트 비우기.
	PurgeComm( m_hComm,					
			   PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);

	// timeout 설정.
	timeouts.ReadIntervalTimeout = 0xFFFFFFFF;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	
	timeouts.WriteTotalTimeoutMultiplier = 2*CBR_9600 / dwBaud;
	timeouts.WriteTotalTimeoutConstant = 0;
	
	SetCommTimeouts( m_hComm, &timeouts);

	// dcb 설정.... 포트의 실제적인..제어를 담당하는 DCB 구조체값 셋팅..
	dcb.DCBlength = sizeof(DCB);

	//--> 현재 설정된 값 중에서..
	GetCommState( m_hComm, &dcb);	
	
	//--> 보드레이트를 바꾸고..
	dcb.BaudRate = dwBaud;
	

	//--> Data 8 Bit
	dcb.ByteSize = byData;

	//--> Noparity
	dcb.Parity = byParity;

	//--> 1 Stop Bit
	dcb.StopBits = byStop;


	//--> 포트를 재..설정값으로.. 설정해보고..
	if( !SetCommState( m_hComm, &dcb) )	
	{
		return FALSE;
	}


// 	// 포트 감시 쓰레드 생성.
	m_bConnected = TRUE;
// 
// 	//--> 포트 감시 쓰레드 생성.
//  	m_hThreadWatchComm = CreateThread( NULL, 0, 
// 									   (LPTHREAD_START_ROUTINE)ThreadWatchComm, 
// 									   this, 0, &dwThreadID);

// 	//--> 쓰레드 생성에 실패하면..
// 	if (! m_hThreadWatchComm)
// 	{
// 		//--> 열린 포트를 닫고..
// 		ClosePort();
// 		return FALSE;
// 	}
	check = FALSE;

	return TRUE;
}
	
// 포트를 닫는다.
void CCommThread::ClosePort()
{
	LightControl(0, 1, 0);


	//--> 연결되지 않았음.
	m_bConnected = FALSE;
	
	//--> 마스크 해제..
	SetCommMask( m_hComm, 0);
	
	//--> 포트 비우기.
	PurgeComm( m_hComm,	PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);
	
	CloseHandle(m_hComm);
	//--> 핸들 닫기
	
	Sleep(0);
	while (1)
	{
		if (m_hThreadWatchComm == NULL)
		{
			break;
		}
	}

}

// 포트에 pBuff의 내용을 nToWrite만큼 쓴다.
// 실제로 쓰여진 Byte수를 리턴한다.
DWORD CCommThread::WriteComm(BYTE *pBuff, DWORD nToWrite)
{
	DWORD	dwWritten, dwError, dwErrorFlags;
	COMSTAT	comstat;

	//--> 포트가 연결되지 않은 상태이면..
	if( !m_bConnected )		
	{
		return 0;
	}


	//--> 인자로 들어온 버퍼의 내용을 nToWrite 만큼 쓰고.. 쓴 갯수를.,dwWrite 에 넘김.
	if( !WriteFile( m_hComm, pBuff, nToWrite, &dwWritten, &m_osWrite))
	{
		//--> 아직 전송할 문자가 남았을 경우..
		if (GetLastError() == ERROR_IO_PENDING)
		{
			// 읽을 문자가 남아 있거나 전송할 문자가 남아 있을 경우 Overapped IO의
			// 특성에 따라 ERROR_IO_PENDING 에러 메시지가 전달된다.
			//timeouts에 정해준 시간만큼 기다려준다.
			while (! GetOverlappedResult( m_hComm, &m_osWrite, &dwWritten, TRUE))
			{
				dwError = GetLastError();
				if (dwError != ERROR_IO_INCOMPLETE)
				{
					ClearCommError( m_hComm, &dwErrorFlags, &comstat);
					break;
				}
			}
		}
		else
		{
			dwWritten = 0;
			ClearCommError( m_hComm, &dwErrorFlags, &comstat);
		}
	}



	//--> 실제 포트로 쓰여진 갯수를 리턴..
	return dwWritten;
}


BOOL CCommThread::WriteCommByte(unsigned char ucByte)
{
	BOOL bWriteStat;
	DWORD dwBytesWritten;

	bWriteStat = WriteFile(m_hComm, (LPSTR)&ucByte, 1, &dwBytesWritten, &m_osWrite);
	if (!bWriteStat && (GetLastError() == ERROR_IO_PENDING)) {
		if (WaitForSingleObject(m_osWrite.hEvent, 1000)) dwBytesWritten = 0;
		else {
			GetOverlappedResult(m_hComm, &m_osWrite, &dwBytesWritten, FALSE);
			m_osWrite.Offset += dwBytesWritten;
		}
	}

	return(TRUE);
}

// 포트로부터 pBuff에 nToWrite만큼 읽는다.
// 실제로 읽혀진 Byte수를 리턴한다.
DWORD CCommThread::ReadComm(BYTE *pBuff, DWORD nToRead)
{
	DWORD	dwRead,dwError, dwErrorFlags;
	COMSTAT comstat;

	//--- system queue에 도착한 byte수만 미리 읽는다.
	ClearCommError( m_hComm, &dwErrorFlags, &comstat);

	//--> 시스템 큐에서 읽을 거리가 있으면..
	dwRead = comstat.cbInQue;
	if(dwRead > 0)
	{

		//--> 버퍼에 일단 읽어들이는데.. 만일..읽어들인값이 없다면..

		if( !ReadFile( m_hComm, pBuff, nToRead, &dwRead, &m_osRead) )
		{
			//--> 읽을 거리가 남았으면..
			if (GetLastError() == ERROR_IO_PENDING)
			{
				//--------- timeouts에 정해준 시간만큼 기다려준다.
				while (! GetOverlappedResult( m_hComm, &m_osRead, &dwRead, TRUE))
				{
					dwError = GetLastError();
					if (dwError != ERROR_IO_INCOMPLETE)
					{
						ClearCommError( m_hComm, &dwErrorFlags, &comstat);
						break;
					}
				}
			}
			else
			{
				dwRead = 0;
				ClearCommError( m_hComm, &dwErrorFlags, &comstat);
			}
		}
	}


	//--> 실제 읽어들인 갯수를 리턴.
	return dwRead;

}





CString CCommThread::byIndexComPort(int xPort)
{
	CString PortName;
	switch (xPort)
	{
	case 0:		PortName = "COM1"; 			break;

	case 1:		PortName = "COM2";			break;

	case 2:		PortName = "COM3"; 			break;

	case 3:		PortName = "COM4";			break;

	case 4:		PortName = "COM5"; 	break;

	case 5:		PortName = "COM6";	break;

	case 6:		PortName = "\\\\.\\COM7"; 	break;

	case 7:		PortName = "COM8";	break;

	case 8:		PortName = "COM9"; 	break;

	case 9:		PortName = "\\\\.\\COM10";	break;

	case 12:	PortName = "COM13"; 	break;

	}



	return PortName;
}

DWORD CCommThread::byIndexBaud(int xBaud)
{
	DWORD dwBaud;
	switch (xBaud)
	{
	case 0:		dwBaud = CBR_4800;		break;

	case 1:		dwBaud = CBR_9600;		break;

	case 2:		dwBaud = CBR_14400;		break;

	case 3:		dwBaud = CBR_19200;		break;

	case 4:		dwBaud = CBR_38400;		break;

	case 5:		dwBaud = CBR_56000;		break;

	case 6:		dwBaud = CBR_57600;		break;

	case 7:		dwBaud = CBR_115200;	break;
	}

	return dwBaud;
}

BYTE CCommThread::byIndexData(int xData)
{
	BYTE byData;
	switch (xData)
	{
	case 0:	byData = 5;			break;

	case 1:	byData = 6;			break;

	case 2:	byData = 7;			break;

	case 3:	byData = 8;			break;
	}
	return byData;
}

BYTE CCommThread::byIndexStop(int xStop)
{
	BYTE byStop;
	if (xStop == 0)
	{
		byStop = ONESTOPBIT;
	}
	else
	{
		byStop = TWOSTOPBITS;
	}
	return byStop;
}

BYTE CCommThread::byIndexParity(int xParity)
{
	BYTE byParity;
	switch (xParity)
	{
	case 0:	byParity = NOPARITY;	break;

	case 1:	byParity = ODDPARITY;	break;

	case 2:	byParity = EVENPARITY;	break;
	}

	return byParity;
}

BYTE CCommThread::byCode2AsciiValue(char cData)
{
	//이 함수는 char문자를 hex값으로 변경해 주는 함수 입니다.
	BYTE byAsciiValue;
	if (('0' <= cData) && (cData <= '9'))
	{
		byAsciiValue = cData - '0';
	}
	else if (('A' <= cData) && (cData <= 'F'))
	{
		byAsciiValue = (cData - 'A') + 10;
	}
	else if (('a' <= cData) && (cData <= 'f'))
	{
		byAsciiValue = (cData - 'a') + 10;
	}
	else
	{
		byAsciiValue = 0;
	}
	return byAsciiValue;
}

int CCommThread::nMakeHexData(CString strSend)
{
	csCommTh.Lock();

	int bufPos = 0;
	int datasize=0, bufsize=0, i, j;
	BYTE *Send_buff, byHigh=0, byLow=0;
	CString byGetData = _T("");

	byGetData = strSend; //컨트롤 맴버 변수를 받는다.
	byGetData.Replace(_T(" "), _T(""));// 공백 없애기 
	byGetData.Replace(_T("\r\n"), _T(""));//엔터 없애기
	datasize = byGetData.GetLength(); // 공백을 없앤 문자열 길이 얻기 

									  // 문자 길이를 %2로 나눈 값이 0이 아니면 홀수 이기 때문에 마지막 문자를 처리 해줘야 함
	if (datasize % 2 == 0)
	{
		bufsize = datasize;
	}
	else
	{
		bufsize = datasize - 1;
	}

	Send_buff = new BYTE[bufsize];

	for (i = 0; i < bufsize; i += 2)
	{
		byHigh = byCode2AsciiValue((char)byGetData[i]);
		byLow = byCode2AsciiValue((char)byGetData[i + 1]);
		Send_buff[bufPos++] = (byHigh << 4) | byLow;

	}
	//마지막 문자가 1자리수 일때 처리 하기 위해  예) 1 -> 01
	if (datasize % 2 != 0)
	{
		Send_buff[bufPos++] = byCode2AsciiValue((char)byGetData[datasize - 1]);
	}

	int etc = bufPos % 8;
	//포트에 데이터를 8개씩 쓰기 위해
	//데이터의 길이가 8의 배수가 아니면 나머지 데이터는 따로 보내줌
	for (j = 0; j < bufPos - etc; j += 8)//8의 배수 보냄
	{
		WriteComm(&Send_buff[j], 8);
	}
	if (etc != 0)//나머지 데이터 전송
	{
		WriteComm(&Send_buff[bufPos - etc], etc);// 포트에 데이터 쓰기 
	}
	delete[] Send_buff;

	csCommTh.Unlock();

	return bufPos;
}

int CCommThread::nMakeAsciiData(CString strSend)
{
	// 	strSend += _T("\r\n");
	int nSize = strSend.GetLength();
	int bufPos = 0;
	BYTE *Send_buff;
	Send_buff = new BYTE[nSize];
	for (int i = 0; i < nSize; i++)//ASCII 데이터 이기 때문에 버퍼에 그대로 저장
	{
		Send_buff[bufPos++] = (BYTE)strSend[i];
	}

	int etc = bufPos % 8;
	//포트에 데이터를 8개씩 쓰기 위해
	//데이터의 길이가 8의 배수가 아니면 나머지 데이터는 따로 보내줌
	for (int j = 0; j < bufPos - etc; j += 8)
	{
		WriteComm(&Send_buff[j], 8);
	}
	if (etc != 0)//나머지 데이터 전송
	{
		WriteComm(&Send_buff[bufPos - etc], etc);
	}
	delete[] Send_buff;
	return nSize;
}

// 포트를 감시하고, 읽힌 내용이 있으면
// m_ReadData에 저장한 뒤에 MainWnd에 메시지를 보내어 Buffer의 내용을
// 읽어가라고 신고한다.

DWORD	ThreadWatchComm(CCommThread* pComm)
{
   DWORD           dwEvent;
   OVERLAPPED      os;
   BOOL            bOk = TRUE;
   BYTE            buff[204800];      // 읽기 버퍼
   DWORD           dwRead;  // 읽은 바이트수.
   DWORD           dwBuffSize;  

   CString		tmpEndStr = _T("");
   WORD			i = 0;
   BOOL			bContinueRead = TRUE;

   // Event, OS 설정.
   memset( &os, 0, sizeof(OVERLAPPED));
   
   //--> 이벤트 설정..
   if( !(os.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL)) )
   {
		bOk = FALSE;
   }

   //--> 이벤트 마스크..
   if( !SetCommMask( pComm->m_hComm, EV_RXCHAR) )
   {
	   bOk = FALSE;
   }

   //--> 이벤트나..마스크 설정에 실패함..
   if( !bOk )
   {
		AfxMessageBox(_T("Error while creating ThreadWatchComm, ") + pComm->m_sPortName);
		return FALSE;
   }
  
   while (pComm ->m_bConnected)//포트가 연결되면 무한 루프에 들어감
   {
 		dwEvent = 0;
	
        // 포트에 읽을 거리가 올때까지 기다린다.
        WaitCommEvent( pComm->m_hComm, &dwEvent, NULL);
	
	
		//--> 데이터가 수신되었다는 메세지가 발생하면..
        if ((dwEvent & EV_RXCHAR) == EV_RXCHAR)
        {
            // 포트에서 읽을 수 있는 만큼 읽는다.
				//--> buff 에 받아놓고..
			do
			{
				dwRead = pComm->ReadComm( buff, 2048); //들어온 데이터 읽어 오기 
				dwBuffSize = pComm->m_QueueRead.GetSize();
				if(Q_BUFF_SIZE - dwBuffSize > (int)dwRead)
				{
// 					i = 0;
// 					bContinueRead = TRUE;
// 					while (bContinueRead)
// 					{
// 						pComm->m_QueueRead.PutByte(buff[i]);//큐 버퍼에  들어온 데이터 넣기 
// 						tmpEndStr.Format(_T("%c%c"), buff[i-1], buff[i]);
// 						if (tmpEndStr == _T("\r\n") || i>Q_BUFF_SIZE)
// 							bContinueRead = FALSE;
// 						i++;
// 					}
					for(i = 0; i < dwRead; i++ )
					{
						pComm->m_QueueRead.PutByte(buff[i]);//큐 버퍼에  들어온 데이터 넣기 
					}
				}
				else
					AfxMessageBox("buff full"); //큐버퍼의 크기를 초과하면 경고 메시지 보냄
			}while(dwRead);
			::PostMessage(hCommWnd, WM_COMM_READ, pComm->m_iPortNum, 0);//CSerialComDlg로 데이터가 들어왔다는 메시지를 보냄
		}
		Sleep(0);	// 받은 데이터를 화면에 보여줄 시간을 벌기 위해.
					// 데이터를 연속으로 받으면 cpu점유율이 100%가 되어 화면에 뿌려주는 작업이 잘 안되고. 결과적으로 
					// 큐 버퍼에 데이터가 쌓이게 됨
   }
   
  CloseHandle( os.hEvent);

   //--> 쓰레드 종료가 되겠죠?
   pComm->m_hThreadWatchComm = NULL;

   return TRUE;

}
