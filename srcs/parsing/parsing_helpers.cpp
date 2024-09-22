#include "Parsing.hpp"

std::string to_string(int value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

void splitIpAddressAndPort(const std::string &str, std::string &ipAddress, int &port)
{
    size_t pos = str.find(':');
    if (pos != std::string::npos) 
	{
        ipAddress = str.substr(0, pos);
        std::string portStr = str.substr(pos + 1);
        std::istringstream portStream(portStr);
        portStream >> port;
    } 
	else 
	{
        ipAddress = str;
        port = 0;
    }
}

std::string cleanStringCopy(const std::string &str)
{
    std::string cleaned = str;
    size_t pos = cleaned.find(';');
    if (pos != std::string::npos) 
        cleaned = cleaned.substr(0, pos);
    return cleaned;
}

void cleanString(std::string &str)
{
    size_t pos = str.find(';');
    if (pos != std::string::npos) 
        str = str.substr(0, pos);
}

std::string trimStd(std::string str)
{
    std::string temp;
    for(size_t i = 0; i < str.size(); ++i)
    {
        if (str[i] != '\t' && str[i] != ' ')
            temp += str[i];
    }
    return temp;
}

std::string trim(const std::string& str)
{
    size_t start = str.find_first_not_of(" \t");
    size_t end = str.find_last_not_of(" \t");
    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

std::string getType(const std::string& fileName) 
{
    size_t pos = fileName.find_last_of(".");
    std::string ext = fileName.substr(pos + 1); 
    if (pos == std::string::npos)
        return "";
    else
        return ext;
}

void copyFileIgnoringComments(const std::string &sourcePath, const std::string &tempPath) 
{
    std::ifstream sourceFile(sourcePath.c_str());
    if (!sourceFile.is_open()) 
    {
        std::cerr << "Error: Unable to open source file " << sourcePath << std::endl;
        throw std::runtime_error("Error: Unable to open source file - check file exists");
    }

    if (sourceFile.peek() == std::ifstream::traits_type::eof()) 
    {
        std::cerr << "Error: Check if config file is valid" << sourcePath << std::endl;
        throw std::runtime_error("Error: Invalid config file - check file is not empty");
    }
    std::ofstream tempFile(tempPath.c_str());
    if (!tempFile.is_open()) 
    {
        std::cerr << "Error: Unable to open temporary file " << tempPath << std::endl;
        throw std::runtime_error("Error: Unable to open temporary file - fatal error");
    }

    std::string line;
    while (getline(sourceFile, line)) 
    {
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) 
        {
            tempFile << line.substr(0, commentPos) << std::endl;
        }
        else 
        {
            tempFile << line << std::endl;
        }
    }
    sourceFile.close();
    tempFile.close();
}

void allocateServerPointers(std::vector<Server>& servers) 
{
    std::vector<int>mappedPorts;
    try 
    {
       for (size_t i = 0; i < servers.size(); i++) {
            if (std::find(mappedPorts.begin(), mappedPorts.end(), servers[i].port) != mappedPorts.end()) {
                for (size_t j = 0; j < servers.size(); j++)
                {
                    if (servers[j].port == servers[i].port)
                    {
                        if (servers[j].serverptr != NULL) {
                            servers[i].serverptr = servers[j].serverptr;
                        }   
                    }
                }
            }
            else 
            {
                servers[i].serverptr = new SimpleServer(AF_INET, SOCK_STREAM, 0, servers[i].port, servers[i].ipAddress , 5);
                mappedPorts.push_back(servers[i].port);
            }
       }
       for (size_t k = 0; k < servers.size(); k++)
        {
            if (servers[k].serverptr == NULL) {
                std::cerr << "Error: Unable to allocate server pointer" << std::endl;
                throw std::runtime_error("Error: Invalid server configuration - check server block");
            }
        }
        
    } 
    catch (const std::exception &e) 
    {
        std::cerr << e.what() << std::endl;
        throw;
    }

}

void checkValidPathsLeading(std::vector<Server> servers) {
    for (size_t i = 0; i < servers.size(); i++) {
        if ((!servers[i]._root.empty() && servers[i]._root[0] != '/') || (!servers[i]._index.empty() && servers[i]._index[0] != '/'))
             throw std::runtime_error("Error: Missing leading '/' in configuration file - check server config");
        if (servers[i]._errorPage.size() > 0) {
                for (std::map<int, std::string>::const_iterator it = servers[i]._errorPage.begin();
                    it != servers[i]._errorPage.end(); it++) {
                        if (!it->second.empty() && (it->second[0] != '/'))
                            throw std::runtime_error("Error: Missing leading '/' in configuration file - check server error_pages");
                    }
                }
        if (servers[i]._location.size() > 0) {
            for (size_t j = 0; j < servers[i]._location.size(); j++) {
                if (servers[i]._location[j]._location.length() == 1 && servers[i]._location[j]._location[0] != '/')
                    throw std::runtime_error("Error: Invalid location format in config file at [" + servers[i]._location[j]._location +"]");
                if ((!servers[i]._location[j]._root.empty() && servers[i]._location[j]._root[0] != '/') || 
                    (!servers[i]._location[j]._locationIndex.empty() && servers[i]._location[j]._locationIndex[0] != '/') ||
                    (!servers[i]._location[j]._location.empty() && servers[i]._location[j]._location[0] != '/'
                        && servers[i]._location[j]._location.length() > 1) ||
                    (!servers[i]._location[j]._redirect.empty() && servers[i]._location[j]._redirect[0] != '/') ||
                    (!servers[i]._location[j]._uploadDir.empty() && servers[i]._location[j]._uploadDir[0] != '/')) {
                         throw std::runtime_error("Error: Missing leading '/' in configuration file - check location config for " + servers[i]._location[j]._location );
                    }
                if (servers[i]._location[j]._errorPage.size() > 0) {
                for (std::map<int, std::string>::const_iterator it = servers[i]._location[j]._errorPage.begin();
                    it != servers[i]._location[j]._errorPage.end(); it++) {
                        if (!it->second.empty() && (it->second[0] != '/'))
                            throw std::runtime_error("Error: Missing leading '/' in configuration file - check location error_pages in " + servers[i]._location[j]._location);
                    }
                }
            }
            
        }
       
    }
}

void checkValidPathsTrailing(std::vector<Server> servers) {
    for (size_t i = 0; i < servers.size(); i++) {
        if ((!servers[i]._root.empty() && servers[i]._root[servers[i]._root.size() - 1] == '/') || 
            (!servers[i]._index.empty() && servers[i]._index[servers[i]._index.size() - 1] == '/'))
             throw std::runtime_error("Error: Remove trailing '/' in configuration file - check server config");
        if (servers[i]._errorPage.size() > 0) {
                for (std::map<int, std::string>::const_iterator it = servers[i]._errorPage.begin();
                    it != servers[i]._errorPage.end(); it++) {
                        if (!it->second.empty() && (it->second[it->second.size() -1] == '/'))
                            throw std::runtime_error("Error: Remove trailing '/' in configuration file - check server error_pages");
                    }
                }
        if (servers[i]._location.size() > 0) {
            for (size_t j = 0; j < servers[i]._location.size(); j++) {
                if (servers[i]._location[j]._location.length() == 1 && servers[i]._location[j]._location[0] != '/')
                    throw std::runtime_error("Error: Invalid location format in config file at [" + servers[i]._location[j]._location +"]");
                if ((!servers[i]._location[j]._root.empty() && servers[i]._location[j]._root[servers[i]._location[j]._root.size() - 1] == '/') || 
                    (!servers[i]._location[j]._locationIndex.empty() && servers[i]._location[j]._locationIndex[servers[i]._location[j]._locationIndex.size() - 1] == '/') ||
                    (!servers[i]._location[j]._location.empty() && servers[i]._location[j]._location[servers[i]._location[j]._location.size() - 1 ] == '/'
                        && servers[i]._location[j]._location.length() > 1) ||
                    (!servers[i]._location[j]._redirect.empty() && servers[i]._location[j]._redirect[servers[i]._location[j]._redirect.size() - 1] == '/') ||
                    (!servers[i]._location[j]._uploadDir.empty() && servers[i]._location[j]._uploadDir[servers[i]._location[j]._uploadDir.size() - 1] == '/')) {
                         throw std::runtime_error("Error: Remove trailing '/' in configuration file - check location config for " + servers[i]._location[j]._location );
                    }
                if (servers[i]._location[j]._errorPage.size() > 0) {
                for (std::map<int, std::string>::const_iterator it = servers[i]._location[j]._errorPage.begin();
                    it != servers[i]._location[j]._errorPage.end(); it++) {
                        if (!it->second.empty() && (it->second[it->second.size() - 1] == '/'))
                            throw std::runtime_error("Error: Remove trailing '/' in configuration file - check location error_pages in " + servers[i]._location[j]._location);
                    }
                }
                if (servers[i]._location[j]._cgi.size() > 0) {
                for (std::map<std::string, std::string>::const_iterator it = servers[i]._location[j]._cgi.begin();
                    it != servers[i]._location[j]._cgi.end(); it++) {
                        if (!it->second.empty() && (it->second[it->second.size() - 1] == '/'))
                            throw std::runtime_error("Error: Remove trailing '/' in configuration file - check location cgi paths in " + servers[i]._location[j]._location);
                    }
                }
            }
            
        }
       
    }
}

void checkBasicServerConfig(const std::vector<Server>& servers) {
    if (servers.size() == 0)
        throw std::runtime_error("Error: Unable to parse server configurations");
    for (size_t i = 0; i < servers.size(); i++) {
        if (servers[i].ipAddress.empty() || servers[i].port == 0 || servers[i]._root.empty() || servers[i]._index.empty())
            throw std::runtime_error("Error: Invalid server configuration - check server block\nServer must have listen, root and index configured");
        if (servers[i]._allowedMethods.size() == 0)
            throw std::runtime_error("Error: Invalid server configuration - check server block\nServer must have at least one allowed method configured");
        if (servers[i]._clientMaxBodySize <= 0)
            throw std::runtime_error("Error: Invalid server configuration - check server block\nServer must have client_max_body_size configured greater than 0");
        if (servers[i].port < 1024 || servers[i].port > 65535)
            throw std::runtime_error("Error: Invalid server configuration - check server block\nServer must have listen port configured between 1024 and 65535");
        if (servers[i]._errorPage.size() > 0) {
            for (std::map<int, std::string>::const_iterator it = servers[i]._errorPage.begin(); it != servers[i]._errorPage.end(); it++) {
                if (it->second.empty())
                        throw std::runtime_error("Error: Missing error page path for error code " + to_string(it->first) + " in server block");
            }
        }
  
    }
}