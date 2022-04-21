#if !defined(AFX_STRUCT_H__71165AE8_0FC5_4063_AB20_7D3871E2D3BB__INCLUDED_)
#define AFX_STRUCT_H__71165AE8_0FC5_4063_AB20_7D3871E2D3BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Define.h"

/*-----------------------------------------------------------------------
	I/O  구조체
-----------------------------------------------------------------------*/
typedef struct _t_LAN_INFO
{
	_t_LAN_INFO()
	{
		bConnect											= FALSE;
		bPassive											= FALSE;
		memset( szIP, 0x00, sizeof( szIP ));
		nPort1												= 0;
		nPort2												= 0;
		nReConnectTime                                      = 0;
	}
	
	BOOL	bConnect;

	BOOL	bPassive;													// PASSIVE 1
	char	szIP[FILE_LEN+1];											// IP
	int		nReConnectTime;												// 재접속 주기
	int		nPort1;														// IP PORT
	int		nPort2;														// ASYNC IP PORT
} t_LAN_INFO;

/*-----------------------------------------------------------------------
	Environment 구조체
-----------------------------------------------------------------------*/
typedef struct _t_ENV_INFO
{
	_t_ENV_INFO()
	{
		memset( szXRFPath, 0x00, sizeof(szXRFPath));
	}
	
	char	szXRFPath[FILE_LEN+1];										// XRF 프로그램 저장위치
} t_ENV_INFO;

/*-----------------------------------------------------------------------
	DATA 구조체
-----------------------------------------------------------------------*/
typedef struct _t_DATA_INFO
{
	_t_DATA_INFO()
	{
		memset( szDate, 0x00, sizeof(szDate));
		memset( szTime, 0x00, sizeof(szTime));
		nSeqNo              = 0;
		dPos                = 0.0;
		memset( dItemData, 0x00, sizeof(dItemData));
	}
	char   szDate[FILE_LEN+1];
	char   szTime[FILE_LEN+1];
	int    nSeqNo;
	double dPos;
	double dItemData[MAX_ITEM_DATA+1];
} t_DATA_INFO;

/*-----------------------------------------------------------------------
	WAPER 좌표생성
-----------------------------------------------------------------------*/
typedef struct _t_WAPER_INFO
{
	_t_WAPER_INFO()
	{
		bCw				= FALSE;
		nTotal			= 0;
		bMsgDisp		= TRUE; 
		memset( dX, 0x00, sizeof( dX ));
		memset( dY, 0x00, sizeof( dY ));
		bOK				= FALSE;
		dMM				= 0.0;
		dR				= 0.0;
		bZeroInclude	= TRUE;
		bOnlyZero		= FALSE;
	}
	BOOL	bCw;			// TRUE : 시계방향, FALSE : 시계반대방향
	BOOL    bOK;			// TRUE : 정상처리, FALSE : 실패
	BOOL    bMsgDisp;		// TRUE : 표시, FALSE : 비표시
	BOOL    bZeroInclude;
	BOOL    bOnlyZero;
	double  dMM;			// 반지름
	double  dR;				// 각도
	int     nTotal;			// 생성갯수
	double  dX[MAX_DATA];	// dX 위치값
	double  dY[MAX_DATA];	// dY 위치값
} t_WAPER_INFO;

/*-----------------------------------------------------------------------
	RS232C / RS485 COM PORT 접속 관련 구조체
-----------------------------------------------------------------------*/
typedef struct _t_COM_PORT_INFO
{
	_t_COM_PORT_INFO()
	{
		bConnect											= FALSE;
		nComPort											= 0;
		nBaudRate											= 0;
		nDataBit											= 0;
		nStopBit											= 0;
		nParityBit											= 0;
		nFlowCtrl											= 0;
		nDataLength											= 0;
		nChannel											= 0;
	}
	
	BOOL	bConnect;

	int		nComPort;													// COM PORT
	int		nBaudRate;													// 속도
	int		nDataBit;													// DATA BIT
	int		nStopBit;													// STOP BIT
	int		nParityBit;													// PARITY BIT
	int		nFlowCtrl;													// FlowCtrl
	int		nDataLength;												// Length
	int     nChannel;
} t_COM_PORT_INFO;

