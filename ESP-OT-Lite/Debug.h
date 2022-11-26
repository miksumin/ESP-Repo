
// Serial Debug functions
#define DebugBegin(...)      ({ Serial.begin(__VA_ARGS__);    })
#define DebugPrint(...)      ({ Serial.print(__VA_ARGS__);    })
#define DebugPrintln(...)    ({ Serial.println(__VA_ARGS__);  })
#define DebugPrintf(...)     ({ Serial.printf(__VA_ARGS__);   })
#define DebugWrite(...)      ({ Serial.write(__VA_ARGS__);    })
