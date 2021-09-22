#include "definitions.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include "AsyncJson.h"
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#ifdef CAPTIVE_PORTAL
#include <DNSServer.h>
#endif

const char* ssid = "Duck_WiFi";
const char* password = "problima?";
#ifdef CAPTIVE_PORTAL
DNSServer dnsServer;
#endif
AsyncWebServer server(80);
String header;

RTC_PCF8523 rtc;
DateTime curTime;

OneWire oneWire(ONE_WIRE);
DallasTemperature ds(&oneWire);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

char gear;
float motorTemp = 60;
float envTemp = 23;
unsigned long lastTemp = 0;
bool dots = false;
String serialStr;

uint16_t samples[NUMSAMPLES];
float res_reading, average = 0;

void setup() {
  btStop(); // Turn off bluetooth
#ifdef SERIAL_DEBUG
  Serial.begin(115200);
  Serial.println("Booting");
#endif
  delay(2000);
  serverSetup();

#ifdef SERIAL_DEBUG
  Serial.println("Starting RTC");
#endif
  rtc.start();

  //reserve memory for variable length serial commands
  serialStr.reserve(40);
  Wire.setClock(I2C_FREQ);

  analogReadResolution(10);

  pinMode(GEAR_N, INPUT_PULLDOWN);
  pinMode(GEAR_1, INPUT_PULLDOWN);
  pinMode(GEAR_2, INPUT_PULLDOWN);
  pinMode(GEAR_3, INPUT_PULLDOWN);
  pinMode(GEAR_4, INPUT_PULLDOWN);

  ds.begin();

#ifdef SERIAL_DEBUG
  Serial.println("Starting display");
#endif
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  display.setRotation(2);
  display.clearDisplay();

#ifdef SHOW_LOGOS
  displayBootSeq();
#endif

  display.setTextColor(WHITE, BLACK);
  display.setTextWrap(false);

  curTime = rtc.now();
  displayGearFet();
  diplayTemps();
  displayClock();

#ifdef SERIAL_DEBUG
  Serial.println("Available commands:\n\trtc:YYYY MM DD hh mm ss:end");
  delay(1000);
  Serial.println("Setup done!\n");
#endif
}

void loop() {
  AsyncElegantOTA.loop();
#ifdef CAPTIVE_PORTAL
  dnsServer.processNextRequest();
#endif
#ifdef SERIAL_DEBUG
  if (Serial.available()) serialEvent();
#endif

  if (millis() - lastTemp >= 500) {
    getMotorTemp();
    getAmbientTemp();
    curTime = rtc.now();

    i2cfree();
    display.clearDisplay();
    display.display();
    displayGearFet();
    diplayTemps();
    displayClock();
    lastTemp = millis();
    dots = !dots;

#ifdef SERIAL_DEBUG
    displaySerialOut();
    Serial.printf("Avrg/Th.res/Volts: %f %f %f\r\n", average, res_reading, average / 1024 * 3.3);
#endif

    display.display();
  }
}

void i2cfree() {
  uint16_t halftime = ((1.0 / I2C_FREQ) / 2.0)*1000000; //half period in microseconds
  for (int times = 0; times < 10; times++) {
    digitalWrite(22, LOW);
    delayMicroseconds(halftime);
    digitalWrite(22, HIGH);
    delayMicroseconds(halftime);
  }
}
