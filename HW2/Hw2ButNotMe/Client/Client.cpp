#include <stdio.h>
#include <iostream>
#include <string>
#include <ws2tcpip.h>
#include <winsock2.h>
#include "StatusCode.h"
#include "tcp_stream.h"

#pragma comment(lib, "Ws2_32.lib")

#define SERVER_ADDR "127.0.0.1"

using namespace std;


/**
* @brief let user choose service
* @param[out] request
*/
void service(char *request);

int main(int argc, char* argv[]) {
	// Validate the parameters
	if (argc != 3)
	{
		printf("Usage: %s <ServerIpAddress> <ServerPortNumber>\n", argv[0]);
		return 1;
	}

	//Inittiate Winsock
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaData))
		printf("Version is not supported \n");

	//Specify server address
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	char *serverIp = argv[1];
	int serverPort = atoi(argv[2]);
	serverAddr.sin_port = htons(serverPort);
	inet_pton(AF_INET, serverIp, &serverAddr.sin_addr);

	SOCKET connSock;
	connSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (connect(connSock, (sockaddr *)&serverAddr, sizeof(serverAddr))) {
		printf("\nError: %d", WSAGetLastError());
		closesocket(connSock);
		return 0;
	}

	char buff[BUFF_SIZE];
	vector<string> responseList;
	int ret;
	while (1) {
		//Send message
		service(buff);
		ret = sendStream(connSock, buff);
		if (ret == SOCKET_ERROR) {
			printf("Error %d: Cannot send data.\n", WSAGetLastError());
			continue;
		}


		ret = receiveStream(connSock, responseList);
		if (ret == SOCKET_ERROR)
			printf("Error %d: Cannot receive data.\n", WSAGetLastError());
		else {
			for (string rs : responseList)
				//Handle message
				cout << Status::reasonPharse(stoi(rs)) << '\n';
		}
	}

	closesocket(connSock);
	WSACleanup();

	return 0;
}

void service(char *request) {
	printf("\n\nChoose service:\n");
	printf("1.Login.\n");
	printf("2.Send message.\n");
	printf("3.Logout.\n\n");

	int choice;
	char temp[BUFF_SIZE];

	while (1)
	{
		/*
		scanf_s("%d", &choice);

		//Clear input buffer
		int c;
		while ((c = getchar()) != '\n');
		*/

		switch (choice)
		{
			//Login service
		case 1: {
			printf("Enter username:");
			gets_s(temp, BUFF_SIZE);
			sprintf_s(request, BUFF_SIZE, "USER %s", temp);
			return;
		}
				//Send message service
		case 2: {
			printf("Enter message:");
			gets_s(temp, BUFF_SIZE);
			sprintf_s(request, BUFF_SIZE, "POST %s", temp);
			return;
		}
				//Logout service
		case 3: {
			strcpy_s(request, BUFF_SIZE, "QUIT");
			return;
		}
				//Invalid input
		default: {printf("Invalid input, choose again.\n"); continue; }
		}
	}
}