/*
 *  Revision 20160728, Kai Peter
 *  - new file: consolidated 'socket_tcp*.c', 'socket_udp*.c'
 *  Revision 20170824, Erwin Hoffmann
 *  - LIBC_HAS_IP6 is mandatory
*/
#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include "close.h"         /* better use unistd.h ? */
#include "ndelay.h"
#include "socket.h"
#include "error.h"

#ifndef EAFNOSUPPORT
#define EAFNOSUPPORT EINVAL
#endif

/* file: socket_tcp.c *********************************************** */
int socket_tcp4(void)
{
  int s;

  s = socket(PF_INET,SOCK_STREAM,0);
  if (s == -1)  return -1;
  if (ndelay_on(s) == -1) { close(s); return -1; }

  return s;
}

/* file: socket_tcp6.c ********************************************** */
int socket_tcp6(void)
{
  int s;

  if (ipv4socket) return socket_tcp4();
  s = socket(PF_INET6,SOCK_STREAM,0);
  if (s == -1)
    if (errno == EINVAL || errno == EAFNOSUPPORT || errno == EPFNOSUPPORT || errno == EPROTONOSUPPORT) 
      return socket_tcp4();

  if (ndelay_on(s) == -1) { close(s); return -1; }

  return s;
}

/* file: socket_udp.c *********************************************** */
int socket_udp4(void)
{
  int s;

  s = socket(PF_INET,SOCK_DGRAM,0);
  if (s == -1) return -1;
  if (ndelay_on(s) == -1) { close(s); return -1; }

  return s;
}

/* file: socket_udp6.c ********************************************** */
int socket_udp6(void)
{
  int s;

  if (ipv4socket) return socket_udp4();
  s = socket(PF_INET6,SOCK_DGRAM,0);
  if (s == -1) 
    if (errno == EINVAL || errno == EAFNOSUPPORT || errno == EPFNOSUPPORT || errno == EPROTONOSUPPORT)
      return socket_udp4();

  if (ndelay_on(s) == -1) { close(s); return -1; }

  return s;
}
