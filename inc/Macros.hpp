#ifndef MACROS_HPP
#define MACROS_HPP

// ============================
// Server configuration
// ============================
#define SERVER_NAME "ircserv.by.Nandreev.Sgramsch"

// ============================
// Error numerics (RFC 2812)
// ============================
#define ERR_NEEDMOREPARAMS   "461"
#define ERR_ALREADYREGISTRED "462"
#define ERR_PASSWDMISMATCH   "464"
#define ERR_NONICKNAMEGIVEN  "431"
#define ERR_ERRONEUSNICKNAME "432"
#define ERR_NICKNAMEINUSE    "433"

// ============================
// Common error messages
// ============================
#define NEEDMOREPARAMS_MSG   "Not enough parameters"
#define ALREADYREGISTRED_MSG "User already connected. You may not reregister again."
#define PASSWDMISMATCH_MSG   "Password incorrect"
#define NONICKNAMEGIVEN_MSG  "No nickname given"
#define ERRONEUSNICKNAME_MSG "Erroneous nickname"
#define NICKNAMEINUSE_MSG    "Nickname is already in use"

#endif // MACRO_HPP
