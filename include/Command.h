#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <cstring>
#include <string>
#include <iostream>
#include <vector>

#define LOGIN_COMMAND 0
#define REGISTER_COMMAND 1
#define LOGOUT_COMMAND 2
#define REQUEST_COMMAND 3

#define MAX_ARGS 4

/*
request schedule all
request schedule [train]
request trainlist
update [train] [delay]









*/

class Command
{
private:
	std::string m_commandname;
	std::vector<std::string> m_commandargs;
	int args;
public:


	Command(const char*);
	~Command();
	std::string getCommandName();
	std::vector<std::string> getCommandArgs();
	int argsCount();
	int getCommandCode();


};

