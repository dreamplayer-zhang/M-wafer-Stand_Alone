

#define WM_COMM_READ (WM_USER +1)
#define Q_BUFF_SIZE			4192

#define SR_CNT 1

#define LVS 1
#define VIT 0

//	��� Ŭ����	CCommThread 

// ��Ʈ���� �б� :
//   ��Ʈ�� �� �Ŀ� ��Ʈ�� �ڷᰡ �����ϸ� WM_COMM_READ �޽����� ���� 
//   �����쿡 ���޵ȴ�. ON_MESSAGE ��ũ�θ� �̿�, �Լ��� �����ϰ�
//   m_ReadData String�� ����� �����͸� �̿� �б�
// ��Ʈ�� ���� : WriteComm(buff, 30)�� ���� ���ۿ� �� ũ�⸦ �ǳ׸� �ȴ�.
class CQueue
{
public:
	BYTE buff[Q_BUFF_SIZE];//ť ���� ������� 4192�� ���� 
	int m_iHead, m_iTail;//ť ���ۿ� �����͸� �ְ� �� �ö� ����� ���� 
	CQueue();
	void Clear();//���۸� �ʱ�ȭ ��Ŵ
	int GetSize();//���� ���ۿ� ����ִ� �������� size�� ����
	BOOL PutByte(BYTE b);//ť���ۿ� 1����Ʈ�� ����
	BOOL GetByte(BYTE *pb);//ť���ۿ��� 1����Ʈ�� �� ��
};

class	CCommThread
{
public:
	CCommThread(int Port = 0);
	~CCommThread();
	//--------- ȯ�� ���� -----------------------------------------//
	int			m_iPortNum;
	BOOL        check;
	HANDLE		m_hComm;				// ��� ��Ʈ ���� �ڵ�
	CString		m_sPortName;			// ��Ʈ �̸� (COM1 ..)
	BOOL		m_bConnected;			// ��Ʈ�� ���ȴ��� ������ ��Ÿ��.
	OVERLAPPED	m_osRead, m_osWrite;	// ��Ʈ ���� Overlapped structure
	HANDLE		m_hThreadWatchComm;		// Watch�Լ� Thread �ڵ�.
	WORD		m_wPortID;				// WM_COMM_READ�� �Բ� ������ �μ�.
	CQueue      m_QueueRead;			//ť����
	//--------- �ܺ� ��� �Լ� ------------------------------------//
	BOOL	OpenPort(CString strPortName, 
					   DWORD dwBaud, BYTE byData, BYTE byStop, BYTE byParity );//��Ʈ ���� 
	void	ClosePort();				//��Ʈ �ݱ�
	DWORD	WriteComm(BYTE *pBuff, DWORD nToWrite);//��Ʈ�� ������ ����
	BOOL	WriteCommByte(unsigned char ucByte);

	//--------- ���� ��� �Լ� ------------------------------------//
	DWORD	ReadComm(BYTE *pBuff, DWORD nToRead);//��Ʈ���� ������ �о����

	CCriticalSection csCommTh;

	// private:
	int m_iSize; //����â�� �ø���� ���� ������ ������ �����ֱ� ����  ������ ���� ���� 
	int m_iCount;// ���� �����͸� ȭ�鿡 �����ٶ� ���� �ϱ� ���� ��� ���ٿ� 8���� 2�� (16�� ������)
	int nMakeHexData(CString strSend); //���� ������ Ÿ���� hex�� ��� hex�����ͷ� ���� ����
	int nMakeAsciiData(CString strSend);//���� ������ Ÿ���� ascii�� ��� ascii�����ͷ� ���� ����
	BYTE byCode2AsciiValue(char cData);// ���ڸ� hex������ ���� 0~F ������ �״�� �� �ܿ� ���ڴ� 0���� 

	CString byIndexComPort(int xPort);// ��Ʈ�̸� �ޱ� 
	DWORD byIndexBaud(int xBaud);// ���۷� �ޱ�
	BYTE byIndexData(int xData);//������ ��Ʈ �ޱ�
	BYTE byIndexStop(int xStop);// �����Ʈ �ޱ� 
	BYTE byIndexParity(int xParity);// �縮Ƽ �ޱ�

	CCriticalSection csLightWrite[3];

};

int LightControl(int portNo, int chNo, int iLightVal);
int LightControl_HSP_Top(int portNo, int chNo, int* iLightVal);
int LightControl_HSP_RunMode(int* iLightVal);

extern CCommThread	*pCommTh[SR_CNT];

// Thread�� ����� �Լ� 
DWORD	ThreadWatchComm(CCommThread* pComm);

