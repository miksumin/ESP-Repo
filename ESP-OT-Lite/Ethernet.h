
// Ethernet module

// WT32-ETH01 ethernet pins
#define ETH_PHY_ADDR	1
//#define ETH_PHY_TYPE    ETH_PHY_LAN8720
#define ETH_PHY_POWER	16
//#define ETH_PHY_MDC     23
//#define ETH_PHY_MDIO    18

#include <ETH.h>

bool eth_enabled = true;
bool eth_connected = false;

// Ether static IP
IPAddress eth_address(192, 168, 0, 99);
IPAddress eth_gateway(192, 168, 0, 1);
IPAddress eth_netmask(255, 255, 255, 0);

// Google DNS Server IP
IPAddress eth_DNS(8, 8, 8, 8);

byte eth_conn_wait = 10;			// time to wait for ethernet connection, sec
bool eth_got_address = false;

//
void Eth_Event(WiFiEvent_t event) {
	//
	switch (event) {
		case SYSTEM_EVENT_ETH_START:
			//DebugPrintln("Ethernet Started");
			//set eth hostname here
			ETH.setHostname(HOSTNAME);
			break;
		case SYSTEM_EVENT_ETH_CONNECTED:
			DebugPrintln("Ethernet Connected");
			eth_connected = true;
			break;
		case SYSTEM_EVENT_ETH_GOT_IP:
			//if (!eth_connected) {
				eth_connected = true;
				eth_got_address = true;
				DebugPrintln("Ethernet MAC: "+String(ETH.macAddress()));
				DebugPrintln("Ethernet IPv4: "+String(ETH.localIP().toString()));
				DebugPrintln("Ethernet Link: "+String(ETH.linkSpeed())+"Mbps, "+String(ETH.fullDuplex()? "FULL_DUPLEX":"HALF_DUPLEX"));
				/* Serial.print("ETH MAC: ");
				Serial.print(ETH.macAddress());
				Serial.print(", IPv4: ");
				Serial.print(ETH.localIP());
				Serial.print(", ");
				Serial.print(ETH.linkSpeed());
				Serial.print("Mbps, ");
				if (ETH.fullDuplex())
					DebugPrintln("FULL_DUPLEX");
				else
					DebugPrintln("HALF_DUPLEX"); */
			/* } */
			break;
		case SYSTEM_EVENT_ETH_DISCONNECTED:
			DebugPrintln("Ethernet Disconnected");
			eth_connected = false;
			break;
		case SYSTEM_EVENT_ETH_STOP:
			DebugPrintln("Ethernet Stopped");
			eth_connected = false;
			break;
		default:
			break;
	}
}

//
void Eth_onEvent() {
	WiFi.onEvent(Eth_Event);
}

//
bool Eth_Begin() {
	
	// To be called before ETH.begin()
	//WiFi.onEvent(Eth_Event);
	Eth_onEvent();

	//bool begin(uint8_t phy_addr=ETH_PHY_ADDR, int power=ETH_PHY_POWER, int mdc=ETH_PHY_MDC, int mdio=ETH_PHY_MDIO, 
	//           eth_phy_type_t type=ETH_PHY_TYPE, eth_clock_mode_t clk_mode=ETH_CLK_MODE);
	//ETH.begin(ETH_PHY_ADDR, ETH_PHY_POWER, ETH_PHY_MDC, ETH_PHY_MDIO, ETH_PHY_TYPE, ETH_CLK_MODE);
	//ETH.begin();
	DebugPrintln(F("Ethernet starting..."));
	if (!ETH.begin(ETH_PHY_ADDR, ETH_PHY_POWER)) {
		DebugPrintln(F("Ethernet failed to start"));
		eth_enabled = false;
		return false;
	}
	
	// Try to use DHCP to get IP address
	uint32_t time_point = millis();
	DebugPrintln(F("Ethernet waiting for connect ..."));
	while ((millis()-time_point < eth_conn_wait*1000) && (!eth_got_address)) {
		//
	}
	if (eth_connected) {
		if (String(ETH.localIP().toString()) != "0.0.0.0") {
			return true;
		}
		else {
			DebugPrintln(F("Ethernet DHCP failed"));
		}
	}
	// Static IP, leave without this line to get IP via DHCP
	//bool config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns1 = 0, IPAddress dns2 = 0);
	
	// Use static IP and wait for 5 sec
	DebugPrintln(F("Ethernet using static IP"));
	ETH.config(eth_address, eth_gateway, eth_netmask, eth_DNS);
	//
	/* time_point = millis();
	DebugPrintln("Ethernet waiting for connect ...");
	while ((millis()-time_point < eth_conn_wait*1000) && (!eth_connected)) {
		//
	} */
	if (eth_connected) {
		if (String(ETH.localIP().toString()) != "0.0.0.0") {
			return true;
		}
	}
	return false;
}

String Eth_getAddress() {
	return String(ETH.localIP().toString());
}

//
String Eth_getHostname() {
	return String(ETH.getHostname());
}

//
void Eth_setHostname(char* hostname) {
	ETH.setHostname(hostname);
}

String Eth_GetState() {
	return String(eth_connected? "Connected":"Not connected");
}

//
bool Eth_isConnected() {
	return eth_connected;
}
	
