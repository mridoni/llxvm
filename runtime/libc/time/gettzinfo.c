#include <sys/types.h>

#if !defined(__LLJVM__)
#include <local.h>
#else
#include "local.h"
#endif

/* Shared timezone information for libc/time functions.  */
static __tzinfo_type tzinfo = {1, 0,
    { {'J', 0, 0, 0, 0, (time_t)0, 0L },
      {'J', 0, 0, 0, 0, (time_t)0, 0L } 
    } 
};

__tzinfo_type *
__gettzinfo (void)
{
  return &tzinfo;
}
