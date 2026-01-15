section .text

global _RGBColorFilter_ProcessImagePart
global RGBColorFilter_ProcessImagePart
global _RGBColorFilter_ProcessImageHDRDLPart
global RGBColorFilter_ProcessImageHDRDLPart

;void RGBColorFilter_ProcessImagePart(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS sbpl, IntOS dbpl, UInt8 *lut, Int32 bpp)

;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 srcPtr
;24 destPtr
;28 width
;32 height
;36 sbpl
;40 dbpl
;44 lut
;48 bpp

	align 16
_RGBColorFilter_ProcessImagePart:
RGBColorFilter_ProcessImagePart:
	mov eax,dword [esp+32] ;bpp
	cmp eax,32
	jz _RGBColorFilter_ProcessImagePart32
	cmp eax,48
	jz _RGBColorFilter_ProcessImagePart48
	ret
	
	align 16
_RGBColorFilter_ProcessImagePart32:
	push ebp
	push ebx
	push esi
	push edi
	mov ebx,dword [esp+44] ;lut
	pxor xmm4,xmm4
	align 16
pip32_0:
	mov ecx,dword [esp+28] ;width
	mov esi,dword [esp+20] ;srcPtr
	mov edi,dword [esp+24] ;destPtr
	align 16
pip32_1:
	movzx eax,byte [esi]
	movsx edx,word [ebx+eax*2]
	pinsrw xmm4,edx,0
	movd xmm0,edx
	movdqa xmm2,xmm0
	movdqa xmm3,xmm0
	movzx eax,byte [esi+1]
	movsx edx,word [ebx+eax*2]
	pinsrw xmm4,edx,1
	movd xmm0,edx
	pminsw xmm2,xmm0
	pmaxsw xmm3,xmm0
	movzx eax,byte [esi+2]
	movsx edx,word [ebx+eax*2]
	pinsrw xmm4,edx,2
	mov al,byte [esi+3]
	movd xmm0,edx
	pminsw xmm2,xmm0
	pmaxsw xmm3,xmm0
	movd edx,xmm2 ;minV
	mov [edi+3],al
	cmp edx,8192
	jge pip32_5
	movd eax,xmm3 ;maxV
	cmp eax,8192
	jle pip32_4

	push ecx
	mov ecx,eax
	mov ebp,edx
	pextrw eax,xmm4,0
	movsx eax,ax
	mov edx,8192
	sub eax,ebp
	sub edx,ebp
	sub ecx,ebp
	imul edx
	idiv ecx
	add eax,ebp
	and eax,0xffff
	mov dl,byte [ebx+eax+512]
	mov byte [edi],dl

	pextrw eax,xmm4,1
	movsx eax,ax
	mov edx,8192
	sub eax,ebp
	sub edx,ebp
	imul edx
	idiv ecx
	add eax,ebp
	and eax,0xffff
	mov dl,byte [ebx+eax+512]
	mov byte [edi+1],dl

	pextrw eax,xmm4,2
	movsx eax,ax
	mov edx,8192
	sub eax,ebp
	sub edx,ebp
	imul edx
	idiv ecx
	add eax,ebp
	and eax,0xffff
	mov dl,byte [ebx+eax+512]
	mov byte [edi+2],dl

	pop ecx
	jmp pip32_6

	align 16
pip32_5:
	mov al,byte [ebx+8704]
	mov byte [edi],al
	mov byte [edi+1],al
	mov byte [edi+2],al
	jmp pip32_6

	align 16
pip32_4:
	pextrw eax,xmm4,0
	mov dl,byte [ebx+eax+512]
	mov byte [edi],dl
	pextrw eax,xmm4,1
	mov dl,byte [ebx+eax+512]
	mov byte [edi+1],dl
	pextrw eax,xmm4,2
	mov dl,byte [ebx+eax+512]
	mov byte [edi+2],dl

	align 16
pip32_6:
	add esi,4
	add edi,4
	dec ecx
	jnz pip32_1

	mov eax,dword [esp+36] ;sbpl
	mov edx,dword [esp+40] ;dbpl
	add dword [esp+20],eax ;srcPtr
	add dword [esp+24],edx ;destPtr
	dec dword [esp+32] ;height
	jnz pip32_0
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

	align 16
_RGBColorFilter_ProcessImagePart48:
	push ebp
	push ebx
	push esi
	push edi
	mov ebx,dword [esp+44] ;lut
	pxor xmm4,xmm4
	align 16
pip48_0:
	mov ecx,dword [esp+28] ;width
	mov esi,dword [esp+20] ;srcPtr
	mov edi,dword [esp+24] ;destPtr
	align 16
pip48_1:
	movzx eax,word [esi]
	movsx edx,word [ebx+eax*2]
	pinsrw xmm4,edx,0
	movd xmm0,edx
	movdqa xmm2,xmm0
	movdqa xmm3,xmm0
	movzx eax,word [esi+2]
	movsx edx,word [ebx+eax*2]
	pinsrw xmm4,edx,1
	movd xmm0,edx
	pminsw xmm2,xmm0
	pmaxsw xmm3,xmm0
	movzx eax,word [esi+4]
	movsx edx,word [ebx+eax*2]
	pinsrw xmm4,edx,2
	movd xmm0,edx
	pminsw xmm2,xmm0
	pmaxsw xmm3,xmm0
	movd edx,xmm2 ;minV
	cmp edx,8192
	jge pip48_5
	movd eax,xmm3 ;maxV
	cmp eax,8192
	jle pip48_4

	push ecx
	mov ecx,eax
	mov ebp,edx
	pextrw eax,xmm4,0
	movsx eax,ax
	mov edx,8192
	sub eax,ebp
	sub edx,ebp
	sub ecx,ebp
	imul edx
	idiv ecx
	add eax,ebp
	and eax,0xffff
	mov dx,word [ebx+eax*2+131072]
	mov word [edi],dx

	pextrw eax,xmm4,1
	movsx eax,ax
	mov edx,8192
	sub eax,ebp
	sub edx,ebp
	imul edx
	idiv ecx
	add eax,ebp
	and eax,0xffff
	mov dx,word [ebx+eax*2+131072]
	mov word [edi+2],dx

	pextrw eax,xmm4,2
	movsx eax,ax
	mov edx,8192
	sub eax,ebp
	sub edx,ebp
	imul edx
	idiv ecx
	add eax,ebp
	and eax,0xffff
	mov dx,word [ebx+eax*2+131072]
	mov word [edi+4],dx

	pop ecx
	jmp pip48_6

	align 16
pip48_5:
	mov ax,word [ebx+147456]
	mov word [edi],ax
	mov word [edi+2],ax
	mov word [edi+4],ax
	jmp pip48_6

	align 16
pip48_4:
	pextrw eax,xmm4,0
	mov dx,word [ebx+eax*2+131072]
	mov word [edi],dx
	pextrw eax,xmm4,1
	mov dx,word [ebx+eax*2+131072]
	mov word [edi+2],dx
	pextrw eax,xmm4,2
	mov dx,word [ebx+eax*2+131072]
	mov word [edi+4],dx
	align 16
pip48_6:
	add esi,6
	add edi,6
	dec ecx
	jnz pip48_1

	mov eax,dword [esp+36] ;sbpl
	mov edx,dword [esp+40] ;dbpl
	add dword [esp+20],eax ;srcPtr
	add dword [esp+24],edx ;destPtr
	dec dword [esp+32] ;height
	jnz pip48_0
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void RGBColorFilter_ProcessImageHDRDLPart(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS sbpl, IntOS dbpl, UInt8 *lut, Int32 bpp, Int32 hdrLev)

;0 currWidth
;4 edi
;8 esi
;12 ebx
;16 ebp
;20 retAddr
;24 srcPtr
;28 destPtr
;32 width
;36 height
;40 sbpl
;44 dbpl
;48 lut
;52 bpp
;56 hdrLev

	align 16
