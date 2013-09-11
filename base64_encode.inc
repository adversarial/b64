; Copyright (c) 2013 x8esix

; Permission is hereby granted, free of charge, to any person obtaining a copy
; of this software and associated documentation files (the "Software"), to deal
; in the Software without restriction, including without limitation the rights
; to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
; copies of the Software, and to permit persons to whom the Software is
; furnished to do so, subject to the following conditions:
;
; The above copyright notice and this permission notice shall be included in
; all copies or substantial portions of the Software.
;
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
; IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
; FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
; AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
; LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
; OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
; THE SOFTWARE.

;======== Exports ====================
public b64_encode as '_base64_encode@12'

;======== Code =======================
; void __stdcall b64_encode(const char* cIn, char* szOut, signed int cbIn)
b64_encode:
        push ebp ebx esi edi
                                 ; stdcall, eip, arg
        mov esi, [esp+4*5+4*0] ; cIn
        mov edi, [esp+4*5+4*1] ; szOut
        mov ecx, [esp+4*5+4*2] ; cbIn
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

    .checkpadding:
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
        mov al, cl        ; will be 0 from loop or index
        stosb             ; zero terminate
        pop edi esi ebx ebp
        ret 2*4