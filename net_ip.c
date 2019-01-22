/*
   File: net_ip.c

   	  Generic TCP/IP networking code.
   	  
   	  (c)2015-2018, Levien van Zon (levien@zonnetjes.net)
   	  
   	  Contains code from <http://beej.us/guide/bgnet>
*/

#define _GNU_SOURCE 1

#include "net_ip.h"
#include "logmsg.h"


/* Functions to handle file descriptor sets */


void fdset_clear(struct fdset *fdset) 
{
	if (fdset == NULL)
		return;
	
	#ifdef USE_SELECT
	
		FD_ZERO(&fdset->master);    // clear the master and temp sets
		FD_ZERO(&fdset->read_fds);
		FD_ZERO(&fdset->write_fds);
		FD_ZERO(&fdset->except_fds);
		fdset->fdmax = 0;
		fdset->timeout.tv_sec = 0;
		fdset->timeout.tv_usec = 0;
		
	#else
		
		fdset->poll_nfds = 0;
		fdset->poll_timeout.tv_sec = 0;
		fdset->poll_timeout.tv_nsec = 0;
		
		int idx;
		
		for (idx = 0 ; idx < fdset->poll_nfds ; idx++) {
			fdset->pollfds[idx].fd = 0;
			fdset->pollfds[idx].events = 0;
			fdset->pollfds[idx].revents = 0;
		}
	#endif
}


void fdset_addfd(struct fdset *fdset, int fd) 
{
	if (fdset == NULL || fd <= 0)
		return;
	
	#ifdef USE_SELECT
	
		FD_SET(fd, &fdset->master);
		if (fd > fdset->fdmax) {
		   // keep track of the biggest file descriptor
		   fdset->fdmax = fd;
		}
		logmsg(LL_VERBOSE, "Adding fd %d to fdset (max = %d)\n", fd, fdset->fdmax);	
		
	#else
	
		if (fdset->poll_nfds < MAX_POLLFDS) {
			fdset->pollfds[fdset->poll_nfds].fd = fd;
			fdset->pollfds[fdset->poll_nfds].events = POLLIN | POLLPRI;
			fdset->pollfds[fdset->poll_nfds].revents = 0;
			fdset->poll_nfds++;
			logmsg(LL_VERBOSE, "Adding fd %d to pollfdset (%d slots taken, max = %d)\n", fd, fdset->poll_nfds, MAX_POLLFDS);	
		} else {	
			logmsg(LL_ERROR, "adding fd %d to pollfdset, MAX_POLLFDS reached (%d slots)!\n", fd, MAX_POLLFDS);	
		}
		
	#endif
}


struct pollfd * fdset_get_pollfdstruct(struct fdset *fdset) 
{
	struct pollfd *pollfds;
	
	if (fdset == NULL)
		return NULL;
	
	#ifdef USE_SELECT
	
		return NULL;
		
	#else
	
		if (fdset->poll_nfds < MAX_POLLFDS) {
			pollfds = &(fdset->pollfds[fdset->poll_nfds++]);
			logmsg(LL_VERBOSE, "Returning pollfd structure (%d slots taken, max = %d)\n", fdset->poll_nfds, MAX_POLLFDS);
			return pollfds;
		} else {	
			logmsg(LL_ERROR, "adding struct to pollfdset, MAX_POLLFDS reached (%d slots)!\n", MAX_POLLFDS);
			return NULL;
		}
		
	#endif
}


