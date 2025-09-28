#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <iostream>		// std::cerr
#include <poll.h>		// poll() and struct pollfd
#include <vector>		// std::vector
#include <unistd.h>		// close()
#include <fcntl.h>		// fcntl() open(), close(), etc.
#include <sys/socket.h> // socket(), AF_INET, SOCK_STREAM
#include <netinet/in.h> // struct sockaddr_in
#include <cstring> 		// memset
#include <arpa/inet.h> 	// inet_ntoa
#include <csignal> 		// signal
#include <map> 			// std::map
#include <cerrno>

#define MAX_CLIENTS 100
#include "Client.hpp"
#include "Color.hpp"
#include "Channel.hpp"
#include "Macros.hpp"

#define L_SOCKET 0

class Server
{
private:
	int _port;
	std::string _password;
	int _serverSocket;
	std::vector<pollfd> _pollFds;
	int	_nFds; 							//number of Fds including Listening Socket
	std::map<int, Client> _clients;		// key = fd, value = Client object
	static bool _running;
	std::map<std::string, Channel> _channels;//key = #servername, value = Channel class object

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
	void disconnectClient(int index);
	void broadcastMessage(const std::string &msg, int senderFd);

	void sendPendingData(Client &c);
	void disablePollout(Client &client);
	void enablePollout(Client &client);


	//void empty_read(int client_fd);
	static void signalHandler(int signum);
	//void errorDisconnect(int client_fd);
	void delegateCommand(Client &c);

	//commands
	void pass(Client& c);
	void nick(Client& c);
	void user(Client& c);
	void privmsg(Client& c);
	void privmsgToChannel(Client&c, std::string& name);
	void join(Client& c);
	void invite(Client& c);

	Channel* getChannelByName(std::string& name);

	Channel* createNewChannel(std::string& name);
	
	void sendError(Client &c, int code, const std::string &message);
	Client* getClientByNick(std::string& nick);
	
	//For debugging only
	void printClients();


};


#endif