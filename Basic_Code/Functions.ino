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

byte calculateChecksum(const byte data[], int length) {
  byte checksum = 0;
  for (int i = 0; i < length; i++) {
    checksum += data[i];
  }
  return checksum % 256;
}

bool isInArray(byte arr[], int size, byte value) {
  for (int i = 0; i < size; i++) {
    if (arr[i] == value) {
      return true;
    }
  }
  return false;
}

// int getArrayLength(byte arr[]) {
//   int count = 0;
//   while (arr[count] != 0) {
//     count++;
//   }
//   return count;
// }

String convertHexToAscii(byte* hexBytes, size_t length) {
  String asciiString = "";
  for (int i = 0; i < length; i++) {
    if (hexBytes[i] >= 0x20 && hexBytes[i] <= 0x7E) {
      char asciiChar = (char)hexBytes[i];
      asciiString += asciiChar;
    }
  }
  return asciiString;
}

String convertBytesToHexString(byte* buffer, int length) {
  String hexString = "";
  for (int i = 0; i < length; i++) {
    if (buffer[i] < 0x10) {
      hexString += "0";
    }
    hexString += String(buffer[i], HEX);
  }
  hexString.toUpperCase();
  return hexString;
}

String convertBytesToHexStringWithComma(byte arr[], int length) {
  String hexString = "";
  for (int i = 0; i < length; i++) {
    if (arr[i] == 0) {
      break;
    }
    if (i > 0) {
      hexString += ", ";
    }
    if (arr[i] < 0x10) {
      hexString += "0";
    }
    hexString += String(arr[i], HEX);
  }
  hexString.toUpperCase();
  return hexString;
}

String joinStringsWithComma(String arr[], int length) {
  String result = "";
  for (int i = 0; i < length; i++) {
    if (arr[i] == 0) {
      break;
    }
    if (i > 0) {
      result += ", ";
    }
    result += arr[i];
  }
  return result;
}