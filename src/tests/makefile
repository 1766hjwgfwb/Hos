

CFLAGS:= -m32 # 32-bit mode
# CFLAGS+= -fno-builtin # disable built-in functions
# CFLAGS+= -nostdinc # disable standard includes
# CFLAGS+= -fno-pic # disable position-independent code
# CFLAGS+= -nostdlib # disable standard libraries
# CFLAGS+= -fno-stack-protector # disable stack protection
CFLAGS:= $(strip ${CFLAGS})


.PHONY: hello.s
hello.s: hello.c
	gcc ${CFLAGS} -S $< -o $@