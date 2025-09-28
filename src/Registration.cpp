#include "../inc/Server.hpp"

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
		if (c.getNickname().size() != 0 && c.getUsername().size())
			c.setRegistered(true);
	}
	else
		sendError(c, ERR_PASSWDMISMATCH, MSG_PASSWDMISMATCH);
}

void Server::nick(Client &c) {
	if (c.getParams().size() < 1) {
		sendError(c, ERR_NONICKNAMEGIVEN, MSG_NONICKNAMEGIVEN);
		std::cout << RED << "Too few Parameters for NICK. Research how to handle this" << RESET << std::endl;
		return ;
	}
	//if too many params == error is dependent on the server
	std::string requested_name = c.getParams()[0];
	//tba: validity of nickname
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); it++) {
		if (it->second.getNickname() == requested_name) {
			sendError(c, ERR_NICKNAMEINUSE, MSG_NICKNAMEINUSE(requested_name));
			return ;
		}
	}
	c.setNickname(requested_name);
	std::cout << GREEN << "NICK successful" << RESET << std::endl;
	if (c.hasPassedPassword() && !c.getUsername().empty())
		c.setRegistered(true);
}

void Server::user(Client &c) {
	if (c.isConnected()) {
		sendError(c, ERR_ALREADYREGISTRED, MSG_ALREADYREGISTRED);
		return;
	}
	const std::vector<std::string> p = c.getParams();
	if (p.size() < 4) {
		sendError(c, ERR_NEEDMOREPARAMS, MSG_NEEDMOREPARAMS("USER"));
		std::cout << RED << "Too few Parameters for USER. Research how to handle this" << RESET << std::endl;
		return ;
	}
	//if too many params == error is dependent on the server
	std::string requested_name = p[0];
	//tba: validity of nickname
	//the other 3 arguments given. to store or not to store?
	c.setUsername(requested_name);
	if (c.hasPassedPassword() && !c.getNickname().empty())
		c.setRegistered(true);
	std::cout << GREEN << "USER successful" << RESET << std::endl;
	
}