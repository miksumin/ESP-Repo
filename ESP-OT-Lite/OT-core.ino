
/*
ESP8266/ESP32 OpenTherm WiFi Controller
By: Mikhail Sumin
Date: March 30, 2021
*/

OpenTherm ot(OT_INPIN, OT_OUTPIN);

char ot_status[32];				// OpenTherm status
bool dhw_present = false;
bool cooling_present = false;
float boiler_temp = 0.0;		// Boiler temperature
float water_temp = 0.0;			// Hot water temperature
byte boiler_setpoint = 60;		// Boiler setpoint
byte boiler_sp_min = 40;		// Boiler setpoint min
byte boiler_sp_max = 80;		// Boiler setpoint max
byte water_setpoint = 50;		// Hot water setpoint
byte water_sp_min = 35;			// Hot water setpoint min
byte water_sp_max = 70;			// Hot water setpoint max
byte ot_modLevel = 0;			// Modulation Level
bool boiler_mode = false;		// Central Heating mode
bool water_mode = false;		// Hot Water mode (if available)
bool cooling_mode = false;		// Cooling mode (if available)
uint16_t fault_code = 0;		// Fault code (in case of fault status)
int8_t fault_status = -1;		// Fault status
int8_t boiler_status = -1;		// Central Heating status
int8_t water_status = -1;		// Hot Water status	(if available)
int8_t cooling_status = -1;		// Cooling status	(if available)
int8_t flame_status = -1;		// Flame Status

int8_t requestCounter = 0;
bool set_boiler_temp = false;
bool set_water_temp = false;
bool request_settings = false;

void ICACHE_RAM_ATTR handleInterrupt() {
    ot.handleInterrupt();
}

void handleResponse(unsigned long response) {
	//
	bool bValue;
	byte nValue;
	float temperature;
	//
	byte DataID = ot.getDataID(response);
	switch (DataID) {
		case OpenThermMessageID::Status:
			bValue = ot.isFault(response);
			if (fault_status != bValue) {
				fault_status = bValue;
				DebugPrintln("Fault status: "+String(fault_status? "ON":"OFF"));
			}
			bValue = ot.isCentralHeatingActive(response);
			if (boiler_status != bValue) {
				boiler_status = bValue;
				DebugPrintln("Central heating status: "+String(boiler_status? "ON":"OFF"));
			}
			bValue = ot.isHotWaterActive(response);
			if (water_status != bValue) {
				water_status = bValue;
				DebugPrintln("Hot water status: "+String(water_status? "ON":"OFF"));
			}
			bValue = ot.isCoolingActive(response);
			if (cooling_status != bValue) {
				cooling_status = bValue;
				DebugPrintln("Cooling status: "+String(cooling_status? "ON":"OFF"));
			}
			bValue = ot.isFlameOn(response);
			if (flame_status != bValue) {
				flame_status = bValue;
				DebugPrintln("Flame status: "+String(flame_status? "ON":"OFF"));
			}
			break;
		case OpenThermMessageID::RelModLevel:
			nValue = (response >> 8) & 0xFF;
			if (ot_modLevel != nValue) {
				ot_modLevel = nValue;
			}
			break;
		case OpenThermMessageID::SConfigSMemberIDcode:
			dhw_present = (response >> 8) & 0x01;
			cooling_present = (response >> 8) & 0x04;
			break;
		case OpenThermMessageID::ASFflags:
			fault_code = response & 0xFFFF;
			break;
		case OpenThermMessageID::Tboiler:
			temperature = ot.getTemperature(response);
			if (boiler_temp != temperature) {
				boiler_temp = temperature;
				DebugPrintln("Central heating temperature: "+String(boiler_temp,1)+"°C");
			}
			break;
		case OpenThermMessageID::Tdhw:
			temperature = ot.getTemperature(response);
			if (water_temp != temperature) {
				water_temp = temperature;
				DebugPrintln("Hot water temperature: "+String(water_temp,1)+"°C");
			}
			break;
		case OpenThermMessageID::TSet:
			boiler_setpoint = ot.getTemperature(response);
			break;
		case OpenThermMessageID::TdhwSet:
			water_setpoint = ot.getTemperature(response);
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

void responseCallback(unsigned long response, OpenThermResponseStatus responseStatus) {
	//
	static char last_status[32];
	static byte ot_try_count = 0;
	if (responseStatus == OpenThermResponseStatus::SUCCESS) {
		//
		handleResponse(response);
		//
		if (fault_status == 1) {
			if (requestCounter == 1)
				sprintf(last_status, "Fault code: 0x%04X", fault_code);
		}
		else {
			stpcpy(last_status, "OnLine");
			request_settings = true;
		}
		//
		ot_try_count = 0;
	}
	else {
		if (responseStatus == OpenThermResponseStatus::NONE)
			stpcpy(last_status, "OpenTherm is not initialized");
		if (responseStatus == OpenThermResponseStatus::INVALID)
			sprintf(last_status, "Response invalid: 0x%08X", response);
		if (responseStatus == OpenThermResponseStatus::TIMEOUT) {
			//
			if (ot_try_count < 3) {	// make 3 tries
				requestCounter--;	// let's repeat last request
				ot_try_count++;
			}
			else {
				stpcpy(last_status, "Request timeout");
				requestCounter = 0;
				fault_status = -1;
				boiler_status = -1;
				water_status = -1;
				cooling_status = -1;
				flame_status = -1;
			}
		}
	}
	//
	if ((String(ot_status) != String(last_status)) && (String(last_status) != "")) {
		strcpy(ot_status, last_status);
		DebugPrintln("OpenTherm status: "+String(ot_status));
	}
}

void ot_init() {
	ot.begin(handleInterrupt, responseCallback);
	DebugPrintln("OpenTherm initialized");
	strcpy(ot_status, "N/A");
}

void ot_loop() {
	
	ot.process();	// OpenTherm process response status
	
	unsigned long request;
	bool result = 0;
	
	if (ot.isReady()) {		// check if OpenTherm is ready
		//
		switch (requestCounter) {
			case 0:		// Status request
				request = ot.buildSetBoilerStatusRequest(boiler_mode, water_mode, cooling_mode, false, false);
				result = ot.sendRequestAync(request);
				break;
			case 1:			// Get Boiler Fault Flags
				if (fault_status == 1) {
					request = ot.buildRequest(OpenThermRequestType::READ, OpenThermMessageID::ASFflags, 0);
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
				request = ot.buildRequest(OpenThermMessageType::READ, OpenThermMessageID::Tboiler, 0);
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
					request = ot.buildRequest(OpenThermMessageType::READ, OpenThermMessageID::Tdhw, 0);
					result = ot.sendRequestAync(request);
				}
				break;
			case 6:		// Get Relative Modulation Level
				if (flame_status == 1) {
					request = ot.buildRequest(OpenThermRequestType::READ, OpenThermMessageID::RelModLevel, 0);
					result = ot.sendRequestAync(request);
				}
				break;
			case 7:		// Get Slave Config & MemberID code
				if (request_settings) {
					request = ot.buildRequest(OpenThermRequestType::READ, OpenThermMessageID::SConfigSMemberIDcode, 0);
					result = ot.sendRequestAync(request);
				}
				break;
			case 8:		// Max CH water setpoint bounds
				if (request_settings) {
					request = ot.buildRequest(OpenThermRequestType::READ, OpenThermMessageID::MaxTSetUBMaxTSetLB, 0);
					result = ot.sendRequestAync(request);
				}
				break;
			case 9:		// Max DHW setpoint bounds
				if (request_settings) {
					request = ot.buildRequest(OpenThermRequestType::READ, OpenThermMessageID::TdhwSetUBTdhwSetLB, 0);
					result = ot.sendRequestAync(request);
					request_settings = false;
				}
				break;
			default:
				requestCounter = -1;
				break;
		}
		//
		if (!result) {
			DebugPrintln("OpenTherm send request error");
		}
		requestCounter++;
	}
	
}

