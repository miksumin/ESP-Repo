
/*
OpenTherm core
Version: 1.0
Author: Mikhail Sumin
Date: March 30, 2021
*/

#include <OpenTherm.h>			// https://github.com/ihormelnyk/opentherm_library

OpenTherm ot(OT_INPIN, OT_OUTPIN);

char ot_status[32];				// OpenTherm status
bool dhw_present = false;		// DHW present flag
//
float boiler_temp = 0.0;		// Boiler temperature
float water_temp = 0.0;			// Hot water temperature
//
byte boiler_setpoint = 60;		// Boiler setpoint
byte boiler_sp_min = 40;		// Boiler setpoint min
byte boiler_sp_max = 80;		// Boiler setpoint max
//
byte water_setpoint = 50;		// Hot water setpoint
byte water_sp_min = 35;			// Hot water setpoint min
byte water_sp_max = 70;			// Hot water setpoint max
//
bool boiler_mode = false;		// Central Heating mode
bool water_mode = false;		// Hot Water mode (if present)
//
uint16_t fault_code = 0;		// Fault code (in case of fault status)
int8_t fault_status = -1;		// Fault status
int8_t boiler_status = -1;		// Central Heating status
int8_t water_status = -1;		// Hot Water status	(if presenr)
int8_t flame_status = -1;		// Flame Status
byte ot_modLevel = 0;			// Modulation Level

bool set_boiler_temp = false;
bool set_water_temp = false;
bool request_settings = false;

int8_t requestCounter = -1;
unsigned long ot_lastUpdated = 0;

//
void ot_command(String cmd, byte value) {
	//
	if (cmd == "boiler_sp") {
		if (value < boiler_sp_min)
			value = boiler_sp_min;
		if (value > boiler_sp_max)
			value = boiler_sp_max;
		//
		boiler_setpoint = value;
		DebugPrintln("OT new CH setpoint: "+String(boiler_setpoint));
		set_boiler_temp = true;		// force Tset
	}
	if (cmd == "water_sp") {
		if (value < water_sp_min)
			value = water_sp_min;
		if (value > water_sp_max)
			value = water_sp_max;
		//
		water_setpoint = value;
		DebugPrintln("OT new DHW setpoint: "+String(water_setpoint));
		set_water_temp = true;		// force TdhwSet
	}
}

//
void ICACHE_RAM_ATTR handleInterrupt() {
    ot.handleInterrupt();
}

//
void handleStatus(unsigned long response) {
	//
	bool bValue;
	String strConfig;
	if ((String(ot_status) == "Request timeout") || (String(ot_status) == "N/A")) {
		DebugPrintln("*** Boiler status: ***");
		strConfig = " ";
	}
	//
	bValue = ot.isFault(response);
	if (fault_status != bValue) {
		fault_status = bValue;
		DebugPrintln(strConfig+"Boiler Fault status: "+String(fault_status? "ON":"OFF"));
	}
	bValue = ot.isCentralHeatingActive(response);
	if (boiler_status != bValue) {
		boiler_status = bValue;
		DebugPrintln(strConfig+"Boiler CH status: "+String(boiler_status? "ON":"OFF"));
	}
	bValue = ot.isHotWaterActive(response);
	if (water_status != bValue) {
		water_status = bValue;
		DebugPrintln(strConfig+"Boiler DHW status: "+String(water_status? "ON":"OFF"));
	}
	bValue = ot.isFlameOn(response);
	if (flame_status != bValue) {
		flame_status = bValue;
		DebugPrintln(strConfig+"Boiler Flame status: "+String(flame_status? "ON":"OFF"));
	}
}

