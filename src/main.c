#include "../inc/ft_ping.h"

uint8_t	looping = 1;

// void	sig_handler( int signum )
// {
// 	if (signum == SIGINT)
// 	{
// 		printf("> Signal SIGINT <");
// 		looping = 0;
// 	}
// }

// void	ignore_quit( void )
// {
// 	struct sigaction	ign;
// 	memset(&ign, 0, sizeof(ign));
// 	ign.sa_handler = SIG_IGN;
// 	sigaction (SIGQUIT, &ign, NULL);
// }

// void	set_signal( void )
// {
// 	struct sigaction	sig;

// 	ignore_quit();
// 	memset(&sig, 0, sizeof(sig));
// 	sigemptyset(&sig.sa_mask);
// 	sig.sa_handler = &sig_handler;
// 	sigaction(SIGINT, &sig, NULL);
// }

char	*dns_lookup( char *name, struct sockaddr_in *addr )
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

	char	*ret = malloc(INET_ADDRSTRLEN * sizeof(char));
	if (ret == NULL)
	{
		fprintf(stderr, "Fatal Error: Allocation error\n");
		return (NULL);
	}
	// printf("ai_flags: %d | ai_family: %d | ai_socktype: %d\n", res->ai_flags, res->ai_family, res->ai_socktype);
	// printf("ai_cannoname: [%s]\n", res->ai_canonname);
	
	addr = (struct sockaddr_in *)res->ai_addr;
	const char *tmp_s = inet_ntop(res->ai_family, &(addr->sin_addr), ret, INET_ADDRSTRLEN);
	if (tmp_s == NULL)
	{
		fprintf(stderr, "inet_ntop error: %s\n", strerror(errno));
	}
	printf("ret: [%s]\n", ret);

	printf("sin_family: [%d] | sin_port: [%d]\n", addr->sin_family, addr->sin_port);

	return (ret);
}

unsigned short checksum( void *packet, int len )
{
	unsigned short	sum = 0;
	uint8_t			*ptr = packet;

	while (len > 0)
	{
		sum = ((sum + *ptr) & 0xFF);
		ptr++;
		len--;
	}
	sum = (((sum ^ 0xFF) + 1) & 0xFF);
	return (sum);
}

void	send_ping( int sockfd, char *hostname, struct sockaddr_in *ip_addr )
{
	
	(void)hostname;
	(void)ip_addr;
	(void)sockfd;
	packet_s	packet;
	struct timeval	time;
	(void) time;
	packet.hdr.type = ICMP_ECHO;
	packet.hdr.code = 1;
	packet.hdr.un.echo.id = getpid();


	while (looping == 1)
	{
		write(1, "OK\n", 3);
		usleep(10000);
	}

	packet.hdr.checksum = checksum(&packet, sizeof(packet));
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

	set_signal();

	struct sockaddr_in	addr;

	//	Checking if argument is valid
	char *ip_addr = dns_lookup(argv[1], &addr);
	if (ip_addr == NULL)
		exit(1);

	int	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd == -1)
	{
		fprintf(stderr, "Fatal error: socket(): %s\n", strerror(errno));
		exit (1);
	}

	send_ping(sockfd, argv[1], &addr);
	
	return (0);
}