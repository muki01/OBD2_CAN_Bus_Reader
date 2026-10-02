# WebServer_Code_CAN — Setup Guide

ESP32 firmware that reads the vehicle over the CAN bus and serves a web dashboard. The ESP32 creates a Wi-Fi access point named **`OBD2 Master`**; connect to it and open **http://192.168.4.1** in your browser.

← Back to the [main README](../README.md)

## How It Works

The ESP32's built-in CAN controller (TWAI) talks to the car through the OBD-II connector. The values it reads are sent to the web page as JSON over a WebSocket.

## Hardware

The ESP32 already contains the CAN controller, so the only extra part is a **CAN transceiver** such as the SN65HVD230 or TJA1050. It can be connected to any free GPIO pair; the pins are set in the sketch. See the [schematic](../README.md#schematic) in the main README.

## Installation

### 1. Install the libraries

In the Arduino IDE, install:

- `ESPAsyncWebServer`
- `AsyncTCP`
- `ArduinoJson`
- `Adafruit NeoPixel`

### 2. Set the pins

Open `WebServer_Code_CAN.ino` and adjust the pins for your board:

```cpp
const uint8_t CAN_rxPin = 12;  // CAN RX pin
const uint8_t CAN_txPin = 13;  // CAN TX pin

#define Led 6
#define Buzzer 8
#define voltagePin 1
```

### 3. Upload the sketch

Select your ESP32 board and upload.

> On ESP32-C3, C6, S2, S3 and H2 boards, disable **USB CDC On Boot** in the **Tools** menu.

### 4. Upload the web interface

The files in the `data` folder have to be written to the SPIFFS file system. Use PlatformIO, or the file system uploader for the Arduino IDE — see [this guide](https://randomnerdtutorials.com/install-esp32-filesystem-uploader-arduino-ide/).

## First Use

1. Plug the device into the OBD-II port and turn the ignition on.
2. Connect your phone to the Wi-Fi network **`OBD2 Master`** with the password `12345678`.
3. Open **http://192.168.4.1**.

The protocol, the Wi-Fi settings and firmware updates are all available on the settings page of the dashboard.

## Web Interface

<a href="https://github.com/muki01/OBD2-Diagnostic-UI">
  <img width="100%" src="https://github.com/user-attachments/assets/9b3aebe5-998d-4731-85bc-a0d7666fd116" alt="Screenshots of the OBD2 web dashboard">
</a>
<a href="https://github.com/muki01/OBD2-Diagnostic-UI">
  <img width="100%" src="https://github.com/user-attachments/assets/8544df16-cf62-4a80-8f19-cbd0daadfb51" alt="More screenshots of the OBD2 web dashboard">
</a>

The source of the web interface lives in its own repository: **[OBD2 Diagnostic UI](https://github.com/muki01/OBD2-Diagnostic-UI)**.
