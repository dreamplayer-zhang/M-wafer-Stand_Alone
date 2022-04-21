#include "StdAfx.h"
#include "Define.h"
#include "EllipsoMeter.h"
#include "AJIN_CtlClass.h"

//--------------------------------------------------------------------------------
// CMMC_CtlClass
//--------------------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CAJIN_CtlClass, CWnd)

CAJIN_CtlClass::CAJIN_CtlClass( )
{
	pMotion = (CATXMotion*)new CATXMotion;

	m_stModuleID.RemoveAll();
	m_stModuleID.FreeExtra();
	m_nModuleCount   = 0;

	m_bInitEnd       = FALSE;
	memset( m_bProcess, 0x00, sizeof( m_bProcess ));
	for( int i = 0; i < MAX_AXIS; i++ )
		m_bInitComp[i] = FALSE;


#if AJINTEX_BOARD
	InitLibrary();
#endif
}


CAJIN_CtlClass::~CAJIN_CtlClass( )
{
	m_stModuleID.RemoveAll();
	m_stModuleID.FreeExtra();

	pMotion->CloseMotion();
	
	delete pMotion;
	pMotion = nullptr;
}

BEGIN_MESSAGE_MAP(CAJIN_CtlClass, CWnd)
END_MESSAGE_MAP()


BOOL CAJIN_CtlClass::InitLibrary( )
{
	DWORD Code;
	DWORD dwStatus = 0;
#if AJINTEX_BOARD
	//++ AXL(AjineXtek Library)을 사용가능하게 하고 장착된 보드들을 초기화합니다.
	Code = AxlOpen(7);
	if ( Code == AXT_RT_SUCCESS )
	{
		TRACE("Library is initialized .\n");
	}
	else
	{
		AfxMessageBox("Failed initialization.");
		return FALSE;
	}

	pMotion->InitMotionSystem();
#endif

#if AJINTEX_BOARD_IO
	// Inspect if DIO module exsits
	Code = AxdInfoIsDIOModule(&dwStatus);
	if (dwStatus == STATUS_EXIST)
	{
		TRACE("DIO module exists.\n");
		long IModuleCounts = 0;
		Code = AxdInfoGetModuleCount(&IModuleCounts);
		if (Code == AXT_RT_SUCCESS)
			TRACE("Number of DIO module: %d\n", IModuleCounts);
		else
			TRACE("AxdInfoGetModuleCount() : ERROR code Ox%x\n", Code);

		long IInputCounts;
		long IOutputCounts;

		long IBoardNo;
		long IModulePos;
		DWORD dwModuleID;

		CString strData;
		for (int ModuleNo = 0; ModuleNo < IModuleCounts; ModuleNo++)
		{
			// Grasp IO channel
			AxdInfoGetInputCount(ModuleNo, &IInputCounts);
			AxdInfoGetOutputCount(ModuleNo, &IOutputCounts);

			if (AxdInfoGetModule(ModuleNo, &IBoardNo, &IModulePos, &dwModuleID) == AXT_RT_SUCCESS)
			{
				switch (dwModuleID)
				{
				case AXT_SIO_RDI32MLIII:		strData.Format("[BD No:%d - MD No:%d] RDI32MLIII",		IBoardNo, ModuleNo); break;
				case AXT_SIO_RDI32MSMLIII:		strData.Format("[BD No:%d - MD No:%d] RDI32MSMLIII",	IBoardNo, ModuleNo); break;
				case AXT_SIO_RDI32PMLIII:		strData.Format("[BD No:%d - MD No:%d] RDI32PMLIII",		IBoardNo, ModuleNo); break;
				case AXT_SIO_RDI32RTEX:			strData.Format("[BD No:%d - MD No:%d] RDI32RTEX",		IBoardNo, ModuleNo); break;
				case AXT_SIO_DI32_P:			strData.Format("[BD No:%d - MD No:%d] DI32_P",			IBoardNo, ModuleNo); break;
				case AXT_SIO_RDI32:				strData.Format("[BD No:%d - MD No:%d] RDI32",			IBoardNo, ModuleNo); break;
				case AXT_SIO_DI32:				strData.Format("[BD No:%d - MD No:%d] DI32",			IBoardNo, ModuleNo); break;
				case AXT_SIO_RDO32MLIII:		strData.Format("[BD No:%d - MD No:%d] RDO32MLIII",		IBoardNo, ModuleNo); break;
				case AXT_SIO_RDO32AMSMLIII:		strData.Format("[BD No:%d - MD No:%d] RDO32AMSMLIII",	IBoardNo, ModuleNo); break;
				case AXT_SIO_RDO32PMLIII:		strData.Format("[BD No:%d - MD No:%d] RDO32PMLIII",		IBoardNo, ModuleNo); break;
				case AXT_SIO_RDO16AMLII:		strData.Format("[BD No:%d - MD No:%d] RDO16AMLII",		IBoardNo, ModuleNo); break;
				case AXT_SIO_RDO16BMLII:		strData.Format("[BD No:%d - MD No:%d] RDO16BMLII",		IBoardNo, ModuleNo); break;
				case AXT_SIO_RDO32RTEX:			strData.Format("[BD No:%d - MD No:%d] RDO32RTEX",		IBoardNo, ModuleNo); break;
				case AXT_SIO_DO32T_P:			strData.Format("[BD No:%d - MD No:%d] DO32T_P",			IBoardNo, ModuleNo); break;
				case AXT_SIO_RDO32:				strData.Format("[BD No:%d - MD No:%d] RDO32",			IBoardNo, ModuleNo); break;
				case AXT_SIO_DO32P:				strData.Format("[BD No:%d - MD No:%d] DO32P",			IBoardNo, ModuleNo); break;
				case AXT_SIO_DO32T:				strData.Format("[BD No:%d - MD No:%d] DO32T",			IBoardNo, ModuleNo); break;
				case AXT_SIO_RDB32MLIII:		strData.Format("[BD No:%d - MD No:%d] RDB32MLIII",		IBoardNo, ModuleNo); break;
				case AXT_SIO_RDB32PMLIII:		strData.Format("[BD No:%d - MD No:%d] RDB32PMLIII",		IBoardNo, ModuleNo); break;
				case AXT_SIO_RDB128MLIIIAI:		strData.Format("[BD No:%d - MD No:%d] RDB128MLIIIAI",	IBoardNo, ModuleNo); break;
				case AXT_SIO_RDB96MLII:			strData.Format("[BD No:%d - MD No:%d] RDB96MLII",		IBoardNo, ModuleNo); break;
				case AXT_SIO_RDB32RTEX:			strData.Format("[BD No:%d - MD No:%d] RDB32RTEX",		IBoardNo, ModuleNo); break;
				case AXT_SIO_RDB128MLII:		strData.Format("[BD No:%d - MD No:%d] RDB128MLII",		IBoardNo, ModuleNo); break;
				case AXT_SIO_DB32P:				strData.Format("[BD No:%d - MD No:%d] DB32P",			IBoardNo, ModuleNo); break;
				case AXT_SIO_RDB32T:			strData.Format("[BD No:%d - MD No:%d] RDB32T",			IBoardNo, ModuleNo); break;
				case AXT_SIO_DB32T:				strData.Format("[BD No:%d - MD No:%d] DB32T",			IBoardNo, ModuleNo); break;
				case AXT_SIO_UNDEFINEMLIII:		strData.Format("[BD No:%d - MD No:%d] UNDEFINEMLIII",	IBoardNo, ModuleNo); break;
				case AXT_SIO_RSIMPLEIOMLII:		strData.Format("[BD No:%d - MD No:%d] RSIMPLEIOMLII",	IBoardNo, ModuleNo); break;
				default:						strData.Format("[BD No:%d - MD No:%d] Unknown",			IBoardNo, ModuleNo); break;
				}
				m_stModuleID.Add(strData);
				m_nModuleCount++;
			}
		}
	}
	else
	{
		AfxMessageBox("Module not exist.");
		return FALSE;
	}
#endif

	return TRUE;
}

DWORD CAJIN_CtlClass::m_fnAxmGantryGetEnable( long lMasterAxis, long lSlaveAxis, DWORD &dwState  )
{
#if AJINTEX_BOARD
	dwState = DISABLE;
	//++ 지정 축이 Gantry축으로 활성화되었는지 확인합니다.
	return AxmGantryGetEnable(lMasterAxis, NULL, NULL, NULL, &dwState);
#endif
	return 0;
}

DWORD CAJIN_CtlClass::m_fnAxmGantrySetEnable( long lMasterAxis, long lSlaveAxis, DWORD dwSlaveHmUse, double dSlaveHmOffset, double dSlaveHmRange )
{
#if AJINTEX_BOARD
	//++ 지정한 Master축과 Slave축으로 겐트리 기능을 활성화 시킵니다.
	return AxmGantrySetEnable(lMasterAxis, lSlaveAxis, dwSlaveHmUse, dSlaveHmOffset, dSlaveHmRange);
#endif
	return 0;
}

