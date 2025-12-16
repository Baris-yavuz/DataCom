#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib,"ws2_32.lib")
#define PORT 8081

// --- 1. PARITY ---
void calculate_parity(const char* data, char* result) {
    int ones = 0;
    int len = strlen(data);
    for (int i = 0; i < len; i++) {
        unsigned char c = data[i];
        for (int j = 0; j < 8; j++) if ((c >> j) & 1) ones++;
    }
    if (ones % 2 == 0) sprintf(result, "0"); else sprintf(result, "1");
}

// --- 2. 2D PARITY ---
void calculate_2d_parity(const char* data, char* result) {
    unsigned char lrc = 0;
    while (*data) lrc ^= *data++;
    sprintf(result, "%02X", lrc);
}

// --- 3. CRC-16 ---
unsigned short calculate_crc16(const char* data, int length) {
    unsigned short crc = 0xFFFF;
    for (int i = 0; i < length; i++) {
        crc ^= (unsigned char)data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x0001) crc = (crc >> 1) ^ 0xA001; else crc = crc >> 1;
        }
    }
    return crc;
}

// --- 4. HAMMING ---
void calculate_hamming(const char* data, char* result) {
    unsigned char val = 0;
    int len = strlen(data);
    for(int i=0; i<len; i++) val ^= data[i];
    int d0 = (val >> 0) & 1;
    int d1 = (val >> 1) & 1;
    int d2 = (val >> 2) & 1;
    int d3 = (val >> 3) & 1;
    int p1 = d0 ^ d1 ^ d3;
    int p2 = d0 ^ d2 ^ d3;
    int p4 = d1 ^ d2 ^ d3;
    sprintf(result, "%d%d%d", p1, p2, p4);
}

// --- 5. CHECKSUM ---
unsigned int calculate_checksum(const char* data) {
    unsigned int sum = 0;
    while (*data) sum += *data++;
    return sum;
}

int main() {
    WSADATA wsa;
    SOCKET server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024];

    WSAStartup(MAKEWORD(2,2), &wsa);
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) return 1;

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    printf("CLIENT 2: Dinleniyor (Port %d)...\n", PORT);

    while(1) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        memset(buffer, 0, 1024);
        int valread = recv(new_socket, buffer, 1024, 0);
        
        if (valread > 0) {
            printf("\n----------------------------------\n");
            printf("GELEN PAKET: %s\n", buffer);

            char temp_buffer[1024];
            strcpy(temp_buffer, buffer);

            char *data_part = strtok(temp_buffer, "|");
            char *method_part = strtok(NULL, "|");
            char *control_part = strtok(NULL, "|");

            if (data_part && method_part && control_part) {
                char calculated_hex[20];
                
                // --- METHOD KONTROLU ---
                if (strcmp(method_part, "PARITY") == 0) 
                    calculate_parity(data_part, calculated_hex);
                else if (strcmp(method_part, "2DPARITY") == 0) 
                    calculate_2d_parity(data_part, calculated_hex);
                else if (strcmp(method_part, "CRC16") == 0) {
                    unsigned short crc = calculate_crc16(data_part, strlen(data_part));
                    sprintf(calculated_hex, "%X", crc);
                } 
                else if (strcmp(method_part, "HAMMING") == 0) 
                    calculate_hamming(data_part, calculated_hex);
                else if (strcmp(method_part, "CHECKSUM") == 0) {
                    unsigned int csum = calculate_checksum(data_part);
                    sprintf(calculated_hex, "%X", csum);
                }

                printf("Metod: %s\n", method_part);
                printf("Hesaplanan: %s | Paketten Cikan: %s\n", calculated_hex, control_part);

                if (strcmp(calculated_hex, control_part) == 0) 
                    printf("SONUC: [ DATA CORRECT ]\n");
                else 
                    printf("SONUC: [ DATA CORRUPTED ] - Hata Var!\n");
            }
        }
        closesocket(new_socket);
    }
    closesocket(server_fd);
    WSACleanup();
    return 0;
}
