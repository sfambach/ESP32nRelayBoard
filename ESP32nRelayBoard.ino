/******************************************************************************
* Test for relay boards 
* The test programm switch all relays one after another to on. After that all will be switched off sequentially. 
* Libs:
* None specific
*
* Licence: AGPL3
* Author: S. Fambach
* Visit http://www.fambach.net if you want
******************************************************************************/
#include <Arduino.h>

/** Debuging *****************************************************************/
#define DEBUG
#ifdef DEBUG
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINT(x) Serial.print(x)
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#endif


/** Wifi *****************************************************************/

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#elif defined ESP32
#include <WiFi.h>
#include <ESPmDNS.h>
#else
#error Unknown controller
#endif

#ifndef SSID
#define SSID "NODES"
#define SSID_PASSWD "HappyNodes1234"
#endif

/** OTA *****************************************************************/
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

void setupOTA() {

  // Port defaults to 8266 / 3232
#ifdef ESP8266
  //ArduinoOTA.setPort(8266);
#elif defined ESP32
  //ArduinoOTA.setPort(3232);
#endif
  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("NewOTAClient");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("ef84180df45cf2f22993e3a03dc71a27");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


/** Relay *****************************************************************/
//#define ONE_RELAY
//#define TWO_RELAY
#define FOUR_RELAY
//#define EIGHT_RELAY


#ifdef ONE_RELAY

const uint8_t RELAY_COUNT = 1;
uint8_t relays[RELAY_COUNT] = { 16 };
const uint8_t LED = 23;

#elif defined TWO_RELAY
const uint8_t RELAY_COUNT = 2;
uint8_t relays[RELAY_COUNT] = { GPIO16 };
const uint8_t LED = 23;

#elif defined FOUR_RELAY
const uint8_t RELAY_COUNT = 4;
uint8_t relays[RELAY_COUNT] = { 32, 33, 25, 26 };
//uint8_t relays[RELAY_COUNT] = { GPIO32, GPIO33, GPIO25, GPIO26 };
const uint8_t LED = 23;

#elif defined EIGHT_RELAY
const uint8_t RELAY_COUNT = 8;
uint8_t relays[RELAY_COUNT] = {};
const uint8_t LED = GPIO23;

#endif


void setupRelays() {
  for (int i = 0; i < RELAY_COUNT; i++) {
    pinMode(relays[i], OUTPUT);
    digitalWrite(relays[i], LOW);
  }
}

long currMillis = 0;
long delayMS = 1000;
uint8_t counter = 0;
bool state = HIGH;

void testRelays() {

  long remaining = millis() - (currMillis + delayMS);

  if(remaining >= 0) {
    counter ++;
    currMillis = millis();
    if(counter == RELAY_COUNT){
      counter = 0; 
      state = !state;
    }   

    DEBUG_PRINT("relay: ");
    DEBUG_PRINT(counter);
    DEBUG_PRINT(" state : ");
    DEBUG_PRINTLN((state?"HIGH": "LOW"));
    digitalWrite(relays[counter], state); 
  }

}

/** XXXXXXXX *****************************************************************/
/** XXXXXXXX *****************************************************************/


/** Main Programm ************************************************************/
void setup() {
  Serial.begin(115200);
  DEBUG_PRINTLN("Setup");

  // wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, SSID_PASSWD);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // do the ota
  setupOTA();

  // relays
  setupRelays();
}


void loop() {
  //DEBUG_PRINTLN("Main Loop");

  // handle ota
  ArduinoOTA.handle();

  testRelays();
}


/** Rest of implementations **************************************************/