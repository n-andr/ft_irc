#include "../inc/Server.hpp"

/* Orthodox Canonical Form */

Server::Server(int port, const std::string& password) : _port(port), _password(password), _serverSocket(-1), _nFds(0) {}

Server& Server::operator=(const Server& other){
	if(this != &other) {
		_port = other._port;
		_password = other._password;
		_serverSocket = -1; 		//reset socket
		_pollFds.clear(); 			// avoid double-close and undefined state if two Server objects manage the same FDs
		_nFds = other._nFds;
	}
	return *this;
}
Server::Server(const Server& other)
: _port(other._port), _password(other._password), _serverSocket(-1), _nFds(other._nFds) {}

Server::~Server() {
	for (std::vector<pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ++it) {
		if (it->fd >= 0) close(it->fd);
	}
}