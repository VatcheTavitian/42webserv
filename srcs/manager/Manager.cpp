#include "Manager.hpp"

std::string EMPTYBUFFER = "";
Manager* Manager::instance = NULL;

Manager::Manager(std::vector<Server>& servers): servers(servers) {
    std::string RESET = "\033[0m";
    std::string YELLOW = "\033[33m";
    FD_ZERO(&readSet);
    FD_ZERO(&writeSet);
    FD_ZERO(&read_master);
    FD_ZERO(&write_master);

    for (unsigned int i = 0;i < servers.size(); i++) {
        fcntl(servers[i].serverptr->get_socket()->get_sock(), F_SETFL, O_NONBLOCK);
        if (!servers[i]._serverName.empty())
            std::cout << YELLOW << "Webserver listening on port " << servers[i].port << " with server name '" \
                << servers[i]._serverName << "'..." <<  RESET << std::endl;
        else
            std::cout << YELLOW << "Webserver listening on port " << servers[i].port << "..." <<  RESET << std::endl;
    }
    listen_for_events();
}

int Manager::stringToInt(const std::string& str) {
    std::istringstream iss(str);
    int result;
    iss >> result;
    return result;
}


int Manager::setNonBlocking(int socket) {
    int flags = fcntl(socket, F_GETFL, 0);
    if (flags == -1) {
        return -1;
    }
    return fcntl(socket, F_SETFL, flags | O_NONBLOCK);
}

void Manager::addServerSockets(std::vector<Server>& servers, int &maxSocket) {
    std::vector<int> uniqueSocket;
    for (size_t i = 0; i < servers.size(); i++) {
        fcntl(servers[i].serverptr->get_socket()->get_sock(), F_SETFL, O_NONBLOCK, FD_CLOEXEC);
        if (!(FD_ISSET(servers[i].serverptr->get_socket()->get_sock(), &read_master)) ) {
            if (std::find(uniqueSocket.begin(), uniqueSocket.end(), servers[i].serverptr->get_socket()->get_sock()) == uniqueSocket.end()) {
                uniqueSocket.push_back(servers[i].serverptr->get_socket()->get_sock());
                FD_SET(servers[i].serverptr->get_socket()->get_sock(), &read_master);
                if (servers[i].serverptr->get_socket()->get_sock() > maxSocket)
                    maxSocket = servers[i].serverptr->get_socket()->get_sock();
            }
        }
    }
}

