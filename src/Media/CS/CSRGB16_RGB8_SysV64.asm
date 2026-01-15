%ifidn __OUTPUT_FORMAT__,elf
section .note.GNU-stack noalloc noexec nowrite progbits
%endif
%ifidn __OUTPUT_FORMAT__,elf32
section .note.GNU-stack noalloc noexec nowrite progbits
%endif
%ifidn __OUTPUT_FORMAT__,elf64
section .note.GNU-stack noalloc noexec nowrite progbits
%endif

section .text

global CSRGB16_RGB8_Convert
global _CSRGB16_RGB8_Convert

;void CSRGB16_RGB8_Convert(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS srcRGBBpl, IntOS destRGBBpl, IntOS srcNBits, IntOS destNbits, UInt8 *rgbTable);
;0 rdi
;8 rsi
;16 rbx
;32 retAddr
;rcx srcPtr
;rdx destPtr
;r8 width
;r9 height
;72 srcRGBBpl
;80 destRGBBpl
;88 srcNBits
;96 destNBits
;104 rgbTable
	align 16
CSRGB16_RGB8_Convert:
_CSRGB16_RGB8_Convert:
	sub rsp,32
	mov qword [rsp],rdi
	mov qword [rsp+8],rsi
	mov qword [rsp+16],rbx
	mov rax,qword [rsp+96] ;destNBits
	cmp rax,32
	jz crgb32start
	jmp crgbexit
	
	align 16
crgb32start:
	mov rax,qword [rsp+88] ;srcNBits
	cmp rax,64
	jz crgb32_64start
	cmp rax,48
	jz crgb32_48start
	jmp crgbexit

	align 16
crgb32_64start:
	mov rsi,rcx ;srcPtr
	mov rdi,rdx ;destPtr
	mov rbx,qword [rsp+104] ;tab
	lea rdx,[r8*4]
	lea rax,[r8*8]
	sub qword [rsp+72],rax ;srcRGBBpl
	sub qword [rsp+80],rdx ;destRGBBpl
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
	add rsi,qword [rsp+72] ;srcRGBBpl
	add rdi,qword [rsp+80] ;destRGBBpl
	dec r9
	jnz crgb32_64lop
	jmp crgbexit

	align 16
crgb32_48start:
	mov rsi,rcx ;srcPtr
	mov rdi,rdx ;destPtr
	mov rbx,qword [rsp+104] ;tab
	lea rdx,[r8*4]
	lea rax,[rdx+r8*2]
	sub qword [rsp+72],rax ;srcRGBBpl
	sub qword [rsp+80],rdx ;destRGBBpl
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
	add rsi,qword [rsp+72] ;srcRGBBpl
	add rdi,qword [rsp+80] ;destRGBBpl
	dec r9
	jnz crgb32_48lop
	
	align 16
crgbexit:
	mov rdi,qword [rsp]
	mov rsi,qword [rsp+8]
	mov rbx,qword [rsp+16]
	add rsp,32
	ret
