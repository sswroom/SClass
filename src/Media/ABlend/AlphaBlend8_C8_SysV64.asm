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
	ALIGN 16
iabbldlop:
	mov rcx,r8 ;width
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
	or ebx,0xff000000
	movd xmm3,ebx

;	movzx ebx,byte [rsi+3]
;	mov bh,bl
;	movzx rax,byte [rsi+2]
;	shl ebx,16
;	mov bx,word [rdx+rax*2+262144]
;	movd xmm2,ebx
;	movzx rax,byte [rsi+1]
;	movzx ebx,word [rdx+rax*2+262656]
;	shl ebx,16
;	movzx rax,byte [rsi]
;	mov bx,word [rdx+rax*2+263168]
;	movd xmm1,ebx
;	punpckldq xmm1,xmm2
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

;	movzx ebx,byte [rdi+3]
;	mov bh,bl
;	shl ebx,16
;	movzx rax,byte [rdi+2]
;	mov bx,word [rdx+rax*2+263680]
;	movd xmm2,ebx
;	movzx rax,byte [rdi+1]
;	movzx ebx,word [rdx+rax*2+264192]
;	shl ebx,16
;	movzx rax,byte [rdi]
;	mov bx,word [rdx+rax*2+264704]
;	movd xmm0,ebx
;	punpckldq xmm0,xmm2
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

	pextrw rbx,xmm0,3
	pextrw rax,xmm0,2
	mov bh,byte [rdx+rbx+196608]
	mov bl,byte [rdx+rax+131072]
	shl ebx,16
	pextrw rax,xmm0,0
	mov bl,byte [rdx+rax]
	pextrw rax,xmm0,1
	mov bh,byte [rdx+rax+65536]
	mov dword [rdi],ebx

	lea rsi,[rsi+4]
	lea rdi,[rdi+4]
	dec rcx
	jnz iadbldlop2

	add rsi,r10 ;sAdd
	add rdi,r11 ;dAdd
	dec r9
	jnz iabbldlop
	
	pop rbx
	pop rbp
	ret
	
;void AlphaBlend8_C8_DoBlendPA(UInt8 *dest, OSInt dbpl, UInt8 *src, OSInt sbpl, OSInt width, OSInt height, UInt8 *rgbTable);

;0 ebx
;8 ebp
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

	xor eax,eax
	mov ebx,0x10101010
	
	movd xmm2,eax
	movd xmm4,ebx
	punpcklbw xmm4, xmm2
	mov r10,rcx ;sAdd
	mov r11,rsi ;dAdd
	mov rsi,rdx ;src

	mov rdx,qword [rsp+24] ;rgbTable
	ALIGN 16
iabbldpalop:
	mov rcx,r8 ;width
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
	or ebx,0xff000000
	movd xmm3,ebx

;	movzx rax,byte [rsi+2]
;	movzx rbx,word [rdx+rax*2+262144]
;	movd xmm2,ebx
;	movzx rax,byte [rsi+1]
;	movzx ebx,word [rdx+rax*2+262656]
;	shl ebx,16
;	movzx rax,byte [rsi]
;	mov bx,word [rdx+rax*2+263168]
;	movd xmm1,ebx
;	punpckldq xmm1,xmm2

;	movzx rax,byte [rdi+2]
;	movzx ebx,word [rdx+rax*2+263680]
;	movd xmm2,ebx
;	movzx rax,byte [rdi+1]
;	movzx ebx,word [rdx+rax*2+264192]
;	shl ebx,16
;	movzx rax,byte [rdi]
;	mov bx,word [rdx+rax*2+264704]
;	movd xmm0,ebx
;	punpckldq xmm0,xmm2

	movzx rax,byte [rsi+2]
	movq xmm1,[rdx+rax*8+262144]
	movzx rax,byte [rsi+1]
	movq xmm2,[rdx+rax*8+264192]
	paddsw xmm1,xmm2
	movzx rax,byte [rsi+0]
	movq xmm2,[rdx+rax*8+266240]
	paddsw xmm1,xmm2

	movzx rax,byte [rdi+2]
	movq xmm0,[rdx+rax*8+270336]
	movzx rax,byte [rdi+1]
	movq xmm2,[rdx+rax*8+272384]
	paddsw xmm0,xmm2
	movzx rax,byte [rdi+0]
	movq xmm2,[rdx+rax*8+274432]
	paddsw xmm0,xmm2

	punpcklbw xmm3, xmm3
	pmulhuw xmm0,xmm3
	paddusw xmm0,xmm1

	mov bh,0xff
	pextrw rax,xmm0,2
	mov bl,byte [rdx+rax+131072]
	shl ebx,16
	pextrw rax,xmm0,0
	mov bl,byte [rdx+rax]
	pextrw rax,xmm0,1
	mov bh,byte [rdx+rax+65536]
	mov dword [rdi],ebx

	lea rsi,[rsi+4]
	lea rdi,[rdi+4]
	dec rcx
	jnz iadbldpalop2

	add rsi,r10 ;sAdd
	add rdi,r11 ;dAdd
	dec r9
	jnz iabbldpalop
	pop rbx
	pop rbp
	ret

