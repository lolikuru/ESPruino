void SSDP_init(void) {
  // SSDP дескриптор
  HTTP.on("/description.xml", HTTP_GET, []() {
    SSDP.schema(HTTP.client());
  });
  //Если версия  2.0.0 закаментируйте следующую строчку
  SSDP.setDeviceType("upnp:rootdevice");
  SSDP.setSchemaURL("description.xml");
  SSDP.setHTTPPort(80);
  SSDP.setName(SSDP_Name);
  SSDP.setSerialNumber("001798102201");
  SSDP.setURL("/");
  SSDP.setModelName("Power swhitcher");
  SSDP.setModelNumber("000000000102");
  SSDP.setModelURL("http://esp8266-arduinoide.ru/step3-ssdp/");
  SSDP.setManufacturer("Noritsune");
  SSDP.setManufacturerURL("noritsune.hldns.net");
  SSDP.begin();
}
