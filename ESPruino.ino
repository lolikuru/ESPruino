#include <ESP8266WiFi.h>        //Содержится в пакете. Видео с уроком http://esp8266-arduinoide.ru/step1-wifi
#include <ESP8266WebServer.h>   //Содержится в пакете. Видео с уроком http://esp8266-arduinoide.ru/step2-webserver
#include <ESP8266SSDP.h>        //Содержится в пакете. Видео с уроком http://esp8266-arduinoide.ru/step3-ssdp
#include <FS.h>                 //Содержится в пакете. Видео с уроком http://esp8266-arduinoide.ru/step4-fswebserver
//                    ПЕРЕДАЧА ДАННЫХ НА WEB СТРАНИЦУ. Видео с уроком http://esp8266-arduinoide.ru/step5-datapages/
//                    ПЕРЕДАЧА ДАННЫХ С WEB СТРАНИЦЫ.  Видео с уроком http://esp8266-arduinoide.ru/step6-datasend/
#include <ArduinoJson.h>        //Установить из менеджера библиотек.
//                    ЗАПИСЬ И ЧТЕНИЕ ПАРАМЕТРОВ КОНФИГУРАЦИИ В ФАЙЛ. Видео с уроком http://esp8266-arduinoide.ru/step7-fileconfig/
#include <ESP8266HTTPUpdateServer.h>  //Содержится в пакете.

#include <ArduinoOTA.h>//Прошивка по воздуху, выбирать flash size с OTA
//flesh minimum 4mb
#include <base64.h>

// Объект для обнавления с web страницы
ESP8266HTTPUpdateServer httpUpdater;

#include <time.h>
long start_t;
String encoded;

// Web интерфейс для устройства
ESP8266WebServer HTTP;


bool is_authenticated() {
  Serial.println("Enter is_authenticated");
  if (HTTP.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = HTTP.header("Cookie");
    Serial.println(cookie);
    Serial.println(encoded);
    if (cookie.indexOf(encoded) != -1) {
      Serial.println("Authentication Successful");
      return true;
    }
  }
  Serial.println("Authentication Failed");
  return false;
}

// Для файловой системы
File fsUploadFile;

// Определяем переменные wifi
String _ssid     = "Saya5G"; // Для хранения SSID
String _password = "markiz18"; // Для хранения пароля сети
String _ssidAP = "WiFi";   // SSID AP точки доступа
String _passwordAP = "00000000"; // пароль точки доступа
String SSDP_Name = "Update"; // Имя SSDP
int timezone = 4;               // часовой пояс GTM
String _ntp = "192.168.1.1"; //сервер времени
String last_time = "";
bool WIFI_AP_on = false;

byte Pinout[8] = {1, 0, 0, 0, 0, 0, 0, 0}; //статус включения вывода(виртуального)
int shifter_a = 0; //бит переключение 74hc595
String Pinout_name[8] = {"Pin1", "Pin2", "Pin3", "Pin4", "Pin5", "Pin6", "Pin7", "Pin8"}; //Названия выводов
String Alarm_on[8] = {"00:00", "00:00", "00:00", "00:00", "00:00", "00:00", "00:00", "00:00"}; //время включения
String Alarm_off[8] = {"23:59", "23:59", "23:59", "23:59", "23:59", "23:59", "23:59", "23:59"}; //время выключения
String alarm_state_on[8] = {"OFF", "OFF", "OFF", "OFF", "OFF", "OFF", "OFF", "OFF"}; //включена
String alarm_state_off[8] = {"OFF", "OFF", "OFF", "OFF", "OFF", "OFF", "OFF", "OFF"}; //или выключена настройка



String jsonConfig = "{}";
int port = 80;

int led[8] = {4, 14, 16, 2, 5, 12, 13}; //вывод для управления
int brightness[3] = {0, 0, 0};  // переменная для fade
int bright[3] = {6, 6, 6}; // делей
int fadeAmount[3] = {5, 5, 5};  // шаг фейда
byte fadeon[3] = {1, 1, 1}; //вкл выкл режима fade
long t0 = 0; //millis()-time_synchr
long tt = 0; //отсчет от текущей точки включения

