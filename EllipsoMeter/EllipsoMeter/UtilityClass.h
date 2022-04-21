#pragma once

// CUtilityClass

class CUtilityClass : public CWnd
{
	DECLARE_DYNAMIC(CUtilityClass)

	// 현재 실행중인 App의 드라이버 얻어오기
	static CString			m_fnGetCurrentDrive( void );
	static CString          m_fnGetCurrentTime( void );
	static CString          m_fnGetCurrentTime1( void );
	static CString          m_fnGetCurrentTime2( void );
	static CString			m_fnGetCurrentTime3( int nHour );
	static CString			m_fnGetCurrentTime4( CString strTime, int nHour );
	static CString          m_fnGetCurrentTime5( void );
	static CString          m_fnGetCurrentTime6( void );
	static CString          m_fnGetDate( void );
	static CString          m_fnGetTime( void );
	static CString			m_fnGetTime24( void );

	static int              m_fnGetCharToNUM( CString strData );

	static CString			m_fnGetCharEdit( char* szItem, int nLen );
	static CString          m_fnGetCharEdit( CString strData, int nLen );
	static CString          m_fnGetZeroEdit( char* szItem, int nLen );

	static void             m_fnSetCharEdit( char* szSrc, char* szTarget, int nLen );
	static BOOL             m_fnAdjustPrivilege(LPCTSTR privilege, DWORD attribute);

	// File-only operations
	static BOOL				Copy(LPCTSTR lpSource, LPCTSTR lpDestination, BOOL bHidePrompt = TRUE);
	static BOOL				Del(LPCTSTR lpSource, BOOL bHidePrompt = TRUE);
	static BOOL				Ren(LPCTSTR lpSource, LPCTSTR lpDestination, BOOL bHidePrompt = TRUE);

	// Directory-only operations
	static BOOL				MkDir(LPCTSTR lpDirectory);
	static BOOL				RmDir(LPCTSTR lpDirectory);
	static int				m_fnSetCreateDir(CString strFolder);

	static CString FormatStr(const char *fmt, ...);
	static void CountingImgFile( LPCTSTR directory, int bdNo, LPCTSTR FileFormat, int *pCnt, int mod );
	static void CreateDir(char* Path);


	// File/directory attributes access
	static BOOL				SetAttribute(LPCTSTR lpSource, DWORD dwNewAttr);
	static DWORD			GetAttribute(LPCTSTR lpSource);

	// File/directory existence check
	static int				Existence(LPCTSTR lpSource);
	static BOOL				XorConvert( BYTE * bySRC, int nbyLen, BYTE& byRESULT, CString& strResult  );
	static BOOL				CharToHex( BYTE* byBuffer, int nLen, CString& strResult );
	static BOOL				CharToHex( char* szBuffer, int nLen, CString& strResult );
	static void             m_fnGetIPConvert(CString strIPNum, int* nFilds );
	static INT_PTR			m_fnGetFileList( CString strPath, CString strName, CStringArray& strFile );
	static void				KillProcess( const char *EXEName );
	static void				KillProcess( );
	static CString			m_fnGetTackTime( double dwTactTime );
	static CString          HexaToDecimalString( CString strSRC );
	static double			m_fnGetRangeRandom( double dRange_Min, double dRange_Max );

	static CString          m_strToday;
	static CString          m_fnGetToday( void );
	static CString			GetIPAddress(void);

	static float			GetAverage(float* set, int num);
	static float			GetVariance(float* set, int num, float average);
	static float			GetStandardDeviation(float variance);
	static float			GetCoVariance(float* setX, float* setY, int num, float averageX, float averageY);
	static float			GetMin(float* set, int num);
	static float			GetMax(float* set, int num);

	static int				m_fnComboFindString(CComboBox* pcl, CString strData );
	static int				m_fnListBoxFindString(CListBox* pcl, CString strData );
	static BSTR				CStringTobstr( CString str );
	static char*			TCHARToChar(TCHAR *str);
	wchar_t*				ConverCtoWC(char* str);

#if OPENCV
	static void				setPutText( IplImage* src_image, IplImage* dst_image, char* text, CvPoint textPos, CvScalar color);
	static HBITMAP			setTextToBitmap( char* text, CvScalar color);
	static void setBitmapToIplImage( HBITMAP hBitmap, IplImage** mImage );
#endif
	static CString			m_fnGetVersionInfo( void );

public:
	CUtilityClass();
	virtual ~CUtilityClass();

protected:
	DECLARE_MESSAGE_MAP()
};


