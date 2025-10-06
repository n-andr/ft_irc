#include "../inc/Server.hpp"

void Server::delegateCommand(Client &c) {
	const std::string &cmd = c.getCommand();

	if (cmd == "PASS")
		pass(c);
	else if (cmd == "USER")
		user(c);
	else if (cmd == "NICK")
		nick(c);
	else if (cmd == "JOIN")
		join(c);
	else if (cmd == "INVITE")
		invite(c);
	else if (cmd == "KICK")
		std::cout << "KICK would be executed here" << std::endl;
	else if (cmd == "PART")
		std::cout << "PART would be executed here" << std::endl;
	else if (cmd == "TOPIC")
		std::cout << "TOPIC would be executed here" << std::endl;
	else if (cmd == "MODE")
		std::cout << "MODE would be executed here" << std::endl;
	else if (cmd == "PRIVMSG")
		privmsg(c);
	else
		sendError(c, ERR_UNKNOWNCOMMAND, MSG_UNKNOWNCOMMAND(cmd));
}

/*the main poll() loop; reacts to POLLIN on listen FD and delegates client I/O*/

void Server::eventLoop()
{
	//std::cout << "Event Loop starts:" << std::endl;//control print
	while (_running)//server is running
	{
	//	std::cout << "loop\n";
		poll(&_pollFds[0], _nFds, 100);
		if (_pollFds[L_SOCKET].revents & POLLIN)
			this->handleNewConnection();
		for (int i = 1; i < _nFds; i++)
		{
			Client &c = _clients[_pollFds[i].fd];
			if (_pollFds[i].revents & POLLIN)
			{
				char buf[512];
				int bytes_read = recv(_pollFds[i].fd, buf, sizeof(buf), 0);
				if (bytes_read <= 0) {
					disconnectClient(_pollFds[i].fd);
					continue;
				}
				else {
					buf[bytes_read] = '\0';
					//std::cout << "Buf = \"" << buf << "\"" << std::endl;
					c.appendReadBuffer(buf);
					if (c.extractCommand() == true)//successfully found a command and extracted it into raw command
					{
						c.parseRawCommand();
						c.printCommand();
						delegateCommand(c);
					}
					//std::cout << "[recv] from fd=" << _pollFds[i].fd << ": " << msg;
					//broadcastMessage(c.getCommand(), _pollFds[i].fd);//this now sets pollout and appends out buf.
				}
			}
			if (_pollFds[i].revents & POLLOUT) {
				sendPendingData(_clients[_pollFds[i].fd]);
			}
		}
	}
	std::cout << "\nAfter Poll Loop\n"; 
}