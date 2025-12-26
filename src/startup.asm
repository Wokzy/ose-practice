[BITS 32]

[EXTERN main]
_start:
  call main
  mov ecx, eax
  xor eax, eax
  int 0x80
  ret