void Manager::addNewSocket(Server &listeningserver, int &maxSocket) {
    struct sockaddr_in address = listeningserver.serverptr->get_socket()->get_address();
    int addrlen = sizeof(address);
    int newSocket = accept(listeningserver.serverptr->get_socket()->get_sock(), (struct sockaddr *) &address, (socklen_t *)&addrlen); 
    if (newSocket == - 1) {
        perror("Error accepting connection");
        return;
    }
    int optval = 1;
    setsockopt(newSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    fcntl(newSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
    FD_SET(newSocket, &read_master);
    if (newSocket > maxSocket)
        maxSocket = newSocket;

    Client *newClientSocket = new Client(newSocket, address,  servers, &listeningserver); 
    openClients.push_back(newClientSocket);
}

void Manager::handle_sigint(int signum) {
    switch (signum) {
        case SIGINT:
        std::cerr << "\rReceived SIGINT (Ctrl+C). Exiting..." << std::endl;
        std::vector<SimpleServer*> uniqueServer;
        for (size_t i = 0; i < instance->servers.size(); i++) {
            if (instance->servers[i].serverptr != NULL) {
                if (std::find(uniqueServer.begin(), uniqueServer.end(),instance->servers[i].serverptr) == uniqueServer.end()) {
                    uniqueServer.push_back(instance->servers[i].serverptr);
                }
            }
        }
        for (size_t i = 0; i < uniqueServer.size(); i++) {
            close(uniqueServer[i]->get_socket()->get_sock());
            delete uniqueServer[i];
        }
        
        exit(EXIT_SUCCESS);
        break;
    }
}

void Manager::checkIfUpload(Client &currentClient) {
   if (!currentClient.listeningserver->_locationIdentified->_location.empty()) {
        if (currentClient.payloadSize <= currentClient.listeningserver->_locationIdentified->_clientMaxBodySize) {
            if (currentClient.listeningserver->_locationIdentified->_permitUpload) {
                try {
                    if (currentClient.payloadSize > 10000000)
                        currentClient.request->handleUploadWithThread();
                    else
                        currentClient.request->handleUpload();
                } catch (const std::runtime_error& e) {
                    std::cerr << "Exception found: " << e.what() << std::endl;
                    currentClient.clientStatusCode = 400;
                }
            }
            else
                currentClient.clientStatusCode = 503;
        }
        else
            currentClient.clientStatusCode = 413;
    }                            
    else if (currentClient.payloadSize > currentClient.maxContentLength && currentClient.listeningserver->_locationIdentified->_location.empty())
        currentClient.clientStatusCode = 413;
}



void Manager::listen_for_events(){
    instance = this;
    int maxSocket = 0;
    addServerSockets(servers, maxSocket);
    std::signal(SIGINT, handle_sigint);

    while (maxSocket) {
        
        this->readSet = read_master;
		this->writeSet = write_master;

        int activity =  select(maxSocket + 1, &readSet, &writeSet, NULL, NULL);
        if (activity <= 0) {
            perror("Error in initiating select");
            continue;
        }

        std::vector<int> uniqueSocket;
        for (unsigned int i = 0; i < servers.size(); i++) {
            if (FD_ISSET(servers[i].serverptr->get_socket()->get_sock(), &readSet)) {
                if (std::find(uniqueSocket.begin(), uniqueSocket.end(), servers[i].serverptr->get_socket()->get_sock()) == uniqueSocket.end()) {  
                    uniqueSocket.push_back(servers[i].serverptr->get_socket()->get_sock());
                    addNewSocket(servers[i], maxSocket);
                }
            } 
        }
     
        for (std::vector<Client *>::iterator it = openClients.begin(); it != openClients.end(); ++it) {
            Client &currentClient = **it;
            
            int currentSocket = currentClient.socket;
            char buffer[1024];
            memset(buffer, 0, 1024);

            if (FD_ISSET(currentSocket, &readSet)) {
                int bytesRead = recv(currentSocket, buffer, 1024, 0);
                currentClient.getPayloadLength();
                if (bytesRead > 0) {
                    currentClient.getPayloadLength();
                }
                if (bytesRead == -1) {
                    delete (*it);
                    it = openClients.erase(it);
                    --it;
                    close(currentSocket);
                    FD_CLR(currentSocket, &read_master);
                } else if (bytesRead < 1024 || bytesRead == 0) {
                    currentClient.requestBuffer += std::string(buffer, bytesRead);
                    size_t sizeOfBuffer = currentClient.requestBuffer.size();
                    if (static_cast<long>(sizeOfBuffer) >= currentClient.payloadSize) {
                        FD_CLR(currentSocket, &read_master);
                        FD_SET(currentSocket, &write_master);
                        currentClient.createRequest();
                        currentClient.request->parseHeaders();
                        currentClient.getPayloadLength();
                        currentClient.checkListeningServerAllocation();
                        currentClient.identifyLocationAndAssign();
                        if (currentClient.request->headers["Method"] == "POST" && currentClient.isMethodAllowed() && currentClient.listeningserver->_locationIdentified)
                            checkIfUpload(currentClient);
                        else if (currentClient.request->headers["Method"] == "DELETE")
                            currentClient.request->handleDelete();
                        currentClient.createResponse();
                    } else if (currentClient.payloadSize == -1) {
                        FD_CLR(currentSocket, &read_master);
                        FD_SET(currentSocket, &write_master);
                        currentClient.createRequest();
                        currentClient.request->parseHeaders();
                        currentClient.createResponse();
                    }   
                } else {
                    currentClient.requestBuffer += std::string(buffer, bytesRead);;
                }
            }
            if (FD_ISSET(currentSocket, &writeSet)) {                
                if (currentClient.clientStatusCode != 200) {
                    Server* queriedServer = const_cast<Server*>(currentClient.listeningserver);
                    currentClient.res->deliverErrorPage(currentSocket, currentClient.clientStatusCode, queriedServer );
                    currentClient.clientStatusCode = 200;
                    currentClient.res->writeStamp();
                }
                else if (currentClient.requestBuffer.length() > 0){
                    currentClient.res->servePage();
                    currentClient.requestBuffer.clear();
                    currentClient.requestBuffer.swap(EMPTYBUFFER);
                }
                if (close(currentSocket) == -1)
                    std::cout << "Error closing socket" << std::endl;
                delete (*it);
                it = openClients.erase(it);
                --it;
                FD_CLR(currentSocket, &write_master);
            }
        }
    }
}

Manager::~Manager() {

}

