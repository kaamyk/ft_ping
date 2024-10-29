#include "../inc/ft_ping.h"

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
	utils->hostname = ret_buf;
	
	return (0);
}

bool	dns_lookup( char *addr, struct sockaddr_in *to, data_s *utils )
{
	struct addrinfo		*res = NULL,
						hints;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	
	int status = getaddrinfo(addr, NULL, &hints, &res);
	if (status != 0)
	{
		fprintf(stderr, "ft_ping: %s: %s\n", addr, gai_strerror(status));
		return (1);
	}
	utils->parameter = strdup(addr);
	if (utils->parameter == NULL)
	{
		fprintf(stderr, "ft_ping: strdup: %s\n", strerror(errno));
		return (1);
	}
	utils->ip_addr = malloc(INET_ADDRSTRLEN);
	if (utils->ip_addr == NULL)
	{
		fprintf(stderr, "ft_ping: malloc: %s\n", strerror(errno));
		free(res);
		return (1);
	}
	to->sin_family = AF_INET;
	to->sin_port = htons(0);
	to->sin_addr = ((struct sockaddr_in *)(res->ai_addr))->sin_addr;
	const char	*tmp = inet_ntop(res->ai_family, &(to->sin_addr), utils->ip_addr, INET_ADDRSTRLEN);
	if (tmp == NULL)
	{
		fprintf(stderr, "ping: inet_ntop: %s\n", strerror(errno));
		free(res);
		return (1);
	}
	printf("to->sin_family: %d | to->sin_port: %d | to->sin_addr.s_addr: %s\n", to->sin_family, to->sin_port, tmp);
	free(res);
	return (0);
}

void	update_time( data_s *utils, struct timespec **times)
{
	double	times_elapsed_ms = 0,
			tmp_min = get_time_in_ms(&(utils->t_min)),
			tmp_max = get_time_in_ms(&(utils->t_max));
	
	times[2]->tv_nsec = times[1]->tv_nsec - times[0]->tv_nsec;
	times[2]->tv_sec = times[1]->tv_sec - times[0]->tv_sec;
	times_elapsed_ms = get_time_in_ms(times[2]);
	utils->times_ms_list[utils->msg_sent] = times_elapsed_ms;

	if (utils->t_min.tv_sec == 0.0f && utils->t_min.tv_nsec == 0.0f)
	{
		utils->t_min.tv_sec = times[2]->tv_sec;
		utils->t_min.tv_nsec = times[2]->tv_nsec;

		utils->t_max.tv_sec = times[2]->tv_sec;
		utils->t_max.tv_nsec = times[2]->tv_nsec;

		utils->t_begin.tv_sec = times[0]->tv_sec;
		utils->t_begin.tv_nsec = times[0]->tv_nsec;
	}
	else
	{	
		if (times_elapsed_ms < tmp_min)
		{
			utils->t_min.tv_sec = times[1]->tv_sec - times[0]->tv_sec;
			utils->t_min.tv_nsec = times[1]->tv_nsec - times[0]->tv_nsec;
		}
		else if (times_elapsed_ms > tmp_max)
		{
			utils->t_max.tv_sec = times[1]->tv_sec - times[0]->tv_sec;
			utils->t_max.tv_nsec = times[1]->tv_nsec - times[0]->tv_nsec;
		}
	}
}

bool	set_up_socket( int *sockfd, data_s *utils )
{
	struct timeval	timeout;

	timeout.tv_sec = 2;
	timeout.tv_usec = 0;

	if (setsockopt(*sockfd, SOL_IP, IP_TTL, &utils->ttl, sizeof(int)) == -1)
	{
		fprintf(stderr, "ft_ping: setsockopt(SOL_IP): %s\n", strerror(errno));
		return (1);
	}
	if (setsockopt(*sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(struct timeval)) == -1)
	{
		fprintf(stderr, "ft_ping: setsockopt(SOL_SOCKET): %s\n", strerror(errno));
		return (1);
	}
	return (0);
}

bool	init_clocks( struct timespec **times )
{
	for (uint8_t i = 0; i < 3; i++)
	{
		times[i] = malloc(sizeof(struct timespec));
		if (times[i] == NULL)
		{
			fprintf(stderr, "ft_ping: init_clocks: %s\n", strerror(errno));
			return (1);
		}
		memset(times[i], 0, sizeof(struct timespec));
	}
	return (0);
}

void	free_clocks( struct timespec **times)
{
	uint8_t	i = 0;
	while (i < 3 && *(times + i) != NULL)
	{
		free(*(times + i));
		i++;
	}
}

