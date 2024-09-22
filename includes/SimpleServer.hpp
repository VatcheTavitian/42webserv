#ifndef SimpleServer_hpp
#define SimpleServer_hpp

#include "serverheaders.hpp"

class ListeningSocket;

class SimpleServer {
	protected:
		char buffer[30000];
		SimpleServer();

	public:
		SimpleServer(int domain, int service, int protocol, int port, std::string ipAddress, int bklg);
		virtual ~SimpleServer();
		ListeningSocket* socket;
		ListeningSocket* get_socket();
		int	new_socket;;
};

#endif