typedef struct _t_EQP_INFO
{
	_t_EQP_INFO()
	{
		bPgmExit						= FALSE;
		bEqpInit                        = FALSE;
		bIoStart						= FALSE;
		bIoStartButton					= FALSE;
		bIoStopButton					= FALSE;
		bEqpEmergencySwitch             = FALSE;
		nJoyStickSpeed                  = JOG_ZERO;
		nJoyStickArrowXY				= JOG_STOP;
		nJoyStickArrowZ					= JOG_STOP;
	}

	BOOL    bPgmExit;					// 프로그램 종료
	BOOL    bEqpInit;
	BOOL    bEqpEmergencySwitch;
	BOOL    bIoStart;
	BOOL    bIoStartButton;
	BOOL    bIoStopButton;
	int     nJoyStickSpeed;
	int     nJoyStickArrowXY;
	int     nJoyStickArrowZ;
	
} t_EQP_INFO;


/*-----------------------------------------------------------------------
	구조체 변경사항을 통지한다.
-----------------------------------------------------------------------*/
typedef struct _t_COMMON_INFO
{
	_t_COMMON_INFO()
	{
		bAlarmOn         = FALSE;
		bLogSave         = FALSE;
		bExcelSave       = FALSE;
		bChartReport     = FALSE;
		dSpeedSlow		 = 0.0L;
		dSpeedMiddle	 = 0.0L;
		dSpeedHigh		 = 0.0L;
		dSpeedSlowZ      = 0.0L;
		dSpeedMiddleZ    = 0.0L;
		dSpeedHighZ      = 0.0L;
		dInspXYSpeed     = 0.0L;
		dInspZSpeed      = 0.0L;
		dXMLimit         = 0.0L;
		dXPLimit         = 0.0L;
		dYMLimit         = 0.0L;
		dYPLimit         = 0.0L;
		dLoadingX		 = 0.0L;
		dLoadingY		 = 0.0L;
		dLoadingX2		 = 0.0L;
		dLoadingY2		 = 0.0L;
		dZLimit          = 0.0L;
		dZAlign          = 0.0L;
		dZMesaureOffset        = 0.0L;
		dZReadySafety        = 0.0L;
		dZMSafety        = 0.0L;
		nInspLight       = 0;
		memset( szExcelResultPath, 0x00, sizeof( szExcelResultPath ));
		memset( szXmlResultPath, 0x00, sizeof( szXmlResultPath ));
		memset( szTrendResultPath, 0x00, sizeof( szTrendResultPath ));
		memset( szLogoPath, 0x00, sizeof( szLogoPath ));
		memset( szQrCodePath, 0x00, sizeof( szQrCodePath ));
		memset( szTitle, 0x00, sizeof( szTitle ));
		memset( dAlignX, 0x00, sizeof( dAlignX ));
		memset( dAlignY, 0x00, sizeof( dAlignY ));
	}

	BOOL    bAlarmOn;
	BOOL    bLogSave;
	BOOL    bExcelSave;
	BOOL    bChartReport;

	char    szLogoPath[FILE_LEN+1];
	char    szQrCodePath[FILE_LEN+1];
	char    szExcelResultPath[FILE_LEN+1];
	char    szXmlResultPath[FILE_LEN+1];
	char    szTrendResultPath[FILE_LEN+1];
	char    szTitle[FILE_LEN+1];

	double  dSpeedSlow;
	double  dSpeedMiddle;
	double  dSpeedHigh;
	double  dSpeedSlowZ;
	double  dSpeedMiddleZ;
	double  dSpeedHighZ;

	double  dInspXYSpeed;
	double  dInspZSpeed;
	int     nInspLight;
	double  dAlignX[MAX_ALIGN_KIND][MAX_WAPER_KIND];
	double  dAlignY[MAX_ALIGN_KIND][MAX_WAPER_KIND];

	double  dXMLimit;
	double  dXPLimit;
	double  dYMLimit;
	double  dYPLimit;

	double  dLoadingX;
	double  dLoadingY;
	double  dLoadingX2;
	double  dLoadingY2;

	double  dZLimit;
	double  dZAlign;
	double  dZMesaureOffset;
	double  dZReadySafety;
	double  dZMSafety;
} t_COMMON_INFO;

