#include "../inc/Channel.hpp"

// Default constructor
Channel::Channel()
	: _name("")
	, _members()
	, _operators()
	, _userLimit(0)	  // 0 = no limit
	, _inviteOnly(false) // default = open channel
	, _topicLocked(false)
	, _topic("")
	, _key("")
{}

// Destructor
Channel::~Channel() {}

// Copy constructor
Channel::Channel(const Channel &other)
	: _name(other._name)
	, _members(other._members)
	, _operators(other._operators)
	, _userLimit(other._userLimit)
	, _inviteOnly(other._inviteOnly)
	, _topicLocked(false)
	, _topic(other._topic)
	, _key("")
	{}

// Copy assignment operator
Channel &Channel::operator=(const Channel &other) {
	if (this != &other) {
		_name	   = other._name;
		_members	= other._members;
		_operators  = other._operators;
		_userLimit  = other._userLimit;
		_inviteOnly = other._inviteOnly;
		_topicLocked = other._topicLocked;
		_topic	  = other._topic;
		_key = other._key;
	}
	return *this;
}

// Constructor with name
Channel::Channel(const std::string &name)
	: _name(name)
	, _members()
	, _operators()
	, _userLimit(0)	  // 0 = no limit
	, _inviteOnly(false) // default = open channel
	, _topicLocked(false)
	, _topic("")
	, _key("")
{}

//getters
std::string& Channel::getName() { return _name; }
std::string& Channel::getTopic() { return _topic; }
size_t Channel::getUserLImit() { return _userLimit; }
bool Channel::getInviteOnly() { return _inviteOnly; }
std::set<int>& Channel::getMembers() { return _members; }
const std::string& Channel::getKey() { return _key; }
bool Channel::getTopicLocked() { return _topicLocked; }

//setters
void Channel::setName(std::string &name) { _name = name; }
void Channel::setTopic(std::string &topic) { _topic = topic; }
void Channel::setUserLimit(size_t limit) { _userLimit = limit; }
void Channel::setInviteOnly(bool status) { _inviteOnly = status; }
void Channel::setTopicLocked(bool status) { _topicLocked = status; }
void Channel::setKey(std::string& new_key) { _key = new_key; }

//actual Member functions
void Channel::addMember(int fd) { _members.insert(fd); }
void Channel::removeMember(int fd) { _members.erase(fd); }
void Channel::removeOperator(int fd) { _operators.erase(fd); }
bool Channel::isMember(int fd) const { return (_members.find(fd) != _members.end()); }
void Channel::addOperator(int fd) { _operators.insert(fd); }
bool Channel::isOperator(int fd) { return (_operators.find(fd) != _operators.end()); }