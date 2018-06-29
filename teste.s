	.text				/*aaa*/		
	.globl foo			/*a*/

	foo:				/*a*/
	pushq %rbp			/*a*/
	movq %rsp,%rbp			/*a*/
	subq $16,%rsp			/*a*/
	
	movl $5,-4(%rbp)
	cmpl $0,-4(%rbp)
	jl L2
	je L3
	L2:
	movl $67,-12(%rbp)
	movl -12(%rbp),%ecx
	movl %ecx,-8(%rbp)
	L4:
	movl -8(%rbp),%eax
	leave
	ret
	L3:
	movl -4(%rbp),%ecx
	movl %ecx,-8(%rbp)
	jmp L4 
	




















