/*
 *  Revision 20170319, Kai Peter
 *  - rewrite (consolidation and compatibility)
 *  Revision 20170210, Kai Peter
 *  - added definition 'V4loopback' and redefinition 'ip4loopback'
 *  Revision 20170908, Erwin Hoffmann
 *  - added some definitions from s/qmail + required for djbdns
*/

/*  Consolidated header files ip.h from *qmail (with IPv6) and ip4.h/ip6.h
    from libowfat. Thus it could be used with 'older' and 'newer' code.
*/

#ifndef IP_H
#define IP_H

#include "byte.h"
#include "stralloc.h"

#define V4MAPPREFIX "::ffff:"
#define HOSTNAMELEN 1025
#define IP4MLMTU 512
#define IP6MLMTU 1280 /* RFC 8200 */
#define MTUSIZE IP6MLMTU

#define IP4_FMT 20          /* backwards compatibility */
#define IP6_FMT 40          /*            "            */
#define IPFMT 72            /* used in qmail-remote only (temp?) */
/* may be better: */
//#define IP4_FMT IPFMT     /* backwards compatibility */
//#define IP6_FMT IPFMT     /*            "            */
/* deprecated: */
#define FMT_IP4 IP4_FMT     /* more backwards compatibility */
#define FMT_IP6 IP6_FMT     /*               "              */

/* these structs are going deprecated (should replaced by socket lib) */
struct ip_address { unsigned char d[4]; };      /* 4 decimal pieces */
#define ip_address ip4_address           /* backwards compatibility */
struct ip6_address { unsigned char d[16]; };    /* 16 hex pieces */

unsigned int ip4_fmt(char *,char *);
unsigned int ip4_scan(const char *,char *);
unsigned int ip4_scanbracket(char *,const char *);
unsigned int ip4_cidr(char *,char *,unsigned long);
unsigned int ip4_bitstring(stralloc *,char *,int);

const static char V4loopback[4] = {127,0,0,1};
const static char V4localnet[4] = {0,0,0,0};

/**
 * Compactified IPv6 addresses are really ugly to parse.
 * Syntax: (h = hex digit) [RFC 5952]
 *   1. hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh
 *   2. leading 0s in any octet can be suppressed
 *   3. any number of 0000 may be abbreviated as "::"
 *      a) but only once;
 *      b) the longest piece or on equal match the first piece,
 *      c) a single instance of 0000 has to displayed as 0
 *   4. The last two words may be written as IPv4 address
 *
 * Flat ip6 address syntax:
 *   hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh (32 hex digits)
 *
 * struct ip6_address format:
 *   cccccccccccccccc (16 chars; d[16]) -- each char swallows two hex values
 *
 * Bitstring representation with length prefix:
 *   bbbbbbb.........bbbb (max 128 bits); stralloc(ip6string); b = 0 || 1
 *
 */

unsigned int ip6_fmt(char *,char *);
unsigned int ip6_scan(const char *,char *);
unsigned int ip6_scanbracket(char *,const char *);
unsigned int ip6_bitstring(stralloc *,char *,int);
unsigned int ip6_fmt_flat(char *,char *);
unsigned int ip6_scan_flat(const char *,char *);
unsigned int ip6_ifscan(char *,char *,stralloc *);
unsigned int ip6_cidr(char *,char *,unsigned long);

const static unsigned char V4mappedprefix[12] = {0,0,0,0,0,0,0,0,0,0,0xff,0xff};
const static unsigned char V6loopback[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
const static unsigned char V6localnet[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

int fromhex(unsigned char);
char tohex(char);

#define V6any V6localnet	/* backward compatibility */
#define ip6_isv4mapped(ip) (byte_equal(ip,12,V4mappedprefix))

#endif
