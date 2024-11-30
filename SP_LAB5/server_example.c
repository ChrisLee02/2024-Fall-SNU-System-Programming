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
#include <signal.h>

#define MAX_LINE 1024

/*--------------------------------------------------------------------*/
void
ignore_signal(int sig)
{
    struct sigaction act;
    
    memset(&act, 0, sizeof(act));
    act.sa_handler = SIG_IGN;    
    if (sigaction(sig, &act, NULL) < 0) {
      perror("sigaction");
      exit(-1);
    }

}
/*--------------------------------------------------------------------*/
void 
MainLoop(int s)
{
  int i, c, res, len, off;
  char buf[MAX_LINE];

  while ((c = accept(s, NULL, NULL)) >= 0) {

   while (1) {
      /* read the message from the client */
      if ((res = read(c,  buf,  sizeof(buf) - 1)) <= 0) {
          if (res == 0) printf("connection closed\n");
          if (res < 0) {
              if (errno == EINTR) continue;
              else perror("read");
          }
          break;
      }

      /* main logic - convert lowercase to uppercase */
      len = res;
      for (i = 0; i < len; i++) {
          if (islower(buf[i]))
            buf[i] = toupper(buf[i]);
      }

      off = 0;
      while (off < len) { 
        /* write may not send the entire message at one time 
           so, call write() repeatedly until the entire
           message is sent
        */
        if ((res = write(c, buf+off, len-off)) <= 0) {
            perror("write");       
            break;
        }
        off += res;
      }      
   }      
   /* this connection is done, so close it */
   close(c);
  }
}
/*--------------------------------------------------------------------*/
int 
main(const int argc, const char** argv) 
{
  int s, port;  
    
  if (argc < 2 ||  ((port = atoi(argv[1])) < 1024 || port > 65535)) {
    fprintf(stderr, "usage: %s port(1024-65535)\n", argv[0]);
    return -1;    
  }    

  /* ignore sigpipe */
  ignore_signal(SIGPIPE);
  // ignore_signal(SIGINT);
  
#ifdef USE_GETADDRINFO
  struct addrinfo hints, *ai, *ai_it;
  int res;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
  hints.ai_protocol = 0;

  res = getaddrinfo(NULL, argv[1], &hints, &ai);;
  if (res != 0) {
    fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(res));
    exit(-1);
  }

  ai_it = ai; 
  while (ai_it != NULL) {        
    if ((s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
       perror("socket()");
       return -1;     
    }
    if ((bind(s, ai_it->ai_addr, ai_it->ai_addrlen) == 0) &&
        (listen(s, 1024) == 0)) break; // success
    close(s);   
    ai_it = ai_it->ai_next;
  }         
  freeaddrinfo(ai);

#else
  struct sockaddr_in saddr;

  if ((s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)  {
     perror("socket()");
     return -1;     
  }
  
  memset(&saddr, 0, sizeof(saddr));
  saddr.sin_family = AF_INET;
  saddr.sin_addr.s_addr = INADDR_ANY;
  saddr.sin_port = htons(port);
  if (bind(s, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
      perror("bind()");
      close(s);
      return -1;     
  }

  if (listen(s, 1024) < 0) {
      perror("listen()");
      close(s);
      return -1;     
  }
#endif

#ifdef MULTIPROCESS
  int numCPU = sysconf(_SC_NPROCESSORS_ONLN);
  int i;
  for (i = 0; i < numCPU - 1; i++)
    if (fork() == 0)
      break;
#endif    

  MainLoop(s);  
  close(s);
  return(0);
}
        