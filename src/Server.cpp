#include "../inc/Server.hpp"

void Server::disablePollout(Client &client) {
	for (size_t i = 0; i < _pollFds.size(); ++i) {
		if (_pollFds[i].fd == client.getSocketFd()) {
			_pollFds[i].events &= ~POLLOUT;  // clear POLLOUT bit
			break;
		}
	}
}

void Server::enablePollout(Client &client) {
	for (size_t i = 0; i < _pollFds.size(); ++i) {
		if (_pollFds[i].fd == client.getSocketFd()) {
			_pollFds[i].events |= POLLOUT;  // clear POLLOUT bit
			break;
		}
	}
}

void Server::sendError(Client &c, int code,
					   const std::string &message) {
	std::ostringstream oss;
	oss << ":" << CYAN << SERVER_NAME << RESET
		<< " " << RED << code << RESET
		<< " " << (c.getNickname().empty() ? "*" : c.getNickname())
		<< " " << c.getCommand()
		<< " :" << RED << message << RESET
		<< "\r\n";

	c.appendOutgoingBuffer(oss.str());// enqueue for POLLOUT
	enablePollout(c);
}

Client* Server::getClientByNick(std::string& nick)//non existend -> NULL
{
	for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->second.getNickname() == nick) {
			return &it->second;
		}
	}
	return NULL;
}

Channel* Server::getChannelByName(std::string& name) {
	for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); it ++){
		if (it->first == name)
			return (&it->second);
	}
	return NULL;
}

Channel* Server::createNewChannel(std::string& name) {
	std::map<std::string, Channel>::iterator it =
		_channels.insert(std::make_pair(name, Channel(name))).first;
	return (&it->second);
}
