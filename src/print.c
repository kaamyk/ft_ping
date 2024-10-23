#include "../inc/ft_ping.h"

void	print_end( data_s *utils )
{
	unsigned int	pack_lost = ((utils->msg_sent - utils->msg_recv) / (utils->msg_recv + utils->msg_sent)) * 100;
	double			time = (((utils->t_end.tv_sec - utils->t_start.tv_sec) * 1000000) \
							+ (utils->t_end.tv_usec - utils->t_start.tv_usec)) / 1000;
	(void)pack_lost;
	(void)time;
}