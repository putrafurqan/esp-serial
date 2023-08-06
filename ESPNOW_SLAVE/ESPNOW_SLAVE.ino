#include <esp_now.h>
#include <WiFi.h>


// RECEIVED DATA FORMAT
// 98;FL: 356 FR: 101 BR: 44 BL: 914 LM: 180 RM: 375 POSX: 814 POSY: 455;F5DA
// SEQUENCE-IDENTIFIER;DATA;CRC

// SEQUENCE-IDENTIFIER : Integer yang menunjukkan urutan data tersebut dalam pengiriman
// DATA : Data utama yang akan digunakan
// CRC : Key untuk validasi, CRC (Cyclic Redundancy Check) dalam hexadecimal.  
//        Nilai CRC didapat dari menhitung karakteristik data. Setiap data memiliki CRC yang unik        

// CONTOH
// 98;FL: 356 FR: 101 BR: 44 BL: 914 LM: 180 RM: 375 POSX: 814 POSY: 455;F5DA
// DATA URUTAN KE 98
// DATA INTI YANG DIKIRIM 'FL: 356 FR: 101 BR: 44 BL: 914 LM: 180 RM: 375 POSX: 814 POSY: 455'
// CRC dari data 'FL: 356 FR: 101 BR: 44 BL: 914 LM: 180 RM: 375 POSX: 814 POSY: 455' adalah 'F5DA'

// penggunaan SEQUENCE-IDENTIFIER belum diimplementasikan, so sejauh ini SEQUENCE-IDENTIFIER belum kepake di esp slave

#define POLYNOMIAL 0x8408

typedef struct struct_message {
    uint32_t seq_num;
    uint8_t  data[100];
} struct_message;

// Create a struct_message called myData
struct_message myData;

// Fungsi yang menghitung nilai CRC data
uint16_t crc16(uint8_t *data_p, uint16_t length) {
    uint8_t i;
    uint16_t data;
    uint16_t crc = 0xFFFF;

    if (length == 0)
        return (~crc);

    do {
        for (i = 0, data = (uint16_t)0xff & *data_p++; i < 8; i++, data >>= 1) {
            if ((crc & 0x0001) ^ (data & 0x0001))
                crc = (crc >> 1) ^ POLYNOMIAL;
            else
                crc >>= 1;
        }
    } while (--length);

    crc = ~crc;
    data = crc;
    crc = (crc << 8) | (data >> 8 & 0xFF);

    return crc;
}

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

    // Copies the incomingData into myData
    memcpy(&myData, incomingData, len);
    myData.data[len - sizeof(myData.seq_num) - 1] = '\0'; // Ensure null termination

    // Converts the data to a string
    String dataString = String((char*)myData.data);

    // Set ';' as mark
    int firstIndex = dataString.indexOf(';');
    int lastIndex = dataString.lastIndexOf(';');

    if (firstIndex > 0 && lastIndex >0){  // Validasi apakah data yang diterima sesuai format 

      // Extract the data between the semicolons
      // ---;DATA;---
      String onlyData = dataString.substring(firstIndex + 1, lastIndex);

      // Extract the CRC string after the second semicolon
      // ---;---;CRC
      String crcString = dataString.substring(lastIndex + 1);

      // Convert the CRC string to an integer
      uint16_t receivedCrc = (uint16_t) strtol(crcString.c_str(), NULL, 16);

      // Calculate the CRC of onlyData
      uint16_t calculatedCrc = crc16((uint8_t *)onlyData.c_str(), onlyData.length());

      // Compare calculated and received CRCs
      if (calculatedCrc == receivedCrc) {
          Serial.println(onlyData);
      }

    }
    
}



void setup() {
    // Initialize Serial Monitor
    Serial.begin(115200);
  
    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);
  
    // Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
  
    // Register for a callback function that will be called when data is received
    esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
    // nothing
}
