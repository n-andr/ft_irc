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
    while (iss >> word) {        // operator>> skips whitespace automatically
        out.push_back(word);
    }
}

static std::string to_upper(const std::string &s) {
    std::string out(s);
    for (size_t i = 0; i < out.size(); ++i)
        out[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(out[i])));
    return out;
}

void Client::parseRawCommand(std::string &line){
	_raw_command_input = removeCRLF(line);
    _command_capitalized.clear();
    _params.clear();
    _trailing.clear();	

	// space followed by colon starts trailing
	// spit into <everything before> " :" <trailing>

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

	//spit into command and params
	std::vector<std::string> tokens;
	split_spaces(before_trailing, tokens);
	if (tokens.empty()) return;

	_command_capitalized = to_upper(tokens[0]);
	for (size_t i = 1; i < tokens.size(); i++)
		_params.push_back(tokens[i]);

}