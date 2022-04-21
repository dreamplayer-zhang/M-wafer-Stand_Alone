/*-----------------------------------------------------------------------
  모듈명 : CTaskHandle.cpp
  설명   : CTaskHandle 공유 메모리 관리용 Thread
-----------------------------------------------------------------------*/
#include "stdafx.h"
#include "TaskHandle.h"
//------------------------------------------------------------
// Thread 구조체
//------------------------------------------------------------
class ThreadInfo
{
public:
	LPCH  m_pMapView;
	CWnd* m_pParent;
	UINT  m_pPort;
} g_threadInfo;

//------------------------------------------------------------
// Thread 구조체에 사용하는 변수 및 포인터
//------------------------------------------------------------
CWnd* m_phWnd     = NULL;
UINT  m_pPort     = 0;
int   m_nWaitTime = 0;

//------------------------------------------------------------
// Thread 함수 정의
//------------------------------------------------------------
UINT MyThreadProc(LPVOID pParam);
/*-----------------------------------------------------------------------
Func Name : CTaskHandle
Reference : CTaskHandle 클래스 생성
-----------------------------------------------------------------------*/
CTaskHandle::CTaskHandle( CWnd*    phWnd,		// 메세지 전달장소 포인터
						  UINT     nPort,		// 유니크 번호
						  int	   nSize,		// 공유맵 파일 크기
						  int      nWaitTime,	// 스래드 대기 시간
						  BOOL     bStart )		// 스래드 생성/비생성 
{
	//--------------------------------------------------------
	// 변수를 초기 설정한다.
	//--------------------------------------------------------
	CString strString = _T("ShareFile_Name");

	m_hMapObject = NULL;
	m_pMapView	 = NULL;
	m_pPort		 = nPort;
	m_phWnd		 = phWnd;
	m_bStart     = bStart;
	m_strMsg     = _T("");

	//--------------------------------------------------------
	// 대기 시간 설정 ( 미설정시 Default = 250 ms ).
	//--------------------------------------------------------
	if ( nWaitTime == 0 )
	{
		m_nWaitTime = 250;
	}
	else
	{
		m_nWaitTime = nWaitTime;
	}

	//--------------------------------------------------------
	// 공유 메모리를 생성한다.
	//--------------------------------------------------------
	m_Share_Memory_Create( strString, nSize );

}

/*-----------------------------------------------------------------------
Func Name : CTaskHandle
Reference : CTaskHandle 클래스 생성
-----------------------------------------------------------------------*/
CTaskHandle::CTaskHandle( CWnd*   phWnd,		// 메세지 전달장소 포인터
						  CString strString,	// 공유메모리 파일명
						  UINT    nPort,		// 유니크 번호
						  int     nSize,		// 공유맵 파일 크기
						  int     nWaitTime,	// 스래드 대기 시간 
						  BOOL    bStart )		// 스래드 생성/비생성
{
	//--------------------------------------------------------
	// 변수를 초기 설정한다.
	//--------------------------------------------------------
	m_hMapObject	= NULL;
	m_pMapView		= NULL;
	m_pPort			= nPort;
	m_phWnd			= phWnd;
	m_bStart		= bStart;
	m_strMsg		= _T("");

	//--------------------------------------------------------
	// 공유메모리명이 없을때 디폴트로 설정한다.
	//--------------------------------------------------------
	if ( strString.IsEmpty())
	{
		strString = _T("Undefine_Name");
	}

	//--------------------------------------------------------
	// 대기 시간 설정 ( 미설정시 Default = 250 ms ).
	//--------------------------------------------------------
	if ( nWaitTime == 0 )
	{
		m_nWaitTime = 250;
	}
	else
	{
		m_nWaitTime = nWaitTime;
	}

	//--------------------------------------------------------
	// 공유 메모리를 생성한다.
	//--------------------------------------------------------
	m_Share_Memory_Create( strString, nSize );

}

/*-----------------------------------------------------------------------
Func Name : CTaskHandle
Reference : CTaskHandle 클래스 소멸
-----------------------------------------------------------------------*/
CTaskHandle::~CTaskHandle()
{
	if ( m_pThread != NULL )
	{
		m_pThread->SuspendThread();
		TerminateThread( m_pThread->m_hThread, 0 );
		/* Thread 상태를 얻어온다.
		   GetExitCodeThread( m_pThread->m_hThread, &dwExitCode );
		*/
		delete m_pThread;
		m_pThread = NULL;
	}

	if (m_hMapObject != NULL)
	{
		CloseHandle(m_hMapObject);
	}

	if (m_pMapView != NULL)
	{
		UnmapViewOfFile(m_pMapView);
	}

}

