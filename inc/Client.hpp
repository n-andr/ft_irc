#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <string>
#include <iostream> // for std::cerr
#include <poll.h> // for poll() and struct pollfd
#include <vector> // for std::vector
#include <map> // for std::map
#include <sys/socket.h> // for recv

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
    // std::set<std::string> channels;
    // std::set<std::string> invite_list;
  
    std::string _read_buffer;// what server gets from client
    std::string _outgoing_buffer;// what server has for client
    std::string _command;// NULL or extracted command that needs to be parsed
public:
	std::string& getReadBuffer();
};

#endif