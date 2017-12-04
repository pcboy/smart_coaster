#include <WiFiManager.h>
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include <CircularBuffer.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
#include "secrets.h"  // Contains the following:
// #define BLYNK_AUTH_KEY 7est7df605ab4543bf7b358bf22161bb // sample key

BlynkTimer timer;

CircularBuffer<int, 20> fsrValues;


boolean waiting = false;
int sensorPin = A0;
int beerCounter = 0;
int emptyGlassValue = 200;
int fullGlassValue = 900;

int average(CircularBuffer<int, 20> *values) {
  int res = 0;
  int i;

  for (i = 0; i < values->size(); ++i) {
    res += (*values)[i];
  }

  return res / i;
}

BLYNK_CONNECTED() {
  //get data stored in virtual pin V0 from server
  Blynk.syncVirtual(V0);
  Blynk.syncVirtual(V2);
  Blynk.syncVirtual(V3);
}

// restoring counter from server
BLYNK_WRITE(V0) {
  //restoring int value
  beerCounter = param.asInt();
  Serial.print("Restoring beerCounter to:");
  Serial.println(beerCounter);
}

BLYNK_WRITE(V1) {
  beerCounter = 0;
  Blynk.virtualWrite(V0, beerCounter);
}

BLYNK_WRITE(V2) {
  int pinValue = param.asInt();

  emptyGlassValue = pinValue;
}

BLYNK_WRITE(V3) {
  int pinValue = param.asInt();

  fullGlassValue = pinValue;
}

BLYNK_WRITE(V4) {
  ESP.deepSleep(31536e9); // 1 year
}

// To manually set the beerCounter
WidgetTerminal terminal(V5);
BLYNK_WRITE(V5) {
  int value = param.asInt();

  terminal.println(value);
  terminal.flush();
  beerCounter = value;
  Blynk.virtualWrite(V0, beerCounter);
}

void incrementGlassCounter() {
  beerCounter++;
  Blynk.virtualWrite(V0, beerCounter);
}

void myLoop() {
  int fsrReading = analogRead(sensorPin);

  Serial.print("FSRReading: ");
  Serial.println(fsrReading);

  fsrValues.push(fsrReading);
  int avg = average(&fsrValues);
  Serial.print("AVERAGE: ");
  Serial.println(avg);

  if (waiting == false && avg < emptyGlassValue) {
    Serial.println("Beer finished. Waiting for new one.");
    waiting = true;
  } else if (waiting == true && avg >= fullGlassValue) {
    Serial.println("New beer");
    incrementGlassCounter();
    waiting = false;
  }
}


void setup() {
  Serial.begin(9600);

  WiFiManager wifiManager;
  wifiManager.autoConnect("SMART_COASTER");

  Serial.println("local ip");
  Serial.println(WiFi.localIP());

  Blynk.config(BLYNK_AUTH_KEY);
  timer.setInterval(1000L, myLoop);
}


void loop() {
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
}