// DWORD CAJIN_CtlClass::m_fnAxdoSetNetworkErrorAct( long lModuleNo, DWORD dwSize, DWORD &dwSetValue  )
// {
// #if AJINTEX_BOARD
// 	return AxdoSetNetworkErrorAct( lModuleNo, dwSize, &dwSetValue );
// #endif
// 	return 0;
// }

DWORD CAJIN_CtlClass::m_fnAxmMoveAllStop(  )
{
	return pMotion->StopAxisAll(true);
}

// DWORD CAJIN_CtlClass::m_fnAxdInfoGetModule( long lModuleID, long &IBoardNo, long &IModulePos, DWORD &dwModuleID )
// {
// #if AJINTEX_BOARD
// 	return AxdInfoGetModule( lModuleID, &IBoardNo,&IModulePos, &dwModuleID);
// #else
// 	return 0;
// #endif
// }

// DWORD CAJIN_CtlClass::m_fnAxdInfoGetModuleCount( long &IModuleCounts )
// {
// #if AJINTEX_BOARD
// 	return AxdInfoGetModuleCount( &IModuleCounts );
// #endif
// 	return 0;
// }
// 
// DWORD CAJIN_CtlClass::m_fnAxmInfoGetAxisCount( long &IAxisCounts )
// {
// #if AJINTEX_BOARD
// 	return AxmInfoGetAxisCount( &IAxisCounts );
// #endif
// 	return 0;
// }
// 
// DWORD CAJIN_CtlClass::m_fnAxdiInterruptEdgeGetWord( long lModuleID, DWORD dwEdge, DWORD &dwState )
// {
// #if AJINTEX_BOARD
// 	return AxdiInterruptEdgeGetWord(lModuleID, 0, dwEdge, &dwState);
// #endif
// 	return 0;
// }
// 
// DWORD CAJIN_CtlClass::m_fnAxdiInterruptGetModuleEnable( long lModuleID, DWORD &dwInterrupt )
// {
// #if AJINTEX_BOARD
// 	return AxdiInterruptGetModuleEnable( lModuleID, &dwInterrupt);
// #endif
// 	return 0;
// }
// 
// DWORD CAJIN_CtlClass::m_fnGetAxdoReadOutportWord( long lModuleID, long lOffset, DWORD &dwState )
// {
// #if AJINTEX_BOARD
// 	return AxdoReadOutportWord( lModuleID, lOffset, &dwState);
// #endif
// 	return 0;
// }

DWORD CAJIN_CtlClass::m_fnGetAxdoReadOutportDword( long lModuleID, long lOffset, DWORD &dwState )
{
#if AJINTEX_BOARD
	return AxdoReadOutportDword( lModuleID, lOffset, &dwState);
#endif
	return 0;
}

// DWORD CAJIN_CtlClass::m_fnGetAxdiReadInportWord( long lModuleID, long lOffset, DWORD &dwState )
// {
// #if AJINTEX_BOARD
// 	return AxdiReadInportWord( lModuleID, lOffset, &dwState );
// #endif
// 	return 0;
// }

DWORD CAJIN_CtlClass::m_fnSetAxdoWriteOutportBit( long lModuleID, long lOffset, DWORD dwState )
{
#if AJINTEX_BOARD
	return AxdoWriteOutportBit(lModuleID, lOffset, dwState );
#endif
	return 0;
}

DWORD CAJIN_CtlClass::m_fnGetAxdiReadInportDword( long lModuleID, long lOffset, DWORD &dwState )
{
#if AJINTEX_BOARD
	return AxdiReadInportDword( lModuleID, lOffset, &dwState );
#endif
	return 0;
}

