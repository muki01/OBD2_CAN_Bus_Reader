void read_CAN() {
  sendRPMQuery();
  delay(1000); 
  receiveRPMResponse();
}

void init_CAN() {
  Serial.println("Initializing TWAI...");
  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(TX_GPIO_NUM, RX_GPIO_NUM, TWAI_MODE_NORMAL);
  twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS(); // Set to 500 kbps
  twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL(); // Accept all messages

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
void sendRPMQuery() {
  twai_message_t message;

  message.identifier = 0x7DF;      // Standard OBD-II query ID
  message.extd = 0;                // Using standard frame
  message.data_length_code = 8;    // 8-byte data frame
  message.data[0] = 0x02;          // Query length (2 bytes: Mode and PID)
  message.data[1] = 0x01;          // Mode 01: Request current data
  message.data[2] = 0x0C;          // PID 0C: RPM
  message.data[3] = 0x00;          // Padding bytes
  message.data[4] = 0x00;
  message.data[5] = 0x00;
  message.data[6] = 0x00;
  message.data[7] = 0x00;

  if (twai_transmit(&message, pdMS_TO_TICKS(1000)) == ESP_OK) {
    Serial.println("RPM query sent.");
  } else {
    Serial.println("Failed to send RPM query!");
  }
}

// Process the received response
void receiveRPMResponse() {
  twai_message_t response;

  if (twai_receive(&response, pdMS_TO_TICKS(1000)) == ESP_OK) {
    Serial.print("Response received. ID: 0x");
    Serial.println(response.identifier, HEX);

    // Check if the received message contains RPM response (PID 0C)
    if (response.data[1] == 0x0C) {
      uint16_t rpm = (response.data[3] << 8) | response.data[4]; // Combine bytes
      rpm /= 4; // RPM formula
      Serial.print("Engine RPM: ");
      Serial.println(rpm);
    } else {
      Serial.println("Unexpected data received.");
    }
  } else {
    Serial.println("No response received.");
  }
}
