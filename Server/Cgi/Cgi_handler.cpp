#include <string.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../Request/Request.hpp"

std::string get_cgi_output(std::string path, char **env)
{
    std::string res;
    int fd_req[2];
    char c;
    pid_t pid;
    pipe(fd_req);
    if ((pid = fork()) < 0)
        std::cerr << "fork error!" << std::endl;
    if (!pid)
    {
        close(fd_req[0]);
        char *cmd[4];
        cmd[0] = (char *)"/cgi-bin/php-cgi";
        cmd[1] = (char *)path.substr(1).data();
        cmd[2] = NULL;
        dup2(fd_req[1], 1);
        if(execve("./cgi-bin/php-cgi", cmd, env) < 0)
            std::cerr << "Exec Error!" << std::endl;
    }
    close(fd_req[1]);
    waitpid(pid, NULL, 0);
    while(read(fd_req[0], &c, 1))
        res += c;
    return res;
}

std::string getCtype(std::string Output)
{
    int Ctype_p;
    std::string Content_type;
    Ctype_p = Output.find("Content-type:", 0);
    for (int i = Ctype_p+13; Output[i] != ';'; i++)
        Content_type += Output[i];
    return (Content_type);
}

std::string getBody(std::string Output)
{
    size_t  Body_p;
    std::string Body;
    
    Body_p = Output.find("\r\n\r\n", 0);
    Body = Output.substr(Body_p+4, Output.length() - Body_p+4);
    return(Body);
}

std::string Header_gen( std::string Output, Request req)
{
    std::string Header;
    std::string Content_type;
    std::string Status;
    std::string Comment;
    if (Output.find("Status:", 0) == std::string::npos)
    {
        Status = "200";
        req.cgiStatus = 200;
        Content_type = getCtype(Output);
        Header = "HTTP/1.1 " + Status + " OK\r\nContent-type:" + Content_type + "\r\n\r\n" + getBody(Output);
    }
    else
    {
        Status = Output.substr(8, 3);
        Content_type = getCtype(Output);
        req.cgiStatus = atoi(Status.c_str());
        for(int i = 12; Output[i] != '\n'; i++)
            Comment += Output[i];
        Header = "HTTP/1.1 " + Status + " " + Comment + "\r\nContent-type:" + Content_type + "\r\n\r\n";
    }
    return (Header);
}

char    **setEnv(Request req)
{
    char **env = new char*[20];
    env[0] = (char *)("QUERY_STRING=" + req.Qurey_String).c_str();
    std::cout << env[0] << std::endl;
    env[20] = NULL;
}

std::string     Cgi_Handler(Request req, std::string Path, char **env)
{
    std::string all;
    std::string out;
    out = get_cgi_output(Path, env);
    all = Header_gen(out, req);
    return all;
}
