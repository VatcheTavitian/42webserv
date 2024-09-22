#include "Request.hpp"
#include "base64.hpp"

Request::Request() {

};

Request::Request(std::vector<Server> servers, std::string buffer, Server* listeningserver) : servers(servers), buffer(buffer), listeningserver(listeningserver) {
    std::time_t now = std::time(0);
    fileToDeleteName ="";
    char* dt = std::ctime(&now);
    dt[strlen(dt) - 1] = '\0';
    RESET = "\033[0m";
    GREEN = "\033[32m";
    BLUE = "\033[34m";
    parseHeaders();
    if (!this->headers["Method"].empty())
        std::cout << GREEN << this->listeningserver->ipAddress << " - - [" << dt << "] \"" << this->headers["Method"] << " " << this->headers["Path"] <<"\"" << RESET << std::endl;
};

void    Request::getFileName() {
    std::string contentDispositionStr = this->headers["ContentDisposition"];
    size_t pos = contentDispositionStr.find("filename=");
    if (pos != std::string::npos) {
        pos += strlen("filename=") ;
        fileName = contentDispositionStr.substr(pos);
        if (!fileName.empty() && fileName[0] == '"') {
            size_t endQuotePos = fileName.find('"', 1);
            if (endQuotePos != std::string::npos) {
                this->fileName = fileName.substr(1,endQuotePos - 1);
            } 
        }
    } else {
        this->fileName = "nofilenamefound";
    }
}

bool    Request::deleteConditionsMet() {
    if (currentClientPtr->listeningserver->_locationIdentified) {
        Location *tmp = currentClientPtr->listeningserver->_locationIdentified;
        if (tmp->_permitDelete && !tmp->_uploadDir.empty())
            return true;
    }
    return false;
}

void    Request::handleDelete() {
    if (currentClientPtr->isMethodAllowed()) {
        try {
            if (deleteConditionsMet() &&!fileToDeleteName.empty()) {
                Location *tmp = currentClientPtr->listeningserver->_locationIdentified;
                std::string filePath = "." + tmp->_root + tmp ->_uploadDir + "/" + fileToDeleteName;
                FILE* fileCheck = std::fopen( filePath.c_str(), "r");
                if (fileCheck) {
                    if (std::remove(filePath.c_str()) == 0) {
                        currentClientPtr->clientStatusCode = 200;
                        std::cout << "File '" << fileToDeleteName << " has been successfully deleted" << std::endl;
                    }
                    else {
                        currentClientPtr->clientStatusCode = 500;
                        throw std::runtime_error("Unable to delete " + fileToDeleteName);
                    }
                }
                else {
                    currentClientPtr->clientStatusCode = 412;
                    throw std::runtime_error("File Doesn't Exist");
                }
            }
            if (!deleteConditionsMet()) {
                currentClientPtr->clientStatusCode = 500;
                throw std::runtime_error("Unable to delete due to server settings!");
            } else if (fileToDeleteName.empty()) {
                currentClientPtr->clientStatusCode = 500;
                throw std::runtime_error("No file name found to delete");
            }
        } catch (const std::runtime_error& e){
                std::cout << "Exception found: " << e.what() << std::endl;
        }
    }
    else {
        currentClientPtr->clientStatusCode = 405;
    }
}

// UPLOAD MANAGEMENT FOR THE SERVER - refer uploadInstructions.txt for more details
void    Request::handleUpload() {
    try {
        getFileName();
        if (this->fileName == "nofilenamefound")
            throw std::runtime_error("Error with filename");
        size_t payloadStart = buffer.find("\r\n\r\n");
        if (payloadStart != std::string::npos) {
            payloadStart += 4;
            this->base64String = buffer.substr(payloadStart);
        }
        std::string decoded = base64_decode(this->base64String);
        writeBufferToFile(this->fileName, decoded);
        decoded.clear();
        decoded.swap(EMPTYBUFFER);
        base64String.clear();
        base64String.swap(EMPTYBUFFER);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error decoding Base64 string: " << e.what() << std::endl;
        throw std::runtime_error("Error decoding Base64 string");
    } catch (const std::runtime_error& e){
        std::cout << "No file name found on file upload attempt: " << e.what() << std::endl;
        currentClientPtr->clientStatusCode = 500;
    }
}

