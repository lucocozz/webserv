#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include "ClassCGI.hpp"

int main(void){
    int ret = 0;
    pid_t pid;
    //arg for execv
    char *const arg[2] = {strdup("cgi_script.cgi"), NULL};
    //env variable of the name of the file to execute
    std::string CGI_FILE("CGI_FILE=cgi_script.cgi");
    //cgi binaries folder
    std::string path("cgi-bin/");

    //add env variable
    putenv(const_cast<char *>(CGI_FILE.c_str()));

    //join path & env variable
    path.append(getenv("CGI_FILE"));

    //fork the execv
    if ((pid = fork()) == -1){
        std::cout << "error handling" << std::endl;
        return (1);
    }
    if (pid == 0)
        execv(path.c_str(), arg);
    waitpid(pid, NULL, 0);
    printf("errno: %d - %s\n", errno, path.c_str());
    delete arg[0];

    return (0);
}