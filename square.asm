call main
hlt

main:
    in
    in
    in
    pop CX
    pop BX
    pop AX

    push 0
    push AX
    je aiszero
    call quadratic
    ret

    aiszero:
    call linear
    ret

;;==============================================
;; Calculate discriminant for quadratic equation
;;
;; Input:
;;      AX, BX, CX - quadratic coefficients
;;
;; Destroys:
;;      DX
;;
;; Returns
;;      DX - discriminant value
;;
;;==============================================
discriminant:
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
    ret

;;==============================================
;; Calculate discriminant for quadratic equation is a is not zero
;;
;; Input:
;;      AX, BX - quadratic coefficients
;;
;; Returns
;;      DX - discriminant value
;;
;;==============================================
quadratic:
    call discriminant

    push DX
    push 0
    je diszero

    push DX
    push 0
    ja disnegative

    jmp dispositive

    dispositive:
    call tworoots
    ret

    diszero:
    call oneroot
    ret

    disnegative:
    call zeroroots
    ret

zeroroots:
    push 0
    out
    ret

oneroot:
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
    ret

tworoots:
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
    ret

linear:
    push 0
    push BX
    je biszero

    push 0
    push CX
    sub
    push BX
    div
    push 1
    out
    out
    ret

    biszero:
    call zeroorinf
    ret

zeroorinf:
    push 0
    push CX
    je czero

    push 0
    out
    ret

    czero:
    call endlessroots
    ret

endlessroots:
    push INF
    out
    ret
