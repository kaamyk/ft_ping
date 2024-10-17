#include "../inc/ft_ping.h"

struct hostent	*dns_lookup( char *name )
{
	struct addrinfo	hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM; //SOCK_RAW;

	struct addrinfo	*res;
	int				status = 0;

	status = getaddrinfo(name, NULL, &hints, &res);
	if (status != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
		return (NULL);
	}

	if (res == NULL)
	{
		printf(">> res == NULL << \n");
	}

	struct addrinfo	*p;
	void	*addr;
	char	ipstr[INET_ADDRSTRLEN];
	p = res;
	while (p)
	{
		// struct sockaddr_in	*tmp = (struct sockaddr_in *)p->ai_addr;
		struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
		addr = &(ipv4->sin_addr);
		// printf("addr: [%s]\n", p->ai_addr->sa_data);
		printf("addr:[%s]\n", inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr)));
		p = p->ai_next;
	}
	return (NULL);
}

int	main( int argc, char **argv )
{
	//	Checking number of arguments
	if (argc < 2 )
	{
		write(2, "Usage Error: Destination address required\n", 42);
		exit(1);
	}
	else if (argc > 2)
	{
		write(2, "Usage Error: Too many arguments. Only one required.\n", 53);
		exit(1);
	}

	// struct hostent	ret;

	//	Checking if argument is valid
	( void )argv;
	struct hostent	*addr = dns_lookup(argv[1]);
	(void) addr;
	return (0);
}