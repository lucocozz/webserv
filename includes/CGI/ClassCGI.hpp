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

#include "../server/Epoll.hpp"
#include "../server/Socket.hpp"
#include "../server/EpollSocket.hpp"

class CGI{
public:
    typedef             EpollSocket                 ClientInfo;
    typedef             std::pair<std::string, int> RequestData;
    typedef             std::string                 HeaderCont;

    CGI(): _info(), _header(){
    }

    CGI(const ClientInfo &EpSoc, const RequestData &data): 
    _info(EpSoc),
    _headerSize(data.second),
    _header(header_parser(data.first, data.second)){
    }

    CGI(const CGI &other){
        *this = other;
    }

    ~CGI(){}

    CGI &operator=(const CGI &other){
        _info = other._info;
        _header = other._header;
        return(*this);
    }
    
    #include <stdio.h>
    void    create_envs(){
        SERVER_SOFTWARE();
      //  printf("%s\n", getenv("SERVER_SOFTWARE"));
        SERVER_NAME();
        GATEWAY_INTERFACE();
    }

private:
    ClientInfo  _info;
    size_t      _headerSize;
    HeaderCont  *_header;

    std::string    *header_parser(const std::string &data, const size_t size) const{
        size_t      start = 0, end = 0;
        size_t      i = 0;
        std::string *buffer;

        buffer = new std::string [14];
        while (start < _headerSize){
            end = data.find('\n', start);
            buffer[i] = data.substr(start, (end - start));
            start = end + 1;
            i++;
        }
        return (buffer);
    }

    void SERVER_SOFTWARE() const{
        std::string name_version;

        name_version = _header[0].substr(_header[0].find("HTTP"), 8);
        setenv("SERVER_SOFTWARE", name_version.c_str(), 1);
    }

    void SERVER_NAME() const{
        std::string name;

        name = _header[1].substr(6, (_header[1].size() - 7));
        setenv("SERVER_NAME", name.c_str(), 1);
    }

    void GATEWAY_INTERFACE(){
        setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
    }
};

int CGI_startup(EpollSocket EplSock, std::pair<std::string, int> Header);


#endif