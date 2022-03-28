#ifndef CLASS_CGI_HPP
# define CLASS_CGI_HPP

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <map>
#include <exception>

#include "../socket/Epoll.hpp"
#include "../socket/Socket.hpp"
#include "../socket/EpollSocket.hpp"

#include "URLDecoder.hpp"

class CGI{
public:
    typedef             std::pair<ServerContext, LocationContext >          serverLocation;
    typedef             std::pair<std::string, std::string>                 clientInfo;

private:
    std::string                         _encodedURL;
    std::string                         _decodedURL;
    std::map<std::string, std::string>  _mapMetaVars;
    ServerContext                       _serverContext;
    LocationContext                     _locationContext;


public:

    CGI(const std::string &url, std::map<std::string, std::string> headers, 
		const serverLocation &serverLocation, const clientInfo &clientInfo,
		const std::string &method): 
    _encodedURL(url),
    _decodedURL(_decodeUrl()),
    _mapMetaVars(),
    _serverContext(serverLocation.first),
    _locationContext(serverLocation.second){
        _setMapEnvVar(headers, serverLocation, clientInfo, method);
    }

    ~CGI(){
    }

    int checkStatus(std::string response){
        int status = 0;
        if (response.find("Status:") != std::string::npos){
            std::string strStatus;
            std::string::iterator begin = response.begin() + (response.find("Status:") + 8);
            std::string::iterator end = begin;
            for (int i = 0; i < 3; i++){
                end++;
            }
            strStatus.append(begin, end);
            std::cout << "strStatus |" << strStatus << "|" <<std::endl;
            return (atoi(strStatus.c_str()));
        }
        return (status);
    }

    std::pair<std::string, int> CGIStartup(){
        char                            readBuffer[1024];
        std::pair<std::string, int>     cgiResponse;
        pid_t                           pid;
        int                             fdsGet[2];
        int                             fdsPost[2];
        int                             childExitStatus = 0;                                    
        char                            **cMetaVar;

        cMetaVar = _createCMetaVar();
        std::cout << "vars " << std::endl;
        for (size_t i = 0;cMetaVar[i]; i++){
            std::cout << "envvar " << cMetaVar[i] << std::endl;
        }

        if (pipe(fdsGet) < 0)
            throw pipeError();
        if (pipe(fdsPost) < 0)
            throw pipeError();
        if ((pid = fork()) == -1)
            throw forkError();

        if (pid == 0)
            childProcess(fdsGet, fdsPost, cMetaVar);

        //reading in pipes
        close(fdsGet[1]);
		bzero(readBuffer, 1024);
        while (read(fdsGet[0], readBuffer, 1024) > 0){
            cgiResponse.first += readBuffer;
            bzero(readBuffer,1024);
        }
		cgiResponse.first += readBuffer;
        close(fdsGet[0]);
		std::cout << "BODYRESULT: " << cgiResponse.first << std::endl;
        int status = checkStatus(cgiResponse.first);
        if (status > 0){
            cgiResponse.second = status;
            return (cgiResponse);
        }
		std::string::iterator begin = cgiResponse.first.begin();
		std::string::iterator end = begin + cgiResponse.first.find("\r\n\r\n") + 4;
		cgiResponse.first.erase(begin, end);
        if ((childExitStatus = waitChild(pid)) > 0)
            throw execveError(childExitStatus);
        for (size_t i = 0; cMetaVar[i]; i++)
            delete [] cMetaVar[i];
        delete [] cMetaVar;
        return (cgiResponse);
    }

    char **_createCMetaVar(){
        char                                                **CMetaVar;
        size_t                                              i;
        std::string                                         join;
        std::map<const std::string, std::string>::iterator  itb;

        itb = this->_mapMetaVars.begin();
        CMetaVar = new char *[this->_mapMetaVars.size() + 1];
        i = 0;
        while(i < this->_mapMetaVars.size()){
            CMetaVar[i] = new char [itb->first.size() + itb->second.size() + 1];
            join = static_cast<std::string>(itb->first).append(itb->second);
            strcpy(CMetaVar[i], join.c_str());
            itb++;
            i++;
        }
        CMetaVar[i] = NULL;
        return (CMetaVar);
    }

