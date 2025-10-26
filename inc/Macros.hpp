#ifndef MACROS_HPP
#define MACROS_HPP

// ============================
// Server configuration
// ============================
#define SERVER_NAME "ircserv.by.Nandreev.Sgramsch"

// ============================
// Error numerics (RFC 2812)
// ============================

/*README
We can delete all unsued == still comented out ERR codes at the end

! behind message -> this message is not yet exctly as in the RFC 1459
*/

#define CUSTOM_INVITED(channel) ("You have been invited to " + std::string(channel))
#define CUSTOM_JOIN(nick, channel) (std::string(nick) + " has joined " + std::string(channel))
#define CUSTOM_PASS_CORRECT "Password Corrrect"
#define CUSTOM_NICK_SET(nick) ("Your Nickname was set to " + std::string(nick))
#define CUSTOM_USER_SET(user) ("Your Username was set to " + std::string(user))
#define CUSTOM_REGISTRATION_SUCCESS "Registration complete. You may use the Server for messaging now."
#define CUSTOM_TOPIC_CHANGED(nick, channel, topic) (std::string(nick) \
	+ " has changed " + std::string(channel) + "'s topic to: " + std::string(topic))
#define CUSTOM_YOU_GOT_KICKED(nick, channel) (std::string(nick) \
	+ " has kicked you from " + std::string(channel))
#define CUSTOM_SOMEONE_WAS_KICKED(kicker, kicked, channel) (std::string(kicker) \
	+ " kicked " + std::string(kicked) + " from " + std::string(channel))
#define CUSTOM_MODE_TOO_MANY_ARGS "MODE: Too many arguments"
#define CUSTOM_MODE_CHANGE(condition, trueMsg, falseMsg) \
	((condition) ? std::string (trueMsg) : std::string (falseMsg))


// #define RPL_NONE                300
// #define MSG_NONE()              ":Dummy reply number. Not used."

// #define RPL_AWAY                301
// #define MSG_AWAY(nick, message) (std::string(nick) + " :" + std::string(message))

// #define RPL_USERHOST            302
// #define MSG_USERHOST(reply)     (":" + std::string(reply))

// #define RPL_ISON                303
// #define MSG_ISON(nicks)         (":" + std::string(nicks))

// #define RPL_UNAWAY              305
// #define MSG_UNAWAY()            ":You are no longer marked as being away"

// #define RPL_NOWAWAY             306
// #define MSG_NOWAWAY()          ":You have been marked as being away"



// #define RPL_WHOISUSER           311
// #define MSG_WHOISUSER(nick, user, host, real) /*\*/
//     (std::string(nick) + " " + std::string(user) + " " + std::string(host) + " * :" + std::string(real))

// #define RPL_WHOISSERVER         312
// #define MSG_WHOISSERVER(nick, server, info) /*\*/
//     (std::string(nick) + " " + std::string(server) + " :" + std::string(info))

// #define RPL_WHOISOPERATOR       313
// #define MSG_WHOISOPERATOR(nick) (std::string(nick) + " :is an IRC operator")

// #define RPL_WHOWASUSER          314
// #define MSG_WHOWASUSER(nick, user, host, real) /*\*/
//     (std::string(nick) + " " + std::string(user) + " " + std::string(host) + " * :" + std::string(real))

// #define RPL_ENDOFWHO            315
// #define MSG_ENDOFWHO(name)      (std::string(name) + " :End of /WHO list")

// #define RPL_WHOISIDLE           317
// #define MSG_WHOISIDLE(nick, seconds) /*\*/
//     (std::string(nick) + " " + std::string(seconds) + " :seconds idle")

// #define RPL_ENDOFWHOIS          318
// #define MSG_ENDOFWHOIS(nick)    (std::string(nick) + " :End of /WHOIS list")

// #define RPL_WHOISCHANNELS       319
// #define MSG_WHOISCHANNELS(nick, channels) /*\*/
//     (std::string(nick) + " :" + std::string(channels))

// #define RPL_LISTSTART           321
// #define MSG_LISTSTART()         "Channel :Users  Name"

// #define RPL_LIST                322
// #define MSG_LIST(channel, users, topic) /*\*/
//     (std::string(channel) + " " + std::string(users) + " :" + std::string(topic))

// #define RPL_LISTEND             323
// #define MSG_LISTEND()           ":End of /LIST"

// #define RPL_CHANNELMODEIS       324
// #define MSG_CHANNELMODEIS(channel, mode, params) /*\*/
//     (std::string(channel) + " " + std::string(mode) + " " + std::string(params))

#define RPL_NOTOPIC             331
#define MSG_NOTOPIC(channel)    (std::string(channel) + " :No topic is set")

#define RPL_TOPIC               332
#define MSG_TOPIC(channel, topic) \
	(std::string(channel) + " :" + std::string(topic))

#define RPL_INVITING            341
#define MSG_INVITING(channel, nick)	(std::string(channel) + " " + std::string(nick))

// #define RPL_SUMMONING           342
// #define MSG_SUMMONING(user)     (std::string(user) + " :Summoning user to IRC")

// #define RPL_VERSION             351
// #define MSG_VERSION(version, debug, server, comments) /*\*/
//     (std::string(version) + "." + std::string(debug) + " " + std::string(server) + " :" + std::string(comments))

// #define RPL_WHOREPLY            352
// #define MSG_WHOREPLY(channel, user, host, server, nick, flags, hopcount, real) /*\*/
//     (std::string(channel) + " " + std::string(user) + " " + std::string(host) + " " + /*\*/
//      std::string(server) + " " + std::string(nick) + " " + std::string(flags) + /*\*/
//      " :" + std::string(hopcount) + " " + std::string(real))

// #define RPL_NAMREPLY            353
// #define MSG_NAMREPLY(channel, nicks) /*\*/
//     (std::string(channel) + " :" + std::string(nicks))

// #define RPL_LINKS               364
// #define MSG_LINKS(mask, server, hopcount, info) /*\*/
//     (std::string(mask) + " " + std::string(server) + " :" + std::string(hopcount) + " " + std::string(info))

// #define RPL_ENDOFLINKS          365
// #define MSG_ENDOFLINKS(mask)    (std::string(mask) + " :End of /LINKS list")

// #define RPL_ENDOFNAMES          366
// #define MSG_ENDOFNAMES(channel) (std::string(channel) + " :End of /NAMES list")

// #define RPL_BANLIST             367
// #define MSG_BANLIST(channel, banid) /*\*/
//     (std::string(channel) + " " + std::string(banid))

// #define RPL_ENDOFBANLIST        368
// #define MSG_ENDOFBANLIST(channel) /*\*/
//     (std::string(channel) + " :End of channel ban list")

// #define RPL_ENDOFWHOWAS         369
// #define MSG_ENDOFWHOWAS(nick)   (std::string(nick) + " :End of WHOWAS")

// #define RPL_INFO                371
// #define MSG_INFO(string)        (":" + std::string(string))

// #define RPL_ENDOFINFO           374
// #define MSG_ENDOFINFO()        ":End of /INFO list"

// #define RPL_MOTDSTART           375
// #define MSG_MOTDSTART(server)   (":- " + std::string(server) + " Message of the day - ")

// #define RPL_MOTD                372
// #define MSG_MOTD(text)          (":- " + std::string(text))

// #define RPL_ENDOFMOTD           376
// #define MSG_ENDOFMOTD()         ":End of /MOTD command"

// #define RPL_YOUREOPER           381
// #define MSG_YOUREOPER()         ":You are now an IRC operator"

// #define RPL_REHASHING           382
// #define MSG_REHASHING(config)   (std::string(config) + " :Rehashing")

// #define RPL_TIME                391
// #define MSG_TIME(server, time)  (std::string(server) + " :" + std::string(time))

// #define RPL_USERSSTART          392
// #define MSG_USERSSTART()       ":UserID   Terminal  Host"

// #define RPL_USERS               393
// #define MSG_USERS(user, term, host) /*\*/
//     (":" + std::string(user) + " " + std::string(term) + " " + std::string(host))

// #define RPL_ENDOFUSERS          394
// #define MSG_ENDOFUSERS()		":End of users"

// #define RPL_NOUSERS             395
// #define MSG_NOUSERS()           ":Nobody logged in"

////////////////////////////////////////////////////////////////

#define ERR_NOSUCHNICK        401
#define MSG_NOSUCHNICK(nick)    (std::string(nick) + " :No such nick/channel")

// #define ERR_NOSUCHSERVER      402
// #define ERR_NOSUCHSERVER(servername)  (std::string(servername) + "No such server")

#define ERR_NOSUCHCHANNEL     403
#define MSG_NOSUCHCHANNEL(channelname) (std::string(channelname) + " :No such channel")

#define ERR_CANNOTSENDTOCHAN  404
#define MSG_CANNOTSENDTOCHAN(channelname) (std::string(channelname) + " :Cannot send to channel")

// #define ERR_TOOMANYCHANNELS   405
// #define MSG_TOOMANYCHANNELS(channelName) (std::string(channelName) + " :You have joined too many channels"

// #define ERR_WASNOSUCHNICK     406
// #define MSG_WASNOSUCHNICK(nickname) (std::string(nickname) + " :There was no such nickname")

// #define ERR_TOOMANYTARGETS    407
// #define MSG_TOOMANYTARGETS(target) (std::string(target) + " :Duplicate recipients. No message delivered")

// #define ERR_NOORIGIN          409
// #define MSG_NOORIGIN      "No origin specified"

#define ERR_NORECIPIENT       411
#define MSG_NORECIPIENT(command)   ("No recipient given (" + std::string(command) + ")")

#define ERR_NOTEXTTOSEND      412
#define MSG_NOTEXTTOSEND  "No text to send"

// #define ERR_NOTOPLEVEL        413
// #define MSG_NOTOPLEVEL    "No toplevel domain specified"!

// #define ERR_WILDTOPLEVEL      414
// #define MSG_WILDTOPLEVEL  "Wildcard in toplevel domain"!

#define ERR_UNKNOWNCOMMAND    421
#define MSG_UNKNOWNCOMMAND(cmd) (std::string(cmd) + " :Unknown command")

// #define ERR_NOMOTD            422
// #define MSG_NOMOTD        "MOTD File is missing"

// #define ERR_NOADMININFO       423
// #define MSG_NOADMININFO   "No administrative info available"!

// #define ERR_FILEERROR         424
// #define MSG_FILEERROR     "File error doing something"!

#define ERR_NONICKNAMEGIVEN   431
#define MSG_NONICKNAMEGIVEN "No nickname given"

#define ERR_ERRONEUSNICKNAME  432
#define MSG_ERRONEUSNICKNAME(nick) (std::string(nick) + " :Erroneous nickname")

#define ERR_NICKNAMEINUSE     433
#define MSG_NICKNAMEINUSE(nick) (std::string(nick) + " :Nickname is already in use")

// #define ERR_NICKCOLLISION     436
// #define MSG_NICKCOLLISION "Nickname collision KILL"!

// #define ERR_UNAVAILRESOURCE   437
// #define MSG_UNAVAILRESOURCE "Nick/channel is temporarily unavailable"!

#define ERR_USERNOTINCHANNEL  441
#define MSG_USERNOTINCHANNEL(nick, channel) (std::string(nick) + " " + std::string(channel) + " :They aren't on that channel")

#define ERR_NOTONCHANNEL      442
#define MSG_NOTONCHANNEL(channel)  (std::string(channel) + " :You're not on that channel")

#define ERR_USERONCHANNEL     443
#define MSG_USERONCHANNEL(user, channel) (std::string(user) + " " + std::string(channel) + " :User is already on channel")

// #define ERR_NOLOGIN           444
// #define MSG_NOLOGIN       "User not logged in"!

// #define ERR_SUMMONDISABLED    445
// #define MSG_SUMMONDISABLED ":SUMMON has been disabled"

// #define ERR_USERSDISABLED     446
// #define MSG_USERSDISABLED ":USERS has been disabled"

#define ERR_NOTREGISTERED     451
#define MSG_NOTREGISTERED ":You have not registered"

#define ERR_NEEDMOREPARAMS    461
#define MSG_NEEDMOREPARAMS(cmd) (std::string(cmd) + " :Not enough parameters")

#define ERR_ALREADYREGISTRED  462
#define MSG_ALREADYREGISTRED ":Unauthorized command. You may not register (already registered)"//original: "You may not register"

// #define ERR_NOPERMFORHOST     463
// #define MSG_NOPERMFORHOST "Your host isn't among the privileged"!

#define ERR_PASSWDMISMATCH    464
#define MSG_PASSWDMISMATCH ":Password incorrect"

// #define ERR_YOUREBANNEDCREEP  465
// #define MSG_YOUREBANNEDCREEP ":You are banned from this server"

// #define ERR_KEYSET            467
// #define MSG_KEYSET(channel)	(std::string(channel) + " :Channel key already set")

// #define ERR_CHANNELISFULL     471
// #define MSG_CHANNELISFULL(channel) (std::string(channel) + " :Cannot join channel (+l)")

#define ERR_UNKNOWNMODE       472
#define MSG_UNKNOWNMODE(character) (std::string(character) + " :is unknown mode char to me")

#define ERR_INVITEONLYCHAN    473
#define MSG_INVITEONLYCHAN(channel) (std::string(channel) + " :Cannot join channel (+i)")

// #define ERR_BANNEDFROMCHAN    474
// #define MSG_BANNEDFROMCHAN(channel) (std::string(channel) + " :Cannot join channel (+b)")

// #define ERR_BADCHANNELKEY     475
// #define MSG_BADCHANNELKEY(channel) (std::string(channel) + " :Cannot join channel (+k)")

// #define ERR_NOCHANMODES       477
// #define MSG_NOCHANMODES   "Channel doesn't support modes"!

// #define ERR_BANLISTFULL       478
// #define MSG_BANLISTFULL   "Channel ban/ignore list is full"!

// #define ERR_NOPRIVILEGES      481
// #define MSG_NOPRIVILEGES  ":Permission Denied- You're not an IRC operator"

#define ERR_CHANOPRIVSNEEDED  482
#define MSG_CHANOPRIVSNEEDED(channel) (std::string(channel) + " :You're not channel operator")

// #define ERR_CANTKILLSERVER    483
// #define MSG_CANTKILLSERVER "You can't kill a server!"

// #define ERR_RESTRICTED        484!
// #define MSG_RESTRICTED    "Your connection is restricted"

// #define ERR_UNIQOPPRIVSNEEDED 485!
// #define MSG_UNIQOPPRIVSNEEDED "You're not the original channel operator"

// #define ERR_NOOPERHOST        491
// #define MSG_NOOPERHOST    "No O-lines for your host"

// #define ERR_UMODEUNKNOWNFLAG  501
// #define MSG_UMODEUNKNOWNFLAG "Unknown MODE flag"

// #define ERR_USERSDONTMATCH    502
// #define MSG_USERSDONTMATCH "Cannot change mode for other users"

#endif // MACRO_HPP
