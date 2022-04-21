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
	//2020�� ����
	//StrSend.Format(_T("L%d%03d%c%c"), chNo, iLightVal, CR, LF); //�븻�����Ʈ�ѷ�
	//StrSend.Format(_T("S%d%04d%c%c"), chNo, iLightVal, CR, LF); //��Ʈ�κ���Ʈ�ѷ�

#else

#ifdef VIT
	pCommTh[portNo]->csLightWrite[0].Lock();
	StrSend.Format(_T("C%d%03d%c%c"), chNo, iLightVal, CR, LF);

#else //HSP(���)
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
//--- Ŭ���� ������
CCommThread::CCommThread(int Port)
{
	m_iPortNum = Port;
	//--> �ʱ�� �翬��..��Ʈ�� ������ ���� ���¿��߰���?
	m_bConnected = FALSE;

	m_hThreadWatchComm = NULL;

	m_hComm = NULL;
}

CCommThread::~CCommThread()
{

}


// ��Ʈ sPortName�� dwBaud �ӵ��� ����.
// ThreadWatchComm �Լ����� ��Ʈ�� ���� ������ �� MainWnd�� �˸���
// ���� WM_COMM_READ�޽����� ������ ���� ���� wPortID���� ���� �޴´�.
BOOL CCommThread::OpenPort(CString strPortName, 
					   DWORD dwBaud, BYTE byData, BYTE byStop, BYTE byParity )
{

	// Local ����.
		COMMTIMEOUTS	timeouts;
		DCB				dcb;
// 		DWORD			dwThreadID;
		
		// overlapped structure ���� �ʱ�ȭ.
		m_osRead.Offset = 0;
		m_osRead.OffsetHigh = 0;
		//--> Read �̺�Ʈ ������ ����..
		if ( !(m_osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL)) ) 	
		{
			return FALSE;
		}
	

		m_osWrite.Offset = 0;
		m_osWrite.OffsetHigh = 0;
		//--> Write �̺�Ʈ ������ ����..
		if (! (m_osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL)))
		{
			return FALSE;
		}
		
		//--> ��Ʈ�� ����..
		m_sPortName = strPortName;
	
		//--> ��������...RS 232 ��Ʈ ����..
		m_hComm = CreateFile( m_sPortName, 
							  GENERIC_READ | GENERIC_WRITE, 0, NULL,
							  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 
							  NULL);

	
		//--> ��Ʈ ���⿡ �����ϸ�..
		if (m_hComm == (HANDLE) -1)
		{
			strPortName += L" Port Open Failed";
// 			AfxMessageBox(strPortName);
			TCHAR tmpStr[512];
			sprintf_s(tmpStr, _countof(tmpStr), _T("%s"), strPortName.GetBuffer());
			TECmsgOut(m_Util.CStringTobstr(tmpStr), LIGHTMAGENTA, 1000);
			return FALSE;
		}
	

	//===== ��Ʈ ���� ����. =====

	// EV_RXCHAR event ����...�����Ͱ� ������.. ���� �̺�Ʈ�� �߻��ϰԲ�..
	SetCommMask( m_hComm, EV_RXCHAR);	

	// InQueue, OutQueue ũ�� ����.
	SetupComm( m_hComm, Q_BUFF_SIZE, Q_BUFF_SIZE);	

	// ��Ʈ ����.
	PurgeComm( m_hComm,					
			   PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);

	// timeout ����.
	timeouts.ReadIntervalTimeout = 0xFFFFFFFF;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	
	timeouts.WriteTotalTimeoutMultiplier = 2*CBR_9600 / dwBaud;
	timeouts.WriteTotalTimeoutConstant = 0;
	
	SetCommTimeouts( m_hComm, &timeouts);

	// dcb ����.... ��Ʈ�� ��������..��� ����ϴ� DCB ����ü�� ����..
	dcb.DCBlength = sizeof(DCB);

	//--> ���� ������ �� �߿���..
	GetCommState( m_hComm, &dcb);	
	
	//--> ���巹��Ʈ�� �ٲٰ�..
	dcb.BaudRate = dwBaud;
	

	//--> Data 8 Bit
	dcb.ByteSize = byData;

	//--> Noparity
	dcb.Parity = byParity;

	//--> 1 Stop Bit
	dcb.StopBits = byStop;


	//--> ��Ʈ�� ��..����������.. �����غ���..
	if( !SetCommState( m_hComm, &dcb) )	
	{
		return FALSE;
	}


// 	// ��Ʈ ���� ������ ����.
	m_bConnected = TRUE;
