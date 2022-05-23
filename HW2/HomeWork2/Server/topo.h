#pragma once
#include <fstream>
#include "string"
#include <map>
#include <iostream>
#include "Session.h"
#include "status_code.h"

using namespace std;

// map of user name and status (lock or active)
map<const string, const int> acc;

/**
* @brief handle user request
* @param[in/out] session
* @param[in] user
* @return code of process
*/
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

/**
* @brief handle post request
* @param[in/out] session
* @param[in] mess
* @return code of process
*/
int handlePOST(Session &session, string mess) {
	//Send message successful
	if (session.isLogin()) {
		return OK;
	}

	//Not login
	else return NOT_LOGIN;
}


/**
* @brief handle logout request
* @param[in/out] session
* @return code of process
*/
int handleBYE(Session &session) {
	//Logout successful
	if (session.isLogin()) {
		session.logout();
		return LOGOUT_SUCCESS;
	}
	//Not login
	else return NOT_LOGIN;
}

/**
* @brief split string s1 into 2 string s1, s2 by delimiter " "
* @param[in] str
* @param[out] s1
* @param[out] s2
*/
int split(string str, string& header, string& message) {
	size_t pos = str.find(" ");
	if (pos != string::npos) {
		header = str.substr(0, pos);
		message = str.substr(pos + 1);
	}
	else header = str;
	return 0;
}

/**
* @brief get account from txt file to a map acc
* @param[in] file
* @return if fail exit program but success return 0
*/
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
	return 0;
}

/**
* @brief handle request
* @param[in/out] session
* @param[in] request
* @return code of process
*/
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
		return handleBYE(session);
	}
	else {
		return BAD_REQUEST;
	}
}