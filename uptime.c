#include "u.h"
#include "uptime.h"
#include "snprintf.h"

struct sysinfo {
  long uptime;             /* Seconds since boot */
  unsigned long loads[3];  /* 1, 5, and 15 minute load averages */
  unsigned long totalram;  /* Total usable main memory size */
  unsigned long freeram;   /* Available memory size */
  unsigned long sharedram; /* Amount of shared memory */
  unsigned long bufferram; /* Memory used by buffers */
  unsigned long totalswap; /* Total swap space size */
  unsigned long freeswap;  /* Swap space still available */
  unsigned short procs;    /* Number of current processes */
  unsigned long totalhigh; /* Total high memory size */
  unsigned long freehigh;  /* Available high memory size */
  unsigned int mem_unit;   /* Memory unit size in bytes */
  char _f[20-2*sizeof(long)-sizeof(int)]; /* Padding to 64 bytes */
};

#define SI_LOAD_SHIFT 16

typedef long time_t;
typedef long suseconds_t;

struct timeval {
  time_t      tv_sec;     /* seconds */
  suseconds_t tv_usec;    /* microseconds */
};

struct timezone {
  int tz_minuteswest;     /* minutes west of Greenwich */
  int tz_dsttime;         /* type of DST correction */
};

static float la(unsigned long n) {
  return n * (1.f / (1 << SI_LOAD_SHIFT));
}

int uptime(char *buf, int buf_len) {
  struct sysinfo info;
  if (my_syscall(99, (long)&info, 0, 0, 0, 0, 0) < 0) return -1;

  int updays = info.uptime / 86400;
  int uphours = info.uptime % 86400 / 3600;
  int upmins = info.uptime % 86400 % 3600 / 60;
  char up[100];
  if (updays > 0)
    snprintf(up, sizeof(up), "%jd day(s) %2d:%02d", updays, uphours, upmins);
  else
    snprintf(up, sizeof(up), "%2d:%02d", uphours, upmins);

  struct timeval tv;
  struct timezone tz;
  if (my_syscall(96, (long)&tv, (long)&tz, 0, 0, 0, 0) < 0) return -1;

  int sec = tv.tv_sec - tz.tz_minuteswest * 60;
  int hours = sec / 3600 % 24;
  int minutes = sec / 60 % 60;
  int seconds = sec % 60;

  snprintf(buf, buf_len,
           " %02d:%02d:%02d up %s,  load average: %.2f, %.2f, %.2f\n",
           hours, minutes, seconds, up,
           la(info.loads[0]), la(info.loads[1]), la(info.loads[2]));
  return 0;
}
