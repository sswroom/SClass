section .text

global _IVTCFilter_CalcField
global _IVTCFilter_CalcFieldP

;void IVTCFilter_CalcField(UInt8 *oddPtr, UInt8 *evenPtr, IntOS w, IntOS h, Int32 *fieldStats)
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 oddPtr
;24 evenPtr
;28 w
;32 h
;36 fieldStats

	align 16
_IVTCFilter_CalcField:
	push ebp
	push ebx
	push esi
	push edi
	mov eax,dword [esp+32] ;h
	sub eax,2
	shr eax,1
	mov esi,dword [esp+20] ;oddPtr
	mov edx,dword [esp+24] ;evenPtr
	mov dword [esp+32],eax ;hLeft
	pxor xmm7,xmm7
	pxor xmm5,xmm5 ;fieldMDiff
	pxor xmm6,xmm6 ;field2MDiff
	align 16
cfslop:
	mov ebp,dword [esp+28] ;w
	shr ebp,4
	align 16
cfslop2:
	mov edi,dword [esp+28] ;w
	movdqu xmm0,[esi]
	movdqu xmm1,[esi+edi*2]
	movdqu xmm3,[edx+edi]
	movdqu xmm2,xmm0
	pmaxub xmm0,xmm1
	pminub xmm1,xmm2
	movdqu xmm2,xmm3
	pmaxub xmm3,xmm0
	pminub xmm2,xmm1
	psubusb xmm3,xmm0
	movdqu xmm4,xmm1
	psubusb xmm4,xmm2
	paddb xmm3,xmm4 ;v4
	movdqu xmm2,xmm3
	psubusb xmm0,xmm1
	psubusb xmm2,xmm0 ;v5
	pmaxub xmm5,xmm3
	pmaxub xmm6,xmm2

	xor ecx,ecx ;fieldDiff
	xor ebx,ebx ;fieldCnt
	pextrw eax,xmm3,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,1
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,2
	shr eax,16
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,3
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,4
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,5
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,6
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,7
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0

	movd xmm4,ecx
	movd xmm1,ebx
	punpckldq xmm4,xmm1

	xor ecx,ecx ;field2Diff
	xor ebx,ebx ;field2Cnt

	pextrw eax,xmm2,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,1
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,2
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,3
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,4
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,5
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,6
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,7
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0

	movd xmm0,ecx
	movd xmm1,ebx
	punpckldq xmm0,xmm1
	punpcklqdq xmm4,xmm0
	paddd xmm7,xmm4

	mov edi,dword [esp+28] ;w
	lea eax,[edi*2+edi]
	movdqu xmm0,[edx+edi]
	movdqu xmm1,[edx+eax]
	movdqu xmm3,[esi+edi*2]
	movdqu xmm2,xmm0
	pmaxub xmm0,xmm1
	pminub xmm1,xmm2
	movdqu xmm2,xmm3
	pmaxub xmm3,xmm0
	pminub xmm2,xmm1
	psubusb xmm3,xmm0
	movdqu xmm4,xmm1
	psubusb xmm4,xmm2
	paddb xmm3,xmm4 ;v4
	movdqu xmm2,xmm3
	psubusb xmm0,xmm1
	psubusb xmm2,xmm0 ;v5
	pmaxub xmm5,xmm3
	pmaxub xmm6,xmm2

	xor ecx,ecx ;fieldDiff
	xor ebx,ebx ;fieldCnt
	pextrw eax,xmm3,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,1
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,2
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,3
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,4
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,5
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,6
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,7
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0

	movd xmm4,ecx
	movd xmm1,ebx
	punpckldq xmm4,xmm1

	xor ecx,ecx ;field2Diff
	xor ebx,ebx ;field2Cnt

	pextrw eax,xmm2,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,1
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,2
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,3
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,4
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,5
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,6
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,7
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0

	movd xmm0,ecx
	movd xmm1,ebx
	punpckldq xmm0,xmm1
	punpcklqdq xmm4,xmm0
	paddd xmm7,xmm4

	add esi,16
	add edx,16
	dec ebp
	jnz cfslop2

	mov edi,dword [esp+28] ;w
	add esi,edi
	add edx,edi
	dec dword [esp+32] ;hLeft
	jnz cfslop

	mov ebp,dword [esp+36] ;fieldStats
	movdqu [ebp],xmm7
	pextrw eax,xmm5,0
	movzx edx,ah
	movzx ebx,al
	cmp edx,ebx
	cmovb edx,ebx
	pextrw eax,xmm5,1
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm5,2
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm5,3
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm5,4
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm5,5
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm5,6
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm5,7
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	mov dword [ebp+16],edx ;fieldMDiff

	pextrw eax,xmm6,0
	movzx edx,ah
	movzx ebx,al
	cmp edx,ebx
	cmovb edx,ebx
	pextrw eax,xmm6,1
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm6,2
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm6,3
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm6,4
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm6,5
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm6,6
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm6,7
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	mov dword [ebp+20],edx ;field2MDiff
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;void IVTCFilter_CalcFieldP(UInt8 *framePtr, IntOS w, IntOS h, Int32 *fieldStats)
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 framePtr
;24 w
;28 h
;32 fieldStats

	align 16
