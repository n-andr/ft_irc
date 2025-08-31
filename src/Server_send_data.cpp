#include "../inc/Server.hpp"

// Send a message to all connected clients (testing simple broadcast)
void Server::broadcastMessage(const std::string &msg, int senderFd) {
    for (size_t j = 1; j < _pollFds.size(); ++j) {
        int fd = _pollFds[j].fd;
        if (fd != senderFd) { //skip sender to avoid echoing msg
            send(fd, msg.c_str(), msg.size(), 0);
        }
    }
}