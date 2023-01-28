

#include <string.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

std::string     Cgi_Handler(std::string path, char **env)
{
    char *buff;
    int req[2];
    pid_t pid;
    //std::cout << (char *)path.data() << std::endl;
    pipe(req);
    pid = fork();
    if (!pid)
    {
        char *cmd[4];
        cmd[0] = "cgi-php";
        cmd[1] = (char *)path.data();
        cmd[2] = "php";
        cmd[3] = NULL;
        close(req[0]);
        dup2(1, req[1]);
        execve("./cgi-bin/cgi-php", cmd, env);
    }
    close(req[1]);
    waitpid(pid, NULL, 0);
    read(req[0], buff, 1000);
    std::string res = buff;
    return (res);
}
