#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <string>
#include <iostream> // for std::cerr
#include <poll.h> // for poll() and struct pollfd
#include <vector> // for std::vector
#include <map> // for std::map
#include <sys/socket.h> // for recv
#include <sstream>   // for std::istringstream
#include "Color.hpp"
#include <set>

class Client
{
private:
	int _socket_fd; // or alternatively a pointer to the pollfd_list instance?
	std::string _ip_address;
	int _port;
	bool _isConnected;

	bool _has_passed_password;
	std::string _nickname;
	std::string _username;
	bool _is_registered;// the above are filled. Can use the server now
	bool _is_operator;// has operator rights

	//something to save channels and invites. here is what GPT recommended:
	std::set<std::string> _channels;
	// std::set<std::string> invite_list;
  
	std::string _read_buffer;// what server gets from client
	std::string _outgoing_buffer;// what server has for client
	//std::string _command;// NULL or extracted command that needs to be parsed

	std::string _raw_command_input;  //without CRLF
	std::string _command_capitalized; //command name in uppercase
	std::vector<std::string> _params; 
	std::string _trailing; //string or NULL if no trailing


public:
	// Orthodox Canonical Form (OCF)
	Client();
	Client(const Client& other);
	Client& operator=(const Client& other);
	~Client();

	// Getters
	int getSocketFd() const;
	const std::string& getIpAddress() const;
	int getPort() const;
	bool isConnected() const;

	bool hasPassedPassword() const;
	const std::string& getNickname() const;
	const std::string& getUsername() const;
	bool isRegistered() const;
	bool isOperator() const;

	std::string& getReadBuffer();
	std::string& getOutgoingBuffer();
	std::string& getCommand();
	std::vector<std::string>& getParams();
	std::string& getTrailing();

	// Setters
	void setSocketFd(int fd);
	void setIpAddress(const std::string& ip);
	void setPort(int port);
	void setConnected(bool status);

	void setHasPassedPassword(bool value);
	void setNickname(const std::string& nick);
	void setUsername(const std::string& user);
	void setRegistered(bool value);
	void setOperator(bool value);

	void setReadBuffer(const std::string& buffer);
	void appendReadBuffer(const std::string& data); // extra helper
	void setOutgoingBuffer(const std::string& buffer);
	void appendOutgoingBuffer(const std::string& data); // extra helper
	void setCommand(const std::string& cmd);

	//actual Functions
	void joinChannel(const std::string& name);
	void leaveChannel(const std::string &name);

	//Read Buffer related
	bool extractCommand();
	void consumeBytesReadBuffer(size_t n);


	//outgoing Buffer realted
	std::string getNextChunk(size_t max) const;
	void consumeBytes(size_t n);
	bool outgoingBufferIsEmpty();
	void parseRawCommand();

	std::string prefix(std::string& target) const;

	//debugger
	void printCommand();


};

#endif