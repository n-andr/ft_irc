#include "../inc/Client.hpp"

std::string& Client::getReadBuffer() {return (_read_buffer);}

void Server::eventLoop()
{
	std::cout << "Event Loop" << std::endl;//control print
	while (true)
	{
		poll(&_pollFds[0], _nFds, 100);
		if (_pollFds[L_SOCKET].revents & POLLIN)
			this->handleNewConnection();
		for (int i = 1; i < _nFds; i++)
		{
			int bytes_read = 0;
			if (_pollFds[i].revents & POLLIN)
			{
				Client *c = &_clients[_pollFds[i].fd];
				bytes_read = recv(_pollFds[i].fd, &(c->getReadBuffer()), sizeof(c->getReadBuffer()), 0);//getters missing
				if (bytes_read <= 0)
					std::cout << "empty read\n";
					//empty_read();
				else
					std::cout << "non empty read\n";
					//non_empty_read();
			}
			if (_pollFds[i].revents & POLLOUT)
				std::cout << "send data\n";
				//send_data();
		}
		break ;//since we aren't actually listening for anything yet
	}
}