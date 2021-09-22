void getMotorTemp() {
  average = 0;
  for (uint8_t i = 0; i < NUMSAMPLES; i++) {
    samples[i] = analogRead(MOTOR_THERM);
    average += samples[i];
    delay(10);
  }
  average /= NUMSAMPLES;
  average += 50;

  // convert the value to resistance
  res_reading = 1023 / average - 1;
  res_reading = MOTOR_S_RES / res_reading;

  motorTemp = res_reading / MOTOR_NOMINAL;          // (R/Ro)
  motorTemp = log(motorTemp);                       // ln(R/Ro)
  motorTemp /= MOTOR_B;                             // 1/B * ln(R/Ro)
  motorTemp += 1.0 / (MOTOR_TEMP_NOMINAL + 273.15); // + (1/To)
  motorTemp = 1.0 / motorTemp;                      // Invert
  motorTemp -= 273.15;                              // convert absolute temp to C
}

void getAmbientTemp() {
  ds.requestTemperatures();
  envTemp = ds.getTempCByIndex(0);
}
