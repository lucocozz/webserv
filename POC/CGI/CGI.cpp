#include "ClassCGI.hpp"


int CGI_startup(EpollSocket EplSock, std::pair<std::string, int> RequestData){
	CGI cgi(EplSock, RequestData);

    cgi.create_envs();
}
