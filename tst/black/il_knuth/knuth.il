
; Knuth's online variance:
;    n = 1
;    mean = 5.0
;    m2 = 0.0
;    delta = 0.0

;    read x :
;        n += 1
;        delta = x - mean
;        mean += delta/n
;        m2 += delta*(x - mean)

;    if n < 2:
;        return float('nan')
;    else:
;        return m2 / (n - 1)

    LD %MF0  ; n = n+1
    ADD %MF4 ;
    ST %MF0  ;

    LD %IF0  ; read x
    SUB %MF1 ;
    ST %MF3  ; delta = x - mean

    LD %MF1  ; Acc = mean
    ADD(%MF3 ; Acc = delta,              Stack = ADD mean
    DIV %MF0 ; Acc = delta/n,            Stack = ADD mean
    )        ; Acc = mean + ( delta/n )
    ST %MF1  ; mean = mean + ( delta/n )

    LD %MF2  ; Acc = m2
    ADD(%MF3 ; Acc = delta,              Stack = ADD m2
    MUL(%IF0 ; Acc = x,                  Stack = MUL delta, ADD m2
    SUB %MF1 ; Acc = x - mean,           Stack = MUL delta, ADD m2
    )        ; Acc = delta * (x - mean), Stack = ADD m2
    )        ; Acc = m2 + (delta * (x - mean))
    ST %MF2  ; m2 = m2 + (delta * (x - mean))

    LD %MF2  ; Acc = m2
    DIV(%MF0 ; Acc = n,                  Stack = DIV m2
    SUB %MF4   ; Acc = n-1,                Stack = DIV m2
    )          ; Acc = m2 / n-1,
    ST %QF0  ; y = m2 / (n - 1)

