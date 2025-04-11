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

global AlphaBlend8_C8_DoBlend
global _AlphaBlend8_C8_DoBlend
global AlphaBlend8_C8_DoBlendPA
global _AlphaBlend8_C8_DoBlendPA

;void AlphaBlend8_C8_DoBlend(UInt8 *dest, OSInt dbpl, UInt8 *src, OSInt sbpl, OSInt width, OSInt height, UInt8 *rgbTable);

;0 rbx
;8 rbp
;16 retAddr
;rdi dest
;rsi dbpl / dAdd
;rdx src
;rcx sbpl / sAdd
;r8 width
;r9 height
;24 rgbTable

	align 16
AlphaBlend8_C8_DoBlend:
_AlphaBlend8_C8_DoBlend:
	push rbp
	push rbx
	lea rbx,[r8 * 4] ;width
	sub rcx,rbx ;sAdd		OSInt sAdd = sbpl - width * 4;
	sub rsi,rbx ;dAdd		OSInt dAdd = dbpl - width * 4;
	xor rax,rax
	mov ebx,0x10101010
	
	movd xmm2,eax
	movd xmm4,ebx
	punpcklbw xmm4, xmm2
	mov r10,rcx ;sAdd
	mov r11,rsi ;dAdd
	mov rsi,rdx

	mov rdx,qword [rsp+24] ;rgbTable
	shr r8,1
	jnbe iabbldlop
	jz iabbldlopone
	jmp iabbldlopodd

	ALIGN 16
iabbldlop:
	mov rcx,r8 ;width
	align 16
iadbldlop2:
	mov al,byte [rsi+7]
	mov ah,al
	movzx rbx,ax
	shl eax,16
	or eax,ebx
	shl rax,16
	mov al,byte [rsi+3]
	mov ah,al
	movzx rbx,ax
	shl rax,16
	or rax,rbx

	movq xmm0,rax
	mov rbx,-1
	sub rbx,rax
	movq xmm3,rbx

	movzx rax,byte [rsi+2]
	movq xmm1,[rdx+rax*8+262144]
	movzx rax,byte [rsi+6]
	movhps xmm1,[rdx+rax*8+262144]
	movzx rax,byte [rsi+1]
	movq xmm2,[rdx+rax*8+264192]
	movzx rax,byte [rsi+5]
	movhps xmm2,[rdx+rax*8+264192]
	paddsw xmm1,xmm2
	movzx rax,byte [rsi+0]
	movq xmm2,[rdx+rax*8+266240]
	movzx rax,byte [rsi+4]
	movhps xmm2,[rdx+rax*8+266240]
	paddsw xmm1,xmm2
	movzx rax,byte [rsi+3]
	movq xmm2,[rdx+rax*8+268288]
	movzx rax,byte [rsi+7]
	movhps xmm2,[rdx+rax*8+268288]
	paddsw xmm1,xmm2

	punpcklbw xmm0,xmm0
	pmulhuw xmm1,xmm0

	movzx rax,byte [rdi+2]
	movq xmm0,[rdx+rax*8+270336]
	movzx rax,byte [rdi+6]
	movhps xmm0,[rdx+rax*8+270336]
	movzx rax,byte [rdi+1]
	movq xmm2,[rdx+rax*8+272384]
	movzx rax,byte [rdi+5]
	movhps xmm2,[rdx+rax*8+272384]
	paddsw xmm0,xmm2
	movzx rax,byte [rdi+0]
	movq xmm2,[rdx+rax*8+274432]
	movzx rax,byte [rdi+4]
	movhps xmm2,[rdx+rax*8+274432]
	paddsw xmm0,xmm2
	movzx rax,byte [rdi+3]
	movq xmm2,[rdx+rax*8+276480]
	movzx rax,byte [rdi+7]
	movhps xmm2,[rdx+rax*8+276480]
	paddsw xmm0,xmm2

	punpcklbw xmm3,xmm3
	pmulhuw xmm0,xmm3
	paddusw xmm0,xmm1

	pextrw rax,xmm0,7
	mov bh,byte [rdx+rax+196608]
	pextrw rax,xmm0,6
	mov bl,byte [rdx+rax+131072]
	shl rbx,16
	pextrw rax,xmm0,4
	mov bl,byte [rdx+rax]
	pextrw rax,xmm0,5
	mov bh,byte [rdx+rax+65536]
	shl rbx,16
	pextrw rax,xmm0,3
	mov bh,byte [rdx+rax+196608]
	pextrw rax,xmm0,2
	mov bl,byte [rdx+rax+131072]
	shl rbx,16
	pextrw rax,xmm0,0
	mov bl,byte [rdx+rax]
	pextrw rax,xmm0,1
	mov bh,byte [rdx+rax+65536]
	mov qword [rdi],rbx

	add rsi,8
	add rdi,8
	dec rcx
	jnz iadbldlop2

	add rsi,r10 ;sAdd
	add rdi,r11 ;dAdd
	dec r9
	jnz iabbldlop
	
	pop rbx
	pop rbp
	ret
	
	ALIGN 16
