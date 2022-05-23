// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "stdio.h"
#include "string.h"
#include <winsock2.h>
#include <WS2tcpip.h>
#define SERVER_PORT 5500
#define SERVER_ADDR "127.0.0.1"
#define BUFF_SIZE 2048
#define MAX_CLIENT 3
#pragma comment (lib, "Ws2_32.lib")

int main()
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
	unsigned long ul = 1;
	listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSock == INVALID_SOCKET) {
		printf("Error %d: Cannot create server socket.", WSAGetLastError());
		return 0;
	}

	if (ioctlsocket(listenSock, FIONBIO, (unsigned long *)&ul)) {
		printf("Error! Cannot change to non-blocking mode.");
		return 0;
	}

	//Step 3: Bind address to socket
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_ADDR, &serverAddr.sin_addr);

	if (/*Please complete this statement*/bind(listenSock, (sockaddr *)&serverAddr, sizeof(serverAddr)))
	{
		printf("Error %d: Cannot associate a local address with server socket.", WSAGetLastError());
		return 0;
	}

	//Step 4: Listen request from client
	if (/*Please complete this statement*/listen(listenSock, 10)) {
		printf("Error %d: Cannot place server socket in state LISTEN.", WSAGetLastError());
		return 0;
	}

	printf("Server started!");

	//Step 5: Communicate with client
	sockaddr_in clientAddr;
	char buff[BUFF_SIZE];
	int i, ret, clientAddrLen = sizeof(clientAddr);
	SOCKET client[MAX_CLIENT];

	for (i = 0; i < MAX_CLIENT; i++)
		client[i] = 0;

	while (1) {
		SOCKET connSock;

		//accept request
		connSock = /*Please complete this statement*/ accept(listenSock, (sockaddr *)&clientAddr, &clientAddrLen);
			if (connSock != SOCKET_ERROR) {
				for (i = 0; i < MAX_CLIENT; i++)
					if (client[i] == 0) {
						client[i] = connSock;
						break;
					}
				if (i == MAX_CLIENT) {
					printf("Error: Cannot response more client.\n");
					closesocket(connSock);
				}
			}

		int errorCode;
		for (i = 0; i < MAX_CLIENT; i++) {
			if (client[i] == 0) continue;

			//receive message from client
			ret = recv(client[i], buff, BUFF_SIZE, 0);
			if (ret == SOCKET_ERROR) {
				errorCode = WSAGetLastError();
				if (errorCode != WSAEWOULDBLOCK) {
					printf("Error %d: Cannot receive data\n", errorCode);
					closesocket(client[i]);
					client[i] = 0;
				}

			}
			else if (ret == 0) {
				printf("Client disconnects");
				closesocket(client[i]);
				client[i] = 0;
			}
			else {
				buff[ret] = 0;
				printf("Receive from client[%s:%d] %s\n",
					inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), buff);

				//Echo to client
				ret = send(client[i], buff, strlen(buff), 0);
				if (ret == SOCKET_ERROR) {
					errorCode = WSAGetLastError();
					if (errorCode != WSAEWOULDBLOCK) {
						printf("Error %d: Cannot send data\n", errorCode);
						closesocket(client[i]);
						client[i] = 0;
					}
				}
			}
		}
	} //end accepting

	  //Step 5: Close socket
	closesocket(listenSock);

	//Step 6: Terminate Winsock
	WSACleanup();

	return 0;
}
