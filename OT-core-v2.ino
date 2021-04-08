
/*
Opentherm core for ESP8266/ESP32 OpenTherm WiFi Controller
By: Mikhail Sumin
Version: 2.0
Date: April 08, 2021
*/

#include "OT-core.h"

OpenTherm ot(OT_INPIN, OT_OUTPIN);

bool OT_DEBUG = false;

char ot_status[32];				// OpenTherm status
bool ot_power = false;			// Boiler power switch

bool dhw_present = false;		// DHW present flag

byte boiler_type;
byte boiler_version;

float boiler_temp = 0.0;		// Boiler temperature
float water_temp = 0.0;			// Hot water temperature
float dhw_flow_rate = 0.0;		// DHW flow rate
uint16_t fan_speed = 0;			// Fan speed
byte ot_modLevel = 0;			// Modulation Level

bool boiler_mode = false;		// Central Heating mode
bool water_mode = false;		// Hot Water mode (if available)
bool cooling_mode = false;		// Cooling mode (if available)
bool OTC_enable = false;
bool CH2_enable = false;

uint16_t fault_code = 0;		// Fault code (in case of fault status)
int8_t fault_status = -1;		// Fault status
int8_t boiler_status = -1;		// Central Heating status
int8_t water_status = -1;		// Hot Water status	(if available)
int8_t cooling_status = -1;		// Cooling status	(if available)
int8_t flame_status = -1;		// Flame Status

byte boiler_setpoint = 50;		// Boiler setpoint
byte boiler_sp_min = 40;		// Boiler setpoint min
byte boiler_sp_max = 80;		// Boiler setpoint max
byte water_setpoint = 60;		// Hot water setpoint
byte water_sp_min = 35;			// Hot water setpoint min
byte water_sp_max = 70;			// Hot water setpoint max

//
//bool set_boiler_temp = false;
//bool set_water_temp = false;

int8_t requestCounter = -1;

byte reqCount = 0;
byte reqNumbers[86];

String strStatus(int8_t status) {
	switch (status) {
		case -1: return "N/A";
		case 0: return "OFF";
		case 1: return "ON";
		default: return "";
	}
}

void handleCommand(String cmd, byte value) {
	//
	if (cmd == "boiler_sp") {
		if ((value >= boiler_sp_min) && (value <= boiler_sp_max)) {
			boiler_setpoint = value;
			DebugPrintln("OT new CH setpoint: "+String(boiler_setpoint));
			//set_boiler_temp = true;
			OTdata[1].set = 1;		// force Tset
			OTdata[1].data = ot.temperatureToData(boiler_setpoint);
		}
		else
			DebugPrintln("OT set new CH setpoint error");
	}
	if (cmd == "water_sp") {
		if ((value >= water_sp_min) && (value <= water_sp_max)) {
			water_setpoint = value;
			DebugPrintln("OT new DHW setpoint: "+String(water_setpoint));
			//set_water_temp = true;
			OTdata[41].set = 1;		// force TdhwSet
			OTdata[41].data = ot.temperatureToData(water_setpoint);
		}
		else
			DebugPrintln("OT set new DHW setpoint error");
	}
}

void ICACHE_RAM_ATTR handleInterrupt() {
    ot.handleInterrupt();
}

void handleStatus(unsigned long response) {
	bool bValue = ot.isFault(response);
	if (fault_status != bValue) {
		fault_status = bValue;
		DebugPrintln("Fault status: "+String(fault_status? "ON":"OFF"));
	}
	bValue = ot.isCentralHeatingActive(response);
	if (boiler_status != bValue) {
		boiler_status = bValue;
		DebugPrintln("Boiler status: "+String(boiler_status? "ON":"OFF"));
	}
	bValue = ot.isHotWaterActive(response);
	if (water_status != bValue) {
		water_status = bValue;
		DebugPrintln("Hot water status: "+String(water_status? "ON":"OFF"));
	}
	bValue = ot.isFlameOn(response);
	if (flame_status != bValue) {
		flame_status = bValue;
		DebugPrintln("Flame status: "+String(flame_status? "ON":"OFF"));
	}
}

