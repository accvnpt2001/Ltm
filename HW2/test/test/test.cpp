#pragma 
#include <winsock2.h>
#include <string>
#include <vector>

#define BUFF_SIZE 2048

#define ENDING_DELIMITER "\r\n"

using namespace std;
/**
* @brief send message in stream
* @param[in] sock: the connected socket
* @param[in] buffer: the message
* @return: if no error occurs, return the number of bytes sent. Otherwise, send a value of SOCKET_ERROR
*/

int sendStream1(SOCKET sock, char * buffs) {
	int ret = 0;

	//Add ending delimiter
	strcat_s(buffs, BUFF_SIZE, ENDING_DELIMITER);
	int messageLen = strlen(buffs);

	//Send all buffer
	for (int i = 0; i < messageLen; i += ret) {
		ret = send(sock, buffs + i, messageLen - i, 0);
		if (ret == SOCKET_ERROR)
			break;				// or return ??
	}

	return ret;
}

/**
* @brief send message in stream
* @param[in] sock: the connected socket
* @param[in] buffer: the message
* @return: if no error occurs, return the number of bytes sent. Otherwise, send a value of SOCKET_ERROR
*/

int sendStream(SOCKET sock, string request) {
	int ret = 0;
	char buffs[20];
	strcpy_s(buffs, 20, "");
	request += ENDING_DELIMITER;

	int messageLen = request.length();
	/*
	//Send all buffer
	for (int i = 0; i < messageLen; i += ret) {
	// coppy message to bufer
	if (messageLen - i >= BUFF_SIZE) {
	strncpy(buffs, request.c_str() + i, BUFF_SIZE);
	}
	else {
	strncpy(buffs, request.c_str() + i, messageLen - i);
	}

	ret = send(sock, buffs, BUFF_SIZE, 0);

	if (ret == SOCKET_ERROR)
	break;				// or return ??
	}*/

	//strncpy(buffs, request.substr(0, BUFF_SIZE-2).c_str(), BUFF_SIZE);

	//cout << request.substr(0, BUFF_SIZE - 2);
	strcat_s(buffs, 20, "la lal la");
	ret = send(sock, buffs, 20, 0);

	return ret;
}

/**
* @brief receive message in stream
* @param[in] sock: the connectied socket
* @param[out] requesList: list of requests
* @return: if no error occurs, return > 0. Otherwise, send a value of SOCKET_ERROR
*/

int recvStream(SOCKET sock, vector<string> &requestList) {

	char temp[BUFF_SIZE + 1];
	string buffer;
	int ret;
	size_t pos;

	requestList.clear();

	//Receive until ending delimiter 
	while ((ret = recv(sock, temp, BUFF_SIZE, 0)) > 0) {
		buffer += temp;

		pos = buffer.rfind(ENDING_DELIMITER);
		if (pos + strlen(ENDING_DELIMITER) == buffer.length())
			break;
	}

	//Split buffer into list of requests

	size_t last = 0;
	size_t next = 0;
	while ((next = buffer.find(ENDING_DELIMITER, last)) != string::npos) {
		requestList.push_back(buffer.substr(last, next - last));
		last = next + strlen(ENDING_DELIMITER);
	}

	return ret;
}


// last

int sendStream1(SOCKET sock, string message) {
	int ret = 0;
	char buffs[BUFF_SIZE];
	//Add ending delimiter
	message += ENDING_DELIMITER;
	int messageLen = message.length();

	//Send all buffer
	for (int i = 0; i < messageLen; i += ret) {
		strcpy_s(buffs, BUFF_SIZE, "");
		strcat_s(buffs, BUFF_SIZE, message.substr(i, BUFF_SIZE - 1).c_str());

		ret = send(sock, buffs, BUFF_SIZE, 0);
		if (ret == SOCKET_ERROR)
			break;				// or return ??
	}

	return ret;
}



int sendStream1(SOCKET sock, string message) {
	int ret = 0;
	char buffs[BUFF_SIZE];
	//Add ending delimiter
	message += ENDING_DELIMITER;
	int messageLen = message.length();

	//Send all buffer
	for (int i = 0; i < messageLen; i += ret) {

		int byteWillSend = 0;
		if (messageLen - i > BUFF_SIZE - 1) byteWillSend = BUFF_SIZE - 1;
		else byteWillSend = messageLen - i;

		strcpy_s(buffs, BUFF_SIZE, "");
		strcat_s(buffs, BUFF_SIZE, message.substr(i, byteWillSend).c_str());
		ret = send(sock, buffs, BUFF_SIZE, 0);
		cout << "ret : " << ret << " end ret   ";
		if (ret == SOCKET_ERROR)
			break;				// or return ??
	}

	return ret;

}
