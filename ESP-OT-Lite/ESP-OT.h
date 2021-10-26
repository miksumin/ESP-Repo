
// ESP OpenTherm version
#define BUILD_NAME	"ESP-OT-Lite"
#define BUILD_VER	"1.0"
#define BUILD_DATE	"20211025"

// Define your WiFi credentials
#define DEFAULT_SSID	"Your_WiFi_SSID"
#define DEFAULT_PASS	"Your_WiFi_PASS"

// OpenTherm Adapter connection
#define OT_INPIN	5	// D1 WemosD1/NodeMCU
#define OT_OUTPIN	4	// D2 WemosD1/NodeMCU

// Buildin LED for your board
#define LED_BUILTIN	2	// WemosD1/NodeMCU

// Boiler power switch
//#define OT_POWER_PIN 18

// Serial Debug functions
#define DebugBegin(...)      ({ Serial.begin(__VA_ARGS__);    })
#define DebugPrint(...)      ({ Serial.print(__VA_ARGS__);    })
#define DebugPrintln(...)    ({ Serial.println(__VA_ARGS__);  })
#define DebugPrintf(...)     ({ Serial.printf(__VA_ARGS__);   })
#define DebugWrite(...)      ({ Serial.write(__VA_ARGS__);    })
