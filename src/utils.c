#include "../inc/ft_ping.h"

double	get_time_in_ms( struct timeval *time )
{
	double	res = 0;

	res = time->tv_sec * 1000000;
	res += time->tv_usec;
	res /= 1000;

	return (res);
}

void	init_values( data_s *utils )
{
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
	utils->t_start.tv_usec = 0;
	
	utils->t_end.tv_sec = 0;
	utils->t_end.tv_usec = 0;
	
	utils->t_min.tv_sec = 0;
	utils->t_min.tv_usec = 0;

	utils->t_max.tv_sec = 0;
	utils->t_max.tv_usec = 0;
}

void	end_program( data_s *utils, int *sockfd )
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
	close(*sockfd);
}