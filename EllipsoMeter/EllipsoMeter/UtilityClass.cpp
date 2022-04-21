// UtilityClass.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "UtilityClass.h"

// CUtilityClass

IMPLEMENT_DYNAMIC(CUtilityClass, CWnd)

CUtilityClass::CUtilityClass()
{
}

CUtilityClass::~CUtilityClass()
{
}

BEGIN_MESSAGE_MAP(CUtilityClass, CWnd)
END_MESSAGE_MAP()

// CUtilityClass �޽��� ó�����Դϴ�.
//---------------------------------------------------------------------------------
// �۾����� ����̹��� �о�´� ex) C:\
//---------------------------------------------------------------------------------
CString CUtilityClass::m_fnGetCurrentDrive()
{
	TCHAR szCurDir[MAX_PATH + 1] = _T("");
	::GetCurrentDirectory(MAX_PATH, szCurDir);

	szCurDir[3] = NULL;
	return szCurDir;
}

//---------------------------------------------------------------------------------
// �۾����� ����̹��� �о�´� ex) C:\
//---------------------------------------------------------------------------------
CString CUtilityClass::m_fnGetCurrentTime()
{
	CString     strTime("");
	SYSTEMTIME mtime;
	GetLocalTime(&mtime);

	strTime.Format( "%04d/%02d/%02d %02d:%02d:%02d.%03d", mtime.wYear, 
		                                        mtime.wMonth, 
												mtime.wDay, 
												mtime.wHour,
												mtime.wMinute,
												mtime.wSecond,
												mtime.wMilliseconds);
	return strTime;
}

//---------------------------------------------------------------------------------
// �۾����� ����̹��� �о�´� ex) C:\
//---------------------------------------------------------------------------------
CString CUtilityClass::m_fnGetCurrentTime2()
{
	CString     strTime("");
	SYSTEMTIME mtime;
	GetLocalTime(&mtime);

	strTime.Format( "%04d-%02d-%02d %02d:%02d:%02d", mtime.wYear, 
													mtime.wMonth, 
													mtime.wDay, 
													mtime.wHour,
													mtime.wMinute,
													mtime.wSecond);
	return strTime;
}


/*-----------------------------------------------------------------------
Func Name : m_fnGetCurrentTime3
Reference : m_fnGetCurrentTime3
-----------------------------------------------------------------------*/
CString CUtilityClass::m_fnGetCurrentTime3( int nHour )
{
	CTime	Ttime;			 
	CTime	TCurrentTime;	 
	CString cstrCurTime;	

	TCurrentTime = Ttime.GetCurrentTime() - CTimeSpan( 0, nHour, 0, 0 );
	cstrCurTime	 = TCurrentTime.Format("%Y%m%d%H%M%S"); 

	return cstrCurTime;
}

/*-----------------------------------------------------------------------
Func Name : m_fnGetCurrentTime3
Reference : m_fnGetCurrentTime3
-----------------------------------------------------------------------*/
CString CUtilityClass::m_fnGetCurrentTime4( CString strTime, int nHour1 )
{
	if ( strTime.GetLength() < 14)
	{
		return "";
	}
	CTime	TCurrentTime;	 
	CString cstrCurTime;

	int nYear	= atoi( strTime.Mid(0,4));
	int nMonth	= atoi(strTime.Mid(4,2));
	int nDay	= atoi(strTime.Mid(6,2));
	int nHour	= atoi(strTime.Mid(8,2));
	int nMinute	= atoi(strTime.Mid(10,2));
	int nSecond = atoi(strTime.Mid(12,2));
	CTime	Ttime( nYear, nMonth, nDay, nHour, nMinute, nSecond );			 

	TCurrentTime = Ttime + CTimeSpan( 0, nHour1, 0, 0 );
	cstrCurTime	 = TCurrentTime.Format("%Y/%m/%d  %H:%M:%S"); 

	return cstrCurTime;
}

double CUtilityClass::m_fnGetRangeRandom( double dRange_Min, double dRange_Max )
{
	return (double)rand() / (RAND_MAX + 1 )*(dRange_Max - dRange_Min ) + dRange_Min; 
}
//---------------------------------------------------------------------------------
// �۾����� ����̹��� �о�´� ex) C:\
//---------------------------------------------------------------------------------
CString CUtilityClass::m_fnGetCurrentTime1()
{
	CString     strTime("");
	SYSTEMTIME mtime;
	GetLocalTime(&mtime);

	strTime.Format( "%04d%02d%02d_%02d%02d%02d_%03d", mtime.wYear, 
														mtime.wMonth, 
														mtime.wDay, 
														mtime.wHour,
														mtime.wMinute,
														mtime.wSecond,
														mtime.wMilliseconds);
	return strTime;
}

//---------------------------------------------------------------------------------
// �۾����� ����̹��� �о�´� ex) C:\
//---------------------------------------------------------------------------------
CString CUtilityClass::m_fnGetCurrentTime5()
{
	CString     strTime("");
	SYSTEMTIME mtime;
	GetLocalTime(&mtime);

	strTime.Format( "%04d%02d%02d%02d%02d%02d", mtime.wYear, 
														mtime.wMonth, 
														mtime.wDay, 
														mtime.wHour,
														mtime.wMinute,
														mtime.wSecond);
	return strTime;
}

//---------------------------------------------------------------------------------
// �۾����� ����̹��� �о�´� ex) C:\
//---------------------------------------------------------------------------------
CString CUtilityClass::m_fnGetCurrentTime6()
{
	CString     strTime("");
	SYSTEMTIME mtime;
	GetLocalTime(&mtime);

	strTime.Format( "%04d%02d%02d", mtime.wYear, mtime.wMonth, mtime.wDay);
	return strTime;
}

//---------------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------------
CString CUtilityClass::m_fnGetDate()
{
	CString     strTime("");
	SYSTEMTIME mtime;
	GetLocalTime(&mtime);

	strTime.Format( "%04d-%02d-%02d", mtime.wYear, mtime.wMonth, mtime.wDay);
	return strTime;
}

//---------------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------------
CString CUtilityClass::m_fnGetTime()
{
	CString     strTime("");
	SYSTEMTIME mtime;
	GetLocalTime(&mtime);

	if (mtime.wHour > 12)
	{
		strTime.Format("%s %02d:%02d:%02d", "����", mtime.wHour - 12, mtime.wMinute, mtime.wSecond);
	}
	else
	{
		strTime.Format("%s %02d:%02d:%02d", "����", mtime.wHour, mtime.wMinute, mtime.wSecond);
	}
	return strTime;
}

//---------------------------------------------------------------------------------
// 
//---------------------------------------------------------------------------------
CString CUtilityClass::m_fnGetTime24()
{
	CString     strTime("");
	SYSTEMTIME mtime;
	GetLocalTime(&mtime);

	strTime.Format( "%02d%02d%02d", mtime.wHour, mtime.wMinute, mtime.wSecond);
	return strTime;
}

//---------------------------------------------------------------------------------
// ���̿� �°� �����Ϳ� ������ �߰��Ѵ�.
//---------------------------------------------------------------------------------
int CUtilityClass::m_fnGetCharToNUM( CString strData )
{
	if ( strData.IsEmpty() )
	{
		return 0;
	}

	for ( int i = 0; i < strData.GetLength(); i++ )
	{
		if ( strData.Mid( i, 1 ) == "0" )
		{
			strData.Mid( i, 1 ) = " ";	
		}
		else
		{
			break;
		}
	}
	strData.TrimLeft();
	strData.TrimRight();
	return atoi( strData );
}

//---------------------------------------------------------------------------------
// ���̿� �°� �����Ϳ� ������ �߰��Ѵ�.
//---------------------------------------------------------------------------------
CString CUtilityClass::m_fnGetCharEdit( char* szItem, int nLen )
{
	CString strData			= _T("");
	int     nTotal			= 0;
	strData.Format( "%s", szItem );
	nTotal	=				strData.GetLength();

	if ( nLen == nTotal )
	{
		return strData;
	}

	if ( nLen > nTotal )
	{
		for( int i = nTotal; i < nLen; i++)
		{
			strData += " ";
		}
	}
	else
	{
		return strData.Mid( 0, nLen );
	}

	return strData;
}

//---------------------------------------------------------------------------------
// ���̿� �°� �����Ϳ� "0" �� �߰��Ѵ�.
//---------------------------------------------------------------------------------
CString CUtilityClass::m_fnGetZeroEdit( char* szItem, int nLen )
{
	CString strData("");
	int     nTotal			= 0;
	strData.Format( "%s", szItem );
	nTotal	=				strData.GetLength();

	if ( nLen == nTotal )
	{
		return strData;
	}

	if ( nLen > nTotal )
	{
		for( int i = nTotal; i < nLen; i++)
		{
			strData += "0";
		}
	}
	else
	{
		return strData.Mid( 0, nLen );
	}

	return strData;
}

//---------------------------------------------------------------------------------
// ���̿� �°� �����Ϳ� ������ �߰��Ѵ�.
//---------------------------------------------------------------------------------
CString CUtilityClass::m_fnGetCharEdit( CString strData, int nLen )
{
	int     nTotal			= 0;
	nTotal	=				strData.GetLength();

	if ( nLen == nTotal )
	{
		return strData;
	}

	if ( nLen > nTotal )
	{
		for( int i = nTotal; i < nLen; i++)
		{
			strData += " ";
		}
	}
	else
	{
		return strData.Mid( 0, nLen );
	}

	return strData;
}
//---------------------------------------------------------------------------------
// ���̿� �°� �����͸� �߰��Ѵ�.
//---------------------------------------------------------------------------------
void CUtilityClass::m_fnSetCharEdit( char* szSrc, char* szTarget, int nLen )
{
	CString strData("");
	int     nTotal      = 0;

	memset( szSrc, 0x20, nLen );
	strData.Format("%s", szTarget );
	nTotal	= strData.GetLength();

	if ( nLen <= nTotal )
	{
		memcpy( szSrc, szTarget, nLen );
		szSrc[ nLen ] = NULL;
		return;
	}
	
	if ( nLen > nTotal )
	{
		memcpy( szSrc, szTarget, nTotal );
		szSrc[ nLen ] = NULL;
	}
}

