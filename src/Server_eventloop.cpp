#include "../inc/Server.hpp"

void Server::kick(Client &c) {
	if (!c.isRegistered()) {
		sendError(c, ERR_NOTREGISTERED, MSG_NOTREGISTERED);
		return ;
	}
	std::vector<std::string> p = c.getParams();
	if (p.empty() || p.size() < 2) {
		sendError(c, ERR_NEEDMOREPARAMS, MSG_NEEDMOREPARAMS("JOIN"));
		return ;
	}
	//check nbr relations channels vs nicks
	Channel *ch = getChannelByName(p[0]);//assuming no list is given. Otherwise put everything starting here into a loop
	if (ch == NULL) {
		sendError(c, ERR_NOSUCHCHANNEL, MSG_NOSUCHCHANNEL(p[0]));
		return ;
	}
	if (ch != NULL && !ch->isMember(c.getSocketFd())) {
		sendError(c, ERR_NOTONCHANNEL, MSG_NOTONCHANNEL(p[0]));
		return ;
	}
	if (!ch->isOperator(c.getSocketFd())) {
		sendError(c, ERR_CHANOPRIVSNEEDED, MSG_CHANOPRIVSNEEDED(p[0]));
		return ;
	}
	Client *target = getClientByNick(p[1]);
	if (target == NULL) {
		sendError(c, ERR_NOSUCHNICK, MSG_NOSUCHNICK(p[1]));
		return ;
	}
	if (!ch->isMember(target->getSocketFd())) {
		sendError(c, ERR_USERNOTINCHANNEL, MSG_USERNOTINCHANNEL(p[1], p[0]));
		return ;
	}
	//do the actual kicking
	ch->removeMember(target->getSocketFd());
	if (ch->isOperator(target->getSocketFd())) {
		ch->removeOperator(target->getSocketFd());
	}
	target->leaveChannel(p[0]);
}

void Server::topic(Client &c) {
	if (!c.isRegistered()) {
		sendError(c, ERR_NOTREGISTERED, MSG_NOTREGISTERED);
		return ;
	}
	std::vector<std::string> p = c.getParams();
	if (p.empty()) {
		sendError(c, ERR_NEEDMOREPARAMS, MSG_NEEDMOREPARAMS("JOIN"));
		return ;
	}
	//optional error if too many args?
	Channel *ch = getChannelByName(p[0]);
	if (ch == NULL) {
		sendError(c, ERR_NOSUCHCHANNEL, MSG_NOSUCHCHANNEL(p[0]));
		return ;
	}
	if (ch != NULL && !ch->isMember(c.getSocketFd())) {
		sendError(c, ERR_NOTONCHANNEL, MSG_NOTONCHANNEL(p[0]));
		return ;
	}
	std::string t = c.getTrailing();
	if (t.empty()) {
		if (ch->getTopic().empty())
		sendError(c, RPL_NOTOPIC, MSG_NOTOPIC(p[0]));
		sendError(c, RPL_TOPIC, MSG_TOPIC(p[0], ch->getTopic()));
	}
	if (ch->getTopicLocked() && !ch->isOperator(c.getSocketFd())) {
		sendError(c, ERR_CHANOPRIVSNEEDED, MSG_CHANOPRIVSNEEDED(p[0]));
		return ;
	}
	ch->setTopic(t);
	//send command response for setting the topic?
	//Potentially to everyone in a channel?
	//:<nick>!<user>@<host> TOPIC <channel> :<topic>
	//for every channel member
		//outgoing append prefix(channel name)
		//outgoing append topic
		//enable pollout
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
		invite(c);
	else if (cmd == "KICK")
		std::cout << "KICK would be executed here" << std::endl;
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