#include "../inc/Server.hpp"

/* send/broadcast helpers and (later) buffered writes/partial send handling */

// Send a message to all connected clients (testing simple broadcast)
void Server::broadcastMessage(const std::string &msg, int senderFd) {
	for (size_t j = 1; j < _pollFds.size(); ++j) {
		int fd = _pollFds[j].fd;
		if (fd != senderFd) { //skip sender to avoid echoing msg
			Client *c = &(_clients[fd]);
			c->appendOutgoingBuffer(msg);
			enablePollout(*c);
			//send(fd, msg.c_str(), msg.size(), 0);
		}
	}
}

void Server::sendPendingData(Client &c)
{
	// 2. Get the next chunk of data from the client’s outgoing buffer
	std::string data = c.getNextChunk(1);

	// 3. Try sending it through the socket
	int bytes_sent = send(c.getSocketFd(), data.c_str(), data.length(), 0);

	// 4. Handle possible outcomes
	if (bytes_sent < 0)
	{
			//where is errno saved?
		if (errno == EAGAIN or errno == EWOULDBLOCK)
		{
			// Socket not ready yet, just try again later
			return ;
		}
		else {
			std::cout << RED << "Unknown error with sending data to Client with fd "
				<< c.getSocketFd() << ". Client will be disconnected.\n" << RESET;
			disconnectClient(c.getSocketFd());
		}
	}
	else
	{
		// 5. Remove the sent bytes from the buffer
		c.consumeBytes(bytes_sent);

		// 6. If all pending data has been sent, 
		//	clear the POLLOUT flag for this socket
		if (c.outgoingBufferIsEmpty())
		{
			disablePollout(c);
		}
	}
}
