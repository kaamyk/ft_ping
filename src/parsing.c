#include "../inc/ft_ping.h"

bool	parsing_flags( char *flag )
{
	switch(*flag)
	{
		case 'v':
			g_flags |= VERBOSE;
			break ;
		default:
			fprintf(stderr, "ft_ping: parsing: %s: invalid flag\n", flag);
			return (1);
	}
	return (0);
}

bool	parsing_arguments( int *argc, char ***argv, data_s *utils )
{
	char	*flags[] = {"ttl", NULL};
	int8_t	r = -1;
	++(**argv);
	for (uint8_t i = 0; flags[i] != NULL; i++)
	{
		if (strcmp(**argv, flags[i]) == 0)
		{
			r = i;
			break ;
		}
	}
	switch (r)
	{
		case 0:
			g_flags |= TTL;
			++(*argv);
			--(*argc);
			if (**argv == NULL)
			{
				fprintf(stderr, "ft_ping: ttl: no value.\n");
				return (1);
			}
			for (uint8_t i = 0; (**argv)[i] != '\0'; i++)
			{
				if (isdigit((**argv)[i]) == 0)
				{
					fprintf(stderr, "ft_ping: ttl: %s: value must be a positive numeric value.\n", **argv);
					return (1);
				}
			}
			utils->ttl = atoi(**argv);
			break ;
		default:
			fprintf(stderr, "ft_ping: parsing: %s: invalid argument\n", **argv);
			return (1);
	}
	return (0);
}

bool	parsing( int *argc, char ***argv, data_s *utils )
{
	while (*argc > 0 && ***argv == '-')
	{
		++(**argv);
		if ((***argv) != '-')
		{
			if (parsing_flags(**argv) == 1)
				return (1);
		}
		else
		{
			if (parsing_arguments(argc, argv, utils) == 1)
				return (1);
		}
		--(*argc);
		++(*argv);
	}
	if (*argc != 1)
	{
		if (*argc < 1)
			return (return_error("ft_ping: usage error: missing host operand"));
		else if (*argc > 1)
			return (return_error("ft_ping: usage error: Only one host required"));
	}
	return (0);
}