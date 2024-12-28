#ifndef SOCKETS_H
#define SOCKETS_H

#include "u.h"

#define AF_INET      2
#define SOCK_STREAM  1
#define INADDR_ANY   0
#define SOL_SOCKET   1
#define SO_REUSEADDR 2

typedef unsigned int    socklen_t;
typedef unsigned short  sa_family_t;
typedef unsigned int    in_addr_t;
typedef unsigned short  in_port_t;

struct in_addr {
  in_addr_t s_addr;
};

struct sockaddr {
  uint8_t      sa_len;
  sa_family_t  sa_family;
  char         sa_data[14];
};

// kernel
struct sockaddr_in {
  sa_family_t sin_family;
  in_port_t sin_port;
  struct in_addr sin_addr;

  // pad to size of `struct sockaddr'. bind() fails w/o this!
  unsigned char __pad[sizeof(struct sockaddr) - sizeof(short int) -
                      sizeof(unsigned short int) - sizeof(struct in_addr)];
};

uint16_t htons(uint16_t n);
int socket(int domain, int type, int protocol);
int bind(int fd, const struct sockaddr *addr, socklen_t len);
int listen(int sockfd, int backlog);
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int setsockopt(int fd, int level, int optname,
               const void *optval, socklen_t optlen);
#endif
