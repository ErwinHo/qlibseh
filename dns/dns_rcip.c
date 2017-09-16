#include "taia.h"
#include "readclose.h"
#include "byte.h"
#include "ip.h"
#include "env.h"
#include "dns.h"

static stralloc data = {0};
stralloc netif = {0};

static int init(char ip[256])
{
  int i;
  int j;
  int iplen = 0;
  char *x;
  char ip4[4];

/* Read (compactified) IPv4|v6 addresses of resolvers
   Store them in array IP with fixed length : 
   ip(64) -> 16 IPv4 addresses
   ip(256) -> 16 IPv6 addresses (we use IPv4 mapped IPv6 addresses) 
*/

  x = env_get("DNSCACHEIP");
  if (x)
    while (iplen <= 240 && x != '\0') {
      if (*x == ' ') 
        ++x; 
      else 
        if ((i = ip6_ifscan(x,ip + iplen,&netif))) {
	  iplen += 16;
	  if (*(x += i) == '\0') break;
        } 
    }

  if (!iplen) {
    i = openreadclose("/etc/resolv.conf",&data,64);
    if (i == -1) return -1;
    if (i) {
      if (!stralloc_append(&data,"\n")) return -1;
      i = 0;
      for (j = 0; j < data.len; ++j)
        if (data.s[j] == '\n') {
          if (byte_equal("nameserver ",11,data.s + i) || byte_equal("nameserver\t",11,data.s + i)) {
            i += 10; 
            while ((data.s[i] == ' ') || (data.s[i] == '\t'))
              i++; 
            if (iplen <= 240) {
	      if (ip4_scan(data.s + i,ip4)) {
                if (byte_equal(ip4,4,"\0\0\0\0"))
                  byte_copy(ip4,4,"\177\0\0\1");
                byte_copy(ip+iplen,12,V4mappedprefix);
                byte_copy(ip+iplen+12,4,ip4);
                iplen += 16;
              } else if (ip6_scan(data.s + i,ip + iplen)) {
                iplen += 16;
              }
            }
          }
          i = j + 1;
        }
    }
  }

  if (!iplen) {
    byte_copy(ip,16,"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\1");
    iplen = 16;
  }
  byte_zero(ip + iplen,256 - iplen);
  return 0;
}

static int ok = 0;
static unsigned int uses;
static struct taia deadline;
static char ip[256]; /* defined if ok */

int dns_resolvconfip(char s[256])
{
  struct taia now;

  taia_now(&now);
  if (taia_less(&deadline,&now)) ok = 0;
  if (!uses) ok = 0;

  if (!ok) {
    if (init(ip) == -1) return -1;
    taia_uint(&deadline,600);
    taia_add(&deadline,&now,&deadline);
    uses = 10000;
    ok = 1;
  }

  --uses;
  byte_copy(s,256,ip);
  return 0;
}
