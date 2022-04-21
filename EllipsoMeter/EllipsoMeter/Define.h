#if !defined(AFX_DEFINE_H__71165AE8_0FC5_4063_AB20_7D3871E2D3BB__INCLUDED_)
#define AFX_DEFINE_H__71165AE8_0FC5_4063_AB20_7D3871E2D3BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define AJINTEX_BOARD_IO								1
#define EQ_DEBUG										0
#define EQ_STATUS_CHECK									1
//-------------------------------------------------------
#define EQ_RUNNING_MODE									1
//-------------------------------------------------------
#if EQ_RUNNING_MODE
#define AJINTEX_BOARD									1
#define OPENCV											1
#define REAL_MEASURE									1
#define MIL_USE											1
#define JAI_CAMERA										1
#define ELLIPSO											1
#else
#define AJINTEX_BOARD									0
#define OPENCV											1
#define REAL_MEASURE									1
#define MIL_USE											0
#define JAI_CAMERA										0
#define ELLIPSO											0
#endif

#define Y_AXIS_REVERSE									1
//=================================================================================
// 프로그램 운영에 필요한 파라메터 START
//=================================================================================
#define DEFAULT_PATH									"EllipsoMeter\\"
#define DEFAULT_LOG_PATH								"LOG\\"
#define DEFAULT_BIN_PATH								"BIN\\"
#define DEFAULT_ENV_PATH								"ENV\\"
#define DEFAULT_USER_PATH								"USER\\"
#define DEFAULT_MODEL_PATH								"MODEL\\"
#define DEFAULT_RESULT_PATH								"RESULT\\"
#define DEFAULT_XML_PATH								"XML\\"
#define DEFAULT_VITLEDLIGHT_FILE						"VITLEDLIGHT.XML"
#define DEFAULT_FALCONFPPMETER_FILE						"FalconFppMeterInspector.XML"
#define LOGPATH											"%s_%04d%02d%02d_%02d.Log"	// 로그명 년 월 일 시
#define DEFAULT_MIN_LINE_CNT							"1000"
#define DEFAULT_LOG_NAME								"Undifined_Log"
#define DEFAULT_MOT_FILE								"EllipsoMeter.mot"
#define DEFAULT_COMMON_FILE								"Common.XML"
#define DEFAULT_MODEL_FILE								"MODEL.XML"	
#define DEFAULT_LAST_MODEL_FILE							"LAST_MODEL.XML"	
#define DEFAULT_EXCEL_FILENAME							"RESULT.XLSX"	
#define DEFAULT_USER_FILENAME							"USER.XML"	
#define DEFAULT_TEMP_PATH								"TEMP\\"	
#define DEFAULT_TEMP_XML								"XML.XML"	
#define DEFAULT_TEMP_2D									"2D.BMP"	
#define DEFAULT_TEMP_3D									"3D.BMP"
#define DEFAULT_TEMP_AXIS								"AXIS.BMP"	
//------------------------------------------------------------
// 타이머 설정
//------------------------------------------------------------
#define IO_READ_TIME									50
#define STAGE_MAP_TIME									50
#define MAP_TIME										100

#define WM_SET_EQUIP_INIT								WM_USER + 1000
#define WM_SET_IO										WM_USER + 2000
#define WM_SET_JOG										WM_USER + 3000
#define WM_SET_JOYSTICK_XY								WM_USER + 4000
#define WM_SET_JOYSTICK_Z								WM_USER + 5000
#define UM_START_BUTTON									WM_USER + 6000
#define UM_LOADING_BUTTON								WM_USER + 7000
#define UM_LOADING_SW_LIGHT								WM_USER + 8000
#define UM_START_SW_LIGHT								WM_USER + 9000
#define UM_STAGE_MAP_DRAW								WM_USER + 10000
#define UM_STAGE_MAP_MOVE								WM_USER + 11000
#define UM_CAMERA_CONNECT								WM_USER + 12000
#define UM_SIGNAL_TOWER									WM_USER + 13000
#define UM_BUZZER										WM_USER + 14000
#define UM_VACUUM										WM_USER + 15000
#define UM_INSP_SWITCH									WM_USER + 16000
#define UM_VAC_STATUS									WM_USER + 17000
#define UM_RUN_BTN_STATUS								WM_USER + 17100
#define UM_MAP_DRAW										WM_USER + 18000
#define UM_REPORT										WM_USER + 19000
#define WM_SET_IO_IN									WM_USER + 20000
#define WM_SET_IO_OUT									WM_USER + 21000
#define UM_EMERGENCY_CHECK								WM_USER + 22000
#define UM_CHART_VIEW									WM_USER + 23000
#define UM_REPORT_READY									WM_USER + 24000
#define UM_REPORT_REVIEW								WM_USER + 25000
#define UM_REPORT_READY_REVIEW							WM_USER + 26000

