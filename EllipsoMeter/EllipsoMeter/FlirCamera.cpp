#include "stdafx.h"
#if SPINNAKER_CAMERA
#include "FlirCamera.h"

CCamera::CCamera()
	:m_pSystem(NULL)
	, m_pInterface(NULL)
	, m_pCamera(NULL)
{
	m_nWidth = 0;
	m_nHeight = 0;
	m_nBpp = 0;

	m_isConnect = false;
	m_isActive = false;

	m_pbyBuffer = NULL;

	m_nValidFrameCount = 0;
	m_nTotalFrameCount = 0;

	m_hGrabDone = CreateEvent(NULL, TRUE, FALSE, NULL);
	ResetEvent(m_hGrabDone);
}

CCamera::~CCamera()
{
	if (m_pSystem != NULL)
	{
		m_pSystem->ReleaseInstance();
	}

	if (m_pbyBuffer != NULL)
	{
		delete m_pbyBuffer;
		m_pbyBuffer = NULL;
	}
		
}

bool CCamera::OnSearchCameraOnSystem()
{
	try
	{
		m_vCameraInfo.clear();

		m_pSystem = System::GetInstance();
		m_CameraList = m_pSystem->GetCameras();
		m_InterfaceList = m_pSystem->GetInterfaces();

		int nInterfaceLst = m_InterfaceList.GetSize();
		for (int i = 0; i < nInterfaceLst; i++)
		{
			InterfacePtr pInterface = m_InterfaceList.GetByIndex(i);
			CameraList cameraList = pInterface->GetCameras();
			int nCameraCount = cameraList.GetSize();
			for (int c = 0; c < nCameraCount; c++)
			{
				stCameraInfo stCameraInfo;

				CameraPtr pCamera = cameraList.GetByIndex(c);
				pCamera->GetTLDeviceNodeMap().InvalidateNodes();
				pInterface->GetTLNodeMap().InvalidateNodes();
				stCameraInfo.m_strInterfaceDisplayName = pInterface->TLInterface.InterfaceDisplayName.GetValue();
				stCameraInfo.m_strModelName = pCamera->TLDevice.DeviceModelName.GetValue();
				stCameraInfo.m_strInterfaceType = pCamera->TLDevice.DeviceType.ToString();
				stCameraInfo.m_strSerialNumber = pCamera->TLDevice.DeviceSerialNumber.GetValue();
				stCameraInfo.m_strAccessStatus = pInterface->TLInterface.DeviceAccessStatus.ToString();

				//U3V는 IPAddress 와 GEVCCP 가 없음.
				if (stCameraInfo.m_strInterfaceType == _T("GEV"))
				{
					unsigned int dec = pCamera->TLDevice.GevDeviceIPAddress.GetValue();
					unsigned char digit[4];
					digit[0] = dec & 0xFF;
					digit[1] = (dec >> 8) & 0xFF;
					digit[2] = (dec >> 16) & 0xFF;
					digit[3] = (dec >> 24) & 0xFF;

					CString IP;
					IP.Format(_T("%d.%d.%d.%d"), digit[3], digit[2], digit[1], digit[0]);
					stCameraInfo.m_strIPAddress = IP; 

					try
					{
						stCameraInfo.m_strGEVCCP = pCamera->TLDevice.GevCCP.ToString();
					}
					catch (Spinnaker::Exception &e)
					{
						stCameraInfo.m_strGEVCCP = _T("UnKnown");
					}
				}
				else
				{
					stCameraInfo.m_strIPAddress = _T("");
					stCameraInfo.m_strGEVCCP = _T("");
				}
				m_vCameraInfo.push_back(stCameraInfo);
			}
		}
		m_CameraList.Clear();
		m_InterfaceList.Clear();
		return true;
	}
	catch(Spinnaker::Exception &e)
	{
		CString Error = (CString)e.what();
		return false;
	}
}

bool CCamera::OnUpdateList()
{
	try
	{
// 		m_InterfaceList = m_pSystem->GetInterfaces();
// 		int nInterface = m_InterfaceList.GetSize();
// 
// 		for (int n = 0; n < nInterface; n++)
// 		{
// 			InterfacePtr pInterface = m_InterfaceList.GetByIndex(n);
// 			INodeMap &nodeMap = pInterface->GetTLNodeMap();
// 
// 			CCommandPtr cmdPtr = nodeMap.GetNode("DeviceUpdateList");
// 			if (!IsAvailable(cmdPtr) || !IsWritable(cmdPtr))
// 				return false;
// 			cmdPtr->Execute();
// 		}
// 		m_InterfaceList.Clear();
		bool result = false;
		result = m_pSystem->UpdateCameras();
		return result;
	}
	catch (Spinnaker::Exception &e)
	{
		CString Error = (CString)e.what();
		return false;
	}
}

