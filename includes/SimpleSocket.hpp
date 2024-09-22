#ifndef SimpleSocket_hpp
#define SimpleSocket_hpp


#include "serverheaders.hpp"

class SimpleSocket {

	protected:
		int sock;
		struct sockaddr_in address;
		int connection;
		int port;
		SimpleSocket();

	public:
		SimpleSocket(int domain, int service, int protocol, int port, std::string ipAddress);
		virtual ~SimpleSocket();
		struct sockaddr_in get_address();
		int get_sock();
		int get_connection();
		int get_port();
		void set_connection(int con);
};

#endif