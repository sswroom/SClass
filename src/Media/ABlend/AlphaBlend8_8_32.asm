section .text

global _AlphaBlend8_8_DoBlend
global _AlphaBlend8_8_DoBlendPA

;void AlphaBlend8_8_DoBlend(UInt8 *dest, OSInt dbpl, UInt8 *src, OSInt sbpl, OSInt width, OSInt height)

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

_AlphaBlend8_8_DoBlend:
	push ebp
	push ebx
	push esi
	push edi
	mov eax,dword [esp+36] ;width
	shl eax,2
	sub dword [esp+32],eax ;sbpl
	sub dword [esp+24],eax ;dbpl
	test eax,15
	jnz iadbldstart
	test dword [esp+20],15 ;dest
	jnz iadbld4start
	test dword [esp+24],15 ;dbpl
	jnz iadbld4start
	jmp iadbld4astart
	
	align 16
iadbldstart:
	mov ebx,0x10101010
	
	pxor xmm2,xmm2
	movd xmm4,ebx
	punpcklbw xmm4, xmm2

	mov edi,dword [esp+20] ;dest
	mov esi,dword [esp+28] ;src
	mov edx,dword [esp+40] ;height
	ALIGN 16
iadbldlop:

	mov ecx,dword [esp+36] ;width
	align 16
iadbldlop2:
	mov bl,byte [esi+3]
	mov bh,bl
	movzx eax,bx
	shl ebx,16
	or eax,ebx

	movd xmm0,eax
	mov ebx,0xffffffff
	sub ebx,eax
	or ebx,0xff000000
	movd xmm3,ebx

	movd xmm1,dword [esi]
	punpcklbw xmm0, xmm2
	punpcklbw xmm1, xmm2
	pmullw xmm0,xmm1
	movdqa xmm1,xmm0

	movd xmm0,dword [edi]
	punpcklbw xmm3, xmm2
	punpcklbw xmm0, xmm2
	pmullw xmm0,xmm3
	paddusw xmm0,xmm1

	movdqa xmm1,xmm0
	psrlw xmm1,8
	paddusw xmm0,xmm1
	paddusw xmm0,xmm4

	psrlw xmm0,8
	packuswb xmm0,xmm2
	movd dword [edi],xmm0

	lea esi,[esi+4]
	lea edi,[edi+4]
	dec ecx
	jnz iadbldlop2

	add esi,dword [esp+32] ;sAdd
	add edi,dword [esp+24] ;dAdd
	dec edx
	jnz iadbldlop
	jmp iadbldexit

	align 16
iadbld4start:
	shr dword [esp+36],2 ;width
	mov ebx,0x10101010
	pxor xmm2,xmm2
	movd xmm4,ebx
	punpckldq xmm4, xmm4
	punpcklbw xmm4, xmm2

	mov ebx,0xffffffff
	movd xmm6,ebx
	mov ebx,0xff000000
	movd xmm7,ebx
	punpckldq xmm6,xmm6
	punpckldq xmm7,xmm7

	mov edi,dword [esp+20] ;dest
	mov esi,dword [esp+28] ;src
	mov edx,dword [esp+40] ;height
	ALIGN 16
iadbld4lop:
	mov ecx,dword [esp+36] ;width
	align 16
iadbld4lop2:
	movzx eax,byte [esi+3]
	mov ah,byte [esi+7]

	movd xmm0,eax
	punpcklbw xmm0,xmm0
	punpcklwd xmm0,xmm0

	movdqa xmm3,xmm6
	psubd xmm3,xmm0
	por xmm3,xmm7

	movq xmm1,[esi]
	punpcklbw xmm0, xmm2
	punpcklbw xmm1, xmm2
	pmullw xmm0,xmm1
	movdqa xmm1,xmm0

	movq xmm0,[edi]
	punpcklbw xmm3, xmm2
	punpcklbw xmm0, xmm2
	pmullw xmm0,xmm3
	paddusw xmm0,xmm1

	movdqa xmm1,xmm0
	psrlw xmm1,8
	paddusw xmm0,xmm1
	paddusw xmm0,xmm4

	movzx eax,byte [esi+11]
	mov ah,byte [esi+15]

	movd xmm5,eax
	punpcklbw xmm5,xmm5
	punpcklwd xmm5,xmm5

	movdqa xmm3,xmm6
	psubd xmm3,xmm5
	por xmm3,xmm7

	movq xmm1,[esi+8]
	punpcklbw xmm5,xmm2
	punpcklbw xmm1,xmm2
	pmullw xmm5,xmm1
	movdqa xmm1,xmm5

	movq xmm5,[edi+8]
	punpcklbw xmm3,xmm2
	punpcklbw xmm5,xmm2
	pmullw xmm5,xmm3
	paddusw xmm5,xmm1

	movdqa xmm1,xmm5
	psrlw xmm1,8
	paddusw xmm5,xmm1
	paddusw xmm5,xmm4

	psrlw xmm0,8
	psrlw xmm5,8
	packuswb xmm0,xmm5
	movdqu [edi],xmm0

	lea esi,[esi+16]
	lea edi,[edi+16]
	dec ecx
	jnz iadbld4lop2

	add esi,dword [esp+32] ;sAdd
	add edi,dword [esp+24] ;dAdd
	dec edx
	jnz iadbld4lop
	jmp iadbldexit
	
	align 16
