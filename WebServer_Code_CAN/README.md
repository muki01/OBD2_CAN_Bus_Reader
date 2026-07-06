# WebServer Code
This code is designed to be used with ESP32 devices. It creates a Web Server that allows you to view the car's values.
The ESP32 creates a WiFi AP (access point) named "OBD2 Master".After connecting to this WiFi network, you should open `192.168.4.1` address in your browser.

## 📡 How It Works
The ESP32's built-in CAN (TWAI) controller connects to the car’s OBD-II interface to retrieve vehicle data. This data is then sent to a website via WebSocket.

## 🛠️ Hardware Setup
Since the ESP32 includes an internal CAN controller, you only need an external CAN transceiver (e.g., SN65HVD230, TJA1050). You can connect the transceiver to any GPIO pins on the ESP32 — the TWAI driver allows you to configure which pins to use in software. For reliable communication, make sure to add a 120Ω termination resistor to the CAN bus if needed.

## ⚙️Instalation
There are two ways to install the code. The first way is to flash the files I have provided on the releases page. The second way is to use the Arduino IDE to manually compile and upload.

### 1. Flash Firmware

### 2. Manuel Instalation using Arduino IDE
* Open .ino file
* 📚Instal these Libraries
  ~~~
  - ESPAsyncWebServer
  - AsyncTCP
  - ArduinoJson
  ~~~
* Edit the pins for your Board
  ~~~
  #define TX_GPIO_NUM GPIO_NUM_13  // CAN TX pin
  #define RX_GPIO_NUM GPIO_NUM_12  // CAN RX pin
  #define Led 8
  #define Buzzer 4
  #define voltagePin 3
  ~~~
* If you are using ESP32 C3, C6, S2, S3 or H2 board, you need to disable "USB CDC On Boot" option in Tools menu
* Upload the code to your Board
* Upload the Website to SPIFFS.
  - To upload the Web Site, you can use PlatformIO or you can upload with tool for Arduino IDE v1.x.x</br>
  [Here](https://randomnerdtutorials.com/install-esp32-filesystem-uploader-arduino-ide/) is documentation how to upload files to SPIFFS with Arduino IDE.


## 📱Pictures of the application I made
<a href="https://github.com/muki01/OBD2-Diagnostic-UI" target="_blank">
  <img width=90% src="https://github.com/user-attachments/assets/5a3e0540-b56d-4c3a-a0bf-8c1affcda00c" />
</a>
<a href="https://github.com/muki01/OBD2-Diagnostic-UI" target="_blank">
  <img width=90% src="https://github.com/user-attachments/assets/8544df16-cf62-4a80-8f19-cbd0daadfb51" />
</a>

📂 *The UI for this application has been moved to a separate repository. You can check out the source code and details here:* **[Explore the Web UI Repository](https://github.com/muki01/OBD2-Diagnostic-UI)**
