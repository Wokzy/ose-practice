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



[GLOBAL sys_set_pde]
sys_set_pde:
  mov eax, dword [esp + 4]
  mov cr3, eax

  ret

[GLOBAL sys_enable_paging]
sys_enable_paging:
  mov eax, cr4
  or eax, 0x10
  mov cr4, eax ; PSE

  mov eax, cr0
  mov edx, 1
  shl edx, 31
  or eax, edx
  mov cr0, eax
  ret

[GLOBAL sys_disable_paging]
sys_disable_paging:
  mov eax, cr0
  mov edx, -1
  shr edx, 1
  and eax, edx
  mov cr0, eax
  ret

[GLOBAL sys_exit]
sys_exit:
  mov eax, 0
  mov ecx, dword [esp + 4]
  int 0x80


[GLOBAL sys_n_rec]
sys_n_rec:
  mov eax, [esp + 4]
  cmp eax, 0
  mov ebx, esp
  jz to_ret
    sub esp, 4088
    sub eax, 1
    push eax
    call sys_n_rec
    add esp, 4092
  to_ret:
  mov eax, ebx
  ret
