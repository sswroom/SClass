section .text

global _NearestNeighbourResizer64_64_Resize

;void NearestNeighbourResizer64_64_Resize(UInt8 *inPt, UInt8 *outPt, IntOS dwidth, IntOS dheight, IntOS dbpl, IntOS *xindex, IntOS *yindex);
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 inPt
;24 outPt
;28 dwidth
;32 dheight
;36 dbpl
;40 xindex
;44 yindex rdi

	align 16
_NearestNeighbourResizer64_64_Resize:
	push ebp
	push ebx
	push esi
	push edi
	mov ecx,dword [esp+20] ;inPt
	mov edx,dword [esp+24] ;outPt
	mov eax,dword [esp+28] ;dwidth
	mov edi,dword [esp+44] ;yindex
	shl eax,3
	sub dword [esp+36],eax ;dbpl
	test edx,15
	jnz nnr64rlop
	test dword [esp+28],1 ;dwidth
	jz nnr64rlop3
	
	align 16
nnr64rlop:
	mov ebx,dword [edi]
	mov esi,dword [esp+40] ;xindex
	lea ebx,[ebx+ecx]
	mov ebp,dword [esp+28] ;dwidth
	
	align 16
nnr64rlop2:
	mov eax,dword [esi]
	lea esi,[esi+4]
	movq xmm0,[ebx+eax]
	movq [edx],xmm0
	dec ebp
	lea edx,[edx+8]
	jnz nnr64rlop2
	
	add edx,dword [esp+36] ;dbpl
	lea edi,[edi+4]
	dec dword [esp+32] ;dheight
	jnz nnr64rlop
	jmp nnr64rexit
	
	align 16
nnr64rlop3:
	shr dword [esp+28],1 ;dwidth

	align 16
nnr64rlop5:
	mov ebx,dword [edi]
	mov esi,dword [esp+40] ;xindex
	lea ebx,[ebx+ecx]
	mov ebp,dword [esp+28] ;dwidth
	
	align 16
nnr64rlop6:
	mov eax,dword [esi]
	movq xmm0,[ebx+eax]
	mov eax,dword [esi+4]
	lea esi,[esi+8]
	movq xmm1,[ebx+eax]
	punpcklqdq xmm0,xmm1
	movntdq [edx],xmm0
	dec ebp
	lea edx,[edx+16]
	jnz nnr64rlop6
	
	add edx,dword [esp+36] ;dbpl
	lea edi,[edi+4]
	dec dword [esp+32] ;dheight
	jnz nnr64rlop5
	
	align 16
nnr64rexit:
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret
