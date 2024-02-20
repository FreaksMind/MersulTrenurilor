#include <Command.h>


	Command::Command(const char* command) {
		args = 0;
		int ok = 0;
		std::string cmmd(command);
		std::size_t found = cmmd.find(' ');
		if(found != std::string::npos){
		std::string delimiter = " ";

		size_t pos = 0;
		std::string token;
		while ((pos = cmmd.find(delimiter)) != std::string::npos) {
			token = cmmd.substr(0, pos);
			if (ok == 0) {
				m_commandname = token;
				ok = 1;
			}
			else {
				m_commandargs.push_back(token);
				args++;
			}
			cmmd.erase(0, pos + delimiter.length());
		}
		m_commandargs.push_back(cmmd);
		args++;
		} else {
			m_commandname = command;
		}

	}
	Command::~Command() {

		m_commandname.clear();
		m_commandargs.clear();
}

	std::string Command::getCommandName() {
		return m_commandname;
	}

	std::vector<std::string> Command::getCommandArgs() {
			return m_commandargs;
	}

	int Command::argsCount(){
		return args;
	}

	int Command::getCommandCode(){
		if(strcmp(getCommandName().c_str(), "login") == 0) return LOGIN_COMMAND;
		if(strcmp(getCommandName().c_str(), "register") == 0) return REGISTER_COMMAND;
		if(strcmp(getCommandName().c_str(), "logout") == 0) return 	LOGOUT_COMMAND;
		if(strcmp(getCommandName().c_str(), "request") == 0) return REQUEST_COMMAND;

	}
