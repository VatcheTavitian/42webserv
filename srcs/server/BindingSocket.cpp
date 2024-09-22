#include "serverheaders.hpp"

BindingSocket::BindingSocket() {
}

BindingSocket::BindingSocket(int domain, int service, int protocol, int port, std::string ipAddress) : SimpleSocket(domain, service, protocol, port, ipAddress) {
	set_connection(connect_to_network(get_sock(), get_address()));
}

int BindingSocket::connect_to_network(int sock, struct sockaddr_in address) {
	int bindFD = bind(sock, (struct sockaddr *) &address, sizeof(address));
	if (bindFD == -1) {
		std::stringstream portStr;
    	portStr << get_port();
		throw std::runtime_error("Fatal error: Binding failed on port " + portStr.str());
	}
	return bindFD;
}

BindingSocket::~BindingSocket() {
}
