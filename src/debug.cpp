#include "../inc/Server.hpp"


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
                  << "   Connected: " << (c.isConnected() ? "yes" : "no") << "\n"
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