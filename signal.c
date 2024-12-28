#include "u.h"
#include "signal.h"

int sigemptyset(sigset_t *set) {
  if (set == NULL) {
    errno = EINVAL;
    return -1;
  }
  memset(set, 0, sizeof(sigset_t));
  return 0;
}

__asm__(
  ".globl restore_rt\n"
  "restore_rt:\n"
  "    movq $15, %rax\n" // 15 is rt_sigreturn on x86_64
  "    syscall\n"
  "    hlt\n" // Ensure the process halts if syscall fails (should never happen)
);

void restore_rt();

struct kernel_sigaction {
  sighandler_t k_sa_handler;
  unsigned long sa_flags;
  void (*sa_restorer) (void);
  sigset_t sa_mask;
};

int sigaction(int sig, const struct sigaction *act, struct sigaction *oact) {
  if (sig <= 0 || sig >= _NSIG) {
    errno = EINVAL;
    return -1;
  }

  struct kernel_sigaction kact, koact;

  kact.k_sa_handler = act->sa_handler;
  memcpy(&kact.sa_mask, &act->sa_mask, sizeof(sigset_t));
  kact.sa_flags = act->sa_flags;
  kact.sa_flags = act->sa_flags | SA_RESTORER;

  kact.sa_restorer = &restore_rt; // the most important step

  int r = my_syscall(13, sig,
                     (long)(act?&kact:NULL), (long)(oact?&koact:NULL),
                     _NSIG/8, 0, 0);

  if (oact && r >= 0) {
    oact->sa_handler = koact.k_sa_handler;
    memcpy (&oact->sa_mask, &koact.sa_mask, sizeof(sigset_t));
    oact->sa_flags = koact.sa_flags;

    oact->sa_restorer = kact.sa_restorer;
  }

  return r;
}

sighandler_t signal(int signo, sighandler_t func) {
  struct sigaction act, oact;

  act.sa_handler = func;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;

  if (signo == SIGALRM) {
    act.sa_flags |= SA_INTERRUPT;
  } else {
    act.sa_flags |= SA_RESTART;
  }

  if (sigaction(signo, &act, &oact) < 0) return SIG_ERR;

  return oact.sa_handler;
}
