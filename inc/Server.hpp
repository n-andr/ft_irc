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
#include <cstdlib> //atoi

#define MAX_CLIENTS 100
#include "Client.hpp"
#include "Color.hpp"
#include "Channel.hpp"
#include "Macros.hpp"

#define L_SOCKET 0

struct ModeChange {
    bool        set;    // +(set) = true, -(unset) = false
    char        mode;
    bool        hasArg;
    std::string arg;       // used if hasArg == true
};

//organize all options and args in this struct
struct ModeParseResult {
    std::vector<ModeChange> order;    // what to execute in order
    std::string             error;   // empty if no errors 
    std::vector<std::string> leftoverArgs; // any args not consumed
};

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
	void eraseClientFootprint(int client_fd);
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
	void kick(Client& c);
	void topic(Client& c);
	void mode(Client& c);

	ModeParseResult splitModeParams(Client &c);
	void 	execute_mode(Client &c, std::string &channelName, ModeParseResult modeOrganized);

	Channel* getChannelByName(const std::string& name);
	std::map<std::string, Channel>::iterator getChannelItByName(const std::string &name);

	Channel* createNewChannel(std::string& name);
	void sendServerReply(Client &c, int code, const std::string &message);
	void sendInfoToTarget(Client &c, Client &t, const std::string &message);
	void sendInfoToChannel(Client &c, Channel &ch, const std::string &message);

	void sendCustomError(Client &c, const std::string &message);
	void sendError(Client &c, int code, const std::string &message);
	Client* getClientByNick(std::string& nick);
	void sendWelcomes(Client &c);
	bool isValidUsername(std::string &s);
	bool isValidNickname(std::string &s);
	bool isSpecial(char c);
	bool validChannelName(std::string &s);
	
	
	//For debugging only
	void printClients();
	void printModeParseResult(const std::string channelName, const ModeParseResult &r);
	void printChannelInfo( Channel &channel);


};

typedef bool (*ModeHandler)(Server&, Channel&, Client&, const ModeChange&);


#endif