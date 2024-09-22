#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <map>
# include <vector>
# include <string>
# include <cstring>
# include <cstdlib>
# include <algorithm>
# include <unistd.h>
# include <iostream>

struct Location 
{
    typedef std::string t_str;

    std::string _redirect;
    std::string _location;
    std::string _root;
    std::map<int, t_str> _errorPage;
    long int _clientMaxBodySize;
    std::string _allow_methods;
    std::vector<t_str> _allowedMethods; 
    std::map<std::string, std::string> _cgi;
    std::string _uploadDir;
    std::string permitUploadStr;
    //vatche var

    std::string _locationIndex;
    bool _permitDelete;
    bool _permitUpload;
    bool _listDirectory;
};

#endif

