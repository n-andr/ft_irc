#include "../inc/Server.hpp"
bool Server::_running = true;
Server::Server(int port, const std::string& password) : _port(port), _password(password), _serverSocket(-1), _nFds(0) {}
Server& Server::operator=(const Server& other){
	if(this != &other) {
		_port = other._port;
		_password = other._password;
		// _serverSocket = other._serverSocket;
		// _pollFds = other._pollFds; //Do  Is it a correct way to copy the poll file descriptors?
		_serverSocket = -1; //reset socket
		_pollFds.clear(); // avoid double-close and undefined state if two Server objects manage the same FDs
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
	addSocketToPoll(_serverSocket);
	std::cout << "[info] Listening on port " << _port << " ...\n";
	eventLoop();
}

void Server::empty_read(int client_fd) 
{
	// 1. Close the socket
	close(client_fd);
	// 2. Remove from poll list
	for (std::vector<pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ++it) {
		if (it->fd == client_fd) {
			_pollFds.erase(it);
			break ;
		}
	}
	_nFds = _pollFds.size();  // keep counter in sync
	// 3. Remove from clients map
	_clients.erase(client_fd);
	// 4. Remove from all chat sets it is in
	// tba
	//control print
	std::cout << "Client " << client_fd << " disconnected." << std::endl;
}

void Server::errorDisconnect(int client_fd)
{
	// 1. Close the socket
	close(client_fd);
	// 2. Remove from poll list
	for (std::vector<pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ++it) {
		if (it->fd == client_fd) {
			_pollFds.erase(it);
			break ;
		}
	}
	_nFds = _pollFds.size();  // keep counter in sync
	// 3. Remove from clients map
	_clients.erase(client_fd);
	// 4. Remove from all chat sets it is in
	// tba
	//control print
	std::cout << RED << "Error with Client " << client_fd << ". Client got disconnected by the server." << RESET << std::endl;
}

void Server::eventLoop()
{
	std::cout << "Event Loop starts:" << std::endl;//control print
	while (_running)//server is running
	{
		poll(&_pollFds[0], _nFds, 100);
		if (_pollFds[L_SOCKET].revents & POLLIN)
			this->handleNewConnection();
		for (int i = 1; i < _nFds; i++)
		{
			if (_pollFds[i].revents & POLLIN)
			{
				char buf[512];
				int bytes_read = recv(_pollFds[i].fd, buf, sizeof(buf), 0);

				if (bytes_read <= 0) {
					disconnectClient(i);
					continue;
				} else {
					std::string msg(buf, bytes_read);
					std::cout << "[recv] from fd=" << _pollFds[i].fd << ": " << msg;
					broadcastMessage(msg, _pollFds[i].fd);
				}
			}



			// ingoring cliet for now

			// int bytes_read = 0;
			// if (_pollFds[i].revents & POLLIN)
			// {
			// 	Client *c = &_clients[_pollFds[i].fd];
			// 	bytes_read = recv(_pollFds[i].fd, &(c->getReadBuffer()), sizeof(c->getReadBuffer()), 0);
			// 	if (bytes_read <= 0) {
			// 		// client disconnected or error
			// 		disconnectClient(i);
			// 		continue;
			// 	} else {
			// 		// got some data
			// 		std::string msg(c->getReadBuffer(), bytes_read);
			// 		std::cout << "[recv] from fd=" << _pollFds[i].fd << ": " << msg;
			// 		broadcastMessage(msg, _pollFds[i].fd);
			// 	}
			// }
			// if (_pollFds[i].revents & POLLOUT)
			// 	std::cout << "send data\n";
			// 	//send_data();
		}
		//break ;//since we aren't actually listening for anything yet
	}
	std::cout << "\nAfter Poll Loop\n";
}

void Server::handleNewConnection() {
	// Logic to handle new connections
// 	new_client_socket = accept(listen_socket, ...);
// set_socket_nonblocking(new_client_socket);
// //now create a new list element for pollfd_list.
// add_to_poll_list(new_client_socket);
// create_client_object(new_client_socket);

	struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    int clientFd = accept(_serverSocket,
                          reinterpret_cast<struct sockaddr*>(&clientAddr),
                          &addrLen);
    if (clientFd < 0) {
        // With non-blocking listen socket it's normal to get EAGAIN/EWOULDBLOCK
        if (errno == EAGAIN || errno == EWOULDBLOCK) return;
        throw std::runtime_error(std::string("accept failed: ") + std::strerror(errno));
    }
	setNonBlocking(clientFd);

	// std::cout << "[info] New client fd=" << clientFd
    //       << " connected" << std::endl;

	//more informative msg:
	std::cout << "[info] New client fd=" << clientFd
          << " from " << inet_ntoa(clientAddr.sin_addr)
          << ":" << ntohs(clientAddr.sin_port) << std::endl;

	addSocketToPoll(clientFd);
	// ? Do we need to create a Client record here and store it in a map keyed by clientFd?

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

void Server::addSocketToPoll(int socket) {
	struct pollfd pfd;
	pfd.fd = socket;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_pollFds.push_back(pfd);
	_nFds += 1;
}

//set non blocking, so it can't freeze the loop
void Server::setNonBlocking(int fd){
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags < 0) throw(std::runtime_error("fcntl(F_GETFL) failed"));
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) throw(std::runtime_error("fcntl(F_GETFL) failed"));
}

void Server::disconnectClient(int index){
	int fd = _pollFds[index].fd;
	close(fd);
    std::cout << "[info] Client fd=" << fd << " disconnected" << std::endl;

    _pollFds[index] = _pollFds.back();
    _pollFds.pop_back();
    _nFds = static_cast<int>(_pollFds.size());
}

// Send a message to all connected clients (testing simple broadcast)
void Server::broadcastMessage(const std::string &msg, int senderFd) {
    for (size_t j = 1; j < _pollFds.size(); ++j) {
        int fd = _pollFds[j].fd;
        if (fd != senderFd) { //skip sender to avoid echoing msg
            send(fd, msg.c_str(), msg.size(), 0);
        }
    }
}

void Server::signalHandler(int signum)
{
	std::cout << "signal received: " << signum << std::endl;
	if (signum == SIGINT)
		_running = false;
}