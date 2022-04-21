// LogProcess.cpp: implementation of the CLogProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "direct.h"
#include "LogProcess.h"
	//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CLogProcess::CLogProcess()
{

	m_bWriteFlag     = TRUE;

	m_strLogPath     = m_Util.m_fnGetCurrentDrive();
	m_strLogPath    += DEFAULT_PATH;
	m_strLogPath    += DEFAULT_LOG_PATH;
	m_Util.m_fnSetCreateDir( m_strLogPath ); 
	m_strLogFileName = DEFAULT_LOG_NAME;
}

CLogProcess::~CLogProcess()
{
}

//////////////////////////////////////////////////////////////////////
// 로그 경로를 설정한다. ex) C:\\BGMSYS\\LOG\\strLogPath\\
//////////////////////////////////////////////////////////////////////
BOOL CLogProcess::m_fnSetLogPath( CString strLogPath, CString strFileName )
{
	m_strLogFileName		= strFileName;
	m_strLogPath.TrimRight();
	m_strLogPath.TrimLeft();
	if ( m_strLogPath.IsEmpty() )
	{
		m_strLogPath        = "";
		m_strLogPath		= m_Util.m_fnGetCurrentDrive();
		m_strLogPath		+= DEFAULT_PATH;
		m_strLogPath		+= DEFAULT_LOG_PATH;
		m_strLogPath		+= strLogPath;
		m_strLogPath		+= "\\";
		m_Util.m_fnSetCreateDir( m_strLogPath ); 
		return FALSE;
	}
	else
	{
		m_strLogPath		+= strLogPath;
		m_strLogPath		+= "\\";
	}
	m_Util.m_fnSetCreateDir( m_strLogPath ); 
	return TRUE;
}

CString CLogProcess::m_fnGetLogPath( )
{
	return m_strLogPath;
}

BOOL CLogProcess::m_fnSetLogWriteFlag( BOOL bWriteFlag )
{
	m_bWriteFlag = bWriteFlag;
	return TRUE;
}

/*-----------------------------------------------------------------------
Func Name : DEBUG_LOG()
Reference : Log
-----------------------------------------------------------------------*/
void CLogProcess::DEBUG_LOG(char* fmt, ...)
{
#if 1
	// 저장 Flag가 FALSE일때는 로그를 저장하지 않는다.
	if ( !m_bWriteFlag )
	{
		return;
	}
	if (( m_strLogFileName.IsEmpty() ) ||
		( m_strLogFileName == DEFAULT_LOG_NAME ))
	{
		return;
	}
	try 
	{
		char    szFileInfo[FILE_LEN+1];

		// 로그경로와 로그명을 설정한다.
		memset(szFileInfo, 0x00, sizeof(szFileInfo));
		sprintf_s(szFileInfo, "%s%s", m_strLogPath.GetString(), m_strLogFileName.GetString());

		va_list ap;
		SYSTEMTIME mtime;
		GetLocalTime(&mtime);
		va_start(ap, fmt);

		memset(m_szLog, 0x00, sizeof(m_szLog));
		sprintf_s( m_szLog, LOGPATH, szFileInfo, mtime.wYear, mtime.wMonth, mtime.wDay, mtime.wHour );
		FILE* fp;
		fopen_s(&fp, m_szLog, "a");

		if (fp) 
		{
			fprintf(fp, "%02d:%02d:%02d.%03d ",
				mtime.wHour, mtime.wMinute, mtime.wSecond,
				mtime.wMilliseconds);
			vfprintf(fp, fmt, ap);
			fprintf(fp, "%s","\n");
			fflush(fp); 
			fclose(fp);
		}

		va_end(ap);
	} 
	catch(...) 
	{
	}
#endif
}

