#include "../inc/Server.hpp"

/*
Listening setup: 
	setupListeningSocket(), setNonBlocking(int)
Poll integration: 
	addSocketToPoll(int)
Connection lifecycle: 
	handleNewConnection(), disconnectClient(int client_fd), errorDisconnect(int idx)
*/

void Server::setupListeningSocket() {
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocket < 0) 
		throw(std::runtime_error("Can't create a socket"));

	int opt = 1;
	if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) //setsockopt(SO_REUSEADDR) :  to restart server quickly SO_REUSEADDR allows to bind to the adress even if it’s in TIME_WAIT
   		throw(std::runtime_error("setsockopt(SO_REUSEADDR) failed"));

	sockaddr_in addr; 								// socket address struct.
	std::memset(&addr, 0, sizeof(addr));			// zero out all fields to avoid garbage in addr
	addr.sin_family			= AF_INET;				// AF_INET = Address Family = IPv4. (IPv6 would be AF_INET6.)
	addr.sin_addr.s_addr	= htonl(INADDR_ANY); 	//INADDR_ANY = 0.0.0.0 → bind on all local interfaces.
	addr.sin_port			= htons(_port);

	if (bind(_serverSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
	   	throw(std::runtime_error("bind failed"));

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


void Server::handleNewConnection() {
	struct sockaddr_in clientAddr;
	socklen_t addrLen = sizeof(clientAddr);
	int clientFd = accept(_serverSocket,
						reinterpret_cast<struct sockaddr*>(&clientAddr),
						&addrLen);
	if (clientFd < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) return; 	// With non-blocking listen socket it's normal to get EAGAIN/EWOULDBLOCK
		throw std::runtime_error(std::string("accept failed: ") + std::strerror(errno));
	}
	setNonBlocking(clientFd);

	std::cout << "[info] New client fd=" << clientFd
		  << " from " << inet_ntoa(clientAddr.sin_addr)
		  << ":" << ntohs(clientAddr.sin_port) << std::endl;

	addSocketToPoll(clientFd);
	// create a Client record here and store it in a map keyed by clientFd
	// create_client_object(new_client_socket); // to be done
	Client c;
	c.setSocketFd(clientFd);
	c.setIpAddress(inet_ntoa(clientAddr.sin_addr));// do we need this?
	c.setPort(ntohs(clientAddr.sin_port));
	_clients.insert(std::make_pair(clientFd, c));
	printClients();
}

void Server::disconnectClient(int client_fd){

	// never kill the listening socket by accident
#ifdef L_SOCKET
	if (!_pollFds.empty() && _pollFds[L_SOCKET].fd == client_fd) {
		std::cout << "[warn] refusing to disconnect listening socket fd=" << client_fd << std::endl;
		return;
	}
#endif

	if (client_fd >= 0)
		close(client_fd);
	for (std::vector<pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ++it) {
		if (it->fd == client_fd) {
			_pollFds.erase(it);
			break ;
		}
	}
	_nFds = _pollFds.size();
	_clients.erase(client_fd);

	std::cout << "[info] Client fd=" << client_fd << " disconnected" << std::endl; // log
	printClients();
	if (_clients.empty())
		_running = false;
}