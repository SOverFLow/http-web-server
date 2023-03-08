void		Socket::_setAddress(int port, const char *ip)
{
	struct sockaddr_in	address;
	
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(ip);
	address.sin_port = htons(port);
	memset(address.sin_zero, '\0', sizeof(address.sin_zero));
	
	this->_address.push_back(address);
	this->_addrLen.push_back(sizeof(address));
}

int		setsocknonblock(int sock)
{
	int flag;

	flag = fcntl(sock, F_GETFL, 0);
	if (flag < 0)
	{
		perror("Fcntl (F_GETFL) failed");
		return (ERR);
	}
	if (fcntl(sock, F_SETFL, flag | O_NONBLOCK) < 0)
	{
		perror("Fcntl (F_SETFL) failed");
		return (ERR);
	}
	return (OK);
}

std::string	deletingUseless(std::string header)
{
	int			find, i = 0;
	
	find = header.find("Content-Type:");
	if (find > 0)
	{
		i = find;
		while (header[i] && header[i] != '\n')
			i++;
		header.erase(find, (i + 1) - find);
	}
	find = header.find("Host:");
	if (find > 0)
	{
		i = find;
		while (header[i] && header[i] != '\n')
			i++;
		header.erase(find, (i + 1) - find);
	}
	find = header.find("Content-Length:");
	if (find > 0)
	{
		i = find;
		while (header[i] && header[i] != '\n')
			i++;
		header.erase(find, (i + 1) - find);
	}
	return header;
}


std::string	headerForCgi(std::string header, Client &client)
{
	std::string			cgiHeader;
	std::string			cgiBody = client.getCgiBody();
	std::string			tmp;
	std::stringstream	out;
	int			i = 0;
	size_t		find;

	cgiHeader = deletingUseless(header);
	find = cgiBody.find("Content-type:");
	if (find != std::string::npos)
	{
		while (cgiBody[i + 3] && (cgiBody[i] != '\r' && cgiBody[i + 1] != '\n' && cgiBody[i + 2] != '\r' && cgiBody[i + 3] != '\n'))
			i++;
		tmp = &cgiBody[i + 6];
		cgiBody.erase(i + 4, strlen(cgiBody.c_str()) - (i + 4));
	}
	else
		tmp = cgiBody;
	client.setCgiBody(tmp);
	out << client.getCgiBody().length();
	cgiHeader += "Content-Length: ";
	cgiHeader += out.str();
	cgiHeader += "\r\n";
	cgiHeader += cgiBody;
	cgiHeader += "\r\n";
	client.setIsContentLen(cgiHeader);
	return (cgiHeader);
}

static void	setCgiString(FILE *temp, int fdtemp, Client &client)
{
	char buff[4096];
	std::string string;

	rewind(temp);
	while(!feof(temp))
 	{
       if (fgets(buff, 4096, temp) == NULL)
             break;
       string += buff;
 	}
	close(fdtemp);
	fclose(temp);
	client.setIsContentLen(string);
	client.setCgiBody(string);
}

static void	execution(std::string cgi, Client &client, int fdtemp, char **arg, char **env)
{
	int	fdToUse;

	if (client.getMethod() == POST)
	{
		if (client.getHeader().isChunkedEncoded())
		{
			rewind(client.getTmpChunked());
			fdToUse = client.getFdTmpChunked();
		}
		else
		{
			fseek(client.getTmp(), client.getHeaderSize(), SEEK_SET);
			fdToUse = client.getFdTmp();	
		}
		if (dup2(fdToUse, STDIN_FILENO) < 0)
		{
			perror("dup2()");
			exit(EXIT_FAILURE);
		}
	}
	if (dup2(fdtemp, STDOUT_FILENO) < 0)
	{
		perror("dup2()");
		exit(EXIT_FAILURE);
	}
	execve((char *)cgi.c_str(), arg, env);
	perror("execve()");
	exit(0);
}

int 		executeCGI(std::string cgi, Client &client)
{
	char		**env;
	char        *arg[2] = {(char *)cgi.c_str(), NULL};
	pid_t		pid;
	int			status;
	FILE		*temp = std::tmpfile();
	int			fdtemp = fileno(temp);
	int			timeoutTime = 1;
	
	if (mallocEnv(&env, client) < 0)
		return (ERR);
	
	pid = fork();
	if (pid < 0)
		exit(EXIT_FAILURE);
	if (pid == 0)
	{
		pid_t workerPid = fork();
		if (workerPid == 0)
			execution(cgi, client, fdtemp, arg, env);

    	pid_t	timeoutPid = fork();
		if (timeoutPid == 0)
		{
			sleep(timeoutTime);
			exit(0);
    	}

		pid_t exitedPid = wait(NULL);
		if (exitedPid == workerPid)
			kill(timeoutPid, SIGKILL);
		else
		{
			kill(workerPid, SIGKILL);
			write(fdtemp, "Content-type: text/plain\r\n\r\nTIMEOUT", 35);
		}
    	wait(NULL);
    	exit(0);
	}
	else
	{
		waitpid(pid, &status, 0);
		setCgiString(temp, fdtemp, client);
		freeEnv(&env, client);
	}
	
	return (OK);
}



void    setEnvForCgi(Socket &sock, int sockNbr, Client &client)
{
	std::string			string;
	HTTPResponse		&response = client.getResponse();
	HTTPHeader			&header = client.getHeader();
	std::string			file = sock.getRealUrl(sockNbr, response.getUrl());
    
	client.setEnvValue("SERVER_NAME", sock.getServerName(sockNbr));
    client.setEnvValue("GATEWAY_INTERFACE", "CGI/1.1");
	client.setEnvValue("PATH_INFO", file);
	client.setEnvValue("REQUEST_METHOD", response.getMethod());
	client.setEnvValue("SCRIPT_FILENAME", file);
	client.setEnvValue("SERVER_PROTOCOL", "HTTP/1.1");
	client.setEnvValue("REDIRECT_STATUS", "200");
	if (header.getContentType() == "")
		client.setEnvValue("CONTENT_TYPE", "application/x-www-form-urlencoded");
	else
		client.setEnvValue("CONTENT_TYPE", header.getContentType());
	if (client.getEnvValue("REQUEST_METHOD") == "POST")
		client.setEnvValue("CONTENT_LENGTH", toString(client.getBodySize()));
	else
	{
		if (client.isQueryString() == false)
			client.setEnvValue("QUERY_STRING", "");
	}
}

void	freeEnv(char ***env, Client &client)
{
	size_t i = 0;

	while (i < client.getEnvSize())
	{
		free((*env)[i]);
		(*env)[i] = NULL;
		i++;
	}
	free((*env));
	(*env) = NULL;
}

int 	mallocEnv(char ***env, Client &client)
{
	size_t				nbr = 0;
	mapStr				tmp = client.getEnv();
	std::string			val;

	(*env) = (char **)malloc(sizeof(char *) * (client.getEnvSize() + 1));
	if (!(*env))
		return (ERR);
	(*env)[client.getEnvSize()] = NULL;
	for (mapStr::iterator it = tmp.begin(); it != tmp.end(); it++)
	{
		val = it->first;
		val += "=";
		val += it->second;
		(*env)[nbr] = (char *)malloc(sizeof(char) * (strlen(val.c_str()) + 1));
		if (!(*env)[nbr])
			return ERR;
		strcpy((*env)[nbr], val.c_str());
		(*env)[nbr][strlen(val.c_str())] = '\0';
		nbr++;
	}
	return (OK);
}