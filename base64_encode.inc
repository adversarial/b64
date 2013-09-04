; Base64 implementation in FASM
;
; by x8esix
;
; see LICENSE

;======== Exports ====================
public b64_encode as '_base64_encode@12'

;======== Code =======================
; void __stdcall b64_encode(const char* cIn, char* szOut, signed int cbIn)
b64_encode:
        push ebx esi edi
                                 ; stdcall, eip, arg
        mov esi, [esp+4*4+4*0] ; cIn
        mov edi, [esp+4*4+4*1] ; szOut
        mov ecx, [esp+4*4+4*2] ; cbIn
        mov ebx, b64index

        cld

        push ecx
  .encode:
        ;jecxz .done      ; optimize later
        test ecx, ecx
        jz .done
        xor eax, eax      ; apparently FASM 1.7x doesn't support anonymous labels
                          ; in repeat statements.
                          ; http://board.flatassembler.net/topic.php?t=1572

        sub ecx, 1
        jl @f             ; signed pls
        lodsb
    @@:
        shl eax, 8        ; xx xx 11 00
        sub ecx, 1
        jl @f
        lodsb
    @@:
        shl eax, 8        ; xx 11 22 00
        sub ecx, 1
        jl @f
        lodsb
    @@:
        shl eax, 8        ; 11 22 33 00
        mov ebp, ecx      ; push ecx
        mov ecx, 4
    .expand:
        rol eax, 6
        and al, 00111111b ; 2^6=64
        xlatb             ; can't remember using this in a while
        stosb
        sub ecx, 1
        jnz .expand

        mov ecx, ebp      ; pop ecx
        test ecx, ecx
        jg .encode

        pop eax           ; strlen
        mov ecx, 3
        cdq               ;
        div ecx           ; if (!strlen(szIn) % 3)
        xchg ecx, eax     ;     goto .done
        jecxz .done       ; else
        sub eax, ecx      ;     numPadding = (3 - (strlen % 3))
        sub edi, eax      ;     szOut -= numPadding
        mov ecx, eax
        mov al, PADDINGCHAR
    .pad:
        stosb
        sub ecx, 1
        jnz .pad
    .done:
        pop edi esi ebx
        ret 2*4