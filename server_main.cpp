#include <iostream>
#include <winsock2.h>
#include <sys/types.h>
#include <time.h>
#define PORT 999

using namespace std;

int main(void)
{
    char timeBuffer[64];
    char buffer[100];
    int bytesRecieved;
    time_t varTime;
    fd_set master;
    fd_set read_fds;
    int fdmax;
    int newfd;

    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    setlocale(LC_ALL, "Polish");

    cout << "Tworze serwer!" << endl;

    WSADATA wsaData;
    int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (err != 0)
    {
        cout << "wsaStartup nieudane" << endl;
        exit(1);
    }

    // Socket

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("tworzenie gniazda nie udane");
        exit(1);
    }

    // Bind

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT); // PORT to numer portu serwera
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        cout << "nieudane zbindowanie portu" << errno << endl;
        exit(EXIT_FAILURE);
    }

    // Listen

    if (listen(server_socket, 10) < 0)
    {
        cout << "nieudanie nasłuchiwanie socketa" << errno << endl;
        exit(1);
    }
    cout << "serwer wystartowal" << endl;

    FD_SET(server_socket, &master);

    fdmax = server_socket;

    while (1)
    {
        read_fds = master;
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1)
        {
            cout << "select error" << WSAGetLastError() << endl;
            exit(1);
        }

        for (int i = 0; i <= fdmax; i++)
        {
            if (FD_ISSET(i, &read_fds))
            {
                if (i == server_socket)
                {
                    int addrlen = sizeof(server_address);
                    if ((newfd = accept(server_socket, (struct sockaddr *)&server_address, &addrlen)) == -1)
                    {
                        cout << "accept error: " << WSAGetLastError() << endl;
                    }
                    else
                    {
                        FD_SET(newfd, &master);
                        if (newfd > fdmax)
                        {
                            fdmax = newfd;
                        }
                        cout << "nowe polaczenie: " << inet_ntoa(server_address.sin_addr) << " socket: " << newfd << endl;
                    }
                }
                else
                {
                    if ((bytesRecieved = recv(i, buffer, sizeof(buffer), 0)) <= 0)
                    {
                        if (bytesRecieved == 0)
                        {
                            cout << "recib" << endl;
                        }
                        else
                        {

                            cout << "socket: " << i << " disconencted" << endl;
                        }
                        closesocket(i);
                        FD_CLR(i, &master);
                    }
                    else
                    {
                        time(&varTime);
                        tm timeTM = *localtime(&varTime);
                        strftime(timeBuffer, sizeof(timeBuffer), "%c", &timeTM);
                        cout << timeBuffer << " [" << inet_ntoa(server_address.sin_addr) << "] : " << buffer;
                        cout << endl;
                        memset(buffer,0,sizeof(buffer));
                    }
                }
            }
        }
    }

    WSACleanup();
}
