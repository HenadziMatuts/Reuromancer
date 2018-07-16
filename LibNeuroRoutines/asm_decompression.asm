.data

    imh_stream  EQU 0h
    imh_a       EQU 08h
    imh_b       EQU 0Ah
    imh_c       EQU 0Ch
    imh_d       EQU 014h
    imh_e       EQU 016h
    imh_area_1  EQU 018h
    imh_area_2  EQU 0418h
    imh_area_3  EQU 0818h
    imh_end     EQU 01418h

imh_decomp_addr:
    dq          0
imh_stream_addr:
    dq          0
imh_decomp_dst_addr:
    dq          0

table_a:
    db      0
    db      80h
    db      0C0h
    db      0E0h
    db      0F0h
    db      0F8h
    db      0FCh
    db      0FEh
    db      0FFh
table_b:
    db      0
    dw      0
    dw      0
    dw      0
    dw      0
    db      0
    db      0

.code

; int asm_decompress (imh_decomp_t *imh, uint8_t *dst)
asm_decompress PROC
    push    rsi
    push    rdi
    push    rbx

    mov     qword ptr [imh_decomp_addr], rcx
    mov     rax, [rcx + imh_stream]
    mov     qword ptr [imh_stream_addr], rax
    mov     qword ptr [imh_decomp_dst_addr], rdx

    sub     rax, rax
    mov     rsi, qword ptr [imh_stream_addr]
    lodsw
    mov     word ptr [rcx + imh_d], ax
    lodsw
    mov     word ptr [rcx + imh_e], ax
    lodsw

    xchg    al, ah
    sub     rcx, rcx
    mov     cl, 10h
    sub     rbx, rbx
    sub     rdx, rdx

    call    sub_1ff94

    push    r8
    mov     r8, qword ptr [imh_decomp_addr]
    mov     qword ptr [r8 + imh_c], rsi
    mov     word ptr [r8 + imh_a], ax
    mov     byte ptr [r8 + imh_b], cl
    pop     r8

    call    sub_1fff2

    call    sub_200b1

    pop     rbx
    pop     rdi
    pop     rsi
    ret
asm_decompress ENDP

;--------------------------------------------------------;

sub_1ff94 PROC
    call    sub_1ffd0
    jb      m0
    shl     dx, 1
    inc     bx
    call    sub_1ff94
    or      dl, 1
    call    sub_1ff94
    shr     dx, 1
    dec     bx
    ret

m0:
    sub     rdi, rdi
    mov     ch, 8

m1:
    call    sub_1ffd0
    rcl     rdi, 1
    dec     ch
    jnz     m1

    add     rdi, rdi
    add     rdi, rdi
    add     rdi, qword ptr [imh_decomp_addr]
    mov     word ptr [rdi + imh_area_1], dx
    mov     word ptr [rdi + imh_area_1 + 2], bx

    ret
sub_1ff94 ENDP

;--------------------------------------------------------;

sub_1ffd0 PROC
    or      cl, cl
    jz      m0
    dec     cl
    shl     ax, 1
    ret

m0:
    lodsw
    xchg    al, ah
    mov     cl, 0Fh
    shl     ax, 1
    ret
sub_1ffd0 ENDP

;--------------------------------------------------------;

sub_1fff2 PROC
    push    rbp
    mov     rbx, qword ptr [imh_decomp_addr]
    mov     rbp, rbx
    add     rbx, imh_area_1
    add     rbp, imh_area_3

    sub     rax, rax
    mov     cx, 8

m0:
    mov     al, [rbx + 2]
    or      al, al
    jz      m2
    cmp     al, cl
    jg      m3
    mov     dl, al
    sub     cl, al
    add     cl, 2
    sub     rax, rax
    mov     al, [rbx]
    shl     ax, cl
    mov     rdi, qword ptr [imh_decomp_addr]
    add     rdi, imh_area_2
    add     rdi, rax
    mov     byte ptr [rdi], dl
    mov     byte ptr [rdi + 1], ch
    mov     word ptr [rdi + 2], 0
    mov     dl, ch
    sub     ch, ch
    sub     cl, 2
    mov     rax, 1
    shl     ax, cl
    mov     cx, ax
    dec     cx
    jz      m1
    mov     rsi, rdi
    add     rdi, 4
    shl     cx, 1
    rep movsw

m1:
    mov     ch, dl
    mov     cl, 8

m2:
    add     rbx, 4
    inc     ch
    cmp     ch, 0
    jnz     m0
    pop     rbp
    ret