bool CCamera::GetCameraCount(OUT int* nCount)
{
	try
	{
		m_CameraList = m_pSystem->GetCameras();
		*nCount = m_CameraList.GetSize();
		m_CameraList.Clear();
		return true;
	}
	catch (Spinnaker::Exception &e)
	{
		CString Error = (CString)e.what();
		return false;
	}
}

bool CCamera::ExecuteForceIP()
{
	try
	{
		m_InterfaceList = m_pSystem->GetInterfaces();
		int nInterface = m_InterfaceList.GetSize();

		for (int n = 0; n < nInterface; n++)
		{
			InterfacePtr pInterface = m_InterfaceList.GetByIndex(n);
			CString strType;
			strType = pInterface->TLInterface.InterfaceType.ToString();
			
			if (strType == _T("GEV"))
				pInterface->TLInterface.GevDeviceForceIP();//.AutoForceIP.Execute();
		}
		m_InterfaceList.Clear();
		return true;
	}
	catch (Spinnaker::Exception &e)
	{
		CString Error = (CString)e.what();
		return false;
	}
}

bool CCamera::SetPersistentIP(IN CString IPAddress)
{
	CString classA, classB, classC, classD;
	AfxExtractSubString(classA, IPAddress, 0, '.');
	AfxExtractSubString(classB, IPAddress, 1, '.');
	AfxExtractSubString(classC, IPAddress, 2, '.');
	AfxExtractSubString(classD, IPAddress, 3, '.');
	int A, B, C, D;
	A = _ttoi(classA);
	B = _ttoi(classB);
	C = _ttoi(classC);
	D = _ttoi(classD);
	unsigned int ip = (A * 256 * 256 * 256) + (B * 256 * 256) + (C * 256) + D;
	unsigned int subnet = (255 * 256 * 256 * 256) + (255 * 256 * 256) + (255 * 256);
	unsigned int gateway = (A * 256 * 256 * 256) + (B * 256 * 256) + (C * 256) + 254;

	try
	{
		m_pCamera->GevCurrentIPConfigurationDHCP.SetValue(FALSE);
		m_pCamera->GevCurrentIPConfigurationPersistentIP.SetValue(TRUE);
		m_pCamera->GevPersistentIPAddress.SetValue(ip);
		m_pCamera->GevPersistentSubnetMask.SetValue(subnet);
		m_pCamera->GevPersistentDefaultGateway.SetValue(gateway);
		m_pCamera->DeviceReset.Execute();
		return true;
	}
	catch (Spinnaker::Exception &e)
	{
		CString Error = (CString)e.what();
		return false;
	}
}

bool CCamera::OnConnectByIndex(IN int nIdx)
{
	return OnConnect(nIdx);
}

bool CCamera::OnConnectBySerial(IN CString SerialNumber)
{
	try
	{
		m_CameraList = m_pSystem->GetCameras();

		std::string tmp((CT2CA)SerialNumber);
		m_pCamera = m_CameraList.GetBySerial(tmp);
		m_pCamera->Init();

		m_nWidth = m_pCamera->Width.GetValue();
		m_nHeight = m_pCamera->Height.GetValue();
		PixelFormatEnums pixelFormat = m_pCamera->PixelFormat.GetValue();

		switch (pixelFormat)
		{
		case PixelFormat_Mono8:				m_nBpp = 8; break;
		case PixelFormat_BayerRG8:
		case PixelFormat_BayerBG8:
		case PixelFormat_BayerGR8:
		case PixelFormat_BayerGB8:			m_nBpp = 24; break;
		}

		m_pbyBuffer = new BYTE[m_nWidth * m_nHeight * m_nBpp / 8];
		ZeroMemory(m_pbyBuffer, m_nWidth * m_nHeight * m_nBpp / 8);

		m_pCamera->RegisterEventHandler(*this);
		m_CameraList.Clear();

		m_isConnect = true;
		return true;
	}
	catch (Spinnaker::Exception &e)
	{
		CString Error = (CString)e.what();
		return false;
	}
}

