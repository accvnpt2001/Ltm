#pragma once
#include <fstream>
#include "string"
#include <iostream>

using namespace std;


int split(string str, string& header, string& message) {
	size_t pos = str.find(" ");
	if (pos != string::npos) {
		header = str.substr(0, pos);
		message = str.substr(pos + 1);
	}
	else header = str;
	return 0;
}