section .text

global _ImageCopy_ImgCopyR
global _ImageCopy_ImgCopy

;void ImageCopy_ImgCopyR(UInt8 *inPt, UInt8 *outPt, UIntOS copySize, UIntOS height, UIntOS sbpl, UIntOS dbpl, Bool upsideDown);

;0 retAddr
;4 inPt
;8 outPt
;12 copySize
;16 height
;20 sstep
;24 dstep
;28 upsideDown
	align 16
_ImageCopy_ImgCopyR:
	cmp dword [esi+28],0
	jz _ImageCopy_ImgCopy
	mov eax,dword [esi+24]
	mul dword [esi+16]
	add eax,dword [esi+24]
	add dword [esi+8],eax
	jmp _ImageCopy_ImgCopy

;void ImageCopy_ImgCopy(UInt8 *inPt, UInt8 *outPt, IntOS copySize, IntOS height, IntOS sstep, IntOS dstep);

;0 esi
;4 retAddr
;8 inPt
;12 outPt
;16 copySize
;20 height
;24 sstep
;28 dstep

	align 16
_ImageCopy_ImgCopy:
	cld
	push esi
	mov ecx,dword [esp+24] ;sstep
	mov edx,dword [esp+28] ;dstep
	mov eax,dword [esp+16] ;copySize
	mov esi,dword [esp+8] ;inPt
	cmp ecx,edx
	jnz icstart
	cmp eax,edx
	jnz icstart
	mul dword [esp+20] ;height
	jb icexit
	mov edx,dword [esp+12] ;outPt
	test edx,15
	jz icflop
	mov ecx,edx
	xchg edi,edx
	neg ecx
	and ecx,15
	sub eax,ecx
	rep movsb
	xchg edi,edx
	align 16
icflop:
	mov ecx,eax
	shr ecx,7
	jz icflop3
	
	align 16
icflop2:
	movdqu xmm0,[esi]
	movdqu xmm1,[esi+16]
	movdqu xmm2,[esi+32]
	movdqu xmm3,[esi+48]
	movdqu xmm4,[esi+64]
	movdqu xmm5,[esi+80]
	movdqu xmm6,[esi+96]
	movdqu xmm7,[esi+112]
	movntdq [edx],xmm0
	movntdq [edx+16],xmm1
	movntdq [edx+32],xmm2
	movntdq [edx+48],xmm3
	movntdq [edx+64],xmm4
	movntdq [edx+80],xmm5
	movntdq [edx+96],xmm6
	movntdq [edx+112],xmm7
	lea esi,[esi+128]
	lea edx,[edx+128]
	dec ecx
	jnz icflop2
	align 16
icflop3:
	and eax,127
	jz icexit
	mov ecx,eax
	shr ecx,2
	jz icflop4
	xchg edi,edx
	rep movsd
	xchg edi,edx
	align 16
icflop4:
	and eax,3
	jz icexit
	mov ecx,eax
	xchg edi,edx
	rep movsb
	xchg edi,edx
	jmp icexit
	
	align 16
icstart:
	sub ecx,eax
	sub edx,eax
	push ebx
	push edi
	push ebp
	mov ebx,ecx
	mov edi,dword [esp+12+12] ;outPt
	mov ebp,dword [esp+12+20] ;height
	
	;ebx sstep
	;edx dstep
	;eax copySize
	;ebp height
	cmp eax,144
	jb icstart2
	push edx
	align 16
iclop:
	mov edx,eax
	test edi,15
	jz iclop2
	mov ecx,edi
	neg ecx
	and ecx,15
	sub edx,ecx
	rep movsb
	align 16
iclop2:
	mov ecx,edx
	shr ecx,7
	align 16
iclop3:
	movdqu xmm0,[esi]
	movdqu xmm1,[esi+16]
	movdqu xmm2,[esi+32]
	movdqu xmm3,[esi+48]
	movdqu xmm4,[esi+64]
	movdqu xmm5,[esi+80]
	movdqu xmm6,[esi+96]
	movdqu xmm7,[esi+112]
	movntdq [edi],xmm0
	movntdq [edi+16],xmm1
	movntdq [edi+32],xmm2
	movntdq [edi+48],xmm3
	movntdq [edi+64],xmm4
	movntdq [edi+80],xmm5
	movntdq [edi+96],xmm6
	movntdq [edi+112],xmm7
	lea esi,[esi+128]
	lea edi,[edi+128]
	dec ecx
	jnz iclop3
	
	and edx,127
	jz iclop4
	mov ecx,edx
	shr ecx,2
	rep movsd
	and edx,3
	jz iclop4
	mov ecx,edx
	rep movsb
	align 16
iclop4:
	add esi,ebx
	add edi,dword [esp]
	dec ebp
	jnz iclop
	
	pop edx
	jmp icexit2

	align 16
icstart2:
	test eax,3
	jnz icstart3
	align 16
ic2lop:
	mov ecx,eax
	shr ecx,2
	rep movsd
	add esi,ebx
	add edi,edx
	dec ebp
	jnz ic2lop
	jmp icexit2
	
	align 16
icstart3:
	mov ecx,eax
	shr ecx,2
	rep movsd
	mov ecx,eax
	and ecx,3
	rep movsb
	add esi,ebx
	add edi,edx
	dec ebp
	jnz icstart3
	
	align 16
icexit2:
	pop ebp
	pop edi
	pop ebx
icexit:
	pop	esi
	ret