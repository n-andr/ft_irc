#include "../inc/Server.hpp"

bool Server::validChannelName(std::string &s) {
	if (s.size() < 2 || s.size() > 50)
		return (false);
	if (s.empty())
		return (false);
	if (s[0] != '#')
		return (false);
	for (size_t i = 1; i < s.size(); i++) {
		if (s[i] == '\0' || s[i] == '\r' || s[i] == '\n' || s[i] == ' ' || s[i] == ',' || s[i] == '\a' || s[i] == ':')
			return false;
	}
	return (true);
}

void Server::joinMSGs(Client& c, Channel& ch) {
	sendInfoToChannel__HexChat_frienly(c, ch, "JOIN", "", ch.getName(), true);
	if (ch.getTopic().empty()) {
		sendServerReply(c, RPL_NOTOPIC, MSG_NOTOPIC(ch.getName()));
	} else {
		sendServerReply(c, RPL_TOPIC, MSG_TOPIC(ch.getName(), ch.getTopic()));
	}
	std::string nicks = "";
	for (std::set<int>::const_iterator it = ch.getMembers().begin(); it != ch.getMembers().end(); ++it) {
		if (it != ch.getMembers().begin())
			nicks += " ";
		nicks += (ch.isOperator(*it) ? "@" : "") + _clients[*it].getNickname();
	}
	std::string line = ":" + std::string(SERVER_NAME) + " 353 " + c.getNickname() + " = " + ch.getName() + " :" + nicks + "\r\n";
	c.appendOutgoingBuffer(line);
	enablePollout(c);
	sendPendingData(c);
	sendServerReply(c, RPL_ENDOFNAMES, MSG_ENDOFNAMES(ch.getName()));
}

void Server::join(Client& c) {
	if (!c.isRegistered()) {
		sendError(c, ERR_NOTREGISTERED, MSG_NOTREGISTERED);
		return ;
	}
	std::vector<std::string> p = c.getParams();
	if (p.empty()) {
		sendError(c, ERR_NEEDMOREPARAMS, MSG_NEEDMOREPARAMS("JOIN"));
		return ;
	}
	if (!c.getTrailing().empty()) {
		sendCustomError(c, CUSTOM_TOO_MANY_ARGS("JOIN"));
		return ;
	}
	for (std::vector<std::string>::iterator it = p.begin(); it != p.end(); it++) {
		if (!validChannelName(*it)) {
			sendError(c, ERR_NOSUCHCHANNEL, MSG_NOSUCHCHANNEL(*it));
			continue ;
		}
		Channel *ch = getChannelByName(*it);
		if (ch == NULL) {
			if (c.getChannels().size() >= MAX_CHANNELS_PER_CLIENT) {
				sendError(c, ERR_TOOMANYCHANNELS, MSG_TOOMANYCHANNELS(*it));
				return ;
			}
			ch = createNewChannel(*it);
			ch->addOperator(c.getSocketFd());
			ch->addMember(c.getSocketFd());
			c.joinChannel(*it);
			joinMSGs(c, *ch);
			continue ;
		}
		if (ch->getInviteOnly()) {
			if (!c.isInvited(*it)) {
				sendError(c, ERR_INVITEONLYCHAN, MSG_INVITEONLYCHAN(*it));
				continue ;
			}
		}
		if (ch->isMember(c.getSocketFd())) {
			sendError(c, ERR_USERONCHANNEL, MSG_USERONCHANNEL(c.getNickname(), ch->getName()));
			return ;
		}
		if (c.getChannels().size() >= MAX_CHANNELS_PER_CLIENT) {
			sendError(c, ERR_TOOMANYCHANNELS, MSG_TOOMANYCHANNELS(*it));
			return ;
		}
		if (ch->getUserLimit() > 0 && ch->getMembers().size() >= ch->getUserLimit()) {
			sendError(c, ERR_CHANNELISFULL, MSG_CHANNELISFULL(ch->getName()));
			return ;
		}
		if (ch->getMembers().size() >= MAX_CLIENTS_PER_CHANNEL) {
			sendError(c, ERR_CHANNELISFULL, MSG_CHANNELISFULL(ch->getName()));
			return ;
		}
		ch->addMember(c.getSocketFd());
		c.joinChannel(*it);
		joinMSGs(c, *ch);
	}
}

//invite nick channel
void Server::invite(Client& c) {
	if (!c.isRegistered()) {
		sendError(c, ERR_NOTREGISTERED, MSG_NOTREGISTERED);
		return ;
	}
	std::vector<std::string> p = c.getParams();
	if (p.size() < 2) {
		sendError(c, ERR_NEEDMOREPARAMS, MSG_NEEDMOREPARAMS("INVITE"));
		return ;
	}
	if (!c.getTrailing().empty() || p.size() > 2) {
		sendCustomError(c, CUSTOM_TOO_MANY_ARGS("INVITE"));
		return ;
	}
	Client *target = getClientByNick(p[0]);
	if (target == NULL) {
		sendError(c, ERR_NOSUCHNICK, MSG_NOSUCHNICK(p[0]));
		return ;
	}
	if (!validChannelName(p[1])) {
		sendError(c, ERR_NOSUCHCHANNEL, MSG_NOSUCHCHANNEL(p[1]));
		return ;
	}
	Channel *ch = getChannelByName(p[1]);
	if (ch == NULL) {
		sendError(c, ERR_NOSUCHCHANNEL, MSG_NOSUCHCHANNEL(p[1]));
		return ;
	}
	if (ch->isMember(target->getSocketFd())) {
		sendError(c, ERR_USERONCHANNEL, MSG_USERONCHANNEL(p[0], p[1]));
		return ;
	}
	if (ch != NULL && !ch->isMember(c.getSocketFd())) {
		sendError(c, ERR_NOTONCHANNEL, MSG_NOTONCHANNEL(p[1]));
		return ;
	}
	if (ch->getInviteOnly() && !ch->isOperator(c.getSocketFd())) {
		sendError(c, ERR_CHANOPRIVSNEEDED, MSG_CHANOPRIVSNEEDED(p[1]));
		return ;
	}
	target->addInvite(p[1]);
	sendInfoToTarget__HexChat_frienly(c, *target, "INVITE", target->getNickname(), p[1]);
	sendServerReply(c, RPL_INVITING, MSG_INVITING(p[1], p[0]));
}
