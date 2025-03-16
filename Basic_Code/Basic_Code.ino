#include "driver/twai.h"
#include "PIDs.h"

#define TX_GPIO_NUM GPIO_NUM_9                   // CAN TX pin
#define RX_GPIO_NUM GPIO_NUM_10                  // CAN RX pin
#define CAN_SPEED TWAI_TIMING_CONFIG_500KBITS()  // CAN SPEED 250KBITS or 500KBITS
#define CAN_BIT 29                               // 11BIT or 29BIT

#define BOOT_BUTTON_PIN 0
bool buttonPressed = false;

void setup() {
  Serial.begin(115200);
  pinMode(BOOT_BUTTON_PIN, INPUT_PULLUP);
  init_CAN();
}

void loop() {
  read_CAN();
}