typedef struct _t_INPUT_BIT_IO
{
	_t_INPUT_BIT_IO()
	{
		Jog_X_Plus									= 0;
		Jog_X_Minus									= 0;
		Jog_Y_Plus									= 0;
		Jog_Y_Minus									= 0;
		Jog_Z_Plus									= 0;
		Jog_Z_Minus									= 0;
		Jog_Speed_Low								= 0;
		Jog_Speed_High								= 0;
		Measure_Switch								= 0;
		Loading_Switcn								= 0;
		Vacuum_on									= 0;
		Reserved									= 0;
	}
	unsigned int Jog_X_Plus											: 1;
	unsigned int Jog_X_Minus										: 1;
	unsigned int Jog_Y_Plus											: 1;
	unsigned int Jog_Y_Minus										: 1;
	unsigned int Jog_Z_Plus											: 1;
	unsigned int Jog_Z_Minus										: 1;
	unsigned int Jog_Speed_Low										: 1;
	unsigned int Jog_Speed_High										: 1;
	unsigned int Measure_Switch										: 1;
	unsigned int Loading_Switcn										: 1;
	unsigned int Vacuum_on											: 1;
	unsigned int Reserved											: 1;
} t_INPUT_BIT_IO;

typedef struct _t_OUTPUT_BIT_IO
{
	_t_OUTPUT_BIT_IO()
	{
		Tower_Lamp_Red								= 0;
		Tower_Lamp_Yellow							= 0;
		Tower_Lamp_Green							= 0;
		Buzzer	      								= 0;
		Vacuum_On									= 0;		
		Loading_Switch_Lamp_On						= 0;
		Measure_Lamp_On								= 0;
		Reserved									= 0;
	}
	unsigned int Measure_Lamp_On									: 1;
	unsigned int Loading_Switch_Lamp_On								: 1;
	unsigned int Vacuum_On		      								: 1;
	unsigned int Tower_Lamp_Red										: 1;
	unsigned int Tower_Lamp_Yellow									: 1;
	unsigned int Tower_Lamp_Green									: 1;
	unsigned int Buzzer			      								: 1;
	unsigned int Reserved											: 9;
} t_OUTPUT_BIT_IO;

/*-----------------------------------------------------------------------
	구조체 변경사항을 통지한다.
-----------------------------------------------------------------------*/
typedef struct _t_IO_INFO
{
	_t_IO_INFO()
	{
		bConnect						= FALSE;
		bPgmExit						= FALSE;
	}

	BOOL    bConnect;

	t_INPUT_BIT_IO						stIn;

	t_OUTPUT_BIT_IO						stOut;

	t_OUTPUT_BIT_IO						stSts;

	BOOL    bPgmExit;					// 프로그램 종료

} t_IO_INFO;

typedef struct _t_ELIPSO_INPUT_BIT_IO
{
	_t_ELIPSO_INPUT_BIT_IO()
	{
		Wafer_Vacuum_Sensor_06_Inch					= 0;
		Wafer_Vacuum_Sensor_08_Inch					= 0;
		Wafer_Vacuum_Sensor_12_Inch					= 0;
		EFEM_Finish									= 0;
		EFEM_Run									= 0;
		Reserved05									= 0;
		Wafer_Detect_Sensor							= 0;
		Reserved07									= 0;
		EFEM_Alarm									= 0;
		EMS_1										= 0;
		EMS_2										= 0;
		EMS_3										= 0;
		EMS_4										= 0;
		Reserved13									= 0;
		Reserved14									= 0;
		Reserved15									= 0;
	}
	unsigned int Wafer_Vacuum_Sensor_06_Inch		: 1;
	unsigned int Wafer_Vacuum_Sensor_08_Inch		: 1;
	unsigned int Wafer_Vacuum_Sensor_12_Inch		: 1;
	unsigned int EFEM_Finish						: 1;
	unsigned int EFEM_Run							: 1;
	unsigned int Reserved05							: 1;
	unsigned int Wafer_Detect_Sensor				: 1;
	unsigned int Reserved07							: 1;
	unsigned int EFEM_Alarm							: 1;
	unsigned int EMS_1								: 1;
	unsigned int EMS_2								: 1;
	unsigned int EMS_3								: 1;
	unsigned int EMS_4								: 1;
	unsigned int Reserved13							: 1;
	unsigned int Reserved14							: 1;
	unsigned int Reserved15							: 16;
} t_ELIPSO_INPUT_BIT_IO;

typedef struct _t_ELIPSO_OUTPUT_BIT_IO
{
	_t_ELIPSO_OUTPUT_BIT_IO()
	{
		Vacuum_On_06_Inch							= 0;
		Vacuum_On_08_Inch							= 0;
		Vacuum_On_12_Inch		      				= 0;
		Reserved03									= 0;
		Reserved04									= 0;
		Measure_Ready								= 0;
		Measure_Busy		      					= 0;
		Reserved07									= 0;
		Ellipsometer_Alarm							= 0;
		Reserved09									= 0;
		Reserved10									= 0;
		Reserved11									= 0;
		Reserved12									= 0;
		Wafer_Bit_01								= 0;
		Wafer_Bit_10								= 0;
		Reserved15									= 0;
	}
	unsigned int Vacuum_On_06_Inch					: 1;
	unsigned int Vacuum_On_08_Inch					: 1;
	unsigned int Vacuum_On_12_Inch		      		: 1;
	unsigned int Reserved03							: 1;
	unsigned int Reserved04							: 1;
	unsigned int Measure_Ready						: 1;
	unsigned int Measure_Busy						: 1;
	unsigned int Reserved07							: 1;
	unsigned int Ellipsometer_Alarm					: 1;
	unsigned int Reserved09							: 1;
	unsigned int Reserved10							: 1;
	unsigned int Reserved11							: 1;
	unsigned int Reserved12							: 1;
	unsigned int Wafer_Bit_01						: 1;
	unsigned int Wafer_Bit_10						: 1;
	unsigned int Reserved15							: 16;
} t_ELIPSO_OUTPUT_BIT_IO;

/*-----------------------------------------------------------------------
	구조체 변경사항을 통지한다.
-----------------------------------------------------------------------*/
typedef struct _t_ELIPSO_IO_INFO
{
	_t_ELIPSO_IO_INFO()
	{
		bConnect						= FALSE;
		bPgmExit						= FALSE;
	}

	BOOL    bConnect;

	t_ELIPSO_INPUT_BIT_IO				stIn;

	t_ELIPSO_OUTPUT_BIT_IO				stOut;

	BOOL    bPgmExit;					// 프로그램 종료

} t_ELIPSO_IO_INFO;

typedef struct _t_LOGIN
{
	_t_LOGIN()
	{
		nLevel = UNKNOWN_USER;
		bLogin = FALSE;
		memset( szUserID, 0x00, sizeof( szUserID ));
		memset( szPasswd, 0x00, sizeof( szPasswd ));
		nTotal = 0;
	}
	BOOL bLogin;
	char szUserID[FILE_LEN];
	char szName[FILE_LEN];
	char szPasswd[FILE_LEN];
	char szJob[FILE_LEN];
	char szDescription[FILE_LEN];
	int  nLevel;
	int  nTotal;
} t_LOGIN;

/*-----------------------------------------------------------------------
	사용자 입력 구조체
-----------------------------------------------------------------------*/
typedef struct _t_USER_INFO
{
	_t_USER_INFO()
	{
		memset(szLotID, 0x00, sizeof(szLotID));

		dSampleSize             = 0.0;
		dThickness				= 0.0;
		dExclusion              = 0.0;

		memset(szRecipeFileName, 0x00, sizeof(szRecipeFileName));

		memset(szInsertMode, 0x00, sizeof(szInsertMode));
		memset(szWaferAlignMode, 0x00, sizeof(szWaferAlignMode));
		memset(szVisionAFMode, 0x00, sizeof(szVisionAFMode));

		memset(szDate, 0x00, sizeof(szDate));
		iSlotNo = 0;
		memset(szOperatorID, 0x00, sizeof(szOperatorID));

		memset(szTimeStart, 0x00, sizeof(szTimeStart));
		memset(szTimeEnd, 0x00, sizeof(szTimeEnd));
		memset(szTimeSpend, 0x00, sizeof(szTimeSpend));

		memset(szDataFile, 0x00, sizeof(szDataFile));

		memset(szDescription, 0x00, sizeof(szDescription));

		iMeasureMode = -1;

		//////////////////////////////////////////////////////////////////////////
		memset(szMatResistivity, 0x00, sizeof(szMatResistivity));
		dProbeSpace				= 0.0;
		dTMeasure				= 0.0;
		memset(szMMode, 0x00, sizeof(szMMode));
		memset(szAnalysis, 0x00, sizeof(szAnalysis));

		dFlat                   = 0.0;
		memset(szMaterial, 0x00, sizeof(szMaterial));
		dCorrectionF			= 0.0;
		dTCoefficient			= 0.0;
		dTReference				= 0.0;
		dSigmaMax               = 0.0;

	    memset(szThinfilmmaterial, 0x00, sizeof(szThinfilmmaterial)); 
		dThinfilmthickness		= 0.0;
		dSigmaMin               = 0.0;
	    memset(szTime, 0x00, sizeof(szTime)); 
	}

	char   szLotID[FILE_LEN + 1];

	double dSampleSize;
	double dThickness;
	double dExclusion;

	char   szRecipeFileName[FILE_LEN + 1];

	char   szInsertMode[FILE_LEN + 1];
	char   szWaferAlignMode[FILE_LEN + 1];
	char   szVisionAFMode[FILE_LEN + 1];

	char   szDate[FILE_LEN];
	int    iSlotNo;
	char   szOperatorID[FILE_LEN+1];

	char   szTimeStart[FILE_LEN + 1];
	char   szTimeEnd[FILE_LEN + 1];
	char   szTimeSpend[FILE_LEN + 1];

	char   szDataFile[FILE_LEN];

	char   szDescription[FILE_LEN+1];

	int iMeasureMode;

	//////////////////////////////////////////////////////////////////////////
	char   szTime[FILE_LEN]; 
	char   szMatResistivity[FILE_LEN+1];
	double dProbeSpace;
	double dTMeasure;
	char   szMMode[FILE_LEN+1];
	char   szAnalysis[FILE_LEN+1];

	double dFlat;
	char   szMaterial[FILE_LEN+1];
	double dCorrectionF;
	double dTCoefficient;
	double dTReference;
	double dSigmaMax;

	char   szThinfilmmaterial[FILE_LEN]; 
	double dThinfilmthickness;
	double dSigmaMin;

} t_USER_INFO;

typedef struct _t_DATA
{
	_t_DATA()
	{
		iColum = 0;
		iRow = 0;
		dColum = 0.0;
		dRow = 0.0;
		memset(szName, 0x00, sizeof(szName));
		iSub		= 0;
		dwXPosition = 0.0L;
		dwYPosition = 0.0L;
		dwZPosition = 0.0L;
		memset(dwResult, 0x00, sizeof(dwResult));
	}
	int    iColum;
	int    iRow;
	double    dColum;
	double    dRow;
	char   szName[FILE_LEN + 1];
	int    iSub;
	double dwXPosition;
	double dwYPosition;
	double dwZPosition;
	double dwResult[MAX_LAYER][MAX_ITEM];
} t_DATA;

typedef struct _t_GET_RECIPE
{
	_t_GET_RECIPE()
	{
		nRecipeCnt = 0;
		bComplete  = FALSE;
		bError     = FALSE;
		memset(szRecipeList, 0x00, sizeof(szRecipeList));
	}
	~_t_GET_RECIPE()
	{
	}
	BOOL   bComplete;
	BOOL   bError;
	int    nRecipeCnt;
	char   szRecipeList[MAX_RECIPE_LIST][FILE_LEN + 1];
} t_GET_RECIPE;

typedef struct _t_GET_LAMP_HOUR
{
	_t_GET_LAMP_HOUR()
	{
		dLampHour = 0.0;
		bComplete = FALSE;
		bError = FALSE;
	}
	BOOL   bComplete;
	BOOL   bError;
	double dLampHour;
} t_GET_LAMP_HOUR;

typedef struct _t_MODEL_STRUCT
{
	_t_MODEL_STRUCT()
	{
		nLayerCnt = 0;
		bComplete = FALSE;
		bError = FALSE;
		memset(szLayerItem, 0x00, sizeof(szLayerItem));
	}
	BOOL   bComplete;
	BOOL   bError;
	int    nLayerCnt;
	char   szLayerItem[MAX_LAYER+1][FILE_LEN + 1];
} t_MODEL_STRUCT;

/*-----------------------------------------------------------------------
	메인 변수
-----------------------------------------------------------------------*/
typedef struct t_MODEL_INFO
{
	t_MODEL_INFO()
	{
	    bUseAlign                               = FALSE;
	    bFlat                                   = FALSE;
		dSRSafetyZ								= 0.0L;
		dAlignSafetyZ							= 0.0L;
		dSRZ									= 0.0L;
		dAlignZ									= 0.0L;
		dAlignX									= 0.0L;
		dAlignY									= 0.0L;
		nWaperSize								= WAPER_4;
		dWaperThickness							= 0.0L;
		bExcelReport                            = FALSE;
		bChartReport                            = FALSE;

		memset( dContourMin, 0x00, sizeof( dContourMin ));
		memset( dContourMax, 0x00, sizeof( dContourMax ));
		memset( dContourStep, 0x00, sizeof( dContourStep ));

		nSRData									= 0;
		nLayer                                  = 0;
		nSlotNo                                 = 0;

		memset( szInspDate, 0x00, sizeof( szInspDate ));
		memset( szLotStartTime, 0x00, sizeof(szLotStartTime) );
		memset( szInspTackTime, 0x00, sizeof( szInspTackTime ));
		memset( szInspModel, 0x00, sizeof( szInspModel ));
		memset( szInspDateTime, 0x00, sizeof( szInspDateTime ));
		memset( sz2DChartPath, 0x00, sizeof( sz2DChartPath ));
		memset( sz3DChartPath, 0x00, sizeof( sz3DChartPath ));
		memset( szResultPath, 0x00, sizeof( szResultPath ));
		memset( szAxisPath, 0x00, sizeof( szAxisPath ));
		memset(dInspStd, 0x00, sizeof(dInspStd));
		memset(dInspMax_Min, 0x00, sizeof(dInspMax_Min));
		memset(dInspUniformity, 0x00, sizeof(dInspUniformity));
		memset(dInspMax, 0x00, sizeof(dInspMax));
		memset(dInspMax, 0x00, sizeof(dInspMax));
		memset(dInspAvg, 0x00, sizeof(dInspAvg));
		nLight									= 0;

		nWaperSize								= 0;
		dWaperThickness							= 0.0;
		dEdgeExclusion							= 0.0;

		iModelType = 0;

		dPattPosX								= 0.0;
		dPattPosY								= 0.0;
		dPattSizeX								= 0.0;
		dPattSizeY								= 0.0;
		dPattGabX								= 0.0;
		dPattGabY								= 0.0;

		dAutoX									= 0.0;
		dAutoY									= 0.0;
		dAutoDistanceX							= 0.0;
		dAutoDistanceY							= 0.0;

		nAutoStepX								= 0;
		nAutoStepY								= 0;
		nAutoMethod								= 0;
		nOutputDisp                             = 0;
	}

	BOOL     bUseAlign;
	BOOL     bFlat;
	BOOL     bExcelReport;
	BOOL     bChartReport;

	//----------------
	// 측정 검사정보
	//----------------
	double	dAlignSafetyZ;
	double	dSRSafetyZ;
	double	dAlignZ;
	double	dSRZ;
	double	dAlignX;
	double	dAlignY;
	double	dSRSpeed;

	int     nLight;
	int     nWaperSize;
	double  dWaperThickness;
	double  dEdgeExclusion;

	int		iModelType;//0:일반 1:패턴 2:타겟 3:서브

	double	 dPattPosX;
	double	 dPattPosY;
	double	 dPattSizeX;
	double	 dPattSizeY;
	double	 dPattGabX;
	double	 dPattGabY;


	double   dContourMin[MAX_LAYER+1][MAX_ITEM+1];
	double   dContourMax[MAX_LAYER + 1][MAX_ITEM + 1];
	double   dContourStep[MAX_LAYER + 1][MAX_ITEM + 1];

	int      nSRData;
	int      nLayer;
	int      nSlotNo;
	t_DATA   stSRData[MAX_DATA + 1];

	char     szInspTackTime[FILE_LEN+1];
	char     szLotStartTime[FILE_LEN + 1];
	char     szInspModel[FILE_LEN+1];
	char     szInspDateTime[FILE_LEN+1];
	char     szInspDate[FILE_LEN+1];
	char     sz2DChartPath[FILE_LEN+1];
	char     sz3DChartPath[FILE_LEN+1];
	char     szResultPath[FILE_LEN+1];
	char     szAxisPath[FILE_LEN+1];
	double   dInspMin[MAX_LAYER + 1][MAX_ITEM + 1];
	double   dInspMax[MAX_LAYER + 1][MAX_ITEM + 1];
	double   dInspAvg[MAX_LAYER + 1][MAX_ITEM + 1];
	double   dInspStd[MAX_LAYER + 1][MAX_ITEM + 1];
	double   dInspMax_Min[MAX_LAYER + 1][MAX_ITEM + 1];
	double   dInspUniformity[MAX_LAYER + 1][MAX_ITEM + 1];

	double   dAutoX;
	double   dAutoY;
	double   dAutoDistanceX;
	double   dAutoDistanceY;
	int      nAutoStepX;
	int      nAutoStepY;
	int      nAutoMethod;
	int      nOutputDisp;

	t_USER_INFO stUser;

} t_MODEL_INFO;

