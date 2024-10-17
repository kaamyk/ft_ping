#ifndef FT_PING_H
# define FT_PING_H

//	Lib specific to project
# include <netinet/ip_icmp.h>	//	struct icmphdr{}; + Macros

# include <sys/socket.h>		//	socket();
								//	getnameinfo(); 

# include <sys/types.h>			//	struct addrinfo{ see man getaddrinfo() };

# include <netdb.h>				//	struct hostent *gethostbyname(const char* name);
								//	int getaddrinfo(const char *restrict node, const char *restrict service,
								//				struct addrinfo *restrict hints, struct addrinfo **restrict res);
								//	struct addrinfo { see man getaddrinfo() };
								//	struct hostent{ see man gethostbyname() };
								//	getnameinfo();

# include <arpa/inet.h>			//	const char *inet_ntop(int af, const void *restrict src,
								//							char dst[restrict .size], socklent_t size);


//	Others
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <netinet/in.h>

#endif