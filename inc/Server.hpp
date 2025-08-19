#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <iostream> // for std::cerr
#include <poll.h> // for poll() and struct pollfd
#include <vector> // for std::vector
#include <map> // for std::map
#include <sys/socket.h> // for recv
#include "Client.hpp"

#define L_SOCKET 0

class Server
{
private:
	int _port;
	std::string _password;
	int _serverSocket;
	std::vector<pollfd> _pollFds; // not sure, which one is better
	unsigned int	_nFds;//number of Fds including Listening Socket
	//std::vector<struct pollfd> _pollFds;
	std::map<int, Client> _clients;   // key = fd, value = Client object

public:
	Server(int port, const std::string& password);
	Server& operator=(const Server&);
	Server(const Server&);
	~Server();
	void start();
	void eventLoop();
	void handleNewConnection();
	void setupListeningSocket();
	void addListeningSocketToPoll();

};


#endif