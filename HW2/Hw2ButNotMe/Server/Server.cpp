#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <ctime>
#include <process.h>
#include "StatusCode.h"
#include "Session.h"
#include "tcp_stream.h"

#pragma comment (lib, "Ws2_32.lib")

#define SERVER_ADDR "127.0.0.1"
#define MAX_CLIENT 128


using namespace std;

//List of account in db
map<const string, const int> acc;

//List of session for multitheard
SessionList sessionList;

unsigned _stdcall serviceTheard(void *param);

/**
* @brief handle requests form client
* @param[in] session The session is being process
* @param[in] request
* @return response code
*/
int handleRequests(Session& session, string request);


/**
* @brief split string s1 into 2 string s1, s2 by delimiter " "
* @param[in] str
* @param[out] s1
* @param[out] s2
*/
void split(string str, string& s1, string& s2);

/**
* @brief write log into logfile
* @param[in] session
* @param[in] request
* @param[in] response
*/
void logFile(Session &session, string request, int response);

int main(int argc, char* argv[]) {

	// Validate the parameters
	if (argc != 2)
	{
		printf_s("Usage: %s <PortNumber>\n", argv[0]);
		return 1;
	}

	//Initiate Winsock
	getAccount("account.txt");

	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaData)) {
		printf("Winsock 2.2 is not supported\n");
		return 0;
	}
	//Construct socket
	SOCKET listenSock;
	listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (listenSock == INVALID_SOCKET) {
		printf("Error %d: Cannot create server socket.", WSAGetLastError());
		return 0;
	}

	//Bind address to socket
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	int serverPort = atoi(argv[1]);
	serverAddr.sin_port = htons(serverPort);
	inet_pton(AF_INET, SERVER_ADDR, &serverAddr.sin_addr);

	if (bind(listenSock, (sockaddr *)&serverAddr, sizeof(serverAddr))) {
		printf("(Error: %d)Cannot associate a local address with server socket.", WSAGetLastError());
		return 0;
	}

	//Listen request from client
	if (listen(listenSock, MAX_CLIENT)) {
		printf("(Error: %d)Cannot place server socket in state LISTEN.", WSAGetLastError());
		return 0;
	}
	printf("Server started!\n");

	//Communicate with client
	SOCKET connSock;
	sockaddr_in clientAddr;
	char clientIP[INET_ADDRSTRLEN];
	int clientAddrLen = sizeof(clientAddr), clientPort;
	while (1) {
		//Accept request
		connSock = accept(listenSock, (sockaddr *)&clientAddr, &clientAddrLen);
		if (connSock == SOCKET_ERROR) {
			printf("(Error: %d)Cannot permit incoming connection.", WSAGetLastError());
			return 0;
		}

		else {
			inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, sizeof(clientIP));
			clientPort = ntohs(clientAddr.sin_port);
			printf("Accept incoming connection from %s:%d\n", clientIP, clientPort);
			Session session(connSock, clientIP, clientPort);
			_beginthreadex(0, 0, serviceTheard, (void *)&session, 0, 0);
		}

	}
	closesocket(listenSock);

	//Terminate Winsock
	WSACleanup();

	return 0;
}

unsigned _stdcall serviceTheard(void *param) {

	Session session(*(Session *)param);
	SOCKET connSock = session.getSocket();

	vector<string> requestList;
	char buff[BUFF_SIZE];
	int ret;
	while (1) {
		//Receive request
		ret = receiveStream(connSock, requestList);
		if (ret == SOCKET_ERROR) {
			printf("Error %d: Cannot receive data.\n", WSAGetLastError());
			if (requestList.empty())
				break;
		}
		else if (ret == 0) {
			printf("Client disconnects.\n");
			break;
		}
		else {
			for (string request : requestList) {
				//Handle
				int result = handleRequests(session, request);

				sprintf_s(buff, BUFF_SIZE, "%d", result);

				//log
				logFile(session, request, result);

				//Send result
				ret = sendStream(connSock, buff);
				if (ret == SOCKET_ERROR)
					break;
			}

			if (ret == SOCKET_ERROR) {
				printf("Error %d: Cannot send data.\n", WSAGetLastError());
				break;
			}
		}
	}
	//Remove session form list
	sessionList.remove(&session);
	//Close socket
	closesocket(connSock);
	return 0;
}

int handleRequests(Session& session, string request) {
	string header;
	string data;

	split(request, header, data);

	if (header == "USER")
		return handleUSER(session, data);
	else if (header == "POST")
		return handlePOST(session, data);
	else if (header == "QUIT")
		return handleQUIT(session);
	else
		return Status::Code::BadRequest;
}



int handleUSER(Session &session, string user) {
	//Client already login
	if (session.isLogin())
		return Status::Code::AlreadyLogin;

	auto search = acc.find(user);

	//Cant find account
	if (search == acc.end())
		return Status::Code::AccountNotFound;

	//Account is locked
	else if (search->second == 0)
		return Status::Code::LockedAccount;

	auto pos = sessionList.findByUserName(user);

	//Account login in a another session
	if (pos != sessionList.end())
		return Status::Code::LoginInAnotherSession;

	//Login successful
	session.login(user);
	//Add session into sessionList
	sessionList.pushBack(&session);
	return Status::Code::Login;
}

int handlePOST(Session &session, string mess) {
	//Send message successful
	if (session.isLogin()) {
		return Status::Code::OK;
	}

	//Not login
	else return Status::Code::NotLogin;
}

int handleQUIT(Session &session) {
	//Logout successful
	if (session.isLogin()) {
		session.logout();

		//remove session form session list
		sessionList.remove(&session);
		return Status::Code::Logout;
	}
	//Not login
	else return Status::Code::Q_NotLogin;
}

void getAccount(string file) {
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

void split(string str, string& s1, string& s2) {
	size_t pos = str.find(" ");
	if (pos != string::npos) {
		s1 = str.substr(0, pos);
		s2 = str.substr(pos + 1);
	}
	else s1 = str;
}

void logFile(Session &session, string request, int response) {
	//Open log file
	string file = "log_20194248.txt";
	ofstream outf{ file, ios::app };
	if (!outf) {
		cout << "Cant open " << file << '\n';
		exit(1);
	}

	//Take current time
	char buffer[20];
	time_t raw_time = time(nullptr);
	tm timeinfo;
	localtime_s(&timeinfo, &raw_time);
	strftime(buffer, 20, "[%x %X]", &timeinfo);

	//Log
	outf << session << ' ' << buffer << " $ " << request << " $ " << response << '\n';
}
