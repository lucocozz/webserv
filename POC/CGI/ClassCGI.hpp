#ifndef CLASS_CGI_HPP
# define CLASS_CGI_HPP

#include "../server/Epoll.hpp"
#include "../server/Socket.hpp"
#include "../server/EpollSocket.hpp"

class ClassCGI{
public:
    typedef EpollSocket                 ClientInfo;
    typedef std::pair<std::string, int> Header;

    ClassCGI(): _info(), _header(){}

    ClassCGI(const ClientInfo &EpSoc, const Header &Header): _info(EpSoc), _header(Header){}

    ClassCGI(const ClassCGI &other){
        *this = other;
    }

    ~ClassCGI(){}

    ClassCGI &operator=(const ClassCGI &other){
        _info = other._info;
        _header = other._header;
        return(*this);
    }
    
private:
    ClientInfo  _info;
    Header      _header;
};

#endif