%ifidn __OUTPUT_FORMAT__,elf
section .note.GNU-stack noalloc noexec nowrite progbits
%endif
%ifidn __OUTPUT_FORMAT__,elf32
section .note.GNU-stack noalloc noexec nowrite progbits
%endif
%ifidn __OUTPUT_FORMAT__,elf64
section .note.GNU-stack noalloc noexec nowrite progbits
%endif

section .text

global ImageCopy_ImgCopyR
global _ImageCopy_ImgCopyR

global ImageCopy_ImgCopy
global _ImageCopy_ImgCopy

;void ImageCopy_ImgCopyR(UInt8 *inPt, UInt8 *outPt, UIntOS copySize, UIntOS height, UIntOS sbpl, UIntOS dbpl, Bool upsideDown);

;0 retAddr
;rdi inPt
;rsi outPt
;rdx copySize
;rcx height
;r8 sbpl
;r9 dbpl
;8 upsideDown
	align 16
ImageCopy_ImgCopyR:
_ImageCopy_ImgCopyR:
	cmp byte [rsp+8],0
	jz ImageCopy_ImgCopy
	mov r10,rdx
	mov r9,rax
	mul rcx
	sub rax,r9
	add rsi,rax
	mov rdx,r10
	jmp ImageCopy_ImgCopy

;void ImageCopy_ImgCopy(UInt8 *inPt, UInt8 *outPt, IntOS copySize, IntOS height, IntOS sstep, IntOS dstep);

;0 retAddr
;rdi inPt
;rsi outPt
;rdx copySize
;rcx height
;r8 sstep
;r9 dstep

	align 16
ImageCopy_ImgCopy:
_ImageCopy_ImgCopy:
	cld
	xchg rsi,rdi
	mov rax,rdx ;copySize
	cmp r8,r9
	jnz icstart
	cmp rax,r9
	jnz icstart
	mul rcx ;height
	jb icexit
	test rdi,15
	jz icflop
	mov rcx,rdi
	neg rcx
	and rcx,15
	sub rax,rcx
	rep movsb
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
	movntdq [rdi],xmm0
	movntdq [rdi+16],xmm1
	movntdq [rdi+32],xmm2
	movntdq [rdi+48],xmm3
	lea rsi,[rsi+64]
	lea rdi,[rdi+64]
	dec rcx
	jnz icflop2
	align 16
icflop3:
	and rax,63
	jz icexit
	mov rcx,rax
	shr rcx,3
	jz icflop4
	rep movsq
	align 16
icflop4:
	and rax,7
	jz icexit
	mov rcx,rax
	rep movsb
	jmp icexit
	
	align 16
icstart:
	mov r10,rcx
	sub r8,rax
	sub r9,rax

	cmp rax,144
	jb icstart2

	align 16
iclop:
	mov rax,rdx
	test rdi,15
	jz iclop2
	mov rcx,rdi
	neg rcx
	and rcx,15
	sub rax,rcx
	rep movsb
	align 16
iclop2:
	mov rcx,rax
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
	
	and rax,63
	jz iclop4
	mov rcx,rax
	shr rcx,3
	rep movsq
	and rax,7
	jz iclop4
	mov rcx,rax
	rep movsb
	align 16
iclop4:
	add rsi,r8
	add rdi,r9
	dec r10
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
	add rsi,r8
	add rdi,r9
	dec r10
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
	add rsi,r8
	add rdi,r9
	dec r10
	jnz icstart3
	
	align 16
icexit2:
icexit:
	ret
