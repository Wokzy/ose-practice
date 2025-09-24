[BITS 16]

cli
xor sp, sp
mov ss, sp
mov ds, sp
; mov ax, 0x7C0
; mov ds, ax
mov ax, 0x7c00
mov sp, ax

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
  jz boot

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

boot:
cld
lgdt [gdt_descriptor]

mov eax, cr0
or eax, 1
mov cr0, eax

jmp 0x10:next

[BITS 32]
next:
mov ax, 0x8 ; index = 1, ti = 0, pl = 0
mov ds, ax ; out =(
mov ss, ax
mov es, ax
mov fs, ax
mov gs, ax

mov ax, 0x10 ; index = 2, ti = 0, pl = 0


[EXTERN kernel_entry]
call 0x10:kernel_entry

gdt_descriptor:
  dw 0x17
  dd gdt

; check sgtd in memory
align 0x8
gdt:
  dq 0x0000
  dq 0xcf92000000ffff ; 0b0000000001001001111100110000000000000000000000001111111111111111
  dq 0xcf9e000000ffff ; 0b0000000001111001111100110000000000000000000000001111111111111111

[BITS 32]
[GLOBAL cpu_halt]
cpu_halt:
  jmp cpu_halt



; pmemsave 0x7C00 65536 res.bin


times 510-($-$$) db 0
dw 0xAA55