void fdset_removefd(struct fdset *fdset, int fd) 
{
	if (fdset == NULL || fd <= 0)
		return;
	
	#ifdef USE_SELECT
	
		FD_CLR(fd, &fdset->master);
		if (fd >= fdset->fdmax) {		
			// We need to determine which descriptor is now the highest
			int i;	
			fdset->fdmax = 0;
			for (i = 0; i <= fd; i++) {
				if (FD_ISSET(i, &fdset->master) && i > fdset->fdmax) {
					fdset->fdmax = fd;
				}
			}
		}
		logmsg(LL_VERBOSE, "Removed fd %d from fdset (max = %d)\n", fd, fdset->fdmax);
		
	#else
	
		// Find fd-entry in pollfds, and replace it by the last entry 
		
		int idx;
		
		for (idx = 0 ; idx < fdset->poll_nfds ; idx++) {
			int fd_curr = fdset->pollfds[idx].fd;
			if (fd_curr == fd) {
				// Replace deleted entry by the last one in the array
				fdset->pollfds[idx].fd = fdset->pollfds[fdset->poll_nfds].fd;
				fdset->pollfds[idx].events = fdset->pollfds[fdset->poll_nfds].events;
				fdset->pollfds[idx].revents = fdset->pollfds[fdset->poll_nfds].revents;
				fdset->poll_nfds--;
			}
		}
		
		logmsg(LL_VERBOSE, "Removed fd %d from pollfdset (%d slots taken, max = %d)\n", fd, fdset->poll_nfds, MAX_POLLFDS);	
		
	#endif
}


int fdset_wait(struct fdset *fdset, int timeout)
{
	int result;
	
	if (fdset == NULL)
		return -1;
	
	#ifdef USE_SELECT
	
		fdset->read_fds = fdset->master; // copy master set
		fdset->except_fds = fdset->master;
		if (timeout > 0) {
			fdset->timeout.tv_sec = timeout;
			fdset->timeout.tv_usec = 0;
			logmsg(LL_VERBOSE, "Waiting for data or exceptions (timeout = %d s)\n", timeout);	
			result = select(fdset->fdmax+1, &fdset->read_fds, NULL, &fdset->except_fds, &fdset->timeout);		
		} else {
			logmsg(LL_VERBOSE, "Waiting for data or exceptions\n");	
			result = select(fdset->fdmax+1, &fdset->read_fds, NULL, &fdset->except_fds, NULL);
		}
		if (result == -1) {
			logmsg(LL_WARNING, "select() returned %d in fdset_wait(), errno %d: %s\n", result, errno, strerror(errno));
		}
	
	#else
	
		//fdset->poll_timeout.tv_sec = timeout;
		//fdset->poll_timeout.tv_usec = 0;
		logmsg(LL_VERBOSE, "Waiting for data or exceptions (timeout = %d s)\n", timeout);
		result = poll(fdset->pollfds, fdset->poll_nfds, timeout);				
		if (result < 0) {
			logmsg(LL_WARNING, "poll() returned %d in fdset_wait(), errno %d: %s\n", result, errno, strerror(errno));
		}
	
	#endif
		
	return result;
}


int fdset_getfd_read(struct fdset *fdset)
{
	if (fdset == NULL)
		return -1;
	
	#ifdef USE_SELECT
	
		int i;
	
		// run through the existing file-descriptors looking for data to read
		for(i = 0; i <= fdset->fdmax; i++) {
			//logmsg(LL_VERBOSE, "Checking fd %d for data to read\n", i);	
			if (FD_ISSET(i, &fdset->read_fds)) { // we got one!!
				logmsg(LL_VERBOSE, "fd %d has data to read\n", i);	
				FD_CLR(i, &fdset->read_fds);
				return i;
			}
		}
		
	#else
	
		int idx;
		int fd = -2;
		
		for (idx = 0 ; idx < fdset->poll_nfds ; idx++) {
			//int fd = fdset->pollfds[idx].fd;
			if (fdset->pollfds[idx].revents & POLLIN) {
				fd = fdset->pollfds[idx].fd;
				logmsg(LL_VERBOSE, "fd %d has data to read\n", fd);	
				fdset->pollfds[idx].revents &= !POLLIN;		// clear read-event bit
				return fd;
			}
		}
	#endif
	
	return 0;
}