// 
// 	//--> ��Ʈ ���� ������ ����.
//  	m_hThreadWatchComm = CreateThread( NULL, 0, 
// 									   (LPTHREAD_START_ROUTINE)ThreadWatchComm, 
// 									   this, 0, &dwThreadID);

// 	//--> ������ ������ �����ϸ�..
// 	if (! m_hThreadWatchComm)
// 	{
// 		//--> ���� ��Ʈ�� �ݰ�..
// 		ClosePort();
// 		return FALSE;
// 	}
	check = FALSE;

	return TRUE;
}
	
// ��Ʈ�� �ݴ´�.
void CCommThread::ClosePort()
{
	LightControl(0, 1, 0);


	//--> ������� �ʾ���.
	m_bConnected = FALSE;
	
	//--> ����ũ ����..
	SetCommMask( m_hComm, 0);
	
	//--> ��Ʈ ����.
	PurgeComm( m_hComm,	PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);
	
	CloseHandle(m_hComm);
	//--> �ڵ� �ݱ�
	
	Sleep(0);
	while (1)
	{
		if (m_hThreadWatchComm == NULL)
		{
			break;
		}
	}

}

// ��Ʈ�� pBuff�� ������ nToWrite��ŭ ����.
// ������ ������ Byte���� �����Ѵ�.
DWORD CCommThread::WriteComm(BYTE *pBuff, DWORD nToWrite)
{
	DWORD	dwWritten, dwError, dwErrorFlags;
	COMSTAT	comstat;

	//--> ��Ʈ�� ������� ���� �����̸�..
	if( !m_bConnected )		
	{
		return 0;
	}


	//--> ���ڷ� ���� ������ ������ nToWrite ��ŭ ����.. �� ������.,dwWrite �� �ѱ�.
	if( !WriteFile( m_hComm, pBuff, nToWrite, &dwWritten, &m_osWrite))
	{
		//--> ���� ������ ���ڰ� ������ ���..
		if (GetLastError() == ERROR_IO_PENDING)
		{
			// ���� ���ڰ� ���� �ְų� ������ ���ڰ� ���� ���� ��� Overapped IO��
			// Ư���� ���� ERROR_IO_PENDING ���� �޽����� ���޵ȴ�.
			//timeouts�� ������ �ð���ŭ ��ٷ��ش�.
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



	//--> ���� ��Ʈ�� ������ ������ ����..
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

// ��Ʈ�κ��� pBuff�� nToWrite��ŭ �д´�.
// ������ ������ Byte���� �����Ѵ�.
DWORD CCommThread::ReadComm(BYTE *pBuff, DWORD nToRead)
{
	DWORD	dwRead,dwError, dwErrorFlags;
	COMSTAT comstat;

	//--- system queue�� ������ byte���� �̸� �д´�.
	ClearCommError( m_hComm, &dwErrorFlags, &comstat);

	//--> �ý��� ť���� ���� �Ÿ��� ������..
	dwRead = comstat.cbInQue;
	if(dwRead > 0)
	{

		//--> ���ۿ� �ϴ� �о���̴µ�.. ����..�о���ΰ��� ���ٸ�..

		if( !ReadFile( m_hComm, pBuff, nToRead, &dwRead, &m_osRead) )
		{
			//--> ���� �Ÿ��� ��������..
			if (GetLastError() == ERROR_IO_PENDING)
			{
				//--------- timeouts�� ������ �ð���ŭ ��ٷ��ش�.
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


	//--> ���� �о���� ������ ����.
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
	//�� �Լ��� char���ڸ� hex������ ������ �ִ� �Լ� �Դϴ�.
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

	byGetData = strSend; //��Ʈ�� �ɹ� ������ �޴´�.
	byGetData.Replace(_T(" "), _T(""));// ���� ���ֱ� 
	byGetData.Replace(_T("\r\n"), _T(""));//���� ���ֱ�
	datasize = byGetData.GetLength(); // ������ ���� ���ڿ� ���� ��� 

									  // ���� ���̸� %2�� ���� ���� 0�� �ƴϸ� Ȧ�� �̱� ������ ������ ���ڸ� ó�� ����� ��
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
	//������ ���ڰ� 1�ڸ��� �϶� ó�� �ϱ� ����  ��) 1 -> 01
	if (datasize % 2 != 0)
	{
		Send_buff[bufPos++] = byCode2AsciiValue((char)byGetData[datasize - 1]);
	}

	int etc = bufPos % 8;
	//��Ʈ�� �����͸� 8���� ���� ����
	//�������� ���̰� 8�� ����� �ƴϸ� ������ �����ʹ� ���� ������
	for (j = 0; j < bufPos - etc; j += 8)//8�� ��� ����
	{
		WriteComm(&Send_buff[j], 8);
	}
	if (etc != 0)//������ ������ ����
	{
		WriteComm(&Send_buff[bufPos - etc], etc);// ��Ʈ�� ������ ���� 
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
	for (int i = 0; i < nSize; i++)//ASCII ������ �̱� ������ ���ۿ� �״�� ����
	{
		Send_buff[bufPos++] = (BYTE)strSend[i];
	}

	int etc = bufPos % 8;
	//��Ʈ�� �����͸� 8���� ���� ����
	//�������� ���̰� 8�� ����� �ƴϸ� ������ �����ʹ� ���� ������
	for (int j = 0; j < bufPos - etc; j += 8)
	{
		WriteComm(&Send_buff[j], 8);
	}
	if (etc != 0)//������ ������ ����
	{
		WriteComm(&Send_buff[bufPos - etc], etc);
	}
	delete[] Send_buff;
	return nSize;
}

// ��Ʈ�� �����ϰ�, ���� ������ ������
// m_ReadData�� ������ �ڿ� MainWnd�� �޽����� ������ Buffer�� ������
// �о��� �Ű��Ѵ�.

DWORD	ThreadWatchComm(CCommThread* pComm)
{
   DWORD           dwEvent;
   OVERLAPPED      os;
   BOOL            bOk = TRUE;
   BYTE            buff[204800];      // �б� ����
   DWORD           dwRead;  // ���� ����Ʈ��.
   DWORD           dwBuffSize;  

   CString		tmpEndStr = _T("");
   WORD			i = 0;
   BOOL			bContinueRead = TRUE;

   // Event, OS ����.
   memset( &os, 0, sizeof(OVERLAPPED));
   
   //--> �̺�Ʈ ����..
   if( !(os.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL)) )
   {
		bOk = FALSE;
   }

   //--> �̺�Ʈ ����ũ..
   if( !SetCommMask( pComm->m_hComm, EV_RXCHAR) )
   {
	   bOk = FALSE;
   }

   //--> �̺�Ʈ��..����ũ ������ ������..
   if( !bOk )
   {
		AfxMessageBox(_T("Error while creating ThreadWatchComm, ") + pComm->m_sPortName);
		return FALSE;
   }
  
   while (pComm ->m_bConnected)//��Ʈ�� ����Ǹ� ���� ������ ��
   {
 		dwEvent = 0;
	
        // ��Ʈ�� ���� �Ÿ��� �ö����� ��ٸ���.
        WaitCommEvent( pComm->m_hComm, &dwEvent, NULL);
	
	
		//--> �����Ͱ� ���ŵǾ��ٴ� �޼����� �߻��ϸ�..
        if ((dwEvent & EV_RXCHAR) == EV_RXCHAR)
        {
            // ��Ʈ���� ���� �� �ִ� ��ŭ �д´�.
				//--> buff �� �޾Ƴ���..
			do
			{
				dwRead = pComm->ReadComm( buff, 2048); //���� ������ �о� ���� 
				dwBuffSize = pComm->m_QueueRead.GetSize();
				if(Q_BUFF_SIZE - dwBuffSize > (int)dwRead)
				{
// 					i = 0;
// 					bContinueRead = TRUE;
// 					while (bContinueRead)
// 					{
// 						pComm->m_QueueRead.PutByte(buff[i]);//ť ���ۿ�  ���� ������ �ֱ� 
// 						tmpEndStr.Format(_T("%c%c"), buff[i-1], buff[i]);
// 						if (tmpEndStr == _T("\r\n") || i>Q_BUFF_SIZE)
// 							bContinueRead = FALSE;
// 						i++;
// 					}
					for(i = 0; i < dwRead; i++ )
					{
						pComm->m_QueueRead.PutByte(buff[i]);//ť ���ۿ�  ���� ������ �ֱ� 
					}
				}
				else
					AfxMessageBox("buff full"); //ť������ ũ�⸦ �ʰ��ϸ� ��� �޽��� ����
			}while(dwRead);
			::PostMessage(hCommWnd, WM_COMM_READ, pComm->m_iPortNum, 0);//CSerialComDlg�� �����Ͱ� ���Դٴ� �޽����� ����
		}
		Sleep(0);	// ���� �����͸� ȭ�鿡 ������ �ð��� ���� ����.
					// �����͸� �������� ������ cpu�������� 100%�� �Ǿ� ȭ�鿡 �ѷ��ִ� �۾��� �� �ȵǰ�. ��������� 
					// ť ���ۿ� �����Ͱ� ���̰� ��
   }
   
  CloseHandle( os.hEvent);

   //--> ������ ���ᰡ �ǰ���?
   pComm->m_hThreadWatchComm = NULL;

   return TRUE;

}
