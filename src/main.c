#include "../inc/ft_ping.h"
#include <sys/time.h>

bool	g_looping = 1;
int		g_flags = 0;

unsigned short checksum(void *b, int len)
{
	unsigned short *buf = b;
	unsigned int sum = 0;
	unsigned short result;

	for (sum = 0; len > 1; len -= 2)
		sum += *buf++;
	if (len == 1)
		sum += *(unsigned char *)buf;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	result = ~sum;
	return result;
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

bool	reverse_dns_lookup( char *ip_addr, data_s *utils )
{
	int	len = sizeof(struct sockaddr_in);
	char	buf[NI_MAXHOST],
			*ret_buf;
	struct sockaddr_in	tmp;
	tmp.sin_family = AF_INET;
	tmp.sin_addr.s_addr = inet_addr(ip_addr);
	
	int ret = getnameinfo((struct sockaddr *) &tmp, len, buf, sizeof(buf), NULL,0, NI_NAMEREQD);
	if (ret != 0)
	{
		fprintf(stderr, "ft_ping: getnameinfo: %s\n", strerror(errno));
		return(1);
	}

	ret_buf = (char *)malloc(strlen(buf) + 1);
	if (ret_buf == NULL)
	{
		fprintf(stderr, "ft_ping: malloc: %s\n", strerror(errno));
		return (1);
	}
	strcpy(ret_buf, buf);
	printf("ret_buf: [%s]\n", ret_buf);
	utils->hostname = ret_buf;
	
	return (0);
}

bool	dns_lookup( char *addr, struct sockaddr_in *to, data_s *utils )
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
	utils->parameter = strdup(addr);
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

void	update_time( data_s *utils, struct timeval *time_elapsed, struct timeval *t_start, struct timeval *t_end )
{
	double	t_elapsed_ms = 0,
			tmp_min = ((utils->t_min.tv_sec * 1000) + (utils->t_min.tv_usec / 1000)),
			tmp_max = ((utils->t_max.tv_sec * 1000) + (utils->t_max.tv_usec / 1000));
	
	time_elapsed->tv_usec = t_end->tv_usec - t_start->tv_usec;
	time_elapsed->tv_sec = t_end->tv_sec - t_start->tv_sec;

	if (utils->t_min.tv_sec == 0 && utils->t_min.tv_usec == 0)
	{
		utils->t_min.tv_sec = t_end->tv_sec - t_start->tv_sec;
		utils->t_min.tv_usec = t_end->tv_usec - t_start->tv_usec;
		utils->t_max.tv_sec = t_end->tv_sec - t_start->tv_sec;
		utils->t_max.tv_usec = t_end->tv_usec - t_start->tv_usec;
	}
	else
	{	
		t_elapsed_ms = ((time_elapsed->tv_sec * 1000) + (time_elapsed->tv_usec / 1000));
		if (t_elapsed_ms < tmp_min)
		{
			utils->t_min.tv_sec = t_end->tv_sec - t_start->tv_sec;
			utils->t_min.tv_usec = t_end->tv_usec - t_start->tv_usec;
		}
		else if (t_elapsed_ms > tmp_max)
		{
			utils->t_max.tv_sec = t_end->tv_sec - t_start->tv_sec;
			utils->t_max.tv_usec = t_end->tv_usec - t_start->tv_usec;
		}
	}
}

bool	send_ping( int *sockfd, struct sockaddr_in *to, struct sockaddr_in *from, data_s *utils )
{
	printf("In send_ping\n");
	(void) from;
	char		r_buf[84];
	unsigned int	msg_sent = 0;
	int 	ttl = 3;
	ssize_t	ret = 0;
	struct timeval	t_start,
					t_end,
					tv_out,
					time_elapsed;
	packet_s	packet;
	packet.hdr.type = ICMP_ECHO;
	packet.hdr.code = 1;
	packet.hdr.un.echo.id = getpid();
	memset(packet.msg, '0', ICMP_PAYLOAD_SIZE - 2);
	packet.msg[ICMP_PAYLOAD_SIZE - 1] = 0;

	if (setsockopt(*sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) == -1)
	{
		fprintf(stderr, "ft_ping: setsockopt1: %s\n", strerror(errno));
		return (1);
	}
	if (setsockopt(*sockfd, SOL_SOCKET, RECV_TIMEOUT, &tv_out, sizeof(tv_out)) == -1)
	{
		fprintf(stderr, "ft_ping: setsockopt: %s\n", strerror(errno));
		return (1);
	}

	while (g_looping)
	{
		packet.hdr.un.echo.sequence = msg_sent++;
		packet.hdr.checksum = checksum(&packet, sizeof(packet));
		gettimeofday(&t_start, NULL);
		ret = sendto(*sockfd, &packet, sizeof(packet_s), 0, (struct sockaddr *) to, sizeof(struct sockaddr));
		if (ret == -1)
		{
			fprintf(stderr, "ping: sendto: (%d)%s\n", errno, strerror(errno));
			sleep(1);
			continue ;
		}
		// socklen = sizeof(struct sockaddr);
		ret = recvfrom(*sockfd, r_buf, IP_PACKET_SIZE, 0, NULL, NULL);
		// ret = recvfrom(*sockfd, r_buf, IP_PACKET_SIZE, 0, (struct sockaddr *) from, &socklen);
		if (ret == -1)
		{
			fprintf(stderr, "ping: recvfrom: %s\n", strerror(errno));
		}
		gettimeofday(&t_end, NULL);
		struct iphdr	*r_ip = (struct iphdr *) r_buf;
		struct icmphdr	*r_icmp = (struct icmphdr *) r_buf + sizeof(struct iphdr);
		if (r_icmp->un.echo.id == 0)
		{
			printf("r_icmp.id == 0\n");
		}
		update_time(utils, &time_elapsed, &t_start, &t_end);
		printf("%ld bytes from %s (%s): icmp_seq=%d ttl=%d time=%f ms\n", 
				ret - sizeof(struct iphdr), utils->hostname, utils->ip_addr, msg_sent, r_ip->ttl, (double)((time_elapsed.tv_usec / 1000) + (time_elapsed.tv_sec * 1000)));
		sleep(1);
	}
	print_end(utils);
	return (0);
}

void	init_values( data_s *utils )
{
	utils->parameter = NULL;
	utils->hostname = NULL;
	utils->ip_addr = NULL;
	utils->msg_recv = 0;
	utils->msg_sent = 0;
	utils->t_start.tv_sec = 0;
	utils->t_start.tv_usec = 0;
	utils->t_end.tv_sec = 0;
	utils->t_end.tv_usec = 0;
	utils->t_min.tv_sec = 0;
	utils->t_min.tv_usec = 0;
	utils->t_max.tv_sec = 0;
	utils->t_max.tv_usec = 0;
}

int	main ( int argc, char **argv )
{
	int	exit_value = 0;
	struct sockaddr_in	to;
	struct sockaddr_in	from;
	data_s	utils;	
	init_values(&utils);
	
	set_signal();

	--argc;
	++argv;
	exit_value = parsing(&argc, &argv);
	if (exit_value != 0)
	{
		exit(exit_value);
	}

	exit_value = dns_lookup(argv[0], &to, &utils);
	if (exit_value != 0)
	{
		exit (exit_value);
	}
	printf("After define_value(): utils.ip_addr:[%s] | utils.hostname:[%s] \n", 
			utils.ip_addr, utils.hostname);

	exit_value = reverse_dns_lookup(utils.ip_addr, &utils);
	if (exit_value != 0)
	{
		exit(exit_value);
	}

	printf("FT_PING %s (%s) %d(%d) bytes of data.\n", utils.parameter, utils.ip_addr, ICMP_PAYLOAD_SIZE, IP_PACKET_SIZE);

	// struct protoent	*proto = getprotobyname("icmp");
	// if (proto == NULL)
	// {
	// 	fprintf(stderr, "ft_ping: getprotobyname: %s\n", strerror(errno));
	// }
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
		free(utils.hostname);
		free(utils.ip_addr);
		exit (1);
	}

	free(utils.hostname);
	free(utils.ip_addr);
	close(sockfd);
	
	return (0);
}