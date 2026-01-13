[BITS 32]

[GLOBAL std_print_char]
std_print_char:
  mov eax, 1
  mov ecx, [esp + 4]
  int 0x80
  ret

[GLOBAL std_n_rec]
std_n_rec:
  mov eax, [esp + 4]
  cmp eax, 0
  mov ebx, esp
  jz to_ret
    sub esp, 4088
    sub eax, 1
    push eax
    call std_n_rec
    add esp, 4092
  to_ret:
  mov eax, ebx
  ret