int fdset_getfd_except(struct fdset *fdset)
{
	if (fdset == NULL)
		return -1;
	
	#ifdef USE_SELECT
	
		int i;
		
		// run through the existing file-descriptors looking for exceptions
		for(i = 0; i <= fdset->fdmax; i++) {
			if (FD_ISSET(i, &fdset->except_fds)) { // we got one!!
				return i;
			}
		}
		
	#else
	
		int idx;
		
		for (idx = 0 ; idx < fdset->poll_nfds ; idx++) {
			//int fd = fdset->pollfds[idx].fd;
			if (fdset->pollfds[idx].revents & POLLPRI) {
				logmsg(LL_VERBOSE, "fd %d has urgent (out-of-band) data to read\n", idx);	
				fdset->pollfds[idx].revents &= !POLLPRI;	// clear event bit
				return idx;				
			}
		}
	#endif
	
	return 0;
}


/* Socket funtions */

/*
   Function: sendall

   Send data over a socket, in multiple chunks if needed.

   Parameters:

      s - The socket you want to send the data to.
      buf - The buffer containing the data.
      len - A pointer to an int containing the number of bytes in the buffer.

   Returns:

      The function returns -1 on error (and errno is still set from the call to send().) 
      Also, the number of bytes actually sent is returned in len. 
      This will be the same number of bytes you asked it to send, 
      unless there was an error. 
      sendall() will do it's best, huffing and puffing, to send the data out, 
      but if there's an error, it gets back to you right away.
*/

int sendall(int s, char *buf, int *len)
{
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n = -1;

    while(total < *len) {
        n = send(s, buf+total, bytesleft, MSG_NOSIGNAL);	// Setting MSG_NOSIGNAL prevents SIGPIPE when the socket is closed by the other side
        if (n == -1) { 
        	logmsg(LL_ERROR, "sendall: send() returned %d: %s\n", errno, strerror(errno));
        	break; 
        }
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    return n==-1?-1:0; // return -1 on failure, 0 on success
} 


/*
   Function: get_in_addr

   Get the IPv4 or IPv6 address field from a sockaddr-structure.
   It returns a pointer that can be passed to e.g. inet_ntop(),
   which will convert it into a human-readable address.

   Parameters:

      sa - Pointer to a sockaddr structure.

   Returns:

      A pointer to the sin_addr field in case of IPv4, 
      or a pointer to the sin6_addr field in case of IPv6,
      or NULL in case of another socket type.
*/

void *get_in_addr(const struct sockaddr *sa)
{
	switch (sa->sa_family) {
		
		case AF_INET:
			return &(((struct sockaddr_in*)sa)->sin_addr);
		case AF_INET6:
			return &(((struct sockaddr_in6*)sa)->sin6_addr);
		case AF_UNIX:
		default:
			return NULL;
	}
}


/*
   Function: get_ip_str

   Convert a struct sockaddr address to a string, IPv4 or IPv6.

   Parameters:

      sa - Pointer to a sockaddr structure.
      outstring - Pointer to the output string.
      maxlen - Maximum length of the output string (should be at least INET6_ADDRSTRLEN).

   Returns:

      A pointer to the output string, or NULL in case of an error.
*/

const char *get_ip_str(const struct sockaddr *sa, char *outstring, size_t maxlen)
{
	
	void *addr;
	
	if ((addr = get_in_addr(sa))) {
		return inet_ntop(sa->sa_family, addr, outstring, maxlen);
	} 
	
	return NULL;
}



/*
   Function: socket_bind_tcp

   Create a TCP-socket and bind it to a port.

   Parameters:

      port - Pointer to string containing the TCP port number.
      address - Pointer to string containing the local IP-address 
                or host name to bind to, or NULL for any local address.

   Returns:

      The file descriptor of the socket if successful, 
      or a negative value in case of an error.
*/

int socket_bind_tcp(const char *port, const char *address) 
{
    int listener;     // listening socket descriptor
    int rv;
    struct addrinfo hints, *ai, *p;
    int yes=1;        // for setsockopt() SO_REUSEADDR, below


    // get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if (address == NULL)
    	hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(address, port, &hints, &ai)) != 0) {
        logmsg(LL_ERROR, "getaddrinfo() returned %d in socket_bind_tcp(%s): %s\n", rv, port, gai_strerror(rv));
        return(-1);
    }
    
    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) {
        	if (errno == EAFNOSUPPORT && p->ai_family == AF_INET6) {
        		// 
        		logmsg(LL_WARNING, "Cannot create TCP-socket for IPv6: %s\n", strerror(errno));        		
        	} else {
        		logmsg(LL_ERROR, "%d (errno %d) when creating TCP socket for family %d, protocol %d: %s\n", listener, errno, p->ai_family, p->ai_protocol, strerror(errno));
        		// For ai_family values, see /usr/include/bits/socket.h (e.g. AF_INET6 = 10)
        		// For ai_protocol values, see /etc/protocols (e.g. TCP = 6)
        		// See also: http://stackoverflow.com/questions/5956516/getaddrinfo-and-ipv6
        	}
            continue;
        }
        
        // lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
        	logmsg(LL_ERROR, "bind() failed in socket_bind_tcp(%s), errno %d: %s\n", port, errno, strerror(errno));
            close(listener);
            continue;
        }

        break;
    }
    
    if (p == NULL) {	// if we got here, it means we didn't get bound
        logmsg(LL_ERROR, "Failed to bind in socket_bind_tcp(%s)\n", port);
        return(-2);
    }

    freeaddrinfo(ai); // all done with this
    
    logmsg(LL_VERBOSE, "TCP/IP socket with fd %d bound to port %s, host %s\n", listener, port, address);
    
    return listener;
}


