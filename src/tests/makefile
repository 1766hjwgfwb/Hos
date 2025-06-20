

CFLAGS:= -m32 # 32-bit mode
CFLAGS+= -Qn # no function names in assembly output
# CFLAGS+= -fno-builtin # disable built-in functions
# CFLAGS+= -nostdinc # disable standard includes
CFLAGS+= -fno-pic # disable position-independent code
# CFLAGS+= -nostdlib # disable standard libraries
# CFLAGS+= -fno-stack-protector # disable stack protection
CFLAGS+= -mpreferred-stack-boundary=2 # set stack boundary to 2
# CFLAGS+= -fomit-frame-pointer # omit frame pointer
CFLAGS+= -fno-asynchronous-unwind-tables # disable async unwind table
CFLAGS+= -O0 # disable optimization
CFLAGS:= $(strip ${CFLAGS})


.PHONY: hello.s
hello.s: hello.c
	gcc ${CFLAGS} -S $< -o $@

.PHONY: add.s
add.s: add.c
	gcc ${CFLAGS} -S $< -o $@