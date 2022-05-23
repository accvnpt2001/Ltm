// TCP_Client.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <iostream>
#include "stdafx.h"
#include <tchar.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "tcp_stream.h"
#include "status_code.h"
#include "vector"
#define SERVER_PORT 5500
#define SERVER_ADDR "127.0.0.1"
#pragma comment (lib, "Ws2_32.lib")

using namespace std;

/**
* @function displayRes: dissplay ressponse received from server
* @param[in] response: std string to the response message
*
* @return: 0 if parse code success
*/
int displayRes(string response);

/**
* @function service: get requst from user and send
* @param[in] client: socket used to send
* @param[out] request: reference to std string of the request message
*
* @return: number byte sent
*/
int service(SOCKET client, string &request);

/**
* @function getRequest: get requst from user
* @param[out] buffs: buffer to get request
*
* @return: 0 if success
*/
int getRequest(char * buffs);

int main(int argc, char * argv[])
{

	// check agrument
	/*
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
	//const int SERVER_PORT = atoi(argv[2]);
	//const char* SERVER_ADDR = argv[1];
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
	vector<string> requestList;
	string request;
	int ret, messageLen;

	while (1) {
		//get request and send message
		ret = service(client, request);

		if (ret == SOCKET_ERROR) {
			printf("Error %d: Cannot send data", WSAGetLastError());
		}

		//Recive message
		ret = recvStream(client, requestList);
		if (ret == SOCKET_ERROR) {
			printf("Error %d: Cannot recive", WSAGetLastError());
		}
		else if (requestList.size() > 0) {
			for (string response : requestList) {
				displayRes(response);
			}
		}
	}
	//Step 6: close socket
	closesocket(client);

	//Step 7: Teminal WinSock
	WSACleanup();

	return 0;
}

int service(SOCKET client, string &request) {
	int ret;
	char buffs[BUFF_SIZE];
	request.clear();
	
	getRequest(buffs);
	buffs[strlen(buffs)] = 0;
	request += buffs;

	ret = sendStream(client, request);

	return ret;
}

int getRequest(char * buffs) {
	int choice;
	char temp[BUFF_SIZE];

	printf("\n\nChoose service:\n");
	printf("   1.Login.\n");
	printf("   2.Send message.\n");
	printf("   3.Logout.\n\n");

	while (1) {
		printf(">>  ");
		scanf_s("%d", &choice);
		
		//Clear input buffer
		int c;
		while ((c = getchar()) != '\n');

		switch (choice) {
			//Login service
			case 1: {
				printf("Enter username:   ");
				if (gets_s(temp, BUFF_SIZE)) {
					sprintf_s(buffs, BUFF_SIZE, "USER %s", temp);
				}
				else {
					printf("Chuoi toi da 2000 ky tu");
					continue;
				}
				return 0;
			}
			//Send message service
			case 2: {
				printf("Enter message:   ");
				gets_s(temp, BUFF_SIZE);
				sprintf_s(buffs, BUFF_SIZE, "POST %s", temp);
				return 0;
			}
			//Logout service
			case 3: {
				strcpy_s(buffs, BUFF_SIZE, "BYE");
				return 0;
			}
			//Invalid input
			default: {
				printf("Invalid input, choose again.\n"); 
				continue; 
			}
		}
	}
}

int displayRes(string response) {
	printf("%s", reasonPharse(stoi(response)).c_str());
	return 0;
}