/*
   Function: socket_bind_udp

   Create a UDP-socket and bind it to a port.

   Parameters:

      port - Pointer to string containing the UDP port number.
      address - Pointer to string containing the local IP-address 
                or host name to bind to, or NULL for any local address.

   Returns:

      The file descriptor of the socket if successful, 
      or a negative value in case of an error.
*/

int socket_bind_udp(const char *port, const char *address) 
{
    int listener;     // listening socket descriptor
    int rv;
    struct addrinfo hints, *ai, *p;
    int yes=1;        // for setsockopt() SO_REUSEADDR, below


    // get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    if (address == NULL)
    	hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(address, port, &hints, &ai)) != 0) {
        logmsg(LL_ERROR, "getaddrinfo() returned %d in socket_bind_udp(%s): %s\n", rv, port, gai_strerror(rv));
        return(-1);
    }
    
    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) { 
        	logmsg(LL_ERROR, "Failed to create UDP socket: %s\n", strerror(errno));
            continue;
        }
        
        // lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
        	logmsg(LL_ERROR, "bind() failed in socket_bind_udp(%s), errno %d: %s\n", port, errno, strerror(errno));
            close(listener);
            continue;
        }

        break;
    }
    
    if (p == NULL) {	// if we got here, it means we didn't get bound
        logmsg(LL_ERROR, "Failed to bind in socket_bind_udp(%s)\n", port);
        return(-2);
    }

    freeaddrinfo(ai); // all done with this
    
    logmsg(LL_VERBOSE, "UDP socket with fd %d bound to port %s, host %s\n", listener, port, address);
    
    return listener;
}


/*
   Function: socket_bind_ud

   Create a UNIX domain socket and bind it to a file or abstract name.

   Parameters:

      path - Pointer to a string identifying the socket. 
             This can be either a file name or an abstract name.
             
      abstract - Set to 0 if path is a file name, 
                 or to 1 if path is an abstract name.

   Returns:

      The file descriptor of the socket if successful, 
      or a negative value in case of an error.
*/

