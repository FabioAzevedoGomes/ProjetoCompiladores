# ==========================================================
#                  INICIO DO DATA SEGMENT
# ==========================================================
	.file	"data_segment_test.c"
	.text
	.comm	global_var_1,4,4
	.comm	global_var_2,4,4
	.comm	global_vec_1,40,32
	.comm	global_vec_2,80,32

# ==========================================================
#                  INICIO DA FUNCAO FOO
# ==========================================================
	.globl	foo
	.type	foo, @function
foo:
.LFB0:
	.cfi_startproc
# ==========================================================
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	
	# Division is here maybe?
	
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
# ==========================================================
	.cfi_endproc
.LFE0:
	.size	foo, .-foo

# ==========================================================
#                  INICIO DA FUNCAO MAIN
# ==========================================================
	.globl	main
	.type	main, @function
main:
.LFB1:
	.cfi_startproc
# ==========================================================
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	
	# Division is here maybe?
	
	movl	$0, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
# ==========================================================
	.cfi_endproc
.LFE1:
	.size	main, .-main

# ==========================================================
#               FIM DO PROGRAMA
# ==========================================================
	.ident	"GCC: (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits

