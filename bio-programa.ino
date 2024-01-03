/*
MIT License

Copyright (c) 2023 Mindaugas Leonas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "DHT.h"
#include "RTClib.h"
#include <SPI.h>
#include <SD.h>

#define controlDHTPIN 5
#define experimentDHTPIN 6
#define statusLED 3
#define confirmLED 4
#define errorLED 2
#define button 8
#define buzzer 7
#define chipSelect 10
#define controlSoil A0
#define experimentSoil A1
#define DHTTYPE DHT22
#define filename "datalog.txt"
#define filetype FILE_WRITE


DHT controlDHT(controlDHTPIN, DHTTYPE);
DHT experimentDHT(experimentDHTPIN, DHTTYPE);
RTC_DS1307 rtc;

File logfile;

void setup() {
  pinMode(statusLED, OUTPUT);
  pinMode(confirmLED, OUTPUT);
  pinMode(errorLED, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(button, INPUT);

  pulseAllLED(1000);
  tone(buzzer, 500, 100);
  pulse(statusLED);
  Serial.begin(9600);

  Serial.println("Initializing SD card");
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    halt();
  }
  logfile = SD.open(filename, filetype);
  if(!logfile){
    Serial.println("Log start failed");
    halt();
  } else {
    Serial.println("Log start");
    confirm();
  }

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    halt();
  } else {
    sendMessage("RTC Start Success");
    confirm();
  }

  sendMessage("Time configuration");
  if (!rtc.isrunning()) {
    sendMessage("RTC is NOT running, inserting current date and time");
    pulse(statusLED);
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    confirm();
  } else {
    sendMessage("Time OK");
    confirm();
  }

  sendMessage("Testing control DHT");
  controlDHT.begin();
  pulse(statusLED);

  float controlHum = controlDHT.readHumidity();
  if (isnan(controlHum)) {
    sendMessage("Control DHT sensor init fail");
    halt();
  } else {
    sendMessage("Success");
    confirm();
  }

  sendMessage("Testing experiment DHT");
  experimentDHT.begin();
  pulse(statusLED);

  float experimentHum = experimentDHT.readHumidity();
  if (isnan(experimentHum)) {
    sendMessage("Experiment DHT sensor init fail");
    halt();
  } else {
    sendMessage("Success");
    confirm();
  }

  digitalWrite(confirmLED, HIGH);
  sendMessage("Switching to loop/logging void func");
}

void pulse(int pin) {
  digitalWrite(pin, HIGH);
  delay(1);
  digitalWrite(pin, LOW);
  delay(1);
}

void sendMessage(String text) {
  DateTime time = rtc.now();
  Serial.println(String("[" + time.timestamp(DateTime::TIMESTAMP_DATE) + " " + time.timestamp(DateTime::TIMESTAMP_TIME) + "] " + text));
  logfile = SD.open(filename, filetype);
  if(!logfile){
    Serial.println("Log failed");
    halt();
  } else {
    pulse(confirmLED);
  }
  logfile.println(String("[" + time.timestamp(DateTime::TIMESTAMP_DATE) + " " + time.timestamp(DateTime::TIMESTAMP_TIME) + "] " + text));
  logfile.close();
}

void confirm() {
  pulse(statusLED);
  pulse(confirmLED);
  tone(buzzer, 1000, 100);
}

void halt() {
  digitalWrite(errorLED, HIGH);
  tone(buzzer, 200, 1000);
  while (true);
}

void pulseAllLED(int time) {
  pinMode(statusLED, OUTPUT);
  pinMode(confirmLED, OUTPUT);
  pinMode(errorLED, OUTPUT);

  digitalWrite(statusLED, HIGH);
  digitalWrite(errorLED, HIGH);
  digitalWrite(confirmLED, HIGH);
  delay(time);
  digitalWrite(statusLED, LOW);
  digitalWrite(errorLED, LOW);
  digitalWrite(confirmLED, LOW);
  delay(time);
}

void loop() {
  float controlHum = controlDHT.readHumidity();
  pulse(statusLED);
  delay(1000);
  float controlTemp = controlDHT.readTemperature();
  pulse(statusLED);
  delay(1000);
  float experimentHum = experimentDHT.readHumidity();
  pulse(statusLED);
  delay(1000);
  float experimentTemp = experimentDHT.readTemperature();
  pulse(statusLED);
  delay(1000);
  int controlSW = analogRead(controlSoil);
  pulse(statusLED);
  int experimentSW = analogRead(experimentSoil);
  pulse(statusLED);

  if (isnan(controlHum) || isnan(experimentHum) || isnan(controlTemp) || isnan(experimentTemp)) halt();

  sendMessage("CH " + String(controlHum));
  sendMessage("CT " + String(controlTemp));
  sendMessage("CS " + String(controlSW));
  sendMessage("ET " + String(experimentTemp));
  sendMessage("EH " + String(experimentHum));
  sendMessage("ES " + String(experimentSW));
  pulse(confirmLED);
  digitalWrite(statusLED, HIGH);
  delay(6000);
  digitalWrite(statusLED, LOW);
}