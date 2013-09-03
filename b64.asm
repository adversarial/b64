; Base64 implementation in FASM
;
; by x8esix
;
; see LICENSE

include 'win32a.inc'

format MS COFF

;======== Exports ====================
public b64_encode as '__b64_encode@12'

;======== Constants ==================
PADDINGCHAR         equ     '='

;======== Code ==========================
section '.text' code executable writeable
;========================================

b64index db 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'
         db 'abcdefghijklmnopqrstuvwxyz'
         db '0123456789+/'

; void __stdcall b64_encode(const char* cIn, char* szOut, signed int cbIn)
b64_encode:
        push ebx esi edi
                                 ; stdcall, eip, arg
        mov esi, [esp+4*4+4*0] ; cIn
        mov edi, [esp+4*4+4*1] ; szOut
        mov ecx, [esp+4*4+4*2] ; cbIn
        mov ebx, b64index

        push ecx
  .encode:
        xor eax, eax
                        ; apparently FASM 1.71 doesn't support anonymous labels
                        ; in repeat statements. bugplz?

        sub ecx, 1
        jl @f           ; signed pls
        lodsb
    @@:
        shl eax, 8      ; xx xx 11 00
        sub ecx, 1
        jl @f
        lodsb
    @@:
        shl eax, 8      ; xx 11 22 00
        sub ecx, 1
        jl @f
        lodsb
    @@:
        shl eax, 8      ; 11 22 33 00
    .loop:
    repeat 4
        rol eax, 6
        and al, 00111111b ; 2^6=64
        xlatb             ; can't remember using this in a while
        stosb
    end repeat
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