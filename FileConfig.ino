// Загрузка данных сохраненных в файл  config.json
bool loadConfig() {
  // Открываем файл для чтения
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
  // если файл не найден  
    Serial.println("Failed to open config file");
  //  Создаем файл запиав в него аные по умолчанию
    saveConfig();
    configFile.close();
    return false;
  }
  // Проверяем размер файла, будем использовать файл размером меньше 1024 байта
  size_t size = configFile.size();
  if (size > 4096) {
    Serial.println("Config file size is too large");
    configFile.close();
    return false;
  }

// загружаем файл конфигурации в глобальную переменную
  jsonConfig = configFile.readString();
  configFile.close();
  // Резервируем памяь для json обекта буфер может рости по мере необходимти предпочтительно для ESP8266 
    DynamicJsonBuffer jsonBuffer;
  //  вызовите парсер JSON через экземпляр jsonBuffer
  //  строку возьмем из глобальной переменной String jsonConfig
    JsonObject& root = jsonBuffer.parseObject(jsonConfig);
  // Теперь можно получить значения из root  
    _ssidAP = root["ssidAPName"].as<String>(); // Так получаем строку
    _passwordAP = root["ssidAPPassword"].as<String>();
    timezone = root["timezone"];               // Так получаем число
    SSDP_Name = root["SSDPName"].as<String>();
    _ssid = root["ssidName"].as<String>();
    _password = root["ssidPassword"].as<String>();
    _ntp = root["ntp"].as<String>();
    rotate_angle = root["angle"];
    dayily_count = root["dayily_count"];
      for(byte i=0;i<8;i++) 
      {//так получаем число и строку в массиве
        //Pinout_name[i] = root["pin"+ String(i)][0].as<String>();
        //Pinout[i] = root["pin"+ String(i)][1];
        alarm_time[i] = root["pin"+ String(i)][0].as<String>();
        //Alarm_off[i] = root["pin"+ String(i)][3].as<String>();
        alarm_state_on[i] = root["pin"+ String(i)][1].as<String>();
        //alarm_state_off[i] = root["pin"+ String(i)][5].as<String>();
        }
       //for(byte i=0;i<3;i++) {
       // fadeon[i] = root["fadeon"+ String(i)];
       // bright[i] = root["bright"+ String(i)];
       // fadeAmount[i] = root["amount"+ String(i)];
       // }
    return true;
}

// Запись данных в файл config.json
bool saveConfig() {
  // Резервируем память для json обекта буфер может рости по мере необходимти предпочтительно для ESP8266 
  DynamicJsonBuffer jsonBuffer;
  //  вызовите парсер JSON через экземпляр jsonBuffer
  JsonObject& json = jsonBuffer.parseObject(jsonConfig);
  // Заполняем поля json 
  json["SSDPName"] = SSDP_Name;
  json["ssidAPName"] = _ssidAP;
  json["ssidAPPassword"] = _passwordAP;
  json["ssidName"] = _ssid;
  json["ssidPassword"] = _password;
  json["timezone"] = timezone;
  json["ntp"] = _ntp;
  json["angle"] = rotate_angle;
  json["dayily_count"] = dayily_count;
  
  for(byte i=0;i<8;i++){
  JsonArray& pins = json.createNestedArray("pin"+String(i));
//  pins.add(Pinout_name[i]);
//pins.add(Pinout[i]);
  Serial.println(alarm_time[i] + " " + alarm_state_on[i]);
  pins.add(alarm_time[i]);
//  pins.add(Alarm_off[i]);
  pins.add(alarm_state_on[i]);
//  pins.add(alarm_state_off[i]);
  }
  /*
  for(byte i=0;i<3;i++){
  json["fadeon"+String(i)] = fadeon[i];
  json["bright"+String(i)] = bright[i];
  if(fadeAmount[i]<0)fadeAmount[i]=-fadeAmount[i];
  json["amount"+String(i)] = fadeAmount[i];
  
  }
  */
 
  // Помещаем созданный json в глобальную переменную json.printTo(jsonConfig);
  json.printTo(jsonConfig);
  // Открываем файл для записи
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    //Serial.println("Failed to open config file for writing");
    configFile.close();
    return false;
  }
  // Записываем строку json в файл 
  json.printTo(configFile);
  configFile.close();
  return true;
  }
