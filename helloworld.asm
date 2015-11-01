BITS 32
start:
  org 0x7c00
  sub esp, 16
  mov ebp, esp
  mov eax, 2
  mov ebx, 3
  mov dword [ebp+4], 5
  add dword [ebp+4], eax
  sub dword [ebp+4], ebx
  mov esi, [ebp+4]
  jmp 0
