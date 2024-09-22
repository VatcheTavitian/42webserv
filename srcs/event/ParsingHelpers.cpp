
#include "ParsingHelpers.hpp"

void    initialiseHeaders(std::map<std::string, std::string>& headers) {
    headers["Method"] = "";
    headers["Host"] = "";
    headers["Port"] = "";
    headers["Path"] = "";
    headers["ContentDisposition"] = "";
    headers["ContentLength"] = "";
    headers["Cookie"] = "";
}

void    getMethod(std::map<std::string, std::string>& headers, char** splitBuffer, int i) {
    (void) i;
    if (headers["Method"].empty()) {
        for (int j = 0; (splitBuffer[0][j] && splitBuffer[0][j] != ' ') ; j++)
            headers["Method"] += splitBuffer[0][j];
    }
    else {
        return ;
    }
}

void    getCookie(std::map<std::string, std::string>& headers, char** splitBuffer, int i) {
    if (headers["Cookie"].empty()) {
        int j = 0;

        while (j < i) {
            if ((strncmp("Cookie:", splitBuffer[j], strlen("Cookoe:"))) == 0 ) {
                int x = strlen("Cookie: ");
                std::string Cookie = "";
                while (splitBuffer[j][x] && splitBuffer[j][x] != '\n') {
                    Cookie += splitBuffer[j][x];
                    x++;
                }
                headers["Cookie"] = Cookie;
                break ;
            }
            j++;
        }
    }
    else {
        return ;
    }
}

void    getFileToDelete(std::string& fileToDeleteName, char** splitBuffer, int i) {
    int j = 0;

    if  (fileToDeleteName.empty()) { 
        while (j < i) {
            if ((strncmp("File-To-Delete:", splitBuffer[j], strlen("File-To-Delete:"))) == 0 ) {
                int x = strlen("File-To-Delete: ");
                while (splitBuffer[j][x] && splitBuffer[j][x] ) {
                    if (splitBuffer[j][x] == '\r' || splitBuffer[j][x] == '\n')
                        break ;
                    fileToDeleteName += splitBuffer[j][x];
                    x++;
                }
            }
            j++;
        }   
    }
}

void    getContentDisposition(std::map<std::string, std::string>& headers, char** splitBuffer, int i) {
    if (headers["ContentDisposition"].empty()) {
        int j = 0;

        while (j < i) {
            if ((strncmp("Content-Disposition:", splitBuffer[j], strlen("Content-Disposition:"))) == 0 ) {
                int x = strlen("Content-Disposition: ");
                std::string ContentDispStr = "";
                while (splitBuffer[j][x] && splitBuffer[j][x] != '\n') {
                    ContentDispStr += splitBuffer[j][x];
                    x++;
                }
                headers["ContentDisposition"] = ContentDispStr;
                break ;
            }
            j++;
        }
    }
    else {
        return ;
    }
}

void    getPath(std::map<std::string, std::string>& headers, char** splitBuffer, int i) {
    int j = 0;

    if (!(headers["Method"].empty())) {
        const char* method = headers["Method"].c_str();
        while (j < i) {
            if (strncmp(method, splitBuffer[j], strlen(method)) == 0 
                && splitBuffer[j][strlen(method)] == ' ' ) {
                int x = strlen(method);
                x++;
                std::string path = "";
                while (splitBuffer[j][x] && splitBuffer[j][x] != ' ') {
                    path += splitBuffer[j][x];
                    x++;
                }
                headers["Path"] = path;
                break ;
            }
            
            j++;
        }
    }
}

void    getHost(std::map<std::string, std::string>& headers, char** splitBuffer, int i) {
    int j = 0;

    if (headers["Host"].empty()) {
        while (j < i) {
            if ((strncmp("Host: ", splitBuffer[j], strlen("Host: ")) == 0))
            {

                int x = strlen("Host: ");
                std::string host = "";
                while (splitBuffer[j][x] && (splitBuffer[j][x] != ' ' && splitBuffer[j][x] != ':' \
                    && splitBuffer[j][x] != '\n' && splitBuffer[j][x] != '\r')){
                    host += splitBuffer[j][x];
                    x++;
                }
                headers["Host"] = host;
                break ;
            }
            j++;
        }
    }
}




void    getPort(std::map<std::string, std::string>& headers, char** splitBuffer, int i) {
    int j = 0;

    if (!(headers["Method"].empty())) {
        std::string hostFull = "Host: " + headers["Host"] + ":";
        while (j < i) {
            if ((strncmp(hostFull.c_str(), splitBuffer[j], strlen(hostFull.c_str())) == 0 
                && isdigit(splitBuffer[j][strlen(hostFull.c_str())]))) {
                int x = strlen(hostFull.c_str());
                std::string port = "";
                while (splitBuffer[j][x] && splitBuffer[j][x] != ' ') {
                    port += splitBuffer[j][x];
                    x++;
                }
                headers["Port"] = port;
                break ;
            }
            j++;
        }
    } 
}

void clearBuffer(char** splitBuffer, int i) {
    for (int j = 0; j < i ; j++)
        free(splitBuffer[j]);
    free(splitBuffer);
}