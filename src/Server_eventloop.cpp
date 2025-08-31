#include "../inc/Server.hpp"

void Server::eventLoop()
{
	std::cout << "Event Loop starts:" << std::endl;//control print
	while (_running)//server is running
	{
		poll(&_pollFds[0], _nFds, 100);
		if (_pollFds[L_SOCKET].revents & POLLIN)
			this->handleNewConnection();
		for (int i = 1; i < _nFds; i++)
		{
			if (_pollFds[i].revents & POLLIN)
			{
				char buf[512];
				int bytes_read = recv(_pollFds[i].fd, buf, sizeof(buf), 0);

				if (bytes_read <= 0) {
					disconnectClient(i);
					continue;
				} else {
					std::string msg(buf, bytes_read);
					std::cout << "[recv] from fd=" << _pollFds[i].fd << ": " << msg;
					broadcastMessage(msg, _pollFds[i].fd);
				}
			}

			// ingoring client for now

			// int bytes_read = 0;
			// if (_pollFds[i].revents & POLLIN)
			// {
			// 	Client *c = &_clients[_pollFds[i].fd];
			// 	bytes_read = recv(_pollFds[i].fd, &(c->getReadBuffer()), sizeof(c->getReadBuffer()), 0);
			// 	if (bytes_read <= 0) {
			// 		// client disconnected or error
			// 		disconnectClient(i);
			// 		continue;
			// 	} else {
			// 		// got some data
			// 		std::string msg(c->getReadBuffer(), bytes_read);
			// 		std::cout << "[recv] from fd=" << _pollFds[i].fd << ": " << msg;
			// 		broadcastMessage(msg, _pollFds[i].fd);
			// 	}
			// }
			// if (_pollFds[i].revents & POLLOUT)
			// 	std::cout << "send data\n";
			// 	//send_data();
		}
	}
	std::cout << "\nAfter Poll Loop\n"; 
}