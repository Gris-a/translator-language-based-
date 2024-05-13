; ===============================================
sys_exit        equ    60
sys_read        equ     0
sys_write       equ     1

stdin_fileno    equ     0
stdout_fileno   equ     1
; ===============================================





; ===============================================
global _start
global printsd
global scansd
global sqrrtsd
global exit
; ===============================================





section         .text

; ===============================================
printsd:
; -----------------------------------------------
    movsd xmm0, [rsp + 8]
; -----------------------------------------------
    xor esi, esi                                ; rsi = 0
; -----------------------------------------------
    xorpd xmm1, xmm1                            ; xmm1 = (xmm0 < 0.0)
    vcmpsd xmm1, xmm0, xmm1, 1
; ---------------------------
    movq rax, xmm1
    test eax, eax
    jz Positive
; -----------------------------------------------
Negative:
; ---------------------------
    movq rax, xmm0                              ; xmm0 = -xmm0
    mov rbx, 0x7FFFFFFFFFFFFFFF
    and rax, rbx
    movq xmm0, rax
; ---------------------------
    mov byte [buffer + rsi], '-'
    inc rsi
; ---------------------------
Positive:
; -----------------------------------------------
    xorpd xmm1, xmm1                            ; xmm1 = (xmm0 == 0.0)
    vcmpsd xmm1, xmm0, xmm1, 0
; ---------------------------
    movq rax, xmm1
    test eax, eax
    jz NonZeroValue
; -----------------------------------------------
ZeroValue:
; ---------------------------
    mov byte [buffer + rsi + 0], '0'
    mov byte [buffer + rsi + 1], '.'
    mov byte [buffer + rsi + 2], '0'
    add rsi, 3
; ---------------------------
    jmp Print
; -----------------------------------------------
NonZeroValue:
; ---------------------------
    mov rax, 0x3FF0000000000000                 ; xmm1 = (xmm0 < 1)
    movq xmm1, rax
    vcmpsd xmm1, xmm0, xmm1, 1
; ---------------------------
    movq rax, xmm1
    test eax, eax
; ---------------------------
    mov rbx, 0x4024000000000000
    mov rcx, 0x3FB999999999999A
; ---------------------------
    cmovz rbx, rcx                              ; xmm2 = (xmm0 < 1) ? 10.0 : 0.1
    movq xmm2, rbx
; ---------------------------
    mov rbx, -1
    mov rcx,  1
; ---------------------------
    cmovz rbx, rcx                              ; rbx = sign(exp(xmm0))
    xor edx, edx                                ; rdx = 0
; -----------------------------------------------
    mov rax, 0x3FF0000000000000                 ; xmm3 = 1.0
    movq xmm3, rax
; ---------------------------
    mov rax, 0x4024000000000000                 ; xmm4 = 10.0
    movq xmm4, rax
; -----------------------------------------------
DecimalNormalRepresentation:
    vcmpsd xmm1, xmm0, xmm4, 1                  ; xmm1 = (xmm0 < 10.0)
    movq rax, xmm1
; ---------------------------
    vcmpsd xmm1, xmm0, xmm3, 5                  ; xmm1 = (xmm0 >= 1.0)
    movq rcx, xmm1
; ---------------------------
    and rax, rcx                                ; 1.0 <= xmm0 < 10.0
    jnz ConversionToString
; ---------------------------
    mulsd xmm0, xmm2                            ; xmm0 -> [1.0,  10.0)
    inc rdx                                     ; rdx += 1
; ---------------------------
    jmp DecimalNormalRepresentation
; -----------------------------------------------
ConversionToString:
; ---------------------------
    xor ecx, ecx
; ---------------------------
    roundsd xmm1, xmm0, 1                       ; xmm1 = floor(xmm0)
    subsd xmm0, xmm1                            ; xmm0 = {xmm0}
; ---------------------------
    cvtsd2si rax, xmm1                          ; x <=> 0x3x
    add rax, 0x30
; ---------------------------
    mov byte [buffer + rsi + 0], al
    mov byte [buffer + rsi + 1], '.'
    add rsi, 2
; ---------------------------
ConvertDigit:
; ---------------------------
    inc rcx
; ---------------------------
    mulsd xmm0, xmm4                            ; xmm0 *= 10
; ---------------------------
    roundsd xmm1, xmm0, 1                       ; xmm1 = floor(xmm0)
    subsd xmm0, xmm1                            ; xmm0 = {xmm0}
; ---------------------------
    cvtsd2si rax, xmm1
    add rax, 0x30                               ; x <=> 0x3x
; ---------------------------
    mov byte [buffer + rsi], al
    inc rsi
; ---------------------------
    mov rax, 0x3EE4F8B588E368F1                 ; xmm1 = (xmm0 < 1.0e-5)
    movq xmm1, rax
    vcmpsd xmm1, xmm0, xmm1, 1
