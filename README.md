# 🚗OBD2 CAN_BUS Reader

![GitHub forks](https://img.shields.io/github/forks/muki01/OBD2_CAN_Bus_Reader?style=flat)
![GitHub Repo stars](https://img.shields.io/github/stars/muki01/OBD2_CAN_Bus_Reader?style=flat)
![GitHub Issues or Pull Requests](https://img.shields.io/github/issues/muki01/OBD2_CAN_Bus_Reader?style=flat)
![GitHub License](https://img.shields.io/github/license/muki01/OBD2_CAN_Bus_Reader?style=flat)
![GitHub last commit](https://img.shields.io/github/last-commit/muki01/OBD2_CAN_Bus_Reader)

This code allows you to interface with the CAN Bus in vehicles. It enables you to read sensor values, diagnose trouble codes, and more. It supports various baud rates (125KBPS, 250KBPS, 500KBPS, 1MBPS) and both 11-bit and 29-bit CAN identifiers.

I will share schematics to communicate with the car. You can use these schematics. I am currently testing this code with ESP32 S3. For now it will only work with ESP32 boards, I will do it for other boards in the future.

In the future, I will release a separate repository that dives deeper into CAN Bus functionality. In that project, I'll show how you can control various vehicle systems such as headlights, windows, locks, and more using the CAN Bus. I am currently testing this feature, and it works well.

🔗 **Looking for the Web UI?** You can explore the dedicated user interface repository here: [OBD2 K-Line Web UI Project](https://github.com/muki01/OBD2-Diagnostic-UI)

You can also see my other car projects:
1. [Тhis](https://github.com/muki01/I-K_Bus) project is for BMW with I/K bus system. 
2. [Тhis](https://github.com/muki01/OBD2_CAN_Bus_Reader) project is for Cars with CAN Bus.
3. [Тhis](https://github.com/muki01/OBD2_K-line_Reader) project is for Cars with ISO9141 and ISO14230 protocols.
4. [Тhis](https://github.com/muki01/OBD2_CAN_Bus_Library) is my OBD2 CAN Bus Communication Library for Arduino IDE.
5. [Тhis](https://github.com/muki01/OBD2_KLine_Library) is my OBD2 K-Line Communication Library for Arduino IDE.
6. [Тhis](https://github.com/muki01/VAG_KW1281) project is for VAG Cars with KW1281 protocol.
<!--7. [Тhis](https://github.com/muki01/I-K_Bus_Library) is my I/K Bus Communication Library for Arduino IDE.-->

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
<a href="https://github.com/muki01/OBD2-Diagnostic-UI" target="_blank">
  <img width=90% src="https://github.com/user-attachments/assets/5a3e0540-b56d-4c3a-a0bf-8c1affcda00c" />
</a>
<a href="https://github.com/muki01/OBD2-Diagnostic-UI" target="_blank">
  <img width=90% src="https://github.com/user-attachments/assets/8544df16-cf62-4a80-8f19-cbd0daadfb51" />
</a>

📂 *The UI for this application has been moved to a separate repository. You can check out the source code and details here:* **[Explore the Web UI Repository](https://github.com/muki01/OBD2-Diagnostic-UI)**


## 🛠️Schematics for communication
<img src="https://github.com/user-attachments/assets/3fdc245d-841e-4733-b52c-21789ddfdf5e" width=70%>

---

## ☕ Support My Work

If you enjoy my projects and want to support me, you can do so through the links below:

[![Buy Me A Coffee](https://img.shields.io/badge/-Buy%20Me%20a%20Coffee-FFDD00?style=for-the-badge&logo=buy-me-a-coffee&logoColor=black)](https://www.buymeacoffee.com/muki01)
[![PayPal](https://img.shields.io/badge/-PayPal-00457C?style=for-the-badge&logo=paypal&logoColor=white)](https://www.paypal.com/donate/?hosted_button_id=SAAH5GHAH6T72)
[![GitHub Sponsors](https://img.shields.io/badge/-Sponsor%20Me%20on%20GitHub-181717?style=for-the-badge&logo=github)](https://github.com/sponsors/muki01)

---

## 📬 Contact

For information, job offers, collaboration, sponsorship, or purchasing my devices, you can contact me via email.

📧 Email: muksin.muksin04@gmail.com

---