_RGBColorFilter_ProcessImageHDRDLPart:
RGBColorFilter_ProcessImageHDRDLPart:
	mov eax,dword [esp+32] ;bpp
	cmp eax,32
	jz _RGBColorFilter_ProcessImageHDRDLPart32
	cmp eax,48
	jz _RGBColorFilter_ProcessImageHDRDLPart48
	ret
	
	align 16
_RGBColorFilter_ProcessImageHDRDLPart32:
	push ecx
	push ebp
	push ebx
	push esi
	push edi
	mov ebx,dword [esp+48] ;lut
	mov ecx,dword [esp+56] ;hdrLev
	pxor xmm4,xmm4
	align 16
pihdlp32_0:
	mov ebp,dword [esp+32] ;width
	mov esi,dword [esp+24] ;srcPtr
	mov edi,dword [esp+28] ;destPtr
	mov dword [esp+0],ebp ;currWidth
	align 16
pihdlp32_1:
	movzx eax,byte [esi]
	mov edx,dword [ebx+eax*4+66048]
	pinsrd xmm4,edx,0
	movd xmm0,edx
	movdqa xmm2,xmm0
	movdqa xmm3,xmm0
	movzx eax,byte [esi+1]
	mov edx,dword [ebx+eax*4+66048]
	pinsrd xmm4,edx,1
	movd xmm0,edx
	pminsd xmm2,xmm0
	pmaxsd xmm3,xmm0
	movzx eax,byte [esi+2]
	mov edx,dword [ebx+eax*4+66048]
	pinsrd xmm4,edx,2
	mov al,byte [esi+3]
	movd xmm0,edx
	pminsd xmm2,xmm0
	pmaxsd xmm3,xmm0
	movd edx,xmm2 ;minV
	mov [edi+3],al
	sar edx,cl
	cmp edx,8192
	jge pihdlp32_5
	movd eax,xmm3 ;maxV
	cmp eax,8192
	jle pihdlp32_4

	push ecx
	mov ecx,eax
	mov ebp,edx
	pextrd eax,xmm4,0
	mov edx,8192
	sub eax,ebp
	sub edx,ebp
	sub ecx,ebp
	imul edx
	idiv ecx
	add eax,ebp
	and eax,0xffff
	mov dl,byte [ebx+eax+512]
	mov byte [edi],dl

	pextrd eax,xmm4,1
	mov edx,8192
	sub eax,ebp
	sub edx,ebp
	imul edx
	idiv ecx
	add eax,ebp
	and eax,0xffff
	mov dl,byte [ebx+eax+512]
	mov byte [edi+1],dl

	pextrd eax,xmm4,2
	mov edx,8192
	sub eax,ebp
	sub edx,ebp
	imul edx
	idiv ecx
	add eax,ebp
	and eax,0xffff
	mov dl,byte [ebx+eax+512]
	mov byte [edi+2],dl

	pop ecx
	jmp pihdlp32_6

	align 16
pihdlp32_5:
	mov al,byte [ebx+8704]
	mov byte [edi],al
	mov byte [edi+1],al
	mov byte [edi+2],al
	jmp pihdlp32_6

	align 16
pihdlp32_4:
	pextrd eax,xmm4,0
	and eax,0xffff
	mov dl,byte [ebx+eax+512]
	mov byte [edi],dl
	pextrd eax,xmm4,1
	and eax,0xffff
	mov dl,byte [ebx+eax+512]
	mov byte [edi+1],dl
	pextrd eax,xmm4,2
	and eax,0xffff
	mov dl,byte [ebx+eax+512]
	mov byte [edi+2],dl

	align 16
