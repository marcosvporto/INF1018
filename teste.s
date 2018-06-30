	.text					
	.globl foo			

	foo:				
	pushq %rbp			
	movq %rsp,%rbp			
	subq $16,%rsp			
	
	jmp A
	
	movl $1, -4(%rbp)
	movl -4(%rbp),%eax
	leave
	ret
	
	movl $2, -4(%rbp)
	movl -4(%rbp),%eax
	leave
	ret

	movl $10, -4(%rbp)
	cmpl $0, -4(%rbp)
	jl L1
	je L2

	movl $5, -4(%rbp)
	movl -4(%rbp),%eax
	leave
	ret
	




















