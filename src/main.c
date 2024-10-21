#include "../inc/ft_ping.h"

bool	g_looping = 1;
int		g_flags = 0;

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

bool	parsing( int *argc, char ***argv )
{
	while (*argc > 0 && ***argv == '-')
	{
		while (***argv)
		{
			switch (***argv)
			{
				case 'v':
					g_flags |= VERBOSE;
					break ;
			}
			++(**argv);
		}
		--(*argc);
		++(*argv);
	}
	if (*argc != 1)
	{
		if (*argc < 1)
		{
			fprintf(stderr, "ft_ping: usage error: Destination address required\n");
			return (1);
		}
		else if (*argc > 1)
		{
			fprintf(stderr, "ft_ping: usage error: Only one address required\n");
			return (1);
		}
	}
	return (0);
}

bool	define_address( char *addr, struct sockaddr_in *to, data_s *utils )
{
	struct addrinfo	*res;
	struct addrinfo	hints;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	
	int status = getaddrinfo(addr, NULL, &hints, &res);
	if (status != 0)
	{
		fprintf(stderr, "ping: %s: %s\n", addr,gai_strerror(status));
		return (1);
	}
	utils->hostname = strdup(addr);
	utils->ip_addr = malloc(INET_ADDRSTRLEN);
	if (utils->ip_addr == NULL)
	{
		fprintf(stderr, "ping: malloc: %s\n", strerror(errno));
		free(res);
		return (1);
	}
	to = (struct sockaddr_in *) res->ai_addr;
	const char	*tmp = inet_ntop(res->ai_family, &(to->sin_addr), utils->ip_addr, INET_ADDRSTRLEN);
	if (tmp == NULL)
	{
		fprintf(stderr, "ping: inet_ntop: %s\n", strerror(errno));
		free(res);
		return (1);
	}
	free(res);
	return (0);
}

bool	send_ping( int *sockfd, struct sockaddr_in *to, struct sockaddr_in *from, data_s *utils )
{
	(void) utils;
	unsigned int	msg_sent = 0;
	ssize_t		ret = 0;
	socklen_t	socklen = 0;
	struct timespec	t_start,
					t_end;
	char		r_buf[84];
	packet_s	packet;
	packet.hdr.type = ICMP_ECHO;
	packet.hdr.code = 1;
	packet.hdr.un.echo.id = getpid();
	memset(packet.msg, '0', sizeof(packet.msg));

	while (g_looping)
	{
		packet.hdr.un.echo.sequence = msg_sent++;
		packet.hdr.checksum = checksum(&packet, sizeof(packet));

		clock_gettime(CLOCK_MONOTONIC, &t_start);

		sockopt();
		
		ret = sendto(*sockfd, &packet, sizeof(packet), 0, (struct sockaddr *) to, sizeof(to));
		if (ret == -1)
		{
			fprintf(stderr, "ping: sendto: (%d)%s\n", errno, strerror(errno));
			sleep(1);
			continue ;
		}

		socklen = sizeof(*from);
		ret = recvfrom(*sockfd, r_buf, IP_PACKET_SIZE, 0, (struct sockaddr *) from, &socklen);
		if (ret == -1)
		{
			fprintf(stderr, "ping: recvfrom: %s\n", strerror(errno));
		}

		clock_gettime(CLOCK_MONOTONIC, &t_end);
		sleep(1);
		++msg_sent;
	}

	return (0);
}

int	main ( int argc, char **argv )
{
	int	exit_value = 0;
	struct sockaddr_in	to;
	struct sockaddr_in	from;
	data_s	utils;
	
	set_signal();

	--argc;
	++argv;
	exit_value = parsing(&argc, &argv);
	if (exit_value != 0)
	{
		exit(exit_value);
	}

	exit_value = define_address(argv[0], &to, &utils);
	if (exit_value != 0)
	{
		exit (1);
	}
	printf("After define_value(): utils.ip_addr:[%s] | utils.hostname:[%s] \n", 
			utils.ip_addr, utils.hostname);

	int	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd == -1)
	{
		fprintf(stderr, "ping: socket: %s\n", strerror(errno));
		free(utils.hostname);
		free(utils.ip_addr);
		exit(1);	
	}

	exit_value = send_ping(&sockfd, &to, &from, &utils);
	if (exit_value != 0)
	{
		exit (1);
	}

	free(utils.hostname);
	free(utils.ip_addr);
	
	return (0);
}