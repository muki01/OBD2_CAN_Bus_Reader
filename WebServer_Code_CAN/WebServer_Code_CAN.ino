#include <WiFi.h>
#include <AsyncTCP.h>
#include <SPIFFS.h>
#include <Update.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "PIDs.h"
#include "driver/twai.h"

JsonDocument jsonDoc;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

const uint8_t CAN_rxPin = 12;
const uint8_t CAN_txPin = 13;
uint8_t CAN_BIT = 11;  // 11-bit or 29-bit
twai_timing_config_t CAN_SPEED = TWAI_TIMING_CONFIG_250KBITS();

#define Led 6
#define Buzzer 8
#define voltagePin 1
#define DEBUG_Serial

#ifdef DEBUG_Serial
#define debugPrint(x) Serial.print(x)
#define debugPrintln(x) Serial.println(x)
#define debugPrintHex(x) printHex(x)
#else
#define debugPrint(x) ((void)0)
#define debugPrintln(x) ((void)0)
#define debugPrintHex(x) ((void)0)
#endif

String STA_ssid, STA_password, IP_address, SubnetMask, Gateway, selectedProtocol, connectedProtocol = "";
int page = -1, unreceivedDataCount = 0;

int oxygenSensor1Voltage = 0, shortTermFuelTrim1 = 0, oxygenSensor2Voltage = 0, shortTermFuelTrim2 = 0;
int oxygenSensor3Voltage = 0, shortTermFuelTrim3 = 0, oxygenSensor4Voltage = 0, shortTermFuelTrim4 = 0;
int oxygenSensor5Voltage = 0, shortTermFuelTrim5 = 0, oxygenSensor6Voltage = 0, shortTermFuelTrim6 = 0;
int oxygenSensor7Voltage = 0, shortTermFuelTrim7 = 0, oxygenSensor8Voltage = 0, shortTermFuelTrim8 = 0;

// Voltage Measurement Settings
float VOLTAGE = 0;
float smoothedVoltage = -1.0;
const float R1 = 47000.0;                // Adjust this value if needed
const float R2 = 10000.0;                // Adjust this value if needed
const float CALIBRATION_FACTOR = 1.017;  // Adjust this value if needed
const float VOLTAGE_RATIO = ((R1 + R2) / R2) * CALIBRATION_FACTOR;

String Vehicle_VIN = "", Vehicle_ID = "", Vehicle_ID_Num = "";

bool connectionStatus = false;
bool clearDTC_Flag = false;

static unsigned long lastWsTime = 100, lastDTCTime = 1000;

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
  debugPrint(F("Selected Protocol: "));
  debugPrintln(selectedProtocol);

  initWiFi();
  initWebSocket();
  initWebServer();
}

void loop() {
  readBatteryVoltage();

  if (connectionStatus == false) {
    Melody3();
    bool init_success = initOBD2();

    if (init_success) {
      digitalWrite(Led, LOW);
      connectMelody();
      readSupportedData(0x01);
      readSupportedData(0x02);
      readSupportedData(0x09);
      // getVIN();
      // getCalibrationID();
      // getCalibrationIDNum();
    }
  } else {
    obdTask();
  }

  if (millis() - lastWsTime >= 100) {
    sendDataToServer();
    lastWsTime = millis();
  }
}

void readBatteryVoltage() {
  // Voltage Measurement (Oversampling + EMA Filter)
  uint32_t mvSum = 0;
  for (int i = 0; i < 16; i++) {
    mvSum += analogReadMilliVolts(voltagePin);
  }
  float currentReading = ((float)mvSum / 16.0 / 1000.0) * VOLTAGE_RATIO;

  if (smoothedVoltage < 0) smoothedVoltage = currentReading;

  const float alpha = 0.01;
  smoothedVoltage = (alpha * currentReading) + ((1.0 - alpha) * smoothedVoltage);
  VOLTAGE = smoothedVoltage;
}