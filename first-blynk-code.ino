/*
* Sketch written by Dean Etim
* Last Edited 6th April, 2025.
*/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "EmonLib.h"

#define BLYNK_PRINT Serial
#define vCalibration 83.3
#define currCalibration 5.6

LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C address 0x27 or 0x3F depending on your module
EnergyMonitor emon;
BlynkTimer timer;

char auth[] = "AUTHENTICATION_CODE";
char ssid[] = "Free WIFI";
char pass[] = "@dean2646EE";

float energyConsumed = 0.0;
unsigned long lastmillis = millis();
const int voltagePin = 35;
const int currentPin = 34;

void myTimerEvent() {
  // Sample the voltage and current waveforms
  emon.calcVI(50, 300);  //20 cycles, 2000ms
  float voltage = emon.Vrms;
  float current = emon.Irms;
  float realPower = emon.realPower;
  float powerFactor = emon.powerFactor;

  energyConsumed += realPower * (millis() - lastmillis) / 3600000000.0;
  lastmillis = millis();

  // RMS_VOLTAGE
  Serial.print("Vrms: ");
  Serial.print(voltage, 2);
  Serial.print("V");

  // RMS_CURRENT
  Serial.print("\tIrms: ");
  Serial.print(current, 2);
  Serial.print("A");

  // REAL_POWER
  Serial.print("\tPower: ");
  Serial.print(realPower, 3);
  Serial.print("W");

  // POWER_FACTOR
  Serial.print("\tPower factor: ");
  Serial.print(powerFactor, 2);

  // ENERGY
  Serial.print("\tEnergy: ");
  Serial.print(energyConsumed, 2);
  Serial.println("kWh");

  // Display Voltage and Current values on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Vrms:");
  lcd.print(voltage, 2);
  lcd.print("V");
  lcd.setCursor(0, 1);
  lcd.print("Irms:");
  lcd.print(current, 4);
  lcd.print("A");
  delay(2500);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Power:");
  lcd.print(realPower, 4);
  lcd.print("W");
  lcd.setCursor(0, 1);
  lcd.print("Energy:");
  lcd.print(energyConsumed, 4);
  lcd.print("kWh");
  delay(2500);

  // Send the values to the Blynk app
  Blynk.virtualWrite(V0, voltage);
  Blynk.virtualWrite(V1, current);
  Blynk.virtualWrite(V2, realPower);
  Blynk.virtualWrite(V3, energyConsumed);
}

void setup() {
  Serial.begin(115200);           //initialize serial monitor
  Blynk.begin(auth, ssid, pass);  //Initialze Blynk

  // initialize LCD
  lcd.init();
  lcd.backlight();

  // Initialize read voltage and current values
  emon.voltage(voltagePin, vCalibration, 1.7);
  emon.current(currentPin, currCalibration);

  //setup the blynk timer function
  timer.setInterval(5000L, myTimerEvent);

  // Display Welcome message on the LCD
  lcd.setCursor(3, 0);
  lcd.print("Hello there! ");
  lcd.setCursor(3, 1);
  lcd.print("Wattmeter Initializing");
  delay(3000);
  lcd.clear();
}  // End setup

void loop() {
  Blynk.run();
  timer.run();
}  // End main loop