/*-----------------------------------------------------------------------
Func Name : m_fnAdjustPrivilege
Reference : 
-----------------------------------------------------------------------*/
BOOL CUtilityClass::m_fnAdjustPrivilege(LPCTSTR privilege, DWORD attribute)
{
	BOOL ret;
	HANDLE procToken;
	LUID luid;
	TOKEN_PRIVILEGES tp;
  
	// get a handle to the access token used
	// by this process
	ret = OpenProcessToken( GetCurrentProcess(),
							TOKEN_ADJUST_PRIVILEGES, 
							&procToken );
	if ( !ret )
		return( FALSE );

	// get LUID for privilege
	ret = LookupPrivilegeValue( NULL, privilege, &luid );
	if ( !ret )
	{
		CloseHandle( procToken );
		return( FALSE );
	}

	// fill in token privileges struct
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = attribute;
  
	// attempt to enable privilege
	AdjustTokenPrivileges( procToken, FALSE, &tp, 
		                   NULL, NULL, NULL );

	if ( GetLastError() != ERROR_SUCCESS )
		ret = FALSE;
	else
		ret = TRUE;

	CloseHandle( procToken );

	return ret;
}

//---------------------------------------------------------------------------------
// ������ �����Ѵ�.
//---------------------------------------------------------------------------------
BOOL CUtilityClass::Copy(LPCTSTR lpSource, LPCTSTR lpDestination, BOOL bHidePrompt)
{
	// Enlightened by jbarton on Codeproject.com:
	// SHFileOperation expects character strings with "double-null terminators",
	// thus LPCTSTR's that are passed in as parameters cannot be directly used
	// by SHFileOperation. We need to duplicate the strings in order to make sure
	// that they are "double-null terminated".
	TCHAR szSource[MAX_PATH + 2] = _T("");
	TCHAR szDestination[MAX_PATH + 2] = _T("");
	::_tcsncpy_s(szSource, lpSource, MAX_PATH);
	::_tcsncpy_s(szDestination, lpDestination, MAX_PATH);

	// Copy should not be able to affect any directories, just like
	// how "copy" works in DOS. If need to copy directories, use 
	// XCopy instead.
	if (Existence(szSource) == FM_DIRECTORY)
	{
		::SetLastError(RC_ERR_DIRECTORY);
		return FALSE;
	}

	SHFILEOPSTRUCT fs;
	::memset(&fs, 0, sizeof(SHFILEOPSTRUCT));

	fs.pFrom = szSource;
	fs.pTo = szDestination;
	fs.wFunc = FO_COPY;
	fs.fFlags = FOF_FILESONLY;

	if (bHidePrompt)
		fs.fFlags |= (FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR);

	// This does the real job...
	return (::SHFileOperation(&fs) == 0);
}

//---------------------------------------------------------------------------------
// ������ �����Ѵ�.
//---------------------------------------------------------------------------------
BOOL CUtilityClass::Del(LPCTSTR lpSource, BOOL bHidePrompt)
{
	TCHAR szSource[MAX_PATH + 2] = _T("");
	::_tcsncpy_s(szSource, lpSource, MAX_PATH);

	// Del should not be able to affect any directories, just like
	// how "del" works in DOS. If need to delete directories, use 
	// DelTree instead.
	if (Existence(szSource) == FM_DIRECTORY)
	{
		::SetLastError(RC_ERR_DIRECTORY);
		return FALSE;
	}

	SHFILEOPSTRUCT fs;
	::memset(&fs, 0, sizeof(SHFILEOPSTRUCT));

	fs.pFrom = szSource;
	fs.wFunc = FO_DELETE;
	fs.fFlags = FOF_FILESONLY;

	if (bHidePrompt)
		fs.fFlags |= (FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR);

	return (::SHFileOperation(&fs) == 0);
}

//---------------------------------------------------------------------------------
// ���ϸ��� �����Ѵ�.
//---------------------------------------------------------------------------------
BOOL CUtilityClass::Ren(LPCTSTR lpSource, LPCTSTR lpDestination, BOOL bHidePrompt)
{
	TCHAR szSource[MAX_PATH + 2] = _T("");
	TCHAR szDestination[MAX_PATH + 2] = _T("");
	::_tcsncpy_s(szSource, lpSource, MAX_PATH);
	::_tcsncpy_s(szDestination, lpDestination, MAX_PATH);

	// Ren should not be able to affect any directories, just like
	// how "ren" works in DOS. If need to rename directories, use 
	// Move instead.
	if (Existence(szSource) == FM_DIRECTORY)
	{
		::SetLastError(RC_ERR_DIRECTORY);
		return FALSE;
	}

	SHFILEOPSTRUCT fs;
	::memset(&fs, 0, sizeof(SHFILEOPSTRUCT));

	fs.pFrom = szSource;
	fs.pTo = szDestination;
	fs.wFunc = FO_RENAME;
	fs.fFlags = FOF_FILESONLY;
	
	if (bHidePrompt)
		fs.fFlags |= (FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR);

	return (::SHFileOperation(&fs) == 0);
}


