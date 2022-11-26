
/*
ESP8266/ESP32 OpenTherm WiFi Controller
Author: Mikhail Sumin
Updated: November 25, 2022
*/

// ESP-OT-Lite version
#define BUILD_NAME	"ESP-OT-Lite"
#define BUILD_VER	"1.1"
#define BUILD_DATE	"20221125"

// Define your WiFi credentials
#define DEFAULT_SSID	"Your_WiFi_SSID"
#define DEFAULT_PASS	"Your_WiFi_PASS"

// OpenTherm Adapter connection
#define OT_INPIN	5	// D1 WemosD1/NodeMCU
#define OT_OUTPIN	4	// D2 WemosD1/NodeMCU

// Buildin LED for your board
#define LED_BUILTIN	2	// WemosD1/NodeMCU

#include <Arduino.h>
#if defined(ESP8266)
	#include <ESP8266WebServer.h>
	#include <ESP8266HTTPUpdateServer.h>
	ESP8266WebServer httpServer(80);
	ESP8266HTTPUpdateServer httpUpdater;
#elif defined(ESP32)
	#include <WebServer.h>
	#include <HTTPUpdateServer.h>
	WebServer httpServer(80);
	HTTPUpdateServer httpUpdater;
#endif

char HOSTNAME[16];

#include "Debug.h"

#include "Ethernet.h"

#include "OT-core.h"

#include "WebPages.h"

int8_t WIFI_MODE = WIFI_STA;
bool wifi_connected = false;

// WiFi variables
byte wifi_connect_timeout = 15;
unsigned long wifi_lastCheck;

// WiFi AP mode settings
IPAddress ap_local_ip(192,168,4,1);
IPAddress ap_gateway(192,168,4,1);
IPAddress ap_netmask(255,255,255,0);

bool ESP_restart = false;

void statusLed(bool state) {
#ifdef LED_BUILTIN
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, !state);
#endif
}

void statusLed_flash() {
	// flash status LED
	statusLed(true);
	delay(10);
	statusLed(false);
}

String strStatus(int8_t status) {
	switch (status) {
		case -1: return "N/A";
		case 0: return "OFF";
		case 1: return "ON";
		default: return "";
	}
}

String strUptime() {
	char uptime[15];
	long uptime_sec = millis()/1000;
	byte ss = uptime_sec % 60;
	byte mm = ( uptime_sec / 60 ) % 60;
	byte hh = ( uptime_sec / 3600 ) % 24;
	int days = uptime_sec/60/1440;
	if (days > 0) {
		sprintf(uptime, "%dd %02d:%02d:%02d\0", days, hh, mm, ss);
	}
	else
		sprintf(uptime, "%02d:%02d:%02d\0", hh, mm, ss);
	return String(uptime);
}

//
String WiFi_getHostname() {
#if defined(ESP8266)
	return String(WiFi.hostname());
#elif defined(ESP32)
	return String(WiFi.getHostname());
#endif
}

void WiFi_setHostname() {
	#if defined(ESP8266)
		WiFi.hostname(HOSTNAME);
	#elif defined(ESP32)
		WiFi.setHostname(HOSTNAME);
	#endif
}

// WiFi connection
bool WiFi_connect(const char* ssid = NULL, const char* pass = NULL) {
	//
	WiFi.mode(WIFI_STA);
	WiFi_setHostname();
	//
	if (ssid != NULL) {
		WiFi.disconnect();
		DebugPrintln("WiFi using SSID: "+String(ssid));
		WiFi.begin(ssid,pass);
	}
	else {
		//DebugPrintln("WiFi trying to connect...");
		WiFi.begin();
	}
	//
	WiFi.waitForConnectResult();
	//
	if (WiFi.status() == WL_CONNECTED) {
		DebugPrintln("WiFi connected: "+String(WiFi.SSID()));
		DebugPrintln("WiFi hostname: "+String(WiFi_getHostname()));
		DebugPrintln("WiFi MAC address: "+String(WiFi.macAddress()));
		DebugPrintln("WiFi local IP: "+String(WiFi.localIP().toString()));
		DebugPrintln("WiFi gateway: "+String(WiFi.gatewayIP().toString()));
		return true;
	}
	else {
		DebugPrint("WiFi waiting for connect...");
		unsigned long time_point = millis();
		while ((WiFi.status() != WL_CONNECTED) && (millis()-time_point < wifi_connect_timeout*1000)) {
			DebugPrint(".");
			statusLed(true);
			delay(50);
			statusLed(false);
			delay(450);
		}
		DebugPrintln();
		DebugPrintln("WiFi connection failed!");
		if ((String(DEFAULT_SSID) != "") && (ssid != DEFAULT_SSID))
			if (WiFi_connect(DEFAULT_SSID,DEFAULT_PASS))
				return true;
		return false;
	}
}

