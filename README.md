
ESP-OT это проект контроллера на основе ESP8266/ESP32 для управления работой котла с помощью адаптера (http://ihormelnyk.com/opentherm_adapter)
или его самодельных аналогов и протокола Opentherm (http://ihormelnyk.com/opentherm_library).

В папке \ESP-OT-Lite находится исходный код ядра прошивки, обеспечивающий подключение к WiFi сети и взаимодействие с котлом, 
без веб-интерфейса. Данный код при желании может быть самостоятельно дополнен веб-интерфейсом и доработан под свои потребности.

В папке \ESP-OT-Lite-v2 находится обновленная версия кода ядра прошивки, переписанная с целью более гибкой организации работы с конкретным котлом.
Так при запуске происходит однократный опрос всех ID на предмет получения разумного ответа от котла и маркировкой ID, на которые котел не отвечает.
В дальнейшей работе с котлом используются только "читаемые" ID. Таким образом код прошивки подстраивается под конкретную модель котла.
Первичный опрос занимает около 15 сек, но работа основного цикла при этом не блокируется, т.к. все запросы делаются в асинхронном режиме.
Алгоритм опроса "читаемых" ID настраивается в коде указанием интервала опроса для каждого ID, либо указанием необходимости только однократного опроса.
Расшифровка ответов котла в приведенном коде сделана только для тех ID, которые поддерживает мой котел. Расшифровка других ID может быть добавлена самостоятельно.
В частности, в процессе тестирования выяснилось, что мой котел поддерживает запрос "Fan Speed" (ID=35), которого нет в описании протокола Opentherm.
Получить отчет о конфигурации вашего котла, а также о поддерживаемых ID, возможно с помощью кнопки "Opentherm report" в бесплатной версии прошивки.

В папке \ESP-OT-Free находится собранный и готовый для работы вариант прошивки для свободного использования, обеспечивающий
подключение к выбранной WiFi сети, отображение и ручное управление основными параметрами работы котла через встроенный веб-интерфейс и 
формирование отчета поддерживаемых котлом DataID. Единственное ограничение - рестарт прошивки каждые 60 мин. 
Прошивка собрана для контроллера Wemos D1 mini (NodeMCU). Также может быть собрана для других типов контроллеров.
Подключение адаптера: выход адаптера (входной сигнал контроллера от котла) - пин D2 (GPIO-04), 
вход адаптера (выходной сигнал контроллера на котел) - пин D1 (GPIO-05).
После запуска прошивки делается попытка использовать данные последнего удачного подключения к WiFi сети (моргает светодиод).
При отсутствии таких данных, либо при невозможности подключения запускается режим точки доступа (горит светодиод), пароль 12345678.
В дальнейшем возможно подключение к другой WiFi сети с помощью веб-страницы "Configure WiFi".

В папке \HomeAssistant выложены YAML файлы для настройки топиков в HomeAssistant

Полная версия контроллера собирается под задачи заказчика и дополнительно может включать в себя следующие возможности:
- чтение датчиков температуры воздуха, теплоносителя, подключенных как непосредственно к контроллеру, 
так и по сети с других устройств, в том числе по протоколам BLE (датчики умных домов) и RF-433 (выносные датчики метеостанций),
- управление другим отопительным оборудованием (насос, клапан двухпозиционный, клапан с сервоприводом),
- алгоритмы термостатирования (автоматическое управление котлом на основе заданных параметров),
- отправка всех параметров работы контроллера на MQTT брокер для дальнейшего сбора, отображения и анализа информации,
- управление работой котла и термостата через MQTT брокер,
- использование OLED дисплея для отображения текущих параметров работы котла и управление с помощью сенсорных кнопок,
- использование GSM модуля для информирования и управления работой котла с помощью SMS сообщений,
- сохранение всех параметров настройки и работы контроллера в файле с возможностью редактирования через веб-интерфейс,
- взаимодействие с другими контроллерами в составе комплексной системы управления отоплением.

Весь код для работы перечисленных функций (за исключением используемых сторонних библиотек) разработан и протестирован 
автором в реальных условиях и гарантирует надежную работу контроллера в режиме Fail-Safe.
Возможна сборка контроллера с адаптером и дисплеем (при необходимости) в стандартном корпусе на DIN-рейку.
Полная версия ПО контроллера предоставляется в виде готовой прошивки на коммерческой основе.

Для связи с автором писать на почту esp-ot@mail.ru

