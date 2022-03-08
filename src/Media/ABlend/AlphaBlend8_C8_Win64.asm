section .text

global AlphaBlend8_C8_DoBlend
global AlphaBlend8_C8_DoBlendPA

;void AlphaBlend8_C8_DoBlend(UInt8 *dest, OSInt dbpl, UInt8 *src, OSInt sbpl, OSInt width, OSInt height, UInt8 *rgbTable);

;0 edi
;8 esi
;16 ebx
;24 ebp
;32 retAddr
;rcx dest r10
;rdx dbpl / dAdd r11
;r8 src
;r9 sbpl / sAdd
;72 width
;80 height
;88 rgbTable

	align 16
AlphaBlend8_C8_DoBlend:
	push rbp
	push rbx
	push rsi
	push rdi
	mov rbx,qword [rsp+72] ;width
	shl rbx,2
	sub r9,rbx ;sAdd		OSInt sAdd = sbpl - width * 4;
	sub rdx,rbx ;dAdd		OSInt dAdd = dbpl - width * 4;
	xor rax,rax
	mov ebx,0x10101010
	
	movd xmm2,eax
	movd xmm4,ebx
	punpcklbw xmm4, xmm2
	mov eax,0x80008000
	movd xmm5,eax
	punpckldq xmm5,xmm5

	mov rsi,r8 ;src
	mov rdi,rcx ;dest
	mov r11,rdx
	mov rdx,qword [rsp+88] ;rgbTable
	mov rbp,qword [rsp+80] ;height
	ALIGN 16
iabbldlop:
	mov rcx,qword [rsp+72] ;width
	align 16
iadbldlop2:
	mov al,byte [rsi+3]
	mov ah,al
	movzx rbx,ax
	shl eax,16
	or eax,ebx

	movd xmm0,eax
	mov ebx,0xffffffff
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
	paddw xmm1,xmm5

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
	paddw xmm0,xmm5

	punpcklbw xmm3,xmm3
	pmulhuw xmm0,xmm3
	paddusw xmm0,xmm1
	paddw xmm0,xmm5

	pextrw rax,xmm0,3
	mov bh,byte [rdx+rax+196608]
	pextrw rax,xmm0,2
	mov bl,byte [rdx+rax+131072]
	shl ebx,16
	pextrw rax,xmm0,1
	mov bh,byte [rdx+rax+65536]
	pextrw rax,xmm0,0
	mov bl,byte [rdx+rax]
	mov dword [rdi],ebx

	lea rsi,[rsi+4]
	lea rdi,[rdi+4]
	dec rcx
	jnz iadbldlop2

	add rsi,r9 ;sAdd
	add rdi,r11 ;dAdd
	dec rbp
	jnz iabbldlop
	
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret
	
;void AlphaBlend8_C8_DoBlendPA(UInt8 *dest, OSInt dbpl, UInt8 *src, OSInt sbpl, OSInt width, OSInt height, UInt8 *rgbTable);

;0 edi
;8 esi
;16 ebx
;24 ebp
;32 retAddr
;rcx dest
;rdx dbpl
;r8 src
;r9 sbpl
;72 width
;80 height
;88 rgbTable

	align 16
AlphaBlend8_C8_DoBlendPA:
	push rbp
	push rbx
	push rsi
	push rdi
	mov rbx,qword [rsp+72] ;width
	shl rbx,2
	sub r9,rbx ;sAdd		OSInt sAdd = sbpl - width * 4;
	sub rdx,rbx ;dAdd		OSInt dAdd = dbpl - width * 4;

	xor eax,eax
	mov ebx,0x10101010
	
	movd xmm2,eax
	movd xmm4,ebx
	punpcklbw xmm4, xmm2
	mov eax,0x80008000
	movd xmm5,eax
	punpckldq xmm5,xmm5

	mov rsi,r8 ;src
	mov rdi,rcx ;dest
	mov r11,rdx
	mov rdx,qword [rsp+88] ;rgbTable
	mov rbp,qword [rsp+80] ;height
	ALIGN 16
iabbldpalop:
	mov rcx,qword [rsp+72] ;width
	align 16
iadbldpalop2:
	mov al,byte [rsi+3]
	mov ah,al
	movzx rbx,ax
	shl eax,16
	or eax,ebx

	movd xmm0,eax
	mov ebx,0xffffffff
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

	punpcklbw xmm3, xmm3
	pmulhuw xmm0,xmm3
	paddusw xmm0,xmm1

	pextrw rax,xmm0,3
	mov bh,byte [rdx+rax+196608]
	pextrw rax,xmm0,2
	mov bl,byte [rdx+rax+131072]
	shl ebx,16
	pextrw rax,xmm0,1
	mov bh,byte [rdx+rax+65536]
	pextrw rax,xmm0,0
	mov bl,byte [rdx+rax]
	mov dword [rdi],ebx

	lea rsi,[rsi+4]
	lea rdi,[rdi+4]
	dec rcx
	jnz iadbldpalop2

	add rsi,r9 ;sAdd
	add rdi,r11 ;dAdd
	dec rbp
	jnz iabbldpalop
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret
