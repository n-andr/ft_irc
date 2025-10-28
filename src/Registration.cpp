#include "../inc/Server.hpp"

void Server::sendWelcomes(Client &c) {
	sendServerReply(c, RPL_WELCOME, MSG_WELCOME(c.userPrefix()));
	sendServerReply(c, RPL_YOURHOST, MSG_YOURHOST(SERVER_NAME, SERVER_VER));
	sendServerReply(c, RPL_CREATED, MSG_CREATED(SERVER_CREATION_DATE));
	sendServerReply(c, RPL_MYINFO, MSG_MYINFO(SERVER_NAME, SERVER_VER, USER_MODES, CHANNEL_MODES));
}

void Server::pass(Client &c) {
	if (c.isRegistered()) {
		sendError(c, ERR_ALREADYREGISTRED, MSG_ALREADYREGISTRED);
		return;
	}
	if (c.getParams().size() < 1) {
		sendError(c, ERR_NEEDMOREPARAMS, MSG_NEEDMOREPARAMS("PASS"));
		std::cout << RED << "Too few Parameters for Pass. Research how to handle this" << RESET << std::endl;
		return ;
	}
	//if too many params == error is dependent on the server
	if (c.getParams()[0] == _password) {
		std::cout << GREEN << "Correct password" << RESET << std::endl;
		c.setHasPassedPassword(true);
		sendServerReply(c, -1, CUSTOM_PASS_CORRECT);
		if (c.getNickname().size() != 0 && c.getUsername().size()) {
			c.setRegistered(true);
			sendServerReply(c, -1, CUSTOM_REGISTRATION_SUCCESS);
			sendWelcomes(c);
		}
	}
	else
		sendError(c, ERR_PASSWDMISMATCH, MSG_PASSWDMISMATCH);
}

bool	Server::isSpecial(char c) {
	switch (c) {
        case '[': case ']': case '\\':
        case '`': case '_': case '^':
        case '{': case '|': case '}':
            return true;
        default:
            return false;
    }
}

bool Server::isValidNickname(std::string &s) {
	if (s.empty()||s.size() > 9)
		return (false);
	if (!std::isalpha(s[0]) && !isSpecial(s[0]))
		return (false);
	for (size_t i = 1; i < s.size(); i++) {
		if ((!std::isalnum(s[i]) && !isSpecial(s[i]) && s[i]!= '-') || s[i] == '#')
			return (false);
	}
	return (true);
}

void Server::nick(Client &c) {
	if (c.getParams().size() < 1) {
		sendError(c, ERR_NONICKNAMEGIVEN, MSG_NONICKNAMEGIVEN);
		std::cout << RED << "Too few Parameters for NICK. Research how to handle this" << RESET << std::endl;
		return ;
	}
	//if too many params == error is dependent on the server
	std::string requested_name = c.getParams()[0];
	if (isValidNickname(requested_name) == false) {
		sendError(c, ERR_NOSUCHNICK, MSG_NOSUCHNICK(requested_name));
		return ;
	}
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); it++) {
		if (it->second.getNickname() == requested_name) {
			sendError(c, ERR_NICKNAMEINUSE, MSG_NICKNAMEINUSE(requested_name));
			return ;
		}
	}
	c.setNickname(requested_name);
	std::cout << GREEN << "NICK successful" << RESET << std::endl;
	sendServerReply(c, -1, CUSTOM_NICK_SET(requested_name));
	if (c.hasPassedPassword() && !c.getUsername().empty() && c.isRegistered() == false) {
		c.setRegistered(true);
		sendServerReply(c, -1, CUSTOM_REGISTRATION_SUCCESS);
		sendWelcomes(c);
	}
}

bool Server::isValidUsername(std::string &s) {
	if (s.empty()||s.size() > 10)
		return (false);
	for (size_t i = 0; i < s.size(); i++) {
		if (!std::isprint(s[i]) || std::isspace(s[i]) || s[i] == '#')
			return (false);
	}
	return (true);
}

void Server::user(Client &c) {
	if (c.isRegistered()) {
		sendError(c, ERR_ALREADYREGISTRED, MSG_ALREADYREGISTRED);
		return;
	}
	const std::vector<std::string> p = c.getParams();
	if (p.size() < 3 || c.getTrailing().empty()) {
		sendError(c, ERR_NEEDMOREPARAMS, MSG_NEEDMOREPARAMS("USER"));
		return ;
	}
	//if too many params == error is dependent on the server
	std::string requested_name = p[0];
	if (!isValidUsername(requested_name)) {
		sendCustomError(c, CUSTOM_INVALID_USERNAME(requested_name));
		return ;
	}
	c.setUsername(requested_name);
	sendServerReply(c, -1, CUSTOM_USER_SET(requested_name));
	if (c.hasPassedPassword() && !c.getNickname().empty()) {
		c.setRegistered(true);
		sendServerReply(c, -1, CUSTOM_REGISTRATION_SUCCESS);
		sendWelcomes(c);
	}
	std::cout << GREEN << "USER successful" << RESET << std::endl;
}