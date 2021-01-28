section .text

global ImageCopy_ImgCopy

extern CPUBrand
extern UseAVX

;void ImageCopy_ImgCopy(UInt8 *inPt, UInt8 *outPt, OSInt copySize, OSInt height, OSInt sstep, OSInt dstep);

;0 rsi
;8 retAddr
;rcx inPt r10
;rdx outPt r11
;r8 copySize
;r9 height
;48 sstep
;56 dstep

	align 16
ImageCopy_ImgCopy:
	cmp dword [rel UseAVX],0
	jnz ImageCopy_ImgCopy_AVX
	cld
	push rsi
	mov r10,rcx
	mov r11,rdx
	mov rcx,qword [rsp+48] ;sstep
	mov rdx,qword [rsp+56] ;dstep
	mov rax,r8 ;copySize
	mov rsi,r10 ;inPt
	cmp rcx,rdx
	jnz icstart
	cmp rax,rdx
	jnz icstart
	mul r9 ;height
	jb icexit
	mov rdx,r11 ;outPt
	test rdx,15
	jz icflop
	mov rcx,rdx
	xchg rdi,rdx
	neg rcx
	and rcx,15
	sub rax,rcx
	rep movsb
	xchg rdi,rdx
	align 16
icflop:
	mov rcx,rax
	shr rcx,6
	jz icflop3
	
	align 16
icflop2:
	movdqu xmm0,[rsi]
	movdqu xmm1,[rsi+16]
	movdqu xmm2,[rsi+32]
	movdqu xmm3,[rsi+48]
	movntdq [rdx],xmm0
	movntdq [rdx+16],xmm1
	movntdq [rdx+32],xmm2
	movntdq [rdx+48],xmm3
	lea rsi,[rsi+64]
	lea rdx,[rdx+64]
	dec rcx
	jnz icflop2
	align 16
icflop3:
	and rax,63
	jz icexit
	mov rcx,rax
	shr rcx,3
	jz icflop4
	xchg rdi,rdx
	rep movsq
	xchg rdi,rdx
	align 16
icflop4:
	and rax,7
	jz icexit
	mov rcx,rax
	xchg rdi,rdx
	rep movsb
	xchg rdi,rdx
	jmp icexit
	
	align 16
icstart:
	sub rcx,rax
	sub rdx,rax
	push rbx
	push rdi
	push rbp
	mov rbx,rcx
	mov rdi,r11 ;outPt
	mov rbp,r9 ;height
	
	;rbx sstep
	;rdx dstep
	;rax copySize
	;rbp height
	cmp rax,144
	jb icstart2
	mov r8,rdx
	align 16
iclop:
	mov rdx,rax
	test rdi,15
	jz iclop2
	mov rcx,rdi
	neg rcx
	and rcx,15
	sub rdx,rcx
	rep movsb
	align 16
iclop2:
	mov rcx,rdx
	shr rcx,6
	align 16
iclop3:
	movdqu xmm0,[rsi]
	movdqu xmm1,[rsi+16]
	movdqu xmm2,[rsi+32]
	movdqu xmm3,[rsi+48]
	movntdq [rdi],xmm0
	movntdq [rdi+16],xmm1
	movntdq [rdi+32],xmm2
	movntdq [rdi+48],xmm3
	lea rsi,[rsi+64]
	lea rdi,[rdi+64]
	dec rcx
	jnz iclop3
	
	and rdx,63
	jz iclop4
	mov rcx,rdx
	shr rcx,3
	rep movsq
	and rdx,7
	jz iclop4
	mov rcx,rdx
	rep movsb
	align 16
iclop4:
	add rsi,rbx
	add rdi,r8
	dec rbp
	jnz iclop
	
	jmp icexit2

	align 16
icstart2:
	test rax,7
	jnz icstart3
	align 16
ic2lop:
	mov rcx,rax
	shr rcx,3
	rep movsq
	add rsi,rbx
	add rdi,rdx
	dec rbp
	jnz ic2lop
	jmp icexit2
	
	align 16
icstart3:
	mov rcx,rax
	shr rcx,3
	rep movsq
	mov rcx,rax
	and rcx,7
	rep movsb
	add rsi,rbx
	add rdi,rdx
	dec rbp
	jnz icstart3
	
	align 16
icexit2:
	pop rbp
	pop rdi
	pop rbx
icexit:
	pop	rsi
	ret

	align 16
ImageCopy_ImgCopy_AVX:
	cld
	push rsi
	mov r10,rcx
	mov r11,rdx
	mov rcx,qword [rsp+48] ;sstep
	mov rdx,qword [rsp+56] ;dstep
	mov rax,r8 ;copySize
	mov rsi,r10 ;inPt
	cmp rcx,rdx
	jnz icavxstart
	cmp rax,rdx
	jnz icavxstart
	mul r9 ;height
	jb icavxexit
	mov rdx,r11 ;outPt
	test rdx,31
	jz icfavxlop
	mov rcx,rdx
	xchg rdi,rdx
	neg rcx
	and rcx,31
	sub rax,rcx
	rep movsb
	xchg rdi,rdx
	align 16
icfavxlop:
	mov rcx,rax
	shr rcx,7
	jz icfavxlop3
	
	align 16
icfavxlop2:
	vmovdqu ymm0,[rsi]
	vmovdqu ymm1,[rsi+32]
	vmovdqu ymm2,[rsi+64]
	vmovdqu ymm3,[rsi+96]
	vmovntdq [rdx],ymm0
	vmovntdq [rdx+32],ymm1
	vmovntdq [rdx+64],ymm2
	vmovntdq [rdx+96],ymm3
	lea rsi,[rsi+128]
	lea rdx,[rdx+128]
	dec rcx
	jnz icfavxlop2
	align 16
icfavxlop3:
	and rax,127
	jz icavxexit
	mov rcx,rax
	shr rcx,3
	jz icfavxlop4
	xchg rdi,rdx
	rep movsq
	xchg rdi,rdx
	align 16
icfavxlop4:
	and rax,7
	jz icavxexit
	mov rcx,rax
	xchg rdi,rdx
	rep movsb
	xchg rdi,rdx
	jmp icavxexit
	
	align 16
icavxstart:
	sub rcx,rax
	sub rdx,rax
	push rbx
	push rdi
	push rbp
	mov rbx,rcx
	mov rdi,r11 ;outPt
	mov rbp,r9 ;height
	
	;rbx sstep
	;rdx dstep
	;rax copySize
	;rbp height
	cmp rax,160
	jb icavxstart2
	mov r8,rdx
	align 16
icavxlop:
	mov rdx,rax
	test rdi,31
	jz icavxlop2
	mov rcx,rdi
	neg rcx
	and rcx,31
	sub rdx,rcx
	rep movsb
	align 16
icavxlop2:
	mov rcx,rdx
	shr rcx,7
	align 16
icavxlop3:
	vmovdqu ymm0,[rsi]
	vmovdqu ymm1,[rsi+32]
	vmovdqu ymm2,[rsi+64]
	vmovdqu ymm3,[rsi+96]
	vmovntdq [rdi],ymm0
	vmovntdq [rdi+32],ymm1
	vmovntdq [rdi+64],ymm2
	vmovntdq [rdi+96],ymm3
	lea rsi,[rsi+128]
	lea rdi,[rdi+128]
	dec rcx
	jnz icavxlop3
	
	and rdx,127
	jz icavxlop4
	mov rcx,rdx
	shr rcx,3
	rep movsq
	and rdx,7
	jz icavxlop4
	mov rcx,rdx
	rep movsb
	align 16
icavxlop4:
	add rsi,rbx
	add rdi,r8
	dec rbp
	jnz icavxlop
	
	jmp icavxexit2

	align 16
icavxstart2:
	test rax,7
	jnz icavxstart3
	align 16
ic2avxlop:
	mov rcx,rax
	shr rcx,3
	rep movsq
	add rsi,rbx
	add rdi,rdx
	dec rbp
	jnz ic2avxlop
	jmp icavxexit2
	
	align 16
icavxstart3:
	mov rcx,rax
	shr rcx,3
	rep movsq
	mov rcx,rax
	and rcx,7
	rep movsb
	add rsi,rbx
	add rdi,rdx
	dec rbp
	jnz icavxstart3
	
	align 16
icavxexit2:
	pop rbp
	pop rdi
	pop rbx
icavxexit:
	vzeroupper
	pop	rsi
	ret
