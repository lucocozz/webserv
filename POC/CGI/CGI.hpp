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

#include "../../includes/socket/Epoll.hpp"
#include "../../includes/socket/Socket.hpp"
#include "../../includes/socket/EpollSocket.hpp"

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
 * header parser: std::string    *header_parser(const std::string &data) const
 * @brief Parse the header and fill each line into an array of std::string
 * 
 * @param Infos header as a std::string
 * @return array of std::string containing each line of the header
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
    std::map<std::string, std::string>  _envVar;

public:

    CGI(const SocketInfo ServerInfo, const RequestData &data): 
    _serverInfo(ServerInfo),
    _headerSize(data.second),
    _headerContent(data.first),
    _encodedURL(_setURL()),
    _decodedURL(_decodeUrl()),
    _envVar(){
    }

    ~CGI(){
    }

    std::map<std::string, std::string>  &getEnvVar() {
        return (_envVar);
    }

    void CGIStartup(std::map<std::string, std::string> headers){
        //Fonction a mettre au propre
        pid_t                                               pid;
        std::string                                         join;
        std::string                                         path("/home/user42/webserv/POC/CGI/cgi-bin/");
        std::map<const std::string, std::string>::iterator  itb;
        char                                                **envVar;
        char *const                                         *args = NULL;

        createEnvs(headers);
        envVar = new char *[this->_envVar.size()];
        itb = this->_envVar.begin();
        for (size_t i = 0; i < this->_envVar.size(); i++){
            join = static_cast<std::string>(itb->first).append(itb->second);
            envVar[i] = strdup(join.c_str());
            itb++;
        }
        path.append(_envVar.find("SCRIPT_NAME=")->second);
        pid = fork();
        if (pid == 0)
            execve(path.c_str(), args, envVar);
        waitpid(pid, NULL, 0);
        delete [] envVar; 
    }

private:
    void    createEnvs(std::map<std::string, std::string> headers){

        _setServerSoftware();
        _setServerName(headers);
        _setGatewayInterface();

        _setServerProtocol();
        _setServerPort();
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

        _setHtppVariables(headers);
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

       this->_envVar.insert(std::make_pair(varName, nameVersion));
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
        this->_envVar.insert(std::make_pair(varName, name));
    }

    void _setGatewayInterface(){
        std::string     varName("GATEWAY_INTERFACE=");
        std::string     name("CGI/1.1");

        this->_envVar.insert(std::make_pair(varName, name));
    }

    void _setServerProtocol(){
        std::string     varName("SERVER_PROTOCOL=");
        std::string     name_version("HTTP/1.1");

        this->_envVar.insert(std::make_pair(varName, name_version));
    }

    void _setServerPort(){
        //get the port througt server
    }

    void _setRequestMethod(){
        std::string varName("REQUEST_METHOD=");
        std::string method("");
        size_t      end;
        
        end = _headerContent.find(' ');
        method = _headerContent.substr(0, end);
        this->_envVar.insert(std::make_pair(varName, method));
    }

    void _setPathInfo(){
        std::string         varName("PATH_INFO=");
        std::string         pathInfo("");
        const std::string   cgiExtension(".cgi"); 
        size_t              begin;
        size_t              end;
        
        begin = this->_decodedURL.find(cgiExtension, 0) + cgiExtension.size();
        end =  _decodedURL.size() - begin;
        if (end == 0){
            _envVar.insert(std::make_pair(varName, ""));
            return ;
        }
        pathInfo = this->_decodedURL.substr(begin, end);
        
        this->_envVar.insert(std::make_pair(varName, pathInfo));
    }

    void _setPathTranslated(){
        std::string varName("PATH_TRANSLATED=");
        std::string path("");
        std::string wd(get_current_dir_name());
        size_t      eraseBegin;
        size_t      eraseEnd;

        eraseBegin = _decodedURL.find(_envVar.find("PATH_INFO=")->second, 0);
        eraseEnd = _decodedURL.size() - eraseBegin;
        if (eraseBegin == 0){
            _envVar.insert(std::make_pair(varName, wd.append(_decodedURL)));
            return ;
        }
        path = _decodedURL.erase(eraseBegin, eraseEnd);
        //wd should be the path from the root of the server
        this->_envVar.insert(std::make_pair(varName, wd.append(path)));
    }

    void _setScriptName(){
        std::string varName("SCRIPT_NAME=");
        std::string     name("");
        size_t          begin;
        size_t          end = 0;

        begin = this->_decodedURL.find("cgi-bin/");
        if (begin == std::string::npos || end == std::string::npos){
            _envVar.insert(std::make_pair(varName, name));
            return ;
        }
        begin += 8;
        end = this->_decodedURL.find(".cgi", begin) - (begin - 5) - 1;
        // may change depending on how the location is managed
        name = this->_decodedURL.substr(begin, end);
        this->_envVar.insert(std::make_pair(varName, name));
    }

    void _setQueryString(){
        std::string varName("QUERY_STRING=");
        std::string query("");
        size_t      queryChar;
        size_t      begin;
        size_t      end;
    
        if ((queryChar = this->_decodedURL .find('?', 0)) == std::string::npos){
            varName.append(query);
            _envVar.insert(std::make_pair(varName, query));
            return ;
        }
        begin = queryChar + 1;
        end = _decodedURL.size() - begin;
        query = this->_decodedURL.substr(begin, end);

        this->_envVar.insert(std::make_pair(varName, query));
    }

    void _setRemoteHost(){
        std::string varName("REMOTE_HOST=");
        std::string host("");

        this->_envVar.insert(std::make_pair(varName, host));
    }

    void _setRemoteAddr(){
        std::string varName("REMOTE_ADDR=");
        std::string addr("");

        this->_envVar.insert(std::make_pair(varName, addr));
    }

    void _setRemoteUser(){
        std::string varName("REMOTE_USER=");
        std::string user("");

        this->_envVar.insert(std::make_pair(varName, user));
    }

    void _setAuthType(){
        std::string varName("AUTH_TYPE=");
        std::string authType("");

        this->_envVar.insert(std::make_pair(varName, authType));
    }

    void _setContentType(){
        std::string varName("CONTENT_TYPE=");
        std::string contentType("");

        this->_envVar.insert(std::make_pair(varName, contentType));
    }

    void _setContentLength(){
        std::string varName("CONTENT_LENGTH=");
        std::string contentLength("");

        this->_envVar.insert(std::make_pair(varName, contentLength));
    }
    
    void _setHtppVariables(std::map<std::string, std::string> &headers){
        std::map<std::string, std::string>::iterator itb = headers.begin();
        std::string                                  varName;

        while (itb != headers.end()){
            varName = _formateVarName(itb->first);
            this->_envVar.insert(std::make_pair(varName, itb->second));
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