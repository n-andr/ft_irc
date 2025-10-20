#include "../inc/Server.hpp"
#include "../inc/Client.hpp"



void Server::printClients()
{
	if (_clients.empty()) {
		std::cout << YELLOW << "[info] No clients connected.\n" << RESET;
		return;
	}

	std::cout << YELLOW << "\n===== Connected Clients =====\n" << RESET;
	for (std::map<int, Client>::const_iterator it = _clients.begin(); it != _clients.end(); ++it) {
		Client c = it->second;
		std::cout << YELLOW << "FD: " << c.getSocketFd() << "\n"
				  << "   IP: " << c.getIpAddress() << ":" << c.getPort() << "\n"
				//  << "   Connected: " << (c.isConnected() ? "yes" : "no") << "\n"
				  << "   Nickname: " << c.getNickname() << "\n"
				  << "   Username: " << c.getUsername() << "\n"
				  << "   Registered: " << (c.isRegistered() ? "yes" : "no") << "\n"
				  << "   Operator: " << (c.isOperator() ? "yes" : "no") << "\n"
				  << "   Passed password: " << (c.hasPassedPassword() ? "yes" : "no") << "\n"
				  << "   Read buffer: \"" << c.getReadBuffer() << "\"\n"
				  << "   Outgoing buffer: \"" << c.getOutgoingBuffer() << "\"\n"
				  << "   Command: \"" << c.getCommand() << "\"\n"
				  << "-----------------------------\n" << RESET;
	}
	std::cout << YELLOW << "==============================\n\n" << RESET;
}


void Client::printCommand() {
	std::cout << "Raw input: [" << _raw_command_input << "]\n";
	std::cout << "Command:   [" << _command_capitalized << "]\n";

	std::cout << "Params:	[";
	for (size_t i = 0; i < _params.size(); ++i) {
		std::cout << _params[i];
		if (i + 1 < _params.size())
			std::cout << ", ";
	}
	std::cout << "]\n";

	if (_trailing.empty())
		std::cout << "Trailing:  [NULL]\n";
	else
		std::cout << "Trailing:  [" << _trailing << "]\n";
}


void Server::printModeParseResult(const std::string channelName, const ModeParseResult &r)
{
    std::cout << "---- MODE DEBUG ----\n";
	std::cout << "Channel name: " << channelName << std::endl;
    if (!r.error.empty())
        std::cout << "Error: " << r.error << "\n";

    for (size_t i = 0; i < r.order.size(); ++i) {
        const ModeChange &m = r.order[i];
        std::cout << (m.set ? '+' : '-') << m.mode;
        if (m.hasArg)
            std::cout << " (" << m.arg << ")";
        std::cout << "\n";
    }

    if (!r.leftoverArgs.empty()) {
        std::cout << "Leftovers:";
        for (size_t i = 0; i < r.leftoverArgs.size(); ++i)
            std::cout << " " << r.leftoverArgs[i];
        std::cout << "\n";
    }

    std::cout << "--------------------\n";
}