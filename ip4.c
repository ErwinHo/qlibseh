/*
 *  Revision 20170816, Erwin Hoffmann
 *  - new file: consolidated 'ip4_fmt.c', 'ip4_scan.c' from ucspi-tcp-0.88
 *  - added function 'ip_scanbracket' from original (qmail) 'ip.c'
 *  - added function 'ip4_bitstring' from s/qmail
 *  - added funcition 'ip4_cidr'
*/
#include "fmt.h"
#include "scan.h"
#include "str.h"
#include "ip.h"

/**
 @brief ip4_fmt
        converts IPv4 address to dotted decimal string format
 @param input:  pointer to IPv4 ip_address struct
        output: pointer to IPv4 address string
 @return int length of address (ok > 0)
 */

unsigned int ip4_fmt(char *s,char ip[4])
{
  unsigned int len;
  unsigned int i;

  len = 0;
  i = fmt_ulong(s,(unsigned long) (unsigned char) ip[0]); len += i; if (s) s += i;
  if (s) *s++ = '.'; ++len;
  i = fmt_ulong(s,(unsigned long) (unsigned char) ip[1]); len += i; if (s) s += i;
  if (s) *s++ = '.'; ++len;
  i = fmt_ulong(s,(unsigned long) (unsigned char) ip[2]); len += i; if (s) s += i;
  if (s) *s++ = '.'; ++len;
  i = fmt_ulong(s,(unsigned long) (unsigned char) ip[3]); len += i; if (s) s += i;
  return len;
}

/**
 @brief ip4_scan
        parse IPv4 address string and convert to IP address array
 @param input:  pointer to IPv4 address string (1st arg)
        output: pointer to IPv4 ip_address struct (2nd arg)
 @return int lenth of ip_address (ok > 0)
 */

unsigned int ip4_scan(const char *s,char ip[4])
{
  unsigned int i;
  unsigned int len;
  unsigned long u;

  byte_zero(ip,4);
  len = 0;
  i = scan_ulong((char *)s,&u); if (!i) return 0; ip[0] = u; s += i; len += i;
  if (*s != '.') return 0; ++s; ++len;
  i = scan_ulong((char *)s,&u); if (!i) return 0; ip[1] = u; s += i; len += i;
  if (*s != '.') return 0; ++s; ++len;
  i = scan_ulong((char *)s,&u); if (!i) return 0; ip[2] = u; s += i; len += i;
  if (*s != '.') return 0; ++s; ++len;
  i = scan_ulong((char* )s,&u); if (!i) return 0; ip[3] = u; s += i; len += i;
  return len;
}

/**
 @brief ip4_scanbracket
        parse IPv4 address string enclosed in brackets and convert to IP address array
 @param input:  pointer to IPv4 address string
        output: pointer to IPv4 ip_address struct
 @return int lenth of ip_address (ok > 0)
 */

unsigned int ip4_scanbracket(char ip[4],const char *s)
{
  unsigned int len;

  if (*s != '[') return 0;
  len = ip4_scan(s + 1,ip);
  if (!len) return 0;
  if (s[len + 1] != ']') return 0;
  return len + 2;
}

/* scan IPv4 or IPv6 ip address enclosed in brackets */
// temp., not "enough" tested yet !!!
//unsigned int ip46_scanbracket(char *s,char *ip_str)

unsigned int ip_scanbracket(char *ip_str,char *s)
{
  unsigned int len;

  if (*s != '[') return 0;
  if (str_chr(ip_str,':')) {
     len = ip6_scan(s + 1,ip_str);
  } else {
     len = ip4_scan(s + 1,ip_str);
  }

  if (!len) return 0;
  if (s[len + 1] != ']') return 0;
  return len + 2;
}

/**
 @brief ip4_cidr
        parse IPv4 address string
        concatinated with the prefix length:  192.168.1/24
 @param input:  pointer to IPv6 address string
        output: IPv6 char array, long plen
 @return int length of ip6_address/ip
 */

unsigned int ip4_cidr(char *s,char ip[4],unsigned long plen)
{
  int j = 0;

  if ((j = str_chr(s,'/'))) {
     s[j] = 0;
     j = scan_ulong(s+j+1,&plen);
  }

  return ip4_scan((const char *)s,ip);
}

/**
 @brief ip4_bitstring
        parse IPv4 address and represent as char string with length prefix
 @param input:  IPv4 address char array, prefix length
        output: pointer to stralloc bitstring
 @return 0, if ok; 1, memory shortage; 2, input error
 */

unsigned int ip4_bitstring(stralloc *ip4string,char ip[4],int prefix)
{
  int i, j;
  unsigned char number;

  ip4string->len = 0;
  if (!stralloc_copys(ip4string,"")) return 1;
  if (!stralloc_readyplus(ip4string,32)) return 1;

  for (i = 0; i < 4; i++) {
    number = (unsigned char) ip[i];
    if (number > 255) return 2;

    for (j = 7; j >= 0; j--) {
      if (number & (1<<j)) {
        if (!stralloc_cats(ip4string,"1")) return 1;
      } else {
        if (!stralloc_cats(ip4string,"0")) return 1;
      }
      if (prefix == 0) {
        if (!stralloc_0(ip4string)) return 1;
        else return 0;
      }
      prefix--;
    }
  }
  if (!stralloc_0(ip4string)) return 1;

  return 0;
}
