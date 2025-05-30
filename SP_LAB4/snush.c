/*---------------------------------------------------------------------------*/
/* snush.c */
/* Author: Jongki Park, Kyoungsoo Park                                       */
/*---------------------------------------------------------------------------*/

#include "snush.h"

#include "dynarray.h"
#include "execute.h"
#include "lexsyn.h"
#include "token.h"
#include "util.h"

volatile int bg_array_idx;
BgProcess *bg_array;
int bg_cnt;
int pipe_fd[2];

/*---------------------------------------------------------------------------*/
void cleanup() {
  free(bg_array);
  close(pipe_fd[0]);
  close(pipe_fd[1]);
}
/*---------------------------------------------------------------------------*/
void check_bg_status() {
  char buffer[64];
  ssize_t bytes_read;
  while ((bytes_read = read(pipe_fd[0], buffer, sizeof(buffer))) > 0) {
    if (write(STDOUT_FILENO, buffer, bytes_read) < 0) {
      const char *error_msg = "Error writing to stdout\n";
      if (write(STDERR_FILENO, error_msg, strlen(error_msg)) < 0) {
      }
    }
  }

  if (bytes_read == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
    const char *error_msg = "Error reading from pipe\n";
    if (write(STDERR_FILENO, error_msg, strlen(error_msg)) < 0) {
    }
  }
}

/*---------------------------------------------------------------------------*/
static void int_to_str(int num, char *str, int *len) {
  char temp[16];
  int index = 0;

  do {
    temp[index++] = '0' + (num % 10);
    num /= 10;
  } while (num > 0);

  for (int i = index - 1; i >= 0; i--) {
    str[(*len)++] = temp[i];
  }
}
/*---------------------------------------------------------------------------*/

