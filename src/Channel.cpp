#include "../inc/Channel.hpp"

// Default constructor
Channel::Channel()
	: _name("")
	, _topic("")
	, _members()
	, _operators()
	, _userLimit(0)	  // 0 = no limit
	, _inviteOnly(false) // default = open channel
{}

// Destructor
Channel::~Channel() {}

// Copy constructor
Channel::Channel(const Channel &other)
	: _name(other._name)
	, _topic(other._topic)
	, _members(other._members)
	, _operators(other._operators)
	, _userLimit(other._userLimit)
	, _inviteOnly(other._inviteOnly)
{}

// Copy assignment operator
Channel &Channel::operator=(const Channel &other) {
	if (this != &other) {
		_name	   = other._name;
		_topic	  = other._topic;
		_members	= other._members;
		_operators  = other._operators;
		_userLimit  = other._userLimit;
		_inviteOnly = other._inviteOnly;
	}
	return *this;
}

// Constructor with name
Channel::Channel(const std::string &name)
	: _name(name)
	, _topic("")
	, _members()
	, _operators()
	, _userLimit(0)
	, _inviteOnly(false)
{}

//getters
std::string& Channel::getName() { return _name; }
std::string& Channel::getTopic() { return _topic; }
size_t Channel::getUserLImit() { return _userLimit; }
bool Channel::getInviteOnly() { return _inviteOnly; }
std::set<int>& Channel::getMembers() { return _members; }

//setters
void Channel::setName(std::string &name) { _name = name; }
void Channel::setTopic(std::string &topic) { _topic = topic; }
void Channel::setUserLimit(size_t limit) { _userLimit = limit; }
void Channel::setInviteOnly(bool status) { _inviteOnly = status; }

//actual Member functions
void Channel::addMember(int fd) { _members.insert(fd); }
void Channel::removeMember(int fd) { _members.erase(fd); }
bool Channel::isMember(int fd) const { return (_members.find(fd) != _members.end()); }
void Channel::addOperator(int fd) { _operators.insert(fd); }
bool Channel::isOperator(int fd) { return (_operators.find(fd) != _operators.end()); }

Channel* Server::getChannelByName(std::string& name) {
	for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); it ++){
		if (it->first == name)
			return (&it->second);
	}
	return NULL;
}

Channel* Server::createNewChannel(std::string& name) {
	std::map<std::string, Channel>::iterator it =
		_channels.insert(std::make_pair(name, Channel(name))).first;
	return (&it->second);
}