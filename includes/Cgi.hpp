#ifndef CGI_HPP
# define CGI_HPP

# include <sys/types.h>
# include <sys/wait.h>
# include "Client.hpp"

class Cgi
{
    private:
        Client *cont;
        std::map<std::string, std::string> env;
        std::map<std::string, std::string> header;
        std::string path;

    public:
        Cgi();
        Cgi(Client *other);
        Cgi(const Cgi &other);
        Cgi &operator=(Cgi const &other);
        virtual ~Cgi();
        void initenv();
        void cgi_run();
        void tofile(std::string path);
        const std::string findscript(std::string uri);
        std::string findquery(std::string uri);

};

#endif