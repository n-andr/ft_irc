#include "../inc/Server.hpp"

/* startup, signal handling, _running flag */

bool Server::_running = true;

void Server::start(){
	setupListeningSocket();
	addSocketToPoll(_serverSocket);
	std::cout << "[info] Listening on port " << _port << " ...\n";
	eventLoop();
}

void Server::signalHandler(int signum)
{
	std::cout << "signal received: " << signum << std::endl;
	if (signum == SIGINT)
		_running = false;
}