bool CCamera::OnConnectByIP(IN CString IPAddress)
{
	try
	{
		m_CameraList = m_pSystem->GetCameras();
		int nCamera = m_CameraList.GetSize();

		for (int c = 0; c < nCamera; c++)
		{
			CameraPtr compareCamera = m_CameraList.GetByIndex(c);
			unsigned int dec = compareCamera->TLDevice.GevDeviceIPAddress.GetValue();
			unsigned char digits[4];
			digits[0] = dec && 0xFF;
			digits[1] = (dec >> 8) & 0xFF;
			digits[2] = (dec >> 16) & 0xFF;
			digits[3] = (dec >> 24) & 0xFF;

			CString IP;
			IP.Format(_T("%d.%d.%d.%d"), digits[3], digits[2], digits[1], digits[0]);

			if (IP == IPAddress)
			{
				bool ret = OnConnect(c);
				m_CameraList.Clear();
				return ret;
			}
		}
		return false;
	}
	catch (Spinnaker::Exception &e)
	{
		CString Error = (CString)e.what();
		return false;
	}
}

bool CCamera::OnDisConnect()
{
	try
	{
		if (m_isConnect)
			m_isConnect = false;

		if (m_isActive)
			OnStopAcquisition();

		if (m_pCamera != NULL)
		{
			m_pCamera->UnregisterEventHandler(*this);
			m_pCamera->DeInit();
			m_pCamera = NULL;
		}

		if (m_pbyBuffer != NULL)
		{
			delete[] m_pbyBuffer;
			m_pbyBuffer = NULL;
		}

		m_CameraList.Clear();

		return true;
	}
	catch (Spinnaker::Exception &e)
	{
		CString Error = (CString)e.what();
		return false;
	}
}

bool CCamera::OnStartAcquisition()
{
	try
	{
		if (m_pCamera->IsStreaming())
			OnStopAcquisition();

		m_pCamera->BeginAcquisition();
		m_isActive = true;
		return true;
	}
	catch (Spinnaker::Exception &e)
	{
		CString Error = (CString)e.what();
		return false;
	}
}

bool CCamera::OnStopAcquisition()
{
	try
	{
		m_isActive = false;
		m_pCamera->EndAcquisition();
		return true;
	}
	catch (Spinnaker::Exception &e)
	{
		CString Error = (CString)e.what();
		return false;
	}
}

HANDLE CCamera::GetHandleGrabDone()
{
	return m_hGrabDone;
}

void CCamera::ResetHandleGrabDone()
{
	ResetEvent(m_hGrabDone);
}

int CCamera::GetWidth()
{
	return m_nWidth;
}

int CCamera::GetHeight()
{
	return m_nHeight;
}

int CCamera::GetBpp()
{
	return m_nBpp;
}

BYTE* CCamera::GetBuffer()
{
	return m_pbyBuffer;
}

CString CCamera::GetInterfaceDisplayName(IN int nIdx)
{
	if (nIdx >= (int)m_vCameraInfo.size())
		return _T("");
	return m_vCameraInfo.at(nIdx).m_strInterfaceDisplayName;
}

CString CCamera::GetModelName(IN int nIdx)
{
	if (nIdx >= (int)m_vCameraInfo.size())
		return _T("");
	return m_vCameraInfo.at(nIdx).m_strModelName;
}

CString CCamera::GetInterfaceType(IN int nIdx)
{
	if (nIdx >= (int)m_vCameraInfo.size())
		return _T("");
	return m_vCameraInfo.at(nIdx).m_strInterfaceType;
}

CString CCamera::GetSerialNumber(IN int nIdx)
{
	if (nIdx >= (int)m_vCameraInfo.size())
		return _T("");
	return m_vCameraInfo.at(nIdx).m_strSerialNumber;
}

CString CCamera::GetIPAddress(IN int nIdx)
{
	if (nIdx >= (int)m_vCameraInfo.size())
		return _T("");
	return m_vCameraInfo.at(nIdx).m_strIPAddress;
}

CString CCamera::GetAccessStatus(IN int nIdx)
{
	if (nIdx >= (int)m_vCameraInfo.size())
		return _T("");
	return m_vCameraInfo.at(nIdx).m_strAccessStatus;
}

CString CCamera::GetGEVCCP(IN int nIdx)
{
	if (nIdx >= (int)m_vCameraInfo.size())
		return _T("");
	return m_vCameraInfo.at(nIdx).m_strGEVCCP;
}

bool CCamera::GetConnectionStatus()
{
	return m_isConnect;
}

bool CCamera::GetActiveStatus()
{
	return m_isActive;
}

bool CCamera::GetSharpnessEnable(OUT bool* Value)
{
	return GetValueBool(_T("SharpnessEnabled"), Value);
}

bool CCamera::SetSharpnessEnable(IN bool Value)
{
	return SetValueBool(_T("SharpnessEnabled"), Value);
}

bool CCamera::GetExposureCompensationAuto(OUT CString* Value)
{
	return GetValueByEnumByString(_T("pgrExposureCompensationAuto"), Value);
}

bool CCamera::SetExposureCompensationAuto(IN CString Value)
{
	return SetValueByEnumByString(_T("pgrExposureCompensationAuto"), Value);
}

bool CCamera::GetExposureMode(OUT CString* Value)
{
	return GetValueByEnumByString(_T("ExposureMode"), Value);
}

bool CCamera::SetExposureMode(IN CString Value)
{
	return SetValueByEnumByString(_T("ExposureMode"), Value);
}

bool CCamera::GetExposureTime(OUT double* Value)
{
	return GetValueDouble(_T("ExposureTime"), Value);
}

bool CCamera::SetExposureTime(IN double Value)
{
	return SetValueDouble(_T("ExposureTime"), Value);
}

bool CCamera::GetGain(OUT double* Value)
{
	return GetValueDouble(_T("Gain"), Value);
}

bool CCamera::SetGain(IN double Value)
{
	return SetValueDouble(_T("Gain"), Value);
}

bool CCamera::GetTriggerMode(OUT CString* Value)
{
	return GetValueByEnumByString(_T("TriggerMode"), Value);
}

bool CCamera::SetTriggerMode(IN CString Value)
{
	return SetValueByEnumByString(_T("TriggerMode"), Value);
}

bool CCamera::GetTriggerSource(OUT CString* Value)
{
	return GetValueByEnumByString(_T("TriggerSource"), Value);
}

bool CCamera::SetTriggerSource(IN CString Value)
{
	return SetValueByEnumByString(_T("TriggerSource"), Value);
}

bool CCamera::GetWidth(OUT int* Value)
{
	return GetValueInt(_T("Width"), Value);
}

bool CCamera::SetWidth(IN int Value)
{
	return SetValueInt(_T("Width"), Value);
}

bool CCamera::GetHeight(OUT int* Value)
{
	return GetValueInt(_T("Height"), Value);
}

bool CCamera::SetHeight(IN int Value)
{
	return SetValueInt(_T("Height"), Value);
}
bool CCamera::GetOffsetX(OUT int* Value)
{
	return GetValueInt(_T("OffsetX"), Value);
}

bool CCamera::SetOffsetX(IN int Value)
{
	return SetValueInt(_T("OffsetX"), Value);
}
bool CCamera::GetOffsetY(OUT int* Value)
{
	return GetValueInt(_T("OffsetY"), Value);
}

bool CCamera::SetOffsetY(IN int Value)
{
	return SetValueInt(_T("OffsetY"), Value);
}
bool CCamera::GetExposureAuto(OUT CString* Value)
{
	return GetValueByEnumByString(_T("ExposureAuto"), Value);
}

bool CCamera::SetExposureAuto(IN CString Value)
{
	return SetValueByEnumByString(_T("ExposureAuto"), Value);
}
bool CCamera::GetGainAuto(OUT CString* Value)
{
	return GetValueByEnumByString(_T("GainAuto"), Value);
}

bool CCamera::SetGainAuto(IN CString Value)
{
	return SetValueByEnumByString(_T("GainAuto"), Value);
}
bool CCamera::GetTargetGreyAuto(OUT CString* Value)
{
	return GetValueByEnumByString(_T("AutoExposureTargetGreyValueAuto"), Value);
}

bool CCamera::SetTargetGreyAuto(IN CString Value)
{
	return SetValueByEnumByString(_T("AutoExposureTargetGreyValueAuto"), Value);
}
bool CCamera::GetWhiteBalanceAuto(OUT CString* Value)
{
	return GetValueByEnumByString(_T("BalanceWhiteAuto"), Value);
}

bool CCamera::SetWhiteBalanceAuto(IN CString Value)
{
	return SetValueByEnumByString(_T("BalanceWhiteAuto"), Value);
}

bool CCamera::GetWidthMax(OUT int* Value)
{
	try
	{
		*Value = m_pCamera->Width.GetMax();
		return true;
	}
	catch (Spinnaker::Exception &e)
	{
		CString Error = (CString)e.what();
		return false;
	}
}

