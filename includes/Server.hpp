#ifndef SERVER_HPP
# define SERVER_HPP

# include <vector>
# include <map>
# include <utility>
# include <string>
# include <cstring>
# include <cctype>
# include <cstdlib>
# include <algorithm>
# include <unistd.h>
# include "Location.hpp"
# include "SimpleServer.hpp"

struct Server 
{
    std::map<std::string, std::string> statusCodes;
    std::string _serverName;
    std::string ipAddress;
    std::string _root;
    std::string _index;
    int port;
    std::map<int, std::string> _errorPage;
    std::string allow_methods;
    long int _clientMaxBodySize;
    std::vector<Location> _location;
    std::vector<std::string> _allowedMethods;

    Location *_locationIdentified;
    SimpleServer *serverptr;
};


#endif