void setup() {
  ESP8266WebServer server(port);
  Serial.begin(115200);
  Serial.println("");
  //Запускаем файловую систему
  Serial.println("Start 4-FS");
  FS_init();
  Serial.println("Step7-FileConfig");
  loadConfig();
  Serial.println("Start 1-WIFI");
  //Запускаем WIFI
  WIFIinit();
  Serial.println("Start 8-Time");
  // Получаем время из сети
  Time_init();
  //Настраиваем и запускаем SSDP интерфейс
  Serial.println("Start 3-SSDP");
  SSDP_init();
  //Настраиваем и запускаем HTTP интерфейс
  Serial.println("Start 2-WebServer");
  HTTP_init();
  Serial.println("Start 9-OTAServer");
  StartOTA();
  Serial.println("Set 10-SetWebToken");
  SetToken();

  for (byte l = 0; l < 7; l++) {
    pinMode(led[l], OUTPUT);
    digitalWrite(led[l], LOW);
  }
}

void loop() {
  ArduinoOTA.handle();
  HTTP.handleClient();
  delay(1);

  if (millis() % 1000 == 0) //счёт времени
    if (tt != millis() / 1000) {
      tt = millis() / 1000;
      long now_t = start_t + tt - t0;
      //    Serial.println(now_t);
      String Time = ""; // Строка для результатов времени
      Time += ctime(&now_t);
      int i = Time.indexOf(":");
      Time = Time.substring(i - 2, i + 6);
      //Serial.println(Time);
      if (last_time!=Time.substring(0, 5)) {
        Serial.println(Time.substring(0, 5));
        for (byte l = 0; l < 8; l++) {
          if (alarm_state_on[l] == "ON" && Alarm_on[l] == Time.substring(0, 5)) {
            Pinout[l - 1] = 1;
            Serial.println(l);
            saveConfig();
            pinShift();
            Serial.println("ON");
          }
          if (alarm_state_off[l] == "ON" && Alarm_off[l] == Time.substring(0, 5)) {
            Pinout[l - 1] = 0;
            Serial.println(l);
            saveConfig();
            pinShift();
            Serial.println("OFF");
          }
        }
        if (last_time.substring(0, 3)!=Time.substring(0, 3)){
          SetToken();
        }//меняем токен каждый час
        last_time=Time.substring(0, 5);
      }
    }


  for (byte l = 0; l < 3; l++){
    if (fadeon[l] == 1) {
      if (Pinout[l] == 1)
        Serial.println("Faid"+l);
        //faid_0(l);
    }
    else if (brightness[l] != 0) {
      brightness[l] = 0;
      analogWrite(led[l], 0);
      digitalWrite(led[l], HIGH);
    }
  }
}


void SetToken(void){
    String toEncode = random(1000) + GetTime().substring(0, 2) + GetDate();//токен меняется каждый час
    //Serial.println(toEncode);
    encoded = "ESPSESSIONID=" + base64::encode(toEncode);
}

void pinShift() { //включение pinout + 74hc595 ttl
  shifter_a = 0;
  for (byte i = 0; i < 8; i++) {
    if (Pinout[i] == 1)
    {
      shifter_a += pow(2, i);
      if (fadeon[i] != 1)
        digitalWrite(led[i], HIGH);
    }
    else if (fadeon[i] != 1)
      digitalWrite(led[i], LOW);
    else {
      brightness[i] = 0;
      analogWrite(led[i], 0);
    }
  }
  Serial.println(shifter_a);
}

void faid_0(int pin) {
  if (millis() % bright[pin] == 0)
  {


    // change the brightness for next time through the loop:
    brightness[pin] = brightness[pin] + fadeAmount[pin];
    if (brightness[pin] < 0)brightness[pin] = 0;

    // reverse the direction of the fading at the ends of the fade:
    if (brightness[pin] <= 0 || brightness[pin] >= 255) {
      fadeAmount[pin] = -fadeAmount[pin];
    }
    analogWrite(led[pin], brightness[pin]);

  }
}
