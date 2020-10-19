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
  }
}
