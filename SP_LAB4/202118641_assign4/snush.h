/*---------------------------------------------------------------------------*/
/* snush.h                                                                   */
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

#define MAX_BG_PRO 16
#define MAX_FG_PRO 16

typedef struct {
  pid_t pid;
  pid_t pgid;
} BgProcess;

/*
        //
        // TODO-start: data structures in snush.h
        //

        You can add your own data structures to manage the background processes
        You can also add macros to manage background processes

        //
        // TODO-end: data structures in snush.h
        //
*/

#endif /* _SNUSH_H_ */