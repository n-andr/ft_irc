#include "../inc/Server.hpp"


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

void Server::disconnectClient(int index){
	int fd = _pollFds[index].fd;
	close(fd);
    std::cout << "[info] Client fd=" << fd << " disconnected" << std::endl;

    _pollFds[index] = _pollFds.back();
    _pollFds.pop_back();
    _nFds = static_cast<int>(_pollFds.size());
}



