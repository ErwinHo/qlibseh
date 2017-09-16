/*
 *  Revision 20170909, Erwin Hoffmann
 *  - switched to 'uint_t.h'
 *  - changed 'socket_tcp' --> 'socket_tcp4', 'socket_udp' --> 'socket_udp4'
 *  - added 'socket_ip6optionskill'
*/
#ifndef SOCKET_H
#define SOCKET_H

#include "uint_t.h"

extern int socket_tcp4(void);
extern int socket_udp4(void);
extern int socket_tcp6(void);
extern int socket_udp6(void);

//extern int socket_connect4(int,const char *,uint16);
extern int socket_connect4(int,const unsigned char *,uint16);
extern int socket_connect6(int s,const char *,uint16,uint32);
extern int socket_connected(int);
extern int socket_bind4(int,const char *,uint16);
extern int socket_bind4_reuse(int,const char *,uint16);
extern int socket_bind6(int s,const char *,uint16,uint32);
extern int socket_bind6_reuse(int s,const char *,uint16,uint32);
extern int socket_listen(int,int);
extern int socket_accept4(int,char *,uint16 *);
extern int socket_accept6(int s,char *,uint16 *,uint32 *);
extern int socket_recv4(int,char *,int,char *,uint16 *);
extern int socket_send4(int,const char *,int,const char *,uint16);
extern int socket_recv6(int,char *,unsigned int,char *,uint16 *,uint32 *);
extern int socket_send6(int,const char *,unsigned int,const char *,uint16,uint32);
extern int socket_local4(int,char *,uint16 *);
extern int socket_remote4(int,char *,uint16 *);
extern int socket_local6(int,char *,uint16 *,uint32 *);
extern int socket_remote6(int,char *,uint16 *,uint32 *);

/* enable sending udp packets to the broadcast address */
extern int socket_broadcast(int);
/* join a multicast group on the given interface */
extern int socket_mcjoin4(int,char *,char *);
extern int socket_mcjoin6(int,char *,int);
/* leave a multicast group on the given interface */
extern int socket_mcleave4(int,char *);
extern int socket_mcleave6(int,char *);
/* set multicast TTL/hop count for outgoing packets */
extern int socket_mcttl4(int,char);
extern int socket_mcttl6(int,char);
/* enable multicast loopback */
extern int socket_mcloop4(int,char);
extern int socket_mcloop6(int,char);

extern const char* socket_getifname(uint32);
extern uint32 socket_getifidx(const char *);

extern void socket_tryreservein(int,int);

extern int socket_ipoptionskill(int);
extern int socket_ip6optionskill(int);
extern int socket_tcpnodelay(int);

extern int ipv4socket; /* required; former noipv6 */

#endif
