#include "../inc/Server.hpp"
#include <unistd.h> // for close()
//#include <fcntl.h> // for open(), close(), etc.

Server::Server(int port, const std::string& password) : _port(port), _password(password), _serverSocket(-1), _nFds(0) {}
Server& Server::operator=(const Server& other){
	if(this != &other) {
		_port = other._port;
		_password = other._password;
		_serverSocket = other._serverSocket;
		_pollFds = other._pollFds; // Is it a correct way to copy the poll file descriptors or do i need to iterate?
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
void Server::start(){
	setupListeningSocket();
	addListeningSocketToPoll();
	std::cout << "[info] Listening on port " << _port << " ...\n";
	eventLoop();
}

void Server::eventLoop()
{
	std::cout << "Event Loop" << std::endl;//control print
	while (true)
	{
		poll(&_pollFds[0], _nFds, 100);
		if (_pollFds[L_SOCKET].revents & POLLIN)
			this->handleNewConnection();
		for (int i = 1; i < _nFds; i++)
		{
			int bytes_read = 0;
			if (_pollFds[i].revents & POLLIN)
			{
				Client *c = &_clients[_pollFds[i].fd];
				bytes_read = recv(_pollFds[i].fd, &(c->getReadBuffer()), sizeof(c->getReadBuffer()), 0);//getters missing
				if (bytes_read <= 0)
					std::cout << "empty read\n";
					//empty_read();
				else
					std::cout << "non empty read\n";
					//non_empty_read();
			}
			if (_pollFds[i].revents & POLLOUT)
				std::cout << "send data\n";
				//send_data();
		}
		break ;//since we aren't actually listening for anything yet
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
//add line for _nFds ++;