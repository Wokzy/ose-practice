[BITS 16]

cli

; enable sse
; mov eax, cr0
; and ax, 0xFFFB
; or ax, 0x2
; mov cr0, eax
; mov eax, cr4
; or ax, 3 << 9
; mov cr4, eax

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
mov ds, ax
mov ss, ax
mov es, ax
mov fs, ax
mov gs, ax

; mov ax, 0x10 ; index = 2, ti = 0, pl = 0
mov eax, TSS
mov word [tss_descriptor + 2], ax
shr eax, 16
mov byte [tss_descriptor + 4], al
mov byte [tss_descriptor + 7], ah
mov ax, 0x28
ltr ax

mov ax, 0x10


[EXTERN kernel_entry]
; sti
call kernel_entry

gdt_descriptor:
  dw 0x2f
  dd gdt

; check sgtd in memory
align 0x8
gdt:
  dq 0x0000
  dq 0xcf92000000ffff ; 0b0000000011001111100100100000000000000000000000001111111111111111 data
  dq 0xcf9a000000ffff ; 0b0000000011001111100110100000000000000000000000001111111111111111 code
  dq 0xcff2000000ffff ; 0b0000000011001111111100100000000000000000000000001111111111111111 user data
  dq 0xcffa000000ffff ; 0b0000000011001111111110100000000000000000000000001111111111111111 user code

  tss_descriptor:
  dq 0x0089000000006b


TSS:
  .previous_task_link: dd 0
  .esp0:               dd 0x7c00
  .ss0:                dw 0x8
  .reserved0:          dw 0
  .esp1:               dd 0
  .ss1:                dw 0
  .reserved1:          dw 0
  .esp2:               dd 0
  .ss2:                dw 0
  .reserved2:          dw 0
  .cr3:                dd 0
  .eip:                dd 0
  .eflags:             dd 0
  .eax:                dd 0
  .ecx:                dd 0
  .edx:                dd 0
  .ebx:                dd 0
  .esp:                dd 0
  .ebp:                dd 0
  .esi:                dd 0
  .edi:                dd 0
  .es:                 dw 0
  .reserved3:          dw 0
  .cs:                 dw 0
  .reserved4:          dw 0
  .ss:                 dw 0
  .reserved5:          dw 0
  .ds:                 dw 0
  .reserved6:          dw 0
  .fs:                 dw 0
  .reserved7:          dw 0
  .gs:                 dw 0
  .reserved8:          dw 0
  .ldt_selector:       dw 0
  .reserved9:          dw 0
  .debug_trap:         dw 0
  .io_map_base:        dw 108
  ;.tss_size:           dw 108
  .ssp:                dd 0

[BITS 32]
[GLOBAL sys_cpu_halt]
sys_cpu_halt:
  cli
  hlt

[GLOBAL sys_infinite_loop]
sys_infinite_loop:
  jmp sys_infinite_loop


[GLOBAL sys_read_eflags]
sys_read_eflags:
  pushfd
  pop eax
  ret


[GLOBAL sys_read_from_port]
sys_read_from_port:
  mov dx, word [esp + 4]
  in al, dx
  ret

[GLOBAL sys_write_to_port]
sys_write_to_port: ; void sys_write_to_port (uint16_t port, uint8_t byte)
  mov dx, word [esp + 4]
  mov al, byte [esp + 8]
  out dx, al
  ret


[GLOBAL load_interrupt_descrtiptors_table]
load_interrupt_descrtiptors_table:
  mov eax, dword [esp + 4]
  lidt [eax]
  ret

[GLOBAL sys_fake_syscall]
sys_fake_syscall:
  mov eax, 1
  mov ebx, 2
  mov edx, 3
  mov ecx, 4
  mov edi, 5
  mov esi, 6
  mov ebp, 10
  int 0x80
  ret

[GLOBAL sys_zero_div]
sys_zero_div:
  xor eax, eax
  idiv eax

[GLOBAL interrupts_collect_context]
[EXTERN interrupts_interrupt_fowarder]
interrupts_collect_context:
  cld

  push ds
  push es
  push fs
  push gs

  ; movdqu xmm0, [pooo]
  ; movdqu [esp - 16], xmm0
  ; movdqu [esp - 32], xmm1
  ; movdqu [esp - 48], xmm2
  ; movdqu [esp - 64], xmm3
  ; movdqu [esp - 80], xmm4
  ; movdqu [esp - 96], xmm5
  ; movdqu [esp - 112], xmm6
  ; movdqu [esp - 128], xmm7
  ; sub esp, 128
  pusha

  mov ax, 0x8
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  mov ebx, esp

  ; выравниваем стек так, чтобы (sp - 4) mod 32 = 0
  mov ecx, esp
  mov eax, 28
  and ecx, 0x1f
  add eax, ecx
  sub esp, eax

  push ebx
  call interrupts_interrupt_fowarder
  mov esp, ebx
  popa

  ; movdqu xmm7, [esp]
  ; movdqu xmm6, [esp + 16]
  ; movdqu xmm5, [esp + 32]
  ; movdqu xmm4, [esp + 48]
  ; movdqu xmm3, [esp + 64]
  ; movdqu xmm2, [esp + 80]
  ; movdqu xmm1, [esp + 96]
  ; movdqu xmm0, [esp + 112]

  ; add esp, 128

  pop gs
  pop fs
  pop es
  pop ds
  add esp, 8 ; vector index + error code
  iretd

[GLOBAL sys_jump_to_userspace]
sys_jump_to_userspace:
  mov esp, dword [esp + 4]
  popa
  pop gs
  pop fs
  pop es
  pop ds
  add esp, 8 ; vector index + error code

  iretd

; pmemsave 0x7C00 65536 res.bin

pooo:
 dd 0x11111111, 0x22222222, 0x33333333, 0x44444444
 ; dq 0xffffffffffffffff, 0x1


times 510-($-$$) db 0
dw 0xAA55
