#include <unistd.h>
#include "buffer.h"
#include "error.h"
#include "strerr.h"

/* Consolidate the "strerr_*.c"  functions into  one source
   file. The original files were of date 19980615,  shipped
   with qmail-1.03. These ones are aligned with djbdns-1.05.
   The "strerr_*" functions  will be linked  to  "strerr.a"
   only!
   Included files:   Size (bytes)   Date:
     - strerr_die.c          852    20010211
     - strerr_sys.c          192    20010211             


   Revision 20170824, Erwin Hoffmann
    - added 'const' casting

*/


/* file strerr_die.c *********************************** */
struct strerr strerr_sys;

void strerr_sysinit(void)
{
  strerr_sys.who = 0;
  strerr_sys.x = error_str(errno);
  strerr_sys.y = "";
  strerr_sys.z = "";
}

/* file: strerr_sys.c ********************************** */
void strerr_warn(const char *x1,const char *x2,const char *x3,
                 const char *x4,const char *x5,const char *x6,
                 const struct strerr *se)
{
  strerr_sysinit();

  if (x1) buffer_puts(buffer_2,(char *) x1);
  if (x2) buffer_puts(buffer_2,(char *) x2);
  if (x3) buffer_puts(buffer_2,(char *) x3);
  if (x4) buffer_puts(buffer_2,(char *) x4);
  if (x5) buffer_puts(buffer_2,(char *) x5);
  if (x6) buffer_puts(buffer_2,(char *) x6);

  while(se) {
    if (se->x) buffer_puts(buffer_2,(char *) se->x);
    if (se->y) buffer_puts(buffer_2,(char *) se->y);
    if (se->z) buffer_puts(buffer_2,(char *) se->z);
    se = se->who;
  }

  buffer_puts(buffer_2,"\n");
  buffer_flush(buffer_2);
}

void strerr_die(int e,const char *x1,const char *x2,
                const char *x3,const char *x4, const char *x5,
                const char *x6,const struct strerr *se)
{
  strerr_warn(x1,x2,x3,x4,x5,x6,se);
  _exit(e);
}
