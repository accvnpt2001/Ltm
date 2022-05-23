#pragma once
#include <string>
#include <algorithm>
#include <iostream>
#include <list>
#include <synchapi.h>
#include <WinSock2.h>

/**
* @brief Implementation of a session
*/
class Session {
	const SOCKET connSock;
	const char * clientIp;
	int clientPort;
	const char * userName;
	int status;

public:
	Session(const SOCKET& i_sock, const char* i_clientIp, int i_port)
		:connSock{ i_sock }, clientIp{ _strdup(i_clientIp) },
		clientPort{ i_port }, userName{ _strdup("") }, status{ 0 }
	{}

	/**
	* @brief is called when a client login, change session's username and status
	* @param i_username account's username
	*/
	void login(const std::string &i_username) {
		userName = _strdup(i_username.c_str());
		status = 1;
	}

	/**
	* @brief is called when a client logout, change session's username and status to default
	*/
	void logout() {
		userName = _strdup("");
		status = 0;
	}

	SOCKET getSocket() const { return connSock; }
	const char* getClientIp() const { return clientIp; }
	int getPort() const { return clientPort; }
	const char* getUserName() const { return userName; }

	/**
	* @brief return status of session
	* @return 1 if client's login, else 0
	*/
	int isLogin() const { return status; }

	/**
	* @brief print session's ip and port
	*/
	friend std::ostream& operator<<(std::ostream &out, const Session s) {
		out << s.clientIp << ":" << s.clientPort;
		return out;
	}
};

/**
* @brief wrapper class for critical section
*/
class CS_Acquire {
	CRITICAL_SECTION &cs;
public:
	CS_Acquire(CRITICAL_SECTION &_cs) : cs(_cs) { EnterCriticalSection(&cs); }
	~CS_Acquire() { LeaveCriticalSection(&cs); }
};

/**
* @brief List of session for multithearding
*/
class SessionList {
	std::list<Session *> sL;
	CRITICAL_SECTION cs;
	int i = 0;
public:
	SessionList() { InitializeCriticalSection(&cs); }

	/**
	* @brief push a session into the list's end
	* @param[in] session: pointer to a section
	*/
	void pushBack(Session *session) {
		CS_Acquire acquire(cs);
		sL.push_back(session);
	}

	/**
	* @brief remove a session form the list
	* @param[in] session: pointer to a section
	*/
	void remove(Session *session) {
		CS_Acquire acquire(cs);
		sL.remove(session);
	}

	/**
	* @brief Find the position of a session in the list by username
	* @param[in] username
	* @return the position of the found session or list'end if cant find
	*/
	auto findByUserName(const std::string& username) {
		CS_Acquire acquire(cs);
		auto pos = std::find_if(sL.begin(), sL.end(), [username](const Session* s) {
			return username == s->getUserName();
		});
		return pos;
	}

	/**
	* @return list's end
	*/
	auto end() {
		CS_Acquire acquire(cs);
		return sL.end();
	}
};