//---------------------------------------------------------------------------------
// ������ �����Ѵ�.
//---------------------------------------------------------------------------------
BOOL CUtilityClass::MkDir(LPCTSTR lpDirectory)
{
	return ::CreateDirectory(lpDirectory, NULL);
}

//---------------------------------------------------------------------------------
// ������ �����Ѵ�.
//---------------------------------------------------------------------------------
BOOL CUtilityClass::RmDir(LPCTSTR lpDirectory)
{
	return ::RemoveDirectory(lpDirectory);
}

//---------------------------------------------------------------------------------
// ������ �����Ѵ�.
//---------------------------------------------------------------------------------
int CUtilityClass::m_fnSetCreateDir(CString strFolder)
{
	CFileStatus	clStatus;
	CString		strDir("");
	int			nIndex = 0;

	// Space Charactor Del
	while ( ( nIndex = strFolder.Find(STR_SPACE) ) >= 0 )
	{
		strFolder = strFolder.Left( nIndex ) + STR_UNDERLINE + strFolder.Right( strFolder.GetLength() - nIndex - 1 );
	}

	if ( ( nIndex = strFolder.Find(STR_COLON) ) >= 0 )
	{
		// Get Drive Name
		strDir      = strFolder.Left( nIndex + 1 );
		strFolder = strFolder.Right(strFolder.GetLength() - nIndex - 1 );
	}
	if ( strFolder.Find(STR_BACKSLASH) == 0 )
	{
		strDir     += STR_BACKSLASH;
		strFolder = strFolder.Right( strFolder.GetLength() - 1 );
	}

	while ( ( nIndex = strFolder.Find(STR_BACKSLASH) ) >= 0 )
	{
		strDir     += strFolder.Left( nIndex );
		strFolder = strFolder.Right( strFolder.GetLength() - nIndex - 1 );

		if( !CFile::GetStatus( (const char*)strDir, clStatus ) )
		{
			// Not Found Folder
			if ( CreateDirectoryA( (const char*)strDir, RC_NORMAL ) != 0 )
			{
				// �̹� ���� ����� ���� ������ �ۼ� �Ѵ�
				// ������ ����� ������ ���߷��� �ϱ� ������ Comment�� �����Ѵ�.
				// return RC_FILE_ACCESS_ERR;
			}
		}
		strDir += STR_BACKSLASH;
	}
	return RC_NORMAL;

}

//---------------------------------------------------------------------------------
// ���Ͽ� �Ӽ��� �����Ѵ�.
//---------------------------------------------------------------------------------
BOOL CUtilityClass::SetAttribute(LPCTSTR lpSource, DWORD dwNewAttr)
{
	return ::SetFileAttributes(lpSource, dwNewAttr);
}

//---------------------------------------------------------------------------------
// ���Ͽ� �Ӽ��� �о��Ѵ�.
//---------------------------------------------------------------------------------
DWORD CUtilityClass::GetAttribute(LPCTSTR lpSource)
{
	return ::GetFileAttributes(lpSource);
}


//---------------------------------------------------------------------------------
// ���� / ������ ���� ���θ� Ȯ���Ѵ�.
//---------------------------------------------------------------------------------
int CUtilityClass::Existence(LPCTSTR lpSource)
{
	WIN32_FIND_DATA fd;
	::memset(&fd, 0, sizeof(WIN32_FIND_DATA));

	HANDLE hFile = ::FindFirstFile(lpSource, &fd);

	// Not exists.
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FM_NOTEXIST;
	}

	// Exists, but file or directory?
	
	if (fd.dwFileAttributes == (fd.dwFileAttributes | FILE_ATTRIBUTE_DIRECTORY))
	{
		while (::FindNextFile(hFile, &fd))
		{
			// There is at least a file with this name.
			if (fd.dwFileAttributes != (fd.dwFileAttributes | FILE_ATTRIBUTE_DIRECTORY))
			{
				::FindClose(hFile);
				return FM_FILE;
			}
		}

		// Exists but there's no file with this name, so it's a directory.
		::FindClose(hFile);
		return FM_DIRECTORY;
	}
	else
	{
		// Exists and is not a directory, so it's a file.
		::FindClose(hFile);
		return FM_FILE;
	}
}

/*-----------------------------------------------------------------------
Func Name : m_fnGetToday
Reference : m_fnGetToday
-----------------------------------------------------------------------*/
CString CUtilityClass::m_fnGetToday()
{
	CTime	TCurrentTime;	 
	CString strToday("");

	TCurrentTime = TCurrentTime.GetCurrentTime();
	strToday	 = TCurrentTime.Format("%Y%m%d"); 
	return strToday;
}