iabbldlopodd:
	mov rcx,r8 ;width
	align 16
iadbldlopodd2:
	mov al,byte [rsi+7]
	mov ah,al
	movzx rbx,ax
	shl eax,16
	or eax,ebx
	shl rax,16
	mov al,byte [rsi+3]
	mov ah,al
	movzx rbx,ax
	shl rax,16
	or rax,rbx

	movq xmm0,rax
	mov rbx,-1
	sub rbx,rax
	movq xmm3,rbx

	movzx rax,byte [rsi+2]
	movq xmm1,[rdx+rax*8+262144]
	movzx rax,byte [rsi+6]
	movhps xmm1,[rdx+rax*8+262144]
	movzx rax,byte [rsi+1]
	movq xmm2,[rdx+rax*8+264192]
	movzx rax,byte [rsi+5]
	movhps xmm2,[rdx+rax*8+264192]
	paddsw xmm1,xmm2
	movzx rax,byte [rsi+0]
	movq xmm2,[rdx+rax*8+266240]
	movzx rax,byte [rsi+4]
	movhps xmm2,[rdx+rax*8+266240]
	paddsw xmm1,xmm2
	movzx rax,byte [rsi+3]
	movq xmm2,[rdx+rax*8+268288]
	movzx rax,byte [rsi+7]
	movhps xmm2,[rdx+rax*8+268288]
	paddsw xmm1,xmm2

	punpcklbw xmm0,xmm0
	pmulhuw xmm1,xmm0

	movzx rax,byte [rdi+2]
	movq xmm0,[rdx+rax*8+270336]
	movzx rax,byte [rdi+6]
	movhps xmm0,[rdx+rax*8+270336]
	movzx rax,byte [rdi+1]
	movq xmm2,[rdx+rax*8+272384]
	movzx rax,byte [rdi+5]
	movhps xmm2,[rdx+rax*8+272384]
	paddsw xmm0,xmm2
	movzx rax,byte [rdi+0]
	movq xmm2,[rdx+rax*8+274432]
	movzx rax,byte [rdi+4]
	movhps xmm2,[rdx+rax*8+274432]
	paddsw xmm0,xmm2
	movzx rax,byte [rdi+3]
	movq xmm2,[rdx+rax*8+276480]
	movzx rax,byte [rdi+7]
	movhps xmm2,[rdx+rax*8+276480]
	paddsw xmm0,xmm2

	punpcklbw xmm3,xmm3
	pmulhuw xmm0,xmm3
	paddusw xmm0,xmm1

	pextrw rax,xmm0,7
	mov bh,byte [rdx+rax+196608]
	pextrw rax,xmm0,6
	mov bl,byte [rdx+rax+131072]
	shl rbx,16
	pextrw rax,xmm0,4
	mov bl,byte [rdx+rax]
	pextrw rax,xmm0,5
	mov bh,byte [rdx+rax+65536]
	shl rbx,16
	pextrw rax,xmm0,3
	mov bh,byte [rdx+rax+196608]
	pextrw rax,xmm0,2
	mov bl,byte [rdx+rax+131072]
	shl rbx,16
	pextrw rax,xmm0,0
	mov bl,byte [rdx+rax]
	pextrw rax,xmm0,1
	mov bh,byte [rdx+rax+65536]
	mov qword [rdi],rbx

	add rsi,8
	add rdi,8
	dec rcx
	jnz iadbldlopodd2

	mov al,byte [rsi+3]
	mov ah,al
	movzx ebx,ax
	shl eax,16
	or eax,ebx

	movd xmm0,eax
	mov ebx,-1
	sub ebx,eax
	movd xmm3,ebx

	movzx rax,byte [rsi+2]
	movq xmm1,[rdx+rax*8+262144]
	movzx rax,byte [rsi+1]
	movq xmm2,[rdx+rax*8+264192]
	paddsw xmm1,xmm2
	movzx rax,byte [rsi+0]
	movq xmm2,[rdx+rax*8+266240]
	paddsw xmm1,xmm2
	movzx rax,byte [rsi+3]
	movq xmm2,[rdx+rax*8+268288]
	paddsw xmm1,xmm2

	punpcklbw xmm0,xmm0
	pmulhuw xmm1,xmm0

	movzx rax,byte [rdi+2]
	movq xmm0,[rdx+rax*8+270336]
	movzx rax,byte [rdi+1]
	movq xmm2,[rdx+rax*8+272384]
	paddsw xmm0,xmm2
	movzx rax,byte [rdi+0]
	movq xmm2,[rdx+rax*8+274432]
	paddsw xmm0,xmm2
	movzx rax,byte [rdi+3]
	movq xmm2,[rdx+rax*8+276480]
	paddsw xmm0,xmm2

	punpcklbw xmm3,xmm3
	pmulhuw xmm0,xmm3
	paddusw xmm0,xmm1

	pextrw rax,xmm0,3
	mov bh,byte [rdx+rax+196608]
	pextrw rax,xmm0,2
	mov bl,byte [rdx+rax+131072]
	shl ebx,16
	pextrw rax,xmm0,0
	mov bl,byte [rdx+rax]
	pextrw rax,xmm0,1
	mov bh,byte [rdx+rax+65536]
	mov dword [rdi],ebx

	add rsi,4
	add rdi,4

	add rsi,r10 ;sAdd
	add rdi,r11 ;dAdd
	dec r9
	jnz iabbldlopodd
	
	pop rbx
	pop rbp
	ret
	
	ALIGN 16
