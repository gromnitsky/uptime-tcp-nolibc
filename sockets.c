#include "sockets.h"

static uint16_t bswap_16(uint16_t x) {
  return x<<8 | x>>8;
}

// musl
uint16_t htons(uint16_t n) {
  union { int i; char c; } u = { 1 };
  return u.c ? bswap_16(n) : n;
}

int bind(int fd, const struct sockaddr *addr, socklen_t len) {
  return my_syscall(49, fd, (long)addr, len, 0, 0, 0);
}

int listen(int sockfd, int backlog) {
  return my_syscall(50, sockfd, backlog, 0, 0, 0, 0);
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
  int r = my_syscall(43, sockfd, (long)addr, (long)addrlen, 0, 0, 0);
  if (r < 0) {
    errno = -r;
    r = -1;
  }
  return r;
}

int socket(int domain, int type, int protocol) {
  return my_syscall(41, domain, type, protocol, 0, 0, 0);
}

int setsockopt(int fd, int level, int optname,
               const void *optval, socklen_t optlen) {
  return my_syscall(54, fd, level, optname, (long)optval, optlen, 0);
}
