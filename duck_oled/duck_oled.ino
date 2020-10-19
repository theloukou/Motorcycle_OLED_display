#define SHOW_LOGOS

#include <DallasTemperature.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>

RTC_PCF8523 rtc;
DateTime curTime;

#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress ambientSensor = { 0x28, 0xF8, 0x7C, 0xEA, 0x9, 0x0, 0x0, 0x5D };
DeviceAddress motorSensor   = { 0x28, 0x2C, 0x6C, 0x63, 0x32, 0x14, 0x1, 0xE5 };

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

char gear;
int motorTemp = 60;
int envTemp = 23;
unsigned long lastTemp = 0;
bool led = false;
String serialStr;

#ifdef SHOW_LOGOS
const unsigned char kawasaki [] PROGMEM = {
  0xfc, 0x0f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x3f,
  0xfe, 0x1f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x3f,
  0xfe, 0x3f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x3f,
  0xfe, 0x7f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x3f,
  0xfe, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00,
  0xff, 0xfe, 0x00, 0xff, 0x1e, 0x07, 0x83, 0xe3, 0xfc, 0x03, 0xf0, 0x07, 0xf8, 0x7e, 0x1f, 0x80,
  0xff, 0xfc, 0x03, 0xff, 0xff, 0x0f, 0xc7, 0xef, 0xff, 0x0f, 0xfc, 0x1f, 0xfe, 0x7e, 0x3f, 0xbf,
  0xff, 0xf8, 0x07, 0xff, 0xff, 0x8f, 0xc7, 0xff, 0xff, 0x9f, 0xfe, 0x3f, 0xff, 0x7e, 0x7f, 0x3f,
  0xff, 0xf8, 0x0f, 0xff, 0xff, 0x9f, 0xc7, 0xff, 0xff, 0xbf, 0xff, 0x7f, 0xff, 0x7e, 0xfe, 0x3f,
  0xff, 0xfc, 0x0f, 0xc7, 0xff, 0x9f, 0xcf, 0xff, 0x1f, 0xbf, 0x1f, 0x7e, 0x3f, 0x7f, 0xfc, 0x3f,
  0xff, 0xfe, 0x00, 0x07, 0xef, 0x9f, 0xef, 0xc0, 0x1f, 0xbf, 0xc0, 0x00, 0x3f, 0x7f, 0xf8, 0x3f,
  0xff, 0xfe, 0x01, 0xff, 0xef, 0x9f, 0xef, 0x87, 0xff, 0xbf, 0xfc, 0x0f, 0xff, 0x7f, 0xf8, 0x3f,
  0xff, 0xff, 0x07, 0xff, 0xe7, 0xff, 0xef, 0x9f, 0xff, 0x9f, 0xff, 0x3f, 0xff, 0x7f, 0xf8, 0x3f,
  0xfe, 0x7f, 0x8f, 0xff, 0xe7, 0xfd, 0xff, 0xbf, 0xff, 0x8f, 0xff, 0x7f, 0xff, 0x7f, 0xfc, 0x3f,
  0xfe, 0x7f, 0x9f, 0xe7, 0xe7, 0xfd, 0xff, 0x7f, 0x9f, 0x81, 0xff, 0xff, 0x3f, 0x7f, 0xfe, 0x3f,
  0xfe, 0x3f, 0xdf, 0x87, 0xe7, 0xfd, 0xff, 0x7e, 0x1f, 0xfe, 0x3f, 0xfe, 0x3f, 0x7e, 0x7e, 0x3f,
  0xfe, 0x3f, 0xdf, 0xcf, 0xe3, 0xfc, 0xff, 0x7f, 0x3f, 0xff, 0x3f, 0xfe, 0x7f, 0x7e, 0x7f, 0x3f,
  0xfe, 0x1f, 0xff, 0xff, 0xe3, 0xf8, 0xfe, 0x7f, 0xff, 0xbf, 0xff, 0x7f, 0xff, 0x7e, 0x3f, 0xbf,
  0xfe, 0x0f, 0xff, 0xff, 0xf3, 0xf8, 0xfe, 0x3f, 0xff, 0x9f, 0xfe, 0x7f, 0xff, 0xfe, 0x3f, 0xbf,
  0xfe, 0x0f, 0xf7, 0xf3, 0xf1, 0xf8, 0x7e, 0x1f, 0xcf, 0xcf, 0xfc, 0x1f, 0x9f, 0xfe, 0x1f, 0xff
};

