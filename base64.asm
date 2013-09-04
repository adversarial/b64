; Base64 implementation in FASM
;
; by x8esix
;
; see LICENSE

include 'win32a.inc'

format MS COFF

;======== Constants ==================
PADDINGCHAR         equ     '='


;======== Code ==========================
section '.text' code executable writeable
;========================================

b64index db 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'
         db 'abcdefghijklmnopqrstuvwxyz'
         db '0123456789+/'

include 'base64_encode.inc'
;include 'base64_decode.inc'