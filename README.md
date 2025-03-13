# network_programming
Network Programming / by Alexeev Bronislav

## Windows Sockets

1. Подключение необходимых библиотек: В начале мы подключаем библиотеки, которые нам нужны. winsock2.h — это библиотека, которая предоставляет функции для работы с сокетами в Windows.

2. Инициализация Winsock: Перед тем, как мы начнем использовать сокеты, необходимо инициализировать библиотеку Winsock, что мы и делаем с помощью функции WSAStartup. Если инициализация завершилась неудачей, программа завершится с ошибкой.

3. Настройка соединения: Мы создаем структуру addrinfo, которая содержит информацию о типе соединения, протоколе и других параметрах. В нашем случае мы хотим использовать IPv4 и TCP.

4. Получение адресов: С помощью getaddrinfo мы запрашиваем IP-адрес для заданного хоста. Эта функция превращает доменное имя или IP-адрес в структуру, содержащую информацию об адресе, необходимую для подключения.

5. Создание сокета: Мы создаем сокет с помощью функции socket. Это своего рода "канал" для связи между компьютерами. Если создание сокета не удалось, программа также завершится с ошибкой.

6. Подключение к серверу: Теперь, когда у нас есть сокет, мы можем подключиться к серверу с помощью функции connect, используя адрес, полученный ранее. Если подключение не удалось, сокет будет закрыт, и снова выведем сообщение об ошибке.

7. Формирование GET-запроса: Мы создаем строку с нашим GET-запросом к серверу. Этот запрос говорит серверу, что мы хотим получить обычно главный документ (например, HTML-страницу) с заданного адреса.

8. Отправка запроса: С помощью функции send мы отправляем сформированный GET-запрос на сервер.

9. Получение ответа: После отправки запроса мы будем получать ответ от сервера. Мы используем recv, чтобы получать данные в цикле, пока сервер отправляет нам информацию. Мы добавляем нулевой терминатор к строке, чтобы сделать её удобной для печати.

10. Закрытие сокета: После завершения работы с сокетом мы его закрываем и очищаем ресурсы с помощью функции WSACleanup.

11. Главная функция: В main мы проверяем, передан ли аргумент командной строки (IP-адрес). Если нет, программа завершится с выводом сообщения о правильном использовании. Затем вызываем функцию, которая отправляет GET-запрос на указанный IP-адрес.

В итоге код делает следующее: берет IP-адрес, подключается к серверу, отправляет GET-запрос и получает ответ от сервера, который затем печатает на экран. Это основной принцип работы с сокетами — мы устанавливаем соединение, отправляем и получаем данные между клиентом и сервером.

```c
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
```

Compiling:

```bash
gcc -o http_client http_client.c -lws2_32
\getclient.exe 188.184.67.127
```
