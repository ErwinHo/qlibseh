/* socket_accept:
 *
 *  - renamed to 'socket_accept.c'
*/
#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "byte.h"
#include "socket.h"
#include "ip.h"
#include "error.h"

/* Part of lib 'socket.a':
   Original file was 'socket_accept6.c', taken from ucspi-ssl-0.88. As the
   file 'socket_accept.c' (with IPv4 functinality only) was not used, this
   one is be used for IPv4 AND IPv6 now. See README for details.
*/

#define socket_accept socket_accept6   /* backwards compatibility (tmp) */
int socket_accept(int s,char ip[16],uint16 *port,uint32 *scope_id)
{
  struct sockaddr_in6 sa;
  unsigned int dummy = sizeof(sa);
  int fd;

  fd = accept(s,(struct sockaddr *)&sa,&dummy);
  if (fd == -1) return -1;

  if (sa.sin6_family == AF_INET) {
    struct sockaddr_in *sa4=(struct sockaddr_in*)&sa;
    byte_copy(ip,12,V4mappedprefix);
    byte_copy(ip+12,4,(char *)&sa4->sin_addr);
    uint16_unpack_big((char *)&sa4->sin_port,port);
    return fd;
  }
  byte_copy(ip,16,(char *)&sa.sin6_addr);
  uint16_unpack_big((char *)&sa.sin6_port,port);
  if (scope_id) *scope_id = sa.sin6_scope_id;

  return fd;
}
