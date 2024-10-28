#include "../inc/ft_ping.h"

void	sig_handler( int signum )
{
	if (signum == SIGINT)
	{
		g_looping = 0;
	}
}

void	ignore_quit( void )
{
	struct sigaction	ign;
	memset(&ign, 0, sizeof(ign));
	ign.sa_handler = SIG_IGN;
	sigaction (SIGQUIT, &ign, NULL);
}

void	set_signal( void )
{
	struct sigaction	sig;

	ignore_quit();
	memset(&sig, 0, sizeof(sig));
	sigemptyset(&sig.sa_mask);
	sig.sa_flags = 0;
	sig.sa_handler = &sig_handler;
	sigaction(SIGINT, &sig, NULL);
}