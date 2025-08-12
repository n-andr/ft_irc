#include "../inc/Server.hpp"
#include <unistd.h> // for close()
//#include <fcntl.h> // for open(), close(), etc.

Server::Server(int port, const std::string& password) : _port(port), _password(password), _serverSocket(-1) {}
Server& Server::operator=(const Server& other){
	if(this != &other) {
		_port = other._port;
		_password = other._password;
		_serverSocket = other._serverSocket;
		_pollFds = other._pollFds; // Is it a correct way to copy the poll file descriptors or do i need to iterate?
	}
	return *this;
}
Server::Server(const Server& other)
: _port(other._port), _password(other._password), _serverSocket(-1) {}
Server::~Server() {
    for (std::vector<pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ++it) {
        if (it->fd >= 0) close(it->fd);
    }
}
void Server::start(){
	setupListeningSocket();
	addListeningSocketToPoll();
	std::cout << "[info] Listening on port " << _port << " ...\n";
	eventLoop();
}

void Server::eventLoop() {
    while (true) {
        // Polling and event handling logic goes here
    }
}

void Server::handleNewConnection() {
	// Logic to handle new connections
}

void Server::setupListeningSocket() {
	// Logic to set up the listening socket
}

void Server::addListeningSocketToPoll() {
	// something like _pollFds.push_back({ _serverSocket, POLLIN, 0 });
}
