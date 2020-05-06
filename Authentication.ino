void handleLogin() {
  String msg;
  if (HTTP.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = HTTP.header("Cookie");
    Serial.println(cookie);
  }
  if (HTTP.hasArg("DISCONNECT")) {
    Serial.println("Disconnection");
    HTTP.sendHeader("Location", "/login");
    HTTP.sendHeader("Cache-Control", "no-cache");
    HTTP.sendHeader("Set-Cookie", "ESPSESSIONID=0");
    HTTP.send(301);
    return;
  }
  Serial.println(HTTP.hasArg("USERNAME")+"  "+HTTP.hasArg("PASSWORD"));
  if (HTTP.hasArg("USERNAME") && HTTP.hasArg("PASSWORD")) {
    if (HTTP.arg("USERNAME") == "admin" &&  HTTP.arg("PASSWORD") == _passwordAP) {
      HTTP.sendHeader("Location", "/");
      HTTP.sendHeader("Cache-Control", "no-cache");
      HTTP.sendHeader("Set-Cookie", encoded);
      HTTP.send(301);
      Serial.println("Log in Successful");
      return;
    }
    msg = "Wrong username/password! try again.";
    Serial.println("Log in Failed");
  }
    File file = SPIFFS.open("/auth.htm", "r");
    size_t sent = HTTP.streamFile(file, "text/html");
    file.close();
  //String content = "<html><body><form action='/login' method='POST'>To log in, please use : admin/admin<br>";
  //content += "User:<input type='text' name='USERNAME' placeholder='user name'><br>";
  //content += "Password:<input type='password' name='PASSWORD' placeholder='password'><br>";
  //content += "<input type='submit' name='SUBMIT' value='Submit'></form>" + msg + "<br>";
  //content += "You also can go <a href='/inline'>here</a></body></html>";
  //HTTP.send(200, "text/html", content);
}

void handleRoot() {
  Serial.println("Enter handleRoot");
  String header;
  if (!is_authenticated() && !WIFI_AP_on) {
    HTTP.sendHeader("Location", "/login");
    HTTP.sendHeader("Cache-Control", "no-cache");
    HTTP.send(301);
    return;
  }
    HTTP.sendHeader("Location", "/fish_feeding.htm");
    HTTP.sendHeader("Cache-Control", "no-cache");
    HTTP.send(301);
}

bool authCheck(){
    if(WIFI_AP_on) return true;
      if (!is_authenticated()) {
      HTTP.sendHeader("Location", "/login");
      HTTP.sendHeader("Cache-Control", "no-cache");
      HTTP.send(301);
      return false;
    }
    return true;
}
