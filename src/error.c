#include "../inc/ft_ping.h"

bool	return_error( char *err_mess )
{
	if (errno)
		perror(err_mess);
	else
		fprintf(stderr, "%s\n", err_mess);
	return (1);
}

bool	get_hostname( char *buf_host , char *ip_addr )
{
	struct sockaddr_in	tmp;
	tmp.sin_family = AF_INET;
	tmp.sin_addr.s_addr = inet_addr(ip_addr);
	(void)getnameinfo((struct sockaddr *) &tmp, sizeof(struct sockaddr_in), buf_host, NI_MAXHOST, NULL, 0, NI_NAMEREQD);

	return (0);
}

bool	print_error_verbose( const char *r_buf )
{
	struct ip	*tmp_ip = (struct ip *)(r_buf + sizeof(struct ip) + sizeof(struct icmphdr));
	uint8_t	flags = tmp_ip->ip_off >> 5;
	char	*tmp = (char *)r_buf + sizeof(struct ip) + sizeof(struct icmphdr);
	uint8_t	len = (tmp_ip->ip_hl * 4) + sizeof(struct icmphdr *) + (tmp_ip->ip_hl * 4) + 8;

	fprintf(stderr, "IP Hdr Dump:\n ");
	for (uint8_t i = 0; i < 10; i++, tmp += 2)
		fprintf(stderr, "%.02hhx%.02hhx ", tmp[0], tmp[1]);

	char	buf_ip[2][INET_ADDRSTRLEN] = {0};	// [0]: source, [1]: destination
	if (inet_ntop(AF_INET, &tmp_ip->ip_src.s_addr, buf_ip[0], INET_ADDRSTRLEN) == NULL)
		return_error("ft_ping: handle_error_packet: inet_ntop");
	if (inet_ntop(AF_INET, &tmp_ip->ip_dst.s_addr, buf_ip[1], INET_ADDRSTRLEN) == NULL)
		return_error("ft_ping: handle_error_packet: inet_ntop");

	fprintf(stderr, "\nVr HL TOS  Len   ID Flg  off TTL Pro  cks      Src      Dst     Data\n");
	fprintf(stderr, "%2hd %2hd  %02hhd %04hd %04x %3hhu %04hd  %02hhd  %02hhd %4hx %s  %s\n", 
						tmp_ip->ip_v, tmp_ip->ip_hl, tmp_ip->ip_tos, len , 
						htons(tmp_ip->ip_id), flags, htons(tmp_ip->ip_off) & IP_OFFMASK, tmp_ip->ip_ttl, tmp_ip->ip_p, 
						htons(tmp_ip->ip_sum), buf_ip[0], buf_ip[1]);

	//	fprintf(icmp);
	struct icmp	*tmp_icmp = (struct icmp *)(r_buf + len - 8);
	fprintf(stderr, "ICMP: type %hhu, code %hhu, size %d, id 0x%x, seq 0x%04x\n", 
					tmp_icmp->icmp_type, tmp_icmp->icmp_code, ICMP_PACKET_SIZE, tmp_icmp->icmp_id, tmp_icmp->icmp_seq);

	return (0);
}
 
bool	handle_error_packet( const struct ip *err_ip_packet, const struct icmp *err_icmp_packet, const char *r_buf, const ssize_t ret )
{
	(void) err_ip_packet;
	char	buf_ip[INET_ADDRSTRLEN] = {0};
	if (get_str_ip_addr(buf_ip, &err_ip_packet->ip_src) == 1)
		return (1);	
	
	char	buf_host[NI_MAXHOST] = {0};
	if (get_hostname(buf_host, buf_ip) == 1)
		return (1);
	
	fprintf(stderr, "%ld bytes from %s (%s): ", 
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
	if (g_flags & VERBOSE)
		print_error_verbose(r_buf);
	return (0);
}