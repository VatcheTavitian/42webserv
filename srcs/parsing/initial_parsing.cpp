# include "Parsing.hpp"
# include <stack>


void checkPortsAndServerNames(const std::vector<Server>& servers) {
    for (size_t i = 0; i < servers.size(); ++i) 
    {
        for (size_t j = i + 1; j < servers.size(); ++j) 
        {
            if (servers[i]._serverName == servers[j]._serverName && !servers[i]._serverName.empty() && servers[i].port == servers[j].port)  
                throw std::runtime_error("Error: Duplicate server names found in configuration file");
            if ((servers[i].port == servers[j].port) &&  (servers[i]._serverName == servers[j]._serverName)) 
                throw std::runtime_error("Error: Duplicate ports found in configuration file");
        }
    }
}

void checkBrackets(const std::string &filePath) 
{
    std::ifstream file(filePath.c_str());
    if (!file.is_open()) 
    {
        std::cerr << "Error: Unable to open file " << filePath << std::endl;
        return;
    }

    std::stack<int> bracketStack;
    std::string line;
    int lineNumber = 0;

    while (getline(file, line)) 
    {
        lineNumber++;
        for (size_t i = 0; i < line.length(); ++i) 
        {
            char ch = line[i];
            if (ch == '{') 
                bracketStack.push(lineNumber);
            else if (ch == '}') 
            {
                if (!bracketStack.empty()) 
                    bracketStack.pop();
                else 
                    throw BracketMismatchException(lineNumber);
            }
        }
    }
    while (!bracketStack.empty()) 
    {
        int unmatchedLine = bracketStack.top();
        bracketStack.pop();
        throw BracketMismatchException2("Error: Unmatched opening bracket '{' at line " + to_string(unmatchedLine));
    }

    file.close();
}

void    compareLocationData(std::vector<Location> &locations)
{
    std::set<std::string> nameSet;
    for (std::vector<Location>::iterator it = locations.begin(); it != locations.end(); ++it)
    {
        if (it->_redirect.empty() && it->_locationIndex.empty())
            throw std::runtime_error("Error: Both redirect and index are empty for a location.");
        if (nameSet.find(it->_location) != nameSet.end()) 
            throw std::runtime_error("Error: Duplicate location name found: " + it->_location);
        if (!it->_locationIndex.empty() && it->_redirect.empty()) {
           if (it->_root.empty())
                throw std::runtime_error("Error: Index defined but root is undefined for a location.");
        }
        if (it->_redirect.empty()) {
            if (it->_uploadDir.empty() && it->_permitUpload) 
                throw std::runtime_error("Error: Upload directory is empty for a location when permit upload is set to on.");
            if (it->_uploadDir.empty() && it->_permitDelete) 
                throw std::runtime_error("Error: Upload directory is empty for a location when permit delete is set to on.");
            if (it->_listDirectory && it->_locationIndex.empty()) 
                throw std::runtime_error("Error: Index is empty for a location when permit directory is set to on.");
            if (it->_redirect.empty() && it->_clientMaxBodySize <= 0) 
                throw std::runtime_error("Error: You must define a client max body size for a location.");
            if (it->_allowedMethods.size() == 0)
                throw std::runtime_error("Error: No allowed methods defined for a location.");
            if (it->_errorPage.size() > 0) {
                for (std::map<int, std::string>::const_iterator it2 = it->_errorPage.begin(); it2 != it->_errorPage.end(); it2++) {
                    if (it2->second.empty())
                        throw std::runtime_error("Error: Missing error page path for error code " + to_string(it2->first) + " in location block");
                }
            }
        }
        if (it->_location == "/cgi-bin") {
            if (it->_cgi.size() == 0)
                throw std::runtime_error("Error: No CGI paths defined for location /cgi-bin");
            for (std::map<std::string, std::string>::const_iterator it2 = it->_cgi.begin(); it2 != it->_cgi.end(); it2++) {
                if (it2->second.empty())
                    throw std::runtime_error("Error: Missing CGI path for CGI type " + it2->first + " in location /cgi-bin");
            }
            for (std::map<std::string, std::string>::const_iterator it2 = it->_cgi.begin(); it2 != it->_cgi.end(); it2++) {
                std::ifstream file(it2->second.c_str());
                if (!file.good())
                    throw std::runtime_error("Error: Invalid path to binary '" + it2->second +"' found for " + it2->first + " in location /cgi-bin");
            }
        }
        nameSet.insert(it->_location);
    }
}

void    compareServerLocations(std::vector<Server> &servers)
{
    for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it) 
        compareLocationData(it->_location);
}

void checkSemiColons(const std::string &filePath) 
{
    std::ifstream file(filePath.c_str());
    if (!file.is_open()) 
    {
        std::cerr << "Error: Unable to open file " << filePath << std::endl;
        return;
    }

    std::stack<int> bracketStack;
    std::string line;
    int lineNumber = 0;
    while (getline(file, line)) {
        lineNumber++;
        line = trim(line);
        if ((line.find("{") == std::string::npos) && (line.find("}") == std::string::npos)) {
            if(line.size() > 0 && line[line.size() - 1] != ';')
                 throw std::runtime_error("Error: Missing terminating semi-colon in config file on line number " + to_string(lineNumber));
        }
    }
    file.close();
}