/*-----------------------------------------------------------------------
Func Name : XorConvert
Reference : XorConvert
-----------------------------------------------------------------------*/
CString CUtilityClass::HexaToDecimalString( CString strSRC )
{
	CString strResult("");
	strSRC = "0x" + strSRC; 
	double dResult = strtol( strSRC, NULL, 0 );
	strResult.Format( "%.f", dResult );
	return strResult;
}
/*-----------------------------------------------------------------------
Func Name : XorConvert
Reference : XorConvert
-----------------------------------------------------------------------*/
static  char* szHEXA[] = { "0","1","2","3","4","5","6","7", "8","9","A","B","C","D","E","F" };
BOOL CUtilityClass::XorConvert( BYTE * bySRC, int nbyLen, BYTE& byRESULT, CString& strResult )
{
	byRESULT = NULL;

	if ( nbyLen == 0 )
	{
		return FALSE;
	}

	for ( int i = 0; i < nbyLen; i++ )
	{
		byRESULT ^= bySRC[i];
	}

	int nBCC1 = byRESULT >> 4;
	int nBCC2 = byRESULT & 0x0f;

	if (( nBCC1 < 0 ) ||
		( nBCC1 > 15 ))
	{
		return FALSE;
	}
	if (( nBCC2 < 0 ) ||
		( nBCC2 > 15 ))
	{
		return FALSE;
	}

	strResult = szHEXA[nBCC1];
	strResult += szHEXA[nBCC2];

	return TRUE;
}

/*-----------------------------------------------------------------------
Func Name : CharToHex
Reference : 
-----------------------------------------------------------------------*/
BOOL CUtilityClass::CharToHex( BYTE* byBuffer, int nLen, CString& strResult )
{
	strResult.Empty();
	CString strData("");

	if ( nLen == 0 ) return TRUE;

	for( int i = 0; i < nLen; i++ )
	{
		strData.Format( "%02x ", byBuffer[i] );
		strResult += strData;
	}
	return TRUE;
}

/*-----------------------------------------------------------------------
Func Name : CharToHex
Reference : 
-----------------------------------------------------------------------*/
BOOL CUtilityClass::CharToHex( char* szBuffer, int nLen, CString& strResult )
{
	strResult.Empty();
	CString strData("");

	if ( nLen == 0 ) return TRUE;

	for( int i = 0; i < nLen; i++ )
	{
		strData.Format( "%02x ", szBuffer[i] );
		strResult += strData;
	}
	return TRUE;
}

CString CUtilityClass::GetIPAddress(void)
{
	CString sIPAddress = _T("");

// 	char szHostName[80];
// 	if(gethostname(szHostName, sizeof(szHostName)) == SOCKET_ERROR)
// 	{
// 		AfxMessageBox(TEXT("GetHostName Function Error"));
// 		return sIPAddress;
// 	}
// 
// 	struct hostent* hostinfo = gethostbyname(szHostName);
// 	if(hostinfo == 0)
// 	{
// 		AfxMessageBox(TEXT("Yow! Bad host lookup."));
// 		return sIPAddress;
// 	}
// 
// 	for(int i = 0; hostinfo->h_addr_list[i] != 0; ++i)
// 	{
// 		sIPAddress = inet_ntoa(*(struct in_addr *)hostinfo->h_addr_list[i]);
// 	}

	return sIPAddress;
}

void CUtilityClass::m_fnGetIPConvert(CString strIPNum, int* nFilds )
{
	CString strFild("");
	CString strData("");

	int iFind        = 0;
	strData         = strIPNum;
	memset( nFilds,0x00,sizeof(nFilds));

	for(int i = 0; i < 4 ; i++)
	{
		iFind = strData.Find(".");
		if(iFind <0)
		{
			nFilds[i] = atoi(strData);
			break;
		}
		strFild = strData.Left(iFind);
		nFilds[i] = atoi(strFild);
		strData = strData.Right(strData.GetLength() - iFind -1);
	}
}

/*-----------------------------------------------------------------------
Func Name : m_fnGetDeliverFileName() 
Reference :
/*-----------------------------------------------------------------------*/
INT_PTR CUtilityClass::m_fnGetFileList( CString strPath, CString strName, CStringArray& strFile )
{
	CString strFullPath("");
	CString strFileName("");

	strFullPath.Format("%s%s", strPath, strName );

	HANDLE hFind;
	WIN32_FIND_DATA fd;

	if( (hFind = ::FindFirstFile( (LPCTSTR)strFullPath, &fd)) != INVALID_HANDLE_VALUE )
	{
		strFileName = (LPCTSTR)&fd.cFileName;
		strFile.Add( strFileName );
		while( (::FindNextFile(hFind, &fd)) )
		{
		//	if (fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE )
		//	{
				strFileName = (LPCTSTR)&fd.cFileName;
				strFile.Add( strFileName );
		//	}
		}
		::FindClose(hFind);
	}

	return strFile.GetSize();
}
 
