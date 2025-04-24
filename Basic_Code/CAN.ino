twai_message_t resultBuffer;
byte supportedLiveData[32];

void obdTask() {
  if (digitalRead(BOOT_BUTTON_PIN) == LOW && !buttonPressed) {
    buttonPressed = true;
    Serial.println("BOOT button Clicked");
    getPID(SUPPORTED_PIDS_1_20);
  }

  if (digitalRead(BOOT_BUTTON_PIN) == HIGH) {
    buttonPressed = false;
  }

  getPID(ENGINE_LOAD);
  getPID(ENGINE_COOLANT_TEMP);
  getPID(INTAKE_MANIFOLD_ABS_PRESSURE);
  getPID(ENGINE_RPM);
  getPID(VEHICLE_SPEED);
  getPID(INTAKE_AIR_TEMP);
}

void init_CAN() {
  Serial.println("Initializing TWAI...");
  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(TX_GPIO_NUM, RX_GPIO_NUM, TWAI_MODE_NORMAL);
  twai_timing_config_t t_config = CAN_SPEED;
  twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();  // Accept all messages

  // Install TWAI driver
  if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
    Serial.println("TWAI driver installed.");
  } else {
    Serial.println("Failed to install TWAI driver!");
    return;
  }

  // Start TWAI
  if (twai_start() == ESP_OK) {
    Serial.println("TWAI started.");
  } else {
    Serial.println("Failed to start TWAI!");
    return;
  }
}

void writeData(byte mode, byte pid) {
  Serial.println("Writing Data");
  twai_message_t message;

  if (CAN_BIT == 29) {
    message.identifier = 0x18DB33F1;
    message.extd = 1;
  } else if (CAN_BIT == 11) {
    message.identifier = 0x7DF;
    message.extd = 0;
  }

  message.rtr = 0;               // Data frame
  message.data_length_code = 8;  // 8-byte data frame
  message.data[0] = 0x02;        // Query length (2 bytes: Mode and PID)
  message.data[1] = mode;        // Mode 01: Request current data
  message.data[2] = pid;         // PID
  message.data[3] = 0x00;        // Padding bytes
  message.data[4] = 0x00;
  message.data[5] = 0x00;
  message.data[6] = 0x00;
  message.data[7] = 0x00;

  if (twai_transmit(&message, pdMS_TO_TICKS(1000)) == ESP_OK) {
    Serial.println("Query sent.");
  } else {
    Serial.println("Failed to send query!");
  }
}

bool readCAN() {
  Serial.println("Reading...");
  twai_message_t response;
  unsigned long start_time = millis();

  while (millis() - start_time < 2000) {
    if (twai_receive(&response, pdMS_TO_TICKS(2000)) == ESP_OK) {
      if (response.identifier == 0x18DAF110 || response.identifier == 0x18DAF111 || response.identifier == 0x7E8) {
        if (memcmp(&resultBuffer, &response, sizeof(twai_message_t)) != 0) {
          memcpy(&resultBuffer, &response, sizeof(twai_message_t));
        }

        Serial.print("Received Data: ");
        Serial.print("ID: 0x");
        Serial.print(response.identifier, HEX);
        Serial.print(", RTR: ");
        Serial.print(response.rtr, HEX);
        Serial.print(", EID: ");
        Serial.print(response.extd, HEX);
        Serial.print(", (DLC): ");
        Serial.print(response.data_length_code);
        Serial.print(", Data: ");
        for (int i = 0; i < response.data_length_code; i++) {
          if (response.data[i] < 10) {
            Serial.print("0");
            Serial.print(response.data[i], HEX);
          } else {
            Serial.print(response.data[i], HEX);
          }

          if (i < response.data_length_code - 1) {
            Serial.print(" ");
          }
        }
        Serial.println();
        return true;
      }
    } else {
      Serial.println("Not Received any Message!");
    }
  }
  Serial.println("OBD2 Timeout!");
  return false;
}

void getPID(byte pid) {
  writeData(read_LiveData, pid);
  if (readCAN()) {

    if (resultBuffer.data[2] == ENGINE_LOAD) {
      engineLoadValue = (100.0 / 255) * resultBuffer.data[3];
    } else if (resultBuffer.data[2] == ENGINE_COOLANT_TEMP) {
      engineCoolantTemp = resultBuffer.data[3] - 40;
    } else if (resultBuffer.data[2] == INTAKE_MANIFOLD_ABS_PRESSURE) {
      intakeManifoldAbsPressure = resultBuffer.data[3];
    } else if (resultBuffer.data[2] == ENGINE_RPM) {
      engineRpmValue = (256 * resultBuffer.data[3] + resultBuffer.data[4]) / 4;
    } else if (resultBuffer.data[2] == VEHICLE_SPEED) {
      vehicleSpeedValue = resultBuffer.data[3];
    } else if (resultBuffer.data[2] == INTAKE_AIR_TEMP) {
      intakeAirTempValue = resultBuffer.data[3] - 40;
    }
  }
}

void get_DTCs() {
  // Request: C2 33 F1 03 F3
  // example Response: 87 F1 11 43 01 70 01 34 00 00 72
  int dtcs = 0;
  char dtcBytes[2];

  writeData(read_DTCs, 0x00);
  readCAN();

  int length = sizeof(resultBuffer.data);
  for (int i = 0; i < length; i++) {
    dtcBytes[0] = resultBuffer.data[3 + i * 2];
    dtcBytes[1] = resultBuffer.data[3 + i * 2 + 1];

    if (dtcBytes[0] == 0 && dtcBytes[1] == 0) {
      break;
    } else {
      String ErrorCode = decodeDTC(dtcBytes[0], dtcBytes[1]);
      dtcBuffer[dtcs++] = ErrorCode;
    }
  }
}

String decodeDTC(char input_byte1, char input_byte2) {
  String ErrorCode = "";
  const static char type_lookup[4] = { 'P', 'C', 'B', 'U' };
  const static char digit_lookup[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

  ErrorCode += type_lookup[(input_byte1 >> 6) & 0b11];
  ErrorCode += digit_lookup[(input_byte1 >> 4) & 0b11];
  ErrorCode += digit_lookup[input_byte1 & 0b1111];
  ErrorCode += digit_lookup[input_byte2 >> 4];
  ErrorCode += digit_lookup[input_byte2 & 0b1111];

  return ErrorCode;
}

void clear_DTC() {
  writeData(clear_DTCs, 0x00);
}

void getSupportedPIDs(const byte option) {
  int pidIndex = 0;
  int supportedCount = 0;

  if (option == 0x01) {
    writeData(option, SUPPORTED_PIDS_1_20);

    if (readCAN()) {
      for (int i = 3; i < 7; i++) {
        byte value = resultBuffer.data[i];
        for (int bit = 7; bit >= 0; bit--) {
          if ((value >> bit) & 1) {
            supportedLiveData[supportedCount++] = pidIndex + 1;
          }
          pidIndex++;
        }
      }

      if (isInArray(supportedLiveData, sizeof(supportedLiveData), 0x20)) {
        writeData(option, SUPPORTED_PIDS_21_40);
        if (readCAN()) {

          for (int i = 3; i < 7; i++) {
            byte value = resultBuffer.data[i];
            for (int bit = 7; bit >= 0; bit--) {
              if ((value >> bit) & 1) {
                supportedLiveData[supportedCount++] = pidIndex + 1;
              }
              pidIndex++;
            }
          }
        }
      }

      if (isInArray(supportedLiveData, sizeof(supportedLiveData), 0x40)) {
        writeData(option, SUPPORTED_PIDS_41_60);
        if (readCAN()) {

          for (int i = 3; i < 7; i++) {
            byte value = resultBuffer.data[i];
            for (int bit = 7; bit >= 0; bit--) {
              if ((value >> bit) & 1) {
                supportedLiveData[supportedCount++] = pidIndex + 1;
              }
              pidIndex++;
            }
          }
        }
      }
    }
  }

  if (option == 0x02) {
    writeData(option, SUPPORTED_PIDS_1_20);

    if (readCAN()) {
      for (int i = 4; i < 8; i++) {
        byte value = resultBuffer.data[i];
        for (int bit = 7; bit >= 0; bit--) {
          if ((value >> bit) & 1) {
            supportedFreezeFrame[supportedCount++] = pidIndex + 1;
          }
          pidIndex++;
        }
      }
    }
  }
  if (option == 0x09) {
    writeData(option, supported_VehicleInfo);

    if (readCAN()) {
      for (int i = 4; i < 8; i++) {
        byte value = resultBuffer.data[i];
        for (int bit = 7; bit >= 0; bit--) {
          if ((value >> bit) & 1) {
            supportedVehicleInfo[supportedCount++] = pidIndex + 1;
          }
          pidIndex++;
        }
      }
      Serial.print("Supported Live Data: ");
      for (int i = 0; i < supportedCount; i++) {
        if (supportedLiveData[i] < 10) {
          Serial.print("0");
          Serial.print(supportedLiveData[i], HEX);
        } else {
          Serial.print(supportedLiveData[i], HEX);
        }

        if (i < supportedCount - 1) {
          Serial.print(" ");
        }
      }
      Serial.println();
    }
  }
}