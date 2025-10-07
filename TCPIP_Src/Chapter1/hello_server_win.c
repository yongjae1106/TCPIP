#include <stdio.h>      // ǥ�� ����� �Լ��� (printf, fprintf ��)
#include <stdlib.h>     // ǥ�� ���̺귯�� (exit, atoi ��)
#include <winsock2.h>   // Windows ���� API ���� (WSAStartup, SOCKET, SOCKADDR_IN ��)

/*
 * ����: Windows���� �� �ڵ� ������/��ũ �� ws2_32 ���̺귯���� ��ũ�ؾ� �մϴ�.
 * - Visual Studio: ������Ʈ ��Ŀ�� ws2_32.lib �߰�
 * - MinGW (gcc): gcc server.c -lws2_32 -o server.exe
 */

void ErrorHandling(char* message);   // ���� ��� �� �����ϴ� �Լ� ����

int main(int argc, char* argv[])
{
    WSADATA wsaData;                // WSAStartup�� ä���ִ� ����ü (Winsock �ʱ�ȭ ����)
    SOCKET hServSock, hClntSock;    // ���� �ڵ� (���� ����, Ŭ���̾�Ʈ ����)
    SOCKADDR_IN servAddr, clntAddr; // IPv4 �ּҸ� ���� ����ü (����, Ŭ���̾�Ʈ)
    int szClntAddr;                 // accept�� ������ �ּ� ���� (Windows������ int* ���)
    char message[] = "Hello World!"; // ������ �޽��� (�� ���� ����)

    // ���α׷� ���� �˻�: ��Ʈ ��ȣ�� �� �� �޾ƾ� ��
    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    // Winsock �ʱ�ȭ: MAKEWORD(2,2)�� Winsock v2.2 ��û
    // �����ϸ� wsaData�� ������ ä����. �����ϸ� ��Ʈ��ũ �Լ� ��� �Ұ�.
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error!");

    // ���� ����:
    // - ù ��° ����: PF_INET (�������� �йи�, IPv4)
    // - �� ��° ����: SOCK_STREAM (��Ʈ�� ���� -> TCP)
    // - �� ��° ����: 0 (�������� �ڵ� ����, TCP�� ���õ�)
    hServSock = socket(PF_INET, SOCK_STREAM, 0);
    if (hServSock == INVALID_SOCKET)
        ErrorHandling("socket() error");

    // servAddr ����ü �ʱ�ȭ: 0���� ä�� �����ϰ� �ʱ�ȭ
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;                 // �ּ� ü��: IPv4
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);   // IP   // INADDR_ANY == 0.0.0.0
    // argv[1]�� ��Ʈ ���ڿ� -> atoi�� ���� ��ȯ -> ��Ʈ��ũ ����Ʈ ������ ��ȯ
    servAddr.sin_port = htons(atoi(argv[1]));

    // bind: ������ ������ Ư�� IP/��Ʈ�� ���ε�
    // (�� ��ȯ) SOCKADDR_IN -> sockaddr* �� �Ѱ���
    if (bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
        ErrorHandling("bind() error");

    // listen: ������ ����->���� ��� ���·� ����
    // �ι�° ����(5)�� backlog, ���� ��⿭�� �ִ� ����(�ü���� ���� �޶��� �� ����)
    if (listen(hServSock, 5) == SOCKET_ERROR)
        ErrorHandling("listen() error");

    // accept: ������ ������ ��ٸ���, ����Ǹ� ���ο� ������ ��ȯ
    // - szClntAddr���� Ŭ���̾�Ʈ �ּ� ����ü ũ�⸦ �־���� ��
    // - accept�� ���ŷ ȣ��: ������ �� ������ ���⼭ ����
    szClntAddr = sizeof(clntAddr);
    hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &szClntAddr);
    if (hClntSock == INVALID_SOCKET)
        ErrorHandling("accept() error");

    // send: Ŭ���̾�Ʈ �������� ������ ����
    // - �� ��° ����: ������ ����Ʈ ��. sizeof(message)�� '\0' ���� ������ ũ��.
    //   (�� "Hello World!\0"�� ���� ����) ������ strlen(message)�� ���⵵ ��.
    send(hClntSock, message, sizeof(message), 0);

    // ���� ����: ���� ����
    closesocket(hClntSock);
    closesocket(hServSock);

    // Winsock ����: WSAStartup���� �Ҵ�� �ڿ� ����
    WSACleanup();

    return 0;
}

// ������ ���� ���/���� �Լ�
void ErrorHandling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
