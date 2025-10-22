#include "../inc/Server.hpp"


bool handle_i(Server& serv, Channel& ch, Client& c, const ModeChange& mode){
	if (mode.hasArg){
		serv.sendError(c, -1, "MODE i: this mode takes no parameter");
		return false;
	}
	ch.setInviteOnly(mode.set); //if was invite-only - stayes invite-only - no error
	// should i send a conformation/info msg to the channel?
	return true;
}

bool handle_t(Server& serv, Channel& ch, Client& c, const ModeChange& mode){
	
	if (mode.hasArg) {
	serv.sendError(c, -1, "MODE t: this mode takes no parameter");
	return false;
	}
	ch.setTopicLocked(mode.set); //if was locked - stayes locked - no error
	// should i send a conformation/info msg to the channel?
	return true;
}

bool handle_k(Server& serv, Channel& ch, Client& c, const ModeChange& mode){
	if (mode.set){
		//set pass
		if (!mode.hasArg || mode.arg.empty()) {
            serv.sendError(c, -1, "MODE +k: missing key");
            return false;
        }
		std::string key = mode.arg;
		ch.setKey(key); // if pass alredy exists - override it
		// should i send a conformation/info msg to the channel?
		return true;
	} else {
		//remove pass
		if (mode.hasArg) {
            serv.sendError(c, -1, "MODE -k: this mode takes no parameter");
            return false;
        }
		std::string empty = "";
		ch.setKey(empty);
		return true;
	}
}

bool handle_o(Server& serv, Channel& ch, Client& c, const ModeChange& mode){
	(void)serv; (void)ch; (void)c; (void)mode;
	return true;
}
bool handle_l(Server& serv, Channel& ch, Client& c, const ModeChange& mode){
	(void)serv; (void)ch; (void)c; (void)mode;
	return true;
}


std::map<char, ModeHandler> modeHandlers;

void handleMode() {
    modeHandlers['i'] = &handle_i;
    modeHandlers['t'] = &handle_t;
    modeHandlers['k'] = &handle_k;
    modeHandlers['o'] = &handle_o;
    modeHandlers['l'] = &handle_l;
}

void 	Server::execute_mode(Client &c, std::string &channelName, ModeParseResult modeOrganized){

	Channel *ch = getChannelByName(channelName);
	if (!ch){
		sendError(c, ERR_NOSUCHCHANNEL, MSG_NOSUCHCHANNEL(channelName));
		return;
	}
	if (!ch->isMember(c.getSocketFd())){
		sendError(c, ERR_NOTONCHANNEL, MSG_NOTONCHANNEL(channelName));
		return;
	}
	//bool isOperatour = ch->isOperator(c.getSocketFd());
	if (!ch->isOperator(c.getSocketFd())){
		sendError(c, ERR_CHANOPRIVSNEEDED, MSG_CHANOPRIVSNEEDED(channelName));
		return;
	}
	handleMode();
	for (size_t i = 0; i < modeOrganized.order.size(); i++)
	{
		const ModeChange current = modeOrganized.order[i];
		std::map<char, ModeHandler>::const_iterator it = modeHandlers.find(current.mode);

		if (it == modeHandlers.end()) { //same as "not found", we are checking the same in parsing already
            sendError(c, ERR_UNKNOWNMODE, MSG_UNKNOWNMODE(std::string(1, current.mode)));
            continue;
        }
		//it->first is the key (current.mode).
		// it->second is the value — the handler function pointer
		(it->second)(*this, *ch, c, current);
	}
	
}