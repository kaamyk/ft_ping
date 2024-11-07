#include "../inc/ft_ping.h"

bool	return_error( char *err_mess )
{
	fprintf(stderr, "%s\n", err_mess);
	return (1);
}
