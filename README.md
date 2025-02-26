# 🚗OBD2 CAN_BUS Reader

This code is for reading the CAN_BUS in Cars. With this code you can read sensor values, troubleshoot codes and more. It is compatible with 250KBPS and 500KBPS, 11BIT and 29BIT.
I have shared schematics to communicate with the car. You can use these schematics or you can make another one. I used Arduino nano and ESP32 C3 as microcontrollers, but you can use another microcontrollers like STM32, ESP8266 and much more.

I will share more information about CAN_BUS protocols and communication later. Stay tuned 😉.

You can also see [this](https://github.com/muki01/I-K_Bus) project for I/K Bus for BMW cars

## ⚙️Instalation
* Edit the pins and options for your Board
     ~~~
     #define TX_GPIO_NUM GPIO_NUM_9  // CAN TX pini
     #define RX_GPIO_NUM GPIO_NUM_10  // CAN RX pini
     #define CAN_SPEED TWAI_TIMING_CONFIG_500KBITS() // CAN SPEED 250KBITS or 500KBITS 
     #define CAN_BIT 29 // 11BIT or 29BIT
     ~~~
* Upload the code to your Board


## 🛠️Schematics for communication

## The device I made with ESP32 C3 SuperMini.
