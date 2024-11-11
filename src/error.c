#include "../inc/ft_ping.h"

bool	return_error( char *err_mess )
{
	if (errno)
		perror(err_mess);
	else
		fprintf(stderr, "%s\n", err_mess);
	return (1);
}


// bool	reverse_dns_lookup( char *ip_addr, data_s *utils )
// {
// 	int	len = sizeof(struct sockaddr_in);
// 	char	buf[NI_MAXHOST],
// 			*ret_buf;
// 	struct sockaddr_in	tmp;
// 	tmp.sin_family = AF_INET;
// 	tmp.sin_addr.s_addr = inet_addr(ip_addr);
	
// 	int ret = getnameinfo((struct sockaddr *) &tmp, len, buf, sizeof(buf), NULL,0, NI_NAMEREQD);
// 	if (ret != 0)
// 		return_error("ft_ping: getnameinfo");

// 	ret_buf = (char *)malloc(strlen(buf) + 1);
// 	if (ret_buf == NULL)
// 		return_error("ft_ping: malloc");
// 	strcpy(ret_buf, buf);
// 	utils->hostname = ret_buf;
	
// 	return (0);
// }



bool	get_hostname( char *buf_host , char *ip_addr )
{
	struct sockaddr_in	tmp;
	tmp.sin_family = AF_INET;
	tmp.sin_addr.s_addr = inet_addr(ip_addr);
	const int ret_val = getnameinfo((struct sockaddr *) &tmp, sizeof(struct sockaddr_in), buf_host, NI_MAXHOST, NULL, 0, NI_NAMEREQD);
	if (ret_val != 0)
	{
		switch (ret_val)
		{
			case EAI_AGAIN:
				printf("EAI_AGAIN\n");
				break;
			case EAI_BADFLAGS:
				printf("EAI_BADFLAGS\n");
				break;
			case EAI_FAIL:
				printf("EAI_FAIL\n");
				break;
			case EAI_FAMILY:
				printf("EAI_FAMILY\n");
				break;
			case EAI_MEMORY:
				printf("EAI_MEMORY\n");
				break;
			case EAI_NONAME:
				printf("EAI_NONAME\n");
				break;
			case EAI_OVERFLOW:
				printf("EAI_OVERFLOW\n");
				break;
			case EAI_SYSTEM:
				printf("EAI_SYSTEM\n");
				break;
			default :
				printf("default\n");
				break;
		}
		return_error("ft_ping: getnameinfo");
	}
	return (0);
}

bool	handle_error_packet( struct ip *err_ip_packet, struct icmp *err_icmp_packet, const ssize_t ret )
{
	(void) err_ip_packet;
	char	buf_ip[INET_ADDRSTRLEN] = {0};
	if (inet_ntop(AF_INET, &err_ip_packet->ip_src.s_addr, buf_ip, INET_ADDRSTRLEN) == NULL)
		return_error("ft_ping: handle_error_packet: inet_ntop");
	
	char	buf_host[NI_MAXHOST] = {0};
	if (get_hostname(buf_host, buf_ip) == 1)
		return (1);
	
	fprintf(stderr, "%ld bytes from %s (%s):", 
					ret - sizeof(struct iphdr), buf_host, buf_ip);
	
	switch (err_icmp_packet->icmp_type)
	{
		case ICMP_DEST_UNREACH:
			printf("Time to live exceeded\n");
			break ;
		case ICMP_SOURCE_QUENCH:
			fprintf(stderr, "Source Quench");
			break;
		case ICMP_REDIRECT:
			fprintf(stderr, "Redirect");
			break ;
		case ICMP_TIME_EXCEEDED:
			fprintf(stderr, "Time to live exceeded\n");
			break ;
		case ICMP_PARAMETERPROB:
			fprintf(stderr, "Parameter Problem");
			break ;
		case ICMP_TIMESTAMP:
			fprintf(stderr, "Timestamp Request");
			break ;
		case ICMP_TIMESTAMPREPLY:
			fprintf(stderr, "Timestamp Reply");
			break ;
		case ICMP_INFO_REQUEST:
			fprintf(stderr, "Information Request");
			break ;
		case ICMP_INFO_REPLY:
			fprintf(stderr, "Information Reply");
			break ;
		case ICMP_ADDRESS:
			fprintf(stderr, "Address Mask Request");
			break ;
		case ICMP_ADDRESSREPLY:
			fprintf(stderr, "Address Mask Reply");
			break ;
		default :
			printf("Unknown error code\n");
			break ;
	}
	return (0);
}