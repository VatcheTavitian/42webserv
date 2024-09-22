#include "Response.hpp"
#include <signal.h>
#include <unistd.h>
#include <ctime>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/types.h>
#include <iostream>
#include <string>

Response::Response() {
}

template <typename T> std::string Response::to_string(T value) {
    std::ostringstream os;
    os << value;
    return os.str();
}

int Response::badHostname() {
    if (this->request->headers["Host"] == "127.0.0.1" || this->request->headers["Host"] == "localhost")
        return 0;

    for (size_t i = 0; i < servers.size() ; i++) {
        if  (servers[i]._serverName == this->request->headers["Host"])
        if (servers[i]._serverName == this->request->headers["Host"] && servers[i].port == std::atoi(this->request->headers["Port"].c_str()))        
            return 0;
    }
    return 1;
}

bool Response::isMethodAllowed() {
    if (this->listeningserver->_locationIdentified) {
        if (this->listeningserver->_locationIdentified->_allowedMethods.size() > 0) {
            std::vector<std::string>::iterator it = this->listeningserver->_locationIdentified->_allowedMethods.begin();
            while (it != this->listeningserver->_locationIdentified->_allowedMethods.end()) {
                if (*it == this->request->headers["Method"]) {
                    return true;
                }
                it++;
            }
        }
    } else if (this->listeningserver->_allowedMethods.size() > 0) {
            std::vector<std::string>::iterator it = const_cast<Server *>(this->listeningserver)->_allowedMethods.begin();
            while (it != this->listeningserver->_allowedMethods.end()) {
                if (*it == this->request->headers["Method"]) {
                    return true;
                }
                it++;
            }
        }
    return false;
}


Response::Response(Request *request, std::vector<Server>& servers,  int i, const Server* listeningserver) : request(request), servers(servers), listeningserver(listeningserver) {
   
    this->i = i;
    statusCode = 200;
    if (request->headers["Method"].empty() || request->headers["Path"].empty() || badHostname() == 1) {
        deliverErrorPage(i, 400, const_cast<Server *>(listeningserver)); // check this
        return ;
    }
}

void    Response::writeStamp() {
        std::time_t now = std::time(0);
        char* dt = std::ctime(&now);
        dt[strlen(dt) - 1] = '\0';
            
        RESET = "\033[0m";
        RED = "\033[31m";
        BLUE = "\033[34m";
        if (this->statusCode >= 400 && this->statusCode <= 600)
            std::cout << RED << this->listeningserver->ipAddress << " - - [" << dt << "]" << " Server responded with [" << this->statusCode << "]" << RESET << std::endl;
        else
            std::cout << BLUE << this->listeningserver->ipAddress << " - - [" << dt << "]" << " Server responded with [" << this->statusCode << "]" << RESET << std::endl;
}

std::string Response::adjustRootAndPath(Location* &result) {
    std::string file_open = "";
    std::string coreLocation = "";
    std::string path = this->request->headers["Path"];
    size_t x = 0;
    coreLocation += path[x];
    x++;
    while (path[x] && path[x] != '/') {
        coreLocation += path[x];
        x++;
    }
    for (size_t i = 0; i < this->listeningserver->_location.size(); i++) {
        if (coreLocation == this->listeningserver->_location[i]._location) {
            result = const_cast<Location *>(&this->listeningserver->_location[i]);
            file_open += "." + this->listeningserver->_location[i]._root + this->listeningserver->_location[i]._locationIndex + (path.substr(x, path.length() - x));
        }
    }
    coreLocation.clear();
    coreLocation.swap(EMPTYBUFFER);
    path.clear();
    path.swap(EMPTYBUFFER);
    return file_open;
}

bool    Response::baseLocationValid() {
    std::string finalLocation = "";
    std::string path = this->request->headers["Path"];
    size_t i = 0;
    finalLocation += path[i];
    i++;
    if (!path.empty()) { 
        while (path[i] && path[i] != '/') {
            finalLocation += path[i];
            i++;
        }
        for (size_t i = 0; i < this->listeningserver->_location.size(); i++) {
            if (finalLocation == this->listeningserver->_location[i]._location)
            {
                finalLocation.clear();
                finalLocation.swap(EMPTYBUFFER);
                return true ;
            }
        }
    }
    path.clear();
    path.swap(EMPTYBUFFER);
    finalLocation.clear();
    finalLocation.swap(EMPTYBUFFER);
    return false;
}

