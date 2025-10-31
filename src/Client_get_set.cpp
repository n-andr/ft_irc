#include "../inc/Client.hpp"

// Getters
int Client::getSocketFd() const { return _socket_fd; }
const std::string& Client::getIpAddress() const { return _ip_address; }
int Client::getPort() const { return _port; }

bool Client::hasPassedPassword() const { return _has_passed_password; }
const std::string& Client::getNickname() const { return _nickname; }
const std::string& Client::getUsername() const { return _username; }
bool Client::isRegistered() const { return _is_registered; }
bool Client::isOperator() const { return _is_operator; }

std::string& Client::getReadBuffer() { return _read_buffer; }
std::string& Client::getOutgoingBuffer() { return _outgoing_buffer; }
std::string& Client::getCommand() { return _command_capitalized; }
std::vector<std::string>& Client::getParams() { return _params; }
std::string& Client::getTrailing() { return _trailing; }
std::set<std::string>& Client::getChannels() { return _channels; }
bool Client::getdisconnect() { return _disconnect; }
bool Client::getBufOverflow() { return _InBufOverflow; }
std::string& Client::getRaw() { return _raw_command_input; }

// Setters
void Client::setdisconnect(bool status) { _disconnect = status; }
void Client::setSocketFd(int fd) { _socket_fd = fd; }
void Client::setIpAddress(const std::string& ip) { _ip_address = ip; }
void Client::setPort(int port) { _port = port; }
void Client::setBufferOverflow(bool status) { _InBufOverflow = status; }

void Client::setHasPassedPassword(bool value) { _has_passed_password = value; }
void Client::setNickname(const std::string& nick) { _nickname = nick; }
void Client::setUsername(const std::string& user) { _username = user; }
void Client::setRegistered(bool value) { _is_registered = value; std::cout << "REGISTERED\n"; }
void Client::setOperator(bool value) { _is_operator = value; }

void Client::setReadBuffer(const std::string& buffer) { _read_buffer = buffer; }
void Client::setOutgoingBuffer(const std::string& buffer) { _outgoing_buffer = buffer; }
void Client::setCommand(const std::string& cmd) { _command_capitalized = cmd; }

//other
void Client::appendOutgoingBuffer(const std::string& data) { _outgoing_buffer += data; }
void Client::appendReadBuffer(const std::string& data) { _read_buffer += data; }

// Build the standard IRC prefix string
std::string Client::prefix(std::string& target) const {
	
	std::string result = ":";
	result += _nickname;

	if (!_username.empty()) {
		result += "!" + _username;
	}
	if (!_ip_address.empty()) {
		result += "@" + _ip_address;//== hostname
	}
	result += " ";
	result += _command_capitalized + " ";
	result += target + " " + ":";
	return result;
}