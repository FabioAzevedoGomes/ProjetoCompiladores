	.file	"function_test.c"
	.text
	.globl	z
	.data
	.align 4
	.type	z, @object
	.size	z, 4
z:
	.long	2
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	pushq	%rbp
	movq	%rsp, %rbp
	movl	z(%rip), %eax
	movl	%eax, -4(%rbp)
	movl	$0, %eax
	popq	%rbp
	ret

.LFE0:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