iabbldlopone:
	mov al,byte [rsi+3]
	mov ah,al
	movzx ebx,ax
	shl eax,16
	or eax,ebx

	movd xmm0,eax
	mov ebx,-1
	sub ebx,eax
	movd xmm3,ebx

	movzx rax,byte [rsi+2]
	movq xmm1,[rdx+rax*8+262144]
	movzx rax,byte [rsi+1]
	movq xmm2,[rdx+rax*8+264192]
	paddsw xmm1,xmm2
	movzx rax,byte [rsi+0]
	movq xmm2,[rdx+rax*8+266240]
	paddsw xmm1,xmm2
	movzx rax,byte [rsi+3]
	movq xmm2,[rdx+rax*8+268288]
	paddsw xmm1,xmm2

	punpcklbw xmm0,xmm0
	pmulhuw xmm1,xmm0

	movzx rax,byte [rdi+2]
	movq xmm0,[rdx+rax*8+270336]
	movzx rax,byte [rdi+1]
	movq xmm2,[rdx+rax*8+272384]
	paddsw xmm0,xmm2
	movzx rax,byte [rdi+0]
	movq xmm2,[rdx+rax*8+274432]
	paddsw xmm0,xmm2
	movzx rax,byte [rdi+3]
	movq xmm2,[rdx+rax*8+276480]
	paddsw xmm0,xmm2

	punpcklbw xmm3,xmm3
	pmulhuw xmm0,xmm3
	paddusw xmm0,xmm1

	pextrw rax,xmm0,3
	mov bh,byte [rdx+rax+196608]
	pextrw rax,xmm0,2
	mov bl,byte [rdx+rax+131072]
	shl ebx,16
	pextrw rax,xmm0,0
	mov bl,byte [rdx+rax]
	pextrw rax,xmm0,1
	mov bh,byte [rdx+rax+65536]
	mov dword [rdi],ebx

	add rsi,4
	add rdi,4

	add rsi,r10 ;sAdd
	add rdi,r11 ;dAdd
	dec r9
	jnz iabbldlopone
	
	pop rbx
	pop rbp
	ret
	
;void AlphaBlend8_C8_DoBlendPA(UInt8 *dest, OSInt dbpl, UInt8 *src, OSInt sbpl, OSInt width, OSInt height, UInt8 *rgbTable);

;0 rbx
;8 rbp
;16 retAddr
;rdi dest
;rsi dbpl
;rdx src
;rcx sbpl
;r8 width
;r9 height
;24 rgbTable

	align 16
AlphaBlend8_C8_DoBlendPA:
_AlphaBlend8_C8_DoBlendPA:
	push rbp
	push rbx
	lea rbx,[r8*4] ;width
	sub rcx,rbx ;sAdd		OSInt sAdd = sbpl - width * 4;
	sub rsi,rbx ;dAdd		OSInt dAdd = dbpl - width * 4;

	mov r10,rcx ;sAdd
	mov r11,rsi ;dAdd
	mov rsi,rdx ;src

	mov rdx,qword [rsp+24] ;rgbTable
	shr r8,1
	jnbe iabbldpalop
	jz iabbldpalopone
	jmp iabbldpalopodd

	ALIGN 16
iabbldpalop:
	mov rcx,r8 ;width
	align 16
iadbldpalop2:
	mov al,byte [rsi+7]
	mov ah,al
	movzx rbx,ax
	shl eax,16
	or eax,ebx
	shl rax,16
	mov al,byte [rsi+3]
	mov ah,al
	movzx rbx,ax
	shl rax,16
	or rax,rbx

	mov rbx,-1
	sub rbx,rax
	movq xmm3,rbx

	movzx rax,byte [rdi+2]
	movq xmm0,[rdx+rax*8+270336]
	movzx rax,byte [rdi+6]
	movhps xmm0,[rdx+rax*8+270336]
	movzx rax,byte [rdi+1]
	movq xmm2,[rdx+rax*8+272384]
	movzx rax,byte [rdi+5]
	movhps xmm2,[rdx+rax*8+272384]
	paddsw xmm0,xmm2
	movzx rax,byte [rdi+0]
	movq xmm2,[rdx+rax*8+274432]
	movzx rax,byte [rdi+4]
	movhps xmm2,[rdx+rax*8+274432]
	paddsw xmm0,xmm2
	movzx rax,byte [rdi+3]
	movq xmm2,[rdx+rax*8+276480]
	movzx rax,byte [rdi+7]
	movhps xmm2,[rdx+rax*8+276480]
	paddsw xmm0,xmm2

	movzx rax,byte [rsi+2]
	movq xmm1,[rdx+rax*8+262144]
	movzx rax,byte [rsi+6]
	movhps xmm1,[rdx+rax*8+262144]
	movzx rax,byte [rsi+1]
	movq xmm2,[rdx+rax*8+264192]
	movzx rax,byte [rsi+5]
	movhps xmm2,[rdx+rax*8+264192]
	paddsw xmm1,xmm2
	movzx rax,byte [rsi+0]
	movq xmm2,[rdx+rax*8+266240]
	movzx rax,byte [rsi+4]
	movhps xmm2,[rdx+rax*8+266240]
	paddsw xmm1,xmm2
	movzx rax,byte [rsi+3]
	movq xmm2,[rdx+rax*8+268288]
	movzx rax,byte [rsi+7]
	movhps xmm2,[rdx+rax*8+268288]
	paddsw xmm1,xmm2

	punpcklbw xmm3, xmm3
	pmulhuw xmm0,xmm3
	paddusw xmm0,xmm1

	pextrw rax,xmm0,7
	mov bh,byte [rdx+rax+196608]
	pextrw rax,xmm0,6
	mov bl,byte [rdx+rax+131072]
	shl ebx,16
	pextrw rax,xmm0,4
	mov bl,byte [rdx+rax]
	pextrw rax,xmm0,5
	mov bh,byte [rdx+rax+65536]
	shl rbx,16
	pextrw rax,xmm0,3
	mov bh,byte [rdx+rax+196608]
	pextrw rax,xmm0,2
	mov bl,byte [rdx+rax+131072]
	shl rbx,16
	pextrw rax,xmm0,0
	mov bl,byte [rdx+rax]
	pextrw rax,xmm0,1
	mov bh,byte [rdx+rax+65536]
	mov qword [rdi],rbx

	add rsi,8
	add rdi,8
	dec rcx
	jnz iadbldpalop2

	add rsi,r10 ;sAdd
	add rdi,r11 ;dAdd
	dec r9
	jnz iabbldpalop
	pop rbx
	pop rbp
	ret

	ALIGN 16
iabbldpalopodd:
	mov rcx,r8 ;width
	test rcx,rcx
	jz iadbldpalopodd2b

	align 16
iadbldpalopodd2:
	mov al,byte [rsi+7]
	mov ah,al
	movzx rbx,ax
	shl eax,16
	or eax,ebx
	shl rax,16
	mov al,byte [rsi+3]
	mov ah,al
	movzx rbx,ax
	shl rax,16
	or rax,rbx

	mov rbx,-1
	sub rbx,rax
	movq xmm3,rbx

	movzx rax,byte [rdi+2]
	movq xmm0,[rdx+rax*8+270336]
	movzx rax,byte [rdi+6]
	movhps xmm0,[rdx+rax*8+270336]
	movzx rax,byte [rdi+1]
	movq xmm2,[rdx+rax*8+272384]
	movzx rax,byte [rdi+5]
	movhps xmm2,[rdx+rax*8+272384]
	paddsw xmm0,xmm2
	movzx rax,byte [rdi+0]
	movq xmm2,[rdx+rax*8+274432]
	movzx rax,byte [rdi+4]
	movhps xmm2,[rdx+rax*8+274432]
	paddsw xmm0,xmm2

	movzx rax,byte [rsi+2]
	movq xmm1,[rdx+rax*8+262144]
	movzx rax,byte [rsi+6]
	movhps xmm1,[rdx+rax*8+262144]
	movzx rax,byte [rsi+1]
	movq xmm2,[rdx+rax*8+264192]
	movzx rax,byte [rsi+5]
	movhps xmm2,[rdx+rax*8+264192]
	paddsw xmm1,xmm2
	movzx rax,byte [rsi+0]
	movq xmm2,[rdx+rax*8+266240]
	movzx rax,byte [rsi+4]
	movhps xmm2,[rdx+rax*8+266240]
	paddsw xmm1,xmm2

	punpcklbw xmm3, xmm3
	pmulhuw xmm0,xmm3
	paddusw xmm0,xmm1

	pextrw rax,xmm0,7
	mov bh,byte [rdx+rax+196608]
	pextrw rax,xmm0,6
	mov bl,byte [rdx+rax+131072]
	shl ebx,16
	pextrw rax,xmm0,4
	mov bl,byte [rdx+rax]
	pextrw rax,xmm0,5
	mov bh,byte [rdx+rax+65536]
	shl rbx,16
	pextrw rax,xmm0,3
	mov bh,byte [rdx+rax+196608]
	pextrw rax,xmm0,2
	mov bl,byte [rdx+rax+131072]
	shl rbx,16
	pextrw rax,xmm0,0
	mov bl,byte [rdx+rax]
	pextrw rax,xmm0,1
	mov bh,byte [rdx+rax+65536]
	mov qword [rdi],rbx

	add rsi,8
	add rdi,8

	dec rcx
	jnz iadbldpalopodd2

iadbldpalopodd2b:
	mov al,byte [rsi+3]
	mov ah,al
	movzx rbx,ax
	shl eax,16
	or eax,ebx

	mov ebx,0xffffffff
	sub ebx,eax
	or ebx,0xff000000
	movd xmm3,ebx

	movzx rax,byte [rdi+2]
	movq xmm0,[rdx+rax*8+270336]
	movzx rax,byte [rsi+2]
	movhps xmm0,[rdx+rax*8+262144]
	movzx rax,byte [rdi+1]
	movq xmm2,[rdx+rax*8+272384]
	movzx rax,byte [rsi+1]
	movhps xmm2,[rdx+rax*8+264192]
	paddsw xmm0,xmm2
	movzx rax,byte [rdi+0]
	movq xmm2,[rdx+rax*8+274432]
	movzx rax,byte [rsi+0]
	movhps xmm2,[rdx+rax*8+266240]
	paddsw xmm0,xmm2

	movhlps xmm1,xmm0
	punpcklbw xmm3, xmm3
	pmulhuw xmm0,xmm3
	paddusw xmm0,xmm1

	pextrw rax,xmm0,3
	mov bh,byte [rdx+rax+196608]
	pextrw rax,xmm0,2
	mov bl,byte [rdx+rax+131072]
	shl ebx,16
	pextrw rax,xmm0,0
	mov bl,byte [rdx+rax]
	pextrw rax,xmm0,1
	mov bh,byte [rdx+rax+65536]
	mov dword [rdi],ebx

	add rsi,4
	add rdi,4

	add rsi,r10 ;sAdd
	add rdi,r11 ;dAdd
	dec r9
	jnz iabbldpalopodd
	pop rbx
	pop rbp
	ret

	ALIGN 16
iabbldpalopone:
	mov al,byte [rsi+3]
	mov ah,al
	movzx rbx,ax
	shl eax,16
	or eax,ebx

	mov ebx,0xffffffff
	sub ebx,eax
	or ebx,0xff000000
	movd xmm3,ebx

	movzx rax,byte [rdi+2]
	movq xmm0,[rdx+rax*8+270336]
	movzx rax,byte [rsi+2]
	movhps xmm0,[rdx+rax*8+262144]
	movzx rax,byte [rdi+1]
	movq xmm2,[rdx+rax*8+272384]
	movzx rax,byte [rsi+1]
	movhps xmm2,[rdx+rax*8+264192]
	paddsw xmm0,xmm2
	movzx rax,byte [rdi+0]
	movq xmm2,[rdx+rax*8+274432]
	movzx rax,byte [rsi+0]
	movhps xmm2,[rdx+rax*8+266240]
	paddsw xmm0,xmm2

	movhlps xmm1,xmm0
	punpcklbw xmm3, xmm3
	pmulhuw xmm0,xmm3
	paddusw xmm0,xmm1

	pextrw rax,xmm0,3
	mov bh,byte [rdx+rax+196608]
	pextrw rax,xmm0,2
	mov bl,byte [rdx+rax+131072]
	shl ebx,16
	pextrw rax,xmm0,0
	mov bl,byte [rdx+rax]
	pextrw rax,xmm0,1
	mov bh,byte [rdx+rax+65536]
	mov dword [rdi],ebx

	add rsi,4
	add rdi,4

	add rsi,r10 ;sAdd
	add rdi,r11 ;dAdd
	dec r9
	jnz iabbldpalopone
	pop rbx
	pop rbp
	ret

