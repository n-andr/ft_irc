
#include <iostream> //cout, cerr etc
#include <cctype> // for isdigit
#include <cstdlib>  // std::atol
#include <cstring>  // std::strlen
//#include <exception> //for exception 
#include "../inc/Server.hpp"

static bool isValidPassword(std::string s) {
	if (s.empty())
        return false;
    for (size_t i = 0; i < s.size(); i++) {
		unsigned char ch = static_cast<unsigned char>(s[i]);
		if (!std::isprint(ch)) return false; // must be printable ASCII
		if (std::isspace(ch)) return false; // no spaces, tabs, etc.
		if (ch == ':') return false; //not to confuse with trailing
        // if (s[i] == '\0' || s[i] == '\r' || s[i] == '\n' || s[i] == ' ')
        //     return false;
    }
    return true;
}

static bool isValidPort(char *port){
	if (std::strlen(port) > 5) return false;
	for (const char *tmp = port; *tmp; ++tmp){
		if (!std::isdigit(static_cast<unsigned char>(*tmp)))
			return false;
	}
	long portNumber = std::atol(port);
	if (portNumber < 1024 || portNumber > 65535) return false;
	return true;
}

static bool checkArgs(int argc, char *argv[]){
	if (argc != 3){
		std::cerr << RED << "Usage: " << argv[0] << " <port> <password>\n" << RESET;
		return false;
	}
	if (!isValidPort(argv[1])){
		std::cerr << RED << "Error: <port> must be in range 1024 - 65535\n" << RESET;
		return false;
	}
	if (std::string(argv[2]).empty()) {
		std::cerr << RED << "Error: <password> must be non-empty\n" << RESET;
		return false;
	}
	if (std::string(argv[2]).size() > MAX_PASS_SIZE) {
		std::cerr << RED << "Error: <password> too long. Max size: 10." << RESET << std::endl;
		return false;
	}
	if (isValidPassword(argv[2]) == false) {
		std::cerr << RED << "Error: <password> invalid." << RESET << std::endl;
		return (false);
	}
	return true;
}

int main(int argc, char* argv[]){
	//register the signalHnadler(s)
	std::signal(SIGINT, Server::signalHandler);
	std::signal(SIGTERM, Server::signalHandler);
	if (!checkArgs(argc, argv)) return 1;
	try {
		Server server(std::atoi(argv[1]), argv[2]);
		server.start();
	} catch (const std::exception& e){
		std::cerr << "[Error] " << e.what() << std::endl;
		return 1;
	} catch (...) {
		std::cerr << "[Error] Unknown exception" << std::endl;
		return 1;
	}
	return 0;
}