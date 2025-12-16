#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib,"ws2_32.lib")

#define PORT 8080

// --- 1. PARITY (EVEN) ---
void calculate_parity(const char* data, char* result) {
    int ones = 0;
    int len = strlen(data);
    for (int i = 0; i < len; i++) {
        unsigned char c = data[i];
        for (int j = 0; j < 8; j++) if ((c >> j) & 1) ones++;
    }
    if (ones % 2 == 0) sprintf(result, "0"); else sprintf(result, "1");
}

// --- 2. 2D PARITY (LRC - Column Parity) ---
// Karakterlerin dikey (sutun) paritesini hesaplar (XOR Sum)
void calculate_2d_parity(const char* data, char* result) {
    unsigned char lrc = 0;
    while (*data) {
        lrc ^= *data++;
    }
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

// --- 4. HAMMING CODE (Signature) ---
// Verinin Hamming kontrol bitlerini hesaplar (Basitlestirilmis Ýmza)
void calculate_hamming(const char* data, char* result) {
    // Veriyi temsil eden kisa bir imza uretmek icin once XOR toplami alalim
    unsigned char val = 0;
    int len = strlen(data);
    for(int i=0; i<len; i++) val ^= data[i];

    // Bu byte icin Hamming (7,4) P1, P2, P4 bitlerini hesaplayalim
    // D7 D6 D5 D4 | D3 D2 D1 D0 (Basitlik icin alt 4 bite bakalim)
    int d0 = (val >> 0) & 1;
    int d1 = (val >> 1) & 1;
    int d2 = (val >> 2) & 1;
    int d3 = (val >> 3) & 1;

    // Hamming Parity Denklemleri
    int p1 = d0 ^ d1 ^ d3;
    int p2 = d0 ^ d2 ^ d3;
    int p4 = d1 ^ d2 ^ d3;

    // Kontrol kodunu olustur (p1, p2, p4 yan yana)
    sprintf(result, "%d%d%d", p1, p2, p4);
}

// --- 5. INTERNET CHECKSUM ---
unsigned int calculate_checksum(const char* data) {
    unsigned int sum = 0;
    while (*data) sum += *data++;
    return sum;
}

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in serv_addr;
    char message[512], packet[1024], code[20];
    int choice;

    WSAStartup(MAKEWORD(2,2), &wsa);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(PORT);

    while(1) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        printf("\n----------------------------------\n");
        printf("Mesaj Girin (Cikmak icin 'exit'): ");
        
        fflush(stdin);
        fgets(message, 512, stdin);
        message[strcspn(message, "\n")] = 0; 

        if (strcmp(message, "exit") == 0) break;

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            printf("Baglanti hatasi. Server kapali olabilir.\n");
            closesocket(sock);
            continue;
        }

        printf("Metod Secin:\n");
        printf("1. Parity (Even)\n");
        printf("2. 2D Parity\n");
        printf("3. CRC-16\n");
        printf("4. Hamming Code\n");
        printf("5. Internet Checksum\n");
        printf("Secim (1-5): ");
        scanf("%d", &choice);

        if (choice == 1) {
            calculate_parity(message, code);
            sprintf(packet, "%s|PARITY|%s", message, code);
        } else if (choice == 2) {
            calculate_2d_parity(message, code);
            sprintf(packet, "%s|2DPARITY|%s", message, code);
        } else if (choice == 3) {
            unsigned short crc = calculate_crc16(message, strlen(message));
            sprintf(code, "%X", crc);
            sprintf(packet, "%s|CRC16|%s", message, code);
        } else if (choice == 4) {
            calculate_hamming(message, code);
            sprintf(packet, "%s|HAMMING|%s", message, code);
        } else if (choice == 5) {
            unsigned int csum = calculate_checksum(message);
            sprintf(code, "%X", csum);
            sprintf(packet, "%s|CHECKSUM|%s", message, code);
        } else {
            printf("Gecersiz secim! Varsayilan CRC kullaniliyor.\n");
            unsigned short crc = calculate_crc16(message, strlen(message));
            sprintf(code, "%X", crc);
            sprintf(packet, "%s|CRC16|%s", message, code);
        }

        send(sock, packet, strlen(packet), 0);
        printf("Paket Gonderildi: %s\n", packet);
        closesocket(sock);
    }
    WSACleanup();
    return 0;
}
