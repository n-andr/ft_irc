#include "../inc/Server.hpp"

Server::Server(int port, const std::string& password) : _port(port), _password(password), _serverSocket(-1) {}
Server& Server::operator=(const Server& other){
	if(this != &other) {
		_port = other._port;
		_password = other._password;
		// _serverSocket = other._serverSocket;
		// _pollFds = other._pollFds; //Do  Is it a correct way to copy the poll file descriptors?
		_serverSocket = -1; //reset socket
        _pollFds.clear(); // avoid double-close and undefined state if two Server objects manage the same FDs
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


/*
AF_INET 
	= Address Family = IPv4. (IPv6 would be AF_INET6.)
SOCK_STREAM 
	= TCP
INADDR_ANY
	= 0.0.0.0 → bind on all local interfaces.

setsockopt(SO_REUSEADDR) :  to restart server quickly

	When you close a TCP server socket, the OS often doesn’t free the port immediately.
	Instead, it leaves it in a special state called TIME_WAIT (usually ~30–120 seconds)
	SO_REUSEADDR allows to bind to the adress even if it’s in TIME_WAIT.
*/
void Server::setupListeningSocket() {
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocket < 0) 
		throw(std::runtime_error("Can't create a socket"));

	int opt = 1;
	if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
   		throw(std::runtime_error("setsockopt(SO_REUSEADDR) failed"));

	sockaddr_in addr; //socket address struct.
	std::memset(&addr, 0, sizeof(addr)); //zero out all fields to avoid garbage in addr
	addr.sin_family			= AF_INET;
	addr.sin_addr.s_addr	= htonl(INADDR_ANY);
	//addr.sin_port			= htons(static_cast<uint16_t>(_port));
	addr.sin_port			= htons(_port);

	if (bind(_serverSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
       	throw(std::runtime_error("bind failed"));

	//maybe better to use connect() ???
	if (listen(_serverSocket, SOMAXCONN) < 0)
       	throw(std::runtime_error("listen failed"));
	
	setNonBlocking(_serverSocket);
}

void Server::addListeningSocketToPoll() {
	// something like, but don't know how to test if correct
	struct pollfd pfd;
    pfd.fd = _serverSocket;
    pfd.events = POLLIN;
    pfd.revents = 0;
    _pollFds.push_back(pfd);
}


void Server::setNonBlocking(int fd){
	int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) throw(std::runtime_error("fcntl(F_GETFL) failed"));
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) throw(std::runtime_error("fcntl(F_GETFL) failed"));
}