iadbld4astart:
	shr dword [esp+36],2 ;width
	mov ebx,0x10101010
	pxor xmm2,xmm2
	movd xmm4,ebx
	punpckldq xmm4, xmm4
	punpcklbw xmm4, xmm2

	mov ebx,0xffffffff
	movd xmm6,ebx
	mov ebx,0xff000000
	movd xmm7,ebx
	punpckldq xmm6,xmm6
	punpckldq xmm7,xmm7

	mov edi,dword [esp+20] ;dest
	mov esi,dword [esp+28] ;src
	mov edx,dword [esp+40] ;height
	ALIGN 16
iadbld4alop:

	mov ecx,dword [esp+36] ;width
	align 16
iadbld4alop2:
	movzx eax,byte [esi+3]
	mov ah,byte [esi+7]

	movd xmm0,eax
	punpcklbw xmm0,xmm0
	punpcklwd xmm0,xmm0

	movdqa xmm3,xmm6
	psubd xmm3,xmm0
	por xmm3,xmm7

	movq xmm1,[esi]
	punpcklbw xmm0, xmm2
	punpcklbw xmm1, xmm2
	pmullw xmm0,xmm1
	movdqa xmm1,xmm0

	movq xmm0,[edi]
	punpcklbw xmm3, xmm2
	punpcklbw xmm0, xmm2
	pmullw xmm0,xmm3
	paddusw xmm0,xmm1

	movdqa xmm1,xmm0
	psrlw xmm1,8
	paddusw xmm0,xmm1
	paddusw xmm0,xmm4

	movzx eax,byte [esi+11]
	mov ah,byte [esi+15]

	movd xmm5,eax
	punpcklbw xmm5,xmm5
	punpcklwd xmm5,xmm5

	movdqa xmm3,xmm6
	psubd xmm3,xmm5
	por xmm3,xmm7

	movq xmm1,[esi+8]
	punpcklbw xmm5,xmm2
	punpcklbw xmm1,xmm2
	pmullw xmm5,xmm1
	movdqa xmm1,xmm5

	movq xmm5,[edi+8]
	punpcklbw xmm3,xmm2
	punpcklbw xmm5,xmm2
	pmullw xmm5,xmm3
	paddusw xmm5,xmm1

	movdqa xmm1,xmm5
	psrlw xmm1,8
	paddusw xmm5,xmm1
	paddusw xmm5,xmm4

	psrlw xmm0,8
	psrlw xmm5,8
	packuswb xmm0,xmm5
	movntdq [edi],xmm0

	lea esi,[esi+16]
	lea edi,[edi+16]
	dec ecx
	jnz iadbld4alop2

	add esi,dword [esp+32] ;sAdd
	add edi,dword [esp+24] ;dAdd
	dec edx
	jnz iadbld4alop

	align 16
iadbldexit:
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret
	
;void AlphaBlend8_8_DoBlendPA(UInt8 *dest, OSInt dbpl, UInt8 *src, OSInt sbpl, OSInt width, OSInt height)

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

_AlphaBlend8_8_DoBlendPA:
	push ebp
	push ebx
	push esi
	push edi
	xor eax,eax
	mov ebx,0x10101010
	
	movd xmm2,eax
	movd xmm4,ebx
	punpcklbw xmm4, xmm2
	mov eax,0xffffffff
	movd xmm5,eax
	punpcklbw xmm5,xmm2

	mov edx,dword [esp+40] ;height
	ALIGN 16
iabpabldlop:
	mov edi,dword [esp+20] ;dest
	mov esi,dword [esp+28] ;src

	mov ecx,dword [esp+36] ;width
	align 16
iadpabldlop2:
	mov bl,byte [esi+3]
	mov bh,bl
	movzx eax,bx
	shl ebx,16
	or eax,ebx

	movd xmm0,eax
	mov ebx,0xffffffff
	sub ebx,eax
	or ebx,0xff000000
	movd xmm3,ebx

	movd xmm1,dword [esi]
	punpcklbw xmm1, xmm2
	pmullw xmm1,xmm5

	movd xmm0,dword [edi]
	punpcklbw xmm3, xmm2
	punpcklbw xmm0, xmm2
	pmullw xmm0,xmm3
	paddusw xmm0,xmm1

	movq xmm1,xmm0
	psrlw xmm1,8
	paddusw xmm0,xmm1
	paddusw xmm0,xmm4

	psrlw xmm0,8
	packuswb xmm0,xmm2
	movd dword [edi],xmm0

	lea esi,[esi+4]
	lea edi,[edi+4]
	dec ecx
	jnz iadpabldlop2

	mov esi,dword [esp+32] ;sbpl
	mov edi,dword [esp+24] ;dbpl
	add dword [esp+28],esi ;src
	add dword [esp+20],edi ;dest
	dec edx
	jnz iabpabldlop
	
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret