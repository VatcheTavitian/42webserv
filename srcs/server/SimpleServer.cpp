#include "serverheaders.hpp"

SimpleServer::SimpleServer() {
}

SimpleServer::SimpleServer(int domain, int service, int protocol, int port, std::string ipAddress, int bklg) {
	this->socket = new ListeningSocket(domain, service, protocol, port, ipAddress, bklg);
	int optval = 1;
	if (setsockopt(this->socket->get_sock(), SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
	{
		std::cerr << "Error: setsockopt failed" << std::endl;
		
	}
	bzero(buffer, 30000);
}

SimpleServer::~SimpleServer() {
	delete this->socket;
	std::cout << "Server destroyed!" << std::endl;
}

ListeningSocket* SimpleServer::get_socket() {
	return this->socket;
}
