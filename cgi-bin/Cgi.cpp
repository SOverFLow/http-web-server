

#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

void php_cgi(char *filename, char **env)
{
    pid_t pid = fork();
    char *cmd[3];
    cmd[0] = "php";
    cmd[1] = filename;
    cmd[2] = NULL;
    if(!pid)
    {
        execve("/usr/bin/php", cmd, env);
    }
    waitpid(pid, NULL, 0);
}
void    header_gen(int status_code)
{
    if (status_code == 404)
    {
        std::cout << "HTTP/1.1 404 Not Found\r\nContent-type: text/html\r\n\r\n";
        exit(1);
    }
    else if (status_code == 200)
        std::cout << "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";
}

int main (int ac, char **av, char **env)
{
    int fd = 0;
    int status_code = 200;
    if (ac != 3)
        return (0);
    fd = open(av[1], O_RDONLY);
    if (fd < 0)
    {
        status_code = 404;
        header_gen(status_code);
    }
    close(fd);
    header_gen(status_code);
    if (!strcmp(av[2], "php"))
    {
        php_cgi(av[1], env);
    }
    else
        std::cout << "Error: cgi not allowed" << std::endl;
}