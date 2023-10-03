#include <iostream>
#include <winsock2.h>
#include <sys/types.h>
#include <time.h>
#define PORT 999

using namespace std;

int main(void)
{
    char timeBuffer[64];
    time_t varTime;
    setlocale(LC_ALL, "Polish");

    cout << "Tworze serwer!" << endl;

    WSADATA wsaData;
    int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (err != 0)
    {
        cout << "wsaStartup nieudane" << endl;
        exit(1);
    }

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("tworzenie gniazda nie udane");
        exit(1);
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT); // PORT to numer portu serwera
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        cout << "nieudane zbindowanie portu" << errno << endl;
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 10) < 0)
    {
        cout << "nieudanie nasłuchiwanie socketa" << errno << endl;
        exit(1);
    }

    auto addrlen = sizeof(server_address);
    int connection = accept(server_socket, (struct sockaddr *)&server_address, NULL);

    if (connection < 0)
    {
        cout << "nieudane polaczenie" << errno << endl;
        exit(1);
    }

    cout << "serwer wystartowal" << endl;
    while (1)
    {
        char buffer[100] = "";
        int bytesRecieved = recv(connection, buffer, sizeof(buffer), 0);
        if (bytesRecieved < 0)
        {
            cout << "Blad odbierania danych" << endl;
            closesocket(connection);
            WSACleanup();
            exit(1);
        }


        time(&varTime);
        tm timeTM = *localtime(&varTime);
        strftime(timeBuffer, sizeof(timeBuffer), "%c", &timeTM);
        cout << timeBuffer << " [" << inet_ntoa(server_address.sin_addr) << "] : " << buffer;
        cout << endl;
    }

    WSACleanup();
}
