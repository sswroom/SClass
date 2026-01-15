section .text

global _LRGBLimiter_LimitImageLRGB
global LRGBLimiter_LimitImageLRGB

;void LRGBLimiter_LimitImageLRGB(UInt8 *imgPtr, IntOS w, IntOS h);
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 imgPtr
;24 w
;28 h

	align 16
_LRGBLimiter_LimitImageLRGB:
LRGBLimiter_LimitImageLRGB:
	push ebp
	push ebx
	push esi
	push edi
	mov eax,dword [esp+24] ;w
	mov esi,dword [esp+20] ;imgPtr
	mul dword [esp+28] ;h
	mov ebp,eax
	ALIGN 16
lilrgblop:
	movsx eax,word [esi]
	mov edx,eax ;minV
	mov ebx,eax ;maxV
	movsx eax,word [esi+2]
	cmp edx,eax
	cmovg edx,eax
	cmp ebx,eax
	cmovl ebx,eax
	movsx eax,word [esi+4]
	cmp edx,eax
	cmovg edx,eax
	cmp ebx,eax
	cmovl ebx,eax

	cmp edx,16384
	jge lilrgblop5
	cmp ebx,16384
	jle lilrgblop4

	mov ecx,ebx
	mov edi,edx
	movsx eax,word [esi]
	mov edx,16384
	sub eax,edi
	sub edx,edi
	sub ecx,edi
	imul edx
	idiv ecx
	add eax,edi
	mov word [esi],ax

	movsx eax,word [esi+2]
	mov edx,16384
	sub eax,edi
	sub edx,edi
	imul edx
	idiv ecx
	add eax,edi
	mov word [esi+2],ax

	movsx eax,word [esi+4]
	mov edx,16384
	sub eax,edi
	sub edx,edi
	imul edx
	idiv ecx
	add eax,edi
	mov word [esi+4],ax
	jmp lilrgblop4
	ALIGN 16
lilrgblop5:
	mov word [esi],16383
	mov word [esi+2],16383
	mov word [esi+4],16383

	ALIGN 16
lilrgblop4:
	add esi,8
	dec ebp
	jnz lilrgblop
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret