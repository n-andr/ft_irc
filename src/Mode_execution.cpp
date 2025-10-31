#include "../inc/Server.hpp"

bool handle_i(Server& serv, Channel& ch, Client& c, const ModeChange& mode){
	if (mode.hasArg){
		serv.sendError(c, -1, "MODE i: this mode takes no parameter");
		return false;
	}
	ch.setInviteOnly(mode.set); //if was invite-only - stayes invite-only - no error
	serv.sendInfoToChannel(c, ch, CUSTOM_MODE_CHANGE(
		ch.getInviteOnly(),
		"This channel is now invite-only.",
		"This channel is open to anyone (no invite required)."
	));
	return true;
}

bool handle_t(Server& serv, Channel& ch, Client& c, const ModeChange& mode){
	
	if (mode.hasArg) {
	serv.sendError(c, -1, "MODE t: this mode takes no parameter");
	return false;
	}
	ch.setTopicLocked(mode.set); //if was locked - stayes locked - no error
	serv.sendInfoToChannel(c, ch, CUSTOM_MODE_CHANGE(
		ch.getTopicLocked(),
		"Only channel operators can set the topic now.",
		"Anyone in the channel can set the topic now."
	));
	return true;
}

static bool isValidPassword(std::string s) {
	if (s.empty() || s.size() > MAX_PASS_SIZE)
		return false;
	for (size_t i = 0; i < s.size(); i++) {
		unsigned char ch = static_cast<unsigned char>(s[i]);
		if (!std::isprint(ch)) return false; // must be printable ASCII
		if (std::isspace(ch)) return false; // no spaces, tabs, etc.
		if (ch == ':') return false; //not to confuse with trailing
	}
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
		if (!isValidPassword(key)){
			serv.sendError(c, -1, "MODE +k: key is not valid");
			return false;
		}
		ch.setKey(key); // if pass alredy exists - override it
	} else {
		//remove pass
		if (mode.hasArg) {
			serv.sendError(c, -1, "MODE -k: this mode takes no parameter");
			return false;
		}
		std::string empty = "";
		ch.setKey(empty);
	}
	serv.sendInfoToChannel(c, ch, CUSTOM_MODE_CHANGE(
		(ch.getKey() != "" ? true : false),
		"A password is now required to join this channel.",
		"The password requirement has been removed."
	));
	return true;
}

bool handle_o(Server& serv, Channel& ch, Client& c, const ModeChange& mode){
	if (!mode.hasArg || mode.arg.empty()) {
		serv.sendError(c, -1, "MODE +/-o: missing key");
		return false;
	}
	std::string targetNick = mode.arg;
	Client *target = serv.getClientByNick(targetNick);
	if (target == NULL) {
		serv.sendError(c, ERR_NOSUCHNICK, MSG_NOSUCHNICK(targetNick));
		return false;
	}
	if (!ch.isMember(target->getSocketFd())){
		serv.sendError(c, ERR_USERNOTINCHANNEL, MSG_USERNOTINCHANNEL(targetNick, ch.getName()));
		return false;
	}
	if (mode.set){
		//give operator rights
		if (!ch.isOperator(target->getSocketFd()))
			ch.addOperator(target->getSocketFd());
	} else {
		//remove operator rights
		if (ch.isOperator(target->getSocketFd()))
			ch.removeOperator(target->getSocketFd());
	}
	serv.sendInfoToChannel(c, ch, CUSTOM_MODE_CHANGE(
		mode.set,
		mode.arg + " is a channel operator.",
		mode.arg + " is not a channel operator."
	));
	return true;
}

//helper to convert int into string
static std::string toStr(int v) { std::ostringstream oss; oss << v; return oss.str(); }

//helper to convert str into positive int
static bool parsePositiveInt(const std::string& s, int& out) {
	if (s.empty()) return false;
	for (size_t i = 0; i < s.size(); ++i) if (s[i] < '0' || s[i] > '9') return false;
	out = std::atoi(s.c_str());
	return out > 0;
}

bool handle_l(Server& serv, Channel& ch, Client& c, const ModeChange& mode){
	
	if (mode.set){
		//set members limit 
		int newLimit = 0;
		if (!mode.hasArg || !parsePositiveInt(mode.arg, newLimit)) {
			serv.sendError(c, -1, "MODE +l: bad or missing limit");
			return false;
		}
		// Don’t allow a limit below current population
		const size_t currentUsers = ch.getMembers().size();
		if ((size_t)newLimit < currentUsers) {
			serv.sendError(c, -1, "MODE +l: limit is below current number of users");
			return false;
		}
		if ((size_t)newLimit > MAX_CLIENTS) {
			serv.sendError(c, -1, "MODE +l: limit is bigger than maximum clients per server");
			return false;
		}
		ch.setUserLimit(newLimit);
	} else {
		//remove limit
		if (mode.hasArg) {
		serv.sendError(c, -1, "MODE -l: this mode takes no parameter");
		return false;
		}
		if (ch.getUserLimit() != 0) ch.setUserLimit(0); // 0 == “no limit”
		
	}

	serv.sendInfoToChannel(c, ch, CUSTOM_MODE_CHANGE(
		(ch.getUserLimit() != 0 ? true : false),
		(std::string("User limit set to ") + toStr(ch.getUserLimit()) + "."),
		"The user limit has been removed."
	));
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

void Server::printChannelModes(Client &c, const std::string &channelName){
	std::string modeString = "+";
	std::string params = "";
	Channel *ch = getChannelByName(channelName);
	if (!ch){
		sendError(c, ERR_NOSUCHCHANNEL, MSG_NOSUCHCHANNEL(channelName));
		return;
	}
	if (ch->getInviteOnly()) modeString += 'i';
	if (ch->getTopicLocked()) modeString += 't';
	if (!ch->getKey().empty()) {
		modeString += 'k';
		params += '*';
	}
	if (ch->getUserLimit() != 0) {
		modeString += 'l';
		if (!params.empty()) params += ' ';
		params += toStr(ch->getUserLimit());
	}
	sendServerReply(c, RPL_CHANNELMODEIS, MSG_CHANNELMODEIS(channelName, modeString, params));
}

void 	Server::execute_mode(Client &c, std::string &channelName, ModeParseResult modeOrganized){

	Channel *ch = getChannelByName(channelName);
	if (!ch){
		sendError(c, ERR_NOSUCHCHANNEL, MSG_NOSUCHCHANNEL(channelName));
		return;
	}
	//debug (delete later)
	//std::cout << "Executing MODE for channel: " << channelName << std::endl;
	//std::cout << "Before execution" << channelName << std::endl;
	//printChannelInfo(*ch);
	//end debug
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
	//debug (delete later)
	//std::cout << "After execution" << channelName << std::endl;
	//printChannelInfo(*ch);
	//end debug
}