bool    Response::locationFinal() {
    std::string finalLocation = "";
    std::string path = this->request->headers["Path"];
    size_t i = 0;
    finalLocation += path[i];
    i++;
    while (path[i] && (path[i] != '/' && path[i] != ' ')) {
        finalLocation += path[i];
        i++;
    }
    if (path[i]) {
        if (path[i] == '/') {
            i++;
            if (!path[i]) {
                path.clear();
                path.swap(EMPTYBUFFER);
                finalLocation.clear();
                finalLocation.swap(EMPTYBUFFER);
                return true;
            }
        }
        path.clear();
        path.swap(EMPTYBUFFER);
        finalLocation.clear();
        finalLocation.swap(EMPTYBUFFER);
        return false;
    }
    path.clear();
    path.swap(EMPTYBUFFER);
    finalLocation.clear();
    finalLocation.swap(EMPTYBUFFER);
    return true;
}

bool Response::notCGI() {
    for (size_t i = 0; i < this->listeningserver->_location.size(); i++) {
        if (this->listeningserver->_location[i]._location == "/cgi-bin") {
            if (this->listeningserver->_location[i]._cgi.find(getExtType(this->request->headers["Path"])) !=
                this->listeningserver->_location[i]._cgi.end())
                return false;
        }
    }
    return true;
}

void    Response::assignResult(Location* &result) {
    std::string finalLocation = "";
    std::string path = this->request->headers["Path"];
    size_t i = 0;
    finalLocation += path[i];
    i++;
    while (path[i] && (path[i] != '/' && path[i] != ' ')) {
        finalLocation += path[i];
        i++;
    }
    for (size_t i = 0; i < this->listeningserver->_location.size(); i++) {
        if (finalLocation == this->listeningserver->_location[i]._location) {
            result = const_cast<Location *>(&this->listeningserver->_location[i]);
        }
    }
    path.clear();
    path.swap(EMPTYBUFFER);
    finalLocation.clear();
    finalLocation.swap(EMPTYBUFFER);
}

std::string Response::getCoreLocation() {
    std::string coreLocation = "";
    std::string path = this->request->headers["Path"];
    size_t x = 0;
    coreLocation += path[x];
    x++;
    while (path[x] && path[x] != '/') {
        coreLocation += path[x];
        x++;
    }
    path.clear();
    path.swap(EMPTYBUFFER);
    return coreLocation;
}


void    Response::deliverErrorPage(int fd, int statusCode, Server* queriedServer) {
    Location result;
    std::string coreLocation = "";
    if (baseLocationValid())
        coreLocation = getCoreLocation();
    if (!notCGI()) {
        for (size_t i = 0; i < listeningserver->_location.size(); i++) {
            if (listeningserver->_location[i]._location == "/cgi-bin") 
                result = listeningserver->_location[i];
            }
    } else {
        for (size_t i = 0; i < this->listeningserver->_location.size(); i++) {
            if (!coreLocation.empty() && coreLocation == this->listeningserver->_location[i]._location) {
                result = this->listeningserver->_location[i];
                break ;
            }
            if (this->request->headers["Path"] == this->listeningserver->_location[i]._location) {
                result = this->listeningserver->_location[i];
                break ;
            }
        }
    }

    this->statusCode = statusCode;
    std::stringstream sCodeStr;
    sCodeStr << statusCode;
   
   
    try {
        std::string locationErrorPath;
        if (locationHasErrorPages(result, this->statusCode))
            locationErrorPath = "." + result._root + result._errorPage[this->statusCode];
        else 
            locationErrorPath = "." + listeningserver->_root + const_cast<Server*>(listeningserver)->_errorPage[statusCode];
        DIR *dir = opendir(locationErrorPath.c_str());
        if (dir) {
            closedir(dir);
            throw std::runtime_error("Location is directory - serve default error page");
        }

        std::string file_open(locationErrorPath);
        std::ifstream file(file_open.c_str());;
        if (!file.is_open() || file_open == ("." + result._root))
            throw std::runtime_error("Could not open file");
        std::ostringstream buffer;
        buffer << file.rdbuf();
        std::string fileContent = buffer.str();
        std::string mimeType = getContentType(request->headers["Path"]);
        std::string response = "HTTP/1.1 "+ sCodeStr.str() + " " + queriedServer->statusCodes[sCodeStr.str()].c_str() +"\r\n";
        response += "Content-Type: " + mimeType + "\r\n";
        response += "Content-Length: " + this->to_string(fileContent.size()) + "\r\n";
        response += "Connection: close\r\n";
        if (!this->request->headers["Cookie"].empty())
            response += "Set-Cookie: " + this->request->headers["Cookie"] + "\r\n";
        response += "\r\n";
        response += fileContent;
        send(fd, response.c_str(), response.size(), 0);
        fileContent.clear();
        mimeType.clear();
        response.clear();
        fileContent.swap(EMPTYBUFFER);
        mimeType.swap(EMPTYBUFFER);
        response.swap(EMPTYBUFFER);
    } catch (const std::runtime_error& e) {
        std::string fileContent = "<h1>DEFAULT SERVER RESPONSE </h1><br><p>" + sCodeStr.str() + " ERROR PAGE</p><br><p>" +  
            queriedServer->statusCodes[sCodeStr.str()].c_str() + "</p>";
        std::string mimeType = getContentType(request->headers["Path"]);
        std::string response = "HTTP/1.1 "+sCodeStr.str() + " " + queriedServer->statusCodes[sCodeStr.str()].c_str() +"\r\n";
        response += "Content-Type: " + mimeType + "\r\n";
        response += "Content-Length: " + this->to_string(fileContent.size()) + "\r\n";
        response += "Connection: close\r\n"; 
        if (!this->request->headers["Cookie"].empty())
            response += "Set-Cookie: " + this->request->headers["Cookie"] + "\r\n";
        response += "\r\n";
        response += fileContent;
        send(fd, response.c_str(), response.size(), 0);
        fileContent.clear();
        mimeType.clear();
        response.clear();
    }
    sCodeStr.clear();

}

