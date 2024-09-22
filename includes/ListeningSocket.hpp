#ifndef ListeningSocket_hpp
#define ListeningSocket_hpp

#include "serverheaders.hpp"

class BindingSocket;

class ListeningSocket: public BindingSocket {
	protected:
		int backlog;
		ListeningSocket();

	public:
		int listening;
		ListeningSocket(int domain, int service, int protocol, int port, std::string ipAddress, int bklog);
		~ListeningSocket();
		void start_listening();
};


#endif