/*---------------------------------------------------------------------------*/
/* client.c                                                                  */
/* Author: Junghan Yoon, KyoungSoo Park                                      */
/* Modified by: (Your Name)                                                  */
/*---------------------------------------------------------------------------*/
#define _GNU_SOURCE
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
/*---------------------------------------------------------------------------*/
int main(int argc, char *argv[]) {
  char *ip = DEFAULT_LOOPBACK_IP;
  int port = DEFAULT_PORT;
  //   int interactive = 0; /* Default is non-interactive mode */
  int opt;

  /*---------------------------------------------------------------------------*/
  /* free to declare any variables */
  int socketfd, res;
  char buffer[BUFFER_SIZE + 10];

  /*---------------------------------------------------------------------------*/

  /* parse command line options */
  while ((opt = getopt(argc, argv, "i:p:th")) != -1) {
    switch (opt) {
      case 'i':
        ip = optarg;
        break;
      case 'p':
        port = atoi(optarg);
        if (port <= 1024 || port >= 65536) {
          perror("Invalid port number");
          exit(EXIT_FAILURE);
        }
        break;
      case 't':
        // interactive = 1;
        break;
      case 'h':
      default:
        printf(
            "Usage: %s [-i server_ip_or_domain (%s)] "
            "[-p port (%d)] [-t]\n",
            argv[0], DEFAULT_LOOPBACK_IP, DEFAULT_PORT);
        exit(EXIT_FAILURE);
    }
  }

  /*---------------------------------------------------------------------------*/
  /* edit here */

  if ((socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    perror("socket() failed");
    return -1;
  }

  struct addrinfo hints, *ai, *ai_it;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  char port_str[6];
  if (snprintf(port_str, sizeof(port_str), "%d", port) < 0) {
    perror("snprintf() failed");
    close(socketfd);
    return -1;
  }
  res = getaddrinfo(ip, port_str, &hints, &ai);
  if (res != 0) {
    fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(res));
    close(socketfd);
    return -1;
  }

  ai_it = ai;
  while (ai_it != NULL) {
    if (connect(socketfd, ai_it->ai_addr, ai_it->ai_addrlen) == 0) break;
    ai_it = ai_it->ai_next;
  }
  if (ai_it == NULL) {
    if (ai_it == NULL) {
      fprintf(stderr, "Could not connect to %s:%d\n", ip, port);
      freeaddrinfo(ai);
      close(socketfd);
      return -1;
    }
  }
  freeaddrinfo(ai);
  // printf("Connected to %s:%d\n", ip, port);

  int len, off;
  while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
    len = strlen(buffer);
    if (len == 1 && buffer[0] == '\n') {
      close(socketfd);
      return 0;
    }
    off = 0;
    while (off < len) {
      if ((res = write(socketfd, buffer + off, len - off)) <= 0) {
        perror("write");
        close(socketfd);
        return -1;
      }
      off += res;
    }
    off = 0;
    while ((res = read(socketfd, buffer + off, sizeof(buffer) - 1 - off)) > 0) {
      off += res;
      if (buffer[off - 1] == '\n') break;
    }
    if (res < 0) {
      perror("read");
      close(socketfd);
      return -1;
    }
    if (res == 0) {
      printf("Connection closed by server\n");
      close(socketfd);
      return 0;
    }
    buffer[off] = '\0';
    printf("%s", buffer);
  }
  close(socketfd);
  /*---------------------------------------------------------------------------*/

  return 0;
}
