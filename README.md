# 🚗OBD2 CAN_BUS Reader

![GitHub forks](https://img.shields.io/github/forks/muki01/OBD2_CAN_Bus_Reader?style=flat)
![GitHub Repo stars](https://img.shields.io/github/stars/muki01/OBD2_CAN_Bus_Reader?style=flat)
![GitHub Issues or Pull Requests](https://img.shields.io/github/issues/muki01/OBD2_CAN_Bus_Reader?style=flat)
![GitHub License](https://img.shields.io/github/license/muki01/OBD2_CAN_Bus_Reader?style=flat)
![GitHub last commit](https://img.shields.io/github/last-commit/muki01/OBD2_CAN_Bus_Reader)

This code allows you to interface with the CAN Bus in vehicles. It enables you to read sensor values, diagnose trouble codes, and more. It supports various baud rates (125KBPS, 250KBPS, 500KBPS, 1MBPS) and both 11-bit and 29-bit CAN identifiers.

I will share schematics to communicate with the car. You can use these schematics. I am currently testing this code with ESP32 S3. For now it will only work with ESP32 boards, I will do it for other boards in the future.

In the future, I will release a separate repository that dives deeper into CAN Bus functionality. In that project, I'll show how you can control various vehicle systems such as headlights, windows, locks, and more using the CAN Bus. I am currently testing this feature, and it works well.

You can also see my other car projects:
1. [Тhis](https://github.com/muki01/I-K_Bus) project is for BMW with I/K bus system. 
2. [Тhis](https://github.com/muki01/OBD2_CAN_Bus_Reader) project is for Cars with CAN Bus.
3. [Тhis](https://github.com/muki01/OBD2_K-line_Reader) project is for Cars with ISO9141 and ISO14230 protocols.
4. [Тhis](https://github.com/muki01/OBD2_CAN_Bus_Library) is my OBD2 CAN Bus Communication Library for Arduino IDE.
5. [Тhis](https://github.com/muki01/OBD2_KLine_Library) is my OBD2 K-Line Communication Library for Arduino IDE.
<!--6. [Тhis](https://github.com/muki01/I-K_Bus_Library) is my I/K Bus Communication Library for Arduino IDE.-->

## ⚙️Instalation
* Edit the pins and options for your Board
     ~~~
     #define TX_GPIO_NUM GPIO_NUM_9                   // CAN TX pin
     #define RX_GPIO_NUM GPIO_NUM_10                  // CAN RX pin
     #define CAN_SPEED TWAI_TIMING_CONFIG_500KBITS()  // CAN SPEED 125KBITS, 250KBITS, 500KBITS or 1MBITS
     #define CAN_BIT 29                               // 11BIT or 29BIT
     ~~~
* Upload the code to your Board

> [!WARNING] 
> I am not responsible for any issues or damages that may occur during your testing. Please proceed with caution and at your own risk.


## 📱Pictures of the application I made
<img width=90% src="https://github.com/user-attachments/assets/766e178a-b956-4bdb-8f64-1919da479c65">
<img width=90% src="https://github.com/user-attachments/assets/24f8f3cd-4056-44de-8414-635a4de0d60c">


## 🛠️Schematics for communication
<img src="https://github.com/user-attachments/assets/3fdc245d-841e-4733-b52c-21789ddfdf5e" width=70%>
