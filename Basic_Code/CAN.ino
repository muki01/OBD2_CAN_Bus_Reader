twai_message_t messageBuffer[20];
int bufferIndex = 0;

void read_CAN() {
  getPID(0x0C);
  delay(100);
  readCAN();
  processFromBuffer();
}

void init_CAN() {
  Serial.println("Initializing TWAI...");
  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(TX_GPIO_NUM, RX_GPIO_NUM, TWAI_MODE_NORMAL);
  twai_timing_config_t t_config = CAN_SPEED;    // Set to 500 kbps
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

// Send RPM query
void getPID(byte pid) {
  twai_message_t message;

  if (CAN_BIT == 29) {
    message.identifier = 0x18DB33F1;
    message.extd = 1;
  } else if (CAN_BIT == 11) {
    message.identifier = 0x7DF;
    message.extd = 0;
  }

  message.data_length_code = 8;  // 8-byte data frame
  message.data[0] = 0x02;        // Query length (2 bytes: Mode and PID)
  message.data[1] = 0x01;        // Mode 01: Request current data
  message.data[2] = pid;         // PID 0C: RPM
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

void readCAN() {
  twai_message_t response;

  // Get CAN Message
  if (twai_receive(&response, pdMS_TO_TICKS(1000)) == ESP_OK) {
    Serial.print("ID: 0x");
    Serial.print(response.identifier, HEX);

    Serial.print(" RTR");
    Serial.print(response.rtr, HEX);

    Serial.print(" EID: ");
    Serial.print(response.extd, HEX);

    Serial.print(" (DLC): ");
    Serial.print(response.data_length_code);

    Serial.print("Data: ");
    for (int i = 0; i < response.data_length_code; i++) {
      Serial.print("0x");
      Serial.print(response.data[i], HEX);
      if (i < response.data_length_code - 1) {
        Serial.print(" ");
      }
    }


    if (bufferIndex < 20) {
      messageBuffer[bufferIndex] = response;
      bufferIndex++;
      Serial.println("Message stored in buffer.");
    } else {
      Serial.println("Buffer is full, message not stored!");
    }

  } else {
    Serial.println("No message received.");
  }
}

void processFromBuffer() {
  for (int i = 0; i < bufferIndex; i++) {
    twai_message_t message = messageBuffer[i];
    
    if (message.identifier == 0x18DAF110 && message.data[2] == 0x0C) {
      uint16_t rpm = (message.data[3] << 8) | message.data[4];  // Combine bytes
      rpm /= 4;
      Serial.print("Engine RPM from buffer: ");
      Serial.println(rpm);
    } else {
      Serial.println("Non-RPM message in buffer.");
    }
  }

  bufferIndex = 0;
  Serial.println("Buffer cleared.");
}