#include "Tlhelp32.h"
void CUtilityClass::KillProcess(const char *EXEName)
{
	HANDLE snapshot_handle = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE != snapshot_handle)
	{
		PROCESSENTRY32 pe;
		if (Process32First(snapshot_handle, &pe))
		{
			do
			{
				//TRACE("KillProcess() PID = %04u, FileName = %s\n", pe.th32ProcessID, pe.szExeFile);
				if (!_tcscmp(pe.szExeFile, (LPCTSTR)EXEName))
				{
					HANDLE process_handle = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
					if (INVALID_HANDLE_VALUE != process_handle)
					{
						TRACE("���μ��� ����!\n");
						TerminateProcess(process_handle, 0);
						CloseHandle(process_handle);
					}
				}
			} while (Process32Next(snapshot_handle, &pe));
		}

		CloseHandle(snapshot_handle);
		snapshot_handle = INVALID_HANDLE_VALUE;
	} 
}

/*-----------------------------------------------------------------------
Func Name : m_fnGetTackTime() 
Reference : 
/*-----------------------------------------------------------------------*/
CString CUtilityClass::m_fnGetTackTime( double dwTactTime )
{
	int nSec			= (int)(dwTactTime / 1000.);
	int nMin			= (int)(dwTactTime / ( 60. * 1000. ));	 
	int nHr				= (int)(dwTactTime / ( 60. * 60. * 1000. ));

	int nMiliSecond		= 0;
	int nSecond   		= 0;
	int nMinute  		= 0;
	int nHour			= 0;

	if ( dwTactTime >= 1000 )
	{
		nMiliSecond		= (int)(dwTactTime - ( nSec * 1000 ));
	}
	else
	{
		nMiliSecond		= (int)dwTactTime;
	}

	if ( nSec >= 60 )
	{
		nSecond   		= nSec - ( ( nSec /60 ) * 60 );
	}
	else
	{
		nSecond   		= nSec;
	}
	if ( nMin >= 60 )
	{
		nMinute  		= nMin - ( ( nMin / 60 ) * 60 );
	}
	else
	{
		nMinute  		= nMin;
	}
	if ( nHr >= 24 )
	{
		nHour       	= nHr - ( ( nHr / 24 ) * 24 );
	}
	else
	{
		nHour       	= nHr;
	}

	CString             strTactTime("");
	strTactTime.Format( "%02d:%02d:%02d.%03d", nHour, nMinute, nSecond, nMiliSecond );
	return strTactTime;
}

//======================================================================================================
// FULL NAME���� �̸��� �����Ѵ�. ex) EXCEL.EXE
//======================================================================================================
void CUtilityClass::KillProcess( )
{
	DWORD dwSize = 250;
	HANDLE hSnapShot;
	PROCESSENTRY32 pEntry;
	BOOL bCrrent=FALSE;

	hSnapShot=CreateToolhelp32Snapshot(TH32CS_SNAPALL,NULL);
	pEntry.dwSize =sizeof(pEntry);
	Process32First (hSnapShot,&pEntry);
	//
	while(1)
	{
		// ������ ���μ����� ������ �����´�.
		BOOL hRes=Process32Next (hSnapShot,&pEntry);

		if(hRes==FALSE)
			break;        
		if(!strncmp(pEntry.szExeFile,"EXCEL.EXE",8))    // find EXCEL.EXE
		{
			bCrrent = TRUE;
		}
		if(bCrrent)
		{
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pEntry.th32ProcessID);
			if(hProcess)
			{
				if(TerminateProcess(hProcess, 0))
				{
					unsigned long nCode; //���μ��� ���� ����
					GetExitCodeProcess(hProcess, &nCode);
				}
				CloseHandle(hProcess);
			}
			break;
		}

	}
}

////////////////////////////////////////////////////////////////////////////////////
// ��� �� ���
////////////////////////////////////////////////////////////////////////////////////
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>

float CUtilityClass::GetAverage(float* set, int num)
{
	float average = 0.0;
	int i = 0;
	for (i=0; i<=num; i++)
	{
		if ( i == 0 )
		{
			average = set[i];
		}
		else
		{
			average += set[i];
		}
	}
	//��� (average) : ������ ������ ������ ���� �� 
	if ( num > 0 )
	{
		average = float(average/num);
	}
	return average;
}

float CUtilityClass::GetMin(float* set, int num)
{
	float fMin = 0.0;
	int i = 0;
	for (i=0; i<=num; i++)
	{
		if ( i == 0 )
		{
			fMin = set[i];
		}
		if ( fMin > set[i] )
		{
			fMin = set[i];
		}
	}
	return fMin;
}

float CUtilityClass::GetMax(float* set, int num)
{
	float fMax = 0.0;
	int i = 0;
	for (i=0; i<=num; i++)
	{
		if ( i == 0 )
		{
			fMax = set[i];
		}
		if ( fMax < set[i] )
		{
			fMax = set[i];
		}
	}
	return fMax;
}

float CUtilityClass::GetVariance(float* set, int num, float average)
{
	float deviation = 0.0;
	float variance = 0.0;
	int i = 0;

	for (i=0; i<num; i++)
	{
		// ����(deviation) : ��տ��� ������ ���� ������ ����
		deviation = set[i] - average;
		printf("deviation set[%d] = %.2f\n", i, deviation);

		variance += deviation * deviation;
	}
	// �л�(variance) : ������ �������� ���� ������ ���� ��
	variance = float(variance/num);
	return variance;
}


