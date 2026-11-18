.code

DotVec3ASM PROC
    ; a est dans RCX, b dans RDX

    movss xmm0, dword ptr [rcx]
    mulss xmm0, dword ptr [rdx]

    movss xmm1, dword ptr [rcx+4]
    mulss xmm1, dword ptr [rdx+4]
    addss xmm0, xmm1

    movss xmm1, dword ptr [rcx+8]
    mulss xmm1, dword ptr [rdx+8]
    addss xmm0, xmm1

    ret
DotVec3ASM ENDP

END