void handleConfig(byte boiler_config) {
	//
	dhw_present = boiler_config & 0x01;
	byte control_type = boiler_config & 0x02;
	byte cooling_config = boiler_config & 0x04;
	byte dhw_config = boiler_config & 0x08;
	byte pump_control = boiler_config & 0x10;
	byte CH2_present = boiler_config & 0x20;
	//
	DebugPrintln("Boiler config:");
	DebugPrintln("DHW present: "+String(dhw_present? "":"not")+" present");
	DebugPrintln("Control type: "+String(control_type? "ON/OFF":"Modulating"));
	DebugPrintln("Cooling config: "+String(cooling_config? "":"not")+" supported");
	DebugPrintln("DHW config: "+String(dhw_config? "storage tank":"instantaneous or not-specified"));
	DebugPrintln("Pump control: "+String(pump_control? "":"not")+" allowed");
	DebugPrintln("CH2 present: "+String(dhw_present? "":"not")+" present");
}

void handleResponse(unsigned long response) {
	//
	bool bValue;
	byte nValue;
	float fValue;
	uint16_t iValue;
	uint16_t boiler_config;
	//
	byte DataID = ot.getDataID(response);
	switch (DataID) {
		case OpenThermMessageID::Status:		// handle Status response
			handleStatus(response);
			break;
		case OpenThermMessageID::RelModLevel:	// handle RelModLevel response
			nValue = (response >> 8) & 0xFF;
			if (ot_modLevel != nValue) {
				ot_modLevel = nValue;
			}
			break;
		case OpenThermMessageID::SConfigSMemberIDcode:	// handle SConfigSMemberIDcode response
			boiler_config = (response >> 8) & 0xFF;
			handleConfig(boiler_config);
			break;
		case OpenThermMessageID::ASFflags:		// handle ASFflags response
			fault_code = response & 0xFFFF;
			break;
		case OpenThermMessageID::Tboiler:		// handle Tboiler response
			fValue = ot.getTemperature(response);
			if (boiler_temp != fValue) {
				boiler_temp = fValue;
				DebugPrintln("Boiler temperature: "+String(boiler_temp,1)+"°C");
			}
			break;
		case OpenThermMessageID::Tdhw:			// handle Tdhw response
			fValue = ot.getTemperature(response);
			if (water_temp != fValue) {
				water_temp = fValue;
				DebugPrintln("Hot water temperature: "+String(water_temp,1)+"°C");
			}
			break;
		case OpenThermMessageID::DHWFlowRate:		// handle DHWFlowRate response
			fValue = ot.getFloat(response);
			if (dhw_flow_rate != fValue) {
				dhw_flow_rate = fValue;
				DebugPrintln("DHW flow rate: "+String(dhw_flow_rate,1)+"l/m");
			}
			break;
		case OpenThermMessageID::FanSpeed:			// handle FanSpeed response
			iValue = response & 0xFFFF;
			if (fan_speed != iValue) {
				fan_speed = iValue;
				DebugPrintln("Fan speed: "+String(fan_speed)+"rpm");
			}
			break;
		case OpenThermMessageID::TSet:							// CH temperature setpoint response
			boiler_setpoint = ot.getTemperature(response);
			break;
		case OpenThermMessageID::TdhwSet:						// DHW temperature setpoint response
			water_setpoint = ot.getTemperature(response);
			break;
		case OpenThermMessageID::MaxTSetUBMaxTSetLB:			// Max CH setpoint upper & lower bounds
			boiler_sp_min = response & 0xFF;
			boiler_sp_max = (response >> 8) & 0xFF;
			DebugPrintln("CH setpoint bounds: "+String(boiler_sp_min)+"..."+String(boiler_sp_max));
			break;
		case OpenThermMessageID::TdhwSetUBTdhwSetLB:			// DHW setpoint upper & lower bounds
			water_sp_min = response & 0xFF;
			water_sp_max = (response >> 8) & 0xFF;
			DebugPrintln("DHW setpoint bounds: "+String(water_sp_min)+"..."+String(water_sp_max));
			break;
		case OpenThermMessageID::SlaveVersion:			// Slave product version number and type
			boiler_version = response & 0xFF;
			boiler_type = (response >> 8) & 0xFF;
			DebugPrintln("Boiler type & version: "+String(boiler_type)+":"+String(boiler_version));
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
			stpcpy(last_status, "OpenTherm is not initialized");
	else if (responseStatus != OpenThermResponseStatus::TIMEOUT) {
		// SUCCESS or INVALID response status
		byte respType = ot.getMessageType(response);	// get response msgType
		OTdata[requestCounter].respType = respType;		// remember last response msgType
		if (respType != 7) {
			if (OTdata[requestCounter].data != response & 0xFFFF) {
				OTdata[requestCounter].data = response & 0xFFFF;
				if (ot.isValidResponse(response))
					handleResponse(response);				// handle response
				else
					sprintf(last_status, "Invalid response: 0x%08X", response);
			}
		}
		//
		if (fault_status == 1) {
			OTdata[5].respType = 0;		// force read ASFflags
			if (OTmap[requestCounter].msgID == 5)
				sprintf(last_status, "Fault code: 0x%04X", fault_code);
		}
		else {
			stpcpy(last_status, "OnLine");
		}
		//
		if (flame_status == 1) {
			OTdata[17].respType = 0;	// force read RelModLevel
		}
		//
		ot_try_count = 0;
	}
	else {		// TIMEOUT
		//
		if (ot_try_count == 3) {	// make 3 tries
			stpcpy(last_status, "Request timeout");
			fault_status = -1;
			boiler_status = -1;
			water_status = -1;
			cooling_status = -1;
			flame_status = -1;
		}
		else {
			ot_try_count++;
		}
		requestCounter = -1;
		reqCount = 0;
	}
	//
	if ((String(ot_status) != String(last_status)) && (String(last_status) != "")) {
		strcpy(ot_status, last_status);
		DebugPrintln("OpenTherm status: "+String(ot_status));
	}
}

void ot_debug(bool value) {
	OT_DEBUG = value;
}

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
		unsigned long request = 0;
		bool result = 0;
		requestCounter++;
		//
		if (requestCounter < OT_MSG_MAX) {
			uint16_t msgData = 0;
			byte msgID = OTmap[requestCounter].msgID;
			byte msgType = OTmap[requestCounter].msgType;
			byte interval = OTmap[requestCounter].interval;
			OpenThermMessageType OTmsgType = READ_DATA;
			//
			if (OTdata[requestCounter].respType != 7) {
				//
				if (boiler_mode) {
					OTdata[1].set = 1;		// force write Tset
					OTdata[1].data = ot.temperatureToData(boiler_setpoint);
				}
				if (water_mode) {
					OTdata[41].set = 1;		// force write TdhwSet
					OTdata[41].data = ot.temperatureToData(water_setpoint);
				}
				// OT_WRITE or OT_RW
				if ((msgType == OT_WRITE) || (msgType == OT_RW)) {
					if (OTdata[requestCounter].set) {
						OTmsgType = WRITE_DATA;
						msgData = OTdata[requestCounter].data;
					}
				}
				// OT_READ or OT_RW
				if (((msgType == OT_READ) || (msgType == OT_RW)) && (OTmsgType == READ_DATA)) {
					if (msgID == 0) {
						msgData = (boiler_mode) | (water_mode << 1) | (cooling_mode << 2) | (OTC_enable << 3) | (CH2_enable << 4);
						msgData <<= 8;
					}
				}
				// build & send request
				if (((OTdata[requestCounter].respType == 0) && (OTmsgType == READ_DATA)) \
					|| ((interval < 255) && (millis() - OTdata[requestCounter].updated >= interval)) \
					|| ((OTdata[requestCounter].set) && (OTmsgType == WRITE_DATA))) {
					//
					request = ot.buildRequest(OTmsgType, (OpenThermMessageID)msgID, msgData);
					result = ot.sendRequestAync(request);
					//
					if (OT_DEBUG) {
						reqNumbers[reqCount] = msgID;
						reqCount++;
					}
					//
					if (OTdata[requestCounter].set)
						OTdata[requestCounter].set = 0;
				}
			}
			//
			if ((request > 0) && (!result)) {
				DebugPrintln("OpenTherm send request error");
			}
		}
		else {
			requestCounter = -1;
			if (OT_DEBUG) {
				String result = "OT requests ["+String(reqCount)+"]: ";
				for (byte i=0; i<reqCount; i++)
					result += String(reqNumbers[i])+";";
				DebugPrintln(result);
				reqCount = 0;
			}
		}
	}
}

