	.file	"hello.c"
	.text
	.globl	message
	.data
	.align 4
	.type	message, @object
	.size	message, 14
message:
	.string	"Hello, World!"
	.globl	buf
	.bss
	.align 32
	.type	buf, @object
	.size	buf, 1024
buf:
	.zero	1024
	.text
	.globl	main
	.type	main, @function
main:
	pushl	$message
	call	puts
	addl	$4, %esp
	movl	$0, %eax
	ret
	.size	main, .-main
	.section	.note.GNU-stack,"",@progbits
