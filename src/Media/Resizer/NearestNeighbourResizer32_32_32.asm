section .text

global _NearestNeighbourResizer32_32_Resize

;void NearestNeighbourResizer32_32_Resize(UInt8 *inPt, UInt8 *outPt, IntOS dwidth, IntOS dheight, IntOS dbpl, IntOS *xindex, IntOS *yindex);
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
_NearestNeighbourResizer32_32_Resize:
	push ebp
	push ebx
	push esi
	push edi
	mov ecx,dword [esp+20] ;inPt
	mov edx,dword [esp+24] ;outPt
	mov eax,dword [esp+28] ;dwidth
	mov edi,dword [esp+44] ;yindex
	shl eax,2
	sub dword [esp+36],eax ;dbpl
	test edx,15
	jnz nnr32rlop
	test dword [esp+28],3 ;dwidth
	jz nnr32rlop3
	
	align 16
nnr32rlop:
	mov ebx,dword [edi]
	mov esi,dword [esp+40] ;xindex
	lea ebx,[ebx+ecx]
	mov ebp,dword [esp+28] ;dwidth
	
	align 16
nnr32rlop2:
	mov eax,dword [esi]
	lea esi,[esi+4]
	mov eax,dword [ebx+eax]
	movnti dword [edx],eax
	dec ebp
	lea edx,[edx+4]
	jnz nnr32rlop2
	
	add edx,dword [esp+36] ;dbpl
	lea edi,[edi+4]
	dec dword [esp+32] ;dheight
	jnz nnr32rlop
	jmp nnr32rexit
	
	align 16
nnr32rlop3:
	shr dword [esp+28],2 ;dwidth

	align 16
nnr32rlop5:
	mov ebx,dword [edi]
	mov esi,dword [esp+40] ;xindex
	lea ebx,[ebx+ecx]
	mov ebp,dword [esp+28] ;dwidth
	
	align 16
nnr32rlop6:
	mov eax,dword [esi]
	movd xmm0,dword [ebx+eax]
	mov eax,dword [esi+8]
	movd xmm2,dword [ebx+eax]
	mov eax,dword [esi+4]
	punpckldq xmm0,xmm2
	movd xmm1,dword [ebx+eax]
	mov eax,dword [esi+12]
	lea esi,[esi+16]
	movd xmm3,dword [ebx+eax]
	punpckldq xmm1,xmm3
	punpckldq xmm0,xmm1
	movntdq [edx],xmm0
	dec ebp
	lea edx,[edx+16]
	jnz nnr32rlop6
	
	add edx,dword [esp+36] ;dbpl
	lea edi,[edi+4]
	dec dword [esp+32] ;dheight
	jnz nnr32rlop5
	
	align 16
nnr32rexit:
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret
