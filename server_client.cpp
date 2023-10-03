#include <iostream>
#include <winsock2.h>
#include <sys/types.h>

#define PORT 999

using namespace std;

int main(void)
{
    WSADATA wsaData;
    WSACleanup();
    int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (err != 0)
    {
        cout << "wsaStartup nieudane" << endl;
        WSACleanup();
        exit(1);
    }

    SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET)
    {
        cout << "blad tworzenia gniazda klienta: " << WSAGetLastError() << endl;
        WSACleanup();
        exit(ERROR);
    }

    SOCKADDR_IN server_address;

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(PORT);

    if (connect(client_socket, (SOCKADDR *)&server_address, sizeof(server_address)) < 0)
    {
        cout << "blad laczenia z serwerem: " << WSAGetLastError() << endl;
        WSACleanup();
        exit(ERROR_CONNECTION_INVALID);
    }

    char request[100];

    while (1)
    {
        cin >> request;

        send(client_socket, request, strlen(request), 0);
    }
}