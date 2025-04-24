twai_message_t resultBuffer;
String dtcBuffer[32];
byte supportedLiveData[32];
byte desiredLiveData[32];
byte supportedFreezeFrame[32];
byte supportedVehicleInfo[32];

void obdTask() {
  if (page == 1 || page == -1) {
    for (const auto& mapping : liveDataMappings) {
      if (isInArray(desiredLiveData, sizeof(desiredLiveData), mapping.pid)) {
        getPID(mapping.pid);
      }
    }
    if (page == -1) {
      get_DTCs();
    }
  } else if (page == 0 || page == 2 || page == 5 || page == 6) {
    if (millis() - lastDTCTime >= 1000) {
      get_DTCs();
      if (page == 2) {
        if (isInArray(supportedLiveData, sizeof(supportedLiveData), DISTANCE_TRAVELED_WITH_MIL_ON)) {
          getPID(DISTANCE_TRAVELED_WITH_MIL_ON);
        }
      }
      lastDTCTime = millis();
    }
  } else if (page == 3) {
    get_DTCs();
    for (const auto& mapping : freezeFrameMappings) {
      if (isInArray(supportedFreezeFrame, sizeof(supportedFreezeFrame), mapping.pid)) {
        getPID(mapping.pid);
      }
    }
  } else if (page == 4) {
    if (isInArray(supportedLiveData, sizeof(supportedLiveData), VEHICLE_SPEED)) {
      getPID(VEHICLE_SPEED);
    }
  }
}

bool init_CAN() {
  twai_stop();
  twai_driver_uninstall();

  Serial.println("Initializing TWAI...");
  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(TX_GPIO_NUM, RX_GPIO_NUM, TWAI_MODE_NORMAL);
  twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();  // Accept all messages
  twai_timing_config_t t_config;
  if (protocol == "11b250") {
    CAN_BIT = 11;
    t_config = TWAI_TIMING_CONFIG_250KBITS();
  } else if (protocol == "11b500") {
    CAN_BIT = 11;
    t_config = TWAI_TIMING_CONFIG_500KBITS();
  } else if (protocol == "29b250") {
    CAN_BIT = 29;
    t_config = TWAI_TIMING_CONFIG_250KBITS();
  } else if (protocol == "29b500") {
    CAN_BIT = 29;
    t_config = TWAI_TIMING_CONFIG_500KBITS();
  }

  else if (protocol == "Automatic") {
    testedProtocol++;
    if (testedProtocol == 1) {
      CAN_BIT = 11;
      t_config = TWAI_TIMING_CONFIG_250KBITS();
    } else if (testedProtocol == 2) {
      CAN_BIT = 11;
      t_config = TWAI_TIMING_CONFIG_500KBITS();
    } else if (testedProtocol == 3) {
      CAN_BIT = 29;
      t_config = TWAI_TIMING_CONFIG_250KBITS();
    } else if (testedProtocol == 4) {
      CAN_BIT = 29;
      t_config = TWAI_TIMING_CONFIG_500KBITS();
      testedProtocol = 0;
    }
  }

  // Install TWAI driver
  if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
    Serial.println("TWAI driver installed.");
  } else {
    Serial.println("Failed to install TWAI driver!");
    return false;
  }

  // Start TWAI
  if (twai_start() == ESP_OK) {
    Serial.println("TWAI started.");
    return true;
  } else {
    Serial.println("Failed to start TWAI!");
    return false;
  }
}

bool tryToConnect() {
  init_CAN();

  writeData(0x01, 0x00);
  if (readData()) {
    if (protocol == "Automatic") {
      if (testedProtocol == 1) connectedProtocol = "11b500";
      else if (testedProtocol == 2) connectedProtocol = "29b250";
      else if (testedProtocol == 3) connectedProtocol = "29b500";
      else if (testedProtocol == 4) connectedProtocol = "11b250";
    } else {
      connectedProtocol = protocol;
    }

    return true;
  }
  return false;
}

void writeData(byte mode, byte pid) {
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
  if (mode == read_FreezeFrame) {
    message.data[0] = 0x03;
  } else if (mode == read_DTCs || mode == clear_DTCs) {
    message.data[0] = 0x01;
  } else {
    message.data[0] = 0x02;  // Query length (2 bytes: Mode and PID)
  }
  message.data[1] = mode;  // Mode 01: Request current data
  message.data[2] = pid;   // PID
  message.data[3] = 0x00;  // Padding bytes
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

bool readData() {
  twai_message_t response;
  unsigned long start_time = millis();

  while (millis() - start_time < 500) {
    if (twai_receive(&response, pdMS_TO_TICKS(500)) == ESP_OK) {
      if (response.identifier == 0x18DAF110 || response.identifier == 0x18DAF111 || response.identifier == 0x7E8) {
        errors = 0;
        if (memcmp(&resultBuffer, &response, sizeof(twai_message_t)) != 0) {
          memcpy(&resultBuffer, &response, sizeof(twai_message_t));
        }

        Serial.print("Received Data: ID: 0x");
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
        delay(2);
        return true;
      }
    } else {
      Serial.println("Not Received any Message!");
    }
  }
  Serial.println("OBD2 Timeout!");
  errors++;
  if (errors > 2) {
    errors = 0;
    if (conectionStatus) {
      conectionStatus = false;
    }
  }
  return false;
}

void getPID(const byte pid) {
  // example Request: C2 33 F1 01 0C F3
  // example Response: 84 F1 11 41 0C 1F 40 32
  writeData(read_LiveData, pid);

  if (readData()) {
    if (resultBuffer.data[2] == pid) {
      for (int i = 0; i < 64; i++) {
        if (liveDataMappings[i].pid == pid) {

          if (pid == FUEL_SYSTEM_STATUS) {
            liveDataMappings[i].value = resultBuffer.data[3];
          } else if (pid == ENGINE_LOAD) {
            liveDataMappings[i].value = (100.0 / 255) * resultBuffer.data[3];
          } else if (pid == ENGINE_COOLANT_TEMP) {
            liveDataMappings[i].value = resultBuffer.data[3] - 40;
          } else if (pid == SHORT_TERM_FUEL_TRIM_BANK_1) {
            liveDataMappings[i].value = (resultBuffer.data[3] / 1.28) - 100.0;
          } else if (pid == LONG_TERM_FUEL_TRIM_BANK_1) {
            liveDataMappings[i].value = (resultBuffer.data[3] / 1.28) - 100.0;
          } else if (pid == SHORT_TERM_FUEL_TRIM_BANK_2) {
            liveDataMappings[i].value = (resultBuffer.data[3] / 1.28) - 100.0;
          } else if (pid == LONG_TERM_FUEL_TRIM_BANK_2) {
            liveDataMappings[i].value = (resultBuffer.data[3] / 1.28) - 100.0;
          } else if (pid == FUEL_PRESSURE) {
            liveDataMappings[i].value = 3 * resultBuffer.data[3];
          } else if (pid == INTAKE_MANIFOLD_ABS_PRESSURE) {
            liveDataMappings[i].value = resultBuffer.data[3];
          } else if (pid == ENGINE_RPM) {
            liveDataMappings[i].value = (256 * resultBuffer.data[3] + resultBuffer.data[4]) / 4;
          } else if (pid == VEHICLE_SPEED) {
            liveDataMappings[i].value = resultBuffer.data[3];
          } else if (pid == TIMING_ADVANCE) {
            liveDataMappings[i].value = (resultBuffer.data[3] / 2) - 64;
          } else if (pid == INTAKE_AIR_TEMP) {
            liveDataMappings[i].value = resultBuffer.data[3] - 40;
          } else if (pid == MAF_FLOW_RATE) {
            liveDataMappings[i].value = (256 * resultBuffer.data[3] + resultBuffer.data[4]) / 100.0;
          } else if (pid == THROTTLE_POSITION) {
            liveDataMappings[i].value = (100.0 / 255) * resultBuffer.data[3];
          } else if (pid == COMMANDED_SECONDARY_AIR_STATUS) {
            liveDataMappings[i].value = resultBuffer.data[3];
          } else if (pid == OXYGEN_SENSORS_PRESENT_2_BANKS) {
            liveDataMappings[i].value = resultBuffer.data[3];
          } else if (pid == OXYGEN_SENSOR_1_A) {
            liveDataMappings[i].value = resultBuffer.data[3] / 200.0;
            shortTermFuelTrim1 = (100.0 / 128) * resultBuffer.data[4] - 100.0;
          } else if (pid == OXYGEN_SENSOR_2_A) {
            liveDataMappings[i].value = resultBuffer.data[3] / 200.0;
            shortTermFuelTrim2 = (100.0 / 128) * resultBuffer.data[4] - 100.0;
          } else if (pid == OXYGEN_SENSOR_3_A) {
            liveDataMappings[i].value = resultBuffer.data[3] / 200.0;
            shortTermFuelTrim3 = (100.0 / 128) * resultBuffer.data[4] - 100.0;
          } else if (pid == OXYGEN_SENSOR_4_A) {
            liveDataMappings[i].value = resultBuffer.data[3] / 200.0;
            shortTermFuelTrim4 = (100.0 / 128) * resultBuffer.data[4] - 100.0;
          } else if (pid == OXYGEN_SENSOR_5_A) {
            liveDataMappings[i].value = resultBuffer.data[3] / 200.0;
            shortTermFuelTrim5 = (100.0 / 128) * resultBuffer.data[4] - 100.0;
          } else if (pid == OXYGEN_SENSOR_6_A) {
            liveDataMappings[i].value = resultBuffer.data[3] / 200.0;
            shortTermFuelTrim6 = (100.0 / 128) * resultBuffer.data[4] - 100.0;
          } else if (pid == OXYGEN_SENSOR_7_A) {
            liveDataMappings[i].value = resultBuffer.data[3] / 200.0;
            shortTermFuelTrim7 = (100.0 / 128) * resultBuffer.data[4] - 100.0;
          } else if (pid == OXYGEN_SENSOR_8_A) {
            liveDataMappings[i].value = resultBuffer.data[3] / 200.0;
            shortTermFuelTrim8 = (100.0 / 128) * resultBuffer.data[4] - 100.0;
          } else if (pid == OBD_STANDARDS) {
            liveDataMappings[i].value = resultBuffer.data[3];
          } else if (pid == OXYGEN_SENSORS_PRESENT_4_BANKS) {
            liveDataMappings[i].value = resultBuffer.data[3];
          } else if (pid == AUX_INPUT_STATUS) {
            liveDataMappings[i].value = resultBuffer.data[3];
          } else if (pid == RUN_TIME_SINCE_ENGINE_START) {
            liveDataMappings[i].value = 256 * resultBuffer.data[3] + resultBuffer.data[4];
          } else if (pid == DISTANCE_TRAVELED_WITH_MIL_ON) {
            liveDataMappings[i].value = 256 * resultBuffer.data[3] + resultBuffer.data[4];
          } else if (pid == SUPPORTED_PIDS_1_20) {
            int pidIndex = 0;
            int supportedCount = 0;

            for (int i = 3; i <= 6; i++) {
              byte value = resultBuffer.data[i];
              for (int bit = 7; bit >= 0; bit--) {
                if ((value >> bit) & 1) {
                  supportedLiveData[supportedCount++] = pidIndex + 1;
                }
                pidIndex++;
              }
            }

            if (isInArray(supportedLiveData, sizeof(supportedLiveData), SUPPORTED_PIDS_21_40)) {
              writeData(read_LiveData, SUPPORTED_PIDS_21_40);
              if (readData()) {

                for (int i = 3; i < 6; i++) {
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

            if (isInArray(supportedLiveData, sizeof(supportedLiveData), SUPPORTED_PIDS_41_60)) {
              writeData(read_LiveData, SUPPORTED_PIDS_41_60);
              if (readData()) {

                for (int i = 3; i < 6; i++) {
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

            if (isInArray(supportedLiveData, sizeof(supportedLiveData), SUPPORTED_PIDS_61_80)) {
              writeData(read_LiveData, SUPPORTED_PIDS_61_80);
              if (readData()) {

                for (int i = 3; i < 6; i++) {
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

            if (isInArray(supportedLiveData, sizeof(supportedLiveData), 0x80)) {
              writeData(read_LiveData, 0x80);
              if (readData()) {

                for (int i = 3; i < 6; i++) {
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
      }
    }
  }

  sendDataToServer();
}

void getFreezeFrame(const byte pid) {
  // example Request: C3 33 F1 02 05 00 EE
  // example Response: 84 F1 11 42 05 00 8A 57
  writeData(read_FreezeFrame, pid);

  if (readData()) {
    if (resultBuffer.data[2] == pid) {
      for (int i = 0; i < 64; i++) {
        if (freezeFrameMappings[i].pid == pid) {
          if (pid == FUEL_SYSTEM_STATUS) {
            freezeFrameMappings[i].value = resultBuffer.data[4];
          } else if (pid == ENGINE_LOAD) {
            freezeFrameMappings[i].value = (100.0 / 255) * resultBuffer.data[4];
          } else if (pid == ENGINE_COOLANT_TEMP) {
            freezeFrameMappings[i].value = resultBuffer.data[4] - 40;
          } else if (pid == SHORT_TERM_FUEL_TRIM_BANK_1) {
            freezeFrameMappings[i].value = (resultBuffer.data[4] / 1.28) - 100.0;
          } else if (pid == LONG_TERM_FUEL_TRIM_BANK_1) {
            freezeFrameMappings[i].value = (resultBuffer.data[4] / 1.28) - 100.0;
          } else if (pid == SHORT_TERM_FUEL_TRIM_BANK_2) {
            freezeFrameMappings[i].value = (resultBuffer.data[4] / 1.28) - 100.0;
          } else if (pid == LONG_TERM_FUEL_TRIM_BANK_2) {
            freezeFrameMappings[i].value = (resultBuffer.data[4] / 1.28) - 100.0;
          } else if (pid == FUEL_PRESSURE) {
            freezeFrameMappings[i].value = 3 * resultBuffer.data[4];
          } else if (pid == INTAKE_MANIFOLD_ABS_PRESSURE) {
            freezeFrameMappings[i].value = resultBuffer.data[4];
          } else if (pid == ENGINE_RPM) {
            freezeFrameMappings[i].value = (256 * resultBuffer.data[4] + resultBuffer.data[5]) / 4;
          } else if (pid == VEHICLE_SPEED) {
            freezeFrameMappings[i].value = resultBuffer.data[4];
          } else if (pid == TIMING_ADVANCE) {
            freezeFrameMappings[i].value = (resultBuffer.data[4] / 2) - 64;
          } else if (pid == INTAKE_AIR_TEMP) {
            freezeFrameMappings[i].value = resultBuffer.data[4] - 40;
          } else if (pid == MAF_FLOW_RATE) {
            freezeFrameMappings[i].value = (256 * resultBuffer.data[4] + resultBuffer.data[5]) / 100.0;
          } else if (pid == THROTTLE_POSITION) {
            freezeFrameMappings[i].value = (100.0 / 255) * resultBuffer.data[4];
          } else if (pid == COMMANDED_SECONDARY_AIR_STATUS) {
            freezeFrameMappings[i].value = resultBuffer.data[4];
          } else if (pid == OXYGEN_SENSORS_PRESENT_2_BANKS) {
            freezeFrameMappings[i].value = resultBuffer.data[4];
          } else if (pid == OXYGEN_SENSOR_1_A) {
            freezeFrameMappings[i].value = resultBuffer.data[4] / 200.0;
            shortTermFuelTrim1 = (100.0 / 128) * resultBuffer.data[5] - 100.0;
          } else if (pid == OXYGEN_SENSOR_2_A) {
            freezeFrameMappings[i].value = resultBuffer.data[4] / 200.0;
            shortTermFuelTrim2 = (100.0 / 128) * resultBuffer.data[5] - 100.0;
          } else if (pid == OXYGEN_SENSOR_3_A) {
            freezeFrameMappings[i].value = resultBuffer.data[4] / 200.0;
            shortTermFuelTrim3 = (100.0 / 128) * resultBuffer.data[5] - 100.0;
          } else if (pid == OXYGEN_SENSOR_4_A) {
            freezeFrameMappings[i].value = resultBuffer.data[4] / 200.0;
            shortTermFuelTrim4 = (100.0 / 128) * resultBuffer.data[5] - 100.0;
          } else if (pid == OXYGEN_SENSOR_5_A) {
            freezeFrameMappings[i].value = resultBuffer.data[4] / 200.0;
            shortTermFuelTrim5 = (100.0 / 128) * resultBuffer.data[5] - 100.0;
          } else if (pid == OXYGEN_SENSOR_6_A) {
            freezeFrameMappings[i].value = resultBuffer.data[4] / 200.0;
            shortTermFuelTrim6 = (100.0 / 128) * resultBuffer.data[5] - 100.0;
          } else if (pid == OXYGEN_SENSOR_7_A) {
            freezeFrameMappings[i].value = resultBuffer.data[4] / 200.0;
            shortTermFuelTrim7 = (100.0 / 128) * resultBuffer.data[5] - 100.0;
          } else if (pid == OXYGEN_SENSOR_8_A) {
            freezeFrameMappings[i].value = resultBuffer.data[4] / 200.0;
            shortTermFuelTrim8 = (100.0 / 128) * resultBuffer.data[5] - 100.0;
          } else if (pid == OBD_STANDARDS) {
            freezeFrameMappings[i].value = resultBuffer.data[4];
          } else if (pid == OXYGEN_SENSORS_PRESENT_4_BANKS) {
            freezeFrameMappings[i].value = resultBuffer.data[4];
          } else if (pid == AUX_INPUT_STATUS) {
            freezeFrameMappings[i].value = resultBuffer.data[4];
          } else if (pid == RUN_TIME_SINCE_ENGINE_START) {
            freezeFrameMappings[i].value = 256 * resultBuffer.data[4] + resultBuffer.data[5];
          } else if (pid == DISTANCE_TRAVELED_WITH_MIL_ON) {
            freezeFrameMappings[i].value = 256 * resultBuffer.data[4] + resultBuffer.data[5];
          }
        }
      }
    }
  }
  sendDataToServer();
}

void get_DTCs() {
  // Request: C1 33 F1 03 F3
  // example Response: 87 F1 11 43 01 70 01 34 00 00 72
  int dtcs = 0;
  char dtcBytes[2];

  writeData(read_DTCs, 0x00);
  if (readData()) {

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
    sendDataToServer();
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

// void getVIN() {
//   // Request: C2 33 F1 09 02 F1
//   // example Response: 87 F1 11 49 02 01 00 00 00 31 06
//   //                   87 F1 11 49 02 02 41 31 4A 43 D5
//   //                   87 F1 11 49 02 03 35 34 34 34 A8
//   //                   87 F1 11 49 02 04 52 37 32 35 C8

//   byte VIN_Array[17];
//   int arrayNum = 0;

//   if (protocol == "ISO9141") {
//     writeData(vehicle_info_SLOW, sizeof(vehicle_info_SLOW), read_VIN);
//   } else if (protocol == "ISO14230_Fast" || protocol == "ISO14230_Slow") {
//     writeData(vehicle_info, sizeof(vehicle_info), read_VIN);
//   }

//   //delay(200);
//   readData();

//   if (resultBuffer[5] == 0x01) {
//     VIN_Array[arrayNum++] = resultBuffer[9];
//     for (int j = 0; j < 4; j++) {
//       for (int i = 1; i <= 4; i++) {
//         VIN_Array[arrayNum++] = resultBuffer[i + 16 + j * 11];
//       }
//     }
//   }

//   Vehicle_VIN = convertHexToAscii(VIN_Array, sizeof(VIN_Array));
// }

// void getCalibrationID() {
//   // Request: C2 33 F1 09 04 F3
//   // example Response: 87 F1 11 49 04 01 4F 32 43 44 DF
//   //                   87 F1 11 49 04 02 31 30 31 41 AB
//   //                   87 F1 11 49 04 03 00 00 00 00 D9
//   //                   87 F1 11 49 04 04 00 00 00 00 DA

//   byte ID_Array[64];
//   int ID_messageCount;
//   int arrayNum = 0;

//   if (protocol == "ISO9141") {
//     writeData(vehicle_info_SLOW, sizeof(vehicle_info_SLOW), read_ID_Length);
//   } else if (protocol == "ISO14230_Fast" || protocol == "ISO14230_Slow") {
//     writeData(vehicle_info, sizeof(vehicle_info), read_ID_Length);
//   }
//   //delay(200);
//   readData();

//   ID_messageCount = resultBuffer[5];

//   if (protocol == "ISO9141") {
//     writeData(vehicle_info_SLOW, sizeof(vehicle_info_SLOW), read_ID);
//   } else if (protocol == "ISO14230_Fast" || protocol == "ISO14230_Slow") {
//     writeData(vehicle_info, sizeof(vehicle_info), read_ID);
//   }
//   //delay(200);
//   readData();


//   if (resultBuffer[5] == 0x01) {
//     for (int j = 0; j < ID_messageCount; j++) {
//       for (int i = 1; i <= 4; i++) {
//         ID_Array[arrayNum++] = resultBuffer[i + 5 + j * 11];
//       }
//     }
//   }
//   Vehicle_ID = convertHexToAscii(ID_Array, arrayNum);
// }

// void getCalibrationIDNum() {
//   // Request: C2 33 F1 09 06 F5
//   // example Response: 87 F1 11 49 06 01 00 00 67 0C 4C

//   byte IDNum_Array[16];
//   int ID_messageCount;
//   int arrayNum = 0;

//   if (protocol == "ISO9141") {
//     writeData(vehicle_info_SLOW, sizeof(vehicle_info_SLOW), read_ID_Num_Length);
//   } else if (protocol == "ISO14230_Fast" || protocol == "ISO14230_Slow") {
//     writeData(vehicle_info, sizeof(vehicle_info), read_ID_Num_Length);
//   }
//   //delay(200);
//   readData();
//   ID_messageCount = resultBuffer[5];

//   if (protocol == "ISO9141") {
//     writeData(vehicle_info_SLOW, sizeof(vehicle_info_SLOW), read_ID_Num);
//   } else if (protocol == "ISO14230_Fast" || protocol == "ISO14230_Slow") {
//     writeData(vehicle_info, sizeof(vehicle_info), read_ID_Num);
//   }
//   //delay(200);
//   readData();

//   if (resultBuffer[5] == 0x01) {
//     for (int j = 0; j < ID_messageCount; j++) {
//       for (int i = 1; i <= 4; i++) {
//         IDNum_Array[arrayNum++] = resultBuffer[i + 5 + j * 11];
//       }
//     }
//   }
//   Vehicle_ID_Num = convertBytesToHexString(IDNum_Array, arrayNum);
// }

void getSupportedPIDs(const byte option) {
  int pidIndex = 0;
  int supportedCount = 0;

  if (option == 0x01) {
    writeData(option, SUPPORTED_PIDS_1_20);

    if (readData()) {
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
        if (readData()) {

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
        if (readData()) {

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

      memcpy(desiredLiveData, supportedLiveData, sizeof(supportedLiveData));
    }
  }
  if (option == 0x02) {
    writeData(option, SUPPORTED_PIDS_1_20);

    if (readData()) {
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

    if (readData()) {
      for (int i = 4; i < 8; i++) {
        byte value = resultBuffer.data[i];
        for (int bit = 7; bit >= 0; bit--) {
          if ((value >> bit) & 1) {
            supportedVehicleInfo[supportedCount++] = pidIndex + 1;
          }
          pidIndex++;
        }
      }
    }
  }
}
