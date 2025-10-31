#include "../inc/Server.hpp"

bool Server::_running = true;

void Server::start(){
	setupListeningSocket();
	addSocketToPoll(_serverSocket);
	eventLoop();
}

void Server::signalHandler(int signum)
{
	if (signum == SIGINT)
		_running = false;
}