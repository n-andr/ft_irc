#include "../inc/Server.hpp"

void Server::disablePollout(Client &client) {
	for (size_t i = 0; i < _pollFds.size(); ++i) {
		if (_pollFds[i].fd == client.getSocketFd()) {
			_pollFds[i].events &= ~POLLOUT; // clear POLLOUT bit
			break;
		}
	}
}

void Server::enablePollout(Client &client) {
	for (size_t i = 0; i < _pollFds.size(); ++i) {
		if (_pollFds[i].fd == client.getSocketFd()) {
			_pollFds[i].events |= POLLOUT;  // fill POLLOUT bit
			break;
		}
	}
}

//when a server sends a non error message, like the response to topic
void Server::sendServerReply(Client &c, int code, const std::string &message) {
	std::ostringstream oss;
	oss << ":" << SERVER_NAME;
	if (code != -1)
		oss << " " << code;
	else
		oss << " NOTICE ";
	oss << " " << (c.getNickname().empty() ? "*" : c.getNickname())
		<< " :" << message 
		<< "\r\n";

	c.appendOutgoingBuffer(oss.str());
	enablePollout(c);
	sendPendingData(c);
}

//when someone elses command affects a user. For example when someone invited you to a channel
void Server::sendInfoToTarget(Client &c, Client &t, const std::string &message) {
	std::ostringstream oss;
	oss << ":" << c.userPrefix()
		<< " " << (c.getNickname().empty() ? "*" : c.getNickname())
		<< " " << c.getCommand()
		<< " " << (t.getNickname().empty() ? "*" : t.getNickname())
		<< " :" << message 
		<< "\r\n";

	t.appendOutgoingBuffer(oss.str());
	enablePollout(t);
}

//sends info message to channel
void Server::sendInfoToChannel(Client &c, Channel &ch, const std::string &message) {
	std::ostringstream oss;
	oss << ":" << c.userPrefix()
		<< " " << (c.getNickname().empty() ? "*" : c.getNickname())
		<< " " << c.getCommand()
		<< " " << (ch.getName().empty() ? "*" : ch.getName())
		<< " :" << message 
		<< "\r\n";
	
	for (std::set<int>::iterator it = ch.getMembers().begin(); it != ch.getMembers().end(); it++) {
		_clients[*it].appendOutgoingBuffer(oss.str());
		enablePollout(_clients[*it]);
	}
}

//new HexChat friendly  broadcast to channel function

// Builds: :nick!user@host COMMAND <params...> [:trailing]\r\n
static std::string ircLine(const std::string& prefix,
                           const std::string& command,
                           const std::string& params,      // space-separated, may be empty
                           const std::string& trailing) {  // without leading ':'
    std::ostringstream oss;
    oss << ":" << prefix << " " << command;
    if (!params.empty())   oss << " " << params;
    if (!trailing.empty()) oss << " :" << trailing;
    oss << "\r\n";
    return oss.str();
}

void Server::sendInfoToChannel__HexChat_frienly(Client& from, Channel& ch,
                                const std::string& command,
                                const std::string& params,
                                const std::string& trailing,
                                bool includeSelf /*=true*/) {
    const std::string line = ircLine(from.userPrefix(), command, params, trailing);
    for (std::set<int>::iterator it = ch.getMembers().begin(); it != ch.getMembers().end(); ++it) {
        if (!includeSelf && *it == from.getSocketFd()) continue;
        _clients[*it].appendOutgoingBuffer(line);
		std::cout << "Line:" << line << std::endl;
        enablePollout(_clients[*it]);
		sendPendingData(_clients[*it]);
    }
}

//send code -1 for custom errors with no error code
void Server::sendError(Client &c, int code, const std::string &message) {
	std::ostringstream oss;
	oss << ":" SERVER_NAME;
	if (code != -1)
		oss << " " << code;
	oss << " " << (c.getNickname().empty() ? "*" : c.getNickname())
		<< " " << c.getCommand()
		<< " :" << message
		<< "\r\n";

	c.appendOutgoingBuffer(oss.str());
	enablePollout(c);
}

//for sending custom errors
void Server::sendCustomError(Client &c, const std::string &message) {
	std::ostringstream oss;
	oss << ":" << SERVER_NAME
		<< " " << (c.getNickname().empty() ? "*" : c.getNickname())
		<< " " << (c.getCommand().empty() ? "no Cmd parsed" : c.getCommand())
		<< " :" << message
		<< "\r\n";

	c.appendOutgoingBuffer(oss.str());
	enablePollout(c);
}

Client* Server::getClientByNick(std::string& nick)
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