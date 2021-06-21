
void HTTP_init(void) {

  HTTP.on("/configs.json", handle_ConfigJSON); // формирование configs.json страницы для передачи данных в web интерфейс
  // API для устройства
  HTTP.on("/ssdp", handle_Set_Ssdp);     // Установить имя SSDP устройства по запросу вида /ssdp?ssdp=proba
  HTTP.on("/ntp", handle_Set_Ntp);        //установка сервера обновления времени NTP
  HTTP.on("/pins", handle_Set_Pins);        // установка Pinout выходов статуса и имени
  HTTP.on("/ssid", handle_Set_Ssid);     // Установить имя и пароль роутера по запросу вида /ssid?ssid=home2&password=12345678
  HTTP.on("/ssidap", handle_Set_Ssidap); // Установить имя и пароль для точки доступа по запросу вида /ssidap?ssidAP=home1&passwordAP=8765439
  HTTP.on("/restart", handle_Restart);   // Перезагрузка модуля по запросу вида /restart?device=ok
  HTTP.on("/mode", handle_mode);
  HTTP.on("/set_alarm", handle_Set_Alarm);
  
  // Добавляем функцию Update для перезаписи прошивки по WiFi при 1М(256K SPIFFS) и выше
  httpUpdater.setup(&HTTP);
  // Запускаем HTTP сервер
  //HTTP.onNotFound(handleNotFound);
  //here the list of headers to be recorded
  const char * headerkeys[] = {"User-Agent", "Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
  //ask HTTP to track these headers
  HTTP.collectHeaders(headerkeys, headerkeyssize);
  HTTP.on("/", handleRoot);
  HTTP.on("/login", handleLogin);
  // Запускаем HTTP сервер
  HTTP.begin();
}
void handle_Set_Alarm(){// set_alarm?pinout=1&alarm_on=60 будильнег
  if (authCheck()) {// Проверка токена
    String pinout = HTTP.arg("pinout");
    if(pinout.toInt() >= 0 && pinout.toInt() < 8&&(HTTP.arg("alarm_state_on")||HTTP.arg("alarm_state_off"))){
    if(HTTP.arg("alarm_state_on")=="ON")alarm_state_on[pinout.toInt()] = "ON";
    if(HTTP.arg("alarm_state_on")=="OFF")alarm_state_on[pinout.toInt()] = "OFF";
    if(HTTP.arg("alarm_state_off")=="ON")alarm_state_off[pinout.toInt()] = "ON";
    if(HTTP.arg("alarm_state_off")=="OFF")alarm_state_off[pinout.toInt()] = "OFF";
      saveConfig();
      HTTP.send(200, "text/plain", "OK");
    }
    String alarm = HTTP.arg("alarm_on");
  if(alarm != "") {
    if(pinout.toInt() >= 0 && pinout.toInt() < 8){
      Alarm_on[pinout.toInt()] = alarm;  
      saveConfig();
      HTTP.send(200, "text/plain", "OK");
      return;
    } else{ 
      HTTP.send(406, "text/plain", "alarm_pin_err");
      return; 
      } 
      alarm = "";
    } else 
    alarm = HTTP.arg("alarm_off");
    if(alarm != ""){
    Alarm_off[pinout.toInt()] = alarm; 
    saveConfig();
    HTTP.send(200, "text/plain", "OK");
    }
    else {
    HTTP.send(406, "text/plain", "alarm_pin_err");
    }
  }
}

// Функции API-Set
// Установка SSDP имени по запросу вида http://192.168.0.101/ssdp?ssdp=proba
void handle_Set_Ssdp() {
  if (authCheck()) {// Проверка токена
    if(HTTP.arg("ssdp") != ""){
    SSDP_Name = HTTP.arg("ssdp"); // Получаем значение ssdp из запроса сохраняем в глобальной переменной
    saveConfig();                 // Функция сохранения данных во Flash
    HTTP.send(200, "text/plain", "OK");}
    else HTTP.send(406, "text/plain", "Name ERRoR");// отправляем ответ о выполнении
  }
}
void handle_mode() {
  if (authCheck()) {// Проверка токена
    for (byte i = 0; i < 3; i++)
      {
        if(HTTP.arg("fadeon" + String(i)) != ""){  
      
        if(HTTP.arg("fadeon" + String(i)).toInt()==1||HTTP.arg("fadeon" + String(i)).toInt()==0)  
        {
          fadeon[i] = HTTP.arg("fadeon"+String(i)).toInt();  
          HTTP.send(200, "text/plain", "OK");
        }
        else HTTP.send(406, "text/plain", "fade must be =0 or =1");
        }
        
        if(HTTP.arg("bright" + String(i)) != ""){  
        if(HTTP.arg("bright" + String(i)).toInt()>0&&HTTP.arg("bright" + String(i)).toInt()<256)  
        {
          bright[i] = HTTP.arg("bright"+String(i)).toInt(); 
          HTTP.send(200, "text/plain", "OK");
        }
        else HTTP.send(406, "text/plain", "bright must be >0 and <265");
          }
        if(HTTP.arg("amount" + String(i)) != "")
        if(HTTP.arg("amount" + String(i)).toInt()>0&&HTTP.arg("amount" + String(i)).toInt()<30)  
        {
          fadeAmount[i] = HTTP.arg("amount"+String(i)).toInt(); 
          HTTP.send(200, "text/plain", "OK");
        }
        else HTTP.send(406, "text/plain", "amount must be >0 and <30");
      } 
      saveConfig(); 
      pinShift(); 
  }
}


void handle_Set_Pins() {
  if (authCheck()) {// Проверка токена
    for (byte i = 0; i < 8; i++)
      if (HTTP.arg("pin" + String(i)) == "0"||HTTP.arg("pin" + String(i)) == "1")//москитная сетка
      {
        Pinout[i] = HTTP.arg("pin" + String(i)).toInt();
        saveConfig();
        
      }else 
        if(HTTP.arg("pin" + String(i)) != "")
        {Pinout_name[i] = HTTP.arg("pin" + String(i));
        saveConfig();}
      pinShift();
      
    HTTP.send(200, "text/plain", "OK");
  }
}
void handle_Set_Ntp() {
  if (authCheck()) {// Проверка токена
    _ntp = HTTP.arg("ntp");
    timeSynch(timezone);
    saveConfig();
    HTTP.send(200, "text/plain", "OK"); // отправляем ответ о выполнении
  }
}
// Установка параметров для подключения к внешней AP по запросу вида http://192.168.0.101/ssid?ssid=home2&password=12345678
void handle_Set_Ssid() {
  if (authCheck()) {// Проверка токена
    _ssid = HTTP.arg("ssid");            // Получаем значение ssid из запроса сохраняем в глобальной переменной
    _password = HTTP.arg("password");    // Получаем значение password из запроса сохраняем в глобальной переменной
    saveConfig();                        // Функция сохранения данных во Flash
    HTTP.send(200, "text/plain", "OK");   // отправляем ответ о выполнении
  }
}
//Установка параметров внутренней точки доступа по запросу вида http://192.168.0.101/ssidap?ssidAP=home1&passwordAP=8765439
void handle_Set_Ssidap() {
  if (authCheck()) {// Проверка токена
    _ssidAP = HTTP.arg("ssidAP");         // Получаем значение ssidAP из запроса сохраняем в глобальной переменной
    _passwordAP = HTTP.arg("passwordAP"); // Получаем значение passwordAP из запроса сохраняем в глобальной переменной
    saveConfig();                         // Функция сохранения данных во Flash
    HTTP.send(200, "text/plain", "OK");   // отправляем ответ о выполнении
  }
}
// Перезагрузка модуля по запросу вида http://192.168.0.101/restart?device=ok
void handle_Restart() {
  if (authCheck()) {// Проверка токена
    String restart = HTTP.arg("device");          // Получаем значение device из запроса
    if (restart == "ok") {                         // Если значение равно Ок
      HTTP.send(200, "text / plain", "Reset OK"); // Oтправляем ответ Reset OK
      ESP.restart();                                // перезагружаем модуль
    }
    else {                                        // иначе
      HTTP.send(200, "text / plain", "No Reset"); // Oтправляем ответ No Reset
    }
  }
}

void handle_ConfigJSON() {
  String root = "{}";  // Формировать строку для отправки в браузер json формат
  //{"SSDP":"SSDP-test","ssid":"home","password":"i12345678","ssidAP":"WiFi","passwordAP":"","ip":"192.168.0.101"}
  // Резервируем память для json обекта буфер может рости по мере необходимти, предпочтительно для ESP8266
  DynamicJsonBuffer jsonBuffer;
  //  вызовите парсер JSON через экземпляр jsonBuffer
  JsonObject& json = jsonBuffer.parseObject(root);
  // Заполняем поля json
  json["SSDP"] = SSDP_Name;
  json["ssidAP"] = _ssidAP;
  json["passwordAP"] = _passwordAP;
  json["ssid"] = _ssid;
  json["password"] = _password;
  json["timezone"] = timezone;
  json["ip"] = WiFi.localIP().toString();
  json["time"] = GetTime();
  json["date"] = GetDate();
  json["ntp"] = _ntp;
  for (byte i = 0; i < 8; i++) {
  JsonArray& pins = json.createNestedArray("pin"+String(i));
  pins.add(Pinout_name[i]);
  pins.add(Pinout[i]);
  pins.add(Alarm_on[i]);
  pins.add(Alarm_off[i]);
  pins.add(alarm_state_on[i]);
  pins.add(alarm_state_off[i]);
  }
  for(byte i=0;i<3;i++){
  json["fadeon"+String(i)] = fadeon[i];
  json["bright"+String(i)] = bright[i];
  if(fadeAmount[i]<0)fadeAmount[i]=-fadeAmount[i];
  json["amount"+String(i)] = fadeAmount[i];
  }
  // Помещаем созданный json в переменную root
  root = "";
  json.printTo(root);
  HTTP.send(200, "text/json", root);
}
