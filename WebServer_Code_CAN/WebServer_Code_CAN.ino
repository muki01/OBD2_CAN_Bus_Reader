#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#include <SPIFFS.h>
#include <Update.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <FS.h>
#endif
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "PIDs.h"
#include "driver/twai.h"

#define TX_GPIO_NUM GPIO_NUM_9                   // CAN TX pin
#define RX_GPIO_NUM GPIO_NUM_10                  // CAN RX pin
#define CAN_SPEED TWAI_TIMING_CONFIG_500KBITS()  // CAN SPEED 125KBITS, 250KBITS, 500KBITS or 1MBITS
#define CAN_BIT 29

JsonDocument jsonDoc;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

#ifdef ESP32
#define Led 48
#define Buzzer 6
#define voltagePin 8
#define DEBUG_Serial
#elif defined(ESP8266)
#define K_Serial Serial
#define K_line_RX 3
#define K_line_TX 1
#define Led 2
#define Buzzer 4
#define voltagePin 5
#endif

#ifdef DEBUG_Serial
#define debugPrint(x) Serial.print(x)
#define debugPrintln(x) Serial.println(x)
#define debugPrintHex(x) Serial.print(x, HEX)
#else
#define debugPrint(x) ((void)0)
#define debugPrintln(x) ((void)0)
#define debugPrintHex(x) ((void)0)
#endif

#define WRITE_DELAY 5
//int REQUEST_DELAY;

String STA_ssid, STA_password, IP_address, SubnetMask, Gateway, protocol, connectedProtocol = "";
int page = -1;

int oxygenSensor1Voltage = 0, shortTermFuelTrim1 = 0, oxygenSensor2Voltage = 0, shortTermFuelTrim2 = 0;
int oxygenSensor3Voltage = 0, shortTermFuelTrim3 = 0, oxygenSensor4Voltage = 0, shortTermFuelTrim4 = 0;
int oxygenSensor5Voltage = 0, shortTermFuelTrim5 = 0, oxygenSensor6Voltage = 0, shortTermFuelTrim6 = 0;
int oxygenSensor7Voltage = 0, shortTermFuelTrim7 = 0, oxygenSensor8Voltage = 0, shortTermFuelTrim8 = 0;

double VOLTAGE = 0;
String Vehicle_VIN = "", Vehicle_ID = "", Vehicle_ID_Num = "";

bool conectionStatus = false, getOnce = false;
int errors = 0;

static unsigned long lastReqestTime = 5000, lastWsTime = 100, lastDTCTime = 1000;

void setup() {
#ifdef DEBUG_Serial
  Serial.begin(115200);
#endif
  pinMode(Led, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(voltagePin, INPUT);
  digitalWrite(Led, HIGH);

  initSpiffs();
  readSettings();

  initWiFi();
  initWebSocket();
  initWebServer();

  init_CAN();
}

void loop() {
  if (getOnce == false) {
    getOnce = true;
    digitalWrite(Led, LOW);
    getSupportedPIDs(0x01);
    getSupportedPIDs(0x02);
    getSupportedPIDs(0x09);
    // getVIN();
    // getCalibrationID();
    // getCalibrationIDNum();
  } else {
    obdTask();
  }

  VOLTAGE = (double)analogRead(voltagePin) / 4096.0 * 20.4;

  if (millis() - lastWsTime >= 100) {
    sendDataToServer();
    lastWsTime = millis();
  }
}
