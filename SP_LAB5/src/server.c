/*---------------------------------------------------------------------------*/
/* server.c                                                                  */
/* Author: Junghan Yoon, KyoungSoo Park                                      */
/* Modified by: (Your Name)                                                  */
/*---------------------------------------------------------------------------*/
#define _GNU_SOURCE
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "common.h"
#include "skvslib.h"
/*---------------------------------------------------------------------------*/
struct thread_args {
  int listenfd;
  int idx;
  struct skvs_ctx *ctx;

  /*---------------------------------------------------------------------------*/
  /* free to use */

  /*---------------------------------------------------------------------------*/
};
/*---------------------------------------------------------------------------*/
volatile static sig_atomic_t g_shutdown = 0;
/*---------------------------------------------------------------------------*/
int set_nonblocking(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags == -1) return -1;
  return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}
/*---------------------------------------------------------------------------*/
void *handle_client(void *arg) {
  TRACE_PRINT();
  struct thread_args *args = (struct thread_args *)arg;
  struct skvs_ctx *ctx = args->ctx;
  int idx = args->idx;
  int listenfd = args->listenfd;
  /*---------------------------------------------------------------------------*/
  /* free to declare any variables */
  int clientfd;
  char buffer[BUFFER_SIZE + 10];
  /*---------------------------------------------------------------------------*/

  free(args);
  printf("%dth worker ready\n", idx);

  /*---------------------------------------------------------------------------*/
  /* edit here */
  while (!g_shutdown) {
    clientfd = accept(listenfd, NULL, NULL);
    if (clientfd < 0) {
      if (errno == EINTR && g_shutdown) break;
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        usleep(1000);
        continue;
      }
      continue;
    }

    if (set_nonblocking(clientfd) < 0) {
      perror("fcntl");
      close(clientfd);
      continue;
    }

    while (!g_shutdown) {
      ssize_t bytes_read, total_read;
      total_read = 0;

      while (!g_shutdown) {
        bytes_read = read(clientfd, buffer + total_read,
                          sizeof(buffer) - 1 - total_read);
        if (bytes_read > 0) {
          total_read += bytes_read;
          buffer[total_read] = '\0';
          if (buffer[total_read - 1] == '\n') {
            break;
          }
        } else if (bytes_read == -1 &&
                   (errno == EAGAIN || errno == EWOULDBLOCK)) {
          usleep(1000);
          continue;
        } else if (bytes_read == 0) {
          break;
        } else {
          perror("Read error");
          break;
        }
      }

      if (total_read <= 0) {
        if (total_read == 0 && !g_shutdown)
          printf("Connection closed by client\n");
        break;
      }

      if (total_read == 1 && buffer[0] == '\n') {
        break;
      }
      // printf("\nrequest: %s\n", buffer);

      const char *response = skvs_serve(ctx, buffer, total_read);
      snprintf(buffer, sizeof(buffer), "%s\n", response);
      // printf("response: %s\n", response);

      ssize_t total_written = 0, bytes_written;
      size_t buffer_length = strlen(buffer);
      while (total_written < buffer_length) {
        bytes_written = write(clientfd, buffer + total_written,
                              buffer_length - total_written);
        if (bytes_written > 0) {
          total_written += bytes_written;
        } else if (bytes_written == -1 &&
                   (errno == EAGAIN || errno == EWOULDBLOCK)) {
          usleep(1000);
          continue;
        } else {
          perror("Write error");
          break;
        }
      }
    }
    close(clientfd);
  }
  /*---------------------------------------------------------------------------*/

  return NULL;
}
/*---------------------------------------------------------------------------*/
/* Signal handler for SIGINT */
void handle_sigint(int sig) {
  TRACE_PRINT();
  printf("\nReceived SIGINT, initiating shutdown...\n");
  g_shutdown = 1;
}
/*---------------------------------------------------------------------------*/
int main(int argc, char *argv[]) {
  size_t hash_size = DEFAULT_HASH_SIZE;
  char *ip = DEFAULT_ANY_IP;
  int port = DEFAULT_PORT, opt;
  int num_threads = NUM_THREADS;
  int delay = RWLOCK_DELAY;
  /*---------------------------------------------------------------------------*/
  /* free to declare any variables */
  struct skvs_ctx *ctx;
  int listen_fd;
  struct sockaddr_in server_addr;
  pthread_t *threads;
  struct thread_args *args;
  /*---------------------------------------------------------------------------*/

  /* parse command line options */
  while ((opt = getopt(argc, argv, "p:t:s:d:h")) != -1) {
    switch (opt) {
      case 'p':
        port = atoi(optarg);
        break;
      case 't':
        num_threads = atoi(optarg);
        break;
      case 's':
        hash_size = atoi(optarg);
        if (hash_size <= 0) {
          perror("Invalid hash size");
          exit(EXIT_FAILURE);
        }
        break;
      case 'd':
        delay = atoi(optarg);
        break;
      case 'h':
      default:
        printf(
            "Usage: %s [-p port (%d)] "
            "[-t num_threads (%d)] "
            "[-d rwlock_delay (%d)] "
            "[-s hash_size (%d)]\n",
            argv[0], DEFAULT_PORT, NUM_THREADS, RWLOCK_DELAY,
            DEFAULT_HASH_SIZE);
        exit(EXIT_FAILURE);
    }
  }

  /*---------------------------------------------------------------------------*/
  /* edit here */
  ctx = skvs_init(hash_size, delay);
  if (ctx == NULL) {
    fprintf(stderr, "Failed to initialize SKVS context\n");
    exit(EXIT_FAILURE);
  }

  if (signal(SIGINT, handle_sigint) == SIG_ERR) {
    perror("signal");
    exit(EXIT_FAILURE);
  }

  listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (listen_fd < 0) {
    perror("socket()");
    skvs_destroy(ctx, 1);
    exit(EXIT_FAILURE);
  }
  if (set_nonblocking(listen_fd) < 0) {
    perror("fcntl()");
    close(listen_fd);
    skvs_destroy(ctx, 1);
    exit(EXIT_FAILURE);
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port);
  if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    perror("bind()");
    close(listen_fd);
    skvs_destroy(ctx, 1);
    exit(EXIT_FAILURE);
  }

  if (listen(listen_fd, NUM_BACKLOG) < 0) {
    perror("listen");
    close(listen_fd);
    skvs_destroy(ctx, 1);
    exit(EXIT_FAILURE);
  }

  printf("Server listening on %s:%d\n", ip, port);

  threads = malloc(num_threads * sizeof(pthread_t));
  if (threads == NULL) {
    perror("malloc");
    close(listen_fd);
    skvs_destroy(ctx, 1);
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < num_threads; i++) {
    args = malloc(sizeof(struct thread_args));
    if (!args) {
      perror("malloc");
      free(threads);
      close(listen_fd);
      skvs_destroy(ctx, 1);
      exit(EXIT_FAILURE);
    }

    args->listenfd = listen_fd;
    args->idx = i;
    args->ctx = ctx;

    if (pthread_create(&threads[i], NULL, handle_client, args) != 0) {
      perror("pthread_create");
      free(args);
      close(listen_fd);
      skvs_destroy(ctx, 1);
      exit(EXIT_FAILURE);
    }
  }

  while (!g_shutdown) {
    sleep(1);
  }

  printf("Shutting down server...\n");

  for (int i = 0; i < num_threads; i++) {
    pthread_join(threads[i], NULL);
  }

  /* Cleanup */
  free(threads);
  close(listen_fd);
  skvs_destroy(ctx, 1);

  /*---------------------------------------------------------------------------*/

  return 0;
}
