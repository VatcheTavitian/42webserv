#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

# include <string>
# include <vector>
# include <map>
# include "Manager.hpp"
# include "Request.hpp"
# include "Response.hpp"

class Request;
class Response;

class Client {

    private:
        int postDetected;
        std::string ipAddress;
        Client();
        std::string getExtType(const std::string& path);

    public:
        Client(int sock, struct sockaddr_in addr, std::vector<Server> servers, Server* listeningserver);
        ~Client();
        int clientStatusCode;
        int socket;
        struct sockaddr_in address;
        long payloadSize;
        long maxContentLength;
        std::vector<Server> servers;
        Server* listeningserver;
        std::string requestBuffer;
        Request *request;
        Response *res;
        bool isMethodAllowed();
        void createRequest();
        void createResponse();
        void getPayloadLength();
        void identifyLocationAndAssign();
        void checkListeningServerAllocation();
        bool transferInProgress;
        size_t toSend;
        size_t processed;
    };

#endif 