m3:
    push    rbx
    push    rcx
    mov     dx, word ptr [rbx]
    mov     cl, 10h
    sub     cl, al
    shl     dx, cl
    mov     byte ptr [table_b], dh
    sub     rax, rax
    mov     al, dh
    shl     ax, 1
    shl     ax, 1
    mov     rdi, qword ptr [imh_decomp_addr]
    add     rdi, imh_area_2
    add     rdi, rax
    mov     word ptr [rdi], 0
    mov     word ptr [rdi + 2], bp

m4:
    mov     al, byte ptr [rbx + 2]
    cmp     al, 8
    jle     m5
    mov     dx, word ptr [rbx]
    mov     cl, 10h
    sub     cl, al
    shl     dx, cl
    cmp     dh, byte ptr [table_b]
    jnz     m5
    mov     rdi, rbp
    mov     byte ptr [rdi + 1], dl
    mov     byte ptr [rdi + 2], ch
    sub     al, 8
    sub     ah, ah
    mov     byte ptr [rdi + 3], al
    mov     rsi, table_a
    add     rsi, rax
    mov     al, byte ptr [rsi]
    mov     byte ptr [rdi], al
    mov     word ptr [rbx + 2], 0
    add     rbp, 4

m5:
    add     rbx, 4
    inc     ch
    jnz     m4
    pop     rcx
    pop     rbx
    jmp     m0

sub_1fff2 ENDP

;--------------------------------------------------------;

sub_200b1 PROC
    push    rsi
    push    rdi
    push    r8

    sub     rax, rax
    sub     rdx, rdx
    mov     ax, 0FFFFh
    mov     dx, 0
    mov     r8, qword ptr [imh_decomp_addr]

m0:
    cmp     ax, word ptr [r8 + imh_d]
    jbe     m1
    cmp     dx, word ptr [r8 + imh_e]
    jb      m1
    mov     ax, word ptr [r8 + imh_d]
    mov     dx, word ptr [r8 + imh_e]

m1:
    sub     word ptr [r8 + imh_e], dx
    sbb     word ptr [r8 + imh_d], ax
    mov     word ptr [table_b + 3], ax
    mov     word ptr [table_b + 1], ax
    mov     rdi, qword ptr [imh_decomp_dst_addr]
    mov     rsi, qword ptr [r8 + imh_c]

    call    sub_202cc
    jmp     m4

m2:
    mov     qword ptr [r8 + imh_c], rsi
    mov     word ptr [r8 + imh_a], dx
    mov     cl, ch
    sub     ch, ch
    mov     word ptr [r8 + imh_b], cx
    pop     r8
    pop     rdi
    pop     rsi
    sub     dx, dx
    mov     ax, word ptr [table_b + 3]
    ret

m3:
    dec     word ptr [table_b + 1]
    jz      m2

m4:
    sub     rbx, rbx
    mov     bl, dh
    shl     bx, 1
    shl     bx, 1
    mov     ax, word ptr [r8 + imh_area_2 + rbx]
    or      al, al
    jz      m7
    xchg    al, ah
    stosb

m5:
    mov     cl, ah
    rol     dx, cl
    sub     ch, cl

m6:
    cmp     ch, 8
    jge     m3
    mov     cl, ch
    rol     dx, cl
    lodsb
    mov     dh, al
    ror     dx, cl
    add     ch, 8
    jmp     m3

m7:
    mov     dh, dl
    mov     cl, ch
    sub     cl, 8
    rol     dx, cl
    lodsb
    jmp     m8

m8:
    mov     dh, al
    ror     dx, cl

    mov     rbx, [r8 + imh_area_2 + rbx + 2]
    and     rbx, 0000FFFFh
    push    r9
    mov     r9, r8
    mov     r9w, bx
    mov     rbx, r9
    pop     r9

m9:
    mov     ax, word ptr [rbx]
    and     al, dh
    cmp     al, ah
    jz      m10
    add     bx, 4
    jmp     m9

m10:
    mov     ax, word ptr [rbx + 2]
    stosb

m11:
    mov     cl, ah
    rol     dx, cl
    sub     ch, ah
    jmp     m6
sub_200b1 ENDP

;--------------------------------------------------------;

sub_202cc PROC
    mov     dx, word ptr [r8 + imh_a]
    mov     cx, word ptr [r8 + imh_b]
    cmp     cl, 8
    jg      m1
    rol     dx, cl

    mov     rsi, qword ptr [r8 + imh_c]
    lodsb

    mov     dh, al
    ror     dx, cl
    add     cl, 8

m1:
    mov     ch, cl
    ret
sub_202cc ENDP

END