typedef struct _t_LAYER_INFO
{
	_t_LAYER_INFO()
	{
		dThickness	= 0.0;
		dN_550		= 0.0;
		dK_550		= 0.0;
		dN_632		= 0.0;
		dK_632		= 0.0;
	}
	double dThickness;
	double dN_550;
	double dK_550;
	double dN_632;
	double dK_632;
	char szThickness[FILE_LEN + 1];
	char szN_550[FILE_LEN+1];
	char szK_550[FILE_LEN+1];
	char szN_632[FILE_LEN+1];
	char szK_632[FILE_LEN+1];
} t_LAYER_INFO;

typedef struct _t_ITEM_INFO
{
	_t_ITEM_INFO()
	{
		memset( dItem, 0x00, sizeof(dItem));
	}
	double dItem[MAX_DATA+1][MAX_LAYER+1][MAX_ITEM+1];
} t_ITEM_INFO;

typedef struct _t_ELIPSO_RESULT_INFO
{
	_t_ELIPSO_RESULT_INFO()
	{
		bComplete		= FALSE;
		bError			= FALSE;
		nLayerCnt		= 0;
		dMSE			= 0.0;
		dTotalThickness = 0.0;
		dRoughness		= 0.0;
		memset( szErrorMsg, 0x00, sizeof(szErrorMsg));
		memset(szTotalThickness, 0x00, sizeof(szTotalThickness));
		memset(szRoughness, 0x00, sizeof(szRoughness));
		memset(szMSE, 0x00, sizeof(szMSE));
	}
	BOOL bComplete;
	BOOL bError;
	char szErrorMsg[FILE_LEN * 4];
	int  nLayerCnt;

	double dTotalThickness;
	double dRoughness;
	double dMSE;
	char szMSE[FILE_LEN+1];
	char szTotalThickness[FILE_LEN + 1];
	char szRoughness[FILE_LEN+1];
	t_LAYER_INFO stLayer[MAX_LAYER+1];
} t_ELIPSO_RESULT_INFO;

