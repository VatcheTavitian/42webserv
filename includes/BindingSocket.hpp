#ifndef BindingSocket_hpp
#define BindingSocket_hpp

#include "serverheaders.hpp"

class BindingSocket:public SimpleSocket {

	protected:
		BindingSocket();
		virtual ~BindingSocket();

	public:
		BindingSocket(int domain, int service, int protocol, int port, std::string ipAddress);
		int connect_to_network(int sock, struct sockaddr_in address);

};

#endif