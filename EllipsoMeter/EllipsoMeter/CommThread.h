

#define WM_COMM_READ (WM_USER +1)
#define Q_BUFF_SIZE			4192

#define SR_CNT 1

#define LVS 1
#define VIT 0

//	통신 클래스	CCommThread 

// 포트에서 읽기 :
//   포트를 연 후에 포트에 자료가 도착하면 WM_COMM_READ 메시지가 메인 
//   윈도우에 전달된다. ON_MESSAGE 매크로를 이용, 함수를 연결하고
//   m_ReadData String에 저장된 데이터를 이용 읽기
// 포트에 쓰기 : WriteComm(buff, 30)과 같이 버퍼와 그 크기를 건네면 된다.
class CQueue
{
public:
	BYTE buff[Q_BUFF_SIZE];//큐 버퍼 사이즈는 4192로 만듬 
	int m_iHead, m_iTail;//큐 버퍼에 데이터를 넣고 빼 올때 사용할 변수 
	CQueue();
	void Clear();//버퍼를 초기화 시킴
	int GetSize();//현재 버퍼에 들어있는 데이터의 size를 리턴
	BOOL PutByte(BYTE b);//큐버퍼에 1바이트를 넣음
	BOOL GetByte(BYTE *pb);//큐버퍼에서 1바이트를 빼 옴
};

class	CCommThread
{
public:
	CCommThread(int Port = 0);
	~CCommThread();
	//--------- 환경 변수 -----------------------------------------//
	int			m_iPortNum;
	BOOL        check;
	HANDLE		m_hComm;				// 통신 포트 파일 핸들
	CString		m_sPortName;			// 포트 이름 (COM1 ..)
	BOOL		m_bConnected;			// 포트가 열렸는지 유무를 나타냄.
	OVERLAPPED	m_osRead, m_osWrite;	// 포트 파일 Overlapped structure
	HANDLE		m_hThreadWatchComm;		// Watch함수 Thread 핸들.
	WORD		m_wPortID;				// WM_COMM_READ와 함께 보내는 인수.
	CQueue      m_QueueRead;			//큐버퍼
	//--------- 외부 사용 함수 ------------------------------------//
	BOOL	OpenPort(CString strPortName, 
					   DWORD dwBaud, BYTE byData, BYTE byStop, BYTE byParity );//포트 열기 
	void	ClosePort();				//포트 닫기
	DWORD	WriteComm(BYTE *pBuff, DWORD nToWrite);//포트에 데이터 쓰기
	BOOL	WriteCommByte(unsigned char ucByte);

	//--------- 내부 사용 함수 ------------------------------------//
	DWORD	ReadComm(BYTE *pBuff, DWORD nToRead);//포트에서 데이터 읽어오기

	CCriticalSection csCommTh;

	// private:
	int m_iSize; //설정창에 시리얼로 들어온 데이터 갯수를 보여주기 위해  갯수를 받을 변수 
	int m_iCount;// 받은 데이터를 화면에 보여줄때 정렬 하기 위해 사용 한줄에 8묶음 2개 (16개 데이터)
	int nMakeHexData(CString strSend); //보낼 데이터 타입이 hex일 경우 hex데이터로 만들어서 전송
	int nMakeAsciiData(CString strSend);//보낼 데이터 타입이 ascii일 경우 ascii데이터로 만들어서 전송
	BYTE byCode2AsciiValue(char cData);// 문자를 hex값으로 변경 0~F 까지는 그대로 그 외에 글자는 0으로 

	CString byIndexComPort(int xPort);// 포트이름 받기 
	DWORD byIndexBaud(int xBaud);// 전송률 받기
	BYTE byIndexData(int xData);//데이터 비트 받기
	BYTE byIndexStop(int xStop);// 스톱비트 받기 
	BYTE byIndexParity(int xParity);// 펠리티 받기

	CCriticalSection csLightWrite[3];

};

int LightControl(int portNo, int chNo, int iLightVal);
int LightControl_HSP_Top(int portNo, int chNo, int* iLightVal);
int LightControl_HSP_RunMode(int* iLightVal);

extern CCommThread	*pCommTh[SR_CNT];

// Thread로 사용할 함수 
DWORD	ThreadWatchComm(CCommThread* pComm);