std::string Response::getContentType(const std::string& fileName) {
    size_t pos = fileName.find_last_of(".");
    std::string ext = fileName.substr(pos + 1); 
    if (pos == std::string::npos)
        return "text/html";
    else if (ext == "html" || ext == "htm")
        return "text/html";
    else if (ext == "jpg" || ext == "jpeg") 
        return "image/jpeg";
    else if (ext == "ico")
        return "image/x-icon"; 
    else if (ext == "png")
        return "image/png";
    else if (ext == "gif")
        return "image/gif";
    else if (ext == "css") 
        return "text/css";
    else if (ext == "js")
        return "application/javascript";
    else if (ext == "pdf")
        return "application/pdf";
    else if (ext == "mp4")
        return "video/mp4";
    return "text/html";
}

std::string Response::getExtType(const std::string& fileName) {
    size_t pos = fileName.find_last_of(".");
    std::string ext = fileName.substr(pos + 1); 
    if (pos == std::string::npos)
        return "";
    else
        return ext;
}

bool Response::locationHasErrorPages(Location& result, int statusCode) {
    if (!result._errorPage[statusCode].empty())
        return true;
    return false;
}

void Response::handleRedirect(Location& result) {
    this->statusCode = 301;
    (void) result;
    std::string response = "HTTP/1.1 " + this->to_string(this->statusCode) + " OK\r\n";
    response += "Location: " + result._redirect + " \r\n";
    response += "Expires: 0\r\n";
    response += "Connection: close \r\n";
    if (!this->request->headers["Cookie"].empty())
            response += "Set-Cookie: " + this->request->headers["Cookie"] + "\r\n";
       response += "\r\n";
    send(i, response.c_str(), response.size(), 0);
    response.clear();
}


void Response::urlDecodeSpace(std::string& file_open) {
    size_t pos = 0;
    std::string search = "%20";
    while ((pos = file_open.find(search, pos)) != std::string::npos)
        file_open.replace(pos, search.length(), " ");
}

void Response::serveDirectoryPages(std::string filePath, Location* result) {
    (void) result;
    std::string preContent;
    std::string postContent;
   
    struct dirent* directory;
    DIR *dir = opendir(filePath.c_str());
    if (!dir) {
        std::cout << "Cannot open directory" << std::endl;
    }
    if (dir) {
        // This section is in case you want to place directory listing in a custom html file
        // For it to work, ensure you have boiler-plate1.html and boiler-plate2.html in the root directory
        // Boiler plate 1 should have the first half of html page you want to display up to the directory listing
        // Boiler plate 2 should have the second half of the html page you want to display after the directory listing
        std::ifstream b1(("." + listeningserver->_root + "/boiler-plate1.html").c_str());
        std::ifstream b2(("." + listeningserver->_root + "/boiler-plate2.html").c_str());
        if (b1.is_open() && b2.is_open()) {
            std::ostringstream fileContent1;
            std::ostringstream fileContent2;
            fileContent1 << b1.rdbuf();
            fileContent2 << b2.rdbuf();
            preContent += fileContent1.str();
            postContent += fileContent2.str();
            b1.close();
            b2.close();
        }
        else {
            std::cout << "Note: Could not open boiler-plate files - refer to config file instructions" << std::endl;
            preContent += "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<title>Directory of " + this->request->headers["Path"] + "</title>\n";
            preContent += "</head>\n<body>\n";
            postContent += "</table>\n</body>\n</html>";
        }
        preContent += "<h1>Directory of " + this->request->headers["Path"] + "</h1>\n<table>\n";
        while ((directory = readdir(dir)) != NULL) {
            if (strcmp(directory->d_name, ".") != 0 && strcmp(directory->d_name, "..") != 0) {
                preContent += "\n<tr>\n<td>\n<li><a href='" + this->request->headers["Path"] + "/";
                preContent +=  directory->d_name;
                preContent +=  "'>\n";
                preContent +=  directory->d_name;
                preContent += "\n</a>\n</td>\n</tr>\n";
            }
        }
        preContent += postContent;
        closedir(dir);
        std::string mimeType = "text/html";
        std::string response = "HTTP/1.1 " + this->to_string(this->statusCode) + " OK\r\n";
        response += "Content-Type: " + mimeType + "\r\n";
        response += "Content-Length: " + this->to_string(preContent.size()) + "\r\n";
        response += "Connection: close\r\n"; 
        if (!this->request->headers["Cookie"].empty())
            response += "Set-Cookie: " + this->request->headers["Cookie"] + "\r\n";
        response += "\r\n";
        response += preContent; 
        send(i, response.c_str(), response.size(), 0);
        preContent.clear();
        mimeType.clear();
        response.clear();
        preContent.swap(EMPTYBUFFER);
        mimeType.swap(EMPTYBUFFER);
        response.swap(EMPTYBUFFER);
    }
}


std::string Response::allocateFileName(std::string path) {
    size_t pos =  path.find_last_of("/");
    std::string filename = path.substr(pos + 1, path.length());
    if (filename.length() == 0)
        filename = "nofilenamefound";
    return filename;
}

void Response::sendBinaryFile(std::string& file_open) {
    std::ifstream file(file_open.c_str(), std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file");
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);
    if (!file.read(&buffer[0], size)) {
        throw std::runtime_error("Failed to read file");
    }

    std::string mimeType = getContentType(request->headers["Path"]);
    std::string response = "HTTP/1.1 " + this->to_string(this->statusCode) + " OK\r\n";
    response += "Content-Type: " + mimeType + "\r\n";
    response += "Content-Length: " + this->to_string(size) + "\r\n";
    response += "Content-Disposition: attachment; filename=\"" + allocateFileName(request->headers["Path"]) + "\"\r\n";
    response += "Connection: keep-alive\r\n";
    if (!this->request->headers["Cookie"].empty()) {
        response += "Set-Cookie: " + this->request->headers["Cookie"] + "\r\n";
    }
    response += "\r\n";

    ssize_t total_sent = 0;
    ssize_t totalToSend = size + response.size();

    try {
        ssize_t byte_sent = send(this->request->currentClientPtr->socket, response.c_str(), response.size(), 0);
        if (byte_sent == -1) {
            throw std::runtime_error("Failed to send headers: " + std::string(strerror(errno)));
        }
        total_sent += byte_sent;

        while (total_sent < totalToSend) {
            ssize_t byte_sent = send(this->request->currentClientPtr->socket, &buffer[0] + (total_sent - response.size()), totalToSend - total_sent, 0);
            if (byte_sent == -1) {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                    continue;
                else
                    throw std::runtime_error("Failed to send content: " + std::string(strerror(errno)));
            }

            if (byte_sent == 0)
                break;

            total_sent += byte_sent;
            this->request->currentClientPtr->processed += byte_sent;
        }

        if (total_sent < totalToSend) {
            std::cout << "Failed to send all data. Total sent: " << total_sent << std::endl;
        }
        
    } catch (const std::runtime_error& e) {
        std::cout << "Exception thrown: " << e.what() << std::endl;
    }

    this->request->currentClientPtr->transferInProgress = false;
}


