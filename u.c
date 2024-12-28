#include "u.h"

long my_syscall(long number, long arg1, long arg2, long arg3, long arg4, long arg5, long arg6) {
    long result;
    register long r10 asm ("r10") = arg4;
    register long r8 asm ("r8") = arg5;
    register long r9 asm ("r9") = arg6;
    __asm__ volatile (
        "syscall"
        : "=a" (result)
        : "a" (number), "D" (arg1), "S" (arg2), "d" (arg3), "r" (r10), "r" (r8), "r" (r9)
        : "rcx", "r11", "memory"
    );
    return result;
}

long my_strlen(const char *s) {
  const char *sc;
  for (sc = s; *sc != '\0'; ++sc)
    /* nothing */;
  return sc - s;
}

long p(int fd, const char *s) {
  return my_syscall(1, fd, (long)s, my_strlen(s), 0, 0, 0);
}

ssize_t read(int fd, void *buf, size_t nbytes) {
  return my_syscall(0, fd, (long)buf, nbytes, 0, 0, 0);
}

int my_open(const char *path, int flags, mode_t mode) {
  return my_syscall(2, (long)path, flags, mode, 0, 0, 0);
}

int putchar(int c) {
  char s[1] = { c };
  return my_syscall(1, 1, (long)s, 1, 0, 0, 0) == 1 ? c : -1;
}

void *memset(void *dest, int c, size_t n) {
  unsigned char *s = dest;
  for (; n; n--, s++) *s = c;
  return dest;
}

void *memcpy(void *dest, const void *src, size_t count) {
  char *tmp = dest;
  const char *s = src;
  while (count--) *tmp++ = *s++;
  return dest;
}

int close(int fd) {
  return my_syscall(3, fd, 0, 0, 0, 0, 0);
}

int waitpid(int pid, int *status, int options) {
  // 61 is wait4
  return my_syscall(61, pid, (long)status, options, 0, 0, 0);
}

int fork() {
  return my_syscall(57, 0, 0, 0, 0, 0, 0);
}

int isspace(int c) {
  return c == ' ' || (unsigned)c-'\t' < 5;
}

int isdigit(int c) {
  return (unsigned)c-'0' < 10;
}

int atoi(const char *s) {
  int i, n;

  for (i = 0; isspace(s[i]); i++)
    ;

  int sign = (s[i] == '-') ? -1 : 1;
  if (s[i] == '+' || s[i] == '-')
    i++;

  for (n = 0; isdigit(s[i]); i++)
    n = 10 * n + (s[i] - '0');

  return sign * n;
}

int my_memcmp(const void *cs, const void *ct, size_t count) {
  const unsigned char *su1, *su2;
  int res = 0;
  for (su1 = cs, su2 = ct; 0 < count; ++su1, ++su2, count--)
    if ((res = *su1 - *su2) != 0)
      break;
  return res;
}
