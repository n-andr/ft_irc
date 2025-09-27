#ifndef MACROS_HPP
#define MACROS_HPP

// ============================
// Server configuration
// ============================
#define SERVER_NAME "ircserv.by.Nandreev.Sgramsch"

// ============================
// Error numerics (RFC 2812)
// ============================
#define ERR_NOSUCHNICK         "401"   // "<nick> :No such nick/channel"
#define ERR_NOSUCHCHANNEL      "403"   // "<channel> :No such channel"
#define ERR_CANNOTSENDTOCHAN   "404"   // "<channel> :Cannot send to channel"
#define ERR_NORECIPIENT        "411"   // "<command> :No recipient given (<command>)"
#define ERR_NOTEXTTOSEND       "412"   // ":No text to send"
#define ERR_NONICKNAMEGIVEN  "431"
#define ERR_ERRONEUSNICKNAME "432"
#define ERR_NICKNAMEINUSE    "433"
#define ERR_NOTREGISTERED      "451"   // ":You have not registered"
#define ERR_NEEDMOREPARAMS   "461"
#define ERR_ALREADYREGISTRED "462"
#define ERR_PASSWDMISMATCH   "464"

// ============================
// Common error messages
// ============================
#define NEEDMOREPARAMS_MSG   "Not enough parameters"
#define ALREADYREGISTRED_MSG "User already connected. You may not reregister again."
#define PASSWDMISMATCH_MSG   "Password incorrect"
#define NONICKNAMEGIVEN_MSG  "No nickname given"
#define ERRONEUSNICKNAME_MSG "Erroneous nickname"
#define NICKNAMEINUSE_MSG    "Nickname is already in use"
#define NORECIPIENT_MSG(cmd)        (std::string(cmd) + " :No recipient given")
#define NOTEXTTOSEND_MSG            "No text to send"
#define NOSUCHNICK_MSG(nick)        (nick + " :No such nick/channel")
#define NOSUCHCHANNEL_MSG(chan)     (chan + " :No such channel")
#define CANNOTSENDTOCHAN_MSG(chan)  (chan + " :Cannot send to channel")
#define NOTREGISTERED_MSG           "You have not registered"

#endif // MACRO_HPP
