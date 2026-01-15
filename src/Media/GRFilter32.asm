section .text

global _GRFilter_ProcessLayer32H
global GRFilter_ProcessLayer32H
global _GRFilter_ProcessLayer32V
global GRFilter_ProcessLayer32V
global _GRFilter_ProcessLayer32HV
global GRFilter_ProcessLayer32HV
global _GRFilter_ProcessLayer64H
global GRFilter_ProcessLayer64H
global _GRFilter_ProcessLayer64V
global GRFilter_ProcessLayer64V
global _GRFilter_ProcessLayer64HV
global GRFilter_ProcessLayer64HV

;void GRFilter_ProcessLayer32H(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS sbpl, IntOS dbpl, IntOS level, IntOS hOfst, IntOS vOfst);
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 srcPtr
;24 destPtr
;28 width
;32 height
;36 sbpl / sAdd
;40 dbpl / dAdd
;44 level
;48 hOfst
;52 vOfst

	align 16
_GRFilter_ProcessLayer32H:
GRFilter_ProcessLayer32H:
	push ebp
	push ebx
	push esi
	push edi
	
	; targetOfst ebp
	; loopX ecx
	; loopY ebx
	xor ebp,ebp
	mov esi,dword [esp+20] ;srcPtr
	mov edi,dword [esp+24] ;destPtr
	mov ecx,dword [esp+28] ;width
	mov ebx,dword [esp+32] ;height
	add esi,dword [esp+36] ;sbpl
	add edi,dword [esp+40] ;dbpl
	dec ecx
	dec ebx
	add esi,4
	add edi,4
	mov eax,dword [esp+48] ;hOfst
	cmp eax,0
	jge pl32hloph
	lea ebp,[ebp+eax*4]
	lea ecx,[ecx+eax+1]
	neg eax
	lea esi,[esi+eax*4-4]
	sub edi,4
	jmp pl32hloph2
	
	align 16
pl32hloph:
	lea ebp,[ebp+eax*4]
	sub ecx,eax
	lea edi,[edi+eax*4]
	
	align 16
pl32hloph2:
	mov eax,dword [esp+52] ;vOfst
	cmp eax,0
	jge pl32hlopv

	lea ebx,[ebx+eax+1]
	mov edx,dword [esp+40] ;dbpl
	sub edi,edx
	mul edx
	add ebp,eax
	mov eax,1
	add eax,dword [esp+52] ;vOfst
	mul dword [esp+36] ;sbpl
	sub esi,eax
	jmp pl32hlopv2
	
	align 16
pl32hlopv:	
	sub ebx,eax
	mul dword [esp+40]
	add ebp,eax
	add edi,eax

	align 16
pl32hlopv2:
	lea eax,[ecx*4]
	sub dword [esp+36],eax
	sub dword [esp+40],eax

	movd xmm4,dword [esp+44] ;level
	punpcklwd xmm4,xmm4
	punpcklwd xmm4,xmm4
	pxor xmm5,xmm5
	psllw xmm4,8
	align 16
pl32hlop:
	mov eax,ecx
	align 16
pl32hlop2:
	movd xmm0,[esi]
	movd xmm2,[esi-4]
	movd xmm3,[edi]
	punpcklbw xmm0,xmm5
	punpcklbw xmm2,xmm5
	punpcklbw xmm3,xmm5
	psubsw xmm0,xmm2
	pmulhw xmm0,xmm4
	paddsw xmm3,xmm0
	packuswb xmm3,xmm5
	movd [edi],xmm3
	lea esi,[esi+4]
	lea edi,[edi+4]
	dec eax
	jnz pl32hlop2
	add esi,dword [esp+36] ;sAdd
	add edi,dword [esp+40] ;dAdd
	dec ebx
	jnz pl32hlop

	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void GRFilter_ProcessLayer32V(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS sbpl, IntOS dbpl, IntOS level, IntOS hOfst, IntOS vOfst);
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
;44 level
;48 hOfst
;52 vOfst

	align 16
_GRFilter_ProcessLayer32V:
GRFilter_ProcessLayer32V:
	push ebp
	push ebx
	push esi
	push edi

	; targetOfst ebp
	; loopX ecx
	; loopY ebx
	xor ebp,ebp
	mov esi,dword [esp+20] ;srcPtr
	mov edi,dword [esp+24] ;destPtr
	mov ecx,dword [esp+28] ;width
	mov ebx,dword [esp+32] ;height
	add esi,dword [esp+36] ;sbpl
	add edi,dword [esp+40] ;dbpl
	dec ecx
	dec ebx
	add esi,4
	add edi,4
	mov eax,dword [esp+48] ;hOfst
	cmp eax,0
	jge pl32vloph
	lea ebp,[ebp+eax*4]
	lea ecx,[ecx+eax+1]
	neg eax
	lea esi,[esi+eax*4-4]
	sub edi,4
	jmp pl32vloph2
	
	align 16
pl32vloph:
	lea ebp,[ebp+eax*4]
	sub ecx,eax
	lea edi,[edi+eax*4]
	
	align 16
pl32vloph2:
	mov eax,dword [esp+52] ;vOfst
	cmp eax,0
	jge pl32vlopv

	lea ebx,[ebx+eax+1]
	mov edx,dword [esp+40] ;dbpl
	sub edi,edx
	mul edx
	add ebp,eax
	mov eax,1
	add eax,dword [esp+52] ;vOfst
	mul dword [esp+36] ;sbpl
	sub esi,eax
	jmp pl32vlopv2
	
	align 16
