// AutoFileDelDLG.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EllipsoMeter.h"
#include "AutoFileDelDLG.h"
#include "afxdialogex.h"


// CAutoFileDelDLG 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAutoFileDelDLG, CDialogEx)

CAutoFileDelDLG::CAutoFileDelDLG(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAutoFileDelDLG::IDD, pParent)
{
	m_bShow = FALSE;

	this->Create( IDD_AUTO_DEL );

	m_hAutoDelThread = NULL;
	m_bAutoDelEnd    = FALSE;

}

CAutoFileDelDLG::~CAutoFileDelDLG()
{
}

void CAutoFileDelDLG::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAutoFileDelDLG, CDialogEx)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_PATH01, &CAutoFileDelDLG::OnBtnPath01)
	ON_BN_CLICKED(IDC_BTN_PATH02, &CAutoFileDelDLG::OnBtnPath02)
	ON_BN_CLICKED(IDC_BTN_PATH03, &CAutoFileDelDLG::OnBtnPath03)
	ON_BN_CLICKED(IDC_BTN_PATH04, &CAutoFileDelDLG::OnBtnPath04)
	ON_BN_CLICKED(IDC_BTN_PATH05, &CAutoFileDelDLG::OnBtnPath05)
	ON_BN_CLICKED(IDC_BTN_PATH06, &CAutoFileDelDLG::OnBtnPath06)
	ON_BN_CLICKED(IDC_BTN_PATH07, &CAutoFileDelDLG::OnBtnPath07)
	ON_BN_CLICKED(IDC_BTN_PATH08, &CAutoFileDelDLG::OnBtnPath08)
	ON_BN_CLICKED(IDC_BTN_PATH009, &CAutoFileDelDLG::OnBtnPath009)
	ON_BN_CLICKED(IDC_BTN_PATH10, &CAutoFileDelDLG::OnBtnPath10)
	ON_BN_CLICKED(IDC_BTN_SAVE2, &CAutoFileDelDLG::OnBtnSave)
	ON_BN_CLICKED(IDC_BTN_HIDE, &CAutoFileDelDLG::OnBnClickedBtnHide)
END_MESSAGE_MAP()

BOOL CAutoFileDelDLG::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (( nID   == 2 ) &&
		( nCode == -1 ))
	{
		m_fnSetShowFlag( FALSE );
	}
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CAutoFileDelDLG::m_fnSetShowFlag( BOOL bShow )
{
	m_bShow = bShow;
	if ( m_bShow )
	{
		this->ShowWindow( SW_SHOW );
	}
	else
	{
		this->ShowWindow( SW_HIDE );
	}
}

