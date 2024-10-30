#include "../inc/ft_ping.h"

double	get_time_in_ms( struct timespec *time )
{
	double	res = 0;

	res = time->tv_sec * 1000000000;
	res += time->tv_nsec;
	res /= 1000000;

	return (res);
}

bool	init_values( struct sockaddr_in **to, data_s *utils )
{
	*to = calloc(1, sizeof(struct sockaddr_in));
	if (*to == NULL)
	{
		fprintf(stderr, "ft_ping: malloc: %s\n", strerror(errno));
		return (1);
	}
	
	utils->ttl = 64;
	utils->parameter = NULL;
	utils->hostname = NULL;
	utils->ip_addr = NULL;

	utils->msg_recv = 0;
	utils->msg_sent = 0;

	for (short i = 0; i < 500; i++)
	{
		utils->times_ms_list[i] = 0;
	}

	utils->t_start.tv_sec = 0;
	utils->t_start.tv_nsec = 0;
	
	utils->t_end.tv_sec = 0;
	utils->t_end.tv_nsec = 0;
	
	utils->t_min.tv_sec = 0;
	utils->t_min.tv_nsec = 0;

	utils->t_max.tv_sec = 0;
	utils->t_max.tv_nsec = 0;

	return (0);
}

void	end_program( data_s *utils, int *sockfd, struct sockaddr_in *to )
{
	if (utils != NULL)
	{
		if (utils->parameter != NULL)
			free(utils->parameter);
		if (utils->hostname != NULL)
			free(utils->hostname);
		if (utils->ip_addr != NULL)
			free(utils->ip_addr);
	}
	if (to != NULL)
		free(to);
	close(*sockfd);
}