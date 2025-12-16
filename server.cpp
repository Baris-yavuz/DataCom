#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <winsock2.h>

#pragma comment(lib,"ws2_32.lib")
#define LISTEN_PORT 8080
#define DEST_PORT 8081

void bit_flip(char *data) {
    int len = strlen(data);
    int data_end_index = 0;
    for(int i=0; i<len; i++){
        if(data[i] == '|') { data_end_index = i; break; }
    }
    if (data_end_index > 0) {
        int target = rand() % data_end_index;
        data[target] ^= 1; 
        printf("[LOG] Bit Flip yapildi: '%c' -> index %d\n", data[target], target);
    }
}

int main() {
    WSADATA wsa;
    SOCKET server_fd, new_socket, client2_sock;
    struct sockaddr_in address, client2_addr;
    int addrlen = sizeof(address);
    char buffer[1024];

    srand(time(NULL));
    WSAStartup(MAKEWORD(2,2), &wsa);

    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) return 1;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(LISTEN_PORT);
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    printf("SERVER: (Corruptor) Calisiyor (%d)...\n", LISTEN_PORT);

    while(1) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        memset(buffer, 0, 1024);
        int valread = recv(new_socket, buffer, 1024, 0);

        if (valread > 0) {
            printf("\n--- YENI PAKET GELDI ---\nPacket: %s\n", buffer);
            int choice;
            printf("1. Ilet (Normal)\n2. Bit Flip (Boz)\nSecim: ");
            scanf("%d", &choice);

            if (choice == 2) {
                bit_flip(buffer);
                printf("Bozulmus Paket: %s\n", buffer);
            }

            client2_sock = socket(AF_INET, SOCK_STREAM, 0);
            client2_addr.sin_family = AF_INET;
            client2_addr.sin_port = htons(DEST_PORT);
            client2_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

            if (connect(client2_sock, (struct sockaddr *)&client2_addr, sizeof(client2_addr)) >= 0) {
                send(client2_sock, buffer, strlen(buffer), 0);
                closesocket(client2_sock);
            } else {
                printf("HATA: Client 2 bulunamadi!\n");
            }
        }
        closesocket(new_socket);
    }
    closesocket(server_fd);
    WSACleanup();
    return 0;
}
