#include "mbed.h"



// Serial communication
BufferedSerial serial_port(USBTX, USBRX, 115200);


#define DATA_LENGTH 300
#define POLYNOMIAL 0x8408
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

int main() {
    while (1) {
        int random_number1 = (std::rand() % 999) + 1;
        int random_number2 = (std::rand() % 999) + 1;
        int random_number3 = (std::rand() % 999) + 1;
        int random_number4 = (std::rand() % 999) + 1;
        int random_number5 = (std::rand() % 999) + 1;
        int random_number6 = (std::rand() % 999) + 1;
        int random_number7 = (std::rand() % 999) + 1;
        int random_number8 = (std::rand() % 999) + 1;
        

        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~AMBIL AREA INI UNTUK PRINTING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        // Create the formatted string
        char data[DATA_LENGTH];

        /// DATA TARO DI SINIIIII
        /// ANGGAP AJA KAYAK PRINTF BIASAAAAA
        // JANGAN GANTI BAGIAN 'data, sizeof(data)'
        snprintf(data, sizeof(data), "FL: %d FR: %d BR: %d BL: %d LM: %d RM: %d POSX: %d POSY: %d", 
                 random_number1, random_number2, random_number3, random_number4, 
                 random_number5, random_number6, random_number7, random_number8);

        uint16_t checksum = crc16((uint8_t *)data, strlen(data));

        // Prepare the buffer with data and CRC
        char buffer[DATA_LENGTH + 5]; // Data length + CRC length + delimiter
        snprintf(buffer, sizeof(buffer), "%s;%04X\n", data, checksum);

        // Send the buffer
        serial_port.write(buffer, strlen(buffer));

        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    }
    return 0;
}
