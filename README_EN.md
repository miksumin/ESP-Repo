## About this project
ESP-OT firmware works on ESP8266/ESP32 boards and it allows you control your boiler by Opentherm protocol using Opentherm Adapter (http://ihormelnyk.com/opentherm_adapter) or it homemade analogue.

## ESP-OT Free version
You can download and try a full function ESP-OT firmware Free version (\ESP-OT-Free). It allows you connect to your WiFi network, 
display and control the boiler using Web-interface, make a report of DataID list supported by your boiler, display and control Thermostat settings.
The only restriction of Free version is automatic restart every 24 hours and settings you made are not saved. 
Current version of ESP-OT Free firmware is build for Wemos D1 mini (NodeMCU) board but it also can be build for any other ESP8266/ESP32 board.
Opentherm adapter connection: adapter output (boiler output to controller) connect to D2 pin (GPIO-04), adapter input (controller output to boiler) connect to D1 pin (GPIO-05).
DS18B20 sensor connection - D3 pin (GPIO-00). During first start it trying to use the last successful WiFi connection credentials (LED flashing).
If failed it starting WiFi AP mode (LED on), use password 12345678 for connect, open "Configure WiFi" page and make a connection to your WiFi network.

## ESP-OT Full version
ESP-OT firmware Full version will be made for your board using ChipID (MAC address). It using startup config file stored in ESP Flash memory for user flexible configuration of your device and
it automaticaly saves all settings made during device usage, using MQTT for publish all working parameters and for controlling device usage (for example from HomeAssistant), 
and you can use Telnet connection (besides Serial) for debugging info.
It also can include the following features:
- reading a wide range of wired sensors connected to your device,
- reading BLE sensors (like Xiaomi Mijia) in case of using ESP32 board,
- reading network connected sensors by MQTT or by HTTP GET requests,
- controlling other equipment (for example PUMP or MIXER) by switching ON/OFF as well as using Servo-control,
- using OLED display for showing working parameters and controlling them by Touch Sensors (ESP32 feature),
- using GSM modem (like TTGO T-Call board) for SMS info and control your device,
- interaction with other controller in complex heating control system.

ESP-OT firmware source code for all above mentioned features except of used third-party libraries developed and tested in real environment and garanted stable Fail-safe work.
Communication wiith author by email esp-ot@mail.ru
