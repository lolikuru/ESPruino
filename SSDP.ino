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
  SSDP.setSerialNumber("001798102202");
  SSDP.setURL("/");
  SSDP.setModelName("Fish feeder");
  SSDP.setModelNumber("000000000102");
  SSDP.setModelURL("https://github.com/lolikuru/ESPruino/tree/fish_feeder");
  SSDP.setManufacturer("lolikuru");
  SSDP.setManufacturerURL("");
  SSDP.begin();
}
