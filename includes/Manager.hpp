#ifndef __MANAGER_HPP__
# define __MANAGER_HPP__

# include "managerheaders.hpp"

extern std::string EMPTYBUFFER;

class Client;

class Manager
{
    private:
        std::vector<Server>& servers;
        void addServerSockets(std::vector<Server>& server, int &maxsocket);
        void addNewSocket(Server &listeningserver, int &maxSocket);
        int stringToInt(const std::string& str);
        int setNonBlocking(int socket) ;
        fd_set readSet;
        fd_set writeSet;
        fd_set read_master;
        fd_set write_master;
        std::vector<Client *> openClients;
        static void handle_sigint(int signum);
        static Manager* instance; 
        void checkIfUpload(Client &currentClient);

    public:
        Manager(std::vector<Server>& servers);
        ~Manager();
        void listen_for_events();

};

#endif