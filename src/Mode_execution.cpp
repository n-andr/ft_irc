#include "../inc/Server.hpp"

bool handle_i(Server& serv, Channel& ch, Client& c, const ModeChange& mode){
	if (mode.hasArg){
		serv.sendError(c, 696, ch.getName() + " i " + mode.arg + " :Mode i does not take a parameter");
		return false;
	}
	ch.setInviteOnly(mode.set); //if was invite-only - stayes invite-only - no error
	std::string params = ch.getName() + (mode.set ? " +i" : " -i");
	serv.sendInfoToChannel__HexChat_frienly(c, ch, "MODE", params, "", /*includeSelf=*/true);
	return true;
}

bool handle_t(Server& serv, Channel& ch, Client& c, const ModeChange& mode){
	
	if (mode.hasArg) {
		serv.sendError(c, 696, ch.getName() + " +t " + mode.arg + " :Mode t does not take a parameter");
		return false;
	}
	ch.setTopicLocked(mode.set); //if was locked - stayes locked - no error
	std::string params = ch.getName() + " " + (mode.set ? "+t" : "-t");
	serv.sendInfoToChannel__HexChat_frienly(c, ch, "MODE", params, "", /*includeSelf=*/true);
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
			serv.sendError(c, ERR_NEEDMOREPARAMS, MSG_NEEDMOREPARAMS("MODE"));
			return false;
		}
		std::string key = mode.arg;
		if (!isValidPassword(key)){
			serv.sendError(c, 696, ch.getName() + " k " + key + " :Invalid key");
			return false;
		}
		ch.setKey(key); // if pass alredy exists - override it
	} else {
		//remove pass
		if (mode.hasArg) {
			serv.sendError(c, 696, ch.getName() + " k " + mode.arg + " :Mode -k does not take a parameter");
			return false;
		}
		std::string empty = "";
		ch.setKey(empty);
	}
	std::string params = ch.getName();
	params += mode.set ? " +k" : " -k";
	serv.sendInfoToChannel__HexChat_frienly(c, ch, "MODE", params, "", /*includeSelf=*/true);
	return true;
}

bool handle_o(Server& serv, Channel& ch, Client& c, const ModeChange& mode){
	if (!mode.hasArg || mode.arg.empty()) {
		serv.sendError(c, ERR_NEEDMOREPARAMS, MSG_NEEDMOREPARAMS("MODE"));
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
	std::string params = ch.getName() + " " + (mode.set ? "+o " : "-o ") + targetNick;
	serv.sendInfoToChannel__HexChat_frienly(c, ch, "MODE", params, "", /*includeSelf=*/true);
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
			serv.sendError(c, 696, ch.getName() + " +l " + mode.arg + " :Invalid limit (positive integer required)");
			return false;
		}
		// Don’t allow a limit below current population
		const size_t currentUsers = ch.getMembers().size();
		if ((size_t)newLimit < currentUsers) {
			serv.sendError(c, 696, ch.getName() + " +l " + mode.arg + " :Limit below current number of users");
			return false;
		}
		if ((size_t)newLimit > MAX_CLIENTS) {
			serv.sendError(c, 696, ch.getName() + " +l " + mode.arg + " :Limit exceeds server maximum");
			return false;
		}
		ch.setUserLimit(newLimit);
	} else {
		//remove limit
		if (mode.hasArg) {
			serv.sendError(c, 696, ch.getName() + " l " + mode.arg + " :this mode takes no parameter");
			return false;
		}
		if (ch.getUserLimit() != 0) ch.setUserLimit(0); // 0 == “no limit”
		
	}
	std::string params = ch.getName();
	params += mode.set ? (" +l " + mode.arg) : " -l";
	serv.sendInfoToChannel__HexChat_frienly(c, ch, "MODE", params, "", /*includeSelf=*/true);
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
	if (!ch->isMember(c.getSocketFd())){
		sendError(c, ERR_NOTONCHANNEL, MSG_NOTONCHANNEL(channelName));
		return;
	}
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