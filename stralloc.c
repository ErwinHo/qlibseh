#include "byte.h"
#include "str.h"
#include "stralloc.h"
#include "alloc.h"
//#include "gen_alloc.h"		/* not really needed here */
//#include "gen_allocdefs.h"
/* Consolidate the "stralloc_*.c" functions into one source
   file. Original files shipped with qmail-1.03.
   These functions will be linked to "stralloc.a" only!
   Included files:     Size (bytes)    Date
     - stralloc_arts.c         196     19980615
     - stralloc_cat.c          161     19980615
     - stralloc_catb.c         320     19980615
     - stralloc_cats.c         150     19980615
     - stralloc_copy.c         163     19980615
     - stralloc_eady.c         203     19980615
     - stralloc_opyb.c         255     19980615
     - stralloc_opys.c         152     19980615
     - stralloc_pend.c         153     19980615          
     - stralloc_num.c          555     20010211 (djbdns-1.05)*/

/* file: stralloc_arts.c ******************************* */
int stralloc_starts(stralloc *sa,const char *s)
{
  int len;
  len = str_len((char *)s);
  return (sa->len >= len) && byte_equal(s,len,sa->s);
}
/* file: stralloc_cat.c ******************************** */
int stralloc_cat(stralloc *sato,stralloc *safrom)
{
  return stralloc_catb(sato,safrom->s,safrom->len);
}
/* file: stralloc_catb.c ******************************* */
int stralloc_catb(stralloc *sa,const char *s,unsigned int n)
{
  if (!sa->s) return stralloc_copyb(sa,s,n);
  if (!stralloc_readyplus(sa,n + 1)) return 0;
  byte_copy(sa->s + sa->len,n,s);
  sa->len += n;
  sa->s[sa->len] = 'Z'; /* ``offensive programming'' */
  return 1;
}
/* file: stralloc_cats.c ******************************* */
int stralloc_cats(stralloc *sa,const char *s)
{
  return stralloc_catb(sa,s,str_len((char *)s));
}
/* file: stralloc_copy.c ******************************* */
int stralloc_copy(stralloc *sato,stralloc *safrom)
{
  return stralloc_copyb(sato,safrom->s,safrom->len);
}

/* file: stralloc_eady.c ******************************* */
GEN_ALLOC_ready(stralloc,char,s,len,a,i,n,x,30,stralloc_ready)
GEN_ALLOC_readyplus(stralloc,char,s,len,a,i,n,x,30,stralloc_readyplus)

/* file: stralloc_opyb.c ******************************* */
int stralloc_copyb(stralloc *sa,const char *s,unsigned int n)
{
  if (!stralloc_ready(sa,n + 1)) return 0;
  byte_copy(sa->s,n,s);
  sa->len = n;
  sa->s[n] = 'Z'; /* ``offensive programming'' */
  return 1;
}
/* *** file: stralloc_opys.c *************************** */
int stralloc_copys(stralloc *sa,const char *s)
{
  return stralloc_copyb(sa,s,str_len((char *)s));
}
/* file: stralloc_pend.c ******************************* */
GEN_ALLOC_append(stralloc,char,s,len,a,i,n,x,30,stralloc_readyplus,stralloc_append)

/* file: stralloc_num.c ****** djbdns ****************** */
int stralloc_catulong0(stralloc *sa,unsigned long u,unsigned int n)
{
  unsigned int len;
  unsigned long q;
  char *s;

  len = 1;
  q = u;
  while (q > 9) { ++len; q /= 10; }
  if (len < n) len = n;

  if (!stralloc_readyplus(sa,len)) return 0;
  s = sa->s + sa->len;
  sa->len += len;
  while (len) { s[--len] = '0' + (u % 10); u /= 10; }

  return 1;
}

int stralloc_catlong0(stralloc *sa,long l,unsigned int n)
{
  if (l < 0) {
    if (!stralloc_append(sa,"-")) return 0;
    l = -l;
  }
  return stralloc_catulong0(sa,l,n);
}
