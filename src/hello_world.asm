[BITS 16]
[ORG 0x7C00]

cli
xor sp, sp
mov ss, sp
mov sp, msg
mov cl, 13

print_loop:
  pop ax
  int 0x10
  loop print_loop

hlt

times 450 db 0

msg: db 'H', 0x0E, 'e', 0x0E, 'l', 0x0E, 'l', 0x0E, 'o', 0x0E, ',', 0x0E, ' ', 0x0E, 'W', 0x0E, 'o', 0x0E, 'r', 0x0E, 'l', 0x0E, 'd', 0x0E, '!', 0x0E


; cli 21 byte
; mov ax, 0x7c0
; mov si, msg
; mov ss, ax
; xor sp, sp
; mov ah, 0x0E
; mov cl, 13

; print_loop:
;   ss lodsb
;   int 0x10
;   loop print_loop

; hlt

; msg2: db 0, '!dlroW ,olle'
; msg: db 'Hello, World!', 0

times 510-($-$$) db 0
dw 0xAA55

; положить секретку в msg
