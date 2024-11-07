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
	
	utils->id = (uint16_t)getpid();
	utils->ttl = 3;
	// utils->ttl = IPDEFTTL;
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

bool	init_clocks( struct timespec **times )
{
	for (uint8_t i = 0; i < 3; i++)
	{
		times[i] = malloc(sizeof(struct timespec));
		if (times[i] == NULL)
			return_error(strcat("ft_ping: init_clocks: ", strerror(errno)));
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

void	update_time( data_s *utils, struct timespec **times)
{
	double	times_elapsed_ms = 0,
			tmp_min = get_time_in_ms(&(utils->t_min)),
			tmp_max = get_time_in_ms(&(utils->t_max));
	
	times[2]->tv_nsec = times[1]->tv_nsec - times[0]->tv_nsec;
	times[2]->tv_sec = times[1]->tv_sec - times[0]->tv_sec;
	times_elapsed_ms = get_time_in_ms(times[2]);
	(utils->times_ms_list)[utils->msg_sent - 1] = times_elapsed_ms;

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