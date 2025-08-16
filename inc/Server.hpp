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

#define MAX_CLIENTS 100

class Server
{
private:
	int _port;
	std::string _password;
	int _serverSocket;
	std::vector<pollfd> _pollFds; // not sure, which one is better
	//std::vector<struct pollfd> _pollFds;
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
	void setNonBlocking(int fd);

};


#endif