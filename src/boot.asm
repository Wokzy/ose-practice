[BITS 16]

cli
mov ax, 0x7C0
mov ss, ax
mov ds, ax
xor sp, sp
; sti

mov di, 0x7E0
mov es, di
xor bx, bx

mov ax, 0x0201
mov cx, 0x02
mov dh, 0x0
int 0x13

mov si, KERNEL_SIZE

read_loop:
  dec si
  add di, 0x20
  mov es, di
  test si, si
  jz halt

  inc cl
  cmp cl, 19
  jne read

  mov cl, 1
  inc dh

  cmp dh, 0x2
  jne read

  xor dh, dh
  inc ch

  read:
  clc
  mov ax, 0x0201
  int 0x13

  jc read
  jmp read_loop

halt:
hlt

; pmemsave 0x7C00 65536 res.bin


times 510-($-$$) db 0
dw 0xAA55
