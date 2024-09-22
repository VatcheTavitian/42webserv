#include "serverheaders.hpp"

SimpleSocket::SimpleSocket() {
}

SimpleSocket::SimpleSocket(int domain, int service, int protocol, int port, std::string ipAddress)
{
	address.sin_family = domain;
	address.sin_port = htons(port);

	inet_pton(AF_INET, ipAddress.c_str(), &(address.sin_addr)); 

	this->sock = socket(domain, service, protocol);
	this->port = port;

};

sockaddr_in SimpleSocket::get_address() {
	return this->address;
}

int SimpleSocket::get_sock() {
	return this->sock;
}

int SimpleSocket::get_port() {
	return this->port;
}

int SimpleSocket::get_connection() {
	return this->connection;
}

void SimpleSocket::set_connection(int con) {
	this->connection = con;
}

SimpleSocket::~SimpleSocket() {
	close(this->sock);
}