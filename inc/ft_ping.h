#ifndef FT_PING_H
# define FT_PING_H

//	Lib specific to project
# include <netinet/ip_icmp.h>	//	struct icmphdr{}; + Macros

# include <sys/socket.h>		//	socket();
								//	ssize_t sendto(int socket, const void *mess, size_t len, int flags,
								//			const struct sockaddr *dest_addr, socklen_t dest_len);
								//	ssize_t recvfrom(int socket, void *restrict buffer, size_t length,
								//				int flags, struct sockaddr *restrict address, 
								//				socklen_t *restrict len);
								//	setsockopt(int socket, int level, int option_name, const void *option_value,
								//				socklen_t option_len);
								//	getnameinfo();

# include <sys/types.h>			//	struct addrinfo{ see man getaddrinfo() };

# include <netdb.h>				//	struct hostent *gethostbyname(const char* name);
								//	int getaddrinfo(const char *restrict node, const char *restrict service,
								//				struct addrinfo *restrict hints, struct addrinfo **restrict res);
								//	struct addrinfo { see man getaddrinfo() };
								//	struct hostent{ see man gethostbyname() };
								//	getnameinfo();
								//	+ Macros

# include <sys/time.h>			//	struct timeval { time_t tv_sec; suseconds_t tv_usec };

# include <arpa/inet.h>			//	const char *inet_ntop(int af, const void *restrict src,
								//							char dst[restrict .size], socklent_t size);


//	Others
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <netinet/in.h>
# include <errno.h>
# include <signal.h>
# include <time.h>
# include <stdbool.h>

# define RECV_TIMEOUT	1
# define ICMP_HEADER_SIZE	8
# define ICMP_PAYLOAD_SIZE	56
# define ICMP_PACKET_SIZE	64
# define IP_PACKET_SIZE		84
# define TTL_DEFAULT		64

//	FLAGS
# define VERBOSE 1

typedef struct	packet_t
{
	struct icmphdr	hdr;
	char	*msg[ICMP_PACKET_SIZE - sizeof(struct icmphdr)];
}				packet_s;

typedef struct	data_t
{
	char	*parameter,
			*hostname,
			*ip_addr;
	double	times_ms_list[500];
	unsigned int	msg_sent,
					msg_recv;
	struct timeval	t_begin,
					t_finish,
					t_start,
					t_end,
					t_min,
					t_max;
}				data_s;

extern bool g_looping;
extern int	g_flags;

//		SIGNAL.C
void	sig_handler( int signum );
void	ignore_quit( void );
void	set_signal( void );

//		PRINT.C
void	print_end( data_s *utils );

//		UTILS.C
double	get_time_in_ms( struct timeval *time );
void	init_values( data_s *utils );
void	end_program( data_s *utils, int *sockfd );
#endif