section .text

global CSRGB16_RGB8_Convert
global _CSRGB16_RGB8_Convert

;void CSRGB16_RGB8_Convert(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, OSInt srcNBits, OSInt destNbits, UInt8 *rgbTable);
;-24 edi
;-16 esi
;-8 ebx
;0 retAddr
;rcx srcPtr
;rdx destPtr
;r8 width
;r9 height
;40 srcRGBBpl
;48 destRGBBpl
;56 srcNBits
;64 destNBits
;72 rgbTable
	align 16
CSRGB16_RGB8_Convert:
_CSRGB16_RGB8_Convert:
	mov qword [rsp-24],rdi
	mov qword [rsp-16],rsi
	mov qword [rsp-8],rbx
	mov rax,qword [rsp+64] ;destNBits
	cmp rax,32
	jz crgb32start
	jmp crgbexit
	
	align 16
crgb32start:
	mov rax,qword [rsp+56] ;srcNBits
	cmp rax,64
	jz crgb32_64start
	cmp rax,48
	jz crgb32_48start
	jmp crgbexit

	align 16
crgb32_64start:
	mov rsi,rcx ;srcPtr
	mov rdi,rdx ;destPtr
	mov rbx,qword [rsp+72] ;tab
	lea rdx,[r8*4]
	lea rax,[r8*8]
	sub qword [rsp+40],rax ;srcRGBBpl
	sub qword [rsp+48],rdx ;destRGBBpl
	align 16
crgb32_64lop:
	mov rcx,r8 ;width
	align 16
crgb32_64lop2:
	movzx rdx,word [rsi+4]
	mov al,byte [rbx+rdx+131072]
	mov ah,byte [rsi+7]
	shl eax,16
	movzx rdx,word [rsi+2]
	mov ah,byte [rbx+rdx+65536]
	movzx rdx,word [rsi]
	mov al,byte [rbx+rdx]
	mov dword [rdi],eax
	lea rsi,[rsi+8]
	lea rdi,[rdi+4]
	dec rcx
	jnz crgb32_64lop2
	add rsi,qword [rsp+40] ;srcRGBBpl
	add rdi,qword [rsp+48] ;destRGBBpl
	dec r9
	jnz crgb32_64lop
	jmp crgbexit

	align 16
crgb32_48start:
	mov rsi,rcx ;srcPtr
	mov rdi,rdx ;destPtr
	mov rbx,qword [rsp+72] ;tab
	lea rdx,[r8*4]
	lea rax,[rdx+r8*2]
	sub qword [rsp+40],rax ;srcRGBBpl
	sub qword [rsp+48],rdx ;destRGBBpl
	align 16
crgb32_48lop:
	mov rcx,r8 ;width
	align 16
crgb32_48lop2:
	movzx rdx,word [rsi+4]
	mov al,byte [rbx+rdx+131072]
	mov ah,0xff
	shl eax,16
	movzx rdx,word [rsi+2]
	mov ah,byte [rbx+rdx+65536]
	movzx rdx,word [rsi]
	mov al,byte [rbx+rdx]
	mov dword [rdi],eax
	lea rsi,[rsi+6]
	lea rdi,[rdi+4]
	dec rcx
	jnz crgb32_48lop2
	add rsi,qword [rsp+40] ;srcRGBBpl
	add rdi,qword [rsp+48] ;destRGBBpl
	dec r9
	jnz crgb32_48lop
	
	align 16
crgbexit:
	mov rdi,qword [rsp-24]
	mov rsi,qword [rsp-16]
	mov rbx,qword [rsp-8]
	ret
