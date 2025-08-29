#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <iostream> // for std::cerr
#include <poll.h> // for poll() and struct pollfd
#include <vector> // for std::vector
#include <unistd.h> // for close()
#include <fcntl.h> // for fcntl() open(), close(), etc.
#include <sys/socket.h> // socket(), AF_INET, SOCK_STREAM
#include <netinet/in.h> // struct sockaddr_in
#include <cstring> //memset
#include <csignal> //signal

#define MAX_CLIENTS 100
#include <map> // for std::map
#include "Client.hpp"
#include "Color.hpp"

#define L_SOCKET 0

class Server
{
private:
	int _port;
	std::string _password;
	int _serverSocket;
	std::vector<pollfd> _pollFds; // not sure, which one is better
	int	_nFds;//number of Fds including Listening Socket
	//std::vector<struct pollfd> _pollFds;
	std::map<int, Client> _clients;   // key = fd, value = Client object
	static bool _running;

public:
	Server(int port, const std::string& password);
	Server& operator=(const Server&);
	Server(const Server&);
	~Server();
	void start();
	void eventLoop();
	void handleNewConnection();
	void setupListeningSocket();
	void addSocketToPoll(int socket);
	void setNonBlocking(int fd);

	void empty_read(int client_fd);
	static void signalHandler(int signum);
	void errorDisconnect(int client_fd);

};


#endif