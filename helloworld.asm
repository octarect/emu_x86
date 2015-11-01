BITS 32
start:
  org 0x7c00
  mov dword [ebp+4], 5
  mov esi, [ebp+4]
  mov eax, 41
  jmp 0
