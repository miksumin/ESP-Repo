
/*
ESP8266/ESP32 OpenTherm WiFi Controller
By: Mikhail Sumin
Date: March 30, 2021
*/

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

#include <OpenTherm.h>			// https://github.com/ihormelnyk/opentherm_library

#include "ESP-OT.h"

// WiFi variables
char HOSTNAME[16];
byte wifi_connect_timeout = 15;
unsigned long wifi_lastCheck;

// WiFi AP mode settings
IPAddress ap_local_ip(192,168,4,1);
IPAddress ap_gateway(192,168,4,1);
IPAddress ap_netmask(255,255,255,0);

bool ESP_restart = false;

String getChipType() {
#if defined(ESP8266)
	return "ESP8266";
#elif defined(ESP32)
	return "ESP32";
#endif
}

uint32_t getChipID() {
	char ChipID[6];
#if defined(ESP8266)
	uint32_t intChipID = ESP.getChipId();
#elif defined(ESP32)
	uint32_t intChipID = ESP.getEfuseMac() & 0xFFFFFF;
#endif
	return intChipID;
}

void statusLed(bool state) {
#ifdef LED_BUILTIN
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, !state);
#endif
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
	sprintf(HOSTNAME, "ESP-%06X", getChipID());
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
		if (String(DEFAULT_SSID) != "")
			if (WiFi_connect(DEFAULT_SSID,DEFAULT_PASS))
				return true;
		return false;
	}
}

// Web server config
void Web_config() {
	httpServer.on("/",[](){ httpServer.send(200, "text/html", "ESP OpenTherm WiFi controller"); });
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
	DebugPrintln("ESP-OT-Lite version "+String(VERSION));
	
	// starting WiFi connection
	if (WiFi_connect()) {
	
		// HTTP server config and start
		Web_config();
		httpServer.begin();
		DebugPrintln("HTTP server configured and started");
		
		// HTTP updater start
		httpUpdater.setup(&httpServer);
		DebugPrintln("HTTP updates ready on http://"+WiFi.localIP().toString()+"/update");
		
		// OpenTherm init
		ot_init();
		
		DebugPrintln("Setup finished, starting main loop...");
	}
	else {
		// starting WiFi in AP mode
		DebugPrint("WiFi starting in AP mode... ");
		WiFi.mode(WIFI_AP);
		WiFi.softAPConfig(ap_local_ip, ap_gateway, ap_netmask);
		statusLed(true);
		bool res = WiFi.softAP(HOSTNAME, "12345678", 1, 0, 1);
		DebugPrintln(res? "Ready" : "Failed!");		
		DebugPrintln("WiFi AP IP address: "+WiFi.softAPIP().toString());
		//
		Web_config();
		httpServer.begin();
		DebugPrintln("HTTP server configured and started");
		DebugPrintln("Waiting for client connection...");
		//
		while (1) {
			httpServer.handleClient();
			delay(1);
		}
	}
}

//
void loop() {
	
	// check if ESP Restart is needed
	if (ESP_restart) {
		DebugPrintln("Restarting...");
		delay(1000);
		ESP.restart();
	}
	
	// check WiFi status
	if (WiFi.status() != WL_CONNECTED) {
		if (millis() - wifi_lastCheck > wifi_connect_timeout*1000) {
			// WiFi try to reconnect
			WiFi_connect();
			wifi_lastCheck = millis();
		}
	}
		
	// OpenTherm loop
	ot_loop();
	
	// HTTP server handle client requests
	httpServer.handleClient();
	delay(1);

}

