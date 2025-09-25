#include "driver/twai.h"
#include "PIDs.h"

const uint8_t CAN_rxPin = 12;
const uint8_t CAN_txPin = 13;
uint8_t CAN_BIT = 11;  // 11-bit or 29-bit
twai_timing_config_t CAN_SPEED = TWAI_TIMING_CONFIG_250KBITS();
#define DEBUG_Serial

#ifdef DEBUG_Serial
#define debugPrint(x) Serial.print(x)
#define debugPrintln(x) Serial.println(x)
#define debugPrintHex(x) Serial.printf("%02lX", x);
#else
#define debugPrint(x) ((void)0)
#define debugPrintln(x) ((void)0)
#define debugPrintHex(x) ((void)0)
#endif

String selectedProtocol, connectedProtocol = "";
int unreceivedDataCount = 0;
bool connectionStatus = false;

int fuelSystemStatus = 0, engineLoadValue = 0, engineCoolantTemp = 0, shortTermFuelTrimBank1 = 0;
int longTermFuelTrimBank1 = 0, shortTermFuelTrimBank2 = 0, longTermFuelTrimBank2 = 0, fuelPressureValue = 0;
int intakeManifoldAbsPressure = 0, engineRpmValue = 0, vehicleSpeedValue = 0, timingAdvanceValue = 0;
int intakeAirTempValue = 0, mafAirFlowRate = 0, throttlePositionValue = 0, secondaryAirStatus = 0;
int oxygenSensorsPresent2Banks = 0, oxygenSensor1Voltage = 0, shortTermFuelTrim1 = 0, oxygenSensor2Voltage = 0;
int shortTermFuelTrim2 = 0, oxygenSensor3Voltage = 0, shortTermFuelTrim3 = 0, oxygenSensor4Voltage = 0;
int shortTermFuelTrim4 = 0, oxygenSensor5Voltage = 0, shortTermFuelTrim5 = 0, oxygenSensor6Voltage = 0;
int shortTermFuelTrim6 = 0, oxygenSensor7Voltage = 0, shortTermFuelTrim7 = 0, oxygenSensor8Voltage = 0;
int shortTermFuelTrim8 = 0, obdStandards = 0, oxygenSensorsPresent4Banks = 0, auxiliaryInputStatus = 0;
int runTimeSinceEngineStart = 0, distanceWithMilOn = 0;

void setup() {
#ifdef DEBUG_Serial
  Serial.begin(115200);
#endif
}

void loop() {
  if (connectionStatus == false) {
    bool init_success = initOBD2();

    if (init_success) {
      readSupportedData(0x01);
      readSupportedData(0x02);
      readSupportedData(0x09);
    }
  } else {
    obdTask();
  }
}