// DWORD CAJIN_CtlClass::m_fnGetAxdReadExtOutportDword(long lModuleID, long lOffset, DWORD &dwState)
// {
// #if AJINTEX_BOARD
// 	return AxdReadExtOutportDword(lModuleID, lOffset, &dwState);
// #endif
// 	return 0;
// }
// 
// BOOL CAJIN_CtlClass::m_fnGetAxlIsOpened(  )
// {
// #if AJINTEX_BOARD
// 	return AxlIsOpened();
// #else
// 	return FALSE;
// #endif
// }
// 
// BOOL CAJIN_CtlClass::m_fnSetAxlClose(  )
// {
// #if AJINTEX_BOARD
// 	return AxlClose();
// #endif
// 	return TRUE;
// }
// 
// DWORD CAJIN_CtlClass::m_fnSetAxdiInterruptSetModuleEnable(long lModuleID, DWORD dwEnable)
// {
// #if AJINTEX_BOARD
// 	return AxdiInterruptSetModuleEnable( lModuleID,dwEnable );
// #endif
// 	return 0;
// }
// 
// DWORD CAJIN_CtlClass::m_fnSetAxdiInterruptEdgeSetDword( long lModuleID, long lOffset, DWORD dwMode, DWORD dwValue)
// {
// #if AJINTEX_BOARD
// 	return AxdiInterruptEdgeSetDword(lModuleID, lOffset, dwMode, dwValue);
// #endif
// 	return 0;
// }
// 
// #if AJINTEX_BOARD
// DWORD CAJIN_CtlClass::m_fnSetAxdiInterruptSetModule(long lModuleID, HWND hWnd, DWORD uMessage, AXT_INTERRUPT_PROC pProc, HANDLE pEvent)
// {
// 	return AxdiInterruptSetModule(lModuleID, hWnd, uMessage, pProc, &pEvent);
// }
// #endif
// DWORD CAJIN_CtlClass::m_fnSetAxlInterruptDisable()
// {
// #if AJINTEX_BOARD
// 	return AxlInterruptDisable();
// #endif
// 	return 0;
// }
// 
// DWORD CAJIN_CtlClass::m_fnSetAxdiInterruptEdgeSetWord( long lModuleID, long lOffset, DWORD dwMode, DWORD dwValue)
// {
// #if AJINTEX_BOARD
// 	return AxdiInterruptEdgeSetWord(lModuleID, lOffset, dwMode, dwValue);
// #endif
// 	return 0;
// }
// 
// DWORD CAJIN_CtlClass::m_fnGetAxdiReadInportBit( long lModuleID, long lOffset, DWORD &dwValue )
// {
// #if AJINTEX_BOARD
// 	return AxdiReadInportBit( lModuleID, lOffset, &dwValue );
// #endif
// 	return 0;
// }
// 
// DWORD CAJIN_CtlClass::m_fnGetAxdiInterruptRead(long &lModuleID, DWORD &dwFlag)
// {
// #if AJINTEX_BOARD
// 	return AxdiInterruptRead( &lModuleID, &dwFlag);
// #endif
// 	return 0;
// }

DWORD CAJIN_CtlClass::LoadMotFile( char* szMotFile )
{
	DWORD dwRet = 0;
#if AJINTEX_BOARD
	//++ 지정한 Mot파일의 설정값들로 모션보드의 설정값들을 일괄변경 적용합니다.
	dwRet = AxmMotLoadParaAll( szMotFile );
	if( dwRet != AXT_RT_SUCCESS)
	{
		AfxMessageBox("Mot File Not Found.");
	}
#endif
	return dwRet;
}

// DWORD CAJIN_CtlClass::m_fnAxmInfoGetAxis(long lAxisNo, DWORD &dwModuleID )
// {
// #if AJINTEX_BOARD
// 	if ( lAxisNo >= MAX_AXIS)
// 	{
// 		dwModuleID = -1;
// 		return 0;
// 	}
// 	return AxmInfoGetAxis(lAxisNo, NULL, NULL, &dwModuleID );	
// #endif
// 	return 0;
// }
// 
// DWORD CAJIN_CtlClass::m_fnAxmStatusSetPosMatch(long lAxisNo, double dPos )
// {
// #if AJINTEX_BOARD
// 	if ( lAxisNo >= MAX_AXIS)
// 	{
// 		return 0;
// 	}
// 	return AxmStatusSetPosMatch(lAxisNo, dPos );	
// #endif
// 	return 0;
// }
// 
// DWORD CAJIN_CtlClass::m_fnSetAxmSignalServoOn(long lAxisNo, DWORD dwOnOff )
// {
// #if AJINTEX_BOARD
// 	if ( lAxisNo >= MAX_AXIS)
// 	{
// 		return 0;
// 	}
// 	return AxmSignalServoOn( lAxisNo, dwOnOff);
// #endif
// 	return 0;
// }
// 
// DWORD CAJIN_CtlClass::m_fnSetAxmSignalIServoAlarmReset(long lAxisNo, DWORD &dwOnOff )
// {
// #if AJINTEX_BOARD
// 	if ( lAxisNo >= MAX_AXIS)
// 	{
// 		dwOnOff = 0;
// 		return 0;
// 	}
// 	return AxmSignalServoAlarmReset( lAxisNo, dwOnOff);
// #endif
// 	return 0;
// }
// 
// DWORD CAJIN_CtlClass::m_fnGetAxmSignalIsServoOn(long lAxisNo, DWORD &dwOnOff )
// {
// #if AJINTEX_BOARD
// 	if ( lAxisNo >= MAX_AXIS)
// 	{
// 		dwOnOff = 0;
// 		return 0;
// 	}
// 	return AxmSignalIsServoOn( lAxisNo, &dwOnOff);
// #endif
// 	return 0;
// }

