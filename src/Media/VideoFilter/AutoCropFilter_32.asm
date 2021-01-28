section .text

global _AutoCropFilter_CropCalc

;void AutoCropFilter_CropCalc(UInt8 *yptr, OSInt w, OSInt h, OSInt ySplit, OSInt *crops)
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 yptr
;24 w
;28 h
;32 ySplit
;36 crops

	align 16
_AutoCropFilter_CropCalc:
	push ebp
	push ebx
	push esi
	push edi
	
	mov edi,dword [esp+36] ;crops
	mov esi,dword [esp+20] ;yptr
	xor edx,edx
	mov ebx,dword [esp+32] ;ySplit
	mov ebp,dword [edi+4] ;cropTop
	test ebp,ebp
	jz cblop0
	align 16
ctlop:
	mov ecx,dword [esp+24] ;w
	align 16
ctlop2:
	cmp byte [esi],24
	jnb ctlop3
	add esi,ebx
	dec ecx
	jnz ctlop2
	inc edx
	cmp edx,ebp
	jb ctlop
	align 16
ctlop3:
	mov dword [edi+4],edx ;cropTop

	align 16
cblop0:
	mov esi,dword [esp+20] ;yptr
	mov eax,dword [esp+24] ;w
	mul ebx
	mov ebp,eax
	mul dword [esp+28] ;h
	add esi,eax

	xor edx,edx
	cmp dword [edi+12],0 ;cropBottom
	jz cllop
	align 16
cblop:
	sub esi,ebp
	mov ecx,dword [esp+24] ;w
	mov eax,esi
	align 16
cblop2:
	cmp byte [eax],24
	jnb cblop3
	add eax,ebx
	dec ecx
	jnz cblop2
	inc edx
	cmp edx,dword [edi+12] ;cropBottom
	jb cblop
	jmp cllop
	align 16
cblop3:
	mov dword [edi+12],edx ;cropBottom

	align 16
cllop:
	xor edx,edx
	cmp dword [edi],0 ;cropLeft
	jz crlop0
	mov eax,dword [esp+24] ;w
	mul ebx
	align 16
cllop2:
	mov esi,dword [esp+20] ;yptr
	add esi,edx
	mov ecx,dword [esp+28] ;h
	align 16
cllop3:
	cmp byte [esi],24
	jnb cllop4
	add esi,eax
	dec ecx
	jnz cllop3
	inc edx
	cmp edx,dword [edi] ;cropLeft
	jb cllop2
	align 16
cllop4:
	mov dword [edi],edx ;cropLeft

;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 yptr
;24 w
;28 h
;32 ySplit
;36 crops

	align 16
crlop0:
	mov eax,dword [esp+24] ;w
	mov esi,dword [esp+20] ;yptr
	mul ebx
	add esi,eax
	xor edx,edx
	cmp dword [edi+8],0 ;cropRight
	jz crExit
	align 16
crlop:
	sub esi,dword [esp+32] ;ySplit
	mov ecx,dword [esp+28] ;h
	mov ebx,esi
	align 16
crlop2:
	cmp byte [ebx],24
	jnb crlop3
	add ebx,eax
	dec ecx
	jnz crlop2
	inc edx
	cmp edx,dword [edi+8] ;cropRight
	jb crlop
	jmp crExit
	align 16
crlop3:
	mov dword [edi+8],edx ;cropRight
	
	align 16
crExit:

	pop edi
	pop esi
	pop ebx
	pop ebp
	ret
