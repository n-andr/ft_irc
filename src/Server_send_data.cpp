#include "../inc/Server.hpp"

// Send a message to all connected clients (testing simple broadcast)
void Server::broadcastMessage(const std::string &msg, int senderFd) {
	for (size_t j = 1; j < _pollFds.size(); ++j) {
		int fd = _pollFds[j].fd;
		if (fd != senderFd) {
			Client *c = &(_clients[fd]);
			c->appendOutgoingBuffer(msg);
			enablePollout(*c);
		}
	}
}

void Server::sendPendingData(Client &c)
{
	std::string data = c.getNextChunk(512);
	if (data.empty())
		return ;
	size_t pos = data.find("PONG");
	if (pos == std::string::npos)
		std::cout << "sending to " << c.getNickname() << ":" << data << std::endl;
	int bytes_sent = send(c.getSocketFd(), data.c_str(), data.length(), 0);
	if (bytes_sent < 0)
	{
		if (errno == EAGAIN or errno == EWOULDBLOCK)
			return ;// Socket not ready yet, just try again later
		else {
			std::cout << RED << "Unknown error with sending data to Client with fd "
				<< c.getSocketFd() << ". Client will be disconnected.\n" << RESET;
			eraseClientFootprint(c.getSocketFd());//results in the disconnection of the client in the next poll
		}
	}
	else
	{
		c.consumeBytes(bytes_sent);
		if (c.outgoingBufferIsEmpty())
			disablePollout(c);
	}
}