const unsigned char kazer [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x01, 0xff, 0xe0,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe3, 0x00, 0x00, 0x3f, 0xff, 0xfe,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x03, 0xf0, 0x03, 0xff, 0x81, 0xff,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x1c, 0x1f, 0x83, 0xff, 0xff,
  0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0xdd, 0x81, 0x90, 0x00, 0x0e, 0x38, 0xff, 0xff, 0xff,
  0x00, 0x01, 0x80, 0x00, 0x00, 0x60, 0x18, 0x08, 0x00, 0xc0, 0x00, 0x1c, 0x4f, 0xff, 0xff, 0xfb,
  0x00, 0x03, 0x00, 0xff, 0x61, 0x80, 0x08, 0x00, 0x00, 0x60, 0x00, 0x7c, 0x9f, 0xde, 0xff, 0xf7,
  0x00, 0x06, 0x01, 0xf0, 0x23, 0x00, 0x08, 0x0f, 0x00, 0xe0, 0xff, 0xf0, 0x3f, 0xbe, 0xff, 0xef,
  0x00, 0x1e, 0x03, 0x00, 0x34, 0x00, 0x30, 0x3e, 0x00, 0xc1, 0x7f, 0xc0, 0x7f, 0x78, 0xff, 0xdf,
  0x00, 0x3c, 0x04, 0x00, 0x70, 0x18, 0x70, 0x70, 0x03, 0xc0, 0x08, 0x06, 0xfe, 0xe3, 0xff, 0x7e,
  0x00, 0xf8, 0x00, 0x00, 0xe0, 0x70, 0xf3, 0xe0, 0x3f, 0x80, 0x1c, 0x0d, 0xfd, 0xcf, 0xfd, 0xf8,
  0x00, 0xe0, 0x00, 0x41, 0x80, 0xc1, 0xff, 0x81, 0xfc, 0x06, 0x7c, 0x79, 0xff, 0x9f, 0xef, 0xe0,
  0x01, 0xc0, 0x0f, 0xcf, 0x00, 0x23, 0xfe, 0x0f, 0xfe, 0x1f, 0xff, 0xe3, 0xf6, 0x7f, 0xff, 0x80,
  0x01, 0x80, 0x3f, 0xbc, 0x00, 0x67, 0xf0, 0x3f, 0xfe, 0x3f, 0xff, 0x1f, 0xe7, 0xff, 0xfe, 0x00,
  0x03, 0x80, 0xff, 0xf8, 0x01, 0xc7, 0xc1, 0xff, 0x06, 0x3f, 0x80, 0x1f, 0xff, 0xc7, 0xf8, 0x00,
  0x07, 0x00, 0x7f, 0xf0, 0x31, 0x8f, 0x83, 0xfc, 0x04, 0x10, 0x00, 0x3f, 0xff, 0xff, 0xe0, 0x00,
  0x06, 0x60, 0x1f, 0xc0, 0x78, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x7b, 0xff, 0xff, 0xfc, 0xf8, 0x00,
  0x0c, 0x86, 0x07, 0x80, 0x78, 0x3c, 0x00, 0x1c, 0x18, 0x03, 0xff, 0xff, 0x5f, 0xff, 0x9f, 0x80,
  0x1b, 0x0f, 0x80, 0x00, 0x78, 0x3c, 0x00, 0x78, 0xf8, 0x0f, 0xff, 0xfe, 0xfb, 0xff, 0xe7, 0xc0,
  0x35, 0x8f, 0xf0, 0x00, 0xf8, 0x1f, 0x0f, 0xff, 0xfe, 0xff, 0xcd, 0xfd, 0xff, 0x7f, 0xff, 0xf8,
  0x60, 0x1f, 0xf8, 0x03, 0xf8, 0x1f, 0x9f, 0xff, 0xff, 0xfe, 0x1b, 0xfb, 0xff, 0xcf, 0xfc, 0x7c,
  0xfd, 0xbf, 0xfe, 0x03, 0xfe, 0xff, 0xff, 0xff, 0x07, 0xec, 0x37, 0xf7, 0xff, 0xfb, 0xff, 0xfe,
  0x3f, 0xfc, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x80, 0x6f, 0xef, 0xff, 0xff, 0xff, 0xfe,
  0x0f, 0xf8, 0x07, 0xff, 0xc3, 0xfc, 0xfe, 0x00, 0x00, 0x00, 0xdf, 0xdf, 0xfe, 0x1f, 0xff, 0xfe,
  0x03, 0xf0, 0x03, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0xbf, 0xbf, 0xf0, 0x00, 0xf8, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x3d, 0x7f, 0x80, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x7f, 0xff, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00
};
#endif

