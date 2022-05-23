#pragma once

#include <string>

#define LOGIN_SUCCESS 10
#define LOCKED_ACCOUNT 11
#define ACCOUNT_NOT_FOUND 12
#define ALREADY_LOGIN 14

#define OK  20
#define NOT_LOGIN 21
#define LOGOUT_SUCCESS 30
#define BAD_REQUEST 99
#define REASON_PHARSE_FAIL "Code not found"

using namespace std;

/**
* @brief parse code to string and display to client
* @param[in] code: the connected socket
* @return: return string of mesage
*/

string reasonPharse(int code) {
	switch (code)
	{

	case LOGIN_SUCCESS: return "Login successful";
	case LOCKED_ACCOUNT: return "Account is locked";
	case ACCOUNT_NOT_FOUND: return "Couldnt find account";
	case ALREADY_LOGIN: return "You already login, try to log out first";

	case OK: return "Send message successful";
	case NOT_LOGIN: return "You must login first to do this";

	case LOGOUT_SUCCESS: return "Logout successful";

	case BAD_REQUEST: return "Bad request";

	default: return REASON_PHARSE_FAIL;
	}
}
