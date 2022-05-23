// TCP_Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <tchar.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#define BUFF_SIZE 2048
#define SERVER_PORT 5500
#define SERVER_ADDR "127.0.0.1"
#pragma comment (lib, "Ws2_32.lib")

/**
* @function displayResponse: dissplay ressponse received from server
* @param[in] res: A poiter to the response message
*
* @return: void
*/
void displayRes(char * buff);

int main(int argc, char * argv[])
{
	/*
	// check agrument
	if (argc < 3) {
		printf("Usage %s <ServerIPAddress> <ServerPortNumber> \n", argv[0]);
		return 1;
	}
	*/

	//Step 1: Intiate WinSock
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaData)) {
		printf("Winsock 2.2 is not suported\n");
		_gettchar();
		return 0;
	}

	//Step 2: Contruct socket
	SOCKET client;
	client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client == INVALID_SOCKET) {
		printf("Error %d: Cannot create server socket.", WSAGetLastError());
		_gettchar();
		return 0;
	}

	//Step 3: Specfy server address

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_ADDR, &serverAddr.sin_addr);

	//Step 4: Request to connect server
	if (connect(client, (sockaddr *)&serverAddr, sizeof(serverAddr))) {
		printf("Error %d: Cannot connect to server.", WSAGetLastError());
		_gettchar();
		return 0;
	}

	printf("Connected server \n");

	//Step 5: comunicate with server
	char buffr[BUFF_SIZE], buffs[BUFF_SIZE];
	int ret, messageLen;

	while (1) {
		//Send message
		printf("Send to server :  ");
		gets_s(buffs, BUFF_SIZE);
		messageLen = strlen(buffs);
		if (messageLen == 0) break;

		ret = send(client, buffs, messageLen, 0);
		//ret = sendStream(client, buffs);
		if (ret == SOCKET_ERROR) {
			printf("Error %d: Cannot send data", WSAGetLastError());
		}

		//Receive  mesage
		//ret = recvStream(client, buffr);
		ret = recv(client, buffr, BUFF_SIZE, 0);
		if (ret == SOCKET_ERROR) {
			printf("Error %d: Cannot recive", WSAGetLastError());
		}
		else if (strlen(buffr) > 0) {
			buffr[ret] = 0;
			displayRes(buffr);
		}
	}
	//Step 6: close socket
	closesocket(client);

	//Step 7: Teminal WinSock
	WSACleanup();

	return 0;
}

void displayRes(char * buff) {
	/*
	if (buff[0] == '+') {
	printf("%s\n", buff + 1);
	}
	else if (buff[0] == '-') {
	printf("%s\n", buff + 1);
	}
	*/
	printf("%s\n", buff);
}