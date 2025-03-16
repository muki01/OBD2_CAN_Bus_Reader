#include "driver/twai.h"
#include "PIDs.h"

#define TX_GPIO_NUM GPIO_NUM_9                   // CAN TX pin
#define RX_GPIO_NUM GPIO_NUM_10                  // CAN RX pin
#define CAN_SPEED TWAI_TIMING_CONFIG_500KBITS()  // CAN SPEED 125KBITS, 250KBITS, 500KBITS or 1MBITS
#define CAN_BIT 29                               // 11BIT or 29BIT

#define BOOT_BUTTON_PIN 0
bool buttonPressed = false;

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
  Serial.begin(115200);
  pinMode(BOOT_BUTTON_PIN, INPUT_PULLUP);
  init_CAN();
}

void loop() {
  read_CAN();
}