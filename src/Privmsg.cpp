#include "../inc/Server.hpp"

void Server::privmsgToChannel(Client& c, std::string& name) {
	Channel *ch = getChannelByName(name);
	if (ch == NULL) {
		sendError(c, ERR_NOSUCHCHANNEL, MSG_NOSUCHCHANNEL(name));
		return ;
	}
	if (ch != NULL && !ch->isMember(c.getSocketFd())) {
		sendError(c, ERR_NOTONCHANNEL, MSG_NOTONCHANNEL(name));
		return ;
	}
	for (std::set<int>::iterator it = ch->getMembers().begin(); it != ch->getMembers().end(); it++) {
		if (*it == c.getSocketFd())
			continue ;
		Client *t = &_clients[*it];
		t->appendOutgoingBuffer(c.prefix(name));
		t->appendOutgoingBuffer(c.getTrailing());
		//t->appendOutgoingBuffer("\r\n");
		enablePollout(*t);
		sendPendingData(*t);
		std::cout << "send data to client in channel\n";
	}
}

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
			privmsgToChannel(c, *it);
			continue ;
		}
		Client *t = getClientByNick(*it);
		if (t == NULL) {
			sendError(c, ERR_NOSUCHNICK, MSG_NOSUCHNICK(*it));
			continue ;
		}
		(*t).appendOutgoingBuffer(c.prefix(*it));
		(*t).appendOutgoingBuffer(c.getTrailing());
		t->appendOutgoingBuffer("\r\n");
		enablePollout(*t);
		sendPendingData(*t);
	}
}
