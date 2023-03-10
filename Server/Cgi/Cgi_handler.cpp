#include <string.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

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

std::string Header_gen( std::string )
{
    
}

std::string     Cgi_Handler(std::string path, char **env)
{
    char *buff;
    char c;
    std::string res;
    res = get_cgi_output(path, env);
    std::cout << res << std::endl;
    return res;
}
