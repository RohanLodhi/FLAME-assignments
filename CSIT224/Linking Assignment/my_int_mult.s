	.file	"my_int_mult.c"
	.text
	.globl	my_int_mult
	.type	my_int_mult, @function
my_int_mult:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	imull	-8(%rbp), %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	my_int_mult, .-my_int_mult
	.ident	"GCC: (GNU) 12.3.1 20230508 (Red Hat 12.3.1-1)"
	.section	.note.GNU-stack,"",@progbits