bool CCamera::GetHeightMax(OUT int* Value)
{
	try
	{
		*Value = m_pCamera->Height.GetMax();
		return true;
	}
	catch (Spinnaker::Exception &e)
	{
		CString Error = (CString)e.what();
		return false;
	}
}

bool CCamera::ExchangeResolution()
{
	try
	{
		if (m_pbyBuffer != NULL)
		{
			delete m_pbyBuffer;
			m_pbyBuffer = NULL;
		}

		m_nWidth = m_pCamera->Width.GetValue();
		m_nHeight = m_pCamera->Height.GetValue();

		m_pbyBuffer = new BYTE[m_nWidth * m_nHeight * m_nBpp / 8];
		ZeroMemory(m_pbyBuffer, m_nWidth * m_nHeight * m_nBpp / 8);
		return true;
	}
	catch (Spinnaker::Exception &e)
	{
		CString Error = (CString)e.what();
		return false;
	}
}

bool CCamera::SaveUserSet()
{
	try
	{
		m_pCamera->UserSetSelector.SetValue(UserSetSelector_UserSet1);
		m_pCamera->UserSetDefault.SetValue(UserSetDefault_UserSet1);
		m_pCamera->UserSetSave.Execute();
	}
	catch (Spinnaker::Exception &e)
	{
		CString Error = (CString)e.what();
		return false;
	}
	return true;
}

bool CCamera::LoadUserSet()
{
	try
	{
		m_pCamera->UserSetSelector.SetValue(UserSetSelector_UserSet1);
		m_pCamera->UserSetDefault.SetValue(UserSetDefault_UserSet1);
		m_pCamera->UserSetLoad.Execute();
	}
	catch (Spinnaker::Exception &e)
	{
		CString Error = (CString)e.what();
		return false;
	}
	return true;
}

bool CCamera::OnConnect(IN int nIdx)
{
	try
	{
		m_CameraList = m_pSystem->GetCameras();

		m_pCamera = m_CameraList.GetByIndex(nIdx);
		m_pCamera->Init();

		m_nWidth = m_pCamera->Width.GetValue();
		m_nHeight = m_pCamera->Height.GetValue();
		PixelFormatEnums pixelFormat = m_pCamera->PixelFormat.GetValue();

		switch (pixelFormat)
		{
		case PixelFormat_Mono8:				m_nBpp = 8; break;
		case PixelFormat_BayerRG8:
		case PixelFormat_BayerBG8:
		case PixelFormat_BayerGR8:
		case PixelFormat_BayerGB8:			/*m_nBpp = 24;*/ m_nBpp = 8; break;
		}

		m_pbyBuffer = new BYTE[m_nWidth * m_nHeight * m_nBpp / 8];
		ZeroMemory(m_pbyBuffer, m_nWidth * m_nHeight * m_nBpp / 8);
		m_pCamera->RegisterEventHandler(*this);
		m_CameraList.Clear();
		m_isConnect = true;
		return true;
	}
	catch (Spinnaker::Exception &e)
	{
		CString Error = (CString)e.what();
		return false;
	}
}

void CCamera::OnImageEvent(ImagePtr image)
{
	m_nTotalFrameCount++;
	ImageStatus imageStatus = image->GetImageStatus();
	if (imageStatus == IMAGE_NO_ERROR)
	{
		if (m_nBpp == 8)
		{
			memcpy(m_pbyBuffer, image->GetData(), image->GetImageSize());
			SetEvent(m_hGrabDone);
			//Check FrameRateCounter
		}
		else if (m_nBpp == 24)
		{
			ImagePtr convertImage = image->Convert(PixelFormat_BGR8, NEAREST_NEIGHBOR_AVG);
			memcpy(m_pbyBuffer, convertImage->GetData(), convertImage->GetImageSize());
			SetEvent(m_hGrabDone);
		}
	}
}

bool CCamera::GetValueByEnumByString(IN CString NodeName, OUT CString* Value)
{
	try
	{
		INodeMap &nodeMap = m_pCamera->GetNodeMap();
		CEnumerationPtr pEnum = nodeMap.GetNode((gcstring)NodeName);
		CEnumEntryPtr pEntry = pEnum->GetCurrentEntry();
		*Value = pEntry->GetSymbolic().c_str();
		return true;
	}
	catch (Spinnaker::Exception &e)
	{
		CString Error = (CString)e.what();
		return false;
	}
}