float CUtilityClass::GetStandardDeviation(float variance)
{
	float sd = 0.0;
	// ǥ������(standard deviation) : �л��� ������
	sd = sqrt(variance);
	return sd;
}

wchar_t* CUtilityClass::ConverCtoWC(char* str)
{
	//wchar_t�� ���� ����
	wchar_t* pStr;

	//��Ƽ ����Ʈ ũ�� ��� ���� ��ȯ
	int strSize = MultiByteToWideChar(CP_ACP, 0,str, -1, NULL, NULL);

	//wchar_t �޸� �Ҵ�
	pStr = new WCHAR[strSize];

	//�� ��ȯ
	MultiByteToWideChar(CP_ACP, 0,str, strlen(str)+1, pStr, strSize);

	return pStr;

}


float CUtilityClass::GetCoVariance(float* setX, float* setY, int num, float averageX, float averageY)
{
	float deviation = 0.0;
	float covariance = 0.0;
	int i = 0;
	for (i=0; i<num; i++)
	{
		covariance += (setX[i] - averageX) * (setY[i] - averageY);
	}
	// ���л�(covariance) : setX�� setY�� ���������� ���� ������ ���� ��
	covariance = float(covariance/num);
	return covariance;
}

#if OPENCV
void CUtilityClass::setPutText(IplImage* src_image, IplImage* dst_image, char* text, CvPoint textPos, CvScalar color)

{
	if( src_image->nChannels != 1 )
		cvCopy( src_image, dst_image );

	int width = src_image->width;
	int height = src_image->height;

	if( (textPos.y > src_image->height) || (textPos.x > src_image->width) )
	{
		cvCopy( src_image, dst_image );
	}

	CvScalar var;
	HBITMAP hBitmap = setTextToBitmap(text, color);
	IplImage* text_image = nullptr;
	setBitmapToIplImage( hBitmap, &text_image);

	if (text_image == nullptr)
		return;

	for( int nRow = textPos.y; nRow<(textPos.y + text_image->height); nRow++)
	{
		for( int nCol = textPos.x; nCol<(textPos.x + text_image->width); nCol++)
		{
			if( nRow >= height || nCol >= width)	continue;
			var = cvGet2D(text_image, nRow-textPos.y, nCol-textPos.x);
			if( var.val[0] == color.val[0] && var.val[1] == color.val[1] && var.val[2] == color.val[2] )
				cvSet2D(dst_image, nRow, nCol, var);
		}
	}

	free(text_image->imageData);
	cvReleaseImage( &text_image );
	DeleteObject( hBitmap );
}

HBITMAP CUtilityClass::setTextToBitmap( char* text, CvScalar color)
{
	int textLength = (int)strlen(text);
	if( textLength <= 0 ) return NULL;
	HDC hTextDC = CreateCompatibleDC(NULL);
	HFONT hOldFont = (HFONT)SelectObject( hTextDC, (HFONT)GetStockObject(SYSTEM_FONT) );
	HBITMAP hDstBMP = NULL;
	RECT textArea = {0, 0, 0, 0};

	DrawText(hTextDC, (LPCTSTR)text, textLength, &textArea, DT_CALCRECT);

	if( (textArea.right > textArea.left) && (textArea.bottom > textArea.top) )
	{
		BITMAPINFOHEADER bitmapInfoHeader;
		memset( &bitmapInfoHeader, 0x0, sizeof(BITMAPINFOHEADER) );
		void* lpvBits = NULL;

		bitmapInfoHeader.biSize		= sizeof(bitmapInfoHeader);
		bitmapInfoHeader.biWidth	= textArea.right-textArea.left;
		bitmapInfoHeader.biHeight	= textArea.bottom-textArea.top;
		bitmapInfoHeader.biPlanes	= 1;
		bitmapInfoHeader.biBitCount	= 32;
		bitmapInfoHeader.biCompression = BI_RGB;

		hDstBMP = CreateDIBSection( hTextDC, (LPBITMAPINFO)&bitmapInfoHeader, 0, (LPVOID*)&lpvBits, NULL, 0 );

		HBITMAP hOldBMP = (HBITMAP)SelectObject( hTextDC, hDstBMP );
		if( hOldBMP != NULL)
		{
			int TEXT_RED = (int)color.val[2];
			int TEXT_GREEN = (int)color.val[1];
			int TEXT_BLUE = (int)color.val[0];
			SetTextColor( hTextDC, RGB(TEXT_RED, TEXT_GREEN, TEXT_BLUE) );	//text color
			SetBkColor( hTextDC, 0x00EFFEFF );	//Background color
			SetBkMode( hTextDC, OPAQUE ); // ����
			DrawText( hTextDC, (LPCTSTR)text, textLength, &textArea, DT_NOCLIP);
		}
		::SelectObject( hTextDC, hOldBMP );
	}

	::SelectObject( hTextDC, hOldFont );

	if( hTextDC) { ::DeleteDC( hTextDC ); }
	if( hOldFont) { ::DeleteObject( hOldFont ); }

	return hDstBMP;

}

