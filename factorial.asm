in
pop AX
push AX
pop BX
func:
push BX
push 1
sub
pop BX
push BX
push AX
mul
pop AX
push BX
push 0
ja func
push AX
out
hlt

call = jmp + return address
ret = pop return address + jump on it

; input: RDX - number
; output: RAX - return value
; 