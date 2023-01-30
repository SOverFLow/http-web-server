

#include <string.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdlib.h>
#include <sys/socket.h>

std::string     Cgi_Handler(std::string path, char **env, int sock)
{
    char *buff;
    std::string all;
    std::string res;
    int req[2];
    pid_t pid;
    //pipe(req);
    write(sock, "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n", 44);
    pid = fork();
    if (!pid)
    {
        char *cmd[4];
        cmd[0] = "cgi-php";
        cmd[1] = (char *)path.data();
        cmd[2] = "php";
        cmd[3] = NULL;
        dup2(sock, 1);
        if(!execve("./cgi-bin/cgi-php", cmd, env))
            std::cout << "Error " << std::endl;
    }
    waitpid(pid, NULL, 0);
    res = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";
    std::cout << all << std::endl;
    //std::cout << res + all << std::endl;
    return (res + all);
}