int socket_bind_ud(const char *path, int abstract)
{
	// Based on example from http://www.thomasstover.com/uds.html
	
	struct sockaddr_un address;
	int socket_fd;
	
	socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
	if (socket_fd < 0) {
		logmsg(LL_ERROR, "Failed to create UNIX domain socket: %s\n", strerror(errno));
		return -1;
	} 
	
	if (access(path, W_OK) == 0) {
		unlink(path);
	}
	
	/* start with a clean address structure */
	memset(&address, 0, sizeof(struct sockaddr_un));
	
	address.sun_family = AF_UNIX;
	snprintf(address.sun_path, UNIX_PATH_MAX, "%s", path);
	
	if (abstract)
		 address.sun_path[0] = 0;
	
	if (bind(socket_fd, (struct sockaddr *) &address, sizeof(struct sockaddr_un)) != 0)
	{
		logmsg(LL_ERROR, "bind() failed in socket_bind_ud(%s, %d), errno %d: %s\n", path, abstract, errno, strerror(errno));
		return -2;
	}	

    logmsg(LL_VERBOSE, "Unix domain socket with fd %d bound to path %s\n", socket_fd, path);
	
    return socket_fd;
}


/*
   Function: socket_connect_ud

   Connect to a UNIX domain socket.

   Parameters:

      path - Pointer to a string identifying the socket. 
             This can be either a file name or an abstract name.
             
      abstract - Set to 0 if path is a file name, 
                 or to 1 if path is an abstract name.

   Returns:

      The file descriptor of the socket if successful, 
      or a negative value in case of an error.
*/

int socket_connect_ud(const char *path, int abstract)
{
	struct sockaddr_un address;
	int socket_fd;
	
	socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
	if (socket_fd < 0) {
		logmsg(LL_ERROR, "Failed to create UNIX domain socket: %s\n", strerror(errno));
		return -1;
	} 
	
	/* start with a clean address structure */
	memset(&address, 0, sizeof(struct sockaddr_un));
	
	address.sun_family = AF_UNIX;
	snprintf(address.sun_path, UNIX_PATH_MAX, "%s", path);
	
	if (abstract)
		 address.sun_path[0] = 0;
	
	if (connect(socket_fd, (struct sockaddr *) &address, sizeof(struct sockaddr_un)) != 0)
	{
		logmsg(LL_ERROR, "connect() failed in socket_connect_ud(%s, %d), errno %d: %s\n", path, abstract, errno, strerror(errno));
		return -2;
	}	

    logmsg(LL_VERBOSE, "Connected to Unix domain socket at path %s with fd %d\n", path, socket_fd);
	
    return socket_fd;
}


/*
   Function: socket_connect_tcp

   Connect to a TCP socket.

   Parameters:

      server - Pointer to a string identifying the destination server address or hostname.              
      port - Pointer to a string identifying the destination server port.

   Returns:

      The file descriptor of the socket if successful, 
      or a negative value in case of an error.
*/

int socket_connect_tcp(const char *server, const char *port)
{
	int socket_fd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (server == NULL || port == NULL)
        return -1;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(server, port, &hints, &servinfo)) != 0) {
        logmsg(LL_ERROR, "getaddrinfo: %s\n", gai_strerror(rv));
        return -2;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
		    logmsg(LL_ERROR, "Failed to create TCP socket: %s\n", strerror(errno));
            continue;
        }

        if (connect(socket_fd, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_fd);
		    logmsg(LL_ERROR, "Failed to connect to TCP socket: %s\n", strerror(errno));
            continue;
        }

        break;
    }

    if (p == NULL) {
		logmsg(LL_ERROR, "Failed to connect to %s:%s\n", server, port);
		freeaddrinfo(servinfo);
        return -3;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);

    freeaddrinfo(servinfo); // all done with this structure
	
    logmsg(LL_VERBOSE, "Connected to TCP socket at %s:%s with fd %d\n", server, port, socket_fd);
	
    return socket_fd;
}


/*
   Function: socket_accept

   Handle incoming connections on a socket.
   Incoming connections will be added to a file descriptor set, if specified.
   Strings containing the remote IP address and/or hostname will be returned, if specified.

   Parameters:

      socket_fd - The file descriptor of the listening socket.
      fdset - An optional pointer to a fdset structure, or NULL.
      ip - An optional output string buffer for the remote IP adress, or NULL.
      iplen - The length of the IP address buffer (should be at least INET6_ADDRSTRLEN).
      host - An optional output string buffer for the remote hostname, or NULL.
      hostlen - The length of the hostname buffer.

   Returns:

      The file descriptor of the new connection if successful, 
      or a negative value in case of an error.
*/

int socket_accept(int socket_fd, struct fdset *fdset, char *ip, size_t iplen, char *host, size_t hostlen)
{
	int type, status;
    socklen_t length = sizeof( int );

    if ((status = getsockopt(socket_fd, SOL_SOCKET, SO_TYPE, &type, &length))) {
		logmsg(LL_ERROR, "Failed to get socket type for fd %d: %s\n", socket_fd, strerror(errno));
    	return -1;
	}

	struct sockaddr_storage remoteaddr; // client address
	socklen_t addrlen;

	addrlen = sizeof remoteaddr;
	int newfd = accept(socket_fd, (struct sockaddr *)&remoteaddr, &addrlen);

	if (newfd == -1) {
		logmsg(LL_ERROR, "accept() failed for socket with fd %d, errno %d: %s\n", socket_fd, errno, strerror(errno));
		return -2;
	} else {
		if (fdset)
			fdset_addfd(fdset, newfd);
	}
	
	char buf[INET6_ADDRSTRLEN];
	
	if (ip == NULL) {
		ip = buf;
		iplen = INET6_ADDRSTRLEN;
	} else if (iplen > 0) {
		ip[0] = '\0';
	}
	
	if (host && hostlen > 0) {
		host[0] = '\0';
	}
	
	if (type == SOCK_STREAM) {
		
			// It's a TCP-socket, so we can return an IP-address and/or hostname
					
			if (ip) {
				if ((ip = (char *)get_ip_str((struct sockaddr *)&remoteaddr, ip, iplen))) {
					logmsg(LL_VERBOSE, "New connection from IP %s on TCP socket with fd %d\n", ip, newfd);
				}
			}
			if (host) {
				if (getnameinfo((struct sockaddr *)&remoteaddr, addrlen, host, hostlen, NULL, 0, 0) == 0) {
					logmsg(LL_VERBOSE, "New connection from host %s on TCP socket with fd %d\n", host, newfd);
				}
			}			
	}
	
	return newfd;		
}



/*
   Function: socket_read

   Read data from a socket.
   If a connection is closed, the file descriptor will also be closed by 
   this function, and it will be removed from a file descriptor set, 
   if one is specified.

   Parameters:

      socket_fd - The file descriptor of the connection.
      buf - A pointer to the output buffer.
      buflen - The size of the output buffer, in bytes.
      flag - Optional flags to pass to recv(), or 0 for no flags. 
      fdset - An optional pointer to a fdset structure, or NULL.

   Returns:

      The number of bytes received if successful, 
      or a negative value in case of an error.
*/


ssize_t socket_read(int socket_fd, char *buf, size_t buflen, int flags, struct fdset *fdset)
{
	ssize_t nbytes;
	
	if ((nbytes = recv(socket_fd, buf, buflen, flags)) <= 0) {
		// got error or connection closed by client
		if (nbytes == 0) {
			// connection closed
			logmsg(LL_VERBOSE, "Socket disconnect, fd %d\n", socket_fd);
		} else {
			logmsg(LL_ERROR, "recv() failed for socket with fd %d, errno %d: %s\n", socket_fd, errno, strerror(errno));
		}
		close(socket_fd); // bye!
		if (fdset)
			fdset_removefd(fdset, socket_fd);
	}
	
	return nbytes;
}


/*
   Function: socket_read_dgram

   Read a message from a UDP-datagram socket.

   Parameters:

      socket_fd - 	The file descriptor of the socket.
      buf 		- 	A pointer to the output buffer.
      buflen 	- 	The size of the output buffer, in bytes.
      flag 		- 	Optional flags to pass to recvfrom(), or 0 for no flags.
      src_addr 	-	A pointer to a sockaddr-struct which will be
      				filled with the source address of the message,
      				or NULL to discard this information.      

   Returns:

      The number of bytes received if successful, 
      or a negative value in case of an error.
*/


