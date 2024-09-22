    #ifndef __REQUEST_HPP__
    # define __REQUEST_HPP__

    # include <vector>
    # include <map>
    # include <iostream>
    # include <fstream>
    # include <string>
    # include "Client.hpp"
    # include <cstdio>
    # include "ParsingHelpers.hpp"
    # include <sstream>
    # include <ctime>
    # include <pthread.h>
    # include <unistd.h>


    struct Server;
    class Client;

    struct ThreadData {
        std::string* base64String;
        std::string filePath;
    };

    class Request
    {
        private:
            Request();
            std::string RESET; 
            std::string GREEN;
            std::string BLUE; 

            void        getFileName();
            std::vector<Server> servers;
            std::string fileName;
            std::string base64String;
            std::string buffer;
            std::string fileToDeleteName;
            bool        deleteConditionsMet();
            static void* uploadTask(void* arg);
            void        writeBufferToFile(const std::string& filename, const std::string& decoded);

        public:
            Request(std::vector<Server> servers, std::string buffer, Server* listeningserver);
            ~Request();
            Client* currentClientPtr;
            Server* listeningserver;
            void    parseHeaders();
            void    handleUpload();
            void    handleUploadWithThread();
            void    handleDelete();
            std::map<std::string, std::string>  headers;
    };

    #endif