    void childProcess(int fdGet[2], int fdPost[2], char **cMetaVar){
        char **args = new char*[3];

        std::string path(this->_serverContext.directives.at("root")[0] + this->_locationContext.args[0]);

        args[0] = strdupa(this->_locationContext.directives.at("cgi_binary")[0].c_str());
        args[1] = strdupa(this->_mapMetaVars.find("SCRIPT_NAME=")->second.c_str());
        args[2] = NULL;

        close(fdGet[0]);
        dup2(fdGet[1], STDOUT_FILENO);
        close(fdGet[1]);

        close(fdPost[0]);
        dup2(fdPost[1], STDIN_FILENO);
        close(fdPost[1]);

        chdir(path.c_str());
        std::cerr << "currentdir " << get_current_dir_name() << " cgi binary: " << args[0]  << " cgi script name: " << args[1] <<  std::endl;
        execve(args[0], args, cMetaVar);
        exit(errno);
    }

    int waitChild(pid_t pid){
        int exitStatus;
        int exitCode;

        exitStatus = 0;
        exitCode = 0;
        waitpid(pid, &exitStatus, 0);
        if (WIFEXITED(exitStatus))
            exitCode = WEXITSTATUS(exitStatus);
        return (exitCode);
    }
    
   class execveError: public std::exception {

    public:
        execveError(int errorCode):
        _errorMessage(strerror(errorCode))
        {}

        virtual ~execveError() throw () {}
        const char* what() const throw () {
            return (_errorMessage.c_str());
        }
    private:
        std::string _errorMessage;

};

    struct pipeError : public std::exception{
        const char *what() const throw(){
            return ("pipe error");
        }
    };

    struct forkError : public std::exception{
        const char *what() const throw(){
            return ("fork error");
        }
    };

    struct chdirError : public std::exception{
        const char *what() const throw(){
            return ("chdir error");
        }
    };

private:
    void    _setMapEnvVar(std::map<std::string, std::string> headers, const serverLocation &serverLocation, 
							const clientInfo &clientInfo, const std::string &method){

        _setServerSoftware();
        _setServerName();
        _setGatewayInterface();
        _setHtppVariables(headers);
        _setServerProtocol();
        _setServerPort(serverLocation.first.directives.find("listen")->second[1]);
        _setRequestUri();
        _setRequestMethod(method);
        _setPathInfo();
        _setPathTranslated();
        _setScriptName();
        _setQueryString();
        _setRemoteHost(clientInfo.second);
        _setRemoteAddr(clientInfo.first);
        _setRemoteUser();
        _setAuthType();
        _setContentType();
        _setContentLength();
        _setRedirectStatus();

    }

    std::string    _decodeUrl(){
        URLDecoder  decoder;

        decoder.decoder(this->_encodedURL);
        return (decoder.getDecodedURL());
    }

    void _setServerSoftware(){
        std::string varName("SERVER_SOFTWARE=");
        std::string nameVersion("WebServ/1.0");

       this->_mapMetaVars.insert(std::make_pair(varName, nameVersion));
    }

    void _setServerName(){
        std::string varName("SERVER_NAME=");
        std::string name("");

        name = this->_serverContext.directives.at("server_name")[0];
        this->_mapMetaVars.insert(std::make_pair(varName, name));
    }

    void _setGatewayInterface(){
        std::string     varName("GATEWAY_INTERFACE=");
        std::string     name("CGI/1.1");

        this->_mapMetaVars.insert(std::make_pair(varName, name));
    }

    void _setServerProtocol(){
        std::string     varName("SERVER_PROTOCOL=");
        std::string     name_version("HTTP/1.1");

        this->_mapMetaVars.insert(std::make_pair(varName, name_version));
    }

    void _setServerPort(std::string port){
        std::string     varName("SERVER_PORT=");

        this->_mapMetaVars.insert(std::make_pair(varName, port));
    }

    void _setRequestMethod(const std::string &method){
        std::string 			varName("REQUEST_METHOD=");
        std::string 			requestMethod(method);

        this->_mapMetaVars.insert(std::make_pair(varName, requestMethod));
    }

    void _setPathInfo(){
        std::string varName("PATH_INFO=");
        std::string pathInfo(this->_mapMetaVars.find("REQUEST_URI=")->second);

		if (pathInfo.find('?') != std::string::npos){
			std::string::iterator begin;
			std::string::iterator end;

			begin = pathInfo.begin() + pathInfo.find('?');
			end = pathInfo.end();
			pathInfo.erase(begin, end);
		}
        this->_mapMetaVars.insert(std::make_pair(varName, pathInfo));
    }

