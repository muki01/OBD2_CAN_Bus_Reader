#include "driver/twai.h"

#define TX_GPIO_NUM GPIO_NUM_9  // CAN TX pini
#define RX_GPIO_NUM GPIO_NUM_10  // CAN RX pini
#define CAN_SPEED TWAI_TIMING_CONFIG_500KBITS() // CAN SPEED 250KBITS or 500KBITS 
#define CAN_BIT 29 // 11BIT or 29BIT

void setup() {
  Serial.begin(115200);
  init_CAN();
}

void loop() {
  read_CAN();
}