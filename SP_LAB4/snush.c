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

/*---------------------------------------------------------------------------*/
void cleanup() { free(bg_array); }
/*---------------------------------------------------------------------------*/
/* Whenever a child process terminates, this handler handles all zombies. */
static void sigzombie_handler(int signo) {
  pid_t pid;
  int stat;

  // sigset_t oldset, blockset;
  // sigemptyset(&blockset);
  // sigaddset(&blockset, SIGCHLD);
  // sigprocmask(SIG_BLOCK, &blockset, &oldset);  // SIGCHLD 블로킹
  printf("HI\n");

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
      if (found_idx == -1) return;
      printf("terminated pid: %d\n", pid);
      printf("found_idx: %d\n", found_idx);
      printf("pgid: %d\n", pgid);

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
        printf("[%d] Done background process group\n", pgid);
      }
    }

    if (pid < 0 && errno != ECHILD && errno != EINTR) {
      printf("TLqkf\n");
      perror("waitpid");
    }
  }
  printf("BYE\n");
  // sigprocmask(SIG_SETMASK, &oldset, NULL);  // SIGCHLD 언블로킹
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

          printf("is_background: %d\n", is_background);
          printf("bg_array_idx: %d\n", bg_array_idx);

          pcount = count_pipe(oTokens);

          if (is_background && bg_array_idx + pcount + 1 > MAX_BG_PRO) {
            error_print(
                "Program exceeds the maximum number of the "
                "background processes\n",
                FPRINTF);
            exit(EXIT_FAILURE);
          }

          if (pcount > 0) {
            ret_pgid = iter_pipe_fork_exec(pcount, oTokens, is_background);
          } else {
            ret_pgid = fork_exec(oTokens, is_background);
          }

          if (ret_pgid > 0) {
            if (is_background == 1)
              printf("[%d] Background process running\n", ret_pgid);
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
  struct sigaction sa;
  sa.sa_handler = sigzombie_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
  sigaction(SIGCHLD, &sa, NULL);
  /* Initialize Background process array and stack */
  bg_array = calloc(MAX_BG_PRO, sizeof(BgProcess));

  atexit(cleanup);

  /* Initialize variables for background/foreground processes */
  bg_array_idx = 0;
  bg_cnt = 0;

  sigemptyset(&sigset);
  sigaddset(&sigset, SIGINT);
  // sigaddset(&sigset, SIGCHLD);
  sigaddset(&sigset, SIGQUIT);
  sigprocmask(SIG_UNBLOCK, &sigset, NULL);

  /* Register signal handler */
  signal(SIGINT, SIG_IGN);
  signal(SIGCHLD, sigzombie_handler);
  signal(SIGQUIT, SIG_IGN);

  error_print(argv[0], SETUP);

  while (1) {
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