// LogProcess.h: interface for the CLogProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGPROCESS_H__476D1382_DADF_4B81_862A_39AEC27AEB7B__INCLUDED_)
#define AFX_LOGPROCESS_H__476D1382_DADF_4B81_862A_39AEC27AEB7B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class CLogProcess  
{
public:

	CString							m_strLogPath;
	CString							m_strLogFileName;
	char							m_szLog[MAX_BUFF_SIZE+1];

	BOOL							m_fnSetLogPath( CString strLogPath, CString strFileName );
	CString							m_fnGetLogPath( void );

	BOOL							m_bWriteFlag;
	BOOL							m_fnSetLogWriteFlag( BOOL bWriteFlag );

	CLogProcess();
	virtual ~CLogProcess();
	void							DEBUG_LOG(char* fmt, ...);
};

#endif // !defined(AFX_LOGPROCESS_H__476D1382_DADF_4B81_862A_39AEC27AEB7B__INCLUDED_)
