#pragma once

// #include "stdafx.h"
#include "afxwin.h"
// #include "math.h"
#include "ATXMotion.h"

// ++ =======================================================================
// >> AXL(AjineXtek Library) 관련 Header파일 포함시킵니다.
// ※ [CAUTION] 예제를 복사하여 다른 위치에서 실행시 참조경로 변경이 필요합니다.
// --------------------------------------------------------------------------
// #if AJINTEX_BOARD
// #include "AXL.h"
// #include "AXM.h"
// #include "AXD.h"
// #include "AXHS.h"
// ++ =======================================================================
// >> AXL(AjineXtek Library) 관련 Library파일을 Link시킵니다.
// ※ [CAUTION] 예제를 복사하여 다른 위치에서 실행시 참조경로 변경이 필요합니다.
// --------------------------------------------------------------------------
// #ifdef  _M_X64
// #pragma comment(lib, "./AXL(Library)/Library/64Bit/AxL.lib")
// #else
// #pragma comment(lib, "AxL.lib")
// #endif
// #endif

class CAJIN_CtlClass : public CWnd
{
	DECLARE_DYNAMIC(CAJIN_CtlClass)

	CStringArray    m_stModuleID;
	int             m_nModuleCount;

public:
	CAJIN_CtlClass( );
	virtual ~CAJIN_CtlClass();

	DWORD					LoadMotFile( char* szMotFile );
	BOOL					InitLibrary( void );
// 	DWORD					m_fnAxdoSetNetworkErrorAct( long lModuleNo, DWORD dwSize, DWORD &dwaSetValue  );
// 	DWORD					m_fnAxdInfoGetModule( long lModuleID, long &IBoardNo, long &IModulePos, DWORD &dwModuleID );
// 	DWORD					m_fnAxmInfoGetAxisCount( long &IAxisCounts );
// 	DWORD					m_fnAxdInfoGetModuleCount( long &IModuleCounts );
// 	DWORD					m_fnAxdiInterruptGetModuleEnable( long lModuleID, DWORD &dwInterrupt );
// 	DWORD					m_fnAxdiInterruptEdgeGetWord( long lModuleID, DWORD dwEdge, DWORD &dwState );
// 	DWORD					m_fnGetAxdoReadOutportWord( long lModuleID, long lOffset, DWORD &dwState );
	DWORD                   m_fnGetAxdoReadOutportDword( long lModuleID, long lOffset, DWORD &dwState );

// 	DWORD					m_fnGetAxdiReadInportWord( long lModuleID, long lOffset, DWORD &dwState );
	DWORD					m_fnGetAxdiReadInportDword( long lModuleID, long lOffset, DWORD &dwState );
// 	DWORD					m_fnGetAxdReadExtOutportDword(long lModuleID, long lOffset, DWORD &dwState);
	DWORD					m_fnSetAxdoWriteOutportBit( long lModuleID, long lOffset, DWORD dwState );
// 	BOOL					m_fnGetAxlIsOpened( void );
// 	BOOL					m_fnSetAxlClose( void );
// 	DWORD					m_fnSetAxdiInterruptSetModuleEnable(long lModuleID, DWORD dwEnable);
// 	DWORD					m_fnSetAxdiInterruptEdgeSetDword( long lModuleID, long lOffset, DWORD dwMode, DWORD dwValue);
	DWORD					m_fnAxmGantryGetEnable( long lMasterAxis, long lSleveAxis, DWORD &dwState );
	DWORD					m_fnAxmGantrySetEnable( long lMasterAxis, long lSleveAxis, DWORD dwSlaveHmUse = 0, double dSlaveHmOffset = 0.0, double dSlaveHmRange = 0.0 );
	DWORD					m_fnGetGenturyAxisCheck( long lMasterAxis, long lSlaveAxis ); 
// #if AJINTEX_BOARD
// 	DWORD					m_fnSetAxdiInterruptSetModule(long lModuleID, HWND hWnd, DWORD uMessage, AXT_INTERRUPT_PROC pProc, HANDLE pEvent);
// #endif
// 	DWORD					m_fnSetAxlInterruptDisable();
// 	DWORD					m_fnSetAxdiInterruptEdgeSetWord( long lModuleID, long lOffset, DWORD dwMode, DWORD dwValue);
// 	DWORD					m_fnGetAxdiReadInportBit( long lModuleID, long lOffset, DWORD &dwValue );
// 	DWORD					m_fnGetAxdiInterruptRead(long &lModuleID, DWORD &dwFlag);
// 	DWORD					m_fnAxmInfoGetAxis(long lAxisNo, DWORD &dwModuleID );
// 	DWORD					m_fnAxmStatusSetPosMatch(long lAxisNo, double dPos );
// 	DWORD					m_fnSetAxmSignalServoOn(long lAxisNo, DWORD dwOnOff );
	DWORD					m_fnSetAxmMoveStartPos(long lAxisNo, double dMovePos, double dMoveVel, double dMoveAcc, double dMoveDec);
	DWORD					m_fnSetAxmMoveSStop( long lAxisNo );
	DWORD                   m_fnSetAxmMotSetAbsRelMode( long lAxisNo, DWORD dwAbsRelMode );
// 	DWORD					m_fnSetAxmMotSetProfileMode( long lAxisNo, DWORD dwProfileMode );
	DWORD					m_fnSetAxmMoveVel( long lAxisNo, double dJogVel, long dJogAcc, long dJogDec );
// 	DWORD					m_fnAxmStatusGetCmdPos( long lAxisNo, double &dCmdPos );
// 	DWORD					m_fnAxmStatusReadVel( long lAxisNo, double &dCmdVel );
	DWORD					m_fnAxmStatusGetActPos( long lAxisNo, double &dActPos );
// 	DWORD					m_fnAxmMotGetAbsRelMode( long lAxisNo, DWORD &dwAbsRelMode );
// 	DWORD					m_fnAxmMotGetProfileMode( long lAxisNo, DWORD &dwProfile );
	DWORD					m_fnGetAxmStatusReadInMotion( long lAxisNo, DWORD &dwBusyStatus);
	DWORD                   m_fnAxmMoveAllStop( void );
// 	DWORD					m_fnGetAxmSignalIsServoOn(long lAxisNo, DWORD &dwOnOff );
// 	DWORD					m_fnSetAxmSignalIServoAlarmReset(long lAxisNo, DWORD &dwOnOff );
// 	DWORD					m_fnSetAlarmReset(long lAxisNo );
	DWORD					m_fnGetAlarm(long lAxisNo, BOOL &bAlarmON );
	DWORD					m_fnGetAxmSignalReadInputBit( long lAxisNo, long lBitNo, DWORD &upOn);
	DWORD					m_fnSetAxmSignalWriteOutputBit( long lAxisNo, long lBitNo, DWORD uOnOff);
	DWORD					m_fnGetAxmSignalReadOutputBit( long lAxisNo, long lBitNo, DWORD &uOnOff);

	CString					TranslateHomeResult(DWORD dwHomeResult);
	BOOL                    m_bProcess[MAX_AXIS+1];
	DWORD					m_fnSetSingleInit( long lAxisNo, int &nStep, CString &strProcess, BOOL &bComplete );
	BOOL                    m_bInitEnd;
	BOOL                    m_bInitComp[MAX_AXIS+1];
	DWORD					m_fnGetAxmSignalGetServoAlarm(long lAxisNo, DWORD *upUse);

public:

protected:

	DECLARE_MESSAGE_MAP()
};

