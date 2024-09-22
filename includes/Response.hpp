#ifndef __RESPONSE_HPP__
# define __RESPONSE_HPP__

# include <fstream>
# include <sstream>
# include <stdio.h>
# include <stdlib.h>
# include "Request.hpp"

#define EXIT_TIMEOUT 124

class Request;
struct Server;
struct Location;

class Response {
private:
    std::string RESET; 
    std::string RED;
    std::string BLUE; 

    Response();
    Request *request;
    std::vector<Server> servers;
    const Server* listeningserver;
    template <typename T> std::string to_string(T value);
    std::string getContentType(const std::string& fileName);
    std::string getExtType(const std::string& fileName);
    int i;
    int badHostname();
    int statusCode;
    void handleRedirect(Location& result);
    void runCGI();
    void serveDirectoryPages(std::string filePath, Location* result);
    std::string adjustRootAndPath(Location* &result);
    std::string getCoreLocation();
    void assignResult(Location* &result);
    void urlDecodeSpace(std::string& file_open);
    bool locationHasErrorPages(Location& result, int statusCode);
    bool isMethodAllowed();
    bool locationFinal();
    bool notCGI();
    bool baseLocationValid();
    void sendBinaryFile(std::string& file_open);
    std::string allocateFileName(std::string path);
    
public:
    Response(Request *request, std::vector<Server>& servers, const int i, const Server* listeningserver);
    ~Response();
    void    servePage();
    void    writeStamp();
    void    deliverErrorPage(int fd, int statusCode, Server* queriedServer);

};

#endif