ssize_t socket_read_dgram(int socket_fd, char *buf, size_t buflen, int flags, struct sockaddr *src_addr)
{
	ssize_t nbytes;
	socklen_t addr_len = sizeof(struct sockaddr);
	
	if ((nbytes = recvfrom(socket_fd, buf, buflen, flags, src_addr, &addr_len)) < 0) {
		logmsg(LL_ERROR, "recvfrom() failed for socket with fd %d, errno %d: %s\n", socket_fd, errno, strerror(errno));
	}
	
	return nbytes;
}


/*
   Function: socket_send_dgram

   Send a message in a UDP-datagram.

   Parameters:

      socket_fd - 	The file descriptor of the socket.
      buf 		- 	A pointer to the message buffer.
      buflen 	- 	The size of the message, in bytes.
      flag 		- 	Optional flags to pass to sendto(), or 0 for no flags.
      dest_addr -	A pointer to a sockaddr-struct that holds the
      				destination address for the message.      

   Returns:

      The number of bytes sent if successful, 
      or a negative value in case of an error.
*/


ssize_t socket_send_dgram(int socket_fd, char *buf, size_t buflen, int flags, struct sockaddr *dest_addr)
{
	ssize_t nbytes;
	
	// TODO: check if sizeof struct sockaddr is sufficient as addrlen
	
	if ((nbytes = sendto(socket_fd, buf, buflen, flags, dest_addr, sizeof(struct sockaddr))) < 0) {
		// TODO: check for MSG_DONTWAIT flag, and don't report EAGAIN or EWOULDBLOCK as errors
		logmsg(LL_ERROR, "sendto() failed for socket with fd %d, errno %d: %s\n", socket_fd, errno, strerror(errno));
	}
	
	return nbytes;
}




#ifdef TEST

/* Testing application 							*/
/* TCP: Connect with "telnet localhost 9034" 	*/

#define PORT "9034"   // port we're listening on


int main(void)
{

    int tcplistener, udplistener;     // listening socket descriptor
    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;

    char buf[256];    // buffer for client data
    int nbytes;

    char remoteIP[INET6_ADDRSTRLEN];

    int i, j;

    struct fdset fdset;

	init_msglogger();
	logger.loglevel = LL_VERBOSE;
    
    tcplistener = socket_bind_tcp(PORT, NULL);
    
    if (tcplistener <= 0)
    	    exit(1);
    	    	
    // listen
    if (listen(tcplistener, 10) == -1) {
        perror("listen (TCP)");
        exit(3);
    }

    fdset_clear(&fdset);
    fdset_addfd(&fdset, tcplistener);

    // main loop
    for(;;) {
    	
    	fdset_wait(&fdset, 0);	// Wait for incoming data and exceptions
    	
        // run through the existing connections looking for data to read
        while (i = fdset_getfd_read(&fdset)) {
                if (i == tcplistener) {
                    // handle new connections
                    socket_accept(i, &fdset, NULL, 0, NULL, 0);
                } else {
                    // handle data from a client
                    nbytes = socket_read(i, buf, sizeof buf, 0, &fdset);
                    
                    if (nbytes > 0) {
                        // we got some data from a client
                        
                        #ifdef USE_SELECT
                        
							for(j = 0; j <= fdset.fdmax; j++) {
								// send to everyone!
								if (FD_ISSET(j, &fdset.master)) {
									// except the listener and ourselves
									if (j != tcplistener && j != i) {
										if (sendall(j, buf, &nbytes) == -1) {
											perror("send (TCP)");
										}
									}
								}
							}
							
						#else
                        
							for(j = 0; j <= fdset.poll_nfds; j++) {
								int fd = fdset.pollfds[j].fd;
								// send to everyone!
								// except the listener and ourselves
								if (fd != tcplistener && fd != i) {
									if (sendall(fd, buf, &nbytes) == -1) {
										perror("send (TCP)");
									}
								}
							}
							
						#endif
                    }
                } // END handle data from client
            } // END got new incoming connection
    } // END for(;;)--and you thought it would never end!
    
    return 0;
}

#endif
