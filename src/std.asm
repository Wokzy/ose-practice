[BITS 32]

[GLOBAL std_print_char]
std_print_char:
  mov eax, 1
  mov ecx, [esp + 4]
  int 0x80
  ret
