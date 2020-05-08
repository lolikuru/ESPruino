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
  SSDP.setSerialNumber("001798102302");
  SSDP.setURL("/");
  SSDP.setModelName("white control");
  SSDP.setModelNumber("000000000300");
  SSDP.setModelURL("https://github.com/lolikuru/ESPruino/tree/white_control");
  SSDP.setManufacturer("lolikuru");
  SSDP.setManufacturerURL("");
  SSDP.begin();
}
