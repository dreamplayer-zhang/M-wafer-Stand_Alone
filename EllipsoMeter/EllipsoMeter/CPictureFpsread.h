#pragma once

// 마법사에서 Microsoft Visual C++를 사용하여 IDispatch 래퍼 클래스를 생성했습니다.
//
// CPictureFpsread 래퍼 클래스입니다.

class CPictureFpsread : public COleDispatchDriver
{
public:
	CPictureFpsread() {}		// COleDispatchDriver 기본 생성자를 호출합니다.
	CPictureFpsread(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	CPictureFpsread(const CPictureFpsread& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	long GetHandle()
	{
		long result;
		GetProperty(0x0, VT_I4, (void*)&result);
		return result;
	}

	long GetHPal()
	{
		long result;
		GetProperty(0x2, VT_I4, (void*)&result);
		return result;
	}

	void SetHPal(long propVal)
	{
		SetProperty(0x2, VT_I4, propVal);
	}

	short GetType()
	{
		short result;
		GetProperty(0x3, VT_I2, (void*)&result);
		return result;
	}

	long GetWidth()
	{
		long result;
		GetProperty(0x4, VT_I4, (void*)&result);
		return result;
	}

	long GetHeight()
	{
		long result;
		GetProperty(0x5, VT_I4, (void*)&result);
		return result;
	}
};
