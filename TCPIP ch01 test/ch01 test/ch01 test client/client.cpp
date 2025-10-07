#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib") // MSVC 자동 링크

using namespace std;

int main(int argc, char* argv[])
{
	unsigned short port = 8080; // 기본 포트

	WSADATA wsadata;
	SOCKET sock_client;
	SOCKADDR_IN servaddr;

	char msgBuf[1024];
	string msg;
	int strlen;

	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
	{
		cout << "WSAStartup 오류!\n";
		return 1;
	}

	sock_client = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_client == INVALID_SOCKET)
	{
		cout << "socket 오류!\n";
		return 1;
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[2]));
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);

	if (connect(sock_client, (SOCKADDR*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR)
	{
		cout << "connect 오류!\n";
		return 1;
	}

	cout << "wait server chat..." << endl;

	while(1)
	{
		strlen = recv(sock_client, msgBuf, sizeof(msgBuf) - 1, 0);
		if (strlen == -1)
		{
			cout << "recv 오류!\n";
			return 1;
		}
		msgBuf[strlen] = '\0';
		msg = msgBuf;

		cout << "server chat: " << msg << endl;
	}

	closesocket(sock_client);
	WSACleanup();
	return 0;
}