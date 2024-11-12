#include "../inc/ft_ping.h"

double	calc_average_time( double *times )
{
	double	res  = 0;
	short	i = 0;

	while (i < 500 && times[i] != 0)
	{
		res += times[i];
		i++;
	}
	if (i != 0)
		res /= i;
	else
	{
		// fprintf(stderr, "ft_ping: calc_average_time: total number of times is 0\n");
		res = 0;
	}
	return (res);
}

double	calc_std_dev( double mean, double *times )
{
	double	res = 0;
	unsigned short i = 0;

	while (i < 500 && times[i] != 0.0f)
	{
		res += pow( mean - times[i], 2);
		i++;
	}
	res = sqrt(res / i);
	
	return (res);
}

void	print_end( data_s *utils )
{
	double	time_ms[4] = {0},
			pack_lost = 0;

	if (utils->msg_recv + utils->msg_sent != 0)
	{
		pack_lost = 100 - (( (double) utils->msg_recv / (double) utils->msg_sent ) * 100);
	}
	
	printf("--- %s ft_ping statistics ---\n%d packets transmitted, %d packets received, %.3f%% packet loss\n",
			utils->parameter, utils->msg_sent, utils->msg_recv, pack_lost);
	time_ms[0] = get_time_in_ms(&(utils->t_min));
	time_ms[1] = calc_average_time(utils->times_ms_list);
	if (time_ms[1] == 0.0f)
		return ;
	time_ms[2] = get_time_in_ms(&(utils->t_max));
	time_ms[3] = calc_std_dev(time_ms[1], utils->times_ms_list);

	printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
			time_ms[0], time_ms[1], time_ms[2], time_ms[3]);
}

void	print_sequence( ssize_t *ret, struct ip *r_ip, struct icmp *r_icmp, data_s *utils, struct timespec *time_elapsed )
{
	utils->sequence = ntohs(r_icmp->icmp_seq);
	if (r_icmp->icmp_type == ICMP_TIME_EXCEEDED)
		printf("ICMP_TIME_EXCEEDED\n");
	else if (r_icmp->icmp_type == ICMP_DEST_UNREACH)
		printf("ICMP_DEST_UNREACH\n");

	char	buf_ip[INET_ADDRSTRLEN] = {0};	// source address
	if (inet_ntop(AF_INET, &r_ip->ip_src.s_addr, buf_ip, INET_ADDRSTRLEN) == NULL)
		return_error("ft_ping: handle_error_packet: inet_ntop");
	
	printf("%ld bytes from %s: icmp_seq=%hu ttl=%d time=%.3f ms\n", 
			*ret - sizeof(struct iphdr), buf_ip, htons(r_icmp->icmp_seq), r_ip->ip_ttl, get_time_in_ms(time_elapsed));
}