bool CCamera::SetValueByEnumByString(IN CString NodeName, IN CString Value)
{
	try
	{
		INodeMap &nodeMap = m_pCamera->GetNodeMap();
		CEnumerationPtr pEnum = nodeMap.GetNode((gcstring)NodeName);
		CEnumEntryPtr pEntry = pEnum->GetEntryByName((gcstring)Value);
		pEnum->SetIntValue(static_cast<int64_t>(pEntry->GetValue()));
		return true;
	}
	catch (Spinnaker::Exception &e)
	{
		CString Error = (CString)e.what();
		return false;
	}
}

bool CCamera::GetValueString(IN CString NodeName, OUT CString* Value)
{
	try
	{
		INodeMap &nodeMap = m_pCamera->GetNodeMap();
		CStringPtr pString = nodeMap.GetNode((gcstring)NodeName);
		*Value = pString->GetValue().c_str();
		return true;
	}
	catch (Spinnaker::Exception &e)
	{
		CString Error = (CString)e.what();
		return false;
	}
}

bool CCamera::SetValueString(IN CString NodeName, IN CString Value)
{
	try
	{
		INodeMap &nodeMap = m_pCamera->GetNodeMap();
		CStringPtr pString = nodeMap.GetNode((gcstring)NodeName);
		pString->SetValue((gcstring)Value);
		return true;
	}
	catch (Spinnaker::Exception &e)
	{
		CString Error = (CString)e.what();
		return false;
	}
}

bool CCamera::GetValueDouble(IN CString NodeName, OUT double* Value)
{
	try
	{
		INodeMap &nodeMap = m_pCamera->GetNodeMap();
		CFloatPtr pFloat = nodeMap.GetNode((gcstring)NodeName);
		*Value = pFloat->GetValue();
		return true;
	}
	catch (Spinnaker::Exception &e)
	{
		CString Error = (CString)e.what();
		return false;
	}
}

bool CCamera::SetValueDouble(IN CString NodeName, IN double Value)
{
	try
	{
		INodeMap &nodeMap = m_pCamera->GetNodeMap();
		CFloatPtr pFloat = nodeMap.GetNode((gcstring)NodeName);
		pFloat->SetValue(Value);
		return true;
	}
	catch (Spinnaker::Exception &e)
	{
		CString Error = (CString)e. what();
		return false;
	}
}

bool CCamera::GetValueInt(IN CString NodeName, OUT int* Value)
{
	try
	{
		INodeMap &nodeMap = m_pCamera->GetNodeMap();
		CIntegerPtr pInteger = nodeMap.GetNode((gcstring)NodeName);
		*Value = pInteger->GetValue();
		return true;
	}
	catch (Spinnaker::Exception &e)
	{
		CString Error = (CString)e.what();
		return false;
	}
}

bool CCamera::SetValueInt(IN CString NodeName, IN int Value)
{
	try
	{
		INodeMap &nodeMap = m_pCamera->GetNodeMap();
		CIntegerPtr pInteger = nodeMap.GetNode((gcstring)NodeName);
		pInteger->SetValue(Value);
		return true;
	}
	catch (Spinnaker::Exception &e)
	{
		CString Error = (CString)e.what();
		return false;
	}
}

bool CCamera::GetValueBool(IN CString NodeName, OUT bool* Value)
{
	try
	{
		INodeMap &nodeMap = m_pCamera->GetNodeMap();
		CBooleanPtr pBoolean = nodeMap.GetNode((gcstring)NodeName);
		*Value = pBoolean->GetValue();
		return true;
	}
	catch (Spinnaker::Exception &e)
	{
		CString Error = (CString)e.what();
		return false;
	}
}

bool CCamera::SetValueBool(IN CString NodeName, IN bool Value)
{
	try
	{
		INodeMap &nodeMap = m_pCamera->GetNodeMap();
		CBooleanPtr pBoolean = nodeMap.GetNode((gcstring)NodeName);
		pBoolean->SetValue(Value);
		return true;
	}
	catch (Spinnaker::Exception &e)
	{
		CString Error = (CString)e.what();
		return false;
	}
}

bool CCamera::OnExecuteCommand(IN CString NodeName)
{
	try
	{
		INodeMap &nodeMap = m_pCamera->GetNodeMap();
		CCommandPtr pCommand = nodeMap.GetNode((gcstring)NodeName);
		pCommand->Execute();
		return true;
	}
	catch (Spinnaker::Exception &e)
	{
		CString Error = (CString)e.what();
		return false;
	}
}
#endif

