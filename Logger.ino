// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"
#include <SPI.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

const int chipSelect = 10;

RTC_PCF8523 rtc;
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

String date;
float ozone;
float light;
float UV;
float pressure;
float temperature;
float altitude;
boolean headerCreated = false;

void setup () {

  while (!Serial); // for Leonardo/Micro/Zero

  Serial.begin(57600);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.initialized()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    
    Serial.println("Card failed, or not present");

    while (true) {

      blinkLED(250, 250, 3);
      delay(500);
    }

    return;
  }
  
  Serial.println("card initialized.");

  Serial.println("Pressure Sensor Test"); Serial.println("");
  
  if (!bmp.begin()) {
    
    Serial.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
    
     while (true) {

      blinkLED(250, 250, 5);
      delay(500);
    }
  }

  Serial.println("Setting pin modes");
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(13, OUTPUT);
}

void loop () {

    Serial.println("Starting loop");
    File dataFile = SD.open("datalog.txt", FILE_WRITE);
    Serial.println("File opened");

    logHeader(dataFile);

    date = getDate();
    ozone = getOzone();
    light = getLight();
    UV = getUV();

    sensors_event_t event;
    bmp.getEvent(&event);
 
    if (event.pressure) {
 
      pressure = getPressure(event);
      temperature = getTemperature();
      altitude = getAltitude(event);
      
    } else {
      
      dataFile.println("Sensor error");
    }

    logData(dataFile);
    
    dataFile.close();

    blinkLED(2500, 2500, 1);
}

void logHeader(File dataFile) {

    if (!headerCreated) {

        dataFile.println("Date, Ozone, Light, UV, Pressure, Temperature, Altitude");
        headerCreated = true;
    }
}

String getDate() {

    DateTime now = rtc.now();

    String date = "";

    date += now.month();
    date += '/';
    date += now.day();
    date += '/';
    date += now.year();
    date += ' ';
    date += now.hour();
    date += ':';
    date += now.minute();
    date += ':';
    date += now.second();

    return date;
}

float getOzone() {

    return analogRead(A0);
}

float getLight() {

    return analogRead(A1);
}

float getUV() {

    return analogRead(A2);
}

float getPressure(sensors_event_t event) {

    return event.pressure;
}

float getTemperature() {

    float temperature;
    bmp.getTemperature(&temperature);
    return temperature;
}

float getAltitude(sensors_event_t event) {

    float temperature;
    bmp.getTemperature(&temperature);
    float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;
    return bmp.pressureToAltitude(seaLevelPressure, event.pressure, temperature); 
}

void logData(File dataFile) {

    dataFile.print(date);
    dataFile.print(", ");
    dataFile.print(ozone);
    dataFile.print(", ");
    dataFile.print(light);
    dataFile.print(", ");
    dataFile.print(UV);
    dataFile.print(", ");
    dataFile.print(pressure);
    dataFile.print(", ");
    dataFile.print(temperature);
    dataFile.print(", ");
    dataFile.println(altitude);
}

void LEDOn(int timeOn) {

    digitalWrite(13, HIGH);
    delay(timeOn);
}

void LEDOff(int timeOff) {

    digitalWrite(13, LOW);
    delay(timeOff);
}

void blinkLED(int timeOn, int timeOff, int numberOfBlinks) {

  for (int blinkCount = 0 ; blinkCount < numberOfBlinks ; blinkCount++) {

    LEDOn(timeOn);
    LEDOff(timeOff);
  }
}

