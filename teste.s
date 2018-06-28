/*	.text				*/		
/*	.globl foo			*/

/*	foo:				*/
/*	pushq %rbp			*/
/*	movq %rsp,%rbp			*/
/*	subq $16,%rsp			*/
	
/*	cmpl $0,%esi
/*	je L2
/*	cmpl $0,%esi
/*	jg L3
/*	jmp L1
/*	L3:
/*	movl $1234, -12(%rbp)		*/
/*	jmp A				*/
/*	ret				*/
/*	A:				*/
/*	jmp B				*/
/*	C:				*/
/*	movl $364,-12(%rbp)		*/
/*	movl -12(%rbp),%eax		*/
/*	leave				*/
/*	ret				*/
/*	B:				*/
/*	movl -12(%rbp),%ecx		*/
/*	movl %ecx,-16(%rbp)		*/
/*	jmp D				*/
/*	ret				*/
/*	D:				*/
/*	jmp C				*/
/*	movl -16(%rbp),%eax		*/
/*	leave				*/
/*	ret				*/



cmpl $0,-8(%rbp)
je L1 
cmpl $0,-8(%rbp)
jg L2
jmp L3
movl $0xffffffff,-16(%rbp) /*1*/
movl $0xffffffff,-16(%rbp) /*2*/
movl $0xffffffff,-16(%rbp) /*3*/
movl $0xffffffff,-16(%rbp) /*4*/
movl $0xffffffff,-16(%rbp) /*5*/
movl $0xffffffff,-16(%rbp) /*6*/
movl $0xffffffff,-16(%rbp) /*7*/
movl $0xffffffff,-16(%rbp) /*8*/
movl $0xffffffff,-16(%rbp) /*9*/
movl $0xffffffff,-16(%rbp) /*10*/
movl $0xffffffff,-16(%rbp) /*11*/
movl $0xffffffff,-16(%rbp) /*12*/
movl $0xffffffff,-16(%rbp) /*13*/
movl $0xffffffff,-16(%rbp) /*14*/
movl $0xffffffff,-16(%rbp) /*15*/
movl $0xffffffff,-16(%rbp) /*16*/
movl $0xffffffff,-16(%rbp) /*17*/
movl $0xffffffff,-16(%rbp) /*18*/
movl $0xffffffff,-16(%rbp) /*19*/
movl $0xffffffff,-16(%rbp) /*20*/
movl $0xffffffff,-16(%rbp) /*21*/
movl $0xffffffff,-16(%rbp) /*22*/
movl $0xffffffff,-16(%rbp) /*23*/
movl $0xffffffff,-16(%rbp) /*24*/
movl $0xffffffff,-16(%rbp) /*25*/
movl $0xffffffff,-16(%rbp) /*26*/
L2:
L3:
L1:

















