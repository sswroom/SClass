section .text

global CSRGBF_LRGBC_Convert

;void CSRGBF_LRGBC_Convert(UInt8 *srcPtr, UInt8 *destPtr, IntOS width, IntOS height, IntOS srcNBits, IntOS srcRGBBpl, IntOS destRGBBpl, UInt8 *rgbTable);
;0 rdi
;8 rsi
;16 rbx
;24 retAddr
;rcx srcPtr
;rdx destPtr
;r8 width
;r9 height
;64 srcNBits
;72 srcRGBBpl
;80 destRGBBpl
;88 rgbTable
	align 16
CSRGBF_LRGBC_Convert:
	push rbx
	push rsi
	push rdi
	mov rsi,rcx ;srcPtr
	mov rdi,rdx ;destPtr
	mov rax,qword [rsp+64] ;srcNBits
	mov rbx,qword [rsp+88] ;rgbTable
	mov rdx,r8 ;width
	cmp rax,128
	jz cargbf32start
	cmp rax,96
	jz crgbf32start
	cmp rax,64
	jz cawf32start
	cmp rax,32
	jz cwf32start
	jmp crgbexit
	
	align 16
cargbf32start:
	lea rcx,[rdx*8]
	lea rax,[rcx*2]
	sub qword [rsp+72],rax ;srcRGBBpl
	sub qword [rsp+80],rcx ;destRGBBpl
	mov rax,32767
	cvtsi2ss xmm4,eax
	unpcklps xmm4,xmm4
	unpcklps xmm4,xmm4
	pxor xmm3,xmm3
	
	align 16
cargbf32lop:
	mov rcx,r8 ;width
	align 16
cargbf32lop2:
	movaps xmm0,[rsi]
	mulps xmm0,xmm4
	cvtps2dq xmm2,xmm0
	packssdw xmm2,xmm3
	
	pextrw rax,xmm2,0
	movq xmm1,[rbx+rax*8+1048576]
	pextrw rax,xmm2,1
	movq xmm0,[rbx+rax*8+524288]
	paddsw xmm1,xmm0
	pextrw rax,xmm2,2
	movq xmm0, [rbx+rax*8]
	paddsw xmm1,xmm0
	movq [rdi],xmm1
	lea rsi,[rsi+16]
	lea rdi,[rdi+8]
	dec rcx
	jnz cargbf32lop2
	add rsi,qword [rsp+72] ;srcRGBBpl
	add rdi,qword [rsp+80] ;destRGBBpl
	dec r9
	jnz cargbf32lop
	jmp crgbexit

	align 16
crgbf32start:
	lea rcx,[rdx*4]
	lea rax,[rcx*2+rcx]
	lea rcx,[rcx*2]
	sub qword [rsp+72],rax ;srcRGBBpl
	sub qword [rsp+80],rcx ;destRGBBpl
	mov rax,32767
	cvtsi2ss xmm4,eax
	unpcklps xmm4,xmm4
	unpcklps xmm4,xmm4
	pxor xmm3,xmm3
	
	align 16
crgbf32lop:
	mov rcx,r8 ;width
	align 16
crgbf32lop2:
	movq xmm0,[rsi]
	movss xmm5,[rsi+8]
	punpcklqdq xmm0,xmm5
	mulps xmm0,xmm4
	cvtps2dq xmm2,xmm0
	packssdw xmm2,xmm3
	
	pextrw rax,xmm2,0
	movq xmm1,[rbx+rax*8+1048576]
	pextrw rax,xmm2,1
	movq xmm0,[rbx+rax*8+524288]
	paddsw xmm1,xmm0
	pextrw rax,xmm2,2
	movq xmm0, [rbx+rax*8]
	paddsw xmm1,xmm0
	movq [rdi],xmm1
	lea rsi,[rsi+12]
	lea rdi,[rdi+8]
	dec rcx
	jnz crgbf32lop2
	add rsi,qword [rsp+72] ;srcRGBBpl
	add rdi,qword [rsp+80] ;destRGBBpl
	dec r9
	jnz crgbf32lop
	jmp crgbexit

	align 16
cawf32start:
	lea rcx,[rdx*8]
	sub qword [rsp+72],rcx ;srcRGBBpl
	sub qword [rsp+80],rcx ;destRGBBpl
	mov rax,32767
	cvtsi2ss xmm4,eax
	unpcklps xmm4,xmm4
	unpcklps xmm4,xmm4
	pxor xmm3,xmm3
	
	align 16
cawf32lop:
	mov rcx,r8 ;width
	align 16
cawf32lop2:
	movq xmm0,[rsi]
	mulps xmm0,xmm4
	cvtps2dq xmm2,xmm0
	packssdw xmm2,xmm3
	
	pextrw rax,xmm2,0
	movq xmm1,[rbx+rax*8+1048576]
	movq xmm0,[rbx+rax*8+524288]
	paddsw xmm1,xmm0
	movq xmm0, [rbx+rax*8]
	paddsw xmm1,xmm0
	movq [rdi],xmm1
	lea rdi,[rdi+8]
	lea rsi,[rsi+8]
	dec rcx
	jnz cawf32lop2
	add rsi,qword [rsp+72] ;srcRGBBpl
	add rdi,qword [rsp+80] ;destRGBBpl
	dec r9
	jnz cawf32lop
	jmp crgbexit

	align 16
cwf32start:
	lea rcx,[rdx*8]
	lea rax,[rdx*4]
	sub qword [rsp+72],rax ;srcRGBBpl
	sub qword [rsp+80],rcx ;destRGBBpl
	mov rax,32767
	cvtsi2ss xmm4,eax
	unpcklps xmm4,xmm4
	unpcklps xmm4,xmm4
	pxor xmm3,xmm3
	
	align 16
cwf32lop:
	mov rcx,r8 ;width
	align 16
cwf32lop2:
	movss xmm0,[rsi]
	mulps xmm0,xmm4
	cvtps2dq xmm2,xmm0
	packssdw xmm2,xmm3
	
	pextrw rax,xmm2,0
	movq xmm1,[rbx+rax*8+1048576]
	movq xmm0,[rbx+rax*8+524288]
	paddsw xmm1,xmm0
	movq xmm0, [rbx+rax*8]
	paddsw xmm1,xmm0
	movq [rdi],xmm1
	lea rsi,[rsi+4]
	lea rdi,[rdi+8]
	dec rcx
	jnz cwf32lop2
	add rsi,qword [rsp+72] ;srcRGBBpl
	add rdi,qword [rsp+80] ;destRGBBpl
	dec r9
	jnz cwf32lop
	jmp crgbexit

	align 16
crgbexit:
	pop rdi
	pop rsi
	pop rbx
	ret
