#pragma 
#include <winsock2.h>
#include <string>
#include <vector>
#include <iostream>
#include <conio.h>

#define BUFF_SIZE 2048
#define ENDING_DELIMITER "\r\n"

/**
* @brief send message in stream
* @param[in] sock: the connected socket
* @param[in] buffer: the message
* @return: if no error occurs, return the number of bytes sent. Otherwise, send a value of SOCKET_ERROR
*/
int sendStream(SOCKET &sock, char * buffer) {
	int ret, bufferLen;

	//Apend ending delimiter
	strcat_s(buffer, BUFF_SIZE, ENDING_DELIMITER);
	bufferLen = strlen(buffer);

	//Send all buffer
	for (int i = 0; i < bufferLen; i += ret) {
		ret = send(sock, buffer + i, bufferLen - i, 0);
		if (ret == SOCKET_ERROR)
			break;
	}

	return ret;
}

/**
* @brief receive message in stream
* @param[in] sock: the connectied socket
* @param[out] requesList: list of requests
* @return: if no error occurs, return the number of bytes received. Otherwise, send a value of SOCKET_ERROR
*/
int receiveStream(SOCKET &sock, std::vector<std::string> &requestList) {
	char temp[20];
	std::string buffer;
	int ret;
	size_t pos;

	requestList.clear();
	
	//Receive until buffer end with ending delimiter or recv fail
	ret = recv(sock, temp, 20, MSG_WAITALL);
		temp[ret] = 0;
		buffer += temp;
		std::cout << buffer << std::endl;

	
	/*
	//Split buffer into list of requests
	while ((pos = buffer.find(ENDING_DELIMITER)) != std::string::npos) {
		requestList.push_back(buffer.substr(0, pos));
		buffer.erase(0, pos + strlen(ENDING_DELIMITER));
	}
	*/
	return ret;
}