// DWORD CAJIN_CtlClass::m_fnGetAxmSignalGetServoAlarm(long lAxisNo, DWORD *upUse)
// {
// 	DWORD dwRtn = 0;
// #if AJINTEX_BOARD
// 	dwRtn = AxmSignalGetServoAlarm(lAxisNo, upUse);
// #endif
// 	return 0;
// }

DWORD CAJIN_CtlClass::m_fnGetAlarm(long lAxisNo, BOOL &bAlarmON )
{
	bAlarmON = (BOOL)pAxis[lAxisNo]->CheckAmpFault();

	return bAlarmON;
}

// DWORD CAJIN_CtlClass::m_fnSetAlarmReset(long lAxisNo )
// {
// 	DWORD dwRtn = 0;
// #if AJINTEX_BOARD
// 	if ( lAxisNo >= MAX_AXIS)
// 	{
// 		return dwRtn;
// 	}
// 	dwRtn         = AxmSignalServoAlarmReset( lAxisNo, TRUE );
// 	if (  dwRtn != AXT_RT_SUCCESS )
// 	{
// 		return dwRtn;
// 	}
// 	Sleep(200);
// 	dwRtn         = AxmSignalServoAlarmReset( lAxisNo, FALSE );
// 	if (  dwRtn != AXT_RT_SUCCESS )
// 	{
// 		return dwRtn;
// 	}
// #endif
// 	return dwRtn;
// }

DWORD CAJIN_CtlClass::m_fnSetAxmMoveStartPos(long lAxisNo, double dMovePos, double dMoveVel, double dMoveAcc, double dMoveDec)
{
	if ( lAxisNo >= MAX_AXIS)
	{
		return 0;
	}

	if ( !m_bInitComp[ lAxisNo ] )
	{
		theApp.m_fnGetViewPointer()->m_fnLogWrite( "Axis Not Initial State" );
		return 1;
	}

// #if AJINTEX_BOARD
	DWORD dwRtn;
	
	dwRtn = pAxis[lAxisNo]->AlarmReset();
	if (dwRtn != AXT_RT_SUCCESS)
		return dwRtn;

	dwRtn = m_fnGetGenturyAxisCheck( lAxisNo, SLAVE_AXIS );
	if (dwRtn != AXT_RT_SUCCESS)
		return dwRtn;

	if (!pAxis[lAxisNo]->AmpEnable())
		return 0;

// 	DWORD dwOnOff = 0;
// 	dwRtn         = AxmSignalIsServoOn( lAxisNo, &dwOnOff );
// 	if (  dwRtn != AXT_RT_SUCCESS )
// 	{
// 		return dwRtn;
// 	}
// 	if ( dwOnOff == 0 )
// 	{
// 		dwOnOff = 1;
// 		dwRtn = AxmSignalServoOn( lAxisNo, dwOnOff );
// 		if (  dwRtn != AXT_RT_SUCCESS )
// 		{
// 			return dwRtn;
// 		}
// 
// 		Sleep(100);
// 	}

// 	char strMsg[MAX_Buf];
	pAxis[lAxisNo]->fVelocity	 = (float)dMoveVel;
	pAxis[lAxisNo]->fAccel		 = (float)dMoveAcc;
	pAxis[lAxisNo]->fDeccel		 = (float)dMoveDec;
	pAxis[lAxisNo]->m_fSpeedFlag = 1.f;
	return pAxis[lAxisNo]->MoveAxis(dMovePos, 0, TRUE);
	
// 	if (( lAxisNo != Z_AXIS ) &&
// 		( lAxisNo != Y_AXIS ))
// 	{
// 		dMovePos *= -1;
// 	}
// 	return AxmMoveStartPos( lAxisNo, dMovePos * AXIS_RESOLUTION, dMoveVel * AXIS_RESOLUTION, dMoveAcc * AXIS_RESOLUTION, dMoveDec * AXIS_RESOLUTION);
// 
// #endif
// 	return 0;
}

DWORD CAJIN_CtlClass::m_fnSetAxmMoveVel( long lAxisNo, double dJogVel, long dJogAcc, long dJogDec )
{
	if ( lAxisNo >= MAX_AXIS)
	{
		return 0;
	}

	DWORD dwRtn   = 0;
	dwRtn = pAxis[lAxisNo]->AlarmReset();
	if (dwRtn != AXT_RT_SUCCESS)
		return dwRtn;

	dwRtn = m_fnGetGenturyAxisCheck( lAxisNo, SLAVE_AXIS );
	if ( dwRtn != AXT_RT_SUCCESS )
	{
		return dwRtn;
	}

	char strMsg[MAX_Buf];
	return pAxis[lAxisNo]->MoveAxisVel(dJogVel, strMsg);
}

