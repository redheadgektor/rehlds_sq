# ReHLDS Query Handler
Плагин для изменения значений ответа SourceQuery для клиентов, мониторингов и остальных браузеров

# Commands
```
sq_help
```

```
//Квары имеющие значение -1 будут использовать значения сервера

//Режим работы
//0 - обработка будет выполняться Reunion (если есть) или Steam (если нет Reunion)
//1 - изменение ответов на запрос (см. квары)
//2 - обработка передаётся Steam-сервисам, с них же и получается информация
//3 - сервер не отвечает на запросы (сервер исчезает из мониторингов, браузера серверов и т.д.)
sq_query "1"

//меняет название сервера (в браузере серверов клиента, мониторингах и т.д.)
sq_hostname "-1"

//скрывает список игроков
sq_hideplayers "0"

//скрывает квары
sq_hiderules "0"

//номер протокола (на подключение не влияет)
sq_protocol "-1"

//карта
sq_map "-1"

//директория игры (мода)
sq_gamedir "-1"

//имя игры (мода)
sq_gamename "-1"

//номер игры в магазине Steam (https://developer.valvesoftware.com/wiki/Steam_Application_IDs)
sq_appid "-1"

//количество игроков (макс. 255)
sq_players "-1"

//макс. игроков (макс. 255)
sq_maxplayers "-1"

//ботов
sq_bots "0"

//тип сервера
//0 - выделенный
//1 - клиент-сервер (New Game)
//2 - прокси-сервер (HLTV)
sq_servertype "0"

//ОС сервера
//0 - Windows
//1 - Linux
//2 - Mac OS
sq_serverenv "-1"

//имеется ли пароль? (на подключение не влияет)
//На клиентах появляется окно ввода пароля и значок что сервер имеет пароль
//но может подключиться через команду connect
sq_password "-1"

//имеется ли VAC?
//На клиентах в браузере показывается значок VAC
//VAC не отключается! Это лишь визуал!!!
sq_vac "-1"

//версия игры (на подключение не влияет)
sq_gameversion "-1"


//показывать запрос-ответ в консоль и адрес
sq_showpackets "0"
```
