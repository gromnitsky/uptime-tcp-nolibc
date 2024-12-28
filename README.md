x86_64 Linux only. Uses clang and its linker for compilation:

~~~
$ sudo dnf install stb_sprintf-devel clang lld
$ make vendor
$ make
~~~

Look at the size of `_out/uptime-tcp-nolibc` (should be < 22KB). This
is a static binary.

Run the server in chroot if don't belive this is possible:

~~~
$ sudo chroot _out ./uptime-tcp-nolibc
Listening on 0.0.0.0:1377
~~~

Connect:

~~~
$ nc --recv-only 127.0.0.1 1377
 12:51:50 up 2 day(s)  3:50,  load average: 0.08, 0.13, 0.14
~~~

## Stats

~~~
$ make info
wc -lc *.[ch] | while read -r lines bytes file; do \
 elf=-;\
 echo $file | grep -q '\.c$' && elf=`stat -c%s _out/${file%.*}.o`;\
 echo $file $lines $bytes $elf;\
done | column -t -N\ ,LINES,BYTES,ELF
            LINES  BYTES  ELF
main.c      134    3657   5896
signal.c    76     1699   1704
signal.h    113    2649   -
snprintf.c  6      120    18168
snprintf.h  12     274    -
sockets.c   37     896    1728
sockets.h   45     1119   -
u.c         98     2152   4496
u.h         46     1353   -
uptime.c    67     2148   2128
uptime.h    6      79     -
total       640    16146  -
~~~

## &#x2672; Loicense

MIT
