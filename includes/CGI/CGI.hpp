#ifndef CLASS_CGI_HPP
# define CLASS_CGI_HPP

#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <iostream>
#include <map>
#include <exception>

#include "Epoll.hpp"
#include "Socket.hpp"
#include "EpollSocket.hpp"

#include "URLDecoder.hpp"

class CGI{
public:
    typedef             std::pair<ServerContext, LocationContext >          serverLocation;
    typedef             std::pair<std::string, std::string>                 clientInfo;

private:
    std::string                         _encodedURL;
    std::string                         _decodedURL;
    std::string                         _requestBody;
    std::map<std::string, std::string>  _mapMetaVars;
    ServerContext                       _serverContext;
    LocationContext                     _locationContext;


public:

    CGI(const std::string &url, std::map<std::string, std::string> headers, 
        const std::string &body,
		const serverLocation &serverLocation, const clientInfo &clientInfo,
		const std::string &method): 
    _encodedURL(url),
    _decodedURL(_decodeUrl()),
    _requestBody(body),
    _mapMetaVars(),
    _serverContext(serverLocation.first),
    _locationContext(serverLocation.second){
        _setMapEnvVar(headers, serverLocation, clientInfo, method);
    }

    ~CGI(){
    }

    std::pair<std::string, int> cgiHandler(){
        std::pair<std::string, int>     cgiResponse;
        pid_t                           pid;
        int                             fdToChild[2];
        int                             fdToParent[2];
        int                             childExitStatus = 0;                                    
        char                            **cMetaVar;
        struct stat                     dummy;

        cMetaVar = this->_createCMetaVar();
        for (size_t i = 0; cMetaVar[i]; i++){
            std::cout << cMetaVar[i] << std::endl;
        }
        if (pipe(fdToChild) < 0)
            throw pipeError();
        if (pipe(fdToParent) < 0)
            throw pipeError();
        if ((pid = fork()) == -1)
            throw forkError();
        if (stat(_mapMetaVars.find("PATH_TRANSLATED=")->second.c_str(), &dummy) == -1){
            for (size_t i = 0; cMetaVar[i]; i++)
                delete [] cMetaVar[i];
            return (std::make_pair("", 404));
        }
        if (pid == 0)
            this->_childProcess(fdToChild, fdToParent, cMetaVar);
        
        for (size_t i = 0; cMetaVar[i]; i++)
            delete [] cMetaVar[i];
        delete [] cMetaVar;

        if (_mapMetaVars.find("REQUEST_METHOD=")->second.compare("POST") == 0){
            write(fdToChild[1], _requestBody.c_str(),  _requestBody.size());
        }

        close(fdToChild[0]);
        close(fdToChild[1]);
        cgiResponse.first = this->_getCgiOutput(fdToParent);
        if ((childExitStatus = this->_waitChild(pid)) > 0)
            throw execveError(childExitStatus);
        cgiResponse.second = this->_getCgiReturnStatus(cgiResponse.first);
        if (cgiResponse.second == 200)
            this->_getCgiOutputBody(cgiResponse.first);
        close(fdToParent[0]);
        return (cgiResponse);
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
        _setScriptName();
        _setPathInfo();
        _setPathTranslated();
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
    
    void _setScriptName(){
        std::string		varName("SCRIPT_NAME=");
        std::string     name("");
        std::string     cgiExtension("");

        if (this->_locationContext.directives.count("cgi_extension") == 1)
            cgiExtension.append(this->_locationContext.directives.find("cgi_extension")->second[0]);
        else{
            cgiExtension.append(this->_locationContext.args[0]);
            cgiExtension.erase(cgiExtension.begin());
        }
        std::string::iterator   begin = this->_decodedURL.begin() + this->_decodedURL.find(cgiExtension);
        std::string::iterator	end = this->_decodedURL.begin() + this->_decodedURL.find(cgiExtension);
		while (*begin != '/')
            begin--;
        begin++;
		while (*end != '/' && *end != '?' && *end != '\0')
			end++;
		name.append(begin ,end);
        this->_mapMetaVars.insert(std::make_pair(varName, name));
    }

    void _setPathInfo(){
        std::string             varName("PATH_INFO=");
        std::string             pathInfo("");
        std::string::iterator   itb;
        std::string::iterator   ite;

        itb = _decodedURL.begin();
        ite = itb;
        while (ite != _decodedURL.end() && *ite != '?'){
            ite++;
        }
        pathInfo.append(itb, ite);
        this->_mapMetaVars.insert(std::make_pair(varName, pathInfo));
    }

    void _setPathTranslated(){
        std::string             varName("PATH_TRANSLATED=");
        std::string             pathTranslated("");
        std::string             scriptName(this->_mapMetaVars.find("SCRIPT_NAME=")->second);
        std::string             currentDirName(get_current_dir_name());
        std::string             path(currentDirName.append("/") + _getCurrentLocation());

        path.append("/");
        pathTranslated.append(path + scriptName);
        _clearUrl(pathTranslated);
        this->_mapMetaVars.insert(std::make_pair(varName, pathTranslated));
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

    std::string _formateVarName(const std::string &varName){
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

    void _childProcess(int fdToChild[2], int fdToParent[2], char **cMetaVar){
        char                    **args = new char*[3];
        std::string             cgiLocationPath(this->_mapMetaVars.find("PATH_TRANSLATED=")->second);

        args[0] = strdupa(this->_locationContext.directives.at("cgi_binary")[0].c_str());
        args[1] = strdupa(this->_mapMetaVars.find("SCRIPT_NAME=")->second.c_str());
        args[2] = NULL;

        close(fdToParent[0]);
        dup2(fdToParent[1], STDOUT_FILENO);
        close(fdToParent[1]);

        close(fdToChild[1]);
        dup2(fdToChild[0], STDIN_FILENO);
        close(fdToChild[0]);

        chdir(cgiLocationPath.c_str());
        std::cerr << "currentdir |" << get_current_dir_name() << "| cgi binary: |" << args[0]  << "| cgi script name: |" << args[1] << "|" << std::endl;
        execve(args[0], args, cMetaVar);
        exit(errno);
    }

    int _waitChild(pid_t pid){
        int exitStatus;
        int exitCode;

        exitStatus = 0;
        exitCode = 0;
        waitpid(pid, &exitStatus, 0);
        if (WIFEXITED(exitStatus))
            exitCode = WEXITSTATUS(exitStatus);
        return (exitCode);
    }

    int _getCgiReturnStatus(std::string &response){
        int status = 200;
        if (response.find("Status:") != std::string::npos){
            std::string strStatus;
            std::string::iterator begin = response.begin() + (response.find("Status:") + 8);
            std::string::iterator end = begin;
            for (int i = 0; i < 3; i++){
                end++;
            }
            strStatus.append(begin, end);
            return (atoi(strStatus.c_str()));
        }
        return (status);
    }

    std::string _getCgiOutput(int fdToParent[2]){
        char        readBuffer[1024 + 1];
        int         nbRead = 0;
        std::string cgiOutput;

        close(fdToParent[1]);
		bzero(readBuffer, 1024 + 1);
        nbRead = read(fdToParent[0], readBuffer, 1024);
        while (nbRead > 0){
            cgiOutput += readBuffer;
            bzero(readBuffer,1024 + 1);
            nbRead = read(fdToParent[0], readBuffer, 1024);
        }
        cgiOutput += readBuffer;
        return (cgiOutput);
    }

    void _getCgiOutputBody(std::string &cgiOutput){
        std::string::iterator begin;
		std::string::iterator end;
        size_t                htmlOpen;

        if (this->_locationContext.directives.at("cgi_binary")[0].find("php-cgi") != std::string::npos)
            htmlOpen = cgiOutput.find("\r\n\r\n");
        else 
            htmlOpen = cgiOutput.find("<html>");
        begin = cgiOutput.begin();
        if (htmlOpen != std::string::npos)
            end = begin + htmlOpen;
        else
            end = begin;
        cgiOutput.erase(begin, end);
    }

    void _clearUrl(std::string &url){
        std::string::iterator   it = url.begin();

        while (it != url.end()){
            if (*it == '/' && *(it + 1) == '/')
                url.erase(it);
            it++;
        }
    }

    std::string _getCurrentLocation(){
        std::string                         url(_decodedURL);
        std::string                         location("");
        std::string::reverse_iterator rit = url.rbegin();

        while(*rit != '/'){
            url.erase(--rit.base());
            rit++;
        }
        location = this->_serverContext.directives.at("root")[0].append(url);
        _clearUrl(location);
        return (location);
    }
};




#endif
