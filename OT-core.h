
/*
Opentherm core for ESP8266/ESP32 OpenTherm WiFi Controller
By: Mikhail Sumin
Version: 2.0
Date: April 08, 2021
*/

#define OT_MSG_MAX 58

enum OTdataType_t { ot_f88, ot_s16, ot_s8s8, ot_u16, ot_u8u8, ot_flag8, ot_flag8flag8, ot_special, ot_flag8u8, ot_u8, ot_undef}; 
enum OTmsgType_t { OT_UNDEF, OT_READ, OT_WRITE, OT_RW };

struct OTlookup_t {
	byte msgID;
	OTmsgType_t msgType;
	OTdataType_t dataType;
	byte  interval;		// request interval in sec, 255 - request on demand
	//char* label;
	//char* friendlyName;
	//char* unit;
};

OTlookup_t OTmap[] = {
	// msgID, msgType, dataType	interval,	, label, friendlyName, unit
	{   0, OT_READ  , ot_flag8flag8, 0	}, 	//"Status", "Master and Slave status", "" },
	{   1, OT_WRITE , ot_f88,        255}, 	//"TSet", "Control setpoint", "°C" },
	{   2, OT_WRITE , ot_flag8u8,    255}, 	//"MConfigMMemberIDcode", "Master Config / Member ID", "" },
	{   3, OT_READ  , ot_flag8u8,    255}, 	//"SConfigSMemberIDcode", "Slave Config / Member ID", "" },
	{   4, OT_RW    , ot_u8u8,       255}, 	//"Command", "Command-Code", "" },
	{   5, OT_READ  , ot_flag8u8,    255}, 	//"ASFflags", "Application-specific fault", "" },
	{   6, OT_READ  , ot_flag8u8,    255}, 	//"RBPflags", "Remote-parameter flags ", "" },
	{   7, OT_WRITE , ot_f88,        255}, 	//"CoolingControl", "Cooling control signal", "%" },
	{   8, OT_WRITE , ot_f88,        255}, 	//"TsetCH2", "Control setpoint for 2e CH circuit", "°C" },
	{   9, OT_READ  , ot_f88,        255}, 	//"TrOverride", "Remote override room setpoint", "" },
	{  10, OT_READ  , ot_u8u8,       255}, 	//"TSP", "Number of Transparent-Slave-Parameters supported by slave", "" },
	{  11, OT_RW    , ot_u8u8,       255}, 	//"TSPindexTSPvalue", "Index number / Value of referred-to transparent slave parameter", "" },
	{  12, OT_READ  , ot_u8u8,       255}, 	//"FHBsize", "Size of Fault-History-Buffer supported by slave", "" },
	{  13, OT_READ  , ot_u8u8,       255}, 	//"FHBindexFHBvalue", "Index number / Value of referred-to fault-history buffer entry", "" },
	{  14, OT_WRITE , ot_f88,        255}, 	//"MaxRelModLevelSetting", "Maximum relative modulation level setting", "%" },
	{  15, OT_READ  , ot_u8u8,       255}, 	//"MaxCapacityMinModLevell", "Maximum boiler capacity (kW) / Minimum boiler modulation level(%)", "kW/%" },
	{  16, OT_WRITE , ot_f88,        255}, 	//"TrSet", "Room Setpoint", "°C" },
	{  17, OT_READ  , ot_f88,        255}, 	//"RelModLevel", "Relative Modulation Level", "%" },
	{  18, OT_READ  , ot_f88,        0	}, 	//"CHPressure", "Water pressure in CH circuit", "bar" },
	{  19, OT_READ  , ot_f88,        0	}, 	//"DHWFlowRate", "Water flow rate in DHW circuit", "l/m" },
	{  20, OT_RW    , ot_special,    60	}, 	//"DayTime", "Day of Week and Time of Day", "" },
	{  21, OT_RW    , ot_u8u8,       60	}, 	//"Date", "Calendar date ", "" },
	{  22, OT_RW    , ot_u16,        60	}, 	//"Year", "Calendar year", "" },
	{  23, OT_WRITE , ot_f88,        255}, 	//"TrSetCH2", "Room Setpoint for 2nd CH circuit", "°C" },
	{  24, OT_WRITE , ot_f88,        0	}, 	//"Tr", "Room Temperature", "°C" },
	{  25, OT_READ  , ot_f88,        0	}, 	//"Tboiler", "Boiler flow water temperature", "°C" },
	{  26, OT_READ  , ot_f88,        0	}, 	//"Tdhw", "DHW temperature", "°C" },
	{  27, OT_READ  , ot_f88,        0	}, 	//"Toutside", "Outside temperature", "°C" },
	{  28, OT_READ  , ot_f88,        0	}, 	//"Tret", "Return water temperature", "°C" },
	{  29, OT_READ  , ot_f88,        0	}, 	//"Tstorage", "Solar storage temperature", "°C" },
	{  30, OT_READ  , ot_f88,        0	}, 	//"Tcollector", "Solar collector temperature", "°C" },
	{  31, OT_READ  , ot_f88,        255}, 	//"TflowCH2", "Flow water temperature CH2 circuit", "°C" },
	{  32, OT_READ  , ot_s16,        0	}, 	//"Tdhw2", "Domestic hot water temperature 2", "°C" },
	{  33, OT_READ  , ot_f88,        0	}, 	//"Texhaust", "Boiler exhaust temperature", "°C" },
	//
	{  35, OT_READ  , ot_u16,		 0	}, 	//"FanSpeed", "Fan speed", "rpm" },
	{  36, OT_READ  , ot_f88, 		 0	}, 	//"ElectricalCurrentBurnerFlame", "Electrical current through burner flame", "µA" },
	{  37, OT_READ  , ot_f88, 		 0	}, 	//"TRoomCH2", "Room temperature for 2nd CH circuit", "°C" },
	{  38, OT_READ  , ot_u8u8, 		 0	}, 	//"RelativeHumidity", "Relative Humidity", "%" },
	//
	{  48, OT_READ  , ot_s8s8,       255}, 	//"TdhwSetUBTdhwSetLB", "DHW setpoint upper & lower bounds for adjustment", "°C" },
	{  49, OT_READ  , ot_s8s8,       255}, 	//"MaxTSetUBMaxTSetLB", "Max CH water setpoint upper & lower bounds for adjustment", "°C" },
	{  50, OT_READ  , ot_s8s8,       255}, 	//"HcratioUBHcratioLB", "OTC heat curve ratio upper & lower bounds for adjustment", "" },
	//
	{  56, OT_RW    , ot_f88,        255}, 	//"TdhwSet", "DHW setpoint", "°C" },	
	{  57, OT_RW    , ot_f88,        255}, 	//"MaxTSet", "MaxCH water setpoint", "°C" },
	{  58, OT_RW    , ot_f88,        255}, 	//"Hcratio", "OTC heat curve ratio", "°C" },
	//
	{ 100, OT_READ  , ot_flag8,      0	}, 	//"RoomRemoteOverrideFunction", "Function of manual and program changes in master and remote room setpoint.", "" },
	//
	{ 115, OT_READ  , ot_u16,        255}, 	//"OEMDiagnosticCode", "OEM-specific diagnostic/service code", "" },
	{ 116, OT_RW    , ot_u16,        60	}, 	//"BurnerStarts", "Nr of starts burner", "" },
	{ 117, OT_RW    , ot_u16,        60	}, 	//"CHPumpStarts", "Nr of starts CH pump", "" },
	{ 118, OT_RW    , ot_u16,        60	}, 	//"DHWPumpValveStarts", "Nr of starts DHW pump/valve", "" },
	{ 119, OT_RW    , ot_u16,        60	}, 	//"DHWBurnerStarts", "Nr of starts burner during DHW mode", "" },
	{ 120, OT_RW    , ot_u16,        60	}, 	//"BurnerOperationHours", "Nr of hours that burner is in operation (i.e. flame on)", "" },
	{ 121, OT_RW    , ot_u16,        60	}, 	//"CHPumpOperationHours", "Nr of hours that CH pump has been running", "" },
	{ 122, OT_RW    , ot_u16,        60	}, 	//"DHWPumpValveOperationHours", "Nr of hours that DHW pump has been running or DHW valve has been opened ", "" },
	{ 123, OT_RW    , ot_u16,        60	}, 	//"DHWBurnerOperationHours", "Nr of hours that burner is in operation during DHW mode", "" },
	{ 124, OT_READ  , ot_f88,        255}, 	//"OpenThermVersionMaster", "Master Version OpenTherm Protocol Specification", "" },
	{ 125, OT_READ  , ot_f88,        255}, 	//"OpenThermVersionSlave", "Slave Version OpenTherm Protocol Specification", "" },
	{ 126, OT_READ  , ot_u8u8,       255}, 	//"MasterVersion", "Master product version number and type", "" },
	{ 127, OT_READ  , ot_u8u8,       255}, 	//"SlaveVersion", "Slave product version number and type", "" },
};

struct OpenthermData {
  byte respType = 0;
  uint16_t data;
  byte set = 0;
  uint32_t updated;
};

// create OTdata object
OpenthermData OTdata[OT_MSG_MAX];

