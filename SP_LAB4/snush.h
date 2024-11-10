/*---------------------------------------------------------------------------*/
/* snush.h */
/* Author: Jongki Park, Kyoungsoo Park                                       */
/*---------------------------------------------------------------------------*/

#ifndef _SNUSH_H_
#define _SNUSH_H_

/* SIG_UNBLOCK & sigset_t */
#ifndef __USE_POSIX
#define __USE_POSIX
#endif

#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define MAX_BG_PRO 5
#define MAX_FG_PRO 5

typedef struct {
  pid_t pid;
  pid_t pgid;
} BgProcess;

#endif /* _SNUSH_H_ */