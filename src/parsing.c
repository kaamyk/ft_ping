#include "../inc/ft_ping.h"

bool	parsing_flags( char *flag )
{
	switch(*flag)
	{
		case 'v':
			g_flags |= VERBOSE;
			break ;
		default:
			fprintf(stderr, "ft_ping: parsing: invalid flag\n");
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
			for (uint8_t i = 0; (**argv)[i] != '\0'; i++)
			{
				if (isdigit((**argv)[i]) == 0)
				{
					fprintf(stderr, "**argv[i] == %c\n", (**argv)[i]);
					fprintf(stderr, "ft_ping: parsing: ttl: value must be a positive numeric value.\n");
					return (1);
				}
			}
			utils->ttl = atoi(**argv);
			break ;
		default:
			fprintf(stderr, "ft_ping: parsing: invalid flag\n");
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
		fprintf(stderr, "argc == %d\n", *argc);
		fprintf(stderr, "**argv == [%s]\n", **argv);
		if (*argc < 1)
			return_error("ft_ping: usage error: Destination address required");
		else if (*argc > 1)
			return_error("ft_ping: usage error: Only one address required");
	}
	return (0);
}