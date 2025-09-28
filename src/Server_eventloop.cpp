#include "../inc/Server.hpp"

void Server::privmsg(Client& c) {
	if (!c.isRegistered()) {
		sendError(c, ERR_NOTREGISTERED, MSG_NOTREGISTERED);
		return ;
	}
	std::vector<std::string> p = c.getParams();
	if (p.empty()) {
		sendError(c, ERR_NORECIPIENT, "PRIVMSG");
		return;
	}
	if (c.getTrailing().empty()) {
		sendError(c, ERR_NOTEXTTOSEND, "");
		return;
	}
	for (std::vector<std::string>::iterator it = p.begin(); it != p.end(); it++) {
		if ((*it)[0] == '#') {
			std::cout << "Send to channel " << *it << std::endl;
			continue ;
		}
		Client *t = getClientByNick(*it);
		if (t == NULL) {
			sendError(c, ERR_NOSUCHNICK, MSG_NOSUCHNICK(*it));
			continue ;
			//or return, depends on weither or not the target list is still executed after an error occurs
		}
		(*t).appendOutgoingBuffer(c.prefix(*it));
		(*t).appendOutgoingBuffer(c.getTrailing());
		enablePollout(*t);
	}
}

Channel* Server::getChannelByName(std::string& name) {
	for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); it ++){
		if (it->first == name)
			return (&it->second);
	}
	return NULL;
}

Channel* Server::createNewChannel(std::string& name) {
	std::map<std::string, Channel>::iterator it =
        _channels.insert(std::make_pair(name, Channel(name))).first;
	return (&it->second);
}

void Server::join(Client& c) {
	std::vector<std::string> p = c.getParams();
	if (p.empty()) {
		sendError(c, ERR_NEEDMOREPARAMS, MSG_NEEDMOREPARAMS("JOIN"));
		return ;
	}
	for (std::vector<std::string>::iterator it = p.begin(); it != p.end(); it++) {
		//check validity of name -> sendError(c, ERR_NOSUCHCHANNEL, <channel>).
		Channel *ch = getChannelByName(*it);
		if (ch == NULL) {
			ch = createNewChannel(*it);
			ch->addOperator(c.getSocketFd()); // creator becomes operator
			ch->addMember(c.getSocketFd());
			c.joinChannel(*it);
			continue ;
		}
		if (ch->getInviteOnly()) {
			if (!c.isInvited(*it)) {
				sendError(c, ERR_INVITEONLYCHAN, MSG_INVITEONLYCHAN(*it));
				continue ;
			}
		}
		(*ch).addMember(c.getSocketFd());
		c.joinChannel(*it);
	}
}

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
		std::cout << "INVITE would be executed here" << std::endl;
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
		std::cout << "Unknown command: " << cmd << std::endl;
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