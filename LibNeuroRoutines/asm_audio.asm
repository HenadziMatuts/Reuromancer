.data
INCLUDE asm_seg7.asm

.code

asm_get_sample PROC
	push    rbx
	push    rdx
	push    rsi
	push    r8

	mov     r8, SEGMENT_7
	mov     eax, 0
	cmp     byte ptr [byte_26F21], 0
	jnz     loc_203F7
	mov     byte ptr [byte_26F21], 1
; ----------------------------------------------------------------
	sub     rbx, rbx
	mov     bx, word ptr [track_num]
	cmp     bx, 0
	jz      short loc_20479
	mov     word ptr [mark_01C5h], 0
	mov     byte ptr [byte_26F22], 4

	sub     rsi, rsi
	mov     si, 1ADh
	mov     word ptr [mark_000Ah], si
	mov     si, 2Dh ; '-'
	mov     word ptr [mark_0008h], si

loc_20438:
	cmp     word ptr [r8 + rsi], 0
	jz      loc_20456
	call    sub_20482
	cmp     word ptr [mark_01C5h], 0
	jnz     loc_20456
	cmp     word ptr [r8 + rsi + 0Ah], 0
	jz      loc_20456
	cmp     word ptr [r8 + rsi], 0
	jz      loc_20456
	mov     word ptr [mark_01C5h], si

loc_20456:
	add     si, 30h ; '0'
	dec     byte ptr [byte_26F22]
	jnz     loc_20438
	mov     bx, word ptr [mark_01C5h]
	cmp     bx, 0
	jz      short loc_20479
	mov     si, bx
	mov     eax, dword ptr [r8 + rsi + 8]
	jmp     loc_203F7

loc_20479:
	mov     eax, 0
; ----------------------------------------------------------------
loc_203F7:
	mov     byte ptr [byte_26F21], 0
	pop     r8
	pop     rsi
	pop     rdx
	pop     rbx
	ret
asm_get_sample ENDP

sub_20482 PROC
	push    rcx
	push    rdi
	mov     ax, word ptr [r8 + rsi + 0Ch]
	add     word ptr [r8 + rsi + 0Ah], ax
	mov     ax, word ptr [r8 + rsi + 6]
	add     word ptr [r8 + rsi + 4], ax
	sub     rdx, rdx
	mov     ax, word ptr [r8 + rsi + 1Eh]
	add     ax, word ptr [r8 + rsi + 20h]
	jz      loc_204B5
	cmp     ax, word ptr [r8 + rsi + 24h]
	jb      loc_204A2
	sub     ax, word ptr [r8 + rsi + 24h]

loc_204A2:
	mov     word ptr [r8 + rsi + 1Eh], ax
	sub     rcx, rcx
	mov     cl, 4
	sar     ax, cl
	add     ax, word ptr [r8 + rsi + 1Ch]
	sub     rdi, rdi
	mov     di, ax
	add     rdi, r8
	mov     ah, byte ptr [rdi]
	sub     rdi, r8
	mov     al, 0
	imul    word ptr [r8 + rsi + 22h]

loc_204B5:
	add     dx, word ptr [r8 + rsi + 4]
	mov     word ptr [r8 + rsi + 8], dx
	cmp     word ptr [r8 + rsi + 14h], 0
	jz      loc_204D0
	dec     word ptr [r8 + rsi + 14h]
	jnz     loc_204D0
	mov     word ptr [r8 + rsi + 18h], 10h
	mov     word ptr [r8 + rsi + 1Ah], 1

loc_204D0:
	dec     word ptr [r8 + rsi]
	jnz     loc_204D7
	call    sub_20513

loc_204D7:
	cmp     word ptr [r8 + rsi + 1Ah], 0
	jz      loc_20510
	dec     word ptr [r8 + rsi + 1Ah]
	jnz     loc_20510

loc_204E2:
	mov     bx, word ptr [r8 + rsi + 16h]
	add     bx, word ptr [r8 + rsi + 18h]
	cmp     word ptr [r8 + rbx + 2], 0FFFFh
	jnz     loc_20501
	mov     ax, word ptr [r8 + rbx]
	mov     word ptr [r8 + rsi + 0Ah], ax
	cmp     ax, 0
	jnz     loc_204FB
	mov     word ptr [r8 + rsi + 0Ch], ax

loc_204FB:
	add     word ptr [r8 + rsi + 18h], 4
	jmp     loc_204E2

loc_20501:
	mov     ax, word ptr [r8 + rbx]
	mov     word ptr [r8 + rsi + 0Ch], ax
	mov     ax, word ptr [r8 + rbx + 2]
	mov     word ptr [r8 + rsi + 1Ah], ax
	add     word ptr [r8 + rsi + 18h], 4
loc_20510:
	pop     rdi
	pop     rcx
	ret
sub_20482 ENDP

sub_20513 PROC
	sub     rdi, rdi
	mov     di, word ptr [r8 + rsi + 2]
	cmp     di, 0
	jnz     loc_2051C
	ret

loc_2051C:
	push    r9
	mov     word ptr [mark_0006h], si

loc_20520:
	add     rdi, r8
	mov     bl, byte ptr [rdi]
	sub     rdi, r8
	inc     di
	cmp     bl, 0FAh ; 'ú'
	jnb     loc_2052B
	jmp     loc_205BF

loc_2052B:
	sub     bh, bh
	sub     bl, 0FAh ; 'ú'
	add     bl, bl

	mov     ax, word ptr [r8 + rbx + 21h]
	cmp     ax, 01DDh
	je      m0
	jmp     m1

	add     rdi, r8
	mov     bl, byte ptr [rdi]
	sub     rdi, r8
	inc     di
	mov     bh, 0
	mov     ax, word ptr [r8 + rdi]
	add     di, 2

	mov     r9, r8
	add     r9, rbx
	add     r9, rsi
	cmp     word ptr [r9], 0
	jz      loc_20549
	dec     word ptr [r9]
	jz      loc_20520

