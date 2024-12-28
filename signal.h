#ifndef SIGNAL_H
#define SIGNAL_H

typedef int           __pid_t;
typedef unsigned int  __uid_t;
typedef long          __clock_t;
typedef unsigned int  __uint32_t;

typedef struct {
  unsigned long __val[16];
} sigset_t;

union sigval {
  int sival_int;
  void *sival_ptr;
};

typedef union sigval __sigval_t;

typedef struct {
  int si_signo;
  int si_errno;
  int si_code;
  int __pad0;
  union {
    int _pad[28];
    struct {
      __pid_t si_pid;
      __uid_t si_uid;
    } _kill;
    struct {
      int si_tid;
      int si_overrun;
      __sigval_t si_sigval;
    } _timer;
    struct {
      __pid_t si_pid;
      __uid_t si_uid;
      __sigval_t si_sigval;
    } _rt;
    struct {
      __pid_t si_pid;
      __uid_t si_uid;
      int si_status;
      __clock_t si_utime;
      __clock_t si_stime;
    } _sigchld;
    struct {
      void *si_addr;
      short si_addr_lsb;
      union {
        struct {
          void *_lower;
          void *_upper;
        } _addr_bnd;
        __uint32_t _pkey;
      } _bounds;
    } _sigfault;
    struct {
      long si_band;
      int si_fd;
    } _sigpoll;
    struct {
      void *_call_addr;
      int _syscall;
      unsigned int _arch;
    } _sigsys;
  } _sifields;
} siginfo_t;

typedef void (*sighandler_t) (int);

struct sigaction {
  union {
    sighandler_t sa_handler;
    void (*sa_sigaction)(int, siginfo_t *, void *);
  } sigaction_handler;
  sigset_t sa_mask;
  int sa_flags;
  void (*sa_restorer)(void);
};

#define sa_handler    sigaction_handler.sa_handler
#define sa_sigaction  sigaction_handler.sa_sigaction

#define __SIGRTMAX  64
#define _NSIG       (__SIGRTMAX + 1)

#define SA_RESTORER   0x04000000

#define SA_RESTART    0x10000000
#define SA_INTERRUPT  0x20000000

/* Fake signal functions.  */
#define SIG_ERR  ((sighandler_t) -1)    /* Error return.  */
#define SIG_DFL  ((sighandler_t)  0)    /* Default action.  */
#define SIG_IGN  ((sighandler_t)  1)    /* Ignore signal.  */
#define SIG_HOLD ((sighandler_t)  2)    /* Add signal to hold mask.  */

#define SIGINT          2       /* Interactive attention signal.  */
#define SIGTERM         15      /* Termination request.  */
#define SIGHUP          1       /* Hangup.  */
#define SIGQUIT         3       /* Quit.  */
#define SIGPIPE         13      /* Broken pipe.  */
#define SIGALRM         14      /* Alarm clock.  */
#define SIGSYS          31      /* Bad system call.  */
#define SIGCHLD         17      /* Child terminated or stopped.  */

int sigemptyset(sigset_t *set);
int sigaction(int sig, const struct sigaction *act, struct sigaction *oact);
sighandler_t signal(int signo, sighandler_t func);

#endif