    void _setPathTranslated(){
        std::string varName("PATH_TRANSLATED=");
        std::string pathTranslated("");
		std::string root(this->_serverContext.directives.at("root")[0]);
        std::string uri(this->_mapMetaVars.find("REQUEST_URI=")->second);

		if (*--root.end() == '/')
			root.erase(--root.end());
		pathTranslated = root.append(uri);
		if (pathTranslated.find('?') != std::string::npos){
			std::string::iterator begin;
			std::string::iterator end;

			begin = pathTranslated.begin() + pathTranslated.find('?');
			end = pathTranslated.end();
			pathTranslated.erase(begin, end);
		}
        this->_mapMetaVars.insert(std::make_pair(varName, pathTranslated));
    }

    void _setScriptName(){
        std::string				varName("SCRIPT_NAME=");
        std::string             name("");
        std::string             cgiFile(this->_locationContext.args[0]);
        std::string::iterator   begin;
        std::string::iterator	end;

		begin = this->_decodedURL.begin() + (this->_decodedURL.find(cgiFile) + cgiFile.size());
		end = begin;
		while (*end != '/' && *end != '?' && *end != '\0')
			end++;
		name.append(begin ,end);
        this->_mapMetaVars.insert(std::make_pair(varName, name));
    }

    void _setQueryString(){
        std::string 			varName("QUERY_STRING=");
        std::string 			queryString("");
    

		if (_decodedURL.find('?') != std::string::npos){
			std::string::iterator	begin;
			std::string::iterator	end;
			
			begin = this->_decodedURL.begin() + (this->_decodedURL.find('?') + 1);
			end = begin;
			while (*end != '/' && *end != '\0')
				end++;
			queryString.append(begin, end);
		}
        this->_mapMetaVars.insert(std::make_pair(varName, queryString));
    }

    void _setRemoteHost(const std::string &clientHostName){
        std::string varName("REMOTE_HOST=");
        std::string host(clientHostName);

        this->_mapMetaVars.insert(std::make_pair(varName, host));
    }

    void _setRemoteAddr(const std::string &clientAddr){
        std::string varName("REMOTE_ADDR=");
        std::string addr(clientAddr);

        this->_mapMetaVars.insert(std::make_pair(varName, addr));
    }

    void _setRemoteUser(){
        std::string varName("REMOTE_USER=");
        std::string user("");

        this->_mapMetaVars.insert(std::make_pair(varName, user));
    }

    void _setAuthType(){
        std::string varName("AUTH_TYPE=");
        std::string authType("");
        
        if (_mapMetaVars.count("HTTP_AUTHORIZATION=") == 1){
            std::string 			type(this->_mapMetaVars.find("HTTP_AUTHORIZATION=")->second);
            std::string::iterator	begin;
            std::string::iterator	end;
            
            begin = type.begin() + type.find(' ',0);
            end = type.end();
			type.erase(begin, end);
            authType = type;
        }
        this->_mapMetaVars.insert(std::make_pair(varName, authType));
    }

    void _setContentType(){
        std::string varName("CONTENT_TYPE=");
        std::string contentType("");

        if (_mapMetaVars.count("HTTP_CONTENT_TYPE=") == 1)
            contentType = this->_mapMetaVars.find("HTTP_CONTENT_TYPE=")->second;  
        this->_mapMetaVars.insert(std::make_pair(varName, contentType));
    }

    void _setContentLength(){
        std::string varName("CONTENT_LENGTH=");
        std::string contentLength("");

        if (_mapMetaVars.count("HTTP_CONTENT_LENGTH=") == 1)
            contentLength = this->_mapMetaVars.find("HTTP_CONTENT_LENGTH=")->second;
        this->_mapMetaVars.insert(std::make_pair(varName, contentLength));
    }

    void _setRequestUri(){
        std::string varName("REQUEST_URI=");
        std::string uri(this->_encodedURL);

        this->_mapMetaVars.insert(std::make_pair(varName, uri));
    }
    
    void _setRedirectStatus(){
        std::string varName("REDIRECT_STATUS=");
        std::string status("200");

        this->_mapMetaVars.insert(std::make_pair(varName, status));
    }

    void _setHtppVariables(std::map<std::string, std::string> &headers){
        std::map<std::string, std::string>::iterator itb = headers.begin();
        std::string                                  varName;

        while (itb != headers.end()){
            varName = _formateVarName(itb->first);
            this->_mapMetaVars.insert(std::make_pair(varName, itb->second));
            itb++;
        }
    }

    std::string _formateVarName(std::string varName){
        std::string formated("");
        std::string ret("HTTP_");

        formated = varName;
        for (size_t i = 0; formated[i]; i++){
            if (formated[i] >= 97 && formated[i] <= 122)
                formated[i] -= 32;
            if (formated[i] == '-')
                formated[i] = '_';
        }
        formated.append("=");
        ret.append(formated);
        return(ret);
    }
};




#endif