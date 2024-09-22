#include "Client.hpp"

Client::Client() {
}

Client::Client(int sock, struct sockaddr_in addr,  std::vector<Server> servers, Server* listeningserver)
        : socket(sock), address(addr),  servers(servers), listeningserver(listeningserver) {
        this->payloadSize = -1;
        this->postDetected = 0;
        this->maxContentLength = listeningserver->_clientMaxBodySize;
        this->clientStatusCode = 200;
        this->transferInProgress = false;
        this->toSend = 0;
        this->processed = 0;
        int optval = 1;
	    setsockopt(listeningserver->serverptr->get_socket()->get_sock(), SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    }

void Client::createRequest() {
 
    this->request = new Request(servers, requestBuffer.c_str(), listeningserver);
    this->request->currentClientPtr = this; 
}

void Client::createResponse() {
    this->res = new Response(request, servers, this->socket, listeningserver);
  
}

int stringToInt(const std::string& str) {
    std::stringstream ss(str);
    int result;
    ss >> result;
    return result;
}

void Client::getPayloadLength() {
    if (!postDetected && payloadSize < 0) {
        size_t pos = requestBuffer.find("Content-Length: ");
        if (pos != std::string::npos) {
            pos += strlen("Content-Length: ");
            size_t endPos = requestBuffer.find("\r\n", pos);
            if (endPos != std::string::npos) {
                std::string lengthStr = requestBuffer.substr(pos, endPos - pos);
                payloadSize = stringToInt(lengthStr);
            }
            postDetected = 1;
        }
    }
}

bool Client::isMethodAllowed() {
    if (this->listeningserver->_locationIdentified) {
        if (this->listeningserver->_locationIdentified->_allowedMethods.size() > 0) {
            std::vector<std::string>::iterator it = listeningserver->_locationIdentified->_allowedMethods.begin();
            while (it != listeningserver->_locationIdentified->_allowedMethods.end()) {
                if (*it == this->request->headers["Method"]) {
                    return true;
                }
                it++;
            }
        }
    } else if (this->listeningserver->_allowedMethods.size() > 0) {
            std::vector<std::string>::iterator it = listeningserver->_allowedMethods.begin();
            while (it != listeningserver->_allowedMethods.end()) {
                if (*it == this->request->headers["Method"]) {
                    return true;
                }
                it++;
            }
        }
    return false;
}

void Client::identifyLocationAndAssign()  {
    for (size_t i = 0; i < listeningserver->_location.size(); i++)
    {
        if (listeningserver->_location[i]._location == this->request->headers["Path"] ||
            listeningserver->_location[i]._locationIndex == this->request->headers["Path"] ) {
            this->listeningserver->_locationIdentified = &(listeningserver->_location[i]);
            return;
        }
    }
     this->listeningserver->_locationIdentified = NULL;
}


Client::~Client() {
    requestBuffer.clear();
    requestBuffer.swap(EMPTYBUFFER);
    delete this->request;
    delete this->res;
}

void Client::checkListeningServerAllocation() {
        if ((listeningserver->_serverName != request->headers["Host"])) {
            for (size_t i = 0; i < servers.size() ; i++) {
                if (servers[i]._serverName != listeningserver->_serverName) {
                    if ((servers[i]._serverName == request->headers["Host"]) && (servers[i].port == listeningserver->port)) {
                        listeningserver = &servers[i];
                        return ;
                    }
                }
        }
        for (size_t i = 0; i < servers.size() ; i++) {
            if (servers[i]._serverName != listeningserver->_serverName) {
                if ((servers[i]._serverName.empty() && (request->headers["Host"] == "127.0.0.1" || request->headers["Host"] == "localhost")) && (servers[i].port == listeningserver->port)) {
                    listeningserver = &servers[i];
                    return ;
                }
            }
        }
    }
}