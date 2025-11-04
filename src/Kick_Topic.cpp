#include "../inc/Server.hpp"

//kick 1channelname 1nickname
void Server::kick(Client &c) {
	if (!c.isRegistered()) {
		sendError(c, ERR_NOTREGISTERED, MSG_NOTREGISTERED);
		return ;
	}
	std::vector<std::string> p = c.getParams();
	if (p.empty() || p.size() < 2) {
		sendError(c, ERR_NEEDMOREPARAMS, MSG_NEEDMOREPARAMS("KICK"));
		return ;
	}
	if (p.size() > 2 || !c.getTrailing().empty()) {
		sendCustomError(c, CUSTOM_TOO_MANY_ARGS("KICK"));
		return ;
	}
	if (!validChannelName(p[0])) {
		sendError(c, ERR_NOSUCHCHANNEL, MSG_NOSUCHCHANNEL(p[0]));
		return ;
	}
	Channel *ch = getChannelByName(p[0]);
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
	ch->removeMember(target->getSocketFd());
	if (ch->isOperator(target->getSocketFd())) {
		ch->removeOperator(target->getSocketFd());
	}
	target->leaveChannel(p[0]);
	sendInfoToTarget(c, *target, CUSTOM_YOU_GOT_KICKED(c.getNickname(), p[0]));
	sendInfoToChannel(c, *ch, CUSTOM_SOMEONE_WAS_KICKED(c.getNickname(), target->getNickname(), p[0]));
	if (ch->getMembers().size() == 0) {
		_channels.erase(ch->getName());
	}
}

void Server::topic(Client &c) {
	if (!c.isRegistered()) {
		sendError(c, ERR_NOTREGISTERED, MSG_NOTREGISTERED);
		return ;
	}
	std::vector<std::string> p = c.getParams();
	if (p.empty()) {
		sendError(c, ERR_NEEDMOREPARAMS, MSG_NEEDMOREPARAMS("TOPIC"));
		return ;
	}
	if (p.size() > 1) {
		sendCustomError(c, CUSTOM_TOO_MANY_ARGS("TOPIC"));
		return ;
	}
	if (!validChannelName(p[0])) {
		sendError(c, ERR_NOSUCHCHANNEL, MSG_NOSUCHCHANNEL(p[0]));
		return ;
	}
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
			sendServerReply(c, RPL_NOTOPIC, MSG_NOTOPIC(p[0]));
		else 
			sendServerReply(c, RPL_TOPIC, MSG_TOPIC(p[0], ch->getTopic()));
		return ;
	}
	if (ch->getTopicLocked() && !ch->isOperator(c.getSocketFd())) {
		sendError(c, ERR_CHANOPRIVSNEEDED, MSG_CHANOPRIVSNEEDED(p[0]));
		return ;
	}
	ch->setTopic(t);

	//hexchat
	sendServerReply(c, RPL_TOPIC, MSG_TOPIC(ch->getName(), ch->getTopic()));
	// sendServerReply(c, RPL_TOPICWHOTIME, MSG_TOPICWHOTIME(
    //     ch->getName(), c.getNickname(), now()));
		//sendInfoToChannel(c, *ch, CUSTOM_TOPIC_CHANGED(c.getNickname(), ch->getName(), t));
		sendInfoToChannel__HexChat_frienly(c, *ch, "TOPIC", ch->getName(), ch->getTopic(), true);

}