void Response::servePage() {
    Server* queriedServer = const_cast<Server*>(listeningserver);
    if (!this->isMethodAllowed() && notCGI()) {
        deliverErrorPage(i, 405, const_cast<Server*>(listeningserver));
        writeStamp();
        return ;
    }
    
    try {
        if (notCGI()) {
            DIR *testIfDir = NULL;
            std::string file_open = "";
            Location *result = NULL;
            if (baseLocationValid() && locationFinal()) {
                assignResult(result);
                if (result != NULL){
                    if (result && (!result->_redirect.empty())) {
                        handleRedirect(*result);
                        return ;
                    }
                    else {
                        file_open = "." + result->_root + result->_locationIndex;
                    }
                }
            }
            else if (baseLocationValid() && !locationFinal()) {
               file_open = adjustRootAndPath(result);

            }
            else if (request->headers["Path"] == "/") 
                file_open = "." + listeningserver->_root + listeningserver->_index;
            else 
                file_open = "." + queriedServer->_root + request->headers["Path"];
            urlDecodeSpace(file_open);
            
            if (result && result->_listDirectory && (request->headers["Path"] != "/")) {
                testIfDir = opendir(file_open.c_str());
                if (testIfDir) {
                    serveDirectoryPages(file_open, result);
                    closedir(testIfDir);
                } 
            }
            if (!testIfDir){
                if (request->headers["Path"] != "/")
                    testIfDir = opendir(file_open.c_str());
                if (testIfDir) {
                        closedir(testIfDir);
                        testIfDir = NULL;
                        this->statusCode = 403;
                        throw std::runtime_error("Could not open file");  
                }
                std::ifstream file(file_open.c_str());
                if (!file.is_open()) {
                    this->statusCode = 404;
                    throw std::runtime_error("Could not open file");  
                }
                std::string extType = getExtType(file_open);
                if ((extType != "pdf" && extType !="mp4" && extType !="zip" ) || this->request->currentClientPtr->payloadSize > 1000000) {
                    std::ostringstream buffer;
                    buffer << file.rdbuf();
                    std::string fileContent = buffer.str();
                    std::string mimeType = getContentType(request->headers["Path"]);
                    std::string response = "HTTP/1.1 " + this->to_string(this->statusCode) + " OK\r\n";
                    response += "Content-Type: " + mimeType + "\r\n";
                    response += "Content-Length: " + this->to_string(fileContent.size()) + "\r\n";
                    response += "Connection: close\r\n"; 
                    if (!this->request->headers["Cookie"].empty())
                        response += "Set-Cookie: " + this->request->headers["Cookie"] + "\r\n";
                    response += "\r\n";
                    response += fileContent;
                    send(i, response.c_str(), response.size(), 0);
                    file_open.clear();
                    fileContent.clear();
                    mimeType.clear();
                    response.clear();
                    fileContent.swap(EMPTYBUFFER);
                    mimeType.swap(EMPTYBUFFER);
                    response.swap(EMPTYBUFFER);
                    file_open.swap(EMPTYBUFFER);
                } else {
                    sendBinaryFile(file_open);
                }
            }
        }
        else if (!notCGI())
            this->runCGI();
    }
    catch (const std::runtime_error& e) {
        deliverErrorPage(i, this->statusCode, queriedServer);
    }
    writeStamp();
}

void handle_timeout(int) {
    std::cerr << "Execution timed out" << std::endl;
    strerror(errno);
    _exit(EXIT_FAILURE);  
}