//
void handleConfig(byte boiler_config) {
	//
	dhw_present = boiler_config & 0x01;
	byte control_type = boiler_config & 0x02;
	byte cooling_config = boiler_config & 0x04;
	byte dhw_config = boiler_config & 0x08;
	byte pump_control = boiler_config & 0x10;
	byte CH2_present = boiler_config & 0x20;
	//
	DebugPrintln("*** Boiler config: ***");
	DebugPrintln(" DHW present: "+String(dhw_present? "":"not")+" present");
	DebugPrintln(" Control type: "+String(control_type? "ON/OFF":"Modulating"));
	DebugPrintln(" Cooling config: "+String(cooling_config? "":"not")+" supported");
	DebugPrintln(" DHW config: "+String(dhw_config? "storage tank":"instantaneous or not-specified"));
	DebugPrintln(" Pump control: "+String(pump_control? "":"not")+" allowed");
	DebugPrintln(" CH2 present: "+String(dhw_present? "":"not")+" present");
}

//
void handleResponse(unsigned long response) {
	//
	byte nValue;
	float fValue;
	byte boiler_config;
	//
	byte DataID = ot.getDataID(response);
	switch (DataID) {
		case OpenThermMessageID::Status:
			handleStatus(response);
			break;
		case OpenThermMessageID::RelModLevel:
			nValue = (response >> 8) & 0xFF;
			if (ot_modLevel != nValue) {
				ot_modLevel = nValue;
				DebugPrintln("Modulation level: "+String(ot_modLevel)+"%");
			}
			break;
		case OpenThermMessageID::SConfigSMemberIDcode:
			boiler_config = (response >> 8) & 0xFF;
			handleConfig(boiler_config);
			break;
		case OpenThermMessageID::ASFflags:
			fault_code = response & 0xFFFF;
			break;
		case OpenThermMessageID::Tboiler:
			fValue = ot.getFloat(response);
			if (boiler_temp != fValue) {
				boiler_temp = fValue;
				DebugPrintln("Boiler CH temperature: "+String(boiler_temp,1)+"°C");
			}
			break;
		case OpenThermMessageID::Tdhw:
			fValue = ot.getFloat(response);
			if (water_temp != fValue) {
				water_temp = fValue;
				DebugPrintln("Boiler DHW temperature: "+String(water_temp,1)+"°C");
			}
			break;
		case OpenThermMessageID::TSet:
			boiler_setpoint = ot.getFloat(response);
			break;
		case OpenThermMessageID::TdhwSet:
			water_setpoint = ot.getFloat(response);
			break;
		case OpenThermMessageID::MaxTSetUBMaxTSetLB:
			boiler_sp_min = response & 0xFF;
			boiler_sp_max = (response >> 8) & 0xFF;
			break;
		case OpenThermMessageID::TdhwSetUBTdhwSetLB:
			water_sp_min = response & 0xFF;
			water_sp_max = (response >> 8) & 0xFF;
			break;
		default: break;
	}
}

//
void responseCallback(unsigned long response, OpenThermResponseStatus responseStatus) {
	//
	static char last_status[32];
	static byte ot_try_count = 0;
	//
	if (responseStatus == OpenThermResponseStatus::NONE)
		strcpy(last_status, "OpenTherm is not initialized");
	else if (responseStatus != OpenThermResponseStatus::TIMEOUT) {
		// we got SUCCESS or INVALID response status
		//byte respType = ot.getMessageType(response);	// get response msgType
		if (ot.getMessageType(response) != OpenThermMessageType::UNKNOWN_DATA_ID) {
			if (ot.isValidResponse(response)) {
				// SUCCESS response
				handleResponse(response);				// handle response
				//
				if (fault_status == 1) {
					if (requestCounter == 1)
						sprintf(last_status, "Fault code: 0x%04X", fault_code);
				}
				else {
					strcpy(last_status, "OnLine");
				}
				//
				ot_try_count = 0;
			}
			else
				// INVALID response status
				sprintf(last_status, "Response invalid: 0x%08X", response);
		}
		else
			sprintf(last_status, "Unknown DataID for request #%d", requestCounter);
	}
	else {
		// TIMEOUT response status
		if (ot_try_count < 3) {
			requestCounter--;	// let's repeat last request for 3 times
			ot_try_count++;
		}
		else {
			// status changed to Timeout
			stpcpy(last_status, "Request timeout");
			requestCounter = -1;
			fault_status = -1;
			boiler_status = -1;
			water_status = -1;
			flame_status = -1;
		}
	}
	//
	if ((String(ot_status) != String(last_status)) && (String(last_status) != "")) {
		if ((String(ot_status) == "Request timeout") && (String(last_status) == "OnLine")) {
			// status changed from Timeout to OnLine
			ot_command("boiler_sp", boiler_setpoint);	// force write Tset
			ot_command("water_sp", water_setpoint);		// force write TdhwSet
			request_settings = true;					// request boiler settings
		}
		//
		strcpy(ot_status, last_status);
		DebugPrintln("OpenTherm status: "+String(ot_status));
	}
}

//
void ot_init() {
	ot.begin(handleInterrupt, responseCallback);
	DebugPrintln("OpenTherm initialized");
	strcpy(ot_status, "N/A");
}

//
void ot_loop() {
	//
	ot.process();	// OpenTherm process response status
	//
	if (ot.isReady()) {		// check if OpenTherm is ready
		//
		// wait 1 sec between survey cycles
		if ((requestCounter < 0) && (millis() - ot_lastUpdated < 1000)) {
			return;
		}
		//
		unsigned long request = 0;
		bool result = 0;
		//
		requestCounter++;
		//
		switch (requestCounter) {
			case 0:		// Status request
				request = ot.buildSetBoilerStatusRequest(boiler_mode, water_mode, false, false, false);
				result = ot.sendRequestAync(request);
				break;
			case 1:			// Get Boiler Fault Flags
				if (fault_status == 1) {
					request = ot.buildRequest(OpenThermRequestType::READ_DATA, OpenThermMessageID::ASFflags, 0);
					result = ot.sendRequestAync(request);
				}
				break;
			case 2:		// Set Boiler Temp request
				if (set_boiler_temp) {
					request = ot.buildSetBoilerTemperatureRequest((float)boiler_setpoint);
					result = ot.sendRequestAync(request);
					set_boiler_temp = false;
				}
				break;
			case 3:		// Get Boiler Temp request
				request = ot.buildRequest(OpenThermMessageType::READ_DATA, OpenThermMessageID::Tboiler, 0);
				result = ot.sendRequestAync(request);
				break;
			case 4:		// Set DHW Temp request
				if ((dhw_present) && (set_water_temp)) {
					request = ot.buildSetDHWTemperatureRequest((float)water_setpoint);
					result = ot.sendRequestAync(request);
					set_water_temp = false;
				}
				break;
			case 5:		// Get DHW Temp request
				if (dhw_present) {
					request = ot.buildRequest(OpenThermMessageType::READ_DATA, OpenThermMessageID::Tdhw, 0);
					result = ot.sendRequestAync(request);
				}
				break;
			case 6:		// Get Relative Modulation Level
				if (flame_status == 1) {
					request = ot.buildRequest(OpenThermRequestType::READ_DATA, OpenThermMessageID::RelModLevel, 0);
					result = ot.sendRequestAync(request);
				}
				break;
			case 7:		// Get Slave Config & MemberID code
				if (request_settings) {
					request = ot.buildRequest(OpenThermRequestType::READ_DATA, OpenThermMessageID::SConfigSMemberIDcode, 0);
					result = ot.sendRequestAync(request);
				}
				break;
			case 8:		// Max CH water setpoint bounds
				if (request_settings) {
					request = ot.buildRequest(OpenThermRequestType::READ_DATA, OpenThermMessageID::MaxTSetUBMaxTSetLB, 0);
					result = ot.sendRequestAync(request);
				}
				break;
			case 9:		// Max DHW setpoint bounds
				if (request_settings) {
					request = ot.buildRequest(OpenThermRequestType::READ_DATA, OpenThermMessageID::TdhwSetUBTdhwSetLB, 0);
					result = ot.sendRequestAync(request);
				}
				break;
			default:
				requestCounter = -1;
				request_settings = false;
				ot_lastUpdated = millis();		// cycle finished time
				break;
		}
		//
		if ((request > 0) && (!result)) {
			DebugPrintln("OpenTherm error send request #"+String(requestCounter));
		}
		//requestCounter++;
	}
	
}

