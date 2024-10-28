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
		fprintf(stderr, "ft_ping: calc_average_time: total number of times is 0\n");
		res = 0;
	}
	return (res);
}

void	print_end( data_s *utils )
{
	double			time_ms[4] = {0};
	unsigned int	pack_lost = 0;

	if (utils->msg_recv + utils->msg_sent != 0)
	{
		pack_lost = ((utils->msg_sent - utils->msg_recv) / (utils->msg_recv + utils->msg_sent)) * 100;
	}
	
	printf("--- %s ft_ping statistics ---\n%d packets transmitted, %d packets received, %d%% packet loss\n",
			utils->parameter, utils->msg_sent, utils->msg_recv, pack_lost);
	time_ms[0] = get_time_in_ms(&(utils->t_min));
	printf("t_min == %.2f\n", time_ms[0]);
	time_ms[1] = calc_average_time(utils->times_ms_list);
	printf("t_avg == %.2f\n", time_ms[1]);
	time_ms[2] = get_time_in_ms(&(utils->t_max));
	printf("t_kmax == %.2f\n", time_ms[2]);
	time_ms[3] = get_time_in_ms(&(utils->t_finish)) - get_time_in_ms(&(utils->t_begin));
	printf("total == %.2f\n", time_ms[3]);

	printf("round-trip min/avg/max/stddev = %.2f/%.2f/%.2f/%.2f\n",
			time_ms[0], time_ms[1], time_ms[2], time_ms[3]);
}