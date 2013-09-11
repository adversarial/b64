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
public b64_decode as '_base64_decode@12'

;======== Code =======================
; void __stdcall b64_decode(const char* cIn, char* szOut, signed int cbIn)
b64_decode:
        push ebp ebx esi edi   ; stdcall, eip, arg

        mov esi, [esp+4*5+4*0] ; cIn
        mov edi, [esp+4*5+4*1] ; szOut
        mov ecx, [esp+4*5+4*2] ; cbIn

        cld

  .decodeloop:
        jecxz .done

        xor eax, eax
        and edx, eax

        mov ebx, 4      ; counter
    .load:
        sub ecx, 1
        jl @f
        lodsb
      @@:
        rol eax, 8
        sub ebx, 1
        jnz .load       ; 22 33 44 11

        mov edx, ebx    ; ebx = 0 counter
        mov ebp, ecx    ; push ecx (strlen(cIn))
        mov ecx, 4      ; counter
        mov ebx, edi    ; push edi (szOut)

    .unpack:
        push ecx
        mov edi, b64index
        mov ecx, 64     ; 1 << 6
        shl edx, 6      ; next decoded
        repnz scasb     ; search for al
        test cl, cl
        jz @f           ; if it's zero remove odd question marks ($3f)
        not cl          ; 64 - index
        and cl, 00111111b ; remove accidental high bits
      @@:
        or dl, cl       ; 6 bit index
        rol eax, 8      ; next encoded
        pop ecx
        sub ecx, 1
        jnz .unpack     ; edx = 00 11 22 33

        xchg ebx, edi   ; edi = out, ebx = table
                        ; pop ebx edi
        mov eax, edx    ; eax = decoded

        rol eax, 16     ; 22 33 00 11
        stosb
        rol eax, 8      ; 33 00 11 22
        stosb
        rol eax, 8      ; 00 11 22 33
        stosb

        mov ecx, ebp    ; pop ecx (strlen(cIn))
        test ecx, ecx
        jg .decodeloop  ; if any chars remain...

  .done:
        xor eax, eax    ; zero terminated
        stosb
        pop edi esi ebx ebp
        ret 4*3