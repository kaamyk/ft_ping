#ifndef FT_PING_H
# define FT_PING_H

//	Lib specific to project
# include <netinet/ip_icmp.h>	//	struct icmphdr{}; + Macros

# include <sys/socket.h>		//	socket();
								//	sendto(int socket, const void *mess, size_t len, int flags,
								//			const struct sockaddr *dest_addr, socklen_t dest_len);
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

typedef struct	packet_t
{
	struct icmphdr	hdr;
	char	*msg;
}				packet_s;

void	sig_handler( int signum );
void	ignore_quit( void );
void	set_signal( void );

#endif