#include "../inc/Client.hpp"

static std::string removeCRLF(std::string line){
	while (!line.empty() && (line[line.size() - 1] == '\r' || line[line.size() - 1] == '\n'))
		line.erase(line.size() - 1);
	return line;
}

/*This helper takes a string, wraps it in an istringstream, and uses the >> operator to automatically split it into whitespace-separated words. 
Each word is pushed into the vector, so you get clean tokens without having to manually scan characters.*/

static void split_spaces(const std::string &s, std::vector<std::string> &out) {
	out.clear();
	std::istringstream iss(s);
	std::string word;
	while (iss >> word) {// operator>> skips whitespace automatically
		out.push_back(word);
	}
}

static std::string to_upper(const std::string &s) {
	std::string out(s);
	for (size_t i = 0; i < out.size(); ++i)
		out[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(out[i])));
	return out;
}

//returns true if a full command was found in read buffer and got successfully moved into raw command.
bool Client::extractCommand() {
	size_t pos = _read_buffer.find("\r\n");
	if (pos == std::string::npos)
		return (false);
	if (pos + 2 > 512)
	{
		setBufferOverflow(true);
		return (false);
	}
	_raw_command_input = removeCRLF(_read_buffer.substr(0, pos));
	consumeBytesReadBuffer(pos + 2);
	return (true);
}

//parse raw command into CMD, params and trailing
void Client::parseRawCommand(){
	_command_capitalized.clear();
	_params.clear();
	_trailing.clear();	

	std::string before_trailing = _raw_command_input;
	size_t position = std::string::npos;

	for (size_t i = 0; i + 1 < before_trailing.size(); i++){
		if (before_trailing[i] == ' ' && before_trailing[i+1] == ':')
		{
			position = i;
			break;
		}
	}
	if (position != std::string::npos){
		before_trailing = _raw_command_input.substr(0, position);
		_trailing = _raw_command_input.substr(position + 2);
	}

	std::vector<std::string> tokens;
	split_spaces(before_trailing, tokens);
	if (tokens.empty()) return;

	_command_capitalized = to_upper(tokens[0]);
	for (size_t i = 1; i < tokens.size(); i++)
		_params.push_back(tokens[i]);
}