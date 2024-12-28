#include "sockets.h"
#include "signal.h"
#include "snprintf.h"
#include "uptime.h"

int errno = 0;
int verbose = 1;
char **environ;

void my_exit(int status) {
  my_syscall(60, status, 0, 0, 0, 0, 0);
}

void message(int fd, const char *prefix, const char *fmt, va_list argp) {
  char s[1024];
  vsnprintf(s, sizeof(s), fmt, argp);
  if (prefix) p(fd, prefix); p(fd, s); p(fd, "\n");
}

void errx(int status, const char *fmt, ...) {
  va_list ap; va_start(ap, fmt); message(2, "Error: ", fmt, ap); va_end(ap);
  my_exit(status);
}

void warnx(const char *fmt, ...) {
  va_list ap; va_start(ap, fmt); message(2, "Warning: ", fmt, ap); va_end(ap);
}

void pp(const char *fmt, ...) {
  if (!verbose) return;
  va_list ap; va_start(ap, fmt); message(1, NULL, fmt, ap); va_end(ap);
}

int send_uptime(int fd) {
  char buf[1024];
  if (uptime(buf, sizeof(buf)) < 0) return -1;
  return p(fd, buf);
}

//void sig_term(int _) { pp("*** SIGINT ***"); }

void sig_child(int _) {
  int pid, stat;
  while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
    pp("Child %d terminated", pid);
}

void usage() {
  p(2, "Usage: uptime-src [port]\n");
  my_exit(1);
}

char* getenv(const char *var) {
  char **ep;

  if ( !(ep = environ)) return NULL;
  int len = my_strlen(var);
  while (*ep) {
    if (my_memcmp(var, *ep, len) == 0 && (*ep)[len] == '=') {
      return *ep + len + 1;
    }
    ep++;
  }
  return NULL;
}

int main(int _, char **argv, char **__environ) {
  environ = __environ;
  if (getenv("VERBOSE"))
    if (0 == my_memcmp("0", getenv("VERBOSE"), 1)) verbose = 0;

  char *addr = "0.0.0.0";
  char *_port = argv[1] ? argv[1] : "1377";
  int port = atoi(_port);
  if (port < 1 || port > 2<<15) usage();

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port);

  int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  int on;
  int r = setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
  if (r < 0) errx(1, "setsockopt: %d", r);
  r = bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (r < 0) errx(1, "bind: %s:%d: %d", addr, port, r);
  r = listen(listen_fd, 5);
  if (r < 0) errx(1, "listen: %s:%d", addr, port, r);

  //signal(SIGINT, sig_term);
  signal(SIGCHLD, sig_child);

  pp("Listening on %s:%d", addr, port);
  for (;;) {
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    int conn_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &len);
    if (conn_fd < 0) {
      if (errno == EINTR) continue;
      errx(1, "no accept: %d", errno);
    }

    int cpid = fork();
    if (cpid == 0) { // child
      close(listen_fd);
      if (send_uptime(conn_fd) < 0) warnx("failed to send reply");
      my_exit(0);
    } else if (cpid < 0) {
      warnx("no fork");
    }

    // parent
    close(conn_fd);
  }

  return 0;
}

asm(".section .text\n"
    ".weak _start\n"
    "_start:\n"
    "pop %rdi\n"                // argc   (first arg, %rdi)
    "mov %rsp, %rsi\n"          // argv[] (second arg, %rsi)
    "lea 8(%rsi,%rdi,8),%rdx\n" // then a NULL then envp (third arg, %rdx)
    "xor %ebp, %ebp\n"          // zero the stack frame
    "and $-16, %rsp\n"          // x86 ABI : esp must be 16-byte aligned before call
    "call main\n"               // main() returns the status code, we'll exit with it.
    "mov %eax, %edi\n"          // retrieve exit code (32 bit)
    "mov $60, %eax\n"           // NR_exit == 60
    "syscall\n"                 // really exit
    "hlt\n"                     // ensure it does not return
    "");
