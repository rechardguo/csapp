	.file	"main.c"
	.text
	.globl	add
	.def	add;	.scl	2;	.type	32;	.endef
	.seh_proc	add
add:
	.seh_endprologue
	leaq	(%rcx,%rdx), %rax
	ret
	.seh_endproc
	.def	__main;	.scl	2;	.type	32;	.endef
	.section .rdata,"dr"
.LC0:
	.ascii "%d\0"
	.text
	.globl	main
	.def	main;	.scl	2;	.type	32;	.endef
	.seh_proc	main
main:
	subq	$40, %rsp
	.seh_stackalloc	40
	.seh_endprologue
	call	__main
	movl	$2, %edx
	movl	$1, %ecx
	call	add
	movq	%rax, %rdx
	leaq	.LC0(%rip), %rcx
	call	printf
	movl	$0, %eax
	addq	$40, %rsp
	ret
	.seh_endproc
	.globl	decode
	.def	decode;	.scl	2;	.type	32;	.endef
	.seh_proc	decode
decode:
	.seh_endprologue
	movl	$1, %eax
	ret
	.seh_endproc
	.comm	function_table, 800, 5
	.comm	reg, 72, 5
	.ident	"GCC: (x86_64-posix-seh-rev0, Built by MinGW-W64 project) 8.1.0"
	.def	printf;	.scl	2;	.type	32;	.endef
