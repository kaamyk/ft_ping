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
		return_error("ft_ping: getnameinfo");

	ret_buf = (char *)malloc(strlen(buf) + 1);
	if (ret_buf == NULL)
		return_error("ft_ping: malloc");
	strcpy(ret_buf, buf);
	utils->hostname = ret_buf;
	
	return (0);
}

bool	dns_lookup( struct sockaddr_in *to, data_s *utils )
{
	struct addrinfo		*res = NULL,
						hints;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	
	int status = getaddrinfo(utils->parameter, NULL, &hints, &res);
	if (status != 0)
	{
		fprintf(stderr, "ft_ping: %s: %s\n", utils->parameter, gai_strerror(status));
		return (1);
	}
	to->sin_family = AF_INET;
	to->sin_port = htons(0);
	to->sin_addr = ((struct sockaddr_in *)(res->ai_addr))->sin_addr;
	utils->ip_addr = malloc(INET_ADDRSTRLEN);
	if (utils->ip_addr == NULL)
	{
		free(res);
		return_error("ft_ping: malloc");
	}
	strcpy(utils->ip_addr, inet_ntoa(to->sin_addr));
	// const char	*tmp = inet_ntop(res->ai_family, &(to->sin_addr), utils->ip_addr, INET_ADDRSTRLEN);
	free(res);
	return (0);
}

bool	set_up_socket( int *sockfd, data_s *utils )
{
	struct timeval	timeout;

	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	*sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (*sockfd == -1)
		return_error("ft_ping: socket: ");

	if (setsockopt(*sockfd, SOL_IP, IP_TTL, &(utils->ttl), (socklen_t)sizeof(utils->ttl)) == -1)
		return_error("ft_ping: setsockopt(SOL_IP)");
	if (setsockopt(*sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout)) == -1)
		return_error("ft_ping: setsockopt(SOL_SOCKET)");
	return (0);
}

void	init_packet( packet_s *packet, data_s *utils )
{
	bzero(packet, sizeof(packet_s));
	packet->hdr.type = ICMP_ECHO;
	packet->hdr.code = 0;
	packet->hdr.un.echo.id = utils->id;
	memset(packet->msg, '0', sizeof(packet->msg));
	packet->msg[sizeof(packet->msg) - 1] = 0;
}

bool	handle_return_packet( ssize_t ret, const char *r_buf, data_s *utils, struct timespec *times[3] )
{
	struct ip	*r_ip = NULL;
	struct icmp	*r_icmp = NULL;

	r_ip = (struct ip *) r_buf;
	r_icmp = (struct icmp *)(r_buf + sizeof(struct iphdr));
	char	buf_ip[INET_ADDRSTRLEN] = {0};
	if (get_str_ip_addr(buf_ip, &r_ip->ip_src) == 1)
	{
		free_clocks(times);
		return (1);
	}
	if (strcmp(utils->ip_addr, buf_ip) == 0)
		utils->msg_recv += 1;

	if (r_icmp->icmp_type != 8)
	{
		if (r_icmp->icmp_type != 0)
		{
			if (get_str_ip_addr(buf_ip, &r_ip->ip_src) == 1)
			{
				free_clocks(times);
				return (1);	
			}
			// printf("utils->hostname == %s", utils->hostname);
			if (strcmp(utils->hostname, "localhost") != 0 && check_id(r_buf, utils, r_ip) == 1)
			{
				free_clocks(times);
				fprintf(stderr, "ft_ping: check_id: IDs do not match.\n");
				return (1);
			}	
			handle_error_packet(r_ip, r_icmp, r_buf, ret);
		}
		else
		{
			update_time(utils, times);
			print_sequence( &ret, r_ip, r_icmp, utils, times[2]);
		}
		sleep(1);
	}
	return (0);
}

bool	send_ping( int *sockfd, struct sockaddr_in *to, data_s *utils )
{
	char	r_buf[84] = {0};
	ssize_t	ret = 0;
	struct timespec	*times[3];	// 0: start, 1: end, 2: elapsed
	init_clocks(times);
	packet_s	packet;

	while (g_looping)
	{
		init_packet(&packet, utils);
		packet.hdr.un.echo.sequence = htons(utils->msg_sent);
		packet.hdr.checksum = checksum(&packet, sizeof(packet_s));
		
		clock_gettime(CLOCK_MONOTONIC, times[0]);
		ret = sendto(*sockfd, &packet, sizeof(packet_s), 0, (struct sockaddr *) to, sizeof(struct sockaddr));
		if (ret == -1)
		{
			fprintf(stderr, "ft_ping: sendto: (%d)%s\n", errno, strerror(errno));
			if (errno != EAGAIN && errno != EWOULDBLOCK)
			{
				free_clocks(times);
				return (1);
			}
		}
		utils->msg_sent += 1;
		bzero(r_buf, 84);
		
		ret = recvfrom(*sockfd, r_buf, sizeof(r_buf), 0, NULL, NULL);
		if (ret == -1)
		{
			if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR)
			{
				fprintf(stderr, "ft_ping: recvfrom: %s\n", strerror(errno));
				free_clocks(times);
				return (1);
			}
			continue ;
		}
		clock_gettime(CLOCK_MONOTONIC, times[1]);
		if (handle_return_packet( ret, r_buf, utils, times) == 1)
			return (1);
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
	exit_value = parsing(&argc, &argv, &utils);
	if (exit_value != 0)
	{
		end_program(&utils, NULL, to);		
		exit(exit_value);
	}

	exit_value = dns_lookup(to, &utils);
	if (exit_value != 0)
	{
		end_program(&utils, NULL, to);
		exit (exit_value);
	}

	exit_value = reverse_dns_lookup(utils.ip_addr, &utils);
	if (exit_value != 0)
	{
		end_program(&utils, NULL, to);
		exit (exit_value);
	}

	printf("FT_PING %s (%s): %d data bytes", utils.parameter, utils.ip_addr, ICMP_PAYLOAD_SIZE);
	if ((g_flags & VERBOSE))
		printf(", id 0x%x = %d", utils.id, utils.id);
	printf("\n");

	int	sockfd = 0;
	if (set_up_socket(&sockfd, &utils) == 1)
	{
		end_program(&utils, &sockfd, to);
		return (1);
	}

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