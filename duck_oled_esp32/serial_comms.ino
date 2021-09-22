#ifdef SERIAL_DEBUG
unsigned long serTime;

void serialEvent() {
  //reset serial input string
  serialStr = "";

  //wait a bit for whole command to come through
  serTime = millis();
  while (millis() - serTime < 100) {}

  //read serial command string
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    serialStr += inChar;
  }

  //check serial command string against known commands
  if (serialStr.startsWith("rtc:") && (serialStr.endsWith(":end"))) {
    rtc.adjust(DateTime(serialStr.substring(4, 8).toInt(),
                        serialStr.substring(9, 11).toInt(),
                        serialStr.substring(12, 14).toInt(),
                        serialStr.substring(15, 17).toInt(),
                        serialStr.substring(18, 20).toInt(),
                        serialStr.substring(21, 23).toInt()));
    Serial.println("New time set!");
  }
  else  Serial.println("Unknown command!");

  delay(1500);
}

void displaySerialOut() {
  if (digitalRead(GEAR_4))
    Serial.print("4");
  else if (digitalRead(GEAR_3))
    Serial.print("3");
  else if (digitalRead(GEAR_2))
    Serial.print("2");
  else if (digitalRead(GEAR_1))
    Serial.print("1");
  else if (digitalRead(GEAR_N))
    Serial.print("N");
  else
    Serial.print("#");
  Serial.print(" ");

  if (curTime.hour() < 10)  Serial.print("0");
  Serial.print(curTime.hour(), DEC);
  if (dots)  Serial.print(':');
  else  Serial.print(' ');
  if (curTime.minute() < 10)  Serial.print("0");
  Serial.print(curTime.minute(), DEC);
  if (dots)  Serial.print(':');
  else  Serial.print(' ');
  Serial.println(curTime.second(), DEC);

  Serial.print("M");
  Serial.print(motorTemp);
  Serial.print("\t");
  Serial.print("A");
  Serial.println(envTemp);
}
#endif
