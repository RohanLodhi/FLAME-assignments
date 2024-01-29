	.file	"my_int_add.c"
	.text
	.globl	my_int_add
	.type	my_int_add, @function
my_int_add:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %edx
	movl	-8(%rbp), %eax
	addl	%edx, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	my_int_add, .-my_int_add
	.ident	"GCC: (GNU) 12.3.1 20230508 (Red Hat 12.3.1-1)"
	.section	.note.GNU-stack,"",@progbits
