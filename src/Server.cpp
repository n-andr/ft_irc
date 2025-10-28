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

//when a server sends a non error message, like the response to topic
void Server::sendServerReply(Client &c, int code,
					   const std::string &message) {
	std::ostringstream oss;
	oss << ":" << CYAN << SERVER_NAME << RESET;
	if (code != -1)
		oss << " " << GREEN << code << RESET;
	oss << " " << (c.getNickname().empty() ? "*" : c.getNickname())
		<< " " << c.getCommand()
		<< " :" << GREEN << message << RESET
		<< "\r\n";

	c.appendOutgoingBuffer(oss.str());// enqueue for POLLOUT
	enablePollout(c);
}

//when someone elses command affects a user. For example when someone invited you to a channel
void Server::sendInfoToTarget(Client &c, Client &t, const std::string &message) {
	std::ostringstream oss;
	oss << ":" << CYAN << c.userPrefix() << RESET
		//<< " " << GREEN << code << RESET
		<< " " << (c.getNickname().empty() ? "*" : c.getNickname())
		<< " " << c.getCommand() //optional add arguments after this?
		<< " " << (t.getNickname().empty() ? "*" : t.getNickname())
		<< " :" << GREEN << message << RESET
		<< "\r\n";

	t.appendOutgoingBuffer(oss.str());// enqueue for POLLOUT
	enablePollout(t);
}

void Server::sendInfoToChannel(Client &c, Channel &ch, const std::string &message) {
	std::ostringstream oss;
	oss << ":" << CYAN << c.userPrefix() << RESET
		//<< " " << GREEN << code << RESET
		<< " " << (c.getNickname().empty() ? "*" : c.getNickname())
		<< " " << c.getCommand() //optional add arguments after this?
		<< " " << (ch.getName().empty() ? "*" : ch.getName())
		<< " :" << GREEN << message << RESET
		<< "\r\n";
	
	for (std::set<int>::iterator it = ch.getMembers().begin(); it != ch.getMembers().end(); it++) {
		//if (*it != c.getSocketFd()) {
		_clients[*it].appendOutgoingBuffer(oss.str());// enqueue for POLLOUT
		enablePollout(_clients[*it]);
		//}
			//sendInfoToTarget(c, _clients[*it], message);
	}
}

//send code -1 for custom errors with no error code
void Server::sendError(Client &c, int code,
					   const std::string &message) {
	std::ostringstream oss;
	oss << ":" << CYAN << SERVER_NAME << RESET;
	if (code != -1) {                      // only print code number when present
        oss << " " << RED << code << RESET;
    }
	oss << " " << (c.getNickname().empty() ? "*" : c.getNickname())
		<< " " << c.getCommand()
		<< " :" << RED << message << RESET
		<< "\r\n";

	c.appendOutgoingBuffer(oss.str());// enqueue for POLLOUT
	enablePollout(c);
}

void Server::sendCustomError(Client &c, const std::string &message) {
	std::ostringstream oss;
	oss << ":" << CYAN << SERVER_NAME << RESET
		<< " " << (c.getNickname().empty() ? "*" : c.getNickname())
		<< " " << (c.getCommand().empty() ? "no Cmd parsed" : c.getCommand())
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

Channel* Server::getChannelByName(const std::string& name) {
	for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); it ++){
		if (it->first == name)
			return (&it->second);
	}
	return NULL;
}

std::map<std::string, Channel>::iterator Server::getChannelItByName(const std::string &name) {
    return _channels.find(name);
}

Channel* Server::createNewChannel(std::string& name) {
	std::map<std::string, Channel>::iterator it =
		_channels.insert(std::make_pair(name, Channel(name))).first;
	return (&it->second);
}