pihdlp32_6:
	add esi,4
	add edi,4
	dec dword [esp]
	jnz pihdlp32_1

	mov eax,dword [esp+40] ;sbpl
	mov edx,dword [esp+44] ;dbpl
	add dword [esp+24],eax ;srcPtr
	add dword [esp+28],edx ;destPtr
	dec dword [esp+36] ;height
	jnz pihdlp32_0
	pop edi
	pop esi
	pop ebx
	pop ebp
	pop ecx
	ret

	align 16
_RGBColorFilter_ProcessImageHDRDLPart48:
	push ecx
	push ebp
	push ebx
	push esi
	push edi
	mov ebx,dword [esp+48] ;lut
	mov ecx,dword [esp+56] ;hdrLev
	pxor xmm4,xmm4
	align 16
pihdlp48_0:
	mov ebp,dword [esp+32] ;width
	mov esi,dword [esp+24] ;srcPtr
	mov edi,dword [esp+28] ;destPtr
	mov dword [esp+0],ebp ;currWidth
	align 16
pihdlp48_1:
	movzx eax,word [esi]
	mov edx,dword [ebx+eax*4+262144]
	pinsrd xmm4,edx,0
	movd xmm0,edx
	movdqa xmm2,xmm0
	movdqa xmm3,xmm0
	movzx eax,word [esi+2]
	mov edx,dword [ebx+eax*4+262144]
	pinsrd xmm4,edx,1
	movd xmm0,edx
	pminsd xmm2,xmm0
	pmaxsd xmm3,xmm0
	movzx eax,word [esi+4]
	mov edx,dword [ebx+eax*4+262144]
	pinsrd xmm4,edx,2
	movd xmm0,edx
	pminsd xmm2,xmm0
	pmaxsd xmm3,xmm0
	movd edx,xmm2 ;minV
	sar edx,cl
	cmp edx,8192
	jge pihdlp48_5
	movd eax,xmm3 ;maxV
	cmp eax,8192
	jle pihdlp48_4

	push ecx
	mov ecx,eax
	mov ebp,edx
	pextrd eax,xmm4,0
	mov edx,8192
	sub eax,ebp
	sub edx,ebp
	sub ecx,ebp
	imul edx
	idiv ecx
	add eax,ebp
	and eax,0xffff
	mov dx,word [ebx+eax*2+131072]
	mov word [edi],dx

	pextrd eax,xmm4,1
	mov edx,8192
	sub eax,ebp
	sub edx,ebp
	imul edx
	idiv ecx
	add eax,ebp
	and eax,0xffff
	mov dx,word [ebx+eax*2+131072]
	mov word [edi+2],dx

	pextrd eax,xmm4,2
	mov edx,8192
	sub eax,ebp
	sub edx,ebp
	imul edx
	idiv ecx
	add eax,ebp
	and eax,0xffff
	mov dx,word [ebx+eax*2+131072]
	mov word [edi+4],dx

	pop ecx
	jmp pihdlp48_6

	align 16
pihdlp48_5:
	mov ax,word [ebx+147456]
	mov word [edi],ax
	mov word [edi+2],ax
	mov word [edi+4],ax
	jmp pihdlp48_6

	align 16
pihdlp48_4:
	pextrd eax,xmm4,0
	and eax,0xffff
	mov dx,word [ebx+eax*2+131072]
	mov word [edi],dx
	pextrd eax,xmm4,1
	and eax,0xffff
	mov dx,word [ebx+eax*2+131072]
	mov word [edi+2],dx
	pextrd eax,xmm4,2
	and eax,0xffff
	mov dx,word [ebx+eax*2+131072]
	mov word [edi+4],dx
	align 16
pihdlp48_6:
	add esi,6
	add edi,6
	dec dword [esp]
	jnz pihdlp48_1

	mov eax,dword [esp+40] ;sbpl
	mov edx,dword [esp+44] ;dbpl
	add dword [esp+24],eax ;srcPtr
	add dword [esp+28],edx ;destPtr
	dec dword [esp+36] ;height
	jnz pihdlp48_0
	pop edi
	pop esi
	pop ebx
	pop ebp
	pop ecx
	ret
