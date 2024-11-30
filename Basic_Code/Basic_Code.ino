#include "driver/twai.h"

#define TX_GPIO_NUM GPIO_NUM_1  // CAN TX pini
#define RX_GPIO_NUM GPIO_NUM_2  // CAN RX pini

void setup() {
  Serial.begin(115200);
  init_CAN();
}

void loop() {
  read_CAN();
}