#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <iostream>
#include <set>

class Channel
{
private:
	std::string _name;

	std::set<int> _members;  // client fds
	std::set<int> _operators;
	
	//Mode related things
	size_t _userLimit;//+l if 0 then not
	
	bool _inviteOnly;//+i

	bool _topicLocked;//+t or not
	std::string _topic;
	
	std::string _key;//if "" no key

public: 
	//OCF
	Channel();
	Channel(const Channel &other);
	~Channel();
	Channel& operator=(const Channel &other);

	Channel(const std::string &name);

	//getters
	std::set<int>& getMembers();
	std::set<int>& getOperators();
	std::string& getName();
	
	std::string& getTopic();
	size_t getUserLImit();
	bool getInviteOnly();
	const std::string& getKey();
	bool getTopicLocked();

	//setters
	void setName(std::string &name);
	void setTopic(std::string &topic);
	void setUserLimit(size_t limit);
	void setInviteOnly(bool status);
	void setTopicLocked(bool status);
	void setKey(std::string& new_key);


	//actual member functions
	void addMember(int fd);
	void removeMember(int fd);
	void removeOperator(int fd);
	bool isMember(int fd) const;
	void addOperator(int fd);
	bool isOperator(int fd);

};

#endif