; ---------------------------
    movq rax, xmm1
    test eax, eax
    jnz Exponent
; ---------------------------
    cmp rcx, 5                                  ; no more than 5 digits
    je Exponent
; ---------------------------
    jmp ConvertDigit
; -----------------------------------------------
Exponent:
; ---------------------------
    test rdx, rdx
    jz Print
; ---------------------------
    mov byte [buffer + rsi], 'e'
    inc rsi
; ---------------------------
    cmp rbx, 0
    jg ExponentPositive
; -----------------------------------------------
ExponentNegative:
; ---------------------------
    mov byte [buffer + rsi], '-'
    inc rsi
; ---------------------------
ExponentPositive:
; -----------------------------------------------
    mov rax, rdx
; ---------------------------
ConvertExponent:
; ---------------------------
    xor edx, edx                        ; rdx = 0
    div qword [base10]                  ; rdx:rax / 10
; ---------------------------
    add dl, 0x30                        ; x <=> 0x3x
; ---------------------------
    mov byte [buffer + rsi], dl
    inc rsi
; ---------------------------
    test eax, eax
    je Print
; ---------------------------
    jmp ConvertExponent
; -----------------------------------------------
Print:
; ---------------------------
    mov byte [buffer + rsi], 0x0a
    inc rsi
; ---------------------------
    mov edi, stdout_fileno
    mov rdx, rsi
    mov rsi, buffer
; ---------------------------
    mov eax, sys_write
    syscall
; -----------------------------------------------
    ret
; ===============================================





; ===============================================
scansd:
; -----------------------------------------------
    xorpd xmm0, xmm0                            ; xmm0 = 0.0
    xorpd xmm1, xmm1                            ; xmm1 = 0.0
; ---------------------------
    mov rax, 0x4024000000000000                 ; xmm2 = 10.0
    movq xmm2, rax
; -----------------------------------------------
    mov edi, stdin_fileno
    mov rsi, buffer
    mov rdx, 1
; ---------------------------
ScanChar:
    mov eax, sys_read       ; read 1 char from stdin to buffer
    syscall
; ---------------------------
    cmp byte [rsi], 0x0a
    jz Sign
; ---------------------------
    inc rsi
    jmp ScanChar
; -----------------------------------------------
Sign:
; ---------------------------
    mov rcx, rsi
    sub rcx, buffer
    xor esi, esi
; ---------------------------
    cmp byte [buffer + rsi], '-'
    jnz GetPositive
; ---------------------------
GetNegative:                ; xmm4 = -1.0
    mov rax, 0xBFF0000000000000
    movq xmm4, rax
; ---------------------------
    inc rsi
    jmp GetInteger
; ---------------------------
GetPositive:                ; xmm4 = 1.0
    mov rax, 0x3FF0000000000000
    movq xmm4, rax
; -----------------------------------------------
GetInteger:
; ---------------------------
BuildInteger:
; ---------------------------
    cmp rsi, rcx            ; buffer[rsi] = '\n'
    jz End
; ---------------------------
    mov al, byte [buffer + rsi]
    inc rsi
; ---------------------------
    cmp al, '.'
    jz GetFractional
; ---------------------------
    sub al, 0x30            ; 0x3x <=> x
    cvtsi2sd xmm3, eax
; ---------------------------
    mulsd xmm0, xmm2        ; xmm0 = xmm0 * 10 + xmm3
    addsd xmm0, xmm3
; ---------------------------
    jmp BuildInteger
; -----------------------------------------------
GetFractional:
; ---------------------------
    dec rsi                 ; buffer[rsi] = '.'
    dec rcx                 ; buffer[rcx] = last digit
; ---------------------------
BuildFractional:
; ---------------------------
    cmp rsi, rcx            ; buffer[rcx] = '.'
    jz End
; ---------------------------
    mov al, byte [buffer + rcx]
    dec rcx
; ---------------------------
    sub al, 0x30            ; 03x <=> x
    cvtsi2sd xmm3, eax
; ---------------------------
    addsd xmm1, xmm3        ; xmm1 = (xmm1 + xmm3) / 10
    divsd xmm1, xmm2
; ---------------------------
    jmp BuildFractional
; -----------------------------------------------
End:
; ---------------------------
    addsd xmm0, xmm1        ; xmm0 += xmm1
    mulsd xmm0, xmm4        ; sign adj
    movq rax, xmm0
; -----------------------------------------------
    ret
; ===============================================





; ===============================================
sqrrtsd:
; -----------------------------------------------
    movsd xmm0, [rsp + 8]
    sqrtsd xmm0, xmm0
    movq rax, xmm0
; -----------------------------------------------
    ret
; ===============================================





; ===============================================
exit:
    mov eax, sys_exit
    syscall
; ===============================================





section         .data

base10  dq  10

buffer  times   512 db  0