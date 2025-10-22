#include "../inc/Server.hpp"

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
	for (std::vector<std::string>::iterator it = p.begin(); it != p.end(); it++) {
		//check validity of name -> sendError(c, ERR_NOSUCHCHANNEL, <channel>).
		Channel *ch = getChannelByName(*it);
		if (ch == NULL) {
			if (c.getChannels().size() >= MAX_CHANNELS_PER_CLIENT) {
				sendError(c, ERR_TOOMANYCHANNELS, MSG_TOOMANYCHANNELS(*it));
				return ;
			}
			ch = createNewChannel(*it);
			ch->addOperator(c.getSocketFd()); // creator becomes operator
			ch->addMember(c.getSocketFd());
			c.joinChannel(*it);
			sendInfoToChannel(c, *ch, CUSTOM_JOIN(c.getNickname(), ch->getName()));
			//sendServerReply(c, -1, CUSTOM_JOIN(c.getNickname(), ch->getName()));
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
		if (ch->getUserLImit() > 0 && ch->getMembers().size() >= ch->getUserLImit()) {
			sendError(c, ERR_CHANNELISFULL, MSG_CHANNELISFULL(ch->getName()));
			return ;
		}
		if (ch->getMembers().size() >= MAX_CLIENTS_PER_CHANNEL) {
			sendServerReply(c, -1, CUSTOM_CHANNELISFULL(ch->getName()));
			return ;
		}
		ch->addMember(c.getSocketFd());
		c.joinChannel(*it);
		//sendServerReply(c, -1, CUSTOM_JOIN(c.getNickname(), ch->getName()));
		sendInfoToChannel(c, *ch, CUSTOM_JOIN(c.getNickname(), ch->getName()));
	}
}

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
	//optional: error if too many args
	Client *target = getClientByNick(p[0]);
	if (target == NULL) {
		sendError(c, ERR_NOSUCHNICK, MSG_NOSUCHNICK(p[0]));
		return ;
	}
	//optional: check if target == c (inviting yourself)
	Channel *ch = getChannelByName(p[1]);
	if (ch != NULL) {
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
	}
	/*else {//throw error at invite to noneexisting channel
		sendError(c, ERR_NOSUCHCHANNEL, MSG_NOSUCHCHANNEL(p[1]));
		return ;
	}*/
	target->addInvite(p[1]);
	//confirmation message to target. check syntax!
	sendInfoToTarget(c, *target, CUSTOM_INVITED(p[1]));
	//confirmation message to c (server or client prefix?)
	sendServerReply(c, RPL_INVITING, MSG_INVITING(p[1], p[0]));
}