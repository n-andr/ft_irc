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
		kick(c);
	else if (cmd == "TOPIC")
		topic(c);
	else if (cmd == "MODE")
		mode(c);
	else if (cmd == "PRIVMSG")
		privmsg(c);
	else
		sendError(c, ERR_UNKNOWNCOMMAND, MSG_UNKNOWNCOMMAND(cmd));
}

/*the main poll() loop; reacts to POLLIN on listen FD and delegates client I/O*/
void Server::eventLoop()
{
	while (_running)
	{
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
					if (c.extractCommand() == true)
					{
						c.parseRawCommand();
						delegateCommand(c);
						c.clearCommand();
					}
					else if (c.getBufOverflow() == true) {
						size_t pos = c.getReadBuffer().find("\r\n");
						sendCustomError(c, CUSTOM_BUFFER_OVERFLOW);
						c.consumeBytesReadBuffer(pos + 2);
						c.setBufferOverflow(false);
					}
				}
			}
			if (_pollFds[i].revents & POLLOUT) {
				sendPendingData(_clients[_pollFds[i].fd]);
			}
		}
	}
}