#include "../inc/Server.hpp"

enum ModeArgKind { ARG_NONE, ARG_ALWAYS, ARG_SET_ONLY, ARG_UNSET_ONLY }; //last is not used in this project

static ModeArgKind modeArgKind(char c) {
	// no-arg flags
	if (c=='i' || c=='t')
		return ARG_NONE;
	// target/list modes (both + and - take arg)
	if (c=='o')
		return ARG_ALWAYS;
	// set-only arg: +k key / -k (no arg), +l N / -l (no arg)
	if (c=='k' || c=='l')
		return ARG_SET_ONLY;

	// unknown — signal error
	return (ModeArgKind)(-1);
}

static bool needsArg(ModeArgKind kind, bool set) {
	if (kind == ARG_ALWAYS)	 return true;
	if (kind == ARG_SET_ONLY)   return set;	   // only on '+'
	if (kind == ARG_UNSET_ONLY) return !set;	  // only on '-'
	return false; // ARG_NONE
}

ModeParseResult Server::splitModeParams(Client &cl)
{
	ModeParseResult out;
	out.error.clear();
	out.order.clear();
	out.leftoverArgs.clear();

	const std::vector<std::string>& params = cl.getParams();
	const std::string &trailing = cl.getTrailing();
	const std::string& modeStr = params[1];

	// Collect the argument queue: everything after modestring + optional trailing
	std::vector<std::string> args;
	for (size_t i = 2; i < params.size(); ++i) args.push_back(params[i]);
	if (!trailing.empty()) args.push_back(trailing);

	int sign = 0; // +1 after '+', -1 after '-', 0 until first sign appears (case for +++++t)
	for (size_t i = 0; i < modeStr.size(); ++i) {
		char c = std::tolower(modeStr[i]);

		if (c == '+' || c == '-') { sign = (c == '+') ? +1 : -1; continue; }
		if (sign == 0) { 
			out.error = "MODE: missing sign before mode letters";
			out.order.clear(); 
			sendError(cl, ERR_NEEDMOREPARAMS, MSG_NEEDMOREPARAMS("MODE"));
			return out; }

		ModeArgKind kind = modeArgKind(c);
		if ((int)kind == -1) {
			out.error = std::string("MODE: unknown mode '") + c + "'";
			out.order.clear();
			sendError(cl, ERR_UNKNOWNMODE, MSG_UNKNOWNMODE(std::string(1, c)));

			return out;
		}

		const bool set = (sign > 0);
		const bool takeArg = needsArg(kind, set);

		ModeChange mc;
		mc.set = set;
		mc.mode   = c;
		mc.hasArg = takeArg;

		if (takeArg) {
			if (args.empty()) {
				out.error = std::string("MODE: need more parameters for '") + (set?'+':'-') + c + "'";
				out.order.clear();
				sendError(cl, ERR_NEEDMOREPARAMS, MSG_NEEDMOREPARAMS("MODE"));

				return out;
			}
			mc.arg = args.front();
			args.erase(args.begin());
		}

		out.order.push_back(mc);
	}

	// Whatever remains are leftovers (we can through an error or just ignore them)
	out.leftoverArgs = args;
	if (!out.leftoverArgs.empty())	sendError(cl, 696, CUSTOM_MODE_TOO_MANY_ARGS);
	return out;
}

void Server::mode(Client &c) {
	if (!c.isRegistered()) {
		sendError(c, ERR_NOTREGISTERED, MSG_NOTREGISTERED);
		return ;
	}
	std::vector<std::string> p = c.getParams();
	if (p.empty() && c.getTrailing().empty()) {
		sendError(c, ERR_NEEDMOREPARAMS, MSG_NEEDMOREPARAMS("MODE"));
		return ;
	}
	if (p.size() == 1 && p[0][0] == '#' ) {
		printChannelModes(c, p[0]);
		return;
	}

	//"MODE: need <channel> and <modestring>";
	if (p.size() < 2) {
		sendError(c, ERR_NEEDMOREPARAMS, MSG_NEEDMOREPARAMS("MODE"));
		return;
	}

	//if first arg is not a channel name
	if (p[0][0] != '#'){
		sendError(c, ERR_NEEDMOREPARAMS, MSG_NEEDMOREPARAMS("MODE"));
		return;
	}
	//parsing and organizing into struct
	std::string  channelName = p[0];
	ModeParseResult modeOrganized = splitModeParams(c);

	//if there was an error don't execute
	if(!modeOrganized.error.empty() || !modeOrganized.leftoverArgs.empty()){
		return;
	}

	//execution:
	execute_mode(c, channelName, modeOrganized);
}



