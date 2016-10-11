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
    
    return;
  }
  
  Serial.println("card initialized.");

  Serial.println("Pressure Sensor Test"); Serial.println("");
  
  if(!bmp.begin()) {
    
    Serial.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
    while(1);
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

    DateTime now = rtc.now();

    dataFile.print(now.year(), DEC);
    dataFile.print('/');
    dataFile.print(now.month(), DEC);
    dataFile.print('/');
    dataFile.print(now.day(), DEC);
    dataFile.print(" (");
    dataFile.print(daysOfTheWeek[now.dayOfTheWeek()]);
    dataFile.print(") ");
    dataFile.print(now.hour(), DEC);
    dataFile.print(':');
    dataFile.print(now.minute(), DEC);
    dataFile.print(':');
    dataFile.print(now.second(), DEC);
    dataFile.println();

    dataFile.println("Ozone: ");
    dataFile.print(analogRead(A0));
    dataFile.print(" ppm");
    dataFile.println("");

    dataFile.println("Light: ");
    dataFile.print(analogRead(A1));
    dataFile.println("");

    dataFile.println("UV: ");
    dataFile.print(analogRead(A2));
    dataFile.println("");

    sensors_event_t event;
    bmp.getEvent(&event);
 
    if (event.pressure) {
    
      dataFile.println("Pressure: ");
      dataFile.print(event.pressure);
      dataFile.print(" hPa");
      dataFile.println("");
      float temperature;
      bmp.getTemperature(&temperature);
      dataFile.println("Temperature: ");
      dataFile.print(temperature);
      dataFile.print(" C");
      dataFile.println("");
 
      float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;
      dataFile.println("Altitude: "); 
      dataFile.print(bmp.pressureToAltitude(seaLevelPressure, event.pressure, temperature)); 
      dataFile.println(" m");
      dataFile.println("");
      
    } else {
      
      dataFile.println("Sensor error");
    }
    
    dataFile.close();

    digitalWrite(13, HIGH);
    delay(2500);
    digitalWrite(13, LOW);
    delay(2500);
}
