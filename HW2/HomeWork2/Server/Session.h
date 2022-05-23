#pragma once
#include <iostream>
#include <WinSock2.h>

using namespace std;
/**
* @brief Implementation of a session
*/
class Session {
	SOCKET connSock;
	const char * clientIp;
	int clientPort;
	const char * userName;
	int status;

public:

	Session(const SOCKET& in_sock, const char * in_clientIP, int in_port) {
		connSock = in_sock;
		clientIp = _strdup(in_clientIP);
		clientPort = in_port;
		userName = _strdup("");
		status = 0;

	}

	/**
	* @brief login, change session's username and status
	* @param in_username account's username
	*/
	void login(const string &in_username) {
		userName = _strdup(in_username.c_str());
		status = 1;
	}

	/**
	* @brief logout, change session's username and status to default
	*/
	void logout() {
		userName = _strdup("");
		status = 0;
	}

	/**
	* @brief return status of session
	* @return 1 if client's login, else 0
	*/
	int isLogin() { 
		return status; 
	}

	SOCKET getSocket() { 
		return connSock; 
	}
	
	void setSocket(const SOCKET &i_socket) { 
		this->connSock = i_socket; 
	}
	
	const char* getClientIp() { 
		return clientIp; 
	}
	
	int getPort() {
		return clientPort; 
	}
	
	const char* getUserName() { 
		return userName; 
	}

};


