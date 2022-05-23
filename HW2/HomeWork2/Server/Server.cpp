// MultithreadTCPEchoServer.cpp : Defines the entry point for the console application.
//

#include "stdio.h"
#include "conio.h"
#include "string"
#include <map>
#include <fstream>
#include "ws2tcpip.h"
#include "winsock2.h"
#include "process.h"
#include "Session.h"
#include "tcp_stream.h"
#include "status_code.h"
#include "topo.h"
#define SERVER_PORT 5500
#define SERVER_ADDR "127.0.0.1"
#pragma comment (lib, "Ws2_32.lib")

using namespace std;

/* echoThread - Thread to receive the message from client and echo*/
unsigned __stdcall echoThread(void *param);

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

	//get database
	getAccount("account.txt");

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
			printf("Accept incoming connection from %s:%d\n", clientIP, clientPort);
			Session session(connSock, clientIP, clientPort);
			_beginthreadex(0, 0, echoThread, (void *)&session, 0, 0); //start thread
		}
	}
	closesocket(listenSock);

	WSACleanup();

	return 0;
}

unsigned __stdcall echoThread(void *param) {
	char buff[BUFF_SIZE];
	int ret;

	Session session(*(Session *)param);
	SOCKET connectedSocket = session.getSocket();
	vector<string> requestList;
	string response;

	while (1) {
		
		ret = recvStream(connectedSocket, requestList);

		if (ret == SOCKET_ERROR) {
			printf("Error %d: Cannot receive data.\n", WSAGetLastError());
			if (requestList.empty())
				break;
		
		}
		else if (ret == 0) {
			printf("Client disconnects.\n");
			break;
		}

		else if (requestList.size() > 0) {
			for (string request : requestList) {

				// handle request
				int result = handleRequests(session, request);

				// send response
				ret = sendStream(connectedSocket, response + to_string(result));
				cout << "Receive from client [" << session.getClientIp() << ":" << session.getPort() << "] : " << request << " -->> " << result << endl;

				if (ret == SOCKET_ERROR) {
					printf("Error %d: Cannot handler request data.\n", WSAGetLastError());
				}
			}
			if (ret == SOCKET_ERROR) {
				printf("Error %d: Cannot send data.\n", WSAGetLastError());
				break;
			}
		}
		else continue;
	}

	closesocket(connectedSocket);
	return 0;
}

