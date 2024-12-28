CC := clang
LD := ld.lld
override CFLAGS := -Wall -Os -fno-ident $(CFLAGS)

out := _out
obj := $(patsubst %.c, $(out)/%.o, $(wildcard *.c))
deps := $(addsuffix .deps, $(obj))
vendor := vendor/stb_sprintf.h

$(out)/uptime-tcp-nolibc: $(obj)
	$(LD) -e _start -o $@ $^
	objcopy -R '.note.*' -R .comment $@

vendor: $(vendor)

$(out)/%.o: %.c
	$(mkdir)
	$(CC) $(CFLAGS) -c -o $@ $<
	cpp -nostdinc -M -MP $< -MT $@ > $@.deps

vendor/stb_sprintf.h: /usr/include/stb_sprintf.h
	cp $< $@
	patch -p0 < vendor/stb_sprintf.h.patch

-include $(deps)

debug:
	rm -rf $(out)
	$(MAKE) CFLAGS='-gdwarf-2 -g3 -O0'
	gdb $(out)/uptime-tcp-nolibc

info: $(obj)
	wc -lc *.[ch] | while read -r lines bytes file; do \
	 elf=-;\
	 echo $$file | grep -q '\.c$$' && elf=`stat -c%s $(out)/$${file%.*}.o`;\
	 echo $$file $$lines $$bytes $$elf;\
	done | column -t -N\ ,LINES,BYTES,ELF

mkdir = @mkdir -p $(dir $@)
