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
#define SERVER_PORT 5500
#define SERVER_ADDR "127.0.0.1"
#define BUFF_SIZE 2048
#define ENDING_DELIMITER "\r\n"
#pragma comment (lib, "Ws2_32.lib")

using namespace std;

map<const string, const int> acc;


int requestHandler(SOCKET connectedSocket, string request);
/* echoThread - Thread to receive the message from client and echo*/
unsigned __stdcall echoThread(void *param);

int getAccount(string file);

int handleRequests(Session& session, string request);

/**
* @brief handle login request
* @param[in] session
* @param[in] user
* @return login's response code
*/
int handleUSER(Session &session, string user);

/**
* @brief handle send messages request
* @param[in] session
* @param[in] mess
* @return post's response code
*/
int handlePOST(Session &session, string mess);

/**
* @brief handle logout request
* @param[in] session
* @return logout's response code
*/
int handleQUIT(Session &session);

/**
* @brief get account list from file
* @param[in] file db
*/
int getAccount(string file);

/**
* @brief split string s1 into 2 string s1, s2 by delimiter " "
* @param[in] str
* @param[out] s1
* @param[out] s2
*/
int split(string str, string& s1, string& s2);

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
				//ret = requestHandler(connectedSocket, request);
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


int handleRequests(Session& session, string request) {
	string header;
	string data;

	split(request, header, data);

	if (header == "USER") {
		return handleUSER(session, data);
	}
	else if (header == "POST") {
		return handlePOST(session, data);
	}
	else if (header == "BYE") {
		return handleQUIT(session);
	}
	else {
		return BAD_REQUEST;
	}
}

int handleUSER(Session &session, string user) {
	//Client already login
	if (session.isLogin()) {
		return ALREADY_LOGIN;
	}

	auto account = acc.find(user);

	//Cant find account
	if (account == acc.end()) {
		return ACCOUNT_NOT_FOUND;
	}

	//Account is locked
	else if (account->second == 1) {
		return LOCKED_ACCOUNT;
	}

	//Login successful
	session.login(user);
	return LOGIN_SUCCESS;
}

int handlePOST(Session &session, string mess) {
	//Send message successful
	if (session.isLogin()) {
		return OK;
	}

	//Not login
	else return NOT_LOGIN;
}

int handleQUIT(Session &session) {
	//Logout successful
	if (session.isLogin()) {
		session.logout();
		return LOGOUT_SUCCESS;
	}
	//Not login
	else return NOT_LOGIN;
}


int getAccount(string file) {
	//Open file
	ifstream inf{ file };
	if (!inf) {
		cout << "Can not open " << file << '\n';
		exit(1);
	}
	string input, s1, s2;
	while (inf) {
		getline(inf, input);
		split(input, s1, s2);
		acc.insert(make_pair(s1, stoi(s2)));
	}
}

int split(string str, string& header, string& message) {
	size_t pos = str.find(" ");
	if (pos != string::npos) {
		header = str.substr(0, pos);
		message = str.substr(pos + 1);
	}
	else header = str;
	return 0;
}