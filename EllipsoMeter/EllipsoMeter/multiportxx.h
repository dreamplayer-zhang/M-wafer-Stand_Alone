#pragma once

// 컴퓨터에서 Microsoft Visual C++를 사용하여 생성한 IDispatch 래퍼 클래스입니다.

// 참고: 이 파일의 내용을 수정하지 마십시오. Microsoft Visual C++에서
//  이 클래스를 다시 생성할 때 수정한 내용을 덮어씁니다.

/////////////////////////////////////////////////////////////////////////////
// CMultiportxx 래퍼 클래스입니다.

class CMultiportxx : public CWnd
{
protected:
	DECLARE_DYNCREATE(CMultiportxx)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0xD3AEF8A8, 0x7CA3, 0x49A4, { 0x91, 0x92, 0x7A, 0x6F, 0x64, 0xB4, 0xD7, 0x38 } };
		return clsid;
	}
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
						const RECT& rect, CWnd* pParentWnd, UINT nID, 
						CCreateContext* pContext = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); 
	}

    BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, 
				UINT nID, CFile* pPersist = NULL, BOOL bStorage = FALSE,
				BSTR bstrLicKey = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
		pPersist, bStorage, bstrLicKey); 
	}

// 특성입니다.
public:


// 작업입니다.
public:

// IMultiPortXX