_IVTCFilter_CalcFieldP:
	push ebp
	push ebx
	push esi
	push edi
	
	mov eax,dword [esp+28] ;h
	sub eax,2
	mul dword [esp+24] ;w
	shr eax,4
	mov ebp,eax ;IntOS wLeft = w * (h - 2) >> 4;

	mov edx,dword [esp+24] ;w
	mov esi,dword [esp+20] ;framePtr
	pxor xmm7,xmm7
	pxor xmm5,xmm5 ;fieldMDiff
	pxor xmm6,xmm6 ;field2MDiff
	align 16
cfsplop:
	movdqu xmm0,[esi]
	movdqu xmm1,[esi+edx*2]
	movdqu xmm3,[esi+edx]
	movdqu xmm2,xmm0
	pmaxub xmm0,xmm1
	pminub xmm1,xmm2
	movdqu xmm2,xmm3
	pmaxub xmm3,xmm0
	pminub xmm2,xmm1
	psubusb xmm3,xmm0
	movdqu xmm4,xmm1
	psubusb xmm4,xmm2
	paddb xmm3,xmm4 ;v4
	movdqu xmm2,xmm3
	psubusb xmm0,xmm1
	psubusb xmm2,xmm0 ;v5
	pmaxub xmm5,xmm3
	pmaxub xmm6,xmm2

	xor ecx,ecx ;fieldDiff
	xor ebx,ebx ;fieldCnt
	pextrw eax,xmm3,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,1
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,2
	shr eax,16
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,3
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,4
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,5
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,6
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm3,7
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0

	movd xmm4,ecx
	movd xmm1,ebx
	punpckldq xmm4,xmm1

	xor ecx,ecx ;field2Diff
	xor ebx,ebx ;field2Cnt

	pextrw eax,xmm2,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,1
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,2
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,3
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,4
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,5
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,6
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	pextrw eax,xmm2,7
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,al
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0
	movzx edi,ah
	add ecx,edi
	cmp edi,1
	cmc
	adc ebx,0

	movd xmm0,ecx
	movd xmm1,ebx
	punpckldq xmm0,xmm1
	punpcklqdq xmm4,xmm0
	paddd xmm7,xmm4

	add esi,16
	dec ebp
	jnz cfsplop

	mov ebp,dword [esp+32] ;fieldStats
	movdqu [ebp],xmm7
	pextrw eax,xmm5,0
	movzx edx,ah
	movzx ebx,al
	cmp edx,ebx
	cmovb edx,ebx
	pextrw eax,xmm5,1
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm5,2
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm5,3
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm5,4
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm5,5
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm5,6
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm5,7
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	mov dword [ebp+16],edx ;fieldMDiff

	pextrw eax,xmm6,0
	movzx edx,ah
	movzx ebx,al
	cmp edx,ebx
	cmovb edx,ebx
	pextrw eax,xmm6,1
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm6,2
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm6,3
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm6,4
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm6,5
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm6,6
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	pextrw eax,xmm6,7
	movzx ebx,ah
	movzx eax,al
	cmp edx,ebx
	cmovb edx,ebx
	cmp edx,eax
	cmovb edx,eax
	mov dword [ebp+20],edx ;field2MDiff
	
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret
