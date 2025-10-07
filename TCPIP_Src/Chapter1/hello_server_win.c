#include <stdio.h>      // 표준 입출력 함수들 (printf, fprintf 등)
#include <stdlib.h>     // 표준 라이브러리 (exit, atoi 등)
#include <winsock2.h>   // Windows 소켓 API 정의 (WSAStartup, SOCKET, SOCKADDR_IN 등)

/*
 * 주의: Windows에서 이 코드 컴파일/링크 시 ws2_32 라이브러리를 링크해야 합니다.
 * - Visual Studio: 프로젝트 링커에 ws2_32.lib 추가
 * - MinGW (gcc): gcc server.c -lws2_32 -o server.exe
 */

void ErrorHandling(char* message);   // 에러 출력 후 종료하는 함수 선언

int main(int argc, char* argv[])
{
    WSADATA wsaData;                // WSAStartup이 채워주는 구조체 (Winsock 초기화 정보)
    SOCKET hServSock, hClntSock;    // 소켓 핸들 (서버 소켓, 클라이언트 소켓)
    SOCKADDR_IN servAddr, clntAddr; // IPv4 주소를 위한 구조체 (서버, 클라이언트)
    int szClntAddr;                 // accept에 전달할 주소 길이 (Windows에서는 int* 사용)
    char message[] = "Hello World!"; // 전송할 메시지 (널 문자 포함)

    // 프로그램 인자 검사: 포트 번호를 한 개 받아야 함
    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    // Winsock 초기화: MAKEWORD(2,2)로 Winsock v2.2 요청
    // 성공하면 wsaData에 정보가 채워짐. 실패하면 네트워크 함수 사용 불가.
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error!");

    // 소켓 생성:
    // - 첫 번째 인자: PF_INET (프로토콜 패밀리, IPv4)
    // - 두 번째 인자: SOCK_STREAM (스트림 소켓 -> TCP)
    // - 세 번째 인자: 0 (프로토콜 자동 선택, TCP가 선택됨)
    hServSock = socket(PF_INET, SOCK_STREAM, 0);
    if (hServSock == INVALID_SOCKET)
        ErrorHandling("socket() error");

    // servAddr 구조체 초기화: 0으로 채워 안전하게 초기화
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;                 // 주소 체계: IPv4
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);   // IP   // INADDR_ANY == 0.0.0.0
    // argv[1]은 포트 문자열 -> atoi로 정수 변환 -> 네트워크 바이트 순서로 변환
    servAddr.sin_port = htons(atoi(argv[1]));

    // bind: 생성한 소켓을 특정 IP/포트에 바인딩
    // (형 변환) SOCKADDR_IN -> sockaddr* 로 넘겨줌
    if (bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
        ErrorHandling("bind() error");

    // listen: 소켓을 수동->수신 대기 상태로 변경
    // 두번째 인자(5)는 backlog, 연결 대기열의 최대 길이(운영체제에 따라 달라질 수 있음)
    if (listen(hServSock, 5) == SOCKET_ERROR)
        ErrorHandling("listen() error");

    // accept: 들어오는 연결을 기다리고, 연결되면 새로운 소켓을 반환
    // - szClntAddr에는 클라이언트 주소 구조체 크기를 넣어줘야 함
    // - accept는 블로킹 호출: 연결이 올 때까지 여기서 멈춤
    szClntAddr = sizeof(clntAddr);
    hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &szClntAddr);
    if (hClntSock == INVALID_SOCKET)
        ErrorHandling("accept() error");

    // send: 클라이언트 소켓으로 데이터 전송
    // - 세 번째 인자: 전송할 바이트 수. sizeof(message)는 '\0' 까지 포함한 크기.
    //   (즉 "Hello World!\0"의 길이 전송) 보통은 strlen(message)를 쓰기도 함.
    send(hClntSock, message, sizeof(message), 0);

    // 연결 종료: 소켓 닫음
    closesocket(hClntSock);
    closesocket(hServSock);

    // Winsock 정리: WSAStartup으로 할당된 자원 해제
    WSACleanup();

    return 0;
}

// 간단한 에러 출력/종료 함수
void ErrorHandling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
