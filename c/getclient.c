#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "80"

void send_get_request(const char *ip_address) {
    WSADATA wsaData;
    SOCKET sock;
    struct addrinfo hints, *result;
    
    // Инициализация Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed: %d\n", WSAGetLastError());
        exit(1);
    }
    
    // Установка информации о соединении
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_protocol = IPPROTO_TCP; // TCP

    // Получение адресов
    if (getaddrinfo(ip_address, DEFAULT_PORT, &hints, &result) != 0) {
        fprintf(stderr, "getaddrinfo failed: %d\n", WSAGetLastError());
        WSACleanup();
        exit(1);
    }
    
    // Создание сокета
    sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sock == INVALID_SOCKET) {
        fprintf(stderr, "socket failed: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        exit(1);
    }
    
    // Подключение к серверу
    if (connect(sock, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
        closesocket(sock);
        sock = INVALID_SOCKET;
    }
    
    freeaddrinfo(result); // Освобождаем адреса

    if (sock == INVALID_SOCKET) {
        fprintf(stderr, "Unable to connect to server!\n");
        WSACleanup();
        exit(1);
    }

    // Формирование GET-запроса
    const char *request_format = "GET / HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n";
    char request[256];
    snprintf(request, sizeof(request), request_format, ip_address);

    // Отправка GET-запроса
    send(sock, request, strlen(request), 0);

    // Получение и вывод ответа
    char response[4096];
    int bytes_received;
    while ((bytes_received = recv(sock, response, sizeof(response) - 1, 0)) > 0) {
        response[bytes_received] = '\0'; // Нулевой терминатор
        printf("%s", response);
    }

    if (bytes_received < 0) {
        fprintf(stderr, "recv failed: %d\n", WSAGetLastError());
    }

    // Закрытие сокета
    closesocket(sock);
    WSACleanup();
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <ip_address>\n", argv[0]);
        return 1;
    }

    send_get_request(argv[1]);
    return 0;
}
