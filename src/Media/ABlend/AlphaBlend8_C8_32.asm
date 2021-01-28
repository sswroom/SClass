section .text

global _AlphaBlend8_C8_DoBlend
global _AlphaBlend8_C8_DoBlendPA

;void AlphaBlend8_C8_DoBlend(UInt8 *dest, OSInt dbpl, UInt8 *src, OSInt sbpl, OSInt width, OSInt height, UInt8 *rgbTable);

;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 dest
;24 dbpl / dAdd
;28 src
;32 sbpl / sAdd
;36 width
;40 height
;44 rgbTable

_AlphaBlend8_C8_DoBlend:
	push ebp
	push ebx
	push esi
	push edi
	mov edx,dword [esp+36] ;width
	shl edx,2
	sub dword [esp+32],edx ;sAdd		OSInt sAdd = sbpl - width * 4;
	sub dword [esp+24],edx ;dAdd		OSInt dAdd = dbpl - width * 4;
	xor eax,eax
	mov ebx,0x10101010
	
	movd xmm2,eax
	movd xmm4,ebx
	punpcklbw xmm4, xmm2

	mov edi,dword [esp+20] ;dest
	mov esi,dword [esp+28] ;src
	mov ebp,dword [esp+40] ;height
	mov edx,dword [esp+44] ;rgbTable
	ALIGN 16
iabbldlop:
	mov ecx,dword [esp+36] ;width
	align 16
iadbldlop2:
	mov al,byte [esi+3]
	mov ah,al
	movzx ebx,ax
	shl eax,16
	or eax,ebx

	movd xmm0,eax
	mov ebx,0xffffffff
	sub ebx,eax
	or ebx,0xff000000
	movd xmm3,ebx

;	movzx ebx,byte [esi+3]
;	mov bh,bl
;	movzx eax,byte [esi+2]
;	shl ebx,16
;	mov bx,word [edx+eax*2+262144]
;	movd xmm2,ebx
;	movzx eax,byte [esi+1]
;	movzx ebx,word [edx+eax*2+262656]
;	shl ebx,16
;	movzx eax,byte [esi]
;	mov bx,word [edx+eax*2+263168]
;	movd xmm1,ebx
;	punpckldq xmm1,xmm2
	movzx eax,byte [esi+2]
	movq xmm1,[edx+eax*8+262144]
	movzx eax,byte [esi+1]
	movq xmm2,[edx+eax*8+264192]
	paddsw xmm1,xmm2
	movzx eax,byte [esi+0]
	movq xmm2,[edx+eax*8+266240]
	paddsw xmm1,xmm2
	movzx eax,byte [esi+3]
	movq xmm2,[edx+eax*8+268288]
	paddsw xmm1,xmm2

	punpcklbw xmm0,xmm0
	pmulhuw xmm1,xmm0

;	movzx ebx,byte [edi+3]
;	mov bh,bl
;	shl ebx,16
;	movzx eax,byte [edi+2]
;	mov bx,word [edx+eax*2+263680]
;	movd xmm2,ebx
;	movzx eax,byte [edi+1]
;	movzx ebx,word [edx+eax*2+264192]
;	shl ebx,16
;	movzx eax,byte [edi]
;	mov bx,word [edx+eax*2+264704]
;	movd xmm0,ebx
;	punpckldq xmm0,xmm2
	movzx eax,byte [edi+2]
	movq xmm0,[edx+eax*8+270336]
	movzx eax,byte [edi+1]
	movq xmm2,[edx+eax*8+272384]
	paddsw xmm0,xmm2
	movzx eax,byte [edi+0]
	movq xmm2,[edx+eax*8+274432]
	paddsw xmm0,xmm2
	movzx eax,byte [edi+3]
	movq xmm2,[edx+eax*8+276480]
	paddsw xmm0,xmm2

	punpcklbw xmm3,xmm3
	pmulhuw xmm0,xmm3
	paddusw xmm0,xmm1

	pextrw ebx,xmm0,3
	pextrw eax,xmm0,2
	mov bl,byte [edx+eax+131072]
	shl ebx,16
	pextrw eax,xmm0,0
	mov bl,byte [edx+eax]
	pextrw eax,xmm0,1
	mov bh,byte [edx+eax+65536]
	mov dword [edi],ebx

	lea esi,[esi+4]
	lea edi,[edi+4]
	dec ecx
	jnz iadbldlop2

	add esi,dword [esp+32] ;sAdd
	add edi,dword [esp+24] ;dAdd
	dec ebp
	jnz iabbldlop
	
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret
	