void	init_packet( packet_s *packet )
{
	memset(packet, 0, sizeof(packet_s));
	packet->hdr.type = ICMP_ECHO;
	packet->hdr.code = 8;
	packet->hdr.un.echo.id = rand();
	packet->hdr.un.echo.sequence = 0;
	memset(packet->msg, '0', sizeof(packet->msg) - 2);
	packet->msg[sizeof(packet->msg) - 1] = 0;
}


bool	send_ping( int *sockfd, struct sockaddr_in *to, data_s *utils )
{
	char	r_buf[84];
	ssize_t	ret = 0;
	struct timespec	*times[3];	// 0: start, 1: end, 2: elapsed
	init_clocks(times);
	struct iphdr	*r_ip = NULL;
	// struct icmphdr	*r_icmp = NULL;
	packet_s	packet;
	init_packet(&packet);
	if (packet.hdr.code != 8 || packet.hdr.type != ICMP_ECHO)
	{
		fprintf(stderr, "ft_ping: init_packet: initialization failed.\n");
		printf("packet.hdr.code == %d | packet.hdr.type == %d\n", packet.hdr.code, packet.hdr.type);
		free_clocks(times);
		return (1);
	}
	
	while (g_looping)
	{
		init_packet(&packet);
		if (packet.hdr.code != 8 || packet.hdr.type != ICMP_ECHO)
		{
			fprintf(stderr, "ft_ping: init_packet: initialization failed.\n");
			free_clocks(times);
			return (1);
		}
		packet.hdr.un.echo.sequence = htons(utils->msg_sent);
		packet.hdr.checksum = checksum(&packet, sizeof(packet_s));
		clock_gettime(CLOCK_MONOTONIC, times[0]);
		ret = sendto(*sockfd, &packet, sizeof(packet_s), 0, (struct sockaddr *) to, sizeof(struct sockaddr));
		if (ret == -1)
		{
			fprintf(stderr, "ping: sendto: (%d)%s\n", errno, strerror(errno));
			sleep(1);
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				continue ;
			else
			{
				free_clocks(times);
				return (1);
			}
		}
		ret = recvfrom(*sockfd, r_buf, 84, 0, NULL, NULL);
		if (ret == -1)
		{
			fprintf(stderr, "ping: recvfrom: %s\n", strerror(errno));
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				continue ;
			else
			{
				free_clocks(times);
				return (1);
			}
		}
		clock_gettime(CLOCK_MONOTONIC, times[1]);
		r_ip = (struct iphdr *) r_buf;
		// r_icmp = (struct icmphdr *) r_buf + sizeof(struct iphdr);
		// printf("r_icmp->type == %d | r_icmp->().id == %d\n",r_icmp->type , r_icmp->un.echo.id);
		update_time(utils, times);
		printf("%ld bytes from %s: icmp_seq=%d ttl=%d time=%.2f ms\n", 
				ret - sizeof(struct iphdr), utils->ip_addr, utils->msg_sent, r_ip->ttl, get_time_in_ms(times[2]));
		packet.hdr.un.echo.sequence = (utils->msg_sent)++;
		sleep(1);
		// if (utils->msg_sent == 2)
		// 	exit(0);
	}
	clock_gettime(CLOCK_MONOTONIC, &(utils->t_finish));
	print_end(utils);
	free_clocks(times);
	return (0);
}

int	main ( int argc, char **argv )
{
	int	exit_value = 0;
	struct sockaddr_in	*to = NULL;
	data_s	utils;	
	if (init_values(&to, &utils) == 1)
		exit (1);	
	
	set_signal();

	--argc;
	++argv;
	exit_value = parsing(&argc, &argv);
	if (exit_value != 0)
		exit(exit_value);

	exit_value = dns_lookup(argv[0], to, &utils);
	if (exit_value != 0)
		exit (exit_value);

	exit_value = reverse_dns_lookup(utils.ip_addr, &utils);
	if (exit_value != 0)
		exit(exit_value);

	printf("FT_PING %s (%s): %d data bytes\n", utils.parameter, utils.ip_addr, ICMP_PAYLOAD_SIZE);

	int	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd == -1)
	{
		fprintf(stderr, "ft_ping: socket: %s\n", strerror(errno));
		end_program(&utils, &sockfd, to);
		exit(1);	
	}
	if (set_up_socket(&sockfd, &utils))
		return (1);

	exit_value = send_ping(&sockfd, to, &utils);
	if (exit_value != 0)
	{
		end_program(&utils, &sockfd, to);
		exit (1);
	}

	end_program(&utils, &sockfd, to);
	close(sockfd);
	
	return (0);
}