#include <string.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdlib.h>
#include <sys/socket.h>

std::string     Cgi_Handler(std::string path, char **env)
{
    char *buff;
    char c;
    std::string res;
    int req[2];
    pid_t pid;
    pipe(req);
    pid = fork();
    if (!pid)
    {
        char *cmd[4];
        cmd[0] = "php-cgi";
        cmd[1] = (char *)path.data();
        cmd[2] = NULL;
        close(req[0]);
        dup2(req[1], 1);
        if(!execve("./cgi-bin/php-cgi", cmd, env))
            std::cout << "Error " << std::endl;
    }
    close(req[1]);
    waitpid(pid, NULL, 0);
    while(read(req[0], &c, 1))
        res += c;
    return (res);
}
