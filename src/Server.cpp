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