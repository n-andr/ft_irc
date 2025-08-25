#include "../inc/Client.hpp"

// Orthodox Canonical Form (OCF)
Client::Client()
    : _socket_fd(-1), _ip_address(""), _port(0), _isConnected(false),
      _has_passed_password(false), _nickname(""), _username(""),
      _is_registered(false), _is_operator(false),
      _read_buffer(""), _outgoing_buffer(""), _command("") {}

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
        _command = other._command;
    }
    return (*this);
}

Client::~Client() {}

// Getters
int Client::getSocketFd() const { return _socket_fd; }
const std::string& Client::getIpAddress() const { return _ip_address; }
int Client::getPort() const { return _port; }
bool Client::isConnected() const { return _isConnected; }

bool Client::hasPassedPassword() const { return _has_passed_password; }
const std::string& Client::getNickname() const { return _nickname; }
const std::string& Client::getUsername() const { return _username; }
bool Client::isRegistered() const { return _is_registered; }
bool Client::isOperator() const { return _is_operator; }

std::string& Client::getReadBuffer() { return _read_buffer; }
std::string& Client::getOutgoingBuffer() { return _outgoing_buffer; }
std::string& Client::getCommand() { return _command; }

// Setters
void Client::setSocketFd(int fd) { _socket_fd = fd; }
void Client::setIpAddress(const std::string& ip) { _ip_address = ip; }
void Client::setPort(int port) { _port = port; }
void Client::setConnected(bool status) { _isConnected = status; }

void Client::setHasPassedPassword(bool value) { _has_passed_password = value; }
void Client::setNickname(const std::string& nick) { _nickname = nick; }
void Client::setUsername(const std::string& user) { _username = user; }
void Client::setRegistered(bool value) { _is_registered = value; }
void Client::setOperator(bool value) { _is_operator = value; }

void Client::setReadBuffer(const std::string& buffer) { _read_buffer = buffer; }
void Client::appendReadBuffer(const std::string& data) { _read_buffer += data; }
void Client::setOutgoingBuffer(const std::string& buffer) { _outgoing_buffer = buffer; }
void Client::appendOutgoingBuffer(const std::string& data) { _outgoing_buffer += data; }
void Client::setCommand(const std::string& cmd) { _command = cmd; }