void Response::runCGI() {
    std::string cgiPath = "";
    std::string rootPath = "";
    std::string filePath = "";
    const unsigned int timeout = 5;
 
    const std::map<std::string, std::string>* cgiInfo = NULL;  

    try{
        for (size_t i = 0; i < this->listeningserver->_location.size(); i++) {
        if (this->listeningserver->_location[i]._location == "/cgi-bin") {
            cgiInfo = &this->listeningserver->_location[i]._cgi;
            Location* nonConstLocation = const_cast<Location*>(&this->listeningserver->_location[i]);
            std::vector<std::string>::iterator it1 = std::find(nonConstLocation->_allowedMethods.begin(),
                                                                  nonConstLocation->_allowedMethods.end(),
                                                                  this->request->headers["Method"]);
                if (it1 == nonConstLocation->_allowedMethods.end()) {
                    this->statusCode = 405;
                    throw std::runtime_error("Method not allowed");
                }


            std::string extType = getExtType(this->request->headers["Path"]);
            std::map<std::string, std::string>::const_iterator it = cgiInfo->find(extType);  
            if (it != cgiInfo->end()) {
                cgiPath = it->second;
                if (cgiPath.empty()) 
                    throw std::runtime_error("Server not correctly configured for CGI");
                rootPath = this->listeningserver->_location[i]._root + this->listeningserver->_location[i]._locationIndex;
                filePath = "." + rootPath + this->request->headers["Path"]; 
            }
            break; 
        }
    }

    if (!cgiInfo) 
        throw std::runtime_error("Server not configured for CGI");
  

    std::string file_open(filePath);
  
    std::ifstream file(filePath.c_str());
    if (!file.is_open()) 
        throw std::runtime_error("Could not open file!!");
    std::ostringstream buffer;
    buffer << file.rdbuf();
    
    std::string script = buffer.str();
    std::string output;
    int pipefds[2];
    if (pipe(pipefds) == -1)
        throw std::runtime_error("Issue with creating pipes...");
    int pid = fork();
    if (pid == -1)
        throw std::runtime_error("Issue with creating pipes...");
    else if (pid == 0) {
            signal(SIGALRM, handle_timeout);
            alarm(timeout);
            dup2(pipefds[1], STDOUT_FILENO);  
            const char* binaryPath = cgiPath.c_str(); 
            std::string scriptPathString = filePath;
            const char* scriptPath = scriptPathString.c_str();
            const char* args[] = {binaryPath, scriptPath, NULL};
            char** env = NULL;
            execve(binaryPath,  const_cast<char* const*>(args), env);
              std::cerr << "Error executing " << binaryPath << ": " << strerror(errno) << std::endl;
            _exit(EXIT_FAILURE);
        } else {
            close(pipefds[1]);
            char buffer[1024];
            bzero(buffer, sizeof(buffer));
            int bytesRead = 0;
            output += "\n<!DOCTYPE html><html><body><H1>\n";
            while ((bytesRead = read(pipefds[0], buffer, sizeof(buffer) - 1)) > 0) {
                buffer[bytesRead] = '\0';
                output += buffer;
            }
            output += "\n</h1><body></html>\n";
            if (bytesRead == -1)
                std::cout << "Internal Error" << std::endl;
            close(pipefds[0]);
            int status;
            if (waitpid(pid, &status, 0) == -1) 
                throw std::runtime_error("Error waiting for child process");
            int signal = WTERMSIG(status);
            int exitCode = WEXITSTATUS(status);
            if (WIFEXITED(status) || WIFSIGNALED(status)) {
                if (signal == 14) {
                    this->statusCode = 408;
                    throw std::runtime_error("CGI script failed due to child process timeout");
                }
                else if (exitCode != EXIT_SUCCESS) {
                    this->statusCode = 500;
                  throw std::runtime_error("CGI script failed with exit code: " + to_string(exitCode));
                }
            }
        
        std::string mimeType = getContentType(request->headers["Path"]);
        std::string response = "HTTP/1.1 " + this->to_string(this->statusCode) + " OK\r\n";
        response += "Content-Type: " + mimeType + "\r\n";
        response += "Content-Length: " + this->to_string(output.size()) + "\r\n";
        if (!this->request->headers["Cookie"].empty())
        response += "Set-Cookie: " + this->request->headers["Cookie"] + "\r\n";
        response += "Connection: close\r\n";
        response += "\r\n" + output;
        send(this->i, response.c_str(), response.size(), 0);
        script.clear();
        mimeType.clear();
        response.clear();
        script.swap(EMPTYBUFFER);
        mimeType.swap(EMPTYBUFFER);
        response.swap(EMPTYBUFFER);
        }
    } catch (const std::runtime_error& e) {
        std::cout << "CGI Error: " << e.what() << std::endl;
        if (this->statusCode != 502)
            deliverErrorPage(i, this->statusCode, this->request->listeningserver);
        
        else
            deliverErrorPage(i, 502, this->request->listeningserver);
    }
}

Response::~Response() {

}