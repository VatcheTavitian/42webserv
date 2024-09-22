#include "Parsing.hpp"

unsigned long to_unsigned_long(std::string& str) {
    size_t start = str.find_first_not_of(' ');
    size_t end = str.find_last_not_of(' ');

    if (start == std::string::npos || end == std::string::npos) {
        throw std::invalid_argument("Invalid input string (only whitespace): " + str);
    }

    std::string trimmed_str = str.substr(start, end - start + 1);
    if (!trimmed_str.empty() && trimmed_str[trimmed_str.length() - 1] == ';') {
        trimmed_str.erase(trimmed_str.length() - 1);
    }

    for (size_t i = 0; i < trimmed_str.length(); ++i) {
        if (!std::isdigit(trimmed_str[i])) {
            throw std::invalid_argument("Invalid input string (non-digit characters found): " + trimmed_str);
        }
    }

    std::stringstream ss(trimmed_str);
    unsigned long result;
    ss >> result;

    if (ss.fail() || !ss.eof()) {
        throw std::invalid_argument("Invalid input string (conversion failed): " + str);
    }

    return result;
}

void cleanDataLoc(Location &loc) 
{
    cleanStringCopy(loc._redirect);
    cleanStringCopy(loc._location);
    cleanStringCopy(loc._root);
    cleanStringCopy(loc.permitUploadStr);
    cleanStringCopy(loc._locationIndex);
    for (std::map<int, Location::t_str>::iterator it = loc._errorPage.begin(); it != loc._errorPage.end(); ++it) 
        cleanStringCopy(it->second);
    for (std::vector<Location::t_str>::iterator it = loc._allowedMethods.begin(); it != loc._allowedMethods.end(); ++it)
        cleanStringCopy(*it);
    for (std::map<std::string, std::string>::iterator it = loc._cgi.begin(); it != loc._cgi.end(); ++it) 
	{
        cleanStringCopy(it->first);
        cleanStringCopy(it->second);
    }
    cleanStringCopy(loc._uploadDir);
};

void cleanDataServer(Server &srv) 
{
    cleanString(srv._serverName);
    cleanString(srv.ipAddress);
    cleanString(srv._root);
    cleanString(srv._index);
    cleanString(srv.allow_methods);
    srv._allowedMethods.clear();
    std::istringstream iss(srv.allow_methods.c_str());
    std::string method;
    while (iss >> method) 
    {
        cleanString(method);
        srv._allowedMethods.push_back(method);
    }
    for (std::map<int, std::string>::iterator it = srv._errorPage.begin(); it != srv._errorPage.end(); ++it) 
        cleanString(it->second);
    for (std::vector<Location>::iterator it = srv._location.begin(); it != srv._location.end(); ++it) 
        cleanDataLoc(*it);
};

void cleanDataServers(std::vector<Server> &servers)
{
    for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); ++it) 
        cleanDataServer(*it);	
};

void parseLocation(Location& location, std::string line) 
{
    std::string line2;
    if (line.find(";"))
        line2 = line.substr(0, line.find(";"));
    else
    line2 = line;
    std::stringstream iss(line2);
    std::string key, value;
    iss >> key >> value;
    if (key == "{") 
       return ;
    if (key == "location") 
        location._location = value;
    else if (key == "redirect") 
    {
        iss >> value;
        location._redirect = value;
    } 
    else if (key == "cgi") 
    {
       
        std::string path;
        iss >> path;
        location._cgi[value] = path;
    } 
    else if (key == "root") 
    {
        iss >> value;
        location._root = value;
    }
    else if (key == "index") 
    {
        iss >> value;
        location._locationIndex = value;
    } 
    else if (key == "error_page") 
    {
        std::string page;
        iss >> page;
        location._errorPage[to_unsigned_long(value)] = page;
    
    } 
    else if (key == "client_max_body_size") 
    {
        iss >> value;
        location._clientMaxBodySize = to_unsigned_long(value);
    } 
    else if (key == "allow_methods") 
    {
        location._allow_methods.clear();
        if (value == "POST" || value == "GET" || value == "DELETE")
             location._allowedMethods.push_back(value);
        while (iss >> value) 
        {
            if (value == "POST" || value == "GET" || value == "DELETE") 
                location._allowedMethods.push_back(value);       
        }
        location._allow_methods = trim(location._allow_methods);
    }
    else if (key == "upload_dir") 
    {
        iss >> value;
        location._uploadDir = value;
    } 
    else if (key == "permit_upload") 
    {
        iss >> value;
        if (value == "on")
            location._permitUpload = true;
        else
            location._permitUpload = false;
    } 
    else if (key == "permit_delete") 
    {
        iss >> value;
        if (value == "on")
            location._permitDelete = true;
        else
            location._permitDelete = false;
    }
    else if (key == "permit_directory") 
    {
        iss >> value;
        if (value == "on")
            location._listDirectory = true;
        else
            location._listDirectory = false;
    }
 
}