/* Whenever a child process terminates, this handler handles all zombies. */
static void sigzombie_handler(int signo) {
  pid_t pid;
  int stat;

  if (signo == SIGCHLD) {
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
      int found_idx = -1;
      pid_t pgid = -1;

      for (int i = 0; i < bg_array_idx; i++) {
        if (bg_array[i].pid == pid) {
          pgid = bg_array[i].pgid;
          found_idx = i;
          break;
        }
      }

      // if not bg process, return
      if (found_idx == -1) continue;

      for (int i = found_idx; i < bg_array_idx - 1; i++) {
        bg_array[i] = bg_array[i + 1];
      }
      bg_array_idx--;

      int pgid_exists = 0;
      for (int i = 0; i < bg_array_idx; i++) {
        if (bg_array[i].pgid == pgid) {
          pgid_exists = 1;
          break;
        }
      }

      if (!pgid_exists) {
        int len = 0;
        char buffer[64];

        buffer[len++] = '[';
        int_to_str(pgid, buffer, &len);
        buffer[len++] = ']';
        buffer[len++] = ' ';
        const char *msg = "Done background process group\n";
        for (int i = 0; msg[i] != '\0'; i++) {
          buffer[len++] = msg[i];
        }

        if (write(pipe_fd[1], buffer, len) < 0) {
          const char *error_msg = "Error writing to pipe\n";
          if (write(STDERR_FILENO, error_msg, strlen(error_msg)) < 0) {
          }
        }
      }
    }

    if (pid < 0 && errno != ECHILD && errno != EINTR) {
      perror("waitpid");
    }
  }

  return;
}
/*---------------------------------------------------------------------------*/
static void shell_helper(const char *in_line) {
  DynArray_T oTokens;

  enum LexResult lexcheck;
  enum SyntaxResult syncheck;
  enum BuiltinType btype;
  int pcount;
  int ret_pgid;  // background pid
  int is_background;

  oTokens = dynarray_new(0);
  if (oTokens == NULL) {
    error_print("Cannot allocate memory", FPRINTF);
    exit(EXIT_FAILURE);
  }

  lexcheck = lex_line(in_line, oTokens);
  switch (lexcheck) {
    case LEX_SUCCESS:
      if (dynarray_get_length(oTokens) == 0) return;

      /* dump lex result when DEBUG is set */
      dump_lex(oTokens);

      syncheck = syntax_check(oTokens);
      if (syncheck == SYN_SUCCESS) {
        btype = check_builtin(dynarray_get(oTokens, 0));
        if (btype == NORMAL) {
          is_background = check_bg(oTokens);

          pcount = count_pipe(oTokens);

          if (is_background) {
            if (bg_array_idx + pcount + 1 > MAX_BG_PRO) {
              printf(
                  "Error: Total background processes "
                  "exceed the limit (%d).\n",
                  MAX_BG_PRO);
              return;
            }
          }

          if (pcount > 0) {
            ret_pgid = iter_pipe_fork_exec(pcount, oTokens, is_background);
          } else {
            ret_pgid = fork_exec(oTokens, is_background);
          }

          if (ret_pgid > 0) {
            if (is_background == 1)
              printf("[%d] Background process group running\n", ret_pgid);
          } else {
            printf(
                "Invalid return value "
                "of external command execution\n");
          }
        } else {
          /* Execute builtin command */
          execute_builtin(oTokens, btype);
        }
      }

      /* syntax error cases */
      else if (syncheck == SYN_FAIL_NOCMD)
        error_print("Missing command name", FPRINTF);
      else if (syncheck == SYN_FAIL_MULTREDOUT)
        error_print("Multiple redirection of standard out", FPRINTF);
      else if (syncheck == SYN_FAIL_NODESTOUT)
        error_print("Standard output redirection without file name", FPRINTF);
      else if (syncheck == SYN_FAIL_MULTREDIN)
        error_print("Multiple redirection of standard input", FPRINTF);
      else if (syncheck == SYN_FAIL_NODESTIN)
        error_print("Standard input redirection without file name", FPRINTF);
      else if (syncheck == SYN_FAIL_INVALIDBG)
        error_print("Invalid use of background", FPRINTF);
      break;

    case LEX_QERROR:
      error_print("Unmatched quote", FPRINTF);
      break;

    case LEX_NOMEM:
      error_print("Cannot allocate memory", FPRINTF);
      break;

    case LEX_LONG:
      error_print("Command is too large", FPRINTF);
      break;

    default:
      error_print("lex_line needs to be fixed", FPRINTF);
      exit(EXIT_FAILURE);
  }

  /* Free memories allocated to tokens */
  dynarray_map(oTokens, free_token, NULL);
  dynarray_free(oTokens);
}
/*---------------------------------------------------------------------------*/
int main(int argc, char *argv[]) {
  sigset_t sigset;
  char c_line[MAX_LINE_SIZE + 2];

  atexit(cleanup);

  /* Initialize variables for background processes */
  if (pipe(pipe_fd) == -1) {
    perror("pipe failed");
    exit(EXIT_FAILURE);
  }

  int flags = fcntl(pipe_fd[0], F_GETFL);
  if (flags == -1) {
    perror("fcntl failed");
    exit(EXIT_FAILURE);
  }
  if (fcntl(pipe_fd[0], F_SETFL, flags | O_NONBLOCK) == -1) {
    perror("fcntl failed");
    exit(EXIT_FAILURE);
  }

  bg_array_idx = 0;
  bg_array = calloc(MAX_BG_PRO, sizeof(BgProcess));
  if (bg_array == NULL) {
    perror("calloc failed");
    exit(EXIT_FAILURE);
  }

  sigemptyset(&sigset);
  sigaddset(&sigset, SIGINT);
  sigaddset(&sigset, SIGCHLD);
  sigaddset(&sigset, SIGQUIT);
  sigprocmask(SIG_UNBLOCK, &sigset, NULL);

  /* Register signal handler */
  signal(SIGINT, SIG_IGN);
  signal(SIGCHLD, sigzombie_handler);
  signal(SIGQUIT, SIG_IGN);
  signal(SIGTSTP, SIG_IGN);

  error_print(argv[0], SETUP);

  while (1) {
    check_bg_status();
    fprintf(stdout, "%% ");
    fflush(stdout);

    if (fgets(c_line, MAX_LINE_SIZE, stdin) == NULL) {
      printf("\n");
      exit(EXIT_SUCCESS);
    }
    shell_helper(c_line);
  }

  return 0;
}
/*---------------------------------------------------------------------------*/