typedef struct _t_RESULT_INFO
{
	_t_RESULT_INFO()
	{
		memset( m_dDataX, 0x00, sizeof( m_dDataX ));
		memset( m_dDataY, 0x00, sizeof( m_dDataY ));
		memset( m_dDataZ, 0x00, sizeof( m_dDataZ ));
		memset( m_dDataL, 0x00, sizeof( m_dDataL ));
		memset( m_dMax, 0x00, sizeof( m_dMax ));
		memset( m_dMin, 0x00, sizeof( m_dMin ));
		memset( m_dAvg, 0x00, sizeof( m_dAvg ));
		memset( m_dStd, 0x00, sizeof( m_dStd ));
		memset( m_dMax_Min, 0x00, sizeof( m_dMax_Min ));
		memset( m_dUniformity, 0x00, sizeof( m_dUniformity ));
		memset( m_fTot, 0x00, sizeof( m_fTot ));
		m_nDataCnt = 0;
		m_nCurrCnt = 0;
		memset( m_dContourMin, 0x00, sizeof( m_dContourMin ));
		memset( m_dContourMax, 0x00, sizeof( m_dContourMax ));
		memset( m_dContourStep, 0x00, sizeof( m_dContourStep ));
		memset(m_iZeroCnt, 0x00, sizeof(m_iZeroCnt));
	}

	double                              m_dMax[MAX_LAYER + 1][MAX_ITEM];
	double                              m_dMin[MAX_LAYER + 1][MAX_ITEM];
	double                              m_dAvg[MAX_LAYER + 1][MAX_ITEM];
	double                              m_dStd[MAX_LAYER + 1][MAX_ITEM];
	double                              m_dMax_Min[MAX_LAYER + 1][MAX_ITEM];
	double                              m_dUniformity[MAX_LAYER + 1][MAX_ITEM];
	float                               m_fTot[MAX_LAYER + 1][MAX_ITEM];

	double								m_dDataX[MAX_DATA+1];	// X축 좌표
	double								m_dDataY[MAX_DATA+1];   // Y축 좌표
	double								m_dDataZ[MAX_DATA+1][MAX_LAYER+1][MAX_ITEM+1];	// 결과값 R,S,V,C
	double								m_dDataH[MAX_DATA+1];	// Z측 좌표
	double								m_dDataL[MAX_DATA+1];	// 순번
	int                                 m_nDataCnt;
	int                                 m_nCurrCnt;

	double								m_dContourMin[MAX_LAYER + 1][MAX_ITEM];
	double								m_dContourMax[MAX_LAYER + 1][MAX_ITEM];
	double								m_dContourStep[MAX_LAYER + 1][MAX_ITEM];

	int									m_iZeroCnt[MAX_LAYER + 1][MAX_ITEM];

} t_RESULT_INFO;

/*-----------------------------------------------------------------------
	DEL 구조체
-----------------------------------------------------------------------*/
typedef struct _t_DEL_INFO
{
	_t_DEL_INFO()
	{
		nDays						= 0;
		memset( szPATH, 0x00, sizeof( szPATH ) );
		memset( szExt, 0x00, sizeof( szExt ) );
	}
	
	BOOL bUSE;

	char szPATH[ 2048 ];												// 폴더 경로
	int  nDays;															// 보존기간
	char szExt[ 256 ];													// 확장자

} t_DEL_INFO;

/*-----------------------------------------------------------------------
	DEL 구조체
-----------------------------------------------------------------------*/
typedef struct _t_LAST_MODEL
{
	_t_LAST_MODEL()
	{
		memset( szLastModel, 0x00, sizeof( szLastModel ) );
	}
	
	BOOL bUSE;

	char szLastModel[ FILE_LEN+1 ];

} t_LAST_MODEL;

struct SHEET_RESISTANCE_Format   
{
	t_EQP_INFO							stEqp;
	t_IO_INFO							stIO;
	t_ELIPSO_IO_INFO                    stElipsoIO;
	t_COMMON_INFO						stCommon;
	t_LAST_MODEL                        stLastModel; 

	t_MODEL_INFO                        stModel;
	t_MODEL_INFO                        stCurrModel;
	t_MODEL_INFO                        stResult;

	t_MODEL_INFO                        stTarget;
	t_MODEL_INFO                        stSub;
 
	t_RESULT_INFO						stData;
	t_RESULT_INFO						stData2D;
	t_RESULT_INFO						stData3D;

	t_RESULT_INFO						stReviewData;
	t_MODEL_INFO                        stReview;

	t_LOGIN								stLogin;
	t_LOGIN								stUsrList[MAX_USER];
};	

#endif // !defined(AFX_STRUCT_H__71165AE8_0FC5_4063_AB20_7D3871E2D3BB__INCLUDED_)
