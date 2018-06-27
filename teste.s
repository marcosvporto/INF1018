/*	.text			*/
/*	.globl foo		*/

/*	foo:			*/
/*	pushq %rbp
/*	movq %rsp,%rbp		*/
/*	subq $16,%rsp		*/

movl -16(%rbp),%ecx
movl %ecx,-4(%rbp)

/*	leave			*/
/*	ret			*/



















