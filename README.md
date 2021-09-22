# Motorcycle information display
ESP32-based Motorcycle Information Display

This is an ESP32 based system, intented for older motorcycles. It is an expansion of the AVR implementation found in the main_avr branch.
It utilizes a small 128x32 OLED diplay module to display motor and ambient temperatures from a set of NTC thermistor/DS18B20, as well as time information through an RTC IC.

New features compared to the Arduino/AVR version are:
* Syncing RTC time via onboard web server
* Uploading new firmware via Wifi, without need to remove system/connect to PC
* Custom PCB to minimize system footprint. Choice between pin-compatible DS1307/PCF8523