DWORD CAJIN_CtlClass::m_fnSetAxmMoveSStop( long lAxisNo )
{
	return pAxis[lAxisNo]->StopAxis();
}

DWORD CAJIN_CtlClass::m_fnSetAxmMotSetAbsRelMode( long lAxisNo, DWORD dwAbsRelMode )
{
// #if AJINTEX_BOARD
// 	return AxmMotSetAbsRelMode( lAxisNo, dwAbsRelMode );
// #endif
	return 0;
}

// DWORD CAJIN_CtlClass::m_fnSetAxmMotSetProfileMode( long lAxisNo, DWORD dwProfileMode )
// {
// #if AJINTEX_BOARD
// 	return AxmMotSetProfileMode( lAxisNo, dwProfileMode );
// #endif
// 	return 0;
// }
// 
// DWORD CAJIN_CtlClass::m_fnAxmStatusGetCmdPos( long lAxisNo, double &dCmdPos )
// {
// #if AJINTEX_BOARD
// 	if ( AxmStatusGetCmdPos( lAxisNo, &dCmdPos ) == AXT_RT_SUCCESS )
// 	{
// 		dCmdPos /= AXIS_RESOLUTION;
// 		if (( lAxisNo != Y_AXIS ) &&
// 		    ( lAxisNo != Z_AXIS ))
// 		{
// 			dCmdPos *= -1;
// 		}
// 	}
// #endif
// 	if ( dCmdPos == -0.0 )
// 	{
// 		dCmdPos = 0.0;
// 	}
// 	return 0;
// }

DWORD CAJIN_CtlClass::m_fnAxmStatusGetActPos( long lAxisNo, double &dActPos )
{
	DWORD dResult = 0;
	dResult = pAxis[lAxisNo]->StatusGetPos(&dActPos);
// #if AJINTEX_BOARD
// 	if (lAxisNo >= MAX_AXIS )
// 	{
// 		dActPos  = 0.0;
// 		return dResult;
// 	}
// #if EQ_DEBUG
// 	dResult = AxmStatusGetCmdPos( lAxisNo, &dActPos );
// 	dActPos /= AXIS_RESOLUTION;
// 	if (( lAxisNo != Y_AXIS ) &&
// 		( lAxisNo != Z_AXIS ))
// 	{	
// 		dActPos *= -1;
// 	}
// #else
// 	dResult = AxmStatusGetActPos( lAxisNo, &dActPos );
// 	dActPos /= AXIS_RESOLUTION;
// 	if (( lAxisNo != Y_AXIS ) &&
// 		( lAxisNo != Z_AXIS ))
// 	{	
// 		dActPos *= -1;
// 	}
// #endif
// #endif
// 	if ( dActPos == -0.0 )
// 	{
// 		dActPos = 0.0;
// 	}
	return dResult;
}

// DWORD CAJIN_CtlClass::m_fnAxmStatusReadVel( long lAxisNo, double &dCmdVel )
// {
// #if AJINTEX_BOARD
// 	if ( lAxisNo >= MAX_AXIS)
// 	{
// 		dCmdVel = 0;
// 		return 0;
// 	}
// 	return AxmStatusReadVel( lAxisNo, &dCmdVel );
// #endif
// 	return 0;
// }
// 
// DWORD CAJIN_CtlClass::m_fnAxmMotGetAbsRelMode( long lAxisNo, DWORD &dwAbsRelMode )
// {
// #if AJINTEX_BOARD
// 	if ( lAxisNo >= MAX_AXIS)
// 	{
// 		dwAbsRelMode = 0;
// 		return 0;
// 	}
// 	return AxmMotGetAbsRelMode( lAxisNo, &dwAbsRelMode );
// #endif
// 	return 0;
// }
// 
// DWORD CAJIN_CtlClass::m_fnAxmMotGetProfileMode( long lAxisNo, DWORD &dwProfile )
// {
// #if AJINTEX_BOARD
// 	if ( lAxisNo >= MAX_AXIS)
// 	{
// 		dwProfile = 0;
// 		return 0;
// 	}
// 	return AxmMotGetProfileMode( lAxisNo, &dwProfile );
// #endif
// 	return 0;
// }

