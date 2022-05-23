#pragma once
#include <winsock2.h>
#include <string.h>
#define BUFF_SIZE 2048

/**
* @function recvStream: receive stream tcp
* @param[in]  sock: socket to receive message
* @param[out] buffr: poiter to the receive buffer
* @return:	-1 error memcpy_s
*			int ret : length of the recived message or error
*/
int recvStream(SOCKET &sock, char* buffr) {
	char lengthBuff[4];
	char tempBuff[BUFF_SIZE];
	int ret;
	unsigned long length = 0;
	errno_t err;

	//Receive the length of the message
	ret = recv(sock, lengthBuff, 4, MSG_WAITALL);
	if (ret < 1) {
		return ret;
	}
	// copy each byte from lengthBuff to length
	err = memcpy_s(&length, sizeof(length), lengthBuff, 4);
	if (err) {
		return -1;
	}
	length = ntohl(length);

	// Receive all the message
	strcpy_s(buffr, BUFF_SIZE, "");
	ret = recv(sock, tempBuff, length, MSG_WAITALL);
	if (ret < 1) {
		return ret;
	}
	tempBuff[ret] = 0;
	strcat_s(buffr, BUFF_SIZE, tempBuff);

	return ret;
}


/**
* @function sendStream: receive stream tcp
* @param[in] sock: socket to send message
* @param[in] buffs: poiter to the send buffer
* @return:	-1 error memcpy_s
*			int ret : length of the sent message or error
*/
int sendStream(SOCKET &sock, char* buffs) {
	char bytes[4];
	int ret, lengthBuff;
	errno_t err;
	lengthBuff = strlen(buffs);
	unsigned long length = htonl(lengthBuff);
	err = memcpy_s(bytes, 4, &length, 4);
	if (err) {
		return -1;
	}

	//Send the length of message
	ret = send(sock, bytes, 4, 0);
	if (ret == SOCKET_ERROR) {
		return ret;
	}

	//Send all the message
	while (lengthBuff != 0) {
		ret = send(sock, buffs, lengthBuff, 0);
		if (ret == SOCKET_ERROR) {
			return ret;
		}
		lengthBuff -= ret;
	}
	return lengthBuff;
}
