# Web Server Project - 42 Webserv

## Overview
This project is a basic web server implementation as part of the webserv project of Ecole 42 with bonus.
It is a non-blocking server using select() method.
It is designed to handle HTTP requests and serve static files.
Specifics are defined in the ProjectInstruction pdf file.
This server is not secure and not recommended for use in production.

## Features
- Handles GET and POST requests
- Serves static files from a specified directory
- Upload and Delete files

## Installation
1. Clone the repository:
	```sh
	git clone https://github.com/VatcheTavitian/42webserv.git
	```
2. Navigate to the project directory:
	```sh
	cd 42webserv
	```

3. !! If you are using Linux, open Makefile and uncomment line LINUX = -lpthread

4. Run make:
	```sh
	make
	```

## Usage
1. Start the server with default config file located in /configs/default.conf:
	```sh
	./webserv
	```
	or to specify your own config file:
	```sh
	./webserv <path to config>
	```
2. Open your web browser and navigate to `http://localhost:<port you defined>`.

3. Quit the server by pressing Ctrl + C;


## Important Notes!
Configuration files must follow specific rules.
For details refer to /configs/configInfo.conf

# Upload process information

Uploading of files must be done with base64 encoding.
This is because we found when uploading large files and simply extracting the content within the HTTP request 'boundary' was not
sufficient to ensure the integrity of the data.
As such, as per JS uploader.js file, the file content is encoded and base64 before sending the request to the webserver.
The server then receives and decodes the content in the handleUpload() function.
Please see upload.html and uploader.js files.

During testing we found that uploading large files can stall/delay the server.
This is because the server utilises base64 decoding function which can consume resources.
As such, if the payload size is found to be greater than 10000 bytes, a separate thread is launched to minimise 
the stalling affect large files have during upload.
Consult handleUpload() and handleUploadWithThread() functions in Request.cpp

# Directory listings
The serveDirectoryPages() function looks for 2 boiler-plate files in the root directory.
This allows you to splice the directory information the server obtains in between html code so that you can maintain the theme/styling of your website.
Boiler plate 1 called boiler-plate1.html should have the first half of html page you want to display up to the directory listing
Boiler plate 2 called boiler-plate2.html should have the second half of the html page you want to display after the directory listing

## Contact
For any questions or suggestions, please feel free to reach out