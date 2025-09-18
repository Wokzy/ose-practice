[BITS 16]
[ORG 0x0]

cli
mov ax, 0x7C0
mov ss, ax
xor sp, sp
; sti

mov ax, 0x7E0
mov es, ax
xor bx, bx

mov ax, 0x0201
mov cx, 0x02
mov dh, 0x0
int 0x13
; lp:
;   jmp lp

hlt

; pmemsave 0x7C00 N+512 res.bin


times 510-($-$$) db 0
dw 0xAA55