pl32vlopv:	
	sub ebx,eax
	mul dword [esp+40]
	add ebp,eax
	add edi,eax

	align 16
pl32vlopv2:
	lea eax,[ecx*4]
	mov edx,dword [esp+36] ;sbpl
	sub dword [esp+36],eax
	sub dword [esp+40],eax

	neg edx
	movd xmm4,dword [esp+44] ;level
	punpcklwd xmm4,xmm4
	punpcklwd xmm4,xmm4
	pxor xmm5,xmm5
	psllw xmm4,8
	align 16
pl32vlop:
	mov eax,ecx
	align 16
pl32vlop2:
	movd xmm0,[esi]
	movd xmm1,[esi+edx]
	movd xmm3,[edi]
	punpcklbw xmm0,xmm5
	punpcklbw xmm1,xmm5
	punpcklbw xmm3,xmm5
	psubsw xmm0,xmm1
	pmulhw xmm0,xmm4
	paddsw xmm3,xmm0
	packuswb xmm3,xmm5
	movd [edi],xmm3
	lea esi,[esi+4]
	lea edi,[edi+4]
	dec eax
	jnz pl32vlop2
	add esi,dword [esp+36] ;sAdd
	add edi,dword [esp+40] ;dAdd
	dec ebx
	jnz pl32vlop

	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void GRFilter_ProcessLayer32HV(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS sbpl, IntOS dbpl, IntOS level, IntOS hOfst, IntOS vOfst);
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
;44 level
;48 hOfst
;52 vOfst

	align 16
_GRFilter_ProcessLayer32HV:
GRFilter_ProcessLayer32HV:
	push ebp
	push ebx
	push esi
	push edi

	; targetOfst ebp
	; loopX ecx
	; loopY ebx
	xor ebp,ebp
	mov esi,dword [esp+20] ;srcPtr
	mov edi,dword [esp+24] ;destPtr
	mov ecx,dword [esp+28] ;width
	mov ebx,dword [esp+32] ;height
	add esi,dword [esp+36] ;sbpl
	add edi,dword [esp+40] ;dbpl
	dec ecx
	dec ebx
	add esi,4
	add edi,4
	mov eax,dword [esp+48] ;hOfst
	cmp eax,0
	jge pl32hvloph
	lea ebp,[ebp+eax*4]
	lea ecx,[ecx+eax+1]
	neg eax
	lea esi,[esi+eax*4-4]
	sub edi,4
	jmp pl32hvloph2
	
	align 16
pl32hvloph:
	lea ebp,[ebp+eax*4]
	sub ecx,eax
	lea edi,[edi+eax*4]
	
	align 16
pl32hvloph2:
	mov eax,dword [esp+52] ;vOfst
	cmp eax,0
	jge pl32hvlopv

	lea ebx,[ebx+eax+1]
	mov edx,dword [esp+40] ;dbpl
	sub edi,edx
	mul edx
	add ebp,eax
	mov eax,1
	add eax,dword [esp+52] ;vOfst
	mul dword [esp+36] ;sbpl
	sub esi,eax
	jmp pl32hvlopv2
	
	align 16
pl32hvlopv:	
	sub ebx,eax
	mul dword [esp+40]
	add ebp,eax
	add edi,eax

	align 16
pl32hvlopv2:
	lea eax,[ecx*4]
	mov edx,dword [esp+36] ;sbpl
	sub dword [esp+36],eax
	sub dword [esp+40],eax

	neg edx
	movd xmm4,dword [esp+44] ;level
	punpcklwd xmm4,xmm4
	punpcklwd xmm4,xmm4
	pxor xmm5,xmm5
	psllw xmm4,8
	align 16
pl32hvlop:
	mov eax,ecx
	align 16
pl32hvlop2:
	movd xmm0,[esi]
	movd xmm1,[esi+edx]
	movd xmm2,[esi-4]
	movd xmm3,[edi]
	punpcklbw xmm0,xmm5
	punpcklbw xmm1,xmm5
	punpcklbw xmm2,xmm5
	punpcklbw xmm3,xmm5
	paddsw xmm0,xmm0
	psubsw xmm0,xmm1
	psubsw xmm0,xmm2
	pmulhw xmm0,xmm4
	paddsw xmm3,xmm0
	packuswb xmm3,xmm5
	movd [edi],xmm3
	lea esi,[esi+4]
	lea edi,[edi+4]
	dec eax
	jnz pl32hvlop2
	add esi,dword [esp+36] ;sAdd
	add edi,dword [esp+40] ;dAdd
	dec ebx
	jnz pl32hvlop
	
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void GRFilter_ProcessLayer64H(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS sbpl, IntOS dbpl, IntOS level, IntOS hOfst, IntOS vOfst);
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
;44 level
;48 hOfst
;52 vOfst

	align 16
_GRFilter_ProcessLayer64H:
GRFilter_ProcessLayer64H:
	ret

;void GRFilter_ProcessLayer64V(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS sbpl, IntOS dbpl, IntOS level, IntOS hOfst, IntOS vOfst);
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
;44 level
;48 hOfst
;52 vOfst

	align 16
_GRFilter_ProcessLayer64V:
GRFilter_ProcessLayer64V:
	ret

;void GRFilter_ProcessLayer64HV(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS sbpl, IntOS dbpl, IntOS level, IntOS hOfst, IntOS vOfst);
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
;44 level
;48 hOfst
;52 vOfst

	align 16
_GRFilter_ProcessLayer64HV:
GRFilter_ProcessLayer64HV:
	ret