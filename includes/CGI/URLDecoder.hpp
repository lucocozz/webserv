#ifndef CLASS_URL_DECODER_HPP
# define CLASS_URL_DECODER_HPP

#include <iostream>
#include <string>
#include <map>
#include <cstdlib>

class URLDecoder
{
public:
    typedef     std::map<char, short>   storage;

    URLDecoder(){
        this->_fillMap('!', 0x21);
        this->_fillMap('#', 0x23);
        this->_fillMap('$', 0x24);
        this->_fillMap('&', 0x26);
        this->_fillMap('\'', 0x27);
        this->_fillMap('(', 0x28);
        this->_fillMap(')', 0x29);
        this->_fillMap('*', 0x2A);
        this->_fillMap('+', 0x2B);
        this->_fillMap(',', 0x2C);
        this->_fillMap('/', 0x2F);
        this->_fillMap(':', 0x3A);
        this->_fillMap(';', 0x3B);
        this->_fillMap('=', 0x3D);
        this->_fillMap('?', 0x3F);
        this->_fillMap('@', 0x40);
        this->_fillMap('[', 0x5B);
        this->_fillMap(']', 0x5D);
        this->_fillMap(' ', 0x20);
        this->_fillMap('\"', 0x22);
        this->_fillMap('%', 0x25);
        this->_fillMap('-', 0x2D);
        this->_fillMap('.', 0x2E);
        this->_fillMap('<', 0x3C);
        this->_fillMap('>', 0x3E);
        this->_fillMap('\\', 0x5C);
        this->_fillMap('^', 0x5E);
        this->_fillMap('_', 0x5F);
        this->_fillMap('`', 0x60);
        this->_fillMap('{', 0x7B);
        this->_fillMap('}', 0x7D);
        this->_fillMap('|', 0x7C);
        this->_fillMap('~', 0x7E);
    }

    ~URLDecoder(){}

    void        decoder(const std::string &encodedURL){
        this->_decodedURL = encodedURL;
        int     atoiRes;

        for (size_t i = 0; this->_decodedURL[i]; i++){
            if (this->_decodedURL[i] == '%'){
                char *end = const_cast<char *>((this->_decodedURL.c_str() + i + 3));
                atoiRes = std::strtol((this->_decodedURL.c_str() + i + 1), &end, 16);
                if (_isReservedChar(static_cast<const char>(atoiRes)) == true){
                    this->_decodedURL.replace((i), 3, 1, atoiRes);
                }
            }
        }
    }

    std::string &getDecodedURL(){
        return (this->_decodedURL);
    }

private:
    storage     _reservedChars;
    std::string _decodedURL;

    void _fillMap(const char c, short hex){
        std::pair<char , short>    pair = std::make_pair(c, hex);
        this->_reservedChars.insert(pair);
    }

    bool _isReservedChar(const char c){
        size_t cnt = 0;

        cnt = this->_reservedChars.count(c);
        if (cnt == 0)
            return (false);
        return (true);
    }
};



#endif