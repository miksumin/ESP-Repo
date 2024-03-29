2.1.5(20231215)


[Thermo Relays]:
Добавлен параметр [thermos_delta] для определения гистерезиса, одинакового для ВСЕХ Thermo Relays (по умолчанию используется значение 0.1). Добавлен параметр [thermo_delta] для определения гистерезиса для конкретного Thermo Relay (по умолчанию используется значение thermos_delta).


[Thermostat]:
Добавлена возможность использования значений [CH Temp] и [DHW Temp] в качестве значений датчика термостата. Для этого в 
параметре [pid_sensor_name] указываем предопределенные значения [BOILER] и [WATER] соответственно.


[Controls]:
Проведена оптимизация алгоритмов функционала работы Контролов. Для публикации состояния Контрола используются топики /State (0/1) и /Status (ON/OFF). Для управления состоянием Контроля используется топик /SetState (0/1).
Добавлена возможность использования значений [CH Status], [DHW status], [Flame status] и [Fault status] в качестве управляющих значений для конкретного Control-а. Для этого при определении Control добавляем параметр [control_source_name], в котором указываем предопределенные значения [BOILER], [WATER], [FLAME] и [FAULT] соответственно. Для использования значений [CH Mode] и [DHW Mode] в качестве управляющих начений для конкретного Control-а в параметре [control_source_name] указываем предопределенные значения [CHMODE] и [DHWMODE] соответственно.


[MQTT]:
Реализован функционал для нативной интеграции параметров работы прошивки в систему HomeAssistant, который автоматически формирует в HA описание нескольких устройств - это собственно сам контроллер, котел (Boiler), Термостат, BLE датчики и Контролы. Для включения функционала необходимо в конфиге указать параметр [mqtt_HA_discovery 1].


Изменены некоторые топики, а именно: /Thermostat/Temp (ранее /Thermostat/TSet), /Opentherm/CHtemp (ранее /Opentherm/CHTset) и /Opentherm/DHWtemp (ранее /Opentherm/DHWTSet).
Расширенный набор значений общих параметров работы прошивки теперь публикуются в формате JSON в топике /System/Info. Информация о доступности контроллера по прежнему публикуется в топике /System/Online
