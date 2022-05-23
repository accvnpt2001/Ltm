// MultithreadTCPEchoServer.cpp : Defines the entry point for the console application.
//

#include "stdio.h"
#include "conio.h"
#include "string.h"
#include "ws2tcpip.h"
#include "winsock2.h"
#include "Session.h"
#include "process.h"
#define SERVER_PORT 5500
#define SERVER_ADDR "127.0.0.1"
#define BUFF_SIZE 2048
#pragma comment (lib, "Ws2_32.lib")

/* echoThread - Thread to receive the message from client and echo*/
unsigned __stdcall echoThread(void *param) {
	char buff[BUFF_SIZE];
	int ret;


	Session session(*(Session *)param);
	SOCKET connectedSocket = session.getSocket();

	while (1) {
		ret = recv(connectedSocket, buff, BUFF_SIZE, 0);
		if (ret == SOCKET_ERROR) {
			printf("Error %d: Cannot receive data.\n", WSAGetLastError());
			break;
		}
		else if (ret == 0) {

			printf("Client disconnects.\n");
			break;
		}
		else if (strlen(buff) > 0) {
			buff[ret] = 0;
			printf("Receive from client[%s:%d] %s\n", session.getClientIp(), session.getPort(), buff);
			//Echo to client
			ret = send(connectedSocket, buff, strlen(buff), 0);
			if (ret == SOCKET_ERROR)
				printf("Error %d: Cannot send data.\n", WSAGetLastError());
		}
	}

	closesocket(connectedSocket);
	return 0;
}

int main(int argc, char* argv[])
{
	//Step 1: Initiate WinSock
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaData)) {
		printf("Winsock 2.2 is not supported\n");
		return 0;
	}

	//Step 2: Construct socket	
	SOCKET listenSock;
	listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//Step 3: Bind address to socket
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_ADDR, &serverAddr.sin_addr);
	if (bind(listenSock, (sockaddr *)&serverAddr, sizeof(serverAddr)))
	{
		printf("Error %d: Cannot associate a local address with server socket.", WSAGetLastError());
		return 0;
	}

	//Step 4: Listen request from client
	if (listen(listenSock, 10)) {
		printf("Error %d: Cannot place server socket in state LISTEN.", WSAGetLastError());
		return 0;
	}

	printf("Server started!\n");

	//Step 5: Communicate with client
	SOCKET connSock;
	sockaddr_in clientAddr;
	char clientIP[INET_ADDRSTRLEN];
	int clientAddrLen = sizeof(clientAddr), clientPort;
	while (1) {
		connSock = accept(listenSock, (sockaddr *)& clientAddr, &clientAddrLen);
		if (connSock == SOCKET_ERROR)
			printf("Error %d: Cannot permit incoming connection.\n", WSAGetLastError());
		else {
			inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, sizeof(clientIP));
			clientPort = ntohs(clientAddr.sin_port);
			printf("Accept incoming connection from %s:%	d\n", clientIP, clientPort);
			Session session = Session(connSock, clientIP, clientPort);
			_beginthreadex(0, 0, echoThread, (void *)&session, 0, 0); //start thread
		}
	}

	closesocket(listenSock);

	WSACleanup();

	return 0;
}