// Functions
//

	long ClosePorts()
	{
		long result;
		InvokeHelper(0xc9, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long ClosePort(LPCTSTR idstr)
	{
		long result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0xca, DISPATCH_METHOD, VT_I4, (void*)&result, parms, idstr);
		return result;
	}
	long OpenPort(LPCTSTR idstr, long Port, long baud, long Parity, long DataBits, long StopBits, long UserMemorySize)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0xcb, DISPATCH_METHOD, VT_I4, (void*)&result, parms, idstr, Port, baud, Parity, DataBits, StopBits, UserMemorySize);
		return result;
	}
	long StringToHexString(LPCTSTR InString, BSTR * OutHexString)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_PBSTR ;
		InvokeHelper(0xcc, DISPATCH_METHOD, VT_I4, (void*)&result, parms, InString, OutHexString);
		return result;
	}
	long SendString(LPCTSTR idstr, LPCTSTR Str)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_BSTR ;
		InvokeHelper(0xcd, DISPATCH_METHOD, VT_I4, (void*)&result, parms, idstr, Str);
		return result;
	}
	BOOL IsOpenPort(LPCTSTR idstr)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0xce, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, idstr);
		return result;
	}
	long get_MaxBlockSize()
	{
		long result;
		InvokeHelper(0xd1, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_MaxBlockSize(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xd1, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_BlockRefreshInterval()
	{
		long result;
		InvokeHelper(0xd2, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_BlockRefreshInterval(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xd2, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	long get_BufferSize()
	{
		long result;
		InvokeHelper(0xd3, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
		return result;
	}
	void put_BufferSize(long newValue)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xd3, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	BOOL get_Visible()
	{
		BOOL result;
		InvokeHelper(0xdd, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	void put_Visible(BOOL newValue)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0xdd, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	void SetSubComponent(BOOL IsSubComponent)
	{
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0xde, DISPATCH_METHOD, VT_EMPTY, NULL, parms, IsSubComponent);
	}
	long Send(LPCTSTR idstr, long DataPoint, long DataSize)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_I4 ;
		InvokeHelper(0xdf, DISPATCH_METHOD, VT_I4, (void*)&result, parms, idstr, DataPoint, DataSize);
		return result;
	}
	long DataToHexString(long DataPoint, long DataSize, BSTR * OutHexString)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_PBSTR ;
		InvokeHelper(0xe0, DISPATCH_METHOD, VT_I4, (void*)&result, parms, DataPoint, DataSize, OutHexString);
		return result;
	}
	long HexStringToBINBuffer(LPCTSTR InString, long * SavePoint)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_PI4 ;
		InvokeHelper(0xe1, DISPATCH_METHOD, VT_I4, (void*)&result, parms, InString, SavePoint);
		return result;
	}
	long SendStringReturn(LPCTSTR idstr, LPCTSTR OutSendStr, long SendMode, long WaittimeMSecs, long TriggerInterval, long MaxbufferSize, BSTR * RetrunString, long * ReturnStrCount)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_PBSTR VTS_PI4 ;
		InvokeHelper(0xe2, DISPATCH_METHOD, VT_I4, (void*)&result, parms, idstr, OutSendStr, SendMode, WaittimeMSecs, TriggerInterval, MaxbufferSize, RetrunString, ReturnStrCount);
		return result;
	}
	long HexStrToString(LPCTSTR HexString, BSTR * OutString)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_PBSTR ;
		InvokeHelper(0xe3, DISPATCH_METHOD, VT_I4, (void*)&result, parms, HexString, OutString);
		return result;
	}
	long SendHexStringReturn(LPCTSTR idstr, LPCTSTR SendHexStr, long SendMode, long WaittimeMSecs, long TriggerInterval, long MaxbufferSize, BSTR * RetrunString, long * ReturnStrCount)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_PBSTR VTS_PI4 ;
		InvokeHelper(0xe4, DISPATCH_METHOD, VT_I4, (void*)&result, parms, idstr, SendHexStr, SendMode, WaittimeMSecs, TriggerInterval, MaxbufferSize, RetrunString, ReturnStrCount);
		return result;
	}
	void AboutBox()
	{
		InvokeHelper(DISPID_ABOUTBOX, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	BOOL SetBreak(LPCTSTR idstr, BOOL OnOff)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR VTS_BOOL ;
		InvokeHelper(0xd0, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, idstr, OnOff);
		return result;
	}
	BOOL SetDTR(LPCTSTR idstr, BOOL OnOff)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR VTS_BOOL ;
		InvokeHelper(0xd4, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, idstr, OnOff);
		return result;
	}
	BOOL SetRTS(LPCTSTR idstr, BOOL OnOff)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR VTS_BOOL ;
		InvokeHelper(0xd5, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, idstr, OnOff);
		return result;
	}
	BOOL SetXonXoff(LPCTSTR idstr, BOOL OnOff)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR VTS_BOOL ;
		InvokeHelper(0xd6, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, idstr, OnOff);
		return result;
	}
	BOOL SetFlowControl(LPCTSTR idstr, long None0_RTSCTS1_XOnOff2)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR VTS_I4 ;
		InvokeHelper(0xcf, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, idstr, None0_RTSCTS1_XOnOff2);
		return result;
	}
	BOOL SetXonOffChar(LPCTSTR idstr, unsigned char OnChar, unsigned char OffChar)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR VTS_UI1 VTS_UI1 ;
		InvokeHelper(0xd7, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, idstr, OnChar, OffChar);
		return result;
	}
	long StringToHexStringSize(LPCTSTR InString, long InStrSize, BSTR * OutHexString)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_PBSTR ;
		InvokeHelper(0xd8, DISPATCH_METHOD, VT_I4, (void*)&result, parms, InString, InStrSize, OutHexString);
		return result;
	}
	long SendHexStringReturnPoint(LPCTSTR idstr, LPCTSTR SendHexStr, long SendMode, long WaittimeMSecs, long TriggerInterval, long MaxbufferSize, long * ReturnDataPoint, long * ReturnDataCount)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_PI4 VTS_PI4 ;
		InvokeHelper(0xd9, DISPATCH_METHOD, VT_I4, (void*)&result, parms, idstr, SendHexStr, SendMode, WaittimeMSecs, TriggerInterval, MaxbufferSize, ReturnDataPoint, ReturnDataCount);
		return result;
	}
	long SendHexString(LPCTSTR idstr, LPCTSTR HexString)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_BSTR ;
		InvokeHelper(0xda, DISPATCH_METHOD, VT_I4, (void*)&result, parms, idstr, HexString);
		return result;
	}
	long SendString2(LPCTSTR idstr, LPCTSTR Str, long UserIndex, LPCTSTR UserString)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_I4 VTS_BSTR ;
		InvokeHelper(0xdb, DISPATCH_METHOD, VT_I4, (void*)&result, parms, idstr, Str, UserIndex, UserString);
		return result;
	}
	long Send2(LPCTSTR idstr, long DataPoint, long DataSize, long UserIndex, LPCTSTR UserString)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_I4 VTS_I4 VTS_BSTR ;
		InvokeHelper(0xdc, DISPATCH_METHOD, VT_I4, (void*)&result, parms, idstr, DataPoint, DataSize, UserIndex, UserString);
		return result;
	}
	long SendHexString2(LPCTSTR idstr, LPCTSTR HexString, long UserIndex, LPCTSTR UserString)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_I4 VTS_BSTR ;
		InvokeHelper(0xe5, DISPATCH_METHOD, VT_I4, (void*)&result, parms, idstr, HexString, UserIndex, UserString);
		return result;
	}
	long TokenCutStr(LPCTSTR Str, long CutChar, long IndexNum, BSTR * OutString)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_I4 VTS_I4 VTS_PBSTR ;
		InvokeHelper(0xe6, DISPATCH_METHOD, VT_I4, (void*)&result, parms, Str, CutChar, IndexNum, OutString);
		return result;
	}
	long Str2StrGet(LPCTSTR MainStr, LPCTSTR StartStr, LPCTSTR EndStr, BSTR * OutString)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_BSTR VTS_PBSTR ;
		InvokeHelper(0xe7, DISPATCH_METHOD, VT_I4, (void*)&result, parms, MainStr, StartStr, EndStr, OutString);
		return result;
	}
	long DataToString(long DataPointer, long DataSize, BSTR * OutString)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_PBSTR ;
		InvokeHelper(0xe8, DISPATCH_METHOD, VT_I4, (void*)&result, parms, DataPointer, DataSize, OutString);
		return result;
	}
	void SetBit(long * ByteVal, long nBit, BOOL BitOnOff)
	{
		static BYTE parms[] = VTS_PI4 VTS_I4 VTS_BOOL ;
		InvokeHelper(0xe9, DISPATCH_METHOD, VT_EMPTY, NULL, parms, ByteVal, nBit, BitOnOff);
	}
	BOOL GetBit(long ByteVal, long nBit)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0xea, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, ByteVal, nBit);
		return result;
	}
	long Int2HexStr(long IntegerVal, BSTR * ReturnHexStr)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_PBSTR ;
		InvokeHelper(0xeb, DISPATCH_METHOD, VT_I4, (void*)&result, parms, IntegerVal, ReturnHexStr);
		return result;
	}
	long HexStr2int(LPCTSTR HexString)
	{
		long result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0xec, DISPATCH_METHOD, VT_I4, (void*)&result, parms, HexString);
		return result;
	}
	BOOL DataRecvStop(LPCTSTR idstr, BOOL OnOff)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR VTS_BOOL ;
		InvokeHelper(0xed, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, idstr, OnOff);
		return result;
	}
	long DataRecvStopALL(BOOL OnOff)
	{
		long result;
		static BYTE parms[] = VTS_BOOL ;
		InvokeHelper(0xee, DISPATCH_METHOD, VT_I4, (void*)&result, parms, OnOff);
		return result;
	}
	CString get_DHeadHexString()
	{
		CString result;
		InvokeHelper(0xef, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_DHeadHexString(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0xef, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}
	CString get_DTailHexString()
	{
		CString result;
		InvokeHelper(0xf0, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	void put_DTailHexString(LPCTSTR newValue)
	{
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0xf0, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, newValue);
	}

// Properties
//



};
