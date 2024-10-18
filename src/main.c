#include "../inc/ft_ping.h"

char	*dns_lookup( char *name )
{
	struct addrinfo	hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM; //SOCK_RAW;

	struct addrinfo	*res;
	int				status = getaddrinfo(name, NULL, &hints, &res);
	if (status != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
		return (NULL);
	}

	char	*addr = malloc(INET_ADDRSTRLEN * sizeof(char));
	if (addr == NULL)
	{
		fprintf(stderr, "Fatal Error: Allocation error\n");
		return (NULL);
	}
	// printf("ai_flags: %d | ai_family: %d | ai_socktype: %d\n", res->ai_flags, res->ai_family, res->ai_socktype);
	// printf("ai_cannoname: [%s]\n", res->ai_canonname);
	
	struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
	const char *tmp_s = inet_ntop(res->ai_family, &(ipv4->sin_addr), addr, INET_ADDRSTRLEN);
	if (tmp_s == NULL)
	{
		fprintf(stderr, "inet_ntop error: %s\n", strerror(errno));
	}
	printf("addr: [%s]\n", addr);

	printf("sin_family: [%d] | sin_port: [%d]\n", ipv4->sin_family, ipv4->sin_port);

	return (addr);
}

char	*reverse_dns_lookup( char *addr )
{
	
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
	char *addr = dns_lookup(argv[1]);
	(void) addr;
	return (0);
}