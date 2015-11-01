BITS 32
start:
  org 0x7c00
  mov eax, 2
  mov dword [ebp+4], 5
  add dword [ebp+4], eax
  mov esi, [ebp+4]
  jmp 0