void CUtilityClass::setBitmapToIplImage( HBITMAP hBitmap, IplImage** mImage )
{
	if (hBitmap == NULL)
		return;

	BITMAP bitmap;
	GetObject( hBitmap, sizeof( BITMAP ), (LPSTR)&bitmap );

	int nChannels = (bitmap.bmBitsPixel == 1)? 1: (int)(bitmap.bmBitsPixel*0.125);
	int depth = (bitmap.bmBitsPixel == 1)? IPL_DEPTH_1U : IPL_DEPTH_8U;

	//IplImage* 
	if (bitmap.bmWidth <= 0)
		return;
	if (bitmap.bmHeight <= 0)
		return;

		*mImage = cvCreateImageHeader( cvSize(bitmap.bmWidth, bitmap.bmHeight), depth, nChannels );

	(*mImage)->imageData = (char*)malloc(bitmap.bmHeight * bitmap.bmWidth * nChannels * sizeof(char) );
	memcpy((*mImage)->imageData, (char*)(bitmap.bmBits), bitmap.bmHeight * bitmap.bmWidth * nChannels );

	cvFlip( *mImage, *mImage, 0 );

	return;// mImage;
}
#endif
CString CUtilityClass:: FormatStr(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	CString sTemp;
	sTemp.FormatV(fmt, args);
	va_end(args);

	return (sTemp);
}

void  CUtilityClass::CountingImgFile(LPCTSTR directory, int bdNo, LPCTSTR FileFormat, int *pCnt, int mod)
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

void  CUtilityClass::CreateDir(char* Path)
{
	char DirName[256];
	char* p = Path;
	char* q = DirName;   

	while(*p) {
		if (('\\' == *p) || ('/' == *p)) {
			if (':' != *(p-1)) {
				CreateDirectory(DirName, NULL);
			}
		}
		*q++ = *p++;
		*q = '\0';
	}
	CreateDirectory(DirName, NULL);
}

int CUtilityClass::m_fnComboFindString(CComboBox* pcl, CString strData )
{
	CString strFild("");
	for( int i = 0; i< pcl->GetCount();i++)
	{
		pcl->GetLBText(i, strFild );
		if ( strFild == strData ) return i;
	}
	return -1;
}

int CUtilityClass::m_fnListBoxFindString(CListBox* pcl, CString strData )
{
	CString strFild("");
	for( int i = 0; i< pcl->GetCount();i++)
	{
		pcl->GetText( i, strFild );
		if ( strFild == strData ) return i;
	}
	return -1;
}

BSTR CUtilityClass::CStringTobstr( CString str )
{
	// 	return str.AllocSysString(); //CString�� BSTR�� convert
	// 	//BSTR�� ����� ���� �� �ݵ�� SysFreeString()�Լ��� ȣ���Ͽ� �����ؾ��Ѵ�.
	BSTR  bstr = str.AllocSysString();
	SysFreeString( bstr ); //<-- �ݵ�� ���־�� �Ѵ�.
	return bstr;
}

char* CUtilityClass::TCHARToChar(TCHAR *str)
{
	char *szStrCv = NULL;
#if defined(UNICODE) || defined(_UNICODE)
	int nSize = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, NULL, NULL, NULL);
	if (szStrCv)
	{
		delete[] szStrCv;
		szStrCv = NULL;
	}
	szStrCv = new char[nSize];
	memset(szStrCv, 0x00, nSize);
	WideCharToMultiByte(CP_ACP, 0, str, -1, szStrCv, nSize, NULL, NULL);
#else
	int nLen = strlen(str) + 1;
	szStrCv = new char[nLen];
	memset(szStrCv, 0x00, nLen);
// 	strcpy_s(szStrCv, str);
	sprintf_s(szStrCv, sizeof(char)*nLen, "%s", str);
#endif
	return szStrCv;
}


#pragma comment(lib, "Version.lib") 
CString CUtilityClass::m_fnGetVersionInfo()
{
	char szModuleName[FILE_LEN];
	GetModuleFileName(AfxGetApp()-> m_hInstance,szModuleName,MAX_PATH);

	DWORD dwDummy,dwInfoSize;
	dwInfoSize = GetFileVersionInfoSize(szModuleName,&dwDummy);
	char* pszVersion = new char[dwInfoSize + 2];
	GetFileVersionInfo(szModuleName,0,dwInfoSize,pszVersion);

	unsigned int dwLen;
	void* pszVer;
	VerQueryValue(pszVersion,"\\",&pszVer,&dwLen);
	VS_FIXEDFILEINFO *pfi = (VS_FIXEDFILEINFO*)pszVer;

	DWORD dwMS = pfi->dwFileVersionMS;
	DWORD dwLS = pfi->dwFileVersionLS;
	CString strVer,strBuild;
	strVer.Format("%d.%d.%d.%d",HIWORD(dwMS),LOWORD(dwMS),
								HIWORD(dwLS),LOWORD(dwLS));
	if(pszVersion)
		delete pszVersion;

	return strVer;
}

