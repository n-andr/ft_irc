#include "../inc/Server.hpp"

void Server::sendError(Client &c, const std::string &code,
                       const std::string &message) {
    std::ostringstream oss;
    oss << ":" << CYAN << SERVER_NAME << RESET
        << " " << RED << code << RESET
        << " " << (c.getNickname().empty() ? "*" : c.getNickname())
        << " " << c.getCommand()
        << " :" << RED << message << RESET
        << "\r\n";

    c.appendOutgoingBuffer(oss.str());// enqueue for POLLOUT
	enablePollout(c);
}

void Server::pass(Client &c) {
	if (c.isConnected()) {
		sendError(c, ERR_ALREADYREGISTRED, ALREADYREGISTRED_MSG);
		return;
	}
	if (c.getParams().size() < 1) {
		sendError(c, ERR_NEEDMOREPARAMS, NEEDMOREPARAMS_MSG);
		std::cout << RED << "Too few Parameters for Pass. Research how to handle this" << RESET << std::endl;
		return ;
	}
	//if too many params == error is dependent on the server
	if (c.getParams()[0] == _password) {
		std::cout << GREEN << "Correct password" << RESET << std::endl;
		c.setHasPassedPassword(true);
		if (c.getNickname().size() != 0 && c.getUsername().size())
			c.setConnected(true);
	}
	else
		sendError(c, ERR_PASSWDMISMATCH, PASSWDMISMATCH_MSG);
}

void Server::delegateCommand(Client &c) {
	const std::string &cmd = c.getCommand();

    if (cmd == "PASS")
        pass(c);
    else if (cmd == "USER")
        std::cout << "USER would be executed here" << std::endl;
    else if (cmd == "NICK")
        std::cout << "NICK would be executed here" << std::endl;
    else if (cmd == "JOIN")
        std::cout << "JOIN would be executed here" << std::endl;
    else if (cmd == "INVITE")
        std::cout << "INVITE would be executed here" << std::endl;
    else if (cmd == "KICK")
        std::cout << "KICK would be executed here" << std::endl;
    else if (cmd == "PART")
        std::cout << "PART would be executed here" << std::endl;
    else if (cmd == "TOPIC")
        std::cout << "TOPIC would be executed here" << std::endl;
    else if (cmd == "MODE")
        std::cout << "MODE would be executed here" << std::endl;
    else
        std::cout << "Unknown command: " << cmd << std::endl;
}

/*the main poll() loop; reacts to POLLIN on listen FD and delegates client I/O*/

void Server::eventLoop()
{
	std::cout << "Event Loop starts:" << std::endl;//control print
	while (_running)//server is running
	{
		poll(&_pollFds[0], _nFds, 100);
		if (_pollFds[L_SOCKET].revents & POLLIN)
			this->handleNewConnection();
		for (int i = 1; i < _nFds; i++)
		{
			Client &c = _clients[_pollFds[i].fd];
			//simple msg broadcaster
			if (_pollFds[i].revents & POLLIN)
			{
				char buf[512];
				int bytes_read = recv(_pollFds[i].fd, buf, sizeof(buf), 0);
				if (bytes_read <= 0) {
					disconnectClient(_pollFds[i].fd);
					continue;
				}
				else {
					c.appendReadBuffer(buf);
					if (c.extractCommand() == true)//successfully found a command and extracted it into raw command
					{
						c.parseRawCommand();
						c.printCommand();
						delegateCommand(c);
					}
					//std::cout << "[recv] from fd=" << _pollFds[i].fd << ": " << msg;
					broadcastMessage(c.getCommand(), _pollFds[i].fd);//this now sets pollout and appends out buf.
				}
			}
			if (_pollFds[i].revents & POLLOUT) {
				sendPendingData(_clients[_pollFds[i].fd]);
			}
		}
	}
	std::cout << "\nAfter Poll Loop\n"; 
}