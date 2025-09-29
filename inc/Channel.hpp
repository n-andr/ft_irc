#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <iostream>
#include <set>

class Channel
{
private:
	std::string _name;
	std::string _topic;
	std::set<int> _members;  // client fds
	std::set<int> _operators;
	size_t _userLimit;
	bool _inviteOnly;

public: 
	//OCF
	Channel();
	Channel(const Channel &other);
	~Channel();
	Channel& operator=(const Channel &other);

	Channel(const std::string &name);

	//getters
	std::string& getName();
	std::string& getTopic();
	size_t getUserLImit();
	bool getInviteOnly();
	std::set<int>& getMembers();

	//setters
	void setName(std::string &name);
	void setTopic(std::string &topic);
	void setUserLimit(size_t limit);
	void setInviteOnly(bool status);


	//actual member functions
	void addMember(int fd);
	void removeMember(int fd);
	bool isMember(int fd) const;
	void addOperator(int fd);
	bool isOperator(int fd);

};

#endif