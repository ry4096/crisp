	.file	"struct_test.c"
	.section	.rodata
	.align 4
.LC0:
	.string	"{(a %c) (b %d) (c %f) (d %llf) (e %c)}"
	.text
	.globl	A_print
	.type	A_print, @function
A_print:
.LFB0:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$60, %esp
	.cfi_offset 7, -12
	.cfi_offset 6, -16
	.cfi_offset 3, -20
	movl	8(%ebp), %eax
	movzbl	24(%eax), %eax
	movsbl	%al, %edi
	movl	8(%ebp), %eax
	movl	20(%eax), %ecx
	movl	16(%eax), %edx
	movl	12(%eax), %eax
	movl	8(%ebp), %ebx
	flds	8(%ebx)
	movl	8(%ebp), %ebx
	movl	4(%ebx), %esi
	movl	8(%ebp), %ebx
	movzbl	(%ebx), %ebx
	movsbl	%bl, %ebx
	movl	%edi, 32(%esp)
	movl	%eax, 20(%esp)
	movl	%edx, 24(%esp)
	movl	%ecx, 28(%esp)
	fstpl	12(%esp)
	movl	%esi, 8(%esp)
	movl	%ebx, 4(%esp)
	movl	$.LC0, (%esp)
	call	printf
	addl	$60, %esp
	popl	%ebx
	.cfi_restore 3
	popl	%esi
	.cfi_restore 6
	popl	%edi
	.cfi_restore 7
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	A_print, .-A_print
	.section	.rodata
	.align 4
.LC2:
	.string	"{(a %hd) (b %c) (c1 %llu) (c2 %llu) (c3 %llu) (c4 %llu) (d %c) (e %c)}"
	.text
	.globl	B_print
	.type	B_print, @function
B_print:
.LFB1:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$124, %esp
	.cfi_offset 7, -12
	.cfi_offset 6, -16
	.cfi_offset 3, -20
	movl	8(%ebp), %eax
	movzbl	12(%eax), %eax
	movsbl	%al, %eax
	movl	%eax, -28(%ebp)
	movl	8(%ebp), %eax
	movzbl	11(%eax), %eax
	movsbl	%al, %eax
	movl	%eax, -32(%ebp)
	movl	8(%ebp), %eax
	movzwl	9(%eax), %eax
	movzwl	%ax, %eax
	movl	%eax, -40(%ebp)
	movl	$0, -36(%ebp)
	movl	8(%ebp), %eax
	movzwl	7(%eax), %eax
	movzwl	%ax, %eax
	movl	%eax, -48(%ebp)
	movl	$0, -44(%ebp)
	movl	8(%ebp), %eax
	movzwl	5(%eax), %eax
	movzwl	%ax, %eax
	movl	%eax, %ebx
	movl	$0, %esi
	movl	8(%ebp), %eax
	movzwl	3(%eax), %eax
	movzwl	%ax, %eax
	movl	%eax, -56(%ebp)
	movl	$0, -52(%ebp)
	movl	8(%ebp), %eax
	movzbl	2(%eax), %eax
	movsbl	%al, %edi
	movl	8(%ebp), %eax
	movzwl	(%eax), %eax
	cwtl
	movl	%eax, -60(%ebp)
	movl	-28(%ebp), %eax
	movl	%eax, 48(%esp)
	movl	-32(%ebp), %edx
	movl	%edx, 44(%esp)
	movl	-40(%ebp), %eax
	movl	-36(%ebp), %edx
	movl	%eax, 36(%esp)
	movl	%edx, 40(%esp)
	movl	-48(%ebp), %eax
	movl	-44(%ebp), %edx
	movl	%eax, 28(%esp)
	movl	%edx, 32(%esp)
	movl	%ebx, 20(%esp)
	movl	%esi, 24(%esp)
	movl	-56(%ebp), %eax
	movl	-52(%ebp), %edx
	movl	%eax, 12(%esp)
	movl	%edx, 16(%esp)
	movl	%edi, 8(%esp)
	movl	-60(%ebp), %edx
	movl	%edx, 4(%esp)
	movl	$.LC2, (%esp)
	call	printf
	addl	$124, %esp
	popl	%ebx
	.cfi_restore 3
	popl	%esi
	.cfi_restore 6
	popl	%edi
	.cfi_restore 7
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE1:
	.size	B_print, .-B_print
	.section	.rodata
	.align 4
.LC3:
	.string	"{(a %lld) (b %c) (c %ld) (d %c) (e %lld)}"
	.text
	.globl	C_print
	.type	C_print, @function
C_print:
.LFB2:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$60, %esp
	.cfi_offset 7, -12
	.cfi_offset 6, -16
	.cfi_offset 3, -20
	movl	8(%ebp), %eax
	movl	20(%eax), %ecx
	movl	24(%eax), %ebx
	movl	8(%ebp), %eax
	movzbl	16(%eax), %eax
	movsbl	%al, %eax
	movl	%eax, -28(%ebp)
	movl	8(%ebp), %eax
	movl	12(%eax), %edi
	movl	8(%ebp), %eax
	movzbl	8(%eax), %eax
	movsbl	%al, %esi
	movl	8(%ebp), %eax
	movl	4(%eax), %edx
	movl	(%eax), %eax
	movl	%ecx, 24(%esp)
	movl	%ebx, 28(%esp)
	movl	-28(%ebp), %ecx
	movl	%ecx, 20(%esp)
	movl	%edi, 16(%esp)
	movl	%esi, 12(%esp)
	movl	%eax, 4(%esp)
	movl	%edx, 8(%esp)
	movl	$.LC3, (%esp)
	call	printf
	addl	$60, %esp
	popl	%ebx
	.cfi_restore 3
	popl	%esi
	.cfi_restore 6
	popl	%edi
	.cfi_restore 7
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE2:
	.size	C_print, .-C_print
	.section	.rodata
.LC4:
	.string	"{(a %c) (b "
.LC5:
	.string	")}"
	.text
	.globl	D_print
	.type	D_print, @function
D_print:
.LFB3:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$24, %esp
	movl	8(%ebp), %eax
	movzbl	(%eax), %eax
	movsbl	%al, %eax
	movl	%eax, 4(%esp)
	movl	$.LC4, (%esp)
	call	printf
	movl	8(%ebp), %eax
	addl	$4, %eax
	movl	%eax, (%esp)
	call	C_print
	movl	$.LC5, (%esp)
	call	printf
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE3:
	.size	D_print, .-D_print
	.section	.rodata
.LC6:
	.string	"{(a %c) (b %lld)}"
	.text
	.globl	E_print
	.type	E_print, @function
E_print:
.LFB4:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$24, %esp
	movl	8(%ebp), %eax
	movl	8(%eax), %edx
	movl	4(%eax), %eax
	movl	8(%ebp), %ecx
	movzbl	(%ecx), %ecx
	movsbl	%cl, %ecx
	movl	%eax, 8(%esp)
	movl	%edx, 12(%esp)
	movl	%ecx, 4(%esp)
	movl	$.LC6, (%esp)
	call	printf
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE4:
	.size	E_print, .-E_print
	.ident	"GCC: (Debian 4.7.2-5) 4.7.2"
	.section	.note.GNU-stack,"",@progbits