void parseLine(Server& server, std::stringstream& iss) 
{
    std::string key, value;

    iss >> key;

    if (key == "server_name") 
    {
        iss >> value;
        server._serverName = value;
    } 
    else if (key == "listen") 
    {
        iss >> value;
        size_t pos = value.find(':');
        if (pos != std::string::npos) 
        {
            server.ipAddress = value.substr(0, pos);
            std::string portStr = value.substr(pos + 1);
            server.port = to_unsigned_long(portStr);
        }
    } 
    else if (key == "root") 
    {
        iss >> value;
        server._root = value;
    } 
    else if (key == "index") 
    {
        iss >> value;
        server._index = value;
    } 
    else if (key == "error_page") 
    {
        int code;
        std::string page;
        iss >> code >> page;
        if (code == 400 || code == 404 || code == 500) 
            server._errorPage[code] = page;
    } 
    else if (key == "client_max_body_size") 
    {
        iss >> value;
        server._clientMaxBodySize = to_unsigned_long(value);
    } 
    else if (key == "allow_methods") 
    {
        server.allow_methods.clear();
        std::string method;
        while (iss >> method) 
        {
            if (method == "POST" || method == "GET" || method == "DELETE" || method == "POST;" || method == "GET;" || method == "DELETE;") 
                server.allow_methods += method + " ";
        }
        server.allow_methods = trim(server.allow_methods);
    }
}

void assignStatusCodes(std::map<std::string, std::string>& statusCodes) 
{
	statusCodes["200"] = "OK";
	statusCodes["201"] = "Created";
	statusCodes["202"] = "Accepted";
	statusCodes["204"] = "No Content";
	statusCodes["301"] = "Moved Permanently";
	statusCodes["302"] = "Found";
	statusCodes["304"] = "Not Modified";
	statusCodes["400"] = "Bad Request";
	statusCodes["401"] = "Unauthorized";
	statusCodes["403"] = "Forbidden";
	statusCodes["404"] = "Not Found";
	statusCodes["405"] = "Method Not Allowed";
	statusCodes["408"] = "Request Timeout";
	statusCodes["412"] = "Precondition Failed";
	statusCodes["413"] = "Content Too Large";
	statusCodes["500"] = "Internal Server Error";
	statusCodes["501"] = "Not Implemented";
	statusCodes["502"] = "Bad Gateway";
	statusCodes["503"] = "Service Unavailable";
}


void parseConfigFile(const std::string& filename, std::vector<Server>& servers)
{
    std::ifstream file(filename.c_str());
    if (!file.is_open()) 
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    std::string line;
    Server server;
    Location location;
    bool inLocation = false;
    bool inServer = false;
    while (std::getline(file, line)) 
    {
        if (line.empty()) // || line[0] == '#') 
            continue;

        std::stringstream iss(line);

        if (line.find("server") != std::string::npos && !inLocation && !inServer)
        {
            if (trimStd(line) != "server{")
                throw std::runtime_error("Invalid server block open");
            server = Server();
            assignStatusCodes(server.statusCodes);
            server.serverptr = NULL;
            inServer = true;
        }
        if (line.find("location") != std::string::npos && inServer)
        {
            location = Location();
            inLocation = true;
            while (inLocation) 
            {
                if (line.find("}") != std::string::npos)
                {
                    if (trimStd(line) != "}")
                        throw std::runtime_error("Invalid closing bracket for location block");
                    inLocation = false;
                    break ;
                }
                else
                {
                    parseLocation(location, line);
                    getline(file, line);
                }
            }
            server._location.push_back(location);
            
        }
        else if (line.find("}") != std::string::npos && inServer)
        {
            if (trimStd(line) != "}")
                throw std::runtime_error("Invalid closing bracket for server block");
            servers.push_back(server);
            inServer = false;
        }
        else 
            parseLine(server, iss);
    }
}
