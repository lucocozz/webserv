#ifndef CLASS_CGI_HPP
# define CLASS_CGI_HPP

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <map>
#include <exception>

#include "../socket/Epoll.hpp"
#include "../socket/Socket.hpp"
#include "../socket/EpollSocket.hpp"

#include "URLDecoder.hpp"


/**
 * constructor: CGI(const SocketInfo ServerInfo, const RequestData &data) 
 * @brief Fill all the CGI datas: _serverInfo, _headerSize, _headerContent, _decodedURL
 * 
 * @param the info to fill the CGI's datas with
 * @return void
 */

/**
 * envs setter:void  create_envs()
 * @brief Creates all the CGI envs with their respective function
 * 
 * @param none
 * @return void
 */

/**
 * CGI startup:void  CGI_startup(struct addrinfo *ServerInfo, std::pair<std::string, int> Header)
 * @brief Init and start the CGI
 * 
 * @param Infos that the CGI needs to set its envs variables
 * @return void
 */

/**
 * decoder: std::string    decodeUrl()
 * @brief Sets the _decodedURL variable with the decode URL
 * 
 * @param Infos None
 * @return Decoded URL as a std::string
 */

class CGI{
public:
    typedef             struct addrinfo             *SocketInfo;
    typedef             std::pair<std::string, int> RequestData;

private:
    //SocketInfo      _clientInfo;
    SocketInfo                          _serverInfo;
    size_t                              _headerSize;
    std::string                         _headerContent;
    std::string                         _encodedURL;
    std::string                         _decodedURL;
    std::string                         _cgiExtension;
    std::string                         _cgiBinary;
    std::map<std::string, std::string>  _mapMetaVars;


public:

    CGI(const SocketInfo ServerInfo, const RequestData &data, std::map<std::string, std::string> headers): 
    _serverInfo(ServerInfo),
    _headerSize(data.second),
    _headerContent(data.first),
    _encodedURL(_setURL()),
    _decodedURL(_decodeUrl()),
    _cgiExtension(".php"),
    _cgiBinary("php-cgi"),
    _mapMetaVars(){
        _setMapEnvVar(headers);
    }

    ~CGI(){
    }

