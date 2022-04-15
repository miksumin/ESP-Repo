## Об этом проекте
ESP-Repo это набор функциональных модулей, разработанных автором в разное время, из которых собирается проект для решения конкретной задачи.

Проект ESP-Opentherm (https://github.com/miksumin/ESP-Repo/blob/main/ESP-OT.md) решает задачи управления работой котла с помощью термостата. В проекте реализован авторский гибкий или "обучаемый" алгоритм взаимодействия с котлом, при котором после старта прошивки происходит однократный опрос всех ID, описанных в протоколе Opentherm, на предмет получения разумного ответа от котла и маркировкой ID, на которые котел не отвечает. В дальнейшей работе с котлом используются только такие "читаемые" ID. Таким образом код прошивки подстраивается под конкретную модель котла.
Первичный опрос занимает около 15 сек, но работа основного цикла при этом не блокируется, т.к. все запросы делаются в асинхронном режиме.
Алгоритм опроса "читаемых" ID настраивается в коде указанием интервала опроса для каждого ID, либо указанием необходимости только однократного опроса.

Проект ESP-Thermo реализует функционал многоканального термостата (до 8 каналов) с управлением исполнительными устройствами с помощью реле. В качестве датчиков температуры возможно использование датчиков типа DHT11/DHT22, 1-Wire(DS18B20) или BLE датчиков (Xiaomi Mijia). Каждый канал привязан к своему датчику и имеет свою настройку поддерживаемой температуры. В качестве исполнительных устройств могут выступать клапаны с электромагнитными или сервоприводами.

Все прошивки при запуске используют файл конфигурации с настройками, который хранится во flash памяти контроллера и доступен для редактирования пользователем через веб-интерфейс.

Установка и запуск прошивки - https://github.com/miksumin/ESP-Repo/wiki#Установка-и-запуск-прошивки

Описание настроек параметров файла конфигурации - https://github.com/miksumin/ESP-Repo/wiki/Параметры-файла-конфигурации-прошивки.

Полный перечень MQTT-топиков с подробным описанием - https://github.com/miksumin/ESP-Repo/wiki/Описание-MQTT-топиков.


## Варианты прошивки
Прошивка контроллера собирается под задачи заказчика и дополнительно может включать в себя следующие возможности:
- чтение проводных датчиков температуры воздуха, теплоносителя, подключенных непосредственно к контроллеру,
- чтение беспроводных датчиков, в том числе по протоколам BLE (датчики умных домов) и RF-433 (выносные датчики метеостанций),
- получение данных с датчиков и с других устройств по сети (MQTT, HTTP GET),
- управление отопительным и другим оборудованием (насос, клапан двухпозиционный, клапан с сервоприводом),
- алгоритмы термостатирования (автоматическое управление работой котла на основе заданных параметров),
- отправка всех параметров работы контроллера на MQTT брокер для дальнейшего сбора, отображения и анализа информации,
- управление работой котла и термостата через MQTT брокер,
- использование OLED дисплея для отображения текущих параметров работы котла и управление с помощью сенсорных кнопок,
- использование GSM модуля для информирования и управления работой котла с помощью SMS сообщений,
- сохранение всех параметров настройки и работы контроллера в файле с возможностью редактирования через веб-интерфейс,
- взаимодействие с другими контроллерами в составе комплексной системы управления отоплением.


В папке \HomeAssistant выложены YAML файлы для настройки MQTT топиков контроллера в HomeAssistant - Сенсоры, Двоичные Сенсоры и Переключатели. 
Предварительно нужно в configuration.yaml прописать путь к ним и в файлах заменить имя хоста в топиках на свое.

## Работа с BLE датчиками
Работа с BLE устройствами (датчики) возможна в двух режимах: в режиме приема рекламных пакетов (1) и в режиме подключения к устройству (2).
В первом случае возможен прием рекламных пакетов с любого кол-ва BLE устройств, во втором случае работа возможна только с одним BLE устройством.

## Заключение
Весь код для работы перечисленных функций (за исключением используемых сторонних библиотек) разработан и протестирован 
автором в реальных условиях и гарантирует надежную работу контроллера в режиме Fail-Safe.
Возможна сборка контроллера с адаптером и дисплеем (при необходимости) в стандартном корпусе на DIN-рейку.
Полнофункциональная версия контроллера предоставляется в виде готовой прошивки на коммерческой основе.

Для связи с автором писать на почту esp-ot@mail.ru
