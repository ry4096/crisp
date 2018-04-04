

section .text

global shift_esp

global call_with_buffer
global call_with_buffer_char
global call_with_buffer_short
global call_with_buffer_int
global call_with_buffer_long
global call_with_buffer_long_long
global call_with_buffer_float
global call_with_buffer_double
global call_with_buffer_long_double
global call_with_buffer_pointer

global call_fn_char
global call_fn_int
global call_fn_float
global call_fn_double
global call_fn_pointer
global call_fn
 

shift_esp:
  mov ebx, [esp]
  mov ecx, [esp+4]
  sub esp, ecx
  mov [esp], ecx
  mov eax, esp
  ret


;; args at (*) = (
;;   pushed ebx       ebp + 4
;;   pushed ebp       ebp + 8
;;   type (*fn)(...), ebp + 12
;;   void* buffer,    ebp + 16
;;   int buff_len     ebp + 20
;;)
call_with_buffer_char:
call_with_buffer_short:
call_with_buffer_int:
call_with_buffer_long:
call_with_buffer_long_long:
call_with_buffer_float:
call_with_buffer_double:
call_with_buffer_long_double:
call_with_buffer_pointer:
call_with_buffer:
   push ebx
   push ebp
   mov ebp, esp

   ;; (*)

   mov ebx, [ebp+20]
   mov ecx, [ebp+16]
   sub esp, ebx
loop_start:
   dec ebx
   jl loop_done
   mov dl, [ecx+ebx]
   mov [esp+ebx], dl
   jmp loop_start
loop_done:
   call [ebp+12]

   mov esp, ebp
   pop ebp
   pop ebx

   ret


call_fn_char:
call_fn_int:
call_fn_pointer:
call_fn:
call_fn_float:
call_fn_double:
  push ebp
  mov ebp, esp
  call [ebp+8]
  mov esp, ebp
  pop ebp
  ret




