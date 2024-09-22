#ifndef PARSING_HPP
# define PARSING_HPP

# include <iostream>
# include <fstream>
# include <sstream>
# include <vector>
# include <string>
# include <map>
# include <stack>
# include <set>
# include <algorithm>
# include "Location.hpp"
# include "Server.hpp"
# include <cstring>


void allocateServerPointers(std::vector<Server>& servers);
void checkValidPathsLeading(std::vector<Server> servers);
void checkValidPathsTrailing(std::vector<Server> servers);
void cleanDataLoc(Location &loc);
void cleanDataServer(Server &srv);
void cleanDataServers(std::vector<Server> &servers);
void parseLocation(Location& location, std::string line);
void parseLine(Server& server, std::stringstream& iss);
void assignStatusCodes(std::map<std::string, std::string>& statusCodes);
void parseConfigFile(const std::string& filename, std::vector<Server>& servers);
void splitIpAddressAndPort(const std::string &str, std::string &ipAddress, int &port);
void cleanString(std::string &str);
void copyFileIgnoringComments(const std::string &sourcePath, const std::string &tempPath);
void checkPortsAndServerNames(const std::vector<Server>& servers);
void checkBrackets(const std::string &filePath);
void compareLocationData(std::vector<Location> &locations);
void compareServerLocations(std::vector<Server> &servers);
void checkSemiColons(const std::string &filePath);
void checkBasicServerConfig(const std::vector<Server>& servers);

unsigned long to_unsigned_long(std::string& str);

std::string cleanStringCopy(const std::string &str);
std::string trimStd(std::string str);
std::string trim(const std::string& str);
std::string getType(const std::string& fileName);
std::string to_string(int value);

class BracketMismatchException : public std::runtime_error 
{
    public:
        BracketMismatchException(int lineNumber)
            : std::runtime_error("Error: Unmatched closing bracket '}' at line " + to_string(lineNumber)) {}
};

class BracketMismatchException2 : public std::runtime_error 
{
    public:
        BracketMismatchException2(const std::string &message)
            : std::runtime_error(message) {}
};

class MissingSemicolonException : public std::runtime_error 
{
    public:
        MissingSemicolonException(const std::string &message)
            : std::runtime_error(message) {}
};

#endif