//------------------------------------------------------------
// 특수문자 정의
//------------------------------------------------------------
#define STR_STX											"<STX>"
#define STR_ETX											"<ETX>"
#define STR_YES											"Yes"
#define STR_NO											"No"
#define STR_CONFIRM										"확인"
#define STR_ON											"ON"
#define STR_OFF											"OFF"
#define STR_COMMA		   								","
#define STR_SPACE										" "	
#define STR_UNDERLINE									"_" 
#define STR_COLON										":"
#define STR_BACKSLASH									"\\"
#define STR_OK											"OK"

#define SOH												0x01
#define EOT												0x04
#define STX												0x02
#define ETX												0x03
#define CR												0x0d
#define LF												0x0a
#define DOLLAR											0x24
#define SHARP											0x23
#define CRLF											"\r\n"
#define LFCR											"\n\r"
#define CR_RETURN										"\r"
#define	PI												3.141592653589793
#define INCH											25.4
//-----------------------------------------------------------------------------------------
// USER INFO
//-----------------------------------------------------------------------------------------
#define USER_SECTION									"USER"
#define USER_ID											"USERID"
#define USER_PASSWORD									"PASSWORD"
#define USER_LEVEL										"LEVEL"
#define USER_NAME										"NAME"
#define USER_DESCRIPTION								"DESCRIPTION"
#define USER_INFO										"USER_INFO"
#define USER_TOTAL										"TOTAL"
#define USER_LIST										"USER_LIST"
#define USER_JOB										"JOB"
//------------------------------------------------------------
// FILE HANDLE
//------------------------------------------------------------
#define	RC_NORMAL										0
#define	RC_FAILED										-1
#define	RC_FILE_ACCESS_ERR								-106
#define RC_ERR_DIRECTORY								0x10000000
enum {	FM_NOTEXIST =									0, FM_DIRECTORY, FM_FILE};
#define FILE_LEN										255
//------------------------------------------------------------------
// 시리얼 포트 정의
//------------------------------------------------------------------
#define RS232C											"RS232C"
#define COMPORT											"COMPORT"
#define BAUDRATE										"BAUDRATE"
#define DATABIT											"DATABIT"
#define STOPBIT											"STOPBIT"
#define PARITYBIT										"PARITYBIT"
#define FLOWCTRL										"FLOWCTRL"
#define EVEN											"EVEN"
#define ODD												"ODD"
#define NONE__											"NONE"

// BaudRate
#define BAUDRATE_1200									"1200"
#define BAUDRATE_2400									"2400"
#define BAUDRATE_4800									"4800"
#define BAUDRATE_9600									"9600"
#define BAUDRATE_19200									"19200"
#define BAUDRATE_28800									"28800"
#define BAUDRATE_38400									"38400"
#define BAUDRATE_57600									"57600"
#define BAUDRATE_115200									"115200"
#define BAUDRATE_230400									"230400"
#define BAUDRATE_460800									"460800"
#define BAUDRATE_921600									"921600"
#define BAUDRATE_1843200								"1843200"
// BaudRate
static  char* szBaudRate[]								= { BAUDRATE_1200, 
															BAUDRATE_2400, 
															BAUDRATE_4800, 
															BAUDRATE_9600, 
															BAUDRATE_19200, 
															BAUDRATE_28800, 
															BAUDRATE_38400, 
															BAUDRATE_57600,
															BAUDRATE_115200,
															BAUDRATE_230400,
															BAUDRATE_460800,
															BAUDRATE_921600,
															BAUDRATE_1843200
};


// ParutyBit
#define PARITYBIT_NONE										"NONE"
#define PARITYBIT_ODD										"ODD"
#define PARITYBIT_EVEN										"EVEN"
static  char* szParityBit[]								= { PARITYBIT_NONE, 
	                                                        PARITYBIT_ODD, 
															PARITYBIT_EVEN };

// FlowCtrl
#define FLOWCTRL_OFF									"OFF"
#define FLOWCTRL_ON										"ON"
static  char* szFlowCtl[]								= { FLOWCTRL_OFF, 
	                                                        FLOWCTRL_ON }; 
static  char* szCH[]									= { "0","1","2" };

static  char* szCHName[]								= {    "0 - 전체채널",
														 	   "1 - 01채널",
															   "2 - 02채널"
};
//--------------------------------------------------------------------------
//         0.001    은 1 m 입니다. 10^-3
//         0.000001 은 1 u 입니다. 10^-6
//      1000.00     은 1 k 입니다. 10^3
//   1000000.0      은 1 M 입니다. 10^6
//1000000000.0      은 1 G 입니다. 10^9
//--------------------------------------------------------------------------
static  CString strDani_n[]								= { "" };
static  CString strDani_k[]								= { "" };
static  CString strDaniThickness[]						= { "㎛", "㎚", "Å" };
static  CString strDaniC[]								= { "" };
static  CString strMeasureItem[] = { "MSE", "Total-Thickness","Thickness", "N-550","K-550","N-632","K-632" };

#define CHANNEL											"CHANNEL"

static  CString strMethod[]								= { "0 : X축방향 Z",
	                                                        "1 : X축방향ㄹ",
															"2 : Y축방향 Z",
															"3 : Y축방향ㄹ" };


// XRF COMPORT INSPECT
#define COMMON			                                "COMMON"
#define LAST_MODEL			                            "LAST_MODEL"
#define ELEMENT											"ELEMENT"
#define XML_START										"<XML>\r\n"
#define XML_END											"</XML>\r\n"
#define SUBITEM											"COUNT=\"%s\""
#define ITEM											"ITEM"
#define BODY											"BODY"
#define LIGHT											"LIGHT"
#define AIM_LOGO_PATH									"AIM_LOGO_PATH"
#define AIM_QRCODE_PATH									"AIM_QRCODE_PATH"
#define EQUIP_TITLE										"EQUIP_TITLE"
#define JOG_SPEED_HIGH									"JOG_SPEED_HIGH"
#define JOG_SPEED_MIDDLE								"JOG_SPEED_MIDDLE"
#define JOG_SPEED_SLOW									"JOG_SPEED_SLOW"
#define JOG_SPEED_HIGH_Z								"JOG_SPEED_HIGH_Z"
#define JOG_SPEED_MIDDLE_Z								"JOG_SPEED_MIDDLE_Z"
#define JOG_SPEED_SLOW_Z								"JOG_SPEED_SLOW_Z"
#define LOG_SAVE										"LOG_SAVE"
#define EXCEL_SAVE										"EXCEL_SAVE"
#define X_M_LIMIT										"X_M_LIMIT"
#define X_P_LIMIT										"X_P_LIMIT"
#define Y_M_LIMIT										"Y_M_LIMIT"
#define Y_P_LIMIT										"Y_P_LIMIT"
#define CHART_2D_PATH									"CHART_2D_PATH"
#define CHART_3D_PATH									"CHART_3D_PATH"
#define AXIS_PATH										"AXIS_PATH"
#define INSP_STD										"INSP_STD"
#define INSP_UNIFORMITY									"INSP_UNIFORMITY"
#define INSP_MAX_MIN									"INSP_MAX_MIN"
#define INSP_XY_SPEED									"INSP_XY_SPEED"
#define INSP_Z_SPEED									"INSP_Z_SPEED"
#define INSP_LIGHT_VALUE								"INSP_LIGHT_VALUE"
#define RESULT_PATH										"RESULT_PATH"

#define DEL_FILE_NAME									"AUTO_DEL.XML"				// 자동파일삭제
#define AUTO_FILE_DEL									"AUTO_FILE_DEL"
#define DEL_FILE_USE									"DELETE_FILE_USE"			// 기능 사용유무
#define DEL_FILE_PATH									"DELETE_FILE_PATH"			// 삭제 대상 폴더
#define DEL_DAY											"DELETE_FILE_DAY"			// 보존기간
#define DEL_FILE_EXT									"DELETE_FILE_EXT"			// 삭제파일 확장자

// XRF LAN INSPECT
#define ETHERNET                                        "ETHERNET"
#define INSPECT											"INSPECT"
#define LAN_PORT										"PORT"
#define LAN_PORT1										"PORT1"
#define LAN_PORT2										"PORT2"
#define LAN_MODE                                        "MODE"
#define LAN_PASSIVE                                     "PASSIVE"
#define LAN_ACTIVE                                      "ACTIVE"
#define LAN_IP                                          "IP"
#define LAN_RECONNECT_TIME                              "RECONNECT_TIME"

#define MAX_BUFF_SIZE									32767
#define MAX_ITEM_DATA									10
#define MAX_LINE_CNT									100
#define FILE_LEN										255
#define MAX_BUFF_SIZE									32767
#define	BUFF_SIZE										8192
#define MAX_CHANNEL										2
#define MAX_AXIS										4
#define	MAX_PNT_NO										10000 
#define	MAX_INSP_NUM									50 
#define MAX_CNT											10
#define MAX_WAPER_KIND									4
#define MAX_ALIGN_KIND									2
#define MAX_ITEM										7
#define MAX_DATA										1000
#define MAX_RECIPE_LIST									100
#define MAX_LAYER										10

#define ELIPSOMETER_CONNECT								10000
#define ELIPSOMETER_COMM								20000
#define ELIPSOMETER_TIME								50
#define IP_PORT											1
#define ASYNC_IP_PORT									2
#define IP_PORT_ROBOT									3
#define DEFAULT_ELIPSOMETER_FILE						"ElipsoMeter.XML"
#define DEFAULT_ELIPSOMETERROBOT_FILE					"ElipsoMeter_Robot.XML"
#define RECONNECT_TIME									"RECONNECT_TIME"
#define ELIPSO_IO_IN_CNT								16
#define ELIPSO_IO_OUT_CNT								16
#define ELIPSO_IO_INPUT_1								0
#define ELIPSO_IO_OUTPUT_1								0


#define START_ACQ										"StartAcq()"
#define LIST_RECIPES									"ListRecipes()"
#define GET_ALIGN										"GetAlign()"
#define LOAD_RECIPE										"LoadRecipe(%s)"
#define RUN_RECIPE										"RunRecipe(%s/%s%s.SE)"
#define STATUS											"Status()"
#define INIT_STATUS										"InitStatus()"
#define GET_MODEL_STRUCTURE								"GetModelStructure()"
#define SYSTEM_LAMP_SHUTTER_OPEN						"SystemLampShutterOpen()"
#define SYSTEM_LAMP_SHUTTER_CLOSED						"SystemLampShutterClosed()"
#define SYSTEM_FILTER_WHEEL_AUTO_ON						"SystemFilterWheelAutoOn()"
#define SYSTEM_FILTER_WHEEL_AUTO_OFF					"SystemFilterWheelAutoOff()"
#define SYSTEM_GET_LAMP_HOURS							"SystemGetLampHours()"
#define SYSTEM_GET_RECEIVER_CAMERA_ALIGN				"SystemGetReceiverCameraAlign()"
#define SYSTEM_LAMP_ON									"SystemLampOn()"
#define SYSTEM_LAMP_OFF									"SystemLampOff()"
#define SYSTEM_LAMP_IS_ON								"SystemLampIsOn()"
#define SYSTEM_LAMP_POWER_IS_ON							"SystemLampPowerIsOn()"

#define WAITING_TO_ACQUIRE_DATA							"Waiting to Acquire Data"
#define HARDWARE_INITIALIZED							"Hardware Initialized"
#define R_MSE											"MSE = "
#define R_ROUGHNESS										"Roughness = "
#define R_THICKNESS										"Thickness # %01d = "
#define R_N_OF_550										"n of %s @ 550.0 nm = "
#define R_K_OF_550										"k of %s @ 550.0 nm = "
#define R_N_OF_632										"n of %s @ 632.8 nm = "
#define R_K_OF_632										"k of %s @ 632.8 nm = "
#define R_TOTAL_THICKNESS								"Total Thickness = "

enum CMD__NO
{
	CMD_START_ACQ = 0,
	CMD_LIST_RECIPES,
	CMD_LOAD_RECIPE,
	CMD_RUN_RECIPE,
	CMD_STATUS,
	CMD_INIT_STATUS,
	CMD_SYSTEM_LAMP_SHUTTER_OPEN,		
	CMD_SYSTEM_LAMP_SHUTTER_CLOSED,		
	CMD_SYSTEM_FILTER_WHEEL_AUTO_ON,		
	CMD_SYSTEM_FILTER_WHEEL_AUTO_OFF,	
	CMD_SYSTEM_GET_LAMP_HOURS,			
	CMD_GET_MODEL_STRUCTURE,
	CMD_SYSTEM_GET_RECEIVER_CAMERA_ALIGN,
	CMD_SYSTEM_LAMP_ON,
	CMD_SYSTEM_LAMP_OFF,
	CMD_SYSTEM_LAMP_IS_ON,
	CMD_SYSTEM_LAMP_POWER_IS_ON
};


#define	CHAR_BUFF										512

#define RC_STEP_HALT									-1
#define RC_STEP_NORMAL									0
#define RC_NORMAL_END									2
#define RC_TIMEOUT										3
#define STEP_HALT										-1

#define	LIGHT_1											1
enum WAPER__TYPE 
{
	FLAT												= 0,
	NORCH												
};

enum WAPER__SIZE
{
	WAPER_4 = 0,
	WAPER_6,
	WAPER_8,
	WAPER_12,

	MAX_WAPER_CNT
};

enum POINT_MODE
{
	PNT_CIRCLE = 0,
	PNT_RECT,
	PNT_MODEL,
	PNT_PATT,

	MAX_PNT_MODE_CNT
};

enum BUTTON_SW
{
	SW_MODEL											= 0,
	SW_AUTO,
	SW_SEARCH,
	SW_ENV,
	SW_MAINT,
	SW_EQ_INIT,
	SW_REVIEW,
	SW_APP_EXT
};

enum INSP_SW 
{
	SW_INSP_START										= 0,
	SW_INSP_END,
	SW_REINSP,
	SW_INSP_PAUSE
};
//------------------------------------------------------------
// COLOR 정의
//------------------------------------------------------------
#define		COLORDLG_0			RGB( 255,   0,   0 ) // 빨강
#define 	COLORDLG_1			RGB(   0, 255,   0 ) // 녹색
#define 	COLORDLG_2			RGB(   0,   0, 255 ) // 파랑
#define		COLORDLG_3			RGB( 255, 255,   0 ) // 노랑
#define		COLORDLG_4			RGB( 255,   0, 255 ) // 분홍
#define		COLORDLG_5			RGB(   0, 255, 255 ) // 하늘색
#define		COLORDLG_6			RGB(   0,   0,   0 ) // 검은색
#define		COLORDLG_7	 		RGB( 255, 255, 255 ) // 흰색
#define	 	COLORDLG_8			RGB( 255, 128,   0 ) // 짙은 주황
#define		COLORDLG_9			RGB( 255,   0, 128 ) // 짙은 분홍
#define		COLORDLG_10			RGB( 128, 255,   0 ) // 연두색
#define		COLORDLG_11			RGB( 128,   0, 255 ) // 보라색
#define		COLORDLG_12			RGB(   0, 255, 128 ) // 밝은 하늘색
#define		COLORDLG_13			RGB(   0, 128, 255 ) // 짙은 하늘색
#define		COLORDLG_14			RGB( 128, 128, 128 ) // 회색
#define		COLORDLG_15			RGB(  64,  64,  64 ) // 짙은 회색
#define		COLORDLG_16			RGB( 240, 240, 240 ) // 밝은 회색

//Color
#define   BLACK					RGB(0,0,0)
#define   BLUE1					RGB(0,0,128)
#define   GREEN1				RGB(0,128,0)
#define   RED1					RGB(128,0,0)

#define   CYAN					RGB(0,128,128)
#define   MAGENTA				RGB(128,0,128)
#define   BROWN					RGB(128,128,0)
#define   DARKGRAY				RGB(128,128,128)
#define   LIGHTGRAY				RGB(192,192,192)
#define   LIGHTBLUE				RGB(0,0,255)
#define   LIGHTGREEN			RGB(0,255,0)
#define   LIGHTCYAN				RGB(0,255,255)
#define   LIGHTRED				RGB(255,0,0)
#define   LIGHTMAGENTA			RGB(255,0,255)
#define   YELLOW				RGB(255,255,0)
#define   WHITE					RGB(255,255,255)

#define L_BIT_BTN_ON									14
#define L_BIT_CHAR_ON									7
#define L_BIT_BTN_OFF									16
#define L_BIT_CHAR_OFF									6
#define L_BIT_ON										0
#define L_BIT_OFF										1

#define STAGE_SIZE_X									300
#define STAGE_SIZE_Y									300

#define SLAVE_AXIS										-1
#define MASTER_AXIS										-1
#define SHARED_NAME										"ELLIPSOMETER"
#define JOG_MOVE										50
#define AUTO_TIME										50
#define JOG_ACC_DEC										200
#define MAX_SPEED										150
#define MIN_SPEED										0.001
#define AXIS_RESOLUTION									10000.
#define MIN_ALIGN_ANGLE									0.05
#define RETATE_ANGLE									1.0
#define MAX_ROTATE_ANGLE								15
#define MAX_USER										100
enum JOG_BTN {
				JOG_STOP								= 0,
				JOG_UP,
				JOG_DOWN,
				JOG_FRONT,
				JOG_BACK,
				JOG_LEFT,
				JOG_RIGHT,
				JOG_CW,
				JOG_CCW,
				JOG_UP_LEFT,
				JOG_UP_RIGHT,
				JOG_DOWN_LEFT,
				JOG_DOWN_RIGHT
};

enum SPEED_BTN {
				JOG_ZERO								= 0,
				JOG_SLOW,
				JOG_MIDDLE,
				JOG_HIGH,
};

enum AXIS	  {
				X_AXIS									= 0,		
				Y_AXIS,	
// 				R_AXIS,
				Z_AXIS									
};

enum IO_BIT	  {
				BIT_2									= 2,		
				BIT_3,	
				BIT_4
};

enum MAP_VIEW	  {
				SW_MAP									= 0,
				SW_2D,	
				SW_3D	
};

//model
enum SR_TBL {
				MEASURE_NAME = 1,
				MEASURE_COL,
				MEASURE_ROW,
				MEASURE_X,
				MEASURE_Y,
				MEASURE_MOVE,
				MEASURE_READ,
				MEASURE_DELETE
};

enum RESULT_TBL {
	            RESULT_TARGET_NO = 1,
				RESULT_COL,
				RESULT_ROW,
                RESULT_SUB,
				RESULT_X1_X2,
				RESULT_Y1_Y2,
				RESULT_MOVE,
				RESULT_MEASURE,
				RESULT_MSE,
				RESULT_TOTAL_THICKNESS,
				RESULT_LAYER_1,
				RESULT_LAYER_2,
				RESULT_LAYER_3,
				RESULT_LAYER_4,
				RESULT_LAYER_5,
				RESULT_LAYER_6,
				RESULT_LAYER_7,
				RESULT_LAYER_8,
				RESULT_LAYER_9,
				RESULT_LAYER_10
};

enum REVIEW_TBL {
				REVIEW_TARGET_NO = 1,
				REVIEW_COL,
				REVIEW_ROW,
				REVIEW_SUB,
				REVIEW_X1_X2,
				REVIEW_Y1_Y2,
				REVIEW_MSE,
				REVIEW_TOTAL_THICKNESS,
				REVIEW_LAYER_1,
				REVIEW_LAYER_2,
				REVIEW_LAYER_3,
				REVIEW_LAYER_4,
				REVIEW_LAYER_5,
				REVIEW_LAYER_6,
				REVIEW_LAYER_7,
				REVIEW_LAYER_8,
				REVIEW_LAYER_9,
				REVIEW_LAYER_10
};

enum RESULT_VAL {
				RES_MSE                                         = 0,
				RES_TOTAL_THICKNESS,
				RES_THICKNESS,
				RES_N_550,
				RES_K_550,
				RES_N_632,
				RES_K_632
};

enum EllipsoMeasureThread
{
	ELLI_THRD_RESULT = 0,
	ELLI_THRD_READY_ALIGN,

	MAX_ELLI_THRD_CNT
};


#define   IO_IN_CNT										16
#define   IO_OUT_CNT									16
enum IO_OUTPUT 
{
	MEASURE_LAMP_ON										= 0,
	LOADING_LAMP_ON,
	VACUUM_ON_OFF,
	SIGNAL_TOWER_RED,											
	SIGNAL_TOWER_YELLOW,	
	SIGNAL_TOWER_GREEN,
	BUZZER_ONOFF,
};

enum LOGIN_LEVEL 
{
	UNKNOWN_USER										= 0,
	OPERATOR,
	SYSTEM_ENGINEER,
	MAKER_ENGINEER
};

enum ELIPSO_IO_OUTPUT 
{
	VACUUM_ON_06_INCH									= 0,
	VACUUM_ON_08_INCH,
	VACUUM_ON_12_INCH,
	RESERVED3,											
	RESERVED4,
	MEASURE_READY,
	MEASURE_BUSY,
	RESERVED7,
	ELIPSOMETER_ALARM,											
	RESERVED9,
	RESERVED10,
	RESERVED11,											
	RESERVED12,											
	WAFER_0B01,											
	WAFER_0B10,
	RESERVED15											
};

#define ITEM											"ITEM"
#define RESULT											"RESULT"
#define INSPECT_RESULT									"INSPECT_RESULT"
#define INSP_MIN										"INSP_MIN"
#define INSP_MAX										"INSP_MAX"
#define INSP_AVG										"INSP_AVG"
#define INSP_DATE										"INSP_DATE"
#define INSP_MODEL										"INSP_MODEL"
#define TACT_TIME										"TACT_TIME"
#define INSP_DATE_TIME									"INSP_DATE_TIME"
#define PRODUCT											"PRODUCT"
#define EXCEL_RESULT_PATH								"EXCEL_RESULT_PATH"
#define XML_RESULT_PATH									"XML_RESULT_PATH"
#define TREND_RESULT_PATH								"TREND_RESULT_PATH"

#define MODEL											"MODEL"
#define SR_USE											"SR_USE"
#define SR_USE_Z										"SR_USE_Z"
#define SR_NAME											"SR_NAME"
#define SR_COL											"SR_COL"
#define SR_ROW											"SR_ROW"
#define SR_COL_DOUBLE									"SR_COL_DOUBLE"
#define SR_ROW_DOUBLE									"SR_ROW_DOUBLE"
#define SR_X											"SR_X"
#define SR_SUB											"SR_SUB"
#define SR_Y											"SR_Y"
#define SR_Z											"SR_Z"
#define ALIGN_Z											"ALIGN_Z"
#define SR_Z											"SR_Z"
#define ALIGN_Z											"ALIGN_Z"
#define SR_V											"SR_V_%01d_%01d"
#define SR_LIST											"SR_LIST"
#define SR_DATA											"SR_DATA"
#define SR_Z_SAFETY_AXIS								"SR_Z_SAFETY_AXIS"
#define ALIGN_Z_SAFETY_AXIS							    "ALIGN_Z_SAFETY_AXIS"
#define SR_SPEED										"SR_SPEED"
#define ALIGN_USE										"ALIGN_USE"
#define UP_LIGHT										"UP_LIGHT"
#define WAPER_TYPE										"WAPER_TYPE"
#define WAPER_SIZE										"WAPER_SIZE"
#define ALIGN_X											"ALIGN_X"
#define ALIGN_Y											"ALIGN_Y"
#define LOADING_X										"LOADING_X"
#define LOADING_Y										"LOADING_Y"
#define LOADING_X2										"LOADING_X2"
#define LOADING_Y2										"LOADING_Y2"
#define CONTOUR_MIN										"CONTOUR_MIN"
#define CONTOUR_MAX										"CONTOUR_MAX"
#define CONTOUR_STEP									"CONTOUR_STEP"
#define OUTPUT_DISP										"OUTPUT_DISP"
#define WAPER_THICKNESS									"WAPER_THICKNESS"
#define EDGE_EXCLUSION									"EDGE_EXCLUSION"
#define CHART_INCLUDE									"CHART_INCLUDE"
#define PATTERN_BASE_POSITION_X							"PATTERN_BASE_POSITION_X"
#define PATTERN_BASE_POSITION_Y							"PATTERN_BASE_POSITION_Y"
#define PATTERN_SIZE_X									"PATTERN_SIZE_X"
#define PATTERN_SIZE_Y									"PATTERN_SIZE_Y"
#define PATTERN_GAB_X									"PATTERN_GAB_X"
#define PATTERN_GAB_Y									"PATTERN_GAB_Y"
#define Z_LIMIT											"Z_LIMIT"
#define Z_ALIGN											"Z_ALIGN"
#define Z_MEASURE										"Z_MEASURE"
#define Z_ALIGN_SAFETY									"Z_ALIGN_SAFETY"
#define Z_MEASURE_SAFETY								"Z_MEASURE_SAFETY"
#define ALIGN_4INCH_NORCH_X								"ALIGN_4INCH_NORCH_X"
#define ALIGN_4INCH_NORCH_Y								"ALIGN_4INCH_NORCH_Y"
#define ALIGN_6INCH_NORCH_X								"ALIGN_6INCH_NORCH_X"
#define ALIGN_6INCH_NORCH_Y								"ALIGN_6INCH_NORCH_Y"
#define ALIGN_8INCH_NORCH_X								"ALIGN_8INCH_NORCH_X"
#define ALIGN_8INCH_NORCH_Y								"ALIGN_8INCH_NORCH_Y"
#define ALIGN_12INCH_NORCH_X							"ALIGN_12INCH_NORCH_X"
#define ALIGN_12INCH_NORCH_Y							"ALIGN_12INCH_NORCH_Y"
#define ALIGN_4INCH_FLAT_X								"ALIGN_4INCH_FLAT_X"
#define ALIGN_4INCH_FLAT_Y								"ALIGN_4INCH_FLAT_Y"
#define ALIGN_6INCH_FLAT_X								"ALIGN_6INCH_FLAT_X"
#define ALIGN_6INCH_FLAT_Y								"ALIGN_6INCH_FLAT_Y"
#define ALIGN_8INCH_FLAT_X								"ALIGN_8INCH_FLAT_X"
#define ALIGN_8INCH_FLAT_Y								"ALIGN_8INCH_FLAT_Y"
#define ALIGN_12INCH_FLAT_X								"ALIGN_12INCH_FLAT_X"
#define ALIGN_12INCH_FLAT_Y								"ALIGN_12INCH_FLAT_Y"

#define LOTID											"LOTID"
#define SAMPLE_SIZE										"SAMPLE_SIZE"
#define THICKNESS										"THICKNESS"
#define EXCLUSION										"EXCLUSION"
#define RECIPE											"RECIPE"
#define INSERTMODE										"INSERTMODE"
#define WAFERALIGNMODE									"WAFERALIGNMODE"
#define VISIONAFMODE									"VISIONAFMODE"
#define USER_DATE										"USER_DATE"
#define SLOT_NO											"SLOT_NO"
#define OPID											"OPID"
#define TIMESTART										"TIMESTART"
#define TIMEEND											"TIMEEND"
#define TIMESPEND										"TIMESPEND"
#define DATA_FILE										"DATA_FILE"
#define DESCRIPTION										"DESCRIPTION"
#define MEASUREAREATYPE									"MEASUREAREATYPE"

#define MAT_RESISTIVITY									"MAT_RESISTIVITY"
#define PROBE_SPACE										"PROBE_SPACE"
#define TMEASURE										"TMEASURE"
#define MMODE											"MMODE"
#define ANALYSIS										"ANALYSIS"
#define USER_FLAT										"USER_FLAT"
#define METERIAL										"METERIAL"
#define CORRECTIONF										"CORRECTIONF"
#define TCOEFFICENT										"TCOEFFICENT"
#define TREFERENCE										"TREFERENCE"
#define SIGMA_MAX										"SIGMA_MAX"
#define THIN_METERIAL									"THIN_METERIAL"
#define THIN_THICKNESS									"THIN_THICKNESS"
#define SIGMA_MIN										"SIGMA_MIN"
#define USER_TIME										"USER_TIME"
#define USER_INPUT										"USER_INPUT"
#define LAYER_CNT										"LAYER_CNT"
#define EXCEL_LAYER										"Layer (%01d)"
#define SR_TOTAL_CNT									"SR_TOTAL_CNT"
#define LAYER_CNT										"LAYER_CNT"
#define DATA_CNT										"DATA_CNT"
#define LOT_START_TIME									"LOT_START_TIME"

#define LIGHT_VALUE										"LIGHT_VALUE" 
#define AUTO_START_X									"AUTO_START_X"
#define AUTO_START_Y									"AUTO_START_Y"
#define AUTO_DISTANCE_X									"AUTO_DISTANCE_X"
#define AUTO_DISTANCE_Y									"AUTO_DISTANCE_Y"
#define AUTO_STEP_X										"AUTO_STEP_X" 
#define AUTO_STEP_Y										"AUTO_STEP_Y" 
#define AUTO_METHOD										"AUTO_METHOD" 
#define EXCEL_REPORT									"EXCEL_REPORT" 
#define ALARM_ON										"ALARM_ON" 

// 장비 운영 STEP
#define INSP_DATA_CLEAR									100
#define INSP_SAFETY_Z_MOVE								200
#define INSP_SAFETY_Z_END								300
#define INSP_ALIGN_MOVE									400
#define INSP_ALIGN_MOVE_END								500
#define INSP_ALIGN_START								600
#define INSP_ALIGN_START_END							700
#define INSP_ALIGN_REPEAT_START							800
#define INSP_ALIGN_REPEAT_END							900
#define INSP_END_CHECK									1000
#define INSP_Z_SAFETY_MOVE								1100
#define INSP_Z_SAFETY_END								1200
#define INSP_XY_MOVE									1300
#define INSP_XY_END										1400	
#define INSP_Z_MOVE										1500
#define INSP_Z_END										1600
#define INSP_MEASURE_START_WAIT							1700
#define INSP_MEASURE_START								1800
#define INSP_MEASURE_END								1900
#define INSP_MEASURE_SET								2000
#define INSP_END										2100
#define INSP_INIT_POS									2200	
#define INSP_INIT_POS_END								2300
#define INSP_STOP_POS									2400
#define INSP_STOP_POS_END								2500
#define INSP_INIT_POS_XY								2600	
#define INSP_INIT_POS_XY_END							2700
#define INSP_STOP_POS_XY								2800
#define INSP_STOP_POS_XY_END							2900
#define INSP_ROTATION_R_START							3000
#define INSP_ROTATION_R_END								3100
#define INSP_ALIGN_REPEAT_WAIT							3200
#define INSP_MEASURE_WAIT								3300
#define INSP_REPORT_CHECK								3400


//Align_MIL_Vision
#define		CHAR_BUFF					512

#define		DEF_MAX_GROUP				16			//그룹개수
#define		DEF_MAX_MARK_NUM			4			//패턴의 등록 개수
#define		DEF_MAX_PATT_NO				10			//찾는 패턴의 개수

typedef enum TIMER_
{
	TIMER_SET_PATT_MASTER = 0,
	TIMER_SET_PATT_DETAIL,
	TIMER_SET_MOTOR_ALIGN,
	TIMER_GET_PATT_CENTER,
	TIMER_GET_TRANS_COORDINATE
}_TIMER;

typedef enum MODEL_TPYE_
{
	MODEL_TYPE_GENERAL = 0,
	MODEL_TYPE_PATTERN,
	MODEL_TYPE_TARGET,
	MODEL_TYPE_SUB

}_MODEL_TYPE;



#endif // !defined(AFX_DEFINE_H__71165AE8_0FC5_4063_AB20_7D3871E2D3BB__INCLUDED_)
