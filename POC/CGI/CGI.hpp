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

#include "../server/Epoll.hpp"
#include "../server/Socket.hpp"
#include "../server/EpollSocket.hpp"

#include "URLDecoder.hpp"

#define CGI_EXTENSION ".cgi/"
#define CGI_FOLDER "/cgi-bin/"

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

    void    create_envs(){

        _SERVER_SOFTWARE();
        _SERVER_NAME();
        _GATEWAY_INTERFACE();

        _SERVER_PROTOCOL();
        _SERVER_PORT();
        _REQUEST_METHOD();
        _PATH_INFO();
        _PATH_TRANSLATED();
        _SCRIPT_NAME();
        _QUERY_STRING();
        _REMOTE_HOST();
        _REMOTE_ADDR();
        _REMOTE_USER();
        _AUTH_TYPE();
        _CONTENT_TYPE();
        _CONTENT_LENGTH();

        _HTTP_ACCEPT();
        _HTTP_ACCEPT_LANGUAGE();
        _HTTP_USER_AGENT();
        _HTTP_COOKIE();
        _HTTP_REFERER();

    }

    static void CGI_startup(struct addrinfo *ServerInfo, std::pair<std::string, int> Header){
        //Fonction a mettre au propre
	    CGI                                                 cgi(ServerInfo, Header);
        std::string                                         join;
        std::map<const std::string, std::string>::iterator  itb;
        const char                                          **envVar;

        cgi.create_envs();
        envVar = new const char *[cgi.getEnvVar().size()];
        itb = cgi.getEnvVar().begin();
        for (size_t i = 0; i < cgi.getEnvVar().size(); i++){
            join = static_cast<std::string>(itb->first).append(itb->second);
            envVar[i] = strdup(join.c_str());
            itb++;
        }
        
        // for (size_t i = 0; i < cgi.getEnvVar().size(); i++){
        //     std::cout << "envVar " << i << " " << envVar[i] << std::endl; 
        //     //cannot use delete because envVar is allocated with malloc throught strdup
        //     //delete envVar[i];
        // }

        delete [] envVar; 
    }

private:
    std::string _getHeaderLine(const std::string hint){
        std::string line("");
        size_t      begin;
        size_t      end;

        begin = _headerContent.find(hint, 0);

        end = _headerContent.find('\n', begin) - begin;
        if (begin == std::string::npos)
            return (line);
        line = _headerContent.substr(begin, end);
        return (line);
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

    void _SERVER_SOFTWARE(){
        std::string varName("SERVER_SOFTWARE=");
        std::string nameVersion("WebServ/1.0");

       _envVar.insert(std::make_pair(varName, nameVersion));
    }

    void _SERVER_NAME(){
        std::string varName("SERVER_NAME=");
        std::string name("");
        std::string line;
        size_t      begin;
        size_t      end;

        line = _getHeaderLine("Host");
        begin = 6;
        end = line.size() - 7;
        name = line.substr(begin, end);
        name.erase(name.find(':',0), name.rfind('\0', 0));
        _envVar.insert(std::make_pair(varName, name));
    }

    void _GATEWAY_INTERFACE(){
        std::string     varName("GATEWAY_INTERFACE=");
        std::string     name("CGI/1.1");

        _envVar.insert(std::make_pair(varName, name));
    }

    void _SERVER_PROTOCOL(){
        std::string     varName("SERVER_PROTOCOL=");
        std::string     name_version("");
        std::string     line;
        size_t          begin;
        size_t          end;

        line = _headerContent.substr(_headerContent.find("HTTP"), _headerContent.find('\n'));
        begin = _headerContent.find("HTTP");
        end = 8;
        name_version = _headerContent.substr(begin, end);
        _envVar.insert(std::make_pair(varName, name_version));
    }

    void _SERVER_PORT(){
        std::string varName("SERVER_PORT=");
        std::string port("");
        std::string line;
        size_t      begin;  
        size_t      end;
        
        line = _getHeaderLine("Host");
        begin = line.find(':',6) + 1;
        end = line.length() - begin;
        port = line.substr(begin, end);
       _envVar.insert(std::make_pair(varName, port));
    }

    void _REQUEST_METHOD(){
        std::string varName("REQUEST_METHOD=");
        std::string method("");
        size_t      end;
        
        end = _headerContent.find(' ');
        method = _headerContent.substr(0, end);
        _envVar.insert(std::make_pair(varName, method));
    }

    void _PATH_INFO(){
        std::string varName("PATH_INFO=");
        std::string pathInfo("");
        size_t      begin;
        size_t      end;
        
        if (_decodedURL.find(CGI_FOLDER) == std::string::npos){
            _envVar.insert(std::make_pair(varName, ""));
            return ;
        }
        begin = this->_decodedURL.rfind('/', this->_decodedURL.size());
        end =  _decodedURL.size() - begin;
        pathInfo = this->_decodedURL.substr(begin, end);
        if (pathInfo.size() == 1){
            _envVar.insert(std::make_pair(varName, ""));
            return ;
        }

        _envVar.insert(std::make_pair(varName, pathInfo));
    }

    void _PATH_TRANSLATED(){
        std::string varName("PATH_TRANSLATED=");
        std::string wd(get_current_dir_name());

        //wd should be the path from the root of the server
        _envVar.insert(std::make_pair(varName, wd.append(_decodedURL)));
    }

    void _SCRIPT_NAME(){
        std::string varName("SCRIPT_NAME=");
        std::string     name("");
        size_t          begin;
        size_t          end;

        begin = this->_decodedURL.find(CGI_FOLDER);
        end = this->_decodedURL.find(CGI_EXTENSION, begin) - (begin - 5);
        if (begin == std::string::npos || end == std::string::npos){
            _envVar.insert(std::make_pair(varName, name));
            return ;
        }
        name = this->_decodedURL.substr(begin, end);
       _envVar.insert(std::make_pair(varName, name));
    }

    void _QUERY_STRING(){
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

        _envVar.insert(std::make_pair(varName, query));
    }

    void _REMOTE_HOST(){
        std::string varName("REMOTE_HOST=");
        std::string host("");

        _envVar.insert(std::make_pair(varName, host));
    }

    void _REMOTE_ADDR(){
        std::string varName("REMOTE_ADDR=");
        std::string addr("");

        _envVar.insert(std::make_pair(varName, addr));
    }

    void _REMOTE_USER(){
        std::string varName("REMOTE_USER=");
        std::string user("");

        _envVar.insert(std::make_pair(varName, user));
    }

    void _AUTH_TYPE(){
        std::string varName("AUTH_TYPE=");
        std::string authType("");

        _envVar.insert(std::make_pair(varName, authType));
    }

    void _CONTENT_TYPE(){
        std::string varName("CONTENT_TYPE=");
        std::string contentType("");

        _envVar.insert(std::make_pair(varName, contentType));
    }

    void _CONTENT_LENGTH(){
        std::string varName("CONTENT_LENGTH=");
        std::string contentLength("");

        _envVar.insert(std::make_pair(varName, contentLength));
    }

    void _HTTP_ACCEPT(){
        std::string varName("HTTP_ACCEPT=");
        std::string dataAccept("");
        std::string line;
        size_t      begin;
        size_t      end;

        line = _getHeaderLine("Accept:");
        begin = 8;
        end = line.size();
        dataAccept = line.substr(begin, end);
        _envVar.insert(std::make_pair(varName, dataAccept));
    }

    void _HTTP_ACCEPT_LANGUAGE(){
        std::string varName("HTTP_ACCEPT_LANGUAGE=");
        std::string languageAccept("");
        std::string line;
        size_t      begin;
        size_t      end;

        line = _getHeaderLine("Accept-Language");
        begin = 17;
        end = line.size() - begin;
        languageAccept = line.substr(begin, end);

        _envVar.insert(std::make_pair(varName, languageAccept));
    }
    
    void _HTTP_USER_AGENT(){
        std::string varName("HTTP_USER_AGENT=");
        std::string browser("");
        std::string line;
        size_t      begin;
        size_t      end;
        
        line = _getHeaderLine("User-Agent:");
        begin = 12;
        end = line.find(' ', begin) - begin;
        browser = line.substr(begin, end);
        _envVar.insert(std::make_pair(varName, browser));
    }

    void _HTTP_COOKIE(){
        std::string varName("HTTP_COOKIE=");
        std::string cookies("");

        _envVar.insert(std::make_pair(varName, cookies));
    } 

    void _HTTP_REFERER(){
        std::string varName("HTTP_REFERER=");
        std::string URL("");

        _envVar.insert(std::make_pair(varName, URL));
    }
};




#endif