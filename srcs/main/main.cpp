#include "serverheaders.hpp"
#include "Manager.hpp"
#include "Parsing.hpp"

int    start(std::string filePath)
{
    std::string tempFile = "configs/tempConfFileDeletable.conf";
    try
    {
        std::vector<Server> servers;
        copyFileIgnoringComments(filePath, tempFile);
        checkBrackets(tempFile);
        checkSemiColons(tempFile);
        parseConfigFile(tempFile, servers);
        cleanDataServers(servers);
        compareServerLocations(servers);
        checkValidPathsLeading(servers);
        checkValidPathsTrailing(servers);
        checkPortsAndServerNames(servers);
        allocateServerPointers(servers);
        remove(tempFile.c_str());
        checkBasicServerConfig(servers);
        Manager *manager = new Manager(servers);
        delete manager;
    }
    catch(const std::exception& e)
    {
        std::ifstream file(tempFile.c_str());
        if (file.is_open()) 
        {
            remove(tempFile.c_str());
        }
        std::cerr << e.what() << '\n';
        return 1;
    }
    return 0;
};

int main(int argc, char **argv) 
{
	if (argc == 1)
        start("configs/default.conf");
    else if (argc == 2 && getType(argv[1]) == "conf")
		start(argv[1]);
    else
        std::cout << "Error:\nTo run with default config file './webserv'\n" << 
        "Default config file must be in /configs/default.conf'\n" <<
        "To specify a config file location use './webserv <path to config file>'\n" << 
        "Config file must have valid extension of .conf" << std::endl;
    return 0;
}
    