#ifdef CAPTIVE_PORTAL
class CaptiveRequestHandler : public AsyncWebHandler {
  public:
    CaptiveRequestHandler() {}
    virtual ~CaptiveRequestHandler() {}

    bool canHandle(AsyncWebServerRequest *request) {
      //request->addInterestingHeader("ANY");
      return true;
    }

    void handleRequest(AsyncWebServerRequest *request) {
      request->send_P(200, "text/html", index_html);
    }
};
#endif

void serverSetup() {
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
#ifdef SERIAL_DEBUG
  Serial.println("AP online!");
  Serial.print("AP IP address: ");
  Serial.println(IP);
#endif
#ifdef CAPTIVE_PORTAL
#ifdef SERIAL_DEBUG
  Serial.println("Starting DNS Server");
  dnsServer.start(53, "*", WiFi.softAPIP());
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP
#endif
#endif

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
#ifdef SERIAL_DEBUG
    Serial.println("Index loading;");
#endif
    request->send_P(200, "text/html", index_html);
  });

  AsyncCallbackJsonWebHandler* JsonHandler = new AsyncCallbackJsonWebHandler("/rtc", [](AsyncWebServerRequest * request, JsonVariant & json) {
    StaticJsonDocument<200> jsonData;
    jsonData = json.as<JsonObject>();
    String response;
    serializeJson(jsonData, response);
#ifdef SERIAL_DEBUG
    Serial.println(response);
#endif
    rtc.adjust(DateTime(jsonData["Y"], jsonData["M"], jsonData["D"], jsonData["h"], jsonData["m"], jsonData["s"]));

#ifdef SERIAL_DEBUG
    Serial.println("New time set!");
#endif

    delay(100);
    request->send_P(200, "text/plain", "RTC Synced!");
  });

  server.addHandler(JsonHandler);
  AsyncElegantOTA.begin(&server);
  server.begin();
}
