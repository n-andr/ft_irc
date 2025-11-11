#include "../inc/Server.hpp"

void Server::delegateCommand(Client &c) {
	const std::string &cmd = c.getCommand();
	std::cout << "RAW: " << c.getRaw() << std::endl;
	
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
		kick(c);
	else if (cmd == "TOPIC")
		topic(c);
	else if (cmd == "MODE")
		mode(c);
	else if (cmd == "PRIVMSG")
		privmsg(c);
	else if (cmd == "WHO") {
		c.clearCommand();
		return ;
	}
	else if (cmd == "PING") {
		std::string out = ":" + std::string(SERVER_NAME) + " PONG :" + c.getTrailing() + "\r\n";
		c.appendOutgoingBuffer(out);
    	enablePollout(c);
		sendPendingData(c);
	}
	else if (cmd == "CAP") {
		if (c.getRaw() == "CAP END") {
			c.clearCommand();
			//if (c.isRegistered())
			//	sendWelcomes(c);
			return ;
		}
		std::ostringstream oss;
		oss << ":" << SERVER_NAME << " CAP ";
		oss << (c.getNickname().empty() ? "*" : c.getNickname());
		oss << " LS :" << " \r\n";

		c.appendOutgoingBuffer(oss.str());
		enablePollout(c);
		sendPendingData(c);
	}
	else
		sendError(c, ERR_UNKNOWNCOMMAND, MSG_UNKNOWNCOMMAND(cmd));
}

/*the main poll() loop; reacts to POLLIN on listen FD and delegates client I/O*/
void Server::eventLoop()
{
	while (_running)
	{
		//std::cout << "\npoll\n";
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
					if (c.getdisconnect())
						disconnectClient(_pollFds[i].fd);
					else
						eraseClientFootprint(_pollFds[i].fd);
					continue;
				}
				else {
					buf[bytes_read] = '\0';
					c.appendReadBuffer(buf);
					while (c.extractCommand() == true)
					{
						c.parseRawCommand();
						delegateCommand(c);
						c.clearCommand();
					}
					if (c.getBufOverflow() == true) {
						size_t pos = c.getReadBuffer().find("\r\n");
						sendCustomError(c, CUSTOM_BUFFER_OVERFLOW);
						c.consumeBytesReadBuffer(pos + 2);
						c.setBufferOverflow(false);
					}
				}
			}
			if (!c.getdisconnect() && (_pollFds[i].revents & POLLOUT)) {
				sendPendingData(_clients[_pollFds[i].fd]);
			}
		}
	}
}