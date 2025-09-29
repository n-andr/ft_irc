#include "../inc/Client.hpp"

// Orthodox Canonical Form (OCF)
Client::Client()
	: _socket_fd(-1), _ip_address(""), _port(0), _isConnected(false),
	  _has_passed_password(false), _nickname(""), _username(""),
	  _is_registered(false), _is_operator(false),
	  _read_buffer(""), _outgoing_buffer(""), _command_capitalized("") {}

Client::Client(const Client& other) {
	if (this != &other)
		*this = other;
}

Client& Client::operator=(const Client& other) {
	if (this != &other) {
		_socket_fd = other._socket_fd;
		_ip_address = other._ip_address;
		_port = other._port;
		_isConnected = other._isConnected;

		_has_passed_password = other._has_passed_password;
		_nickname = other._nickname;
		_username = other._username;
		_is_registered = other._is_registered;
		_is_operator = other._is_operator;

		_read_buffer = other._read_buffer;
		_outgoing_buffer = other._outgoing_buffer;
		_command_capitalized = other._command_capitalized;
	}
	return (*this);
}

Client::~Client() {}

//actual functions
void Client::joinChannel(const std::string &name) {
	if (isInvited(name))
		_invites.erase(name);
	_channels.insert(name);
}
void Client::leaveChannel(const std::string &name) { _channels.erase(name); }

std::string Client::getNextChunk(size_t max) const
{
	if (_outgoing_buffer.empty()) 
		return "";
	if (_outgoing_buffer.size() <= max) 
		return _outgoing_buffer;
	return _outgoing_buffer.substr(0, max);
}

void Client::consumeBytes(size_t n){
	if (n >= _outgoing_buffer.size()) {
		_outgoing_buffer.clear();
	} else {
		_outgoing_buffer.erase(0, n);
	}
}


bool Client::outgoingBufferIsEmpty() { return _outgoing_buffer.empty(); }

void Client::consumeBytesReadBuffer(size_t n){
	if (n >= _read_buffer.size()) {
		_read_buffer.clear();
	} else {
		_read_buffer.erase(0, n);
	}
}

bool Client::isInvited(const std::string& name) {
	for (std::set<std::string>::iterator it = _invites.begin(); it != _invites.end(); it++) {
		if (*it == name)
			return (true);
	}
	return (false);
}

void Client::addInvite(const std::string& name) {
	_invites.insert(name);
}