DWORD CAJIN_CtlClass::m_fnGetAxmStatusReadInMotion( long lAxisNo, DWORD &dwBusyStatus)
{
	dwBusyStatus = pAxis[lAxisNo]->CheckInMotion();
// #if AJINTEX_BOARD
// 	if ( lAxisNo >= MAX_AXIS)
// 	{
// 		dwBusyStatus = 0;
// 		return 0;
// 	}
// 	return AxmStatusReadInMotion( lAxisNo, &dwBusyStatus );
// #endif
	return dwBusyStatus;
}

CString  CAJIN_CtlClass::TranslateHomeResult(DWORD dwHomeResult)
{
	CString strResult = _T("");	
#if AJINTEX_BOARD

	switch(dwHomeResult){		
	case HOME_SUCCESS:			strResult.Format("[01H] HOME_SUCCESS");			break;
	case HOME_SEARCHING:		strResult.Format("[02H] HOME_SEARCHING");		break;
	case HOME_ERR_GNT_RANGE:	strResult.Format("[10H] HOME_ERR_GNT_RANGE");	break;
	case HOME_ERR_USER_BREAK:	strResult.Format("[11H] HOME_ERR_USER_BREAK");	break;
	case HOME_ERR_VELOCITY:		strResult.Format("[12H] HOME_ERR_VELOCITY");	break;
	case HOME_ERR_AMP_FAULT:	strResult.Format("[13H] HOME_ERR_AMP_FAULT");	break;
	case HOME_ERR_NEG_LIMIT:	strResult.Format("[14H] HOME_ERR_NEG_LIMIT");	break;
	case HOME_ERR_POS_LIMIT:	strResult.Format("[15H] HOME_ERR_POS_LIMIT");	break;
	case HOME_ERR_NOT_DETECT:	strResult.Format("[16H] HOME_ERR_NOT_DETECT");	break;
	case HOME_ERR_UNKNOWN:		strResult.Format("[FFH] HOME_ERR_UNKNOWN");		break;
	}
#endif
	return strResult;	
}

