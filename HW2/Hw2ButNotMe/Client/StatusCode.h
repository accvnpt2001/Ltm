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
