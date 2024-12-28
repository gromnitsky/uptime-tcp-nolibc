#ifndef U_H
#define U_H

#define NULL 0

extern int errno;
#define EINVAL          22      /* Invalid argument */
#define EINTR            4      /* Interrupted system call */

/* Bits in the third argument to `waitpid'.  */
#define WNOHANG         1       /* Don't block waiting.  */

// aliases of GCC builtins
#define bool _Bool
#define va_start(v,l)   __builtin_va_start(v,l)
#define va_end(v)       __builtin_va_end(v)
#define va_arg(v,l)     __builtin_va_arg(v,l)
#define va_copy(d,s)    __builtin_va_copy(d,s)
typedef __builtin_va_list va_list;

typedef unsigned char   uint8_t;
typedef unsigned long   size_t;
typedef long            ssize_t;
typedef unsigned short  uint16_t;
typedef long            ptrdiff_t;
typedef unsigned int    mode_t;

#define O_RDONLY 0

long my_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6);
long my_strlen(const char *s);
long p(int fd, const char *s);
ssize_t read (int fd, void *buf, size_t nbytes);
int my_open(const char *path, int flags, mode_t mode);
int putchar(int c);
void *memset(void *dest, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t count);
int my_memcmp(const void *cs, const void *ct, size_t count);
int close(int fd);
int waitpid(int pid, int *status, int options);
int fork();
int isspace(int c);
int isdigit(int c);
int atoi(const char *s);

#endif
