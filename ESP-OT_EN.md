## About this project
ESP-OT firmware works on ESP8266/ESP32 boards and it allows you control your boiler by Opentherm protocol using Opentherm Adapter (http://ihormelnyk.com/opentherm_adapter) or it homemade analogue.
Full version as well as Free version are uses a "smart" flexible algoritm to communicate with boiler using only responsible DataIDs. After startup it making a full scan of all DataID's described in Opentherm protocol and marking each one for getting boiler response. Further communication with boiler are made using only responsible DataID. This make it possible to adjust a composition of requests during communication depending on boiler model.

## Code example - ESP-OT Lite version
Folder \ESP-OT-Lite contains the core code with an example of standard algorithm to communicate with boiler using sequential requests of necessary DataIDs as well as web interface with boiler current data and settings. Here you can also find the compiled version for Wemos D1 mini / NodeMCU controller. Adapter connection uses standard pins 5 and 4. Boiler response description are made only for DataIDs supported by my Buderus Logamax U072 and can be added by yourself if needed. 

You can also make a report of DataIDs supported by your boiler using "OT report" link in ESP-OT Free version firmware.

## ESP-OT Free version
You can download and try a full function ESP-OT firmware Free version (\ESP-OT-Free). It allows you connect to your WiFi network, 
display and control the boiler using Web-interface, make a report of DataID list supported by your boiler, display and control Thermostat settings.
The only restriction of Free version is automatic restart every 24 hours and settings you made are not saved. 

ESP-OT-Free.D1-mini.bin - build for Wemos D1 mini (NodeMCU). Opentherm adapter connection: adapter output (boiler output to controller) connect to D2 pin (GPIO-04), adapter input (controller output to boiler) connect to D1 pin (GPIO-05). DS18B20 sensor connection - D5 pin (GPIO-14). Sensor read interval 30 sec.

ESP-OT-Free.D1-mini32.bin - build for ESP32 D1 mini board. Opentherm adapter connection: adapter output (boiler output to controller) connect to GPIO-21 pin, adapter input (controller output to boiler) connect GPIO-22 pin. DS18B20 sensor connection - GPIO-18 pin. Sensor read interval and thermostat calc interval 20 sec.

During first start it will try to use the last successful WiFi connection credentials (LED flashing).
If failed it will start in WiFi AP mode (LED on), use password 12345678 for connect, open "Configure WiFi" page and make a connection to your WiFi network.

## ESP-OT Full version
ESP-OT firmware Full version will be made for your board using ChipID (MAC address). It using startup config file stored in ESP Flash memory for user flexible configuration of your device and it automaticaly saves all settings made during device usage, using MQTT for publish all working parameters and for controlling device work (for example from HomeAssistant), and you can use Telnet connection (besides Serial) for debugging info.

It also can include the following features:
- reading a wide range of wired sensors connected to your device,
- reading BLE sensors (like Xiaomi Mijia) in case of using ESP32 board,
- reading network connected sensors by MQTT or by HTTP GET requests,
- controlling other equipment (for example PUMP or MIXER) by switching ON/OFF as well as using Servo-control,
- using OLED display for showing working parameters and controlling them by Touch Sensors (ESP32 feature),
- using GSM modem (like TTGO T-Call board) for SMS info and control your device,
- interaction with other controllers in complex heating control system.

## Using BLE sensors
There is two modes of using BLE sensors: connection mode (1) and advertizing mode (2). In mode (1) bluetooth connection established to the sensor so can use only one sensor but receiving the BLE packages in this mode are not interrupting the main cycle of your device. In mode (2) you can use a several sensors but receiving the BLE packages are carried out by scanning broadcats so this mode have to interrupt the main cycle of your device.

## Conclusion
ESP-OT firmware source code for all above mentioned features developed and tested in real environment and garanted stable Fail-safe work.
Communication with author by email esp-ot@mail.ru
