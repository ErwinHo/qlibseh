/*
 *  Revision 20170911, Erwin Hoffmann
 *  - consolidated 'ip6_fmt.c', 'ip6_scan.c' from ucspi-tcp-0.88
 *  - taken 'ip6_bitstring', 'ip6_scanbracket', 'fromhex' from s/qmail
 *  - note: ipX_scan reverses arguments
 *  - added ip6_scan_flat from djbdns IPv6 patch (fefe)
 *  - added ip6_ifcan and ip6_cidr
*/
#include "fmt.h"
#include "byte.h"
#include "scan.h"
#include "ip.h"
#include "str.h"

/**
 @file ip.c
 @brief IPv6 functions: ip6_fmt, ip6_scan, ip6_scanbracket, ip6_bitstring;
        ip6_fmt_flat, ip6_scan_flat, ip6_ifcan, ip6_cidr, fromhex, tohex
 */


/**
 @brief ip6_fmt
        convert IPv6 address to compactified IPv6 address string
 @param input:  IPv6 char array
        output: pointer to IPv6 address string
 @return int length of address (ok > 0)
 */

unsigned int ip6_fmt(char *s,char ip[16])
{
  unsigned int k;
  unsigned int len;
  unsigned int temp;
  unsigned int len0 = 0;
  unsigned int pos0 = 0;
  unsigned int pos1 = 0;
  unsigned int compr = 0;

  unsigned int compressed;
  int j;

  for (k = 0; k < 16; k += 2) {
    if (ip[k] == 0 && ip[k+1] == 0) {
      if (!compr) {
        compr=1;
        pos1=k;
      }
      if (k == 14) { k = 16; goto last; }
    } else if (compr) {
    last:
      if ((temp = k - pos1) > len0) {
        len0 = temp;
        pos0 = pos1;
      }
      compr = 0;
    }
  }

  for (len = 0, k = 0; k < 16; k += 2) {
    if (k == 12 && ip6_isv4mapped(ip)) {
      len += ip4_fmt(s,ip+12);
      break;
    }
    if (pos0 == k && len0) {
      if (k == 0) { ++len; if (s) *s++ = ':'; }
      ++len; 
      if (s) *s++ = ':';
      k += len0-2;
      continue;
    }
    temp = ((unsigned int) (unsigned char) ip[k] << 8) +
            (unsigned int) (unsigned char) ip[k+1];
    temp = fmt_xlong(s,temp); 
    len += temp; 
    if (s) s += temp;
    if (k < 14) { ++len; if (s) *s++ = ':'; }
  }

  return len;
}

int fromhex(unsigned char c) {
  if (c>='0' && c<='9')
    return c-'0';
  else if (c>='A' && c<='F')
    return c-'A'+10;
  else if (c>='a' && c<='f')
    return c-'a'+10;
  return -1;
}

char tohex(char num) {
  if (num<10)
    return num+'0';
  else if (num<16)
    return num-10+'a';
  else
    return -1;
}

unsigned int ip6_fmt_flat(char *s,char ip[16])
{
  int i;
  for (i=0; i<16; i++) {
    *s++=tohex((unsigned char)ip[i] >> 4);
    *s++=tohex((unsigned char)ip[i] & 15);
  }
  return 32;
}

unsigned int ip6_scan_flat(const char *s,char ip[16])
{
  int i, tmp;

  for (i = 0; i < 16; i++) {
    tmp = fromhex(*s++);
    if (tmp < 0) return 0;
    ip[i] = tmp << 4;
    tmp = fromhex(*s++);
    if (tmp < 0) return 0;
    ip[i] += tmp;
  }
  return 32;
}

/**
 @brief ip6_scan
        parse compactified IPv6 address string and convert to IPv6 address array
 @param input:  pointer to IPv6 address string
        output: IPv6 char array
 @return int length of ip6_address/ip
 */

unsigned int ip6_scan(const char *s,char ip[16])
{
  unsigned int i, j;
  unsigned int len = 0;
  unsigned long u;

  char suffix[16];
  int prefixlen = 0;
  int suffixlen = 0;
  const unsigned char *c;

 /* Always return IPv4 as IPv4-mapped IPv6 address */
  if ((i = ip4_scan((char *)s,ip+12))) {	
    for (len = 0; len < 12; ++len) 
      ip[len] = V4mappedprefix[len];
    return i;
  }
  byte_zero(ip,16);

  for (;;) {
    if (*s == ':') {
      len++;
      if (s[1] == ':') {        /* Found "::", skip to part 2 */
        s += 2; len++;
        break;
      }
      s++;
    }
    i = scan_xlong((char *)s,&u);
    if (!i) return 0;

    if (prefixlen == 12 && s[i] == '.') {
      /* the last 4 bytes may be written as IPv4 address */
      i = ip4_scan((char *)s,ip+12);
      if (i) 
        return i+len;
      else
        return 0;
    }

    ip[prefixlen++] = (u >> 8);
    ip[prefixlen++] = (u & 255);
    s += i; len += i;
    if (prefixlen == 16) return len;
  }

/* part 2, after "::" */
  for (;;) {
    if (*s == ':') {
      if (suffixlen == 0) break;
      s++;
      len++;
    } else if (suffixlen != 0) break;

    i = scan_xlong((char *)s,&u);
    if (!i) {
      len--;
      break;
    }

    if (suffixlen + prefixlen <= 12 && s[i] == '.') {
      j = ip4_scan((char *)s,suffix+suffixlen);
      if (j) {
        suffixlen += 4;
        len += j;
        break;
      } else
        prefixlen = 12 - suffixlen; /* make end-of-loop test true */
    }

    suffix[suffixlen++] = (u >> 8);
    suffix[suffixlen++] = (u & 255);
    s += i; len += i;
    if (prefixlen + suffixlen == 16) break;
  }

  for (i = 0; i < suffixlen; i++)
    ip[16 - suffixlen + i] = suffix[i];

  return len;
}

/**
 @brief ip6_scanbracket
        parse IPv6 string address enclosed in brackets
 @param input:  pointer to IPv6 address string
        output: IPv6 address char array
 @return int length of ip_address (ok > 0)
 */

unsigned int ip6_scanbracket(char ip[16],const char *s)
{
  unsigned int len;

  if (*s != '[') return 0;
  len = ip6_scan(s + 1,ip);
  if (!len) return 0;
  if (s[len + 1] != ']') return 0;
  return len + 2;
}

/**
 @brief ip6_ifscan
        parse compactified IPv6 address string 
        concatinated with the interface name: fe80::1%eth0
 @param input:  pointer to IPv6 address string
        output: IPv6 char array, stralloc interface_name
 @return int length of ip6_address/ip
 */

unsigned int ip6_ifscan(char *s,char ip[16],stralloc *ifname)
{
  int j = 0;

  if ((j = str_chr(s,'%'))) {
     if (!stralloc_copys(ifname,s+j+1)) return 0;
     s[j] = 0;
  }
  if (!stralloc_0(ifname)) return 0;

  return ip6_scan((const char *)s,ip);
}

/**
 @brief ip6_cidr
        parse compactified IPv6 address string
        concatinated with the prefix length:  fe80::1/64
 @param input:  pointer to IPv6 address string
        output: IPv6 char array, long plen 
 @return int length of ip6_address/ip
 */

unsigned int ip6_cidr(char *s,char ip[16],unsigned long plen)
{
  int j = 0;

  if ((j = str_chr(s,'/'))) {
     s[j] = 0;
     j = scan_ulong(s+j+1,&plen);
  }

  return ip6_scan((const char *)s,ip);
}

/**
 @brief ip6_bitstring
        parse IPv6 address and represent as char string with length prefix
 @param input:  IPv6 address char array, prefix length
        output: pointer to stralloc bit string;
 @return 0, if ok; 1, memory shortage
 */

unsigned int ip6_bitstring(stralloc *ip6string,char ip[16],int prefix)
{
  int i, j;
  unsigned char lowbyte, highbyte;

  ip6string->len = 0;
  if (!stralloc_copys(ip6string,"")) return 1;
  if (!stralloc_readyplus(ip6string,128)) return 1;

  for (i = 0; i < 16; i++) {
    lowbyte = (unsigned char) (ip[i]) & 0x0f;
    highbyte = (unsigned char) (ip[i] >> 4) & 0x0f;

    for (j = 3; j >= 0; j--) {
      if (highbyte & (1<<j)) {
        if (!stralloc_cats(ip6string,"1")) return 1;
      } else {
        if (!stralloc_cats(ip6string,"0")) return 1;
      }
      if (prefix == 0) {
        if (!stralloc_0(ip6string)) return 1;
        else return 0;
      }
      prefix--;
    }
    for (j = 3; j >= 0; j--) {
      if (lowbyte & (1<<j)) {
        if (!stralloc_cats(ip6string,"1")) return 1;
      } else {
        if (!stralloc_cats(ip6string,"0")) return 1;
      }
      if (prefix == 0) {
        if (!stralloc_0(ip6string)) return 1;
        else return 0;
      }
      prefix--;
    }
  }
  if (!stralloc_0(ip6string)) return 1;

  return 0;
}
