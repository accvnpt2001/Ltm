#pragma once
#include <string>

/**
* @brief class for response code and function relate
*/
namespace Status {
	enum Code
	{
		Login = 10,						//Login Successful
		LockedAccount = 11,				//Account is locked
		LoginInAnotherSession = 12,		//Account is already lock in another session
		AccountNotFound = 13,			//Cant find accout
		AlreadyLogin = 14,				//Alreadu login

		OK = 20,						//Send message successful
		NotLogin = 21,					//Not login

		Logout = 30,					//Logout successful
		Q_NotLogin = 31,				//Not login

		BadRequest = 01					//Request undefined
	};

	inline std::string reasonPharse(int code) {
		switch (code)
		{

		case 10: return "Login successful";
		case 11: return "Account is locked";
		case 12: return "Already login in anthor session";
		case 13: return "Couldnt find account";
		case 14: return "Already login";

		case 20: return "Send message successful";
		case 21: return "Must login to send message";

		case 30: return "Logout successful";
		case 31: return "Must login before logout";

		case 01: return "Bade request";

		default: return std::string();
		}
	}

	inline std::string reasonPharse(Code code) {
		return reasonPharse(static_cast<int>(code));
	}
}





/**
* @brief get account from txt file to a map acc
* @param[in] file
* @return if fail exit program but success return 0
*/
int getAccount(string file);

/**
* @brief handle request
* @param[in/out] session
* @param[in] request
* @return code of process
*/
int handleRequests(Session& session, string request);

/**
* @brief handle user request
* @param[in/out] session
* @param[in] user
* @return code of process
*/
int handleUSER(Session &session, string user);

/**
* @brief handle post request
* @param[in/out] session
* @param[in] mess
* @return code of process
*/
int handlePOST(Session &session, string mess);

/**
* @brief handle logout request
* @param[in/out] session
* @return code of process
*/
int handleBYE(Session &session);

/**
* @brief split string s1 into 2 string s1, s2 by delimiter " "
* @param[in] str
* @param[out] s1
* @param[out] s2
*/
int split(string str, string& s1, string& s2);

















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

int handleBYE(Session &session) {
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
	return 0;
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