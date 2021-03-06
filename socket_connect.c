/*  Revision 20170213, Kai Peter
 *   - function socket_connect4(): changed 'char' to 'unsigned char'
 *  Revision 20160728, Kai Peter
 *  - new file: consolidated 'socket_conn6.c', 'socket_conn.c'
 *  Revision 20170824, Erwin Hoffmann
 *  - LIBC_HAS_IP6 is mandatory
*/
#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include "byte.h"
#include "socket.h"
#include "ip.h"

int ipv4socket;

/* file: socket_conn6.c ************************************************ */
int socket_connect6(int s,const char ip[16],uint16 port,uint32 scope_id)
{
  struct sockaddr_in6 sa;

  if ((ipv4socket = ip6_isv4mapped(ip)))
    return socket_connect4(s,(unsigned char *)ip+12,port);
  if (byte_equal(ip,16,V6localnet) || byte_equal(ip,16,V6loopback))
    return socket_connect4(s,(unsigned char *)ip+12,port);

  byte_zero(&sa,sizeof(sa));
  sa.sin6_family = AF_INET6;
  uint16_pack_big((char *)&sa.sin6_port,port);
  sa.sin6_flowinfo = 0;
  sa.sin6_scope_id = scope_id;
  byte_copy((char *)&sa.sin6_addr,16,ip);

  return connect(s,(struct sockaddr *)&sa,sizeof(sa));
}

/* file: socket_conn.c ************************************************* */

/* this explizit declaration is needed to prevent compiler warnings */
int read(int _str, void *_buf, int _b);

int socket_connect4(int s,const unsigned char ip[4],uint16 port)
{
  struct sockaddr_in sa;

  byte_zero(&sa,sizeof(sa));
  sa.sin_family = AF_INET;
  uint16_pack_big((char *)&sa.sin_port,port);
  byte_copy((char *)&sa.sin_addr,4,ip);

  return connect(s,(struct sockaddr *)&sa,sizeof(sa));
}

/* used by IP4 and IP6 */
int socket_connected(int s)
{
  struct sockaddr_in6 sa;
  int dummy;
  char ch;

  dummy = sizeof(sa);
  if (getpeername(s,(struct sockaddr *)&sa,(socklen_t *)&dummy) == -1) {
    read(s,&ch,1); /* sets errno */
    return 0;
  }
  return 1;
}
