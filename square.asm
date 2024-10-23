in
in
in
pop CX
pop BX
pop AX
push BX
push BX
mul
push AX
push CX
push 4
mul
mul
sub
pop DX
push DX
push 0
je equal
push DX
push 0
ja negative
push 0
push BX
sub
push DX
sqrt
add
push AX
div
push 2
div
push 0
push BX
sub
push DX
sqrt
sub
push AX
div
push 2
div
push 2
out
out
out
hlt
negative:
push 0
out
hlt
equal:
push 0
push BX
sub
push AX
div
push 2
div
push 1
out
out
hlt