section .text

global _MediaPlayer_VideoCropImageY

;_MediaPlayer_VideoCropImageY(UInt8 *yptr, OSInt w, OSInt h, OSInt ySplit, OSInt *crops)

;0 edi
;4 ebx
;8 esi
;12 ebp
;16 retAddr
;20 yptr
;24 w
;28 h
;32 ySplit
;36 crops

_MediaPlayer_VideoCropImageY:
	push ebp
	push esi
	push ebx
	push edi
	mov esi,dword [esp+20] ;yptr
	mov ebp,dword [esp+36] ;crops
	xor edx,edx
	mov ebx,dword [esp+32] ;ySplit
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
	cmp edx, dword [esp+28] ;h
	jb ctlop
	jmp cropError
	
	align 16
ctlop3:
	mov dword [ebp+4],edx ;cropTop
	mov esi,dword [esp+20] ;yptr
	mov eax,dword [esp+24] ;w
	mul dword [esp+32] ; ySplit
	mov edi,eax
	mul dword [esp+28] ; h
	add esi,eax
	xor edx,edx
	align 16
cblop:
	sub esi,edi
	mov ecx,dword [esp+24] ;w
	push esi
	align 16
cblop2:
	cmp byte [esi],24
	jnb cblop3
	add esi,ebx
	dec ecx
	jnz cblop2
	pop esi
	inc edx
	jmp cblop
	align 16
cblop3:
	pop esi
	mov dword [ebp+12],edx ;cropBottom

	xor edx,edx
	align 16
cblop4b:
	mov esi,dword [esp+20] ;yptr
	add esi,edx
	mov ecx,dword [esp+28] ;h
	align 16
cblop4:
	cmp byte [esi],24
	jnb cblop5
	add esi,dword [esp+24] ;w
	dec ecx
	jnz cblop4
	inc edx
	cmp edx,dword [esp+24] ;w
	jb cblop4b
	align 16
cblop5:
	mov dword [ebp],edx ;cropLeft
	
	mov edx,dword [esp+24] ;w
	dec edx
	align 16
cblop6b:
	mov esi,dword [esp+20] ;yptr
	add esi,edx
	mov ecx,dword [esp+28] ;h
	align 16
cblop6:
	cmp byte [esi],24
	jnb cblop7
	add esi,dword [esp+28] ;w
	dec ecx
	jnz cblop6
	dec edx
	cmp edx,dword [ebp] ;cropLeft
	ja cblop6b
	align 16
cblop7:
	mov eax,dword [esp+24] ;w
	sub eax,edx
	dec eax
	mov dword [ebp+8],eax ;cropRight

	jmp cropExit
cropError:
	mov dword [ebp],0 ;cropLeft
	mov dword [ebp+4],0 ;cropTop
	mov dword [ebp+8],0 ;cropRight
	mov dword [ebp+12],0 ;cropBottom
cropExit:
	pop edi
	pop ebx
	pop esi
	pop ebp
	ret

