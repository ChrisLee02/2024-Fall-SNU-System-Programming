/*---------------------------------------------------------------------------*/
/* execute.c                                                                 */
/* Author: Jongki Park, Kyoungsoo Park                                       */
/*---------------------------------------------------------------------------*/

#include "execute.h"

#include "dynarray.h"
#include "lexsyn.h"
#include "snush.h"
#include "token.h"
#include "util.h"

extern volatile int bg_array_idx;
extern BgProcess *bg_array;
extern int bg_cnt;

/*---------------------------------------------------------------------------*/
void redout_handler(char *fname) {
  int fd;

  fd = open(fname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd < 0) {
    error_print(NULL, PERROR);
    exit(EXIT_FAILURE);
  }

  if (dup2(fd, STDOUT_FILENO) < 0) {
    error_print(NULL, PERROR);
    close(fd);
    exit(EXIT_FAILURE);
  }

  if (close(fd) < 0) {
    error_print(NULL, PERROR);
    exit(EXIT_FAILURE);
  }
}
/*---------------------------------------------------------------------------*/
void redin_handler(char *fname) {
  int fd;

  fd = open(fname, O_RDONLY);
  if (fd < 0) {
    error_print(NULL, PERROR);
    exit(EXIT_FAILURE);
  }

  if (dup2(fd, STDIN_FILENO) < 0) {
    error_print(NULL, PERROR);
    close(fd);
    exit(EXIT_FAILURE);
  }

  if (close(fd) < 0) {
    error_print(NULL, PERROR);
    exit(EXIT_FAILURE);
  }
}
/*---------------------------------------------------------------------------*/
int build_command_partial(DynArray_T oTokens, int start, int end,
                          char *args[]) {
  int i, ret = 0, redin = FALSE, redout = FALSE, cnt = 0;
  struct Token *t;

  /* Build command */
  for (i = start; i < end; i++) {
    t = dynarray_get(oTokens, i);

    if (t->token_type == TOKEN_WORD) {
      if (redin == TRUE) {
        redin_handler(t->token_value);
        redin = FALSE;
      } else if (redout == TRUE) {
        redout_handler(t->token_value);
        redout = FALSE;
      } else
        args[cnt++] = t->token_value;
    } else if (t->token_type == TOKEN_REDIN)
      redin = TRUE;
    else if (t->token_type == TOKEN_REDOUT)
      redout = TRUE;
  }
  args[cnt] = NULL;

#ifdef DEBUG
  for (i = 0; i < cnt; i++) {
    if (args[i] == NULL)
      printf("CMD: NULL\n");
    else
      printf("CMD: %s\n", args[i]);
  }
  printf("END\n");
#endif
  return ret;
}
/*---------------------------------------------------------------------------*/
int build_command(DynArray_T oTokens, char *args[]) {
  return build_command_partial(oTokens, 0, dynarray_get_length(oTokens), args);
}
/*---------------------------------------------------------------------------*/
void execute_builtin(DynArray_T oTokens, enum BuiltinType btype) {
  int ret;
  char *dir = NULL;
  struct Token *t1;

  switch (btype) {
    case B_EXIT:
      if (dynarray_get_length(oTokens) == 1) {
        dynarray_map(oTokens, free_token, NULL);
        dynarray_free(oTokens);

        exit(EXIT_SUCCESS);
      } else
        error_print("exit does not take any parameters", FPRINTF);

      break;

    case B_CD:
      if (dynarray_get_length(oTokens) == 1) {
        dir = getenv("HOME");
        if (dir == NULL) {
          error_print("cd: HOME variable not set", FPRINTF);
          break;
        }
      } else if (dynarray_get_length(oTokens) == 2) {
        t1 = dynarray_get(oTokens, 1);
        if (t1->token_type == TOKEN_WORD) dir = t1->token_value;
      }

      if (dir == NULL) {
        error_print("cd takes one parameter", FPRINTF);
        break;
      } else {
        ret = chdir(dir);
        if (ret < 0) error_print(NULL, PERROR);
      }
      break;

    default:
      error_print("Bug found in execute_builtin", FPRINTF);
      exit(EXIT_FAILURE);
  }
}
/*---------------------------------------------------------------------------*/
/* Important Notice!!
        Add "signal(SIGINT, SIG_DFL);" after fork
*/
int fork_exec(DynArray_T oTokens, int is_background) {
  int status;
  char *args[1024];
  sigset_t oldset, blockset;
  sigemptyset(&blockset);
  sigaddset(&blockset, SIGCHLD);
  pid_t pid = fork();

  if (pid < 0) {
    error_print("Fork failed", PERROR);
    return -1;
  }

  if (pid == 0) {
    signal(SIGINT, SIG_DFL);
    build_command(oTokens, args);

    if (execvp(args[0], args) < 0) {
      error_print(args[0], PERROR);
      exit(EXIT_FAILURE);
    }
  } else {
    if (is_background) {
      sigprocmask(SIG_BLOCK, &blockset, &oldset);
      bg_array[bg_array_idx].pid = pid;
      bg_array[bg_array_idx].pgid = pid;
      bg_array_idx++;
      sigprocmask(SIG_SETMASK, &oldset, NULL);
    } else {
      if (waitpid(pid, &status, 0) < 0) {
        error_print("Waitpid failed", PERROR);
        return -1;
      }
    }
  }

  return pid;
}

void close_all_pipes(int pipe_fds[][2], int pcount) {
  for (int j = 0; j < pcount; j++) {
    if (close(pipe_fds[j][0]) < 0) perror("close failed");
    if (close(pipe_fds[j][1]) < 0) perror("close failed");
  }
}

