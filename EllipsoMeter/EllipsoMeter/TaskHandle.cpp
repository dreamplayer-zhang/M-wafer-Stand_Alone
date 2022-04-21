/*-----------------------------------------------------------------------
  ���� : CTaskHandle.cpp
  ����   : CTaskHandle ���� �޸� ������ Thread
-----------------------------------------------------------------------*/
#include "stdafx.h"
#include "TaskHandle.h"
//------------------------------------------------------------
// Thread ����ü
//------------------------------------------------------------
class ThreadInfo
{
public:
	LPCH  m_pMapView;
	CWnd* m_pParent;
	UINT  m_pPort;
} g_threadInfo;

//------------------------------------------------------------
// Thread ����ü�� ����ϴ� ���� �� ������
//------------------------------------------------------------
CWnd* m_phWnd     = NULL;
UINT  m_pPort     = 0;
int   m_nWaitTime = 0;

//------------------------------------------------------------
// Thread �Լ� ����
//------------------------------------------------------------
UINT MyThreadProc(LPVOID pParam);
/*-----------------------------------------------------------------------
Func Name : CTaskHandle
Reference : CTaskHandle Ŭ���� ����
-----------------------------------------------------------------------*/
CTaskHandle::CTaskHandle( CWnd*    phWnd,		// �޼��� ������� ������
						  UINT     nPort,		// ����ũ ��ȣ
						  int	   nSize,		// ������ ���� ũ��
						  int      nWaitTime,	// ������ ��� �ð�
						  BOOL     bStart )		// ������ ����/����� 
{
	//--------------------------------------------------------
	// ������ �ʱ� �����Ѵ�.
	//--------------------------------------------------------
	CString strString = _T("ShareFile_Name");

	m_hMapObject = NULL;
	m_pMapView	 = NULL;
	m_pPort		 = nPort;
	m_phWnd		 = phWnd;
	m_bStart     = bStart;
	m_strMsg     = _T("");

	//--------------------------------------------------------
	// ��� �ð� ���� ( �̼����� Default = 250 ms ).
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
	// ���� �޸𸮸� �����Ѵ�.
	//--------------------------------------------------------
	m_Share_Memory_Create( strString, nSize );

}

/*-----------------------------------------------------------------------
Func Name : CTaskHandle
Reference : CTaskHandle Ŭ���� ����
-----------------------------------------------------------------------*/
CTaskHandle::CTaskHandle( CWnd*   phWnd,		// �޼��� ������� ������
						  CString strString,	// �����޸� ���ϸ�
						  UINT    nPort,		// ����ũ ��ȣ
						  int     nSize,		// ������ ���� ũ��
						  int     nWaitTime,	// ������ ��� �ð� 
						  BOOL    bStart )		// ������ ����/�����
{
	//--------------------------------------------------------
	// ������ �ʱ� �����Ѵ�.
	//--------------------------------------------------------
	m_hMapObject	= NULL;
	m_pMapView		= NULL;
	m_pPort			= nPort;
	m_phWnd			= phWnd;
	m_bStart		= bStart;
	m_strMsg		= _T("");

	//--------------------------------------------------------
	// �����޸𸮸��� ������ ����Ʈ�� �����Ѵ�.
	//--------------------------------------------------------
	if ( strString.IsEmpty())
	{
		strString = _T("Undefine_Name");
	}

	//--------------------------------------------------------
	// ��� �ð� ���� ( �̼����� Default = 250 ms ).
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
	// ���� �޸𸮸� �����Ѵ�.
	//--------------------------------------------------------
	m_Share_Memory_Create( strString, nSize );

}

/*-----------------------------------------------------------------------
Func Name : CTaskHandle
Reference : CTaskHandle Ŭ���� �Ҹ�
-----------------------------------------------------------------------*/
CTaskHandle::~CTaskHandle()
{
	if ( m_pThread != NULL )
	{
		m_pThread->SuspendThread();
		TerminateThread( m_pThread->m_hThread, 0 );
		/* Thread ���¸� ���´�.
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
Reference : m_Share_Memory_Create ���� �޸� ����
-----------------------------------------------------------------------*/
void CTaskHandle::m_Share_Memory_Create( CString strString, int nSize )
{

	//--------------------------------------------------------
	// ���� �޸𸮸� �����Ѵ�.
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
	// ���� �޸��� �����͸� ����Ѵ�.
	//--------------------------------------------------------
	m_pMapView = (LPCH)MapViewOfFile (m_hMapObject, FILE_MAP_WRITE, 0, 0, 0);
	if ( m_pMapView == NULL )
	{
		m_strMsg.Format("TaskHandle : Can't map view of %s file",m_Share_FileName); 
		AfxMessageBox( m_strMsg );
		return;
	}
	
	//--------------------------------------------------------
	// �Ķ���Ͱ��� ������ �����̸� �۾��� Thread�� �����Ѵ�.
	//--------------------------------------------------------
	m_pThread = NULL;
	g_threadInfo.m_pMapView  = m_pMapView;

	if ( m_bStart )
	{
		//--------------------------------------------------------
		// ���� �޸��� �����͸� �����Ѵ�.
		//--------------------------------------------------------
		g_threadInfo.m_pParent   = m_phWnd;
		g_threadInfo.m_pPort     = m_pPort;

		m_pThread = AfxBeginThread(MyThreadProc, &g_threadInfo);
		if ( m_pThread == NULL )
		{
			//------------------------------------------------------
			// ������ ���� ����
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
Reference : MyThreadProc Thread���� ���ð��� �ֱ�� ��� �޼��� ����
-----------------------------------------------------------------------*/
UINT MyThreadProc(LPVOID pParam)
{
	//--------------------------------------------------------
	// �����忡 �����޸� ������ �����Ѵ�.
	//--------------------------------------------------------
	LPCH  pMapView  = ((ThreadInfo*)pParam)->m_pMapView;
	CWnd* pParent   = ((ThreadInfo*)pParam)->m_pParent;
	UINT  pPort     = ((ThreadInfo*)pParam)->m_pPort;

	while( 1 )
	{
		//-------------------------------------------------
		// �޼����� ���� �����͸� ���� ���
		//-------------------------------------------------
		if (((ThreadInfo*)pParam)->m_pParent == NULL )
		{
			ExitThread( 0 );
			continue;
		}
		//-------------------------------------------------
		// �޼����� ������.
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
Reference : m_pFilePointer MapView �����͸� �ܺη� �����Ѵ�.
-----------------------------------------------------------------------*/
LPCH CTaskHandle::m_pFilePointer(  ) 
{
	return g_threadInfo.m_pMapView;
}

/*-----------------------------------------------------------------------
Func Name : m_pFilePointer
Reference : m_pFilePointer MapView �����͸� �ܺη� �����Ѵ�.
-----------------------------------------------------------------------*/
void CTaskHandle::m_fnSetEnd(  ) 
{
	g_threadInfo.m_pParent   = NULL;
}