DWORD CAJIN_CtlClass::m_fnGetGenturyAxisCheck( long lMasterAxis, long lSlaveAxis ) 
{
	DWORD  dwRetCode        = 0;
#if AJINTEX_BOARD
	if ( lMasterAxis == MASTER_AXIS )
	{
		DWORD dwState = DISABLE;
		dwRetCode = m_fnAxmGantryGetEnable( MASTER_AXIS, SLAVE_AXIS, dwState );
		if ( dwRetCode == AXT_RT_SUCCESS )
		{
			if ( dwState != ENABLE )
			{
				dwRetCode = m_fnAxmGantrySetEnable( MASTER_AXIS, SLAVE_AXIS );
				if ( dwRetCode != AXT_RT_SUCCESS )
				{
					return dwRetCode;
				}
				else
				{
					dwRetCode = m_fnAxmGantryGetEnable( MASTER_AXIS, SLAVE_AXIS, dwState );
					if ( dwRetCode == AXT_RT_SUCCESS )
					{
						if ( dwState != ENABLE )
						{
							return STEP_HALT;
						}
					}
					else
					{
						return dwRetCode;
					}
				}
			}
		}
		else
		{
			return dwRetCode;
		}
	}
	return AXT_RT_SUCCESS;
#endif
	return 0;
}
DWORD CAJIN_CtlClass::m_fnSetSingleInit( long lAxisNo, int &nStep, CString &strProcess, BOOL &bComplete ) 
{
	if ( lAxisNo >= MAX_AXIS )
	{
		strProcess           = "초기화 완료";
		bComplete            = TRUE;
		m_bProcess[lAxisNo]  = FALSE;
		m_bInitComp[lAxisNo] = TRUE;
		return 0;
	}

	if ( m_bProcess[lAxisNo] ) return 0;
	m_bProcess[lAxisNo]		= TRUE;

	DWORD dwRetCode     = 0;
	DWORD dwUse			= 0;
	DWORD dwOnOff		= 0;
	DWORD dwStepMain	= 0;
	DWORD dwStepSub		= 0;

#if AJINTEX_BOARD
	switch ( nStep )
	{
		// 축 초기화
		case 100:	// Servo On Check
			{
				dwUse = 0;
				dwOnOff = 0;
				dwRetCode = AxmSignalReadServoAlarm( lAxisNo, &dwUse );
				if ( dwRetCode == AXT_RT_SUCCESS )
				{
					if ( dwUse != 0 )
					{
						dwRetCode         = AxmSignalServoAlarmReset( lAxisNo, TRUE );
						if (  dwRetCode != AXT_RT_SUCCESS )
						{
							return dwRetCode;
						}
						Sleep(200);
						dwRetCode         = AxmSignalServoAlarmReset( lAxisNo, FALSE );
						if (  dwRetCode != AXT_RT_SUCCESS )
						{
							return dwRetCode;
						}
					}
				}
				dwRetCode = m_fnGetGenturyAxisCheck( lAxisNo, SLAVE_AXIS );
				if ( dwRetCode != AXT_RT_SUCCESS )
				{
					nStep		         = STEP_HALT;
					m_bProcess[lAxisNo]  = FALSE;
					return dwRetCode;
				}
				dwRetCode = AxmSignalIsServoOn( lAxisNo, &dwOnOff );
				if ( dwRetCode != AXT_RT_SUCCESS )
				{
					nStep		         = STEP_HALT;
					m_bProcess[lAxisNo]  = FALSE;
					return dwRetCode;

				}
				if ( dwOnOff == 1 )
				{
					strProcess  = "서버 ON 완료";
					nStep		= 120;
				}
				else
				{
					dwOnOff = 1;
					dwRetCode = AxmSignalServoOn( lAxisNo, dwOnOff );
					if ( dwRetCode != AXT_RT_SUCCESS )
					{
						nStep		         = STEP_HALT;
						m_bProcess[lAxisNo]  = FALSE;
						return dwRetCode;
					}
					strProcess  = "서버 ON 시작";
					nStep		= 110;
				}
			}
			break;
		case 110:	// Servo On Check
			{
				//++ 지정한 축의 서보온 상태를 반환합니다.
				dwOnOff = 0;
				dwRetCode = AxmSignalIsServoOn( lAxisNo, &dwOnOff);
				if ( dwRetCode != AXT_RT_SUCCESS )
				{
					nStep		         = STEP_HALT;
					m_bProcess[lAxisNo]  = FALSE;
					return dwRetCode;
				}
				if ( dwOnOff == 1 )
				{
					strProcess  = "서버 ON 완료";
					nStep		= 120;
				}
			}
			break;
		case 120:	// Init
			{
				//++ 지정한 축에 원점검색을 진행합니다.
				dwRetCode = AxmHomeSetStart( lAxisNo );	
				if (dwRetCode != AXT_RT_SUCCESS)
				{
					strProcess = TranslateHomeResult(dwRetCode);
					m_bProcess[lAxisNo]  = FALSE;
					return dwRetCode;
				}
				else
				{
					strProcess  = "초기화 시작";
					nStep		= 140;
				}
			}
			break;
		case 140:	// End
			{
				dwStepMain    = 0;
				dwStepSub		= 0;
				dwRetCode		= AxmHomeGetRate(lAxisNo, &dwStepMain, &dwStepSub);
				if(dwRetCode == AXT_RT_SUCCESS)
				{
					if ( dwStepSub == 100 )
					{
						AxmStatusSetPosMatch(lAxisNo, 0.0);	
						strProcess           = "초기화 완료";
						bComplete            = TRUE;
						m_bProcess[lAxisNo]  = FALSE;
						m_bInitComp[lAxisNo] = TRUE;
						return 0;
					}
				}
				else
				{
					strProcess           = TranslateHomeResult(dwRetCode);
					nStep		         = STEP_HALT;
					m_bProcess[lAxisNo]  = FALSE;
					return dwRetCode;
				}
			}
			break;
	}
#endif
	m_bProcess[lAxisNo]  = FALSE;
	return 0;
}

DWORD CAJIN_CtlClass::m_fnGetAxmSignalReadInputBit( long lAxisNo, long lBitNo, DWORD &upOn)
{
#if AJINTEX_BOARD
	if ( lAxisNo >= MAX_AXIS)
	{
		return 0;
	}
	return AxmSignalReadInputBit( lAxisNo, lBitNo, &upOn );

#endif
	return 0;
}

DWORD CAJIN_CtlClass::m_fnSetAxmSignalWriteOutputBit( long lAxisNo, long lBitNo, DWORD uOnOff)
{
#if AJINTEX_BOARD
	if ( lAxisNo >= MAX_AXIS)
	{
		return 0;
	}
	return AxmSignalWriteOutputBit( lAxisNo, lBitNo, uOnOff );
#endif
	return 0;
}

DWORD CAJIN_CtlClass::m_fnGetAxmSignalReadOutputBit( long lAxisNo, long lBitNo, DWORD &uOnOff)
{
#if AJINTEX_BOARD
	if ( lAxisNo >= MAX_AXIS)
	{
		return 0;
	}
	return AxmSignalReadOutputBit( lAxisNo, lBitNo, &uOnOff );
#endif
	return 0;
}