// For uploading in main thread
void Request::writeBufferToFile(const std::string& filename, const std::string& decoded) {
    std::string uploadDirectory = "." + this->listeningserver->_locationIdentified->_root + this->listeningserver->_locationIdentified->_uploadDir;
    std::ofstream outputFile((uploadDirectory + "/" + filename).c_str()); 
    if (outputFile.is_open()) {
        outputFile << decoded << std::endl; 
        outputFile.close();
        std::cout << GREEN << "File '" << filename << "' uploaded successfully." << RESET << std::endl;
    } else {
        std::cerr << "Unable to open file: " << filename << "\nCheck path to write to exists" << std::endl;
        this->currentClientPtr->clientStatusCode = 500;
    }
}

// For uploading in a separate thread
void writeBufferToFile(const std::string& filename, const std::string& decoded) {
    std::ofstream outputFile(("./" + filename).c_str()); 
    std::string RESET = "\033[0m";
    std::string RED = "\033[31m";
    std::string GREEN = "\033[32m";
    if (outputFile.is_open()) {
        outputFile << decoded << std::endl; 
        outputFile.close();
        std::cout << GREEN << filename << "' uploaded successfully.\n[Conducted via thread]" << RESET << std::endl;
    } else {
        std::cerr << RED << "Unable to open file to upload...: " << filename << "\nCheck path to write to exists" << RESET << std::endl;
    }
}

void* uploadThread(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);
    std::string decoded = base64_decode(*(data->base64String));
    writeBufferToFile(data->filePath, decoded);
    delete data->base64String; 
    delete data;               
    
    return NULL;
}

void    Request::handleUploadWithThread() {
    try {
        getFileName();
        if (this->fileName == "nofilenamefound")
            throw std::runtime_error("Error with filename");
        size_t payloadStart = buffer.find("\r\n\r\n");
        if (payloadStart != std::string::npos) {
            payloadStart += 4;
            std::string* base64String = new std::string(buffer.substr(payloadStart));
            ThreadData* data = new ThreadData;
            data->base64String = base64String;
            data->filePath = this->listeningserver->_locationIdentified->_root + this->listeningserver->_locationIdentified->_uploadDir + "/" + this->fileName;

            pthread_t thread;
            if (pthread_create(&thread, NULL, uploadThread, (void*)data) != 0) {
                std::cerr << "Error creating thread\n";
                delete base64String;
                delete data;
            }
            pthread_detach(thread);
        }
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error decoding Base64 string: " << e.what() << std::endl;
         currentClientPtr->clientStatusCode = 500;
        throw std::runtime_error("Error decoding Base64 string");
    } catch (const std::runtime_error& e){
        std::cout << "No file name found on file upload attempt: " << e.what() << std::endl;
        currentClientPtr->clientStatusCode = 500;
    }
}


void    Request::parseHeaders() {
    char** splitBuffer = ft_split(buffer.c_str(), '\n');
    int i = 0;

    for (; splitBuffer[i]; i++) {
    }
    initialiseHeaders(this->headers);

    if (splitBuffer[0] != NULL) {
        getMethod(this->headers, splitBuffer, i);
        if (this->headers["Method"] == "DELETE")
            getFileToDelete(this->fileToDeleteName, splitBuffer, i);
        getPath(this->headers, splitBuffer, i);
        getHost(this->headers, splitBuffer, i);
        getPort(this->headers, splitBuffer, i);
        if (this->headers["Port"].empty()) {
            std::stringstream ss;
            ss << listeningserver->port;
            this->headers["Port"] = ss.str();
        }
        getCookie(this->headers, splitBuffer, i);
        getContentDisposition(this->headers, splitBuffer, i);
    }       
    clearBuffer(splitBuffer, i);
}


Request::~Request()
{
    fileName.clear();
    base64String.clear();
    servers.clear();
    buffer.clear();
    buffer.swap(EMPTYBUFFER);
};