BOOL CAutoFileDelDLG::PreTranslateMessage(MSG* pMsg) 
{
	// ENTER KEY or ESC KEY CANCEL
	if (( pMsg->wParam == 13 ) || 
		( pMsg->wParam == 27 ))
	{
		return FALSE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

//---------------------------------------------------------------------------------------------------------
// 시작에 필요한 작업을 한다.
//---------------------------------------------------------------------------------------------------------
void CAutoFileDelDLG::m_fnSetInit( ) 
{
	CString s("");
	s.Format( "자동 파일 삭제" );
	this->SetWindowText(s);

	CString strData("");
	CString strFullPath("");
	CString strWork("");
	CString strTemp("");

	m_strToday			= _T("");

	int  i				= 0;

	strFullPath.Format( "%s%s%s%s", m_Util.m_fnGetCurrentDrive(), DEFAULT_PATH, DEFAULT_ENV_PATH, DEL_FILE_NAME );

	CStringArray strLst;
	CXmlElement *pElement     = NULL;
	CXmlElement *pRootElement = NULL;

	CXmlDocument* pXML = new CXmlDocument();
	pXML->Load( strFullPath );

	pXML->GetTagToString( AUTO_FILE_DEL , strData );
	pXML->Parse( strData );
	pRootElement = pXML->GetRootElement();
	if ( pRootElement == NULL )
	{
		delete pXML;
		pXML = NULL;
		return;
	}

	strLst.RemoveAll();
	strLst.Add(ELEMENT);
	pElement = pXML->FindElement( pRootElement, strLst );
	if ( pElement != NULL )
	{
		for( i = 1; i <= MAX_CNT; i++ )
		{
			strWork.Format( "%s_%01d", DEL_FILE_USE, i );
			pXML->FindData( pElement, strWork, strTemp );
			strTemp.TrimLeft();
			strTemp.TrimRight();

			// 구조체 초기화
			m_stDEL[i-1].bUSE			= FALSE;
			memset( m_stDEL[i-1].szPATH, 0x00, sizeof( m_stDEL[i-1].szPATH ));
			memset( m_stDEL[i-1].szExt, 0x00, sizeof( m_stDEL[i-1].szExt ));
			m_stDEL[i-1].nDays			= 0;

			if ( strTemp == "YES" )
			{
				m_stDEL[i-1].bUSE		= TRUE;
				((CButton*)GetDlgItem( nUSE[i-1] ))->SetCheck( TRUE );

				// PATH
				strWork.Format( "%s_%01d", DEL_FILE_PATH, i );
				pXML->FindData( pElement, strWork, strTemp );
				strTemp.TrimLeft();
				strTemp.TrimRight();
				sprintf_s( m_stDEL[i-1].szPATH, "%s", strTemp.GetString());
				GetDlgItem( nPATH[i-1] )->SetWindowText( strTemp );

				// DAY
				strWork.Format( "%s_%01d", DEL_DAY, i );
				pXML->FindData( pElement, strWork, strTemp );
				strTemp.TrimLeft();
				strTemp.TrimRight();
				m_stDEL[i-1].nDays = atoi( strTemp );
				GetDlgItem( nDAYS[i-1] )->SetWindowText( strTemp );

				// EXT
				strWork.Format( "%s_%01d", DEL_FILE_EXT, i );
				pXML->FindData( pElement, strWork, strTemp );
				strTemp.TrimLeft();
				strTemp.TrimRight();
				sprintf_s( m_stDEL[i-1].szExt, "%s", strTemp.GetString());
				GetDlgItem( nEXT[i-1] )->SetWindowText( strTemp );
			}
		}
		delete pXML;
		pXML = NULL;
	}

// 	m_fnAutoDelProcess();

// 	SetTimer( 1, 1000 * 60 * 60, NULL );
}

//---------------------------------------------------------------------------------------------------------
// 시작에 필요한 작업을 한다.
//---------------------------------------------------------------------------------------------------------
void CAutoFileDelDLG::m_fnSetEnd( ) 
{
	if ( m_hWnd != NULL )
	{
		KillTimer( 1 );
		m_bAutoDelEnd			= FALSE;
		SuspendThread( m_hAutoDelThread );
		CloseHandle(m_hAutoDelThread);
		m_hAutoDelThread	    = NULL;
	}

}

/*-----------------------------------------------------------------------
Func Name : m_fnGetToday
Reference : m_fnGetToday
-----------------------------------------------------------------------*/
CString CAutoFileDelDLG::m_fnGetToday()
{
	CTime	TCurrentTime;	 
	CString strToday("");

	TCurrentTime = TCurrentTime.GetCurrentTime();
	strToday	 = TCurrentTime.Format("%Y%m%d"); 
	return strToday;
}

/*-----------------------------------------------------------------------
Func Name : m_fnLogFind
Reference : m_fnLogFind
-----------------------------------------------------------------------*/
BOOL CAutoFileDelDLG::m_fnLogFind(CString cstrFilePath, CString strExt, int nDay)
{

	BOOL      bWorking		 = FALSE;
	BOOL      bWorkingSub    = FALSE;
	int       nRtn			 = 0;
	CString   strFileName("");
	CString   strFileNameSub("");
	CString   strCreateTime("");
	CFileFind finder;
	CFileFind finderSub;
	CTime     clTime;

	CString strCurTime = m_fnGetCurrentTime3(nDay);
	strCurTime         = strCurTime.Left(8);

	cstrFilePath       += "*.*";
	bWorking = finder.FindFile(cstrFilePath);
	while(bWorking)
	{
		bWorking = finder.FindNextFile();

		if (finder.IsDots())
		{
			continue;
		}

		if (finder.IsDirectory())
		{
			strFileNameSub = finder.GetFilePath();

			if (m_fnLogFind(finder.GetFilePath() + "\\*.*", strExt, nDay))
				return TRUE;
			nRtn = _rmdir(strFileNameSub);
		}
		else
		{
			strFileName     = finder.GetFilePath();
			finder.GetCreationTime(clTime);

			strCreateTime	 = clTime.Format("%Y%m%d%H%M%S"); 
			strCreateTime   = strCreateTime.Left(8);
			// CurTime : 삭제 대상일자
			// CreTime : 생성일자
			if (strCreateTime < strCurTime)
			{
				CString str("");
				str = strFileName.Right(3);
				if ( strFileName.Right(3) == strExt )
				{
					SetFileAttributes(strFileName, FILE_ATTRIBUTE_ARCHIVE);	
					::DeleteFile(strFileName);
				}
			}
		}
	}

	return FALSE;

}

/*-----------------------------------------------------------------------
Func Name : m_fnGetCurrentTime3
Reference : m_fnGetCurrentTime3
-----------------------------------------------------------------------*/
CString CAutoFileDelDLG::m_fnGetCurrentTime3(int nDay)
{
	CTime	Ttime;			 
	CTime	TCurrentTime;	 
	CString cstrCurTime;	

	TCurrentTime = Ttime.GetCurrentTime() - CTimeSpan( nDay, 0, 0, 0 );
	cstrCurTime	 = TCurrentTime.Format("%Y%m%d%H%M%S"); 

	return cstrCurTime;
}

BOOL CAutoFileDelDLG::Del(LPCTSTR lpSource, BOOL bHidePrompt)
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
// 파일 / 폴더에 존재 여부를 확인한다.
//---------------------------------------------------------------------------------
int CAutoFileDelDLG::Existence(LPCTSTR lpSource)
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

void CAutoFileDelDLG::OnTimer(UINT nIDEvent) 
{
	if ( nIDEvent == 1  )
	{
// 		m_fnAutoDelProcess();
	}

	CDialog::OnTimer(nIDEvent);
}

void CAutoFileDelDLG::m_fnAutoDelProcess( )
{
	unsigned long	ThreadID;
	m_bAutoDelEnd			= TRUE;
	m_hAutoDelThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AutoDelThread, this, NULL, &ThreadID);
}

UINT AutoDelThread(LPVOID pFuncData)
{
	CAutoFileDelDLG		*pQI = (CAutoFileDelDLG *)pFuncData;

	while ( pQI->m_bAutoDelEnd )
	{
		if ( pQI->m_hWnd == NULL ) continue;

		if ( pQI->m_strToday != pQI->m_fnGetToday() )
		{
			pQI->m_strToday = pQI->m_fnGetToday();

			char szWork[256];
			CString strFolderPath("");
			CString strExtention("");
			int     nCheckDate    = 0;
			for( int i = 1 ; i <= MAX_CNT; i++ )
			{
				if ( pQI->m_stDEL[i-1].bUSE )
				{
					strFolderPath.Format( "%s", pQI->m_stDEL[i-1].szPATH );
					nCheckDate				= pQI->m_stDEL[i-1].nDays;
					strExtention.Format( "%s", pQI->m_stDEL[i-1].szExt );
					if (( !strFolderPath.IsEmpty() ) &&
						( !strExtention.IsEmpty()  ) &&
						( nCheckDate != 0 ))
					{
						memset( szWork, 0x00, sizeof(szWork));
						sprintf_s( szWork, "%s\\", strFolderPath.GetString());
						pQI->m_fnLogFind( szWork, strExtention, nCheckDate );
					}
				}
			}
			pQI->m_bAutoDelEnd = FALSE;
		}
		Sleep( 100 );
	}
	CloseHandle(pQI->m_hAutoDelThread);
	pQI->m_hAutoDelThread	= NULL;

	return 0;
}

CString CAutoFileDelDLG::m_fnGetFolderPath( )
{
	LPCTSTR lpszTitle = _T( "삭제 대상 폴더 선택" );

	UINT uFlags		  = BIF_RETURNONLYFSDIRS;

	CString strFullPath("");

	CFolderDialog dlg(lpszTitle, "D:\\", this, uFlags );

	if ( dlg.DoModal() == IDOK )
	{    
		strFullPath = dlg.GetFolderPath();    
	}
	return strFullPath;
}	

void CAutoFileDelDLG::OnBtnSave() 
{

	BOOL	bUSE					= FALSE;
	CString strPath("");
	CString strDays("");
	CString strExt("");

	CString strWork("");
	CString strTemp("");
	CString strData("");
	CString strFullPath("");

	int     i						= 0;

	// 항목 정합성 체크
	for( i = 1; i <= MAX_CNT; i++ )
	{
		bUSE = ((CButton*)GetDlgItem( nUSE[i-1] ))->GetCheck();
		if ( !bUSE )
		{
			break;
		}
		GetDlgItem( nPATH[i-1] )->GetWindowText( strPath );
		strPath.TrimLeft();
		strPath.TrimRight();
		GetDlgItem( nDAYS[i-1] )->GetWindowText( strDays );
		strDays.TrimLeft();
		strDays.TrimRight();
		GetDlgItem( nEXT[i-1] )->GetWindowText( strExt );
		strExt.TrimLeft();
		strExt.TrimRight();
		if (( strPath.IsEmpty() ) ||
			( strDays.IsEmpty() ) ||
			( strExt.IsEmpty() ))
		{
			TECmsgOut( L"미입력 항목을 모두 기입후 처리하세요!!!!!", GREEN_1, 1 );
			return;
		}
		int nDays = atoi( strDays );
		if ( nDays < 1 ) 
		{
			TECmsgOut( L"주기(일)은 1일 이상으로 입력 하세요!!!!!", GREEN_1, 1 );
			return;
		}
		int nExt = strExt.Find(".");
		if ( nExt != -1 )
		{
			TECmsgOut( L"확장지만 입력 하세요!!!!!", GREEN_1, 1 );
			return;
		}
	}

	// 파일 삭제
	// 경로 변경을 원하시면 stdafx.h에서 수정하세요.
	strFullPath.Format( "%s%s%s%s", m_Util.m_fnGetCurrentDrive(), DEFAULT_PATH, DEFAULT_ENV_PATH, DEL_FILE_NAME );

	CStringArray strLst;
	CXmlElement *pElement     = NULL;
	CXmlElement *pRootElement = NULL;

	CXmlDocument* pXML = new CXmlDocument();
	pXML->Load( strFullPath );

	pXML->GetTagToString( AUTO_FILE_DEL , strData );
	pXML->Parse( strData );
	pRootElement = pXML->GetRootElement();
	if ( pRootElement == NULL )
	{
		delete pXML;
		pXML = NULL;
		return;
	}

	strLst.RemoveAll();
	strLst.Add(ELEMENT);
	pElement = pXML->FindElement( pRootElement, strLst );
	if ( pElement != NULL )
	{
		for( i = 1; i <= MAX_CNT; i++ )
		{
			// Windows네서 항목 취득
			bUSE = ((CButton*)GetDlgItem( nUSE[i-1] ))->GetCheck();
			GetDlgItem( nPATH[i-1] )->GetWindowText( strPath );
			strPath.TrimLeft();
			strPath.TrimRight();
			GetDlgItem( nDAYS[i-1] )->GetWindowText( strDays );
			strDays.TrimLeft();
			strDays.TrimRight();
			GetDlgItem( nEXT[i-1] )->GetWindowText( strExt );
			strExt.TrimLeft();
			strExt.TrimRight();

			// 구조체에 데이터를 재 설정한다.
			m_stDEL[i-1].bUSE			= FALSE;
			memset( m_stDEL[i-1].szPATH, 0x00, sizeof( m_stDEL[i-1].szPATH ));
			memset( m_stDEL[i-1].szExt, 0x00, sizeof( m_stDEL[i-1].szExt ));
			m_stDEL[i-1].nDays			= 0;

			// 사용
			m_stDEL[i-1].bUSE				= bUSE;
			strTemp                         = "NO";
			if ( bUSE )
			{
				strTemp                     = "YES";
			}

			strWork.Format( "%s_%01d", DEL_FILE_USE, i );
			pXML->DataSet( pElement, strWork, strTemp );

			// PATH
			strTemp = strPath;
			strWork.Format( "%s_%01d", DEL_FILE_PATH, i );
			pXML->DataSet( pElement, strWork, strTemp );
			sprintf_s( m_stDEL[i-1].szPATH,"%s", strPath.GetString());

			// DAY
			strTemp = strDays;
			strWork.Format( "%s_%01d", DEL_DAY, i );
			pXML->DataSet( pElement, strWork, strTemp );
			m_stDEL[i-1].nDays			= atoi( strDays );

			// EXT
			strTemp = strExt;
			strWork.Format( "%s_%01d", DEL_FILE_EXT, i );
			pXML->DataSet( pElement, strWork, strTemp );
			sprintf_s( m_stDEL[i-1].szExt,"%s", strExt.GetString());
		}
		pXML->Store( strFullPath );
		delete pXML;
		pXML = NULL;
	}

	KillTimer( 1 );
	SetTimer( 1, 1000 * 60 * 60, NULL );
}

void CAutoFileDelDLG::OnBtnPath01() 
{
	GetDlgItem( IDC_EDT_PATH01 )->SetWindowText( m_fnGetFolderPath() );
}

void CAutoFileDelDLG::OnBtnPath02() 
{
	GetDlgItem( IDC_EDT_PATH02 )->SetWindowText( m_fnGetFolderPath() );
}

void CAutoFileDelDLG::OnBtnPath03() 
{
	GetDlgItem( IDC_EDT_PATH03 )->SetWindowText( m_fnGetFolderPath() );
}

void CAutoFileDelDLG::OnBtnPath04() 
{
	GetDlgItem( IDC_EDT_PATH04 )->SetWindowText( m_fnGetFolderPath() );
}

void CAutoFileDelDLG::OnBtnPath05() 
{
	GetDlgItem( IDC_EDT_PATH05 )->SetWindowText( m_fnGetFolderPath() );
}

void CAutoFileDelDLG::OnBtnPath06() 
{
	GetDlgItem( IDC_EDT_PATH06 )->SetWindowText( m_fnGetFolderPath() );
}

void CAutoFileDelDLG::OnBtnPath07() 
{
	GetDlgItem( IDC_EDT_PATH07 )->SetWindowText( m_fnGetFolderPath() );
}

void CAutoFileDelDLG::OnBtnPath08() 
{
	GetDlgItem( IDC_EDT_PATH08 )->SetWindowText( m_fnGetFolderPath() );
}

void CAutoFileDelDLG::OnBtnPath009() 
{
	GetDlgItem( IDC_EDT_PATH09 )->SetWindowText( m_fnGetFolderPath() );
}

void CAutoFileDelDLG::OnBtnPath10() 
{
	GetDlgItem( IDC_EDT_PATH10 )->SetWindowText( m_fnGetFolderPath() );
}




void CAutoFileDelDLG::OnBnClickedBtnHide()
{
	m_fnSetShowFlag( FALSE );
}
