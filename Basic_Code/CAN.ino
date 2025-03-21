twai_message_t lastMessage;
byte supportedLiveData[32];

void read_CAN() {
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

void getPID(byte pid) {
  writeData(pid);
  if (readCAN()) {

    if (lastMessage.data[2] == ENGINE_LOAD) {
      engineLoadValue = (100.0 / 255) * lastMessage.data[3];
      Serial.println("Calculated load value: " + String(engineLoadValue));
    } else if (lastMessage.data[2] == ENGINE_COOLANT_TEMP) {
      engineCoolantTemp = lastMessage.data[3] - 40;
      Serial.println("Coolang Temp: " + String(engineCoolantTemp));
    } else if (lastMessage.data[2] == INTAKE_MANIFOLD_ABS_PRESSURE) {
      intakeManifoldAbsPressure = lastMessage.data[3];
      Serial.println("Intake manifold pressure: " + String(intakeManifoldAbsPressure));
    } else if (lastMessage.data[2] == ENGINE_RPM) {
      engineRpmValue = (256 * lastMessage.data[3] + lastMessage.data[4]) / 4;
      Serial.println("Engine RPM: " + String(engineRpmValue));
    } else if (lastMessage.data[2] == VEHICLE_SPEED) {
      vehicleSpeedValue = lastMessage.data[3];
      Serial.println("Speed: " + String(vehicleSpeedValue));
    } else if (lastMessage.data[2] == INTAKE_AIR_TEMP) {
      intakeAirTempValue = lastMessage.data[3] - 40;
      Serial.println("Intake Temp: " + String(intakeAirTempValue));
    } else if (lastMessage.data[2] == SUPPORTED_PIDS_1_20) {
      int pidIndex = 0;
      int supportedCount = 0;

      for (int i = 3; i <= 6; i++) {
        byte value = lastMessage.data[i];
        for (int bit = 7; bit >= 0; bit--) {
          if ((value >> bit) & 1) {
            supportedLiveData[supportedCount++] = pidIndex + 1;
          }
          pidIndex++;
        }
      }

      if (isInArray(supportedLiveData, sizeof(supportedLiveData), SUPPORTED_PIDS_21_40)) {
        writeData(SUPPORTED_PIDS_21_40);
        readCAN();

        for (int i = 3; i < 6; i++) {
          byte value = lastMessage.data[i];
          for (int bit = 7; bit >= 0; bit--) {
            if ((value >> bit) & 1) {
              supportedLiveData[supportedCount++] = pidIndex + 1;
            }
            pidIndex++;
          }
        }
      }

      if (isInArray(supportedLiveData, sizeof(supportedLiveData), SUPPORTED_PIDS_41_60)) {
        writeData(SUPPORTED_PIDS_41_60);
        readCAN();

        for (int i = 3; i < 6; i++) {
          byte value = lastMessage.data[i];
          for (int bit = 7; bit >= 0; bit--) {
            if ((value >> bit) & 1) {
              supportedLiveData[supportedCount++] = pidIndex + 1;
            }
            pidIndex++;
          }
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

void writeData(byte pid) {
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
  message.data[1] = 0x01;        // Mode 01: Request current data
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
  twai_message_t response;
  unsigned long start_time = millis();

  while (millis() - start_time < 2000) {
    if (twai_receive(&response, pdMS_TO_TICKS(2000)) == ESP_OK) {
      if (response.identifier == 0x18DAF110 || response.identifier == 0x7E8) {
        if (memcmp(&lastMessage, &response, sizeof(twai_message_t)) != 0) {
          memcpy(&lastMessage, &response, sizeof(twai_message_t));
        }

        // Serial.print("ID: 0x");
        // Serial.print(response.identifier, HEX);
        // Serial.print(", RTR: ");
        // Serial.print(response.rtr, HEX);
        // Serial.print(", EID: ");
        // Serial.print(response.extd, HEX);
        // Serial.print(", (DLC): ");
        // Serial.print(response.data_length_code);
        // Serial.print(", Data: ");
        // for (int i = 0; i < response.data_length_code; i++) {
        //   if (response.data[i] < 10) {
        //     Serial.print("0");
        //     Serial.print(response.data[i], HEX);
        //   } else {
        //     Serial.print(response.data[i], HEX);
        //   }

        //   if (i < response.data_length_code - 1) {
        //     Serial.print(" ");
        //   }
        // }
        // Serial.println();
        return true;
      }
    } else {
      Serial.println("Not Received any Message!");
    }
  }
  Serial.println("OBD2 Timeout!");
  return false;
}