    std::string CGIStartup(){
        char                                                readBuffer[1024];
        std::string                                         bodyResult;
        pid_t                                               pid;
        int                                                 fds[2];
        int                                                 storeStd[2];
        char                                                **cMetaVar;

        cMetaVar = _createCMetaVar();
        std::cout << "vars " << std::endl;
        for (size_t i = 0;cMetaVar[i]; i++){
            std::cout << "envvar " << cMetaVar[i] << std::endl;
        }
        storeStd[0] = dup(STDIN_FILENO);
        storeStd[1] = dup(STDOUT_FILENO);

        if (pipe(fds) < 0)
            throw pipeError();
        if ((pid = fork()) == -1)
            throw forkError();

        if (pid == 0)
            childProcess(fds, cMetaVar);
        if (waitChild(pid) > 0)
            throw execveError(errno);
        dup2(fds[1], STDOUT_FILENO);
        close (fds[1]);
        while (read(fds[1], readBuffer, 1024) > 0){
            bodyResult += readBuffer;
            bzero(readBuffer, 1024);
        }
        dup2(storeStd[0], STDIN_FILENO);
        dup2(storeStd[1], STDOUT_FILENO);
        close(fds[0]);
        close(fds[1]);
        close(storeStd[0]);
        close(storeStd[1]);
        for (size_t i = 0; cMetaVar[i]; i++)
            delete [] cMetaVar[i];
        delete [] cMetaVar;
        return (bodyResult);
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

    void childProcess(int fds[2], char **cMetaVar){
        char **args = new char*[2];
        std::string path("/home/user42/webserv/includes/CGI/cgi-bin");

        args[0] = strdupa(_mapMetaVars.find("SCRIPT_NAME=")->second.c_str());
        args[1] = NULL;

        dup2(fds[0], STDIN_FILENO);
        close(fds[0]);

        chdir(path.c_str());
        std::cerr << "CGI OUTPUT " <<  "args1 "<<  args[0] << std::endl;
        execve("/usr/bin/php-cgi", args, cMetaVar);
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
    void    _setMapEnvVar(std::map<std::string, std::string> headers){

        _setServerSoftware();
        _setServerName(headers);
        _setGatewayInterface();
        _setHtppVariables(headers);
        _setServerProtocol();
        _setServerPort();
        _setRequestUri();
        _setRequestMethod();
        _setPathInfo();
        _setPathTranslated();
        _setScriptName();
        _setQueryString();
        _setRemoteHost();
        _setRemoteAddr();
        _setRemoteUser();
        _setAuthType();
        _setContentType();
        _setContentLength();
        _setRedirectStatus();

    }

    std::string    _setURL(){
        std::string URL;
        std::string line;
        size_t      begin;
        size_t      end;

        line = _headerContent.substr(0, _headerContent.find('\n'));
        begin = line.find_first_of('/', 0);
        end =  line.find(' ',line.find_first_of(' ') + 1) - begin;
        URL = line.substr(begin, end);
        return (URL);
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

    void _setServerName(std::map<std::string, std::string> headers){
        std::string varName("SERVER_NAME=");
        std::string name("");
        std::string line;
        size_t      begin;
        size_t      end;

        line = headers.find("Host")->second;
        begin = 0;
        end = line.find(':');
        name = line.substr(begin, end);
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

    void _setServerPort(){
        std::string     varName("SERVER_PORT=");
        std::string     port("8080");

        this->_mapMetaVars.insert(std::make_pair(varName, port));
    }

    void _setRequestMethod(){
        std::string varName("REQUEST_METHOD=");
        std::string method("");
        size_t      end;
        
        end = _headerContent.find(' ');
        method = _headerContent.substr(0, end);
        this->_mapMetaVars.insert(std::make_pair(varName, method));
    }

    void _setPathInfo(){
        std::string varName("PATH_INFO=");
        std::string pathInfo("");
        std::string uri(_mapMetaVars.find("REQUEST_URI=")->second);
        
        this->_mapMetaVars.insert(std::make_pair(varName, uri));
    }

    void _setPathTranslated(){
        std::string varName("PATH_TRANSLATED=");
        std::string pathTranslated("");
        std::string uri(_mapMetaVars.find("REQUEST_URI=")->second);

        this->_mapMetaVars.insert(std::make_pair(varName, uri));
    }

    void _setScriptName(){
        std::string varName("SCRIPT_NAME=");
        std::string name("");
        size_t      begin;
        size_t      end = 0;

        begin = this->_decodedURL.rfind('/', _decodedURL.find(_cgiExtension)) + 1;
        if (begin == std::string::npos || end == std::string::npos){
            _mapMetaVars.insert(std::make_pair(varName, name));
            return ;
        }
        end = (this->_decodedURL.find(_cgiExtension) + 4) - begin;
        name = this->_decodedURL.substr(begin, end);
        this->_mapMetaVars.insert(std::make_pair(varName, name));
    }

    void _setQueryString(){
        std::string varName("QUERY_STRING=");
        std::string query("");
        size_t      queryChar;
        size_t      begin;
        size_t      end;
    
        if ((queryChar = this->_decodedURL .find('?', 0)) == std::string::npos){
            varName.append(query);
            _mapMetaVars.insert(std::make_pair(varName, query));
            return ;
        }
        begin = queryChar + 1;
        end = _decodedURL.size() - begin;
        query = this->_decodedURL.substr(begin, end);

        this->_mapMetaVars.insert(std::make_pair(varName, query));
    }

    void _setRemoteHost(){
        std::string varName("REMOTE_HOST=");
        std::string host("");

        this->_mapMetaVars.insert(std::make_pair(varName, host));
    }

    void _setRemoteAddr(){
        std::string varName("REMOTE_ADDR=");
        std::string addr("");

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
            std::string type(_mapMetaVars.find("HTTP_AUTHORIZATION=")->second);
            size_t      begin;
            size_t      end;
            
            begin = type.find(' ',0);
            end = type.size() - begin;
            type.erase(begin, end);
            authType = type;
        }
        this->_mapMetaVars.insert(std::make_pair(varName, authType));
    }

    void _setContentType(){
        std::string varName("CONTENT_TYPE=");
        std::string contentType("");

        if (_mapMetaVars.count("HTTP_CONTENT_TYPE=") == 1)
            contentType = _mapMetaVars.find("HTTP_CONTENT_TYPE=")->second;  
        this->_mapMetaVars.insert(std::make_pair(varName, contentType));
    }

    void _setContentLength(){
        std::string varName("CONTENT_LENGTH=");
        std::string contentLength("");

        if (_mapMetaVars.count("HTTP_CONTENT_LENGTH=") == 1)
            contentLength = _mapMetaVars.find("HTTP_CONTENT_LENGTH=")->second;
        this->_mapMetaVars.insert(std::make_pair(varName, contentLength));
    }

    void _setRequestUri(){
        std::string varName("REQUEST_URI=");
        std::string uri("");
        std::string scheme("http://");
        std::string serverName(this->_mapMetaVars.find("SERVER_NAME=")->second);
        serverName.append(":");
        std::string serverPort(this->_mapMetaVars.find("SERVER_PORT=")->second);
        std::string url(_encodedURL);

        uri = scheme.append(serverName).append(serverPort).append(url);
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