;void AlphaBlend8_C8_DoBlendPA(UInt8 *dest, OSInt dbpl, UInt8 *src, OSInt sbpl, OSInt width, OSInt height, UInt8 *rgbTable);

;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 dest
;24 dbpl
;28 src
;32 sbpl
;36 width
;40 height
;44 rgbTable

_AlphaBlend8_C8_DoBlendPA:
	push ebp
	push ebx
	push esi
	push edi
	mov edx,dword [esp+36] ;width
	shl edx,2
	sub dword [esp+32],edx ;sAdd		OSInt sAdd = sbpl - width * 4;
	sub dword [esp+24],edx ;dAdd		OSInt dAdd = dbpl - width * 4;

	xor eax,eax
	mov ebx,0x10101010
	
	movd xmm2,eax
	movd xmm4,ebx
	punpcklbw xmm4, xmm2

	mov edi,dword [esp+20] ;dest
	mov esi,dword [esp+28] ;src
	mov ebp,dword [esp+40] ;height
	mov edx,dword [esp+44] ;rgbTable
	ALIGN 16
iabbldpalop:
	mov ecx,dword [esp+36] ;width
	align 16
iadbldpalop2:
	mov al,byte [esi+3]
	mov ah,al
	push ax
	shl eax,16
	pop ax

	movd xmm0,eax
	mov ebx,0xffffffff
	sub ebx,eax
	or ebx,0xff000000
	movd xmm3,ebx

;	movzx eax,byte [esi+2]
;	movzx ebx,word [edx+eax*2+262144]
;	movd xmm2,ebx
;	movzx eax,byte [esi+1]
;	movzx ebx,word [edx+eax*2+262656]
;	shl ebx,16
;	movzx eax,byte [esi]
;	mov bx,word [edx+eax*2+263168]
;	movd xmm1,ebx
;	punpckldq xmm1,xmm2

;	movzx eax,byte [edi+2]
;	movzx ebx,word [edx+eax*2+263680]
;	movd xmm2,ebx
;	movzx eax,byte [edi+1]
;	movzx ebx,word [edx+eax*2+264192]
;	shl ebx,16
;	movzx eax,byte [edi]
;	mov bx,word [edx+eax*2+264704]
;	movd xmm0,ebx
;	punpckldq xmm0,xmm2

	movzx eax,byte [esi+2]
	movq xmm1,[edx+eax*8+262144]
	movzx eax,byte [esi+1]
	movq xmm2,[edx+eax*8+264192]
	paddsw xmm1,xmm2
	movzx eax,byte [esi+0]
	movq xmm2,[edx+eax*8+266240]
	paddsw xmm1,xmm2

	movzx eax,byte [edi+2]
	movq xmm0,[edx+eax*8+270336]
	movzx eax,byte [edi+1]
	movq xmm2,[edx+eax*8+272384]
	paddsw xmm0,xmm2
	movzx eax,byte [edi+0]
	movq xmm2,[edx+eax*8+274432]
	paddsw xmm0,xmm2

	punpcklbw xmm3, xmm3
	pmulhuw xmm0,xmm3
	paddusw xmm0,xmm1

	mov bh,0xff
	pextrw eax,xmm0,2
	mov bl,byte [edx+eax+131072]
	shl ebx,16
	pextrw eax,xmm0,0
	mov bl,byte [edx+eax]
	pextrw eax,xmm0,1
	mov bh,byte [edx+eax+65536]
	mov dword [edi],ebx

	lea esi,[esi+4]
	lea edi,[edi+4]
	dec ecx
	jnz iadbldpalop2

	add esi,dword [esp+32] ;sAdd
	add edi,dword [esp+24] ;dAdd
	dec ebp
	jnz iabbldpalop
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret
