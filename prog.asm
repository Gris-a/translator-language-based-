extern exit
extern printsd
extern scansd
extern sqrrtsd

section .text
global _start

_start:
	call main

	xor edi, edi
	call exit

factorial:
	push rbp
	mov rbp, rsp
	sub rsp, 0

IF0:
	mov rax, 4611686018427387904
	push rax
	push qword [rbp + 16]

	movsd xmm0, [rsp]
	add rsp, 8
	movsd xmm1, [rsp]
	cmpsd xmm0, xmm1, 1

	movsd [rsp], xmm0


	pop rax
	test rax, rax
	jz near ELSE2

	mov rax, 4607182418800017408
	push rax

	pop rax
	mov rsp, rbp
	pop rbp
	ret


	jmp near IF_END1

ELSE2:

IF_END1:
	push qword [rbp + 16]

	mov rax, 4607182418800017408
	push rax
	push qword [rbp + 16]

	movsd xmm0, [rsp]
	add rsp, 8
	movsd xmm1, [rsp]
	subsd xmm0, xmm1

	movsd [rsp], xmm0


	call factorial

	add rsp, 8
	push rax


	movsd xmm0, [rsp]
	add rsp, 8
	movsd xmm1, [rsp]
	mulsd xmm0, xmm1

	movsd [rsp], xmm0


	pop rax
	mov rsp, rbp
	pop rbp
	ret

	mov rsp, rbp
	pop rbp
	ret
main:
	push rbp
	mov rbp, rsp
	sub rsp, 8


	call scansd

	push rax

	movsd xmm0, [rsp]
	movsd [rbp - 8], xmm0

IF3:
	mov rax, 0
	push rax
	push qword [rbp - 8]

	movsd xmm0, [rsp]
	add rsp, 8
	movsd xmm1, [rsp]
	cmpsd xmm0, xmm1, 1

	movsd [rsp], xmm0


	pop rax
	test rax, rax
	jz near ELSE5

	mov rax, 4607182418800017408
	push rax
	mov rax, 0
	push rax

	movsd xmm0, [rsp]
	add rsp, 8
	movsd xmm1, [rsp]
	subsd xmm0, xmm1

	movsd [rsp], xmm0

	movsd xmm0, [rsp]
	movsd [rbp - 8], xmm0

	jmp near IF_END4

ELSE5:

	push qword [rbp - 8]

	call factorial

	add rsp, 8
	push rax

	movsd xmm0, [rsp]
	movsd [rbp - 8], xmm0

IF_END4:

	push qword [rbp - 8]

	call printsd

	add rsp, 8
	push rax

	mov rsp, rbp
	pop rbp
	ret
