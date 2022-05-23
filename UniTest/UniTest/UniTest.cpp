// UniTest.cpp : Defines the entry point for the console application.
//
#include "string"
#include "stdafx.h"
#include "vector"
#include "iostream"
#include "conio.h"

using namespace std;



int main()
{
	vector<string> requestList;
	string a = "aconcac\r\nbuniii ; lla\r\nc lal\r\ndgag dasdasdad \r\n";
	size_t last = 0;
	size_t next = 0;
	while ((next = a.find("\r\n", last)) != string::npos) {
		requestList.push_back(a.substr(last, next - last));
		last = next + strlen("\r\n");
	}
	
	cout << "size : " << requestList.size() << endl;

	for (int i = 0; i < requestList.size(); i++) {
		cout << "value" << i  << " :  "<< requestList[i] << endl;
		cout << "size " << i  << " :  "<< requestList[i].size() << endl;

	}
	_getch();

}