void setup() {
  //reserve memory for variable length serial commands
  serialStr.reserve(40);
  Wire.setClock(400000);

  pinMode(5, INPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  pinMode(13, OUTPUT);

  digitalWrite(13, led);

  Serial.begin(115200);

  sensors.begin();

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  display.setRotation(2);
  display.clearDisplay();

#ifdef SHOW_LOGOS
  display.drawBitmap(0, 5, kawasaki, 128, 20, WHITE);
  display.display();
  delay(1500);

  display.clearDisplay();
  display.display();
  delay(100);

  display.clearDisplay();
  display.drawBitmap(0, 1, kazer, 128, 30, WHITE);
  display.display();
  delay(1500);

  display.clearDisplay();
  display.display();
  delay(100);
#endif

  display.setTextColor(WHITE, BLACK);
  display.setTextWrap(false);

  displayGearFet();
  diplayTemps();
  displayClock();

  Serial.println("Available commands:\n\trtc:YYYY MM DD hh mm ss:end");
  delay(1000);
  Serial.println("Setup done!\n");
}

void loop() {
  if (millis() - lastTemp >= 500) {
    displayGearFet();
    diplayTemps();
    displayClock();
    lastTemp = millis();
    led = !led;
    digitalWrite(13, led);

    serialOut();

    display.display();
  }
}

void displayGearFet() {
  display.setCursor(0, 0);
  display.setTextSize(4);
  if (digitalRead(9))
    display.print("4");
  else if (digitalRead(8))
    display.print("3");
  else if (digitalRead(7))
    display.print("2");
  else if (digitalRead(6))
    display.print("1");
  else if (digitalRead(5))
    display.print("N");
  else
    display.print(" ");
}

void diplayTemps() {
  sensors.requestTemperatures();

  motorTemp = sensors.getTempC(motorSensor);
  envTemp = sensors.getTempC(ambientSensor);

  display.setCursor(27, 16);
  display.print("         ");
  display.setTextSize(2);
  display.setCursor(27, 16);
  display.print("A");
  display.print(motorTemp);
  display.setCursor(82, 16);
  display.print("S");
  display.print(envTemp);
}

void displayClock() {
  display.setCursor(48, 0);

  curTime = rtc.now();

  if (curTime.hour() < 10)  display.print("0");
  display.print(curTime.hour(), DEC);
  if (led)  display.print(':');
  else  display.print(' ');
  if (curTime.minute() < 10)  display.print("0");
  display.print(curTime.minute(), DEC);
}

void serialOut() {
  if (digitalRead(9))
    Serial.print("4");
  else if (digitalRead(8))
    Serial.print("3");
  else if (digitalRead(7))
    Serial.print("2");
  else if (digitalRead(6))
    Serial.print("1");
  else if (digitalRead(5))
    Serial.print("N");
  else
    Serial.print(" ");
  Serial.print(" ");

  if (curTime.hour() < 10)  Serial.print("0");
  Serial.print(curTime.hour(), DEC);
  if (led)  Serial.print(':');
  else  Serial.print(' ');
  if (curTime.minute() < 10)  Serial.print("0");
  Serial.print(curTime.minute(), DEC);
  if (led)  Serial.print(':');
  else  Serial.print(' ');
  Serial.println(curTime.second(), DEC);

  Serial.print("A");
  Serial.print(motorTemp);
  Serial.print("\t");
  Serial.print("S");
  Serial.println(envTemp);
}