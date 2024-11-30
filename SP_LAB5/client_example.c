#define _POSIX_C_SOURCE 200809L
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 1024
int main(const int argc, const char** argv) 
{
  int s, port, len, res, off;
  char buf[MAX_LINE];  
    
  if (argc < 3 ||  ((port = atoi(argv[2])) < 1024 || port > 65535)) {
    fprintf(stderr, "usage: %s domain-name port(1024-65535)\n", argv[0]);
    return -1;    
  }    

  /* create a socket for connection */
  if ((s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      perror("socket() failed");
      return -1;     
  }

#ifdef USE_GETADDRINFO
 struct addrinfo hints, *ai, *ai_it;

  /* domain name lookup */
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;    
  res = getaddrinfo(argv[1], argv[2], &hints, &ai);;
  if (res != 0) {
    fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(res));
    exit(-1);
  }
 
  /* try connecting to next address if any fails */
  ai_it = ai; 
  while (ai_it != NULL) {        
    if (connect(s, ai_it->ai_addr, ai_it->ai_addrlen) == 0)
      break;      
    ai_it = ai_it->ai_next;
  }         
  freeaddrinfo(ai);

#else
  struct hostent *hp;
  struct sockaddr_in saddr;
  
  /* domain name lookup */
  if ((hp = gethostbyname(argv[1])) == NULL) {
     fprintf(stderr, "gethostbyname failed: %s\n", strerror(errno));
     exit(-1);
  }

  /* connect to one address returned by gethostbyname() */
  saddr.sin_family = AF_INET;
  memcpy(&saddr.sin_addr.s_addr, hp->h_addr_list[0], hp->h_length);
  saddr.sin_port = htons(port);
  if (connect(s, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
    perror("connect()");
    exit(-1);      
  }
  #endif

  /* this works only when the input size is small */
  while (fgets(buf, sizeof(buf), stdin) != NULL) {
    
    len = strlen(buf);
    off = 0;
    while (off < len) {
      if ((res = write(s, buf + off, len - off)) <= 0) {
        perror("write");
        exit(-1);
      }
      off += res;
    }
    if ((res = read(s, buf, sizeof(buf)-1)) <= 0) {
      perror("read");
      exit(-1);
    }
    buf[res] = 0;
    printf("%s", buf);
  }  
  close(s);
  
  return 0;
}
        