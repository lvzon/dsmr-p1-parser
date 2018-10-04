/*
   Header: net_ip.h

   	  Generic TCP/IP networking code.
   	  
   	  (c)2015-2018, Levien van Zon (levien@zonnetjes.net)
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <poll.h>

#define UNIX_PATH_MAX 108

#define MAX_POLLFDS	16

struct fdset {
	
	#ifdef USE_SELECT		// Use the select() call to monitor I/O-events
	
		fd_set master;    	// master file descriptor list
		fd_set read_fds;  	// temp file descriptor list for select()
		fd_set write_fds;  	// temp file descriptor list for select()
		fd_set except_fds;  // temp file descriptor list for select()
		int fdmax;        	// maximum file descriptor number
		struct timeval timeout;		// timeout value
		
    #else 					// Use poll() to monitor I/O-events
    
		struct pollfd pollfds[MAX_POLLFDS];
		int poll_nfds;
		struct timespec poll_timeout;

	#endif
};

/* Functions to handle file descriptor sets */

void fdset_clear(struct fdset *fdset);
void fdset_addfd(struct fdset *fdset, int fd);
void fdset_removefd(struct fdset *fdset, int fd);
int fdset_wait(struct fdset *fdset, int timeout);
int fdset_getfd_read(struct fdset *fdset);
int fdset_getfd_except(struct fdset *fdset);

/* Socket funtions */

int sendall(int s, char *buf, int *len);
void *get_in_addr(const struct sockaddr *sa);
const char *get_ip_str(const struct sockaddr *sa, char *outstring, size_t maxlen);
int socket_bind_tcp(const char *port, const char *address);
int socket_bind_udp(const char *port, const char *address);
int socket_bind_ud(const char *path, int abstract);
int socket_connect_ud(const char *path, int abstract);
int socket_connect_tcp(const char *server, const char *port);
int socket_accept(int socket_fd, struct fdset *fdset, char *ip, size_t iplen, char *host, size_t hostlen);
ssize_t socket_read(int socket_fd, char *buf, size_t buflen, int flags, struct fdset *fdset);
ssize_t socket_read_dgram(int socket_fd, char *buf, size_t buflen, int flags, struct sockaddr *src_addr);
ssize_t socket_send_dgram(int socket_fd, char *buf, size_t buflen, int flags, struct sockaddr *dest_addr);
