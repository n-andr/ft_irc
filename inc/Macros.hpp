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

// #define ERR_USERNOTINCHANNEL  441
// #define MSG_USERNOTINCHANNEL(nick, channel) (std::string(nick) + " " + std::string(channel) + " :They aren't on that channel")

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

// #define ERR_UNKNOWNMODE       472
// #define MSG_UNKNOWNMODE(character) (std::string(character) + " :is unknown mode char to me")

#define ERR_INVITEONLYCHAN    473
#define MSG_INVITEONLYCHAN(channel) (std::string(channel) + " :Cannot join channel (+i)")

// #define ERR_BANNEDFROMCHAN    474
// #define MSG_BANNEDFROMCHAN(channel) (std::string(channel) + " :Cannot join channel (+b)")

// #define ERR_BADCHANNELKEY     475
// #define MSG_BADCHANNELKEY(channel) (std::string(channel) + " :Cannot join channel (+k)")

// #define ERR_BADCHANMASK       476
// #define MSG_BADCHANMASK   "Bad Channel Mask"!

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
