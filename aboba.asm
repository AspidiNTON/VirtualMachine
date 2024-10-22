push 7
pop AX
push 3
jmp xdd
pop [3]
push AX
aboba:
pop [AX]
jmp lol
xdd:
jmp aboba
lol:
hlt