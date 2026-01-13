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


CYLINDERS_LIMIT equ 79
HEADS_LIMIT equ 1
SECTORS_PER_TRACK_LIMIT equ 36
KERNEL_STACK equ 0x7C00

[BITS 16]

xor cx, cx
mov ds, cx
mov ss, cx
mov sp, KERNEL_STACK

xor bx, bx
mov ax, 0x7E0
mov es, ax

mov al, 1
; ch == 0
mov cl, 2
xor dh, dh  

mov di, KERNEL_SIZE

.read:

  dec di
  jz .end_read_success

  mov ah, 2
  int 0x13
  ; jc read_error

  mov si, es
  add si, 0x20
  mov es, si

  inc cl
  cmp cl, SECTORS_PER_TRACK_LIMIT
  jbe .read

  mov cl, 1
  xor dh, 1
  jnz .read

  inc ch
  jmp .read

.end_read_success:

.end_read:

boot:
cld
lgdt [gdt_descriptor]

mov eax, cr0
or eax, 1
mov cr0, eax

jmp sys_code_selector:next

[BITS 32]
next:
mov ax, sys_data_selector ; index = 1, ti = 0, pl = 0
mov ds, ax
mov ss, ax
mov es, ax
mov fs, ax
mov gs, ax

mov ax, tss_selector
ltr ax

mov ax, sys_code_selector


[EXTERN kernel_entry]
call kernel_entry

gdt_descriptor:
  dw 0x2f
  dd gdt

; check sgtd in memory
align 0x8
gdt:
  sys_data_selector equ 0x08
  sys_code_selector equ 0x10
  tss_selector      equ 0x28
  dq 0x0000
  dq 0xcf92000000ffff ; 0b0000000011001111100100100000000000000000000000001111111111111111 data
  dq 0xcf9a000000ffff ; 0b0000000011001111100110100000000000000000000000001111111111111111 code
  dq 0xcff2000000ffff ; 0b0000000011001111111100100000000000000000000000001111111111111111 user data
  dq 0xcffa000000ffff ; 0b0000000011001111111110100000000000000000000000001111111111111111 user code

  tss_descriptor:
  ; dq 0x0089000000006b
  dw 0x0067
  dw TSS
  dd 0x00008900


TSS:
  .previous_task_link: dd 0
  .esp0:               dd 0x7c00
  .ss0:                dw sys_data_selector
  ; times 96 db 0


[BITS 32]
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

[GLOBAL sys_break_gdt]
sys_break_gdt:
  mov dword [gdt + 16], 0
  mov dword [gdt + 20], 0
  ret

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

  mov ax, sys_data_selector
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

  __restore_context:
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
  jmp __restore_context




; pmemsave 0x7C00 65536 res.bin
; pmemsave 0x0 300000 res.bin

pooo:
 dd 0x11111111, 0x22222222, 0x33333333, 0x44444444
 ; dq 0xffffffffffffffff, 0x1


times 510-($-$$) db 0
dw 0xAA55
