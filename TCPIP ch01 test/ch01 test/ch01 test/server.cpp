// simple_tcp_server.cpp
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <cstdlib>
#include <string>

#pragma comment(lib, "Ws2_32.lib") // MSVC 자동 링크

using namespace std;

void ErrorHandling(const char* message);   // 에러 출력 후 종료

int main(int argc, char* argv[])
{
    unsigned short port = 8080; // 기본 포트

    WSADATA wsa;
    SOCKET servSock;
    SOCKADDR_IN servAddr;
    SOCKADDR_IN clntAddr;
    string msg;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        ErrorHandling("WSAStartup failed");
    }

    servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (servSock == INVALID_SOCKET) {
        ErrorHandling("socket() failed");
    }

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);

    if (bind(servSock, reinterpret_cast<SOCKADDR*>(&servAddr), sizeof(servAddr)) == SOCKET_ERROR) {
        ErrorHandling("bind() failed");
    }

    if (listen(servSock, SOMAXCONN) == SOCKET_ERROR) {
        ErrorHandling("listen() failed");
    }

    cout << "Listening on port " << port << " ...\n";

    int clntAddrSize = sizeof(clntAddr);
    SOCKET clntSock = accept(servSock, reinterpret_cast<SOCKADDR*>(&clntAddr), &clntAddrSize);
    if (clntSock == INVALID_SOCKET) {
        ErrorHandling("accept() failed");
    }

    cout << "Accepted connection from "
        << inet_ntoa(clntAddr.sin_addr) << ":" << ntohs(clntAddr.sin_port) << "\n";


    while(1)
    {
        cout << "massage Enter: ";
        getline(cin, msg);

        if (msg == "q")
        {
            break;
        }

        int sent = send(clntSock, msg.data(), msg.size(), 0);
        if (sent == SOCKET_ERROR) {
            cerr << "send() failed: " << WSAGetLastError() << "\n";
        }
        else {
            cout << "Sent " << sent << " bytes.\n";
        }
    }

    closesocket(clntSock);
    closesocket(servSock);
    WSACleanup();
    return 0;
}

void ErrorHandling(const char* message)
{
    cerr << message << " (WSAError: " << WSAGetLastError() << ")\n";
    WSACleanup();
    exit(1);
}
