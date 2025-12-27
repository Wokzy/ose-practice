[BITS 32]

[EXTERN main]
_start:
  mov eax, [esp + 8]
  mov ecx, [esp + 4]
  sub esp, 8
  push eax
  push ecx
  call main
  mov ecx, eax
  xor eax, eax
  int 0x80
  ret