BEGIN_MESSAGE_MAP(CTaskHandle, CWnd)
	//{{AFX_MSG_MAP(TaskHandle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*-----------------------------------------------------------------------
Func Name : m_Share_Memory_Create
Reference : m_Share_Memory_Create 공유 메모리 생성
-----------------------------------------------------------------------*/
void CTaskHandle::m_Share_Memory_Create( CString strString, int nSize )
{

	//--------------------------------------------------------
	// 공유 메모리를 생성한다.
	//--------------------------------------------------------
	m_Share_FileName = strString;
	m_hMapObject = OpenFileMapping(FILE_MAP_WRITE, FALSE, m_Share_FileName );
	if ( m_hMapObject == NULL )
	{
		m_hMapObject = ::CreateFileMapping((HANDLE)-1, NULL,
			                               PAGE_READWRITE, 0,
										   nSize + 1, m_Share_FileName );
		if ( m_hMapObject == NULL )
		{
			m_strMsg.Format("TaskHandle : Unable to create %s file",m_Share_FileName); 
			AfxMessageBox( m_strMsg );
			return;
		}
	}


	//--------------------------------------------------------
	// 공유 메모리의 포인터를 취득한다.
	//--------------------------------------------------------
	m_pMapView = (LPCH)MapViewOfFile (m_hMapObject, FILE_MAP_WRITE, 0, 0, 0);
	if ( m_pMapView == NULL )
	{
		m_strMsg.Format("TaskHandle : Can't map view of %s file",m_Share_FileName); 
		AfxMessageBox( m_strMsg );
		return;
	}
	
	//--------------------------------------------------------
	// 파라메터값이 스래드 생성이면 작업용 Thread를 생성한다.
	//--------------------------------------------------------
	m_pThread = NULL;
	g_threadInfo.m_pMapView  = m_pMapView;

	if ( m_bStart )
	{
		//--------------------------------------------------------
		// 공유 메모리의 포인터를 설정한다.
		//--------------------------------------------------------
		g_threadInfo.m_pParent   = m_phWnd;
		g_threadInfo.m_pPort     = m_pPort;

		m_pThread = AfxBeginThread(MyThreadProc, &g_threadInfo);
		if ( m_pThread == NULL )
		{
			//------------------------------------------------------
			// 스래드 생성 실패
			//------------------------------------------------------
			m_strMsg.Format("TaskHandle : Can't create thread to monitor file(%s)",
						 m_Share_FileName); 
			AfxMessageBox( m_strMsg );
			return;
		}
	}
}

/*-----------------------------------------------------------------------
Func Name : MyThreadProc
Reference : MyThreadProc Thread에서 대기시간을 주기로 계속 메세지 전달
-----------------------------------------------------------------------*/
UINT MyThreadProc(LPVOID pParam)
{
	//--------------------------------------------------------
	// 스래드에 공유메모리 정보를 설정한다.
	//--------------------------------------------------------
	LPCH  pMapView  = ((ThreadInfo*)pParam)->m_pMapView;
	CWnd* pParent   = ((ThreadInfo*)pParam)->m_pParent;
	UINT  pPort     = ((ThreadInfo*)pParam)->m_pPort;

	while( 1 )
	{
		//-------------------------------------------------
		// 메세지를 보낼 포인터를 잃은 경우
		//-------------------------------------------------
		if (((ThreadInfo*)pParam)->m_pParent == NULL )
		{
			ExitThread( 0 );
			continue;
		}
		//-------------------------------------------------
		// 메세지를 보낸다.
		//-------------------------------------------------
		if ( pParent != NULL )
		{
			if ( pParent->m_hWnd != NULL )
			{
				pParent->PostMessage(WM_SET_COORDINATES, pPort, 0);
			}
		}
		Sleep(m_nWaitTime);
	}
	return 0;
}

/*-----------------------------------------------------------------------
Func Name : m_pFilePointer
Reference : m_pFilePointer MapView 포인터를 외부로 전달한다.
-----------------------------------------------------------------------*/
LPCH CTaskHandle::m_pFilePointer(  ) 
{
	return g_threadInfo.m_pMapView;
}

/*-----------------------------------------------------------------------
Func Name : m_pFilePointer
Reference : m_pFilePointer MapView 포인터를 외부로 전달한다.
-----------------------------------------------------------------------*/
void CTaskHandle::m_fnSetEnd(  ) 
{
	g_threadInfo.m_pParent   = NULL;
}