// Root page response
void handleRoot() {
	//
	int8_t refresh = -1;
	//
	if ((httpServer.args() > 0 )) {
		//
		#ifdef OT_POWER_PIN
			if (httpServer.hasArg("ot_power")) {
				ot_power = (httpServer.arg("ot_power") == "ON")? true:false;
				// Boiler power control
				pinMode(OT_POWER_PIN, OUTPUT);
				digitalWrite(OT_POWER_PIN, !value);
			}
		#endif
		//
		if (httpServer.hasArg("boiler_mode")) {
			boiler_mode = (httpServer.arg("boiler_mode") == "ON")? true:false;
		}
		if (httpServer.hasArg("water_mode")) {
			water_mode = (httpServer.arg("water_mode") == "ON")? true:false;
		}
		//
		if (httpServer.hasArg("boiler_sp")) {
			ot_command("boiler_sp", httpServer.arg("boiler_sp").toInt());
		}
		if (httpServer.hasArg("water_sp")) {
			ot_command("water_sp", httpServer.arg("water_sp").toInt());
		}
		refresh = 0;
	}

		//
		String strPage = rootPage;
		if (strPage.indexOf(String("{rftime}")) > 0)
			strPage.replace(String("{rftime}"), String(refresh));
		//
		while (strPage.indexOf(String("{bsp}")) > 0)
			strPage.replace(String("{bsp}"), String(boiler_setpoint));
		if (strPage.indexOf(String("{bsp_min}")) > 0)
			strPage.replace(String("{bsp_min}"), String(boiler_sp_min));
		if (strPage.indexOf(String("{bsp_max}")) > 0)
			strPage.replace(String("{bsp_max}"), String(boiler_sp_max));
		//
		while (strPage.indexOf(String("{wsp}")) > 0)
			strPage.replace(String("{wsp}"), String(water_setpoint));
		if (strPage.indexOf(String("{wsp_min}")) > 0)
			strPage.replace(String("{wsp_min}"), String(water_sp_min));
		if (strPage.indexOf(String("{wsp_max}")) > 0)
			strPage.replace(String("{wsp_max}"), String(water_sp_max));
		//
		httpServer.send(200, "text/html", strPage);
		statusLed_flash();

}

// Status response
String handleStatus() {
	//
	String strJSON = "{";
	//
	strJSON += "\"build_name\":\"" + String(BUILD_NAME) + "\",";
	strJSON += "\"build_ver\":\"" + String(BUILD_VER) + "\",";
	strJSON += "\"build_date\":\"" + String(BUILD_DATE) + "\",";
	//
	strJSON += "\"ot_inpin\":" + String(OT_INPIN) + ",";
	strJSON += "\"ot_outpin\":" + String(OT_OUTPIN) + ",";
	//
	#ifdef OT_POWER_PIN
		strJSON += "\"ot_power\":\"" + String(ot_power? "ON":"OFF") + "\",";
	#endif
	strJSON += "\"ot_status\":\"" + String(ot_status) + "\",";
	//
	strJSON += "\"flame_status\":\"" + String(strStatus(flame_status)) + "\",";
	strJSON += "\"ot_modLevel\":" + String(ot_modLevel) + ",";
	//
	strJSON += "\"boiler_temp\":\"" + String(boiler_temp,1) + "\",";
	strJSON += "\"boiler_setpoint\":" + String(boiler_setpoint) + ",";
	strJSON += "\"boiler_sp_min\":" + String(boiler_sp_min) + ",";
	strJSON += "\"boiler_sp_max\":" + String(boiler_sp_max) + ",";
	//
	strJSON += "\"water_temp\":\"" + String(water_temp,1) + "\",";
	strJSON += "\"water_setpoint\":" + String(water_setpoint) + ",";
	strJSON += "\"water_sp_min\":" + String(water_sp_min) + ",";
	strJSON += "\"water_sp_max\":" + String(water_sp_max) + ",";
	//
	strJSON += "\"boiler_status\":\"" + String(strStatus(boiler_status)) + "\",";
	strJSON += "\"boiler_mode\":\"" + String(boiler_mode? "ON":"OFF") + "\",";
	//
	strJSON += "\"water_status\":\"" + String(strStatus(water_status)) + "\",";
	strJSON += "\"water_mode\":\"" + String(water_mode? "ON":"OFF") + "\",";
	//
	strJSON += "\"uptime\":\"" + String(strUptime()) + "\"";
	//
	strJSON += "}";
	
	return strJSON;
	
	// send JSON response
	//httpServer.send(200, "text/json", strJSON);
	
}