loc_20549:
	mov     di, ax
	jmp     loc_20520
m0:
	mov     si, word ptr [r8 + rdi]
	add     di, 2
	add     si, word ptr [mark_0008h]
	sub     rbx, rbx
	mov     word ptr [r8 + rsi + 4], bx
	mov     word ptr [r8 + rsi + 6], bx
	mov     word ptr [r8 + rsi + 8], bx
	mov     word ptr [r8 + rsi + 0Ah], bx
	mov     word ptr [r8 + rsi + 0Ch], bx
	mov     word ptr [r8 + rsi + 10h], bx
	mov     word ptr [r8 + rsi + 12h], bx
	mov     word ptr [r8 + rsi + 16h], bx
	mov     word ptr [r8 + rsi + 18h], bx
	mov     word ptr [r8 + rsi + 1Ah], bx
	mov     word ptr [r8 + rsi + 1Ch], bx
	mov     word ptr [r8 + rsi + 1Eh], bx
	mov     word ptr [r8 + rsi + 20h], bx
	mov     word ptr [r8 + rsi + 22h], bx
	mov     word ptr [r8 + rsi + 24h], bx
	jmp     loc_20520
m1:
	add     rdi, r8
	mov     bl, byte ptr [rdi]
	sub     rdi, r8
	inc     di
	mov     bh, 0
	mov     ax, word ptr [r8 + rdi]
	inc     di
	inc     di
	add     r8, rsi
	mov     word ptr [r8 + rbx], ax
	sub     r8, rsi
	cmp     bl, 0
	jnz     loc_20520

loc_20597:
	mov     si, word ptr [mark_0006h]
	cmp     word ptr [r8 + rsi], 0
	jnz     loc_205A3
	mov     di, 0

loc_205A3:
	mov     word ptr [r8 + rsi + 2], di
	pop     r9
	ret

	mov     di, word ptr [mark_000Ch]
	jmp     loc_20520

	mov     ax, word ptr [r8 + rdi]
	add     di, 2
	mov     word ptr [mark_000Ch], di
	mov     di, ax
	jmp     loc_20520

loc_205BC:
	add     rdi, r8
	mov     bl, byte ptr [rdi]
	sub     rdi, r8
	inc     di

loc_205BF:
	push    rdi
	mov     al, bl
	mov     cl, 5
	shr     al, cl
	mul     byte ptr [mark_0003h]
	add     ax, word ptr [mark_0008h]
	mov     di, ax
	and     bx, 1Fh
	mov     ax, word ptr [r8 + rsi + 0Eh]
	cmp     al, 0
	jnz     loc_205DD
	inc     al

loc_205DD:
	mul     byte ptr [r8 + rbx + 0Eh]
	mov     word ptr [r8 + rsi], ax
	pop     rbx
	push    rbx
	add     rbx, r8
	mov     al, byte ptr [rbx]
	sub     rbx, r8
	and     ax, 7Fh
	cmp     ax, 7Fh
	jz      loc_20625
	mov     cx, word ptr [r8 + rsi]
	mov     word ptr [r8 + rdi], cx
	sub     cx, [r8 + rdi + 10h]
	mov     word ptr [r8 + rdi + 14h], cx
	add     ax, word ptr [r8 + rdi + 12h]
	mov     word ptr [r8 + rdi + 18h], 0
	mov     word ptr [r8 + rdi + 1Ah], 1
	mov     cx, 0FFh

loc_20609:
	inc     cl
	sub     ax, 0Ch
	jnb     loc_20609
	add     ax, 0Ch
	mov     bx, ax
	add     bx, bx
	add     bx, word ptr [mark_000Ah]
	mov     ax, word ptr [r8 + rbx]
	shr     ax, cl
	mov     word ptr [r8 + rdi + 4], ax
	mov     word ptr [r8 + rdi + 8], ax

loc_20625:
	pop     rdi
	add     rdi, r8
	mov     al, byte ptr [rdi]
	sub     rdi, r8
	inc     di
	and     al, 80h
	jz      loc_2062F
	jmp     loc_205BC

loc_2062F:
	jmp     loc_20597
sub_20513 ENDP

asm_set_track_on_playback PROC ; asm_set_track_on_playback(int track_num)
	push    rax
	push    rbx
	push    rdi
	push    rsi
	push    r8

	mov     r8, SEGMENT_7 ; instead of ds
	sub     rsi, rsi
	mov     rsi, rcx ; rsi <- track_num
	mov     word ptr [track_num], si

	shl     si, 3
	mov     byte ptr [byte_26F21], 1
	mov     byte ptr [byte_26F22], 4
	sub     rdi, rdi
	mov     di, 2Dh ; '-'

loc_20659:
	sub     rax, rax
	mov     ax, word ptr [r8 + rsi + 1D0h]
	cmp     ax, 0
	jz      loc_20675
	sub     rbx, rbx
	mov     bx, 2Eh ; '.'

loc_20665:
	add     r8, rdi
	mov     word ptr [r8 + rbx], 0
	sub     r8, rdi
	sub     bx, 2
	jge     short loc_20665
	mov     word ptr [r8 + rdi + 2], ax
	mov     word ptr [r8 + rdi], 1

loc_20675:
	add     di, 30h ; '0'
	add     si, 2
	dec     byte ptr [byte_26F22]
	jnz     short loc_20659
	mov     byte ptr [byte_26F21], 0

	pop     r8
	pop     rsi
	pop     rdi
	pop     rbx
	pop     rax
	ret
asm_set_track_on_playback ENDP


END