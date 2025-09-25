#include "../inc/Server.hpp"

void Server::delegateCommand(Client &c) {
	const std::string &cmd = c.getCommand();

    if (cmd == "PASS")
        std::cout << "PASS would be executed here" << std::endl;
    else if (cmd == "USER")
        std::cout << "USER would be executed here" << std::endl;
    else if (cmd == "NICK")
        std::cout << "NICK would be executed here" << std::endl;
    else
        std::cout << "Unknown command: " << cmd << std::endl;
}

/*the main poll() loop; reacts to POLLIN on listen FD and delegates client I/O*/

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
			Client &c = _clients[_pollFds[i].fd];
			//simple msg broadcaster
			if (_pollFds[i].revents & POLLIN)
			{
				char buf[512];
				int bytes_read = recv(_pollFds[i].fd, buf, sizeof(buf), 0);
				if (bytes_read <= 0) {
					disconnectClient(_pollFds[i].fd);
					continue;
				}
				else {
					c.appendReadBuffer(buf);
					if (c.extractCommand() == true)//successfully found a command and extracted it into raw command
					{
						c.parseRawCommand();
						c.printCommand();
						delegateCommand(c);
					}
					//std::cout << "[recv] from fd=" << _pollFds[i].fd << ": " << msg;
					//broadcastMessage(msg, _pollFds[i].fd);//this now sets pollout and appends out buf.
				}
			}
			if (_pollFds[i].revents & POLLOUT) {
				sendPendingData(_clients[_pollFds[i].fd]);
			}
		}
	}
	std::cout << "\nAfter Poll Loop\n"; 
}