// Web server config
void http_Config() {
	//httpServer.on("/",[](){ httpServer.send(200, "text/html", rootPage); });
	httpServer.on("/", handleRoot);
	//httpServer.on("/status", handleStatus);
	httpServer.on("/status",[](){
		httpServer.send(200, "text/json", handleStatus());
		statusLed_flash();
	});
	httpServer.on("/restart",[](){ httpServer.send(200, "text/html", "Restarting..."); ESP_restart = true; });
	httpServer.onNotFound([](){
		String message = "[HTTP] Page Not Found\r\n";
			message += "URI: "+httpServer.uri()+"\r\n";
			message += "Method: "+String((httpServer.method() == HTTP_GET) ? "GET" : "POST")+"\r\n";
			message += "Arguments: "+String(httpServer.args())+"\r\n";

		for (uint8_t i = 0; i < httpServer.args(); i++) {
			message += " " + httpServer.argName(i) + ": " + httpServer.arg(i) + "\r\n";
		}
		//
		httpServer.send(404, "text/plain", message);
		DebugPrint(message);
	});
}

//
void setup() {
	
	DebugBegin(115200);
	delay(100);
	
	DebugPrintln("Starting...");
	DebugPrintln(String(BUILD_NAME)+" ver: "+String(BUILD_VER)+"("+String(BUILD_DATE)+")");
	
	sprintf(HOSTNAME, BUILD_NAME);
	
	// Starting Ethernet
	if (eth_enabled) {
		if (Eth_Begin()) {
			WIFI_MODE = WIFI_AP;
		}
		else {
			DebugPrintln(F("Ethernet not connected"));
			WIFI_MODE = WIFI_STA;
		}
	}
	
	// Starting WiFi network
	if (WIFI_MODE == WIFI_STA) {
		// starting WiFi in STA mode
		if (WiFi_connect()) {
			//
			wifi_connected = true;
		}
		else {
			//
			wifi_connected = false;
			WIFI_MODE = WIFI_AP;
		}
	}
	//
	if (WIFI_MODE == WIFI_AP) {
		// starting WiFi in AP mode
		DebugPrint("WiFi starting in AP mode... ");
		WiFi.mode(WIFI_AP);
		WiFi.softAPConfig(ap_local_ip, ap_gateway, ap_netmask);
		statusLed(true);
		bool res = WiFi.softAP(HOSTNAME, "12345678", 1, 0, 1);
		DebugPrintln(res? "Ready" : "Failed!");		
		DebugPrintln("WiFi AP IP address: "+WiFi.softAPIP().toString());
	}
	
	// HTTP server & HTTP updater config and start
	http_Config();
	httpUpdater.setup(&httpServer);
	httpServer.begin();
	DebugPrintln("HTTP server configured and started");
	
	// HTTP updater info
	if (eth_connected)
		DebugPrintln("HTTP updates on http://"+ETH.localIP().toString()+"/update");
	//
	if (WIFI_MODE == WIFI_STA)
		DebugPrintln("HTTP updates on http://"+WiFi.localIP().toString()+"/update");
	if (WIFI_MODE == WIFI_AP)
		DebugPrintln("HTTP updates on http://"+WiFi.softAPIP().toString()+"/update");
	
	// OpenTherm init
	ot_init();
	
	// Finishing setup
	DebugPrintln("Setup finished, starting main loop...");
	
}

//
void loop() {
	
	//
	if (WIFI_MODE == WIFI_STA) {
		// check WiFi status
		if (WiFi.status() != WL_CONNECTED) {
			if (millis() - wifi_lastCheck > wifi_connect_timeout*1000) {
				// WiFi try to reconnect
				WiFi_connect();
				wifi_lastCheck = millis();
			}
		}
	}
		
	// OpenTherm loop
	ot_loop();
	
	// HTTP server handle client requests
	httpServer.handleClient();
	delay(1);
	
	// check if ESP Restart is needed
	if (ESP_restart) {
		DebugPrintln("Restarting...");
		delay(1000);
		ESP.restart();
	}

}