/*---------------------------------------------------------------------------*/
/* Important Notice!!
        Add "signal(SIGINT, SIG_DFL);" after fork
*/

int iter_pipe_fork_exec(int pcount, DynArray_T oTokens, int is_background) {
  int pipe_fds[1024][2];
  int pipe_index[1024];
  char *args[1024];
  pid_t pids[1024];
  int status;
  sigset_t oldset, blockset;
  sigemptyset(&blockset);
  sigaddset(&blockset, SIGCHLD);
  pid_t pgid, pid;

  for (int i = 0; i < pcount; i++) {
    if (pipe(pipe_fds[i]) < 0) {
      close_all_pipes(pipe_fds, i);
      error_print("Pipe failed", PERROR);
      return -1;
    }
  }

  for (int i = 0, j = 0; i < dynarray_get_length(oTokens); i++) {
    struct Token *t;
    t = dynarray_get(oTokens, i);
    if (t->token_type == TOKEN_PIPE) {
      pipe_index[j++] = i;
    }
  }

  // first child
  pgid = fork();

  if (pgid < 0) {
    error_print("Fork failed", PERROR);
    return -1;
  }

  if (pgid == 0) {
    signal(SIGINT, SIG_DFL);
    build_command_partial(oTokens, 0, pipe_index[0], args);
    if (dup2(pipe_fds[0][1], STDOUT_FILENO) < 0) {
      perror("dup2 failed");
      close_all_pipes(pipe_fds, pcount);  // ====== 추가된 리소스 정리 ======
      exit(EXIT_FAILURE);
    }

    close_all_pipes(pipe_fds, pcount);
    if (execvp(args[0], args) < 0) {
      error_print(args[0], PERROR);
      close_all_pipes(pipe_fds, pcount);
      exit(EXIT_FAILURE);
    }
  } else {
    if (is_background) {
      sigprocmask(SIG_BLOCK, &blockset, &oldset);
      bg_array[bg_array_idx].pid = pgid;
      bg_array[bg_array_idx].pgid = pgid;
      bg_array_idx++;
      sigprocmask(SIG_SETMASK, &oldset, NULL);
    }
    pids[0] = pgid;
  }

  // between
  for (int i = 1; i <= pcount - 1; i++) {
    pid = fork();
    if (pid < 0) {
      error_print("Fork failed", PERROR);
      return -1;
    }
    if (pid == 0) {
      signal(SIGINT, SIG_DFL);
      build_command_partial(oTokens, pipe_index[i - 1] + 1, pipe_index[i],
                            args);

      if (dup2(pipe_fds[i - 1][0], STDIN_FILENO) < 0) {
        perror("dup2 failed");
        close_all_pipes(pipe_fds, pcount);  // 추가된 부분
        exit(EXIT_FAILURE);
      }
      if (dup2(pipe_fds[i][1], STDOUT_FILENO) < 0) {
        perror("dup2 failed");
        close_all_pipes(pipe_fds, pcount);  // 추가된 부분
        exit(EXIT_FAILURE);
      }

      close_all_pipes(pipe_fds, pcount);

      if (execvp(args[0], args) < 0) {
        error_print(args[0], PERROR);
        exit(EXIT_FAILURE);
      }
    } else {
      if (sigprocmask(SIG_BLOCK, &blockset, &oldset) < 0) {
        close_all_pipes(pipe_fds, pcount);
        error_print("Failed to block signals", PERROR);
        return -1;
      }

      if (is_background) {
        bg_array[bg_array_idx].pid = pid;
        bg_array[bg_array_idx].pgid = pgid;
        bg_array_idx++;
      }

      if (sigprocmask(SIG_SETMASK, &oldset, NULL) < 0) {
        close_all_pipes(pipe_fds, pcount);
        error_print("Failed to restore signal mask", PERROR);
        return -1;
      }

      pids[i] = pgid;
    }
  }

  // last child
  pid = fork();

  if (pid < 0) {
    error_print("Fork failed", PERROR);
    return -1;
  }

  if (pid == 0) {
    signal(SIGINT, SIG_DFL);
    build_command_partial(oTokens, pipe_index[pcount - 1],
                          dynarray_get_length(oTokens), args);

    if (dup2(pipe_fds[pcount - 1][0], STDIN_FILENO) < 0) {
      perror("dup2 failed");
      close_all_pipes(pipe_fds, pcount);  // 추가된 부분
      exit(EXIT_FAILURE);
    }

    close_all_pipes(pipe_fds, pcount);

    if (execvp(args[0], args) < 0) {
      error_print(args[0], PERROR);
      exit(EXIT_FAILURE);
    }
  } else {
    if (sigprocmask(SIG_BLOCK, &blockset, &oldset) < 0) {
      close_all_pipes(pipe_fds, pcount);
      error_print("Failed to block signals", PERROR);
      return -1;
    }

    if (is_background) {
      bg_array[bg_array_idx].pid = pid;
      bg_array[bg_array_idx].pgid = pgid;
      bg_array_idx++;
    }

    if (sigprocmask(SIG_SETMASK, &oldset, NULL) < 0) {
      close_all_pipes(pipe_fds, pcount);
      error_print("Failed to restore signal mask", PERROR);
      return -1;
    }
    pids[pcount] = pgid;
  }

  close_all_pipes(pipe_fds, pcount);

  if (is_background) return pgid;

  for (int i = 0; i <= pcount; i++) {
    if ((pid = waitpid(pids[i], &status, 0)) < 0) {
      if (pid == -1 && errno != ECHILD && errno != EINTR) {
        error_print("Waitpid failed", PERROR);
        return -1;
      }
    }
  }

  return pgid;
}
/*---------------------------------------------------------------------------*/