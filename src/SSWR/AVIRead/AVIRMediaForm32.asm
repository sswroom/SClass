section .text

global _AVIRMediaForm_VideoCropImageY

;_AVIRMediaForm_VideoCropImageY(UInt8 *yptr, IntOS w, IntOS h, IntOS ySplit, IntOS *crops)

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

_AVIRMediaForm_VideoCropImageY:
	push ebp
	push esi
	push ebx
	push edi
	mov ebp,dword [esp+36] ;;crops
	mov esi,dword [esp+20] ;yptr
	xor edx,edx
	mov ebx,dword [esp+32] ;ySplit
	align 16
ctlop:
	mov ecx,dword [esp+24] ;w
	align 16
ctlop2:
	cmp byte [esi],18
	jnb ctlop3
	add esi,ebx
	dec ecx
	jnz ctlop2
	inc edx
	cmp edx,dword [esp+28] ;h
	jb ctlop
	jmp cropError
ctlop3:
	mov dword [ebp+4],edx ;cropTop
	mov esi,dword [esp+20] ;yptr
	mov eax,dword [esp+24] ;w
	mul dword [esp+32] ;ySplit
	mov edi,eax
	mul dword [esp+28] ;h
	add esi,eax
	xor edx,edx
	align 16
cblop:
	sub esi,edi
	mov ecx,dword [esp+24] ;w
	push esi
cblop2:
	cmp byte [esi],18
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
	mov dword [ebp],0 ;cropLeft
	mov dword [ebp+8],0 ;cropRight
	jmp cropExit
	align 16
cropError:
	mov dword [ebp],0 ;cropLeft
	mov dword [ebp+4],0 ;cropTop
	mov dword [ebp+8],0 ;cropRight
	mov dword [ebp+12],0 ;cropBottom
	align 16
cropExit:
	pop edi
	pop ebx
	pop esi
	pop ebp
	ret

