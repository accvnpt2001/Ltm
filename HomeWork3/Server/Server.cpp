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

void processData(char *, char *);
int Receive(SOCKET, char *, int, int);
int Send(SOCKET, char *, int, int);

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


	//////////////////////////
	SOCKET client[FD_SETSIZE], connSock; 
	Session sessions[FD_SETSIZE];

	fd_set readfds, initfds; //use initfds to initiate readfds at the begining of every loop step
	sockaddr_in clientAddr;
	int ret, nEvents, clientAddrLen;
	char rcvBuff[BUFF_SIZE], sendBuff[BUFF_SIZE];
	
	// initializa value for client array
	for (int i = 0; i < FD_SETSIZE; i++)
		client[i] = 0;	// 0 indicates available entry
	

	FD_ZERO(&initfds);
	FD_SET(listenSock, &initfds);

	//Step 5: Communicate with clients
	while (1) {
		readfds = initfds;		/* structure assignment */
		nEvents = select(0, &readfds, 0, 0, 0);
		if (nEvents < 0) {
			printf("\nError! Cannot poll sockets: %d", WSAGetLastError());
			break;
		}

		//new client connection
		if (FD_ISSET(listenSock, &readfds)) {
			clientAddrLen = sizeof(clientAddr);
			if ((connSock = accept(listenSock, (sockaddr *)&clientAddr, &clientAddrLen)) < 0) {
				printf("\nError! Cannot accept new connection: %d", WSAGetLastError());
				break;
			}
			else {
				printf("You got a connection from [%s:%d]\n", inet_ntoa(clientAddr.sin_addr), clientAddr.sin_port); /* prints client's IP */

				int i;
				for (i = 0; i < FD_SETSIZE; i++)
					if (client[i] == 0) {
						client[i] = connSock;					
						sessions[i].setSocket(connSock);
						sessions[i].setPort(clientAddr.sin_port);
						sessions[i].setClientIp(inet_ntoa(clientAddr.sin_addr));
						FD_SET(client[i], &initfds);
						break;
					}

				if (i == FD_SETSIZE) {
					printf("\nToo many clients.");
					closesocket(connSock);
				}

				if (--nEvents == 0)
					continue; //no more event
			}
		}

		//receive data from clients
		for (int i = 0; i < FD_SETSIZE; i++) {
			if (client[i] == 0)
				continue;

			if (FD_ISSET(client[i], &readfds)) {
				vector<string> requestList;
				string response;
				ret = recvStream(client[i], requestList);
				
				if (ret <= 0) {
					printf("Error %d: Cannot receive data.\n", WSAGetLastError());
					FD_CLR(client[i], &initfds);
					closesocket(client[i]);
					client[i] = 0;
					sessions[i] = Session();
				}

				else if (ret > 0) {
					for (string request : requestList) {

						// handle request
						int result = handleRequests(sessions[i], request);

						// send response
						ret = sendStream(client[i], response + to_string(result));
						cout << "Receive from client [" << sessions[i].getClientIp() << ":" << sessions[i].getPort() << "] : " << request << " -->> " << result << endl;

						if (ret == SOCKET_ERROR) {
							printf("Error %d: Cannot handler request data.\n", WSAGetLastError());
							FD_CLR(client[i], &initfds);
							closesocket(client[i]);
							client[i] = 0;
							sessions[i] = Session();
						}
					}
					if (ret == SOCKET_ERROR) {
						printf("Error %d: Cannot send data.\n", WSAGetLastError());
						break;
					}
				}

			}

			if (--nEvents <= 0)
				continue; //no more event
		}
	}

	closesocket(listenSock);
	WSACleanup();
	return 0;
}


/* The processData function copies the input string to output
* @param in Pointer to input string
* @param out Pointer to output string
* @return No return value
*/
void processData(char *in, char *out) {
	memcpy(out, in, BUFF_SIZE);
}

/* The recv() wrapper function */
int Receive(SOCKET s, char *buff, int size, int flags) {
	int n;

	n = recv(s, buff, size, flags);
	if (n == SOCKET_ERROR)
		printf("Error: cant receive message code : %\n", WSAGetLastError());

	return n;
}

/* The send() wrapper function*/
int Send(SOCKET s, char *buff, int size, int flags) {
	int n;

	n = send(s, buff, size, flags);
	if (n == SOCKET_ERROR)
		printf("Error2: cant send message code :%\n", WSAGetLastError());

	return n;
}