#pragma once

//#include "./Include/Spinnaker.h"
#if SPINNAKER_CAMERA
#include "Spinnaker.h"
#include "FrameRateCounter.h"

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;

//#ifdef _DEBUG
//#pragma comment (lib, "./Lib/Spinnaker1	_v100")
//#else
//#pragma comment (lib, "./Lib/Spinnaker_v100")
//#endif
#ifdef _DEBUG
#pragma comment (lib, "Spinnaker_v100")
#else
#pragma comment (lib, "Spinnaker_v100")
#endif

class CCamera : public ImageEventHandler
{
public:
	CCamera();
	~CCamera();

public:
	bool OnSearchCameraOnSystem();
	bool OnUpdateList(void);
	bool GetCameraCount(OUT int* nCount);
	bool ExecuteForceIP();
	bool SetPersistentIP(IN CString IPAddress);

public:
	bool OnConnectByIndex(IN int nIdx);
	bool OnConnectBySerial(IN CString SerialNumber);
	bool OnConnectByIP(IN CString IPAddress);
	bool OnDisConnect();
	bool OnStartAcquisition();
	bool OnStopAcquisition();

public:
	HANDLE GetHandleGrabDone();
	void ResetHandleGrabDone();
	int GetWidth();
	int GetHeight();
	int GetBpp();
	BYTE* GetBuffer();

	// For Connection Windows
public:
	CString GetInterfaceDisplayName(IN int nIdx);
	CString GetModelName(IN int nIdx);
	CString GetInterfaceType(IN int nIdx);
	CString GetSerialNumber(IN int nIdx);
	CString GetIPAddress(IN int nIdx);
	CString GetAccessStatus(IN int nIdx);
	CString GetGEVCCP(IN int nIdx);

	// For Main Dialog
public:
	bool GetConnectionStatus();
	bool GetActiveStatus();

	bool GetSharpnessEnable(OUT bool* Value);
	bool SetSharpnessEnable(IN bool Value);
	bool GetExposureCompensationAuto(OUT CString* Value);
	bool SetExposureCompensationAuto(IN CString Value);
	bool GetExposureMode(OUT CString* Value);
	bool SetExposureMode(IN CString Value);
	bool GetExposureTime(OUT double* Value);
	bool SetExposureTime(IN double Value);
	bool GetGain(OUT double* Value);
	bool SetGain(IN double Value);
	bool GetTriggerMode(OUT CString* Value);
	bool SetTriggerMode(IN CString Value);
	bool GetTriggerSource(OUT CString* Value);
	bool SetTriggerSource(IN CString Value);
	bool GetWidth(OUT int* Value);
	bool SetWidth(IN int Value);
	bool GetHeight(OUT int* Value);
	bool SetHeight(IN int Value);
	bool GetOffsetX(OUT int* Value);
	bool SetOffsetX(IN int Value);
	bool GetOffsetY(OUT int* Value);
	bool SetOffsetY(IN int Value);
	bool GetExposureAuto(OUT CString* Value);
	bool SetExposureAuto(IN CString Value);
	bool GetGainAuto(OUT CString* Value);
	bool SetGainAuto(IN CString Value);
	bool GetTargetGreyAuto(OUT CString* Value);
	bool SetTargetGreyAuto(IN CString Value);
	bool GetWhiteBalanceAuto(OUT CString* Value);
	bool SetWhiteBalanceAuto(IN CString Value);
	bool GetWidthMax(OUT int* Value);
	bool GetHeightMax(OUT int* Value);

	bool ExchangeResolution();

	bool SaveUserSet();
	bool LoadUserSet();
private:
	bool OnConnect(IN int nIdx);
	void OnImageEvent(ImagePtr image);

	// Macro Function
private:
	bool GetValueByEnumByString(IN CString NodeName, OUT CString* Value);
	bool SetValueByEnumByString(IN CString NodeName, IN CString Value);
	bool GetValueString(IN CString NodeName, OUT CString* Value);
	bool SetValueString(IN CString NodeName, IN CString Value);
	bool GetValueDouble(IN CString NodeName, OUT double* Value);
	bool SetValueDouble(IN CString NodeName, IN double Value);
	bool GetValueInt(IN CString NodeName, OUT int* Value);
	bool SetValueInt(IN CString NodeName, IN int Value);
	bool GetValueBool(IN CString NodeName, OUT bool* Value);
	bool SetValueBool(IN CString NodeName, IN bool Value);
	bool OnExecuteCommand(IN CString NodeName);

private:
	typedef struct 
	{
		CString m_strInterfaceDisplayName;
		CString m_strModelName;
		CString m_strInterfaceType;
		CString m_strSerialNumber;
		CString m_strIPAddress;
		CString m_strAccessStatus;
		CString m_strGEVCCP;
	}stCameraInfo;

private:
	SystemPtr		m_pSystem;
	InterfacePtr	m_pInterface;
	CameraPtr		m_pCamera;
	CameraList		m_CameraList;
	InterfaceList	m_InterfaceList;
	std::vector<stCameraInfo> m_vCameraInfo;

private:
	int				m_nWidth;
	int				m_nHeight;
	int				m_nBpp;

	bool			m_isConnect;
	bool			m_isActive;

	BYTE*			m_pbyBuffer;

	HANDLE			m_hGrabDone;

	int64_t			m_nTotalFrameCount;
	int64_t			m_nValidFrameCount;
};

#endif