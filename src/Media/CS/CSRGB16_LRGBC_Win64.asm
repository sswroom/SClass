section .text

global CSRGB16_LRGBC_ConvertB16G16R16A16
global CSRGB16_LRGBC_ConvertR16G16B16A16
global CSRGB16_LRGBC_ConvertB16G16R16
global CSRGB16_LRGBC_ConvertR16G16B16
global CSRGB16_LRGBC_ConvertW16A16
global CSRGB16_LRGBC_ConvertW16
global CSRGB16_LRGBC_ConvertA2B10G10R10

;void CSRGB16_LRGBC_ConvertB16G16R16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, UInt8 *rgbTable);
;0 rdi
;8 rsi
;16 rbx
;24 retAddr
;rcx srcPtr
;rdx destPtr
;r8 width
;r9 height
;64 srcRGBBpl
;72 destRGBBpl
;80 rgbTable
	align 16
CSRGB16_LRGBC_ConvertB16G16R16A16:
	push rbx
	push rsi
	push rdi
	mov rsi,rcx ;srcPtr
	mov rdi,rdx ;destPtr
	mov rbx,qword [rsp+80] ;rgbTable
	mov rdx,r8 ;width

	lea rcx,[rdx*8]
	sub qword [rsp+64],rcx ;srcRGBBpl
	sub qword [rsp+72],rcx ;destRGBBpl
	test r8,1
	jnz cbgra16_1lop
	shr r8,1
	test rdi,15
	jnz cbgra16_2nalop
	test qword [rsp+64],15
	jnz cbgra16_2nalop
	
	align 16
cbgra16_2lop:
	mov rcx,r8 ;width
	align 16
cbgra16_2lop2:
	movzx rax,word [rsi]
	movzx rdx,word [rsi+8]
	movq xmm1,[rbx+rax*8+1048576]
	movhpd xmm1,[rbx+rdx*8+1048576]
	movzx rax,word [rsi+2]
	movzx rdx,word [rsi+10]
	movq xmm0,[rbx+rax*8+524288]
	movhpd xmm0,[rbx+rdx*8+524288]
	paddsw xmm1,xmm0
	movzx rax,word [rsi+4]
	movzx rdx,word [rsi+12]
	movq xmm0, [rbx+rax*8]
	movhpd xmm0, [rbx+rdx*8]
	paddsw xmm1,xmm0
	movntdq [rdi],xmm1
	lea rdi,[rdi+16]
	lea rsi,[rsi+16]
	dec rcx
	jnz cbgra16_2lop2
	add rsi,qword [rsp+64] ;srcRGBBpl
	add rdi,qword [rsp+72] ;destRGBBpl
	dec r9
	jnz cbgra16_2lop
	jmp cbgra16exit
	
	align 16
cbgra16_2nalop:
	mov rcx,r8 ;width
	align 16
cbgra16_2nalop2:
	movzx rax,word [rsi]
	movzx rdx,word [rsi+8]
	movq xmm1,[rbx+rax*8+1048576]
	movhpd xmm1,[rbx+rdx*8+1048576]
	movzx rax,word [rsi+2]
	movzx rdx,word [rsi+10]
	movq xmm0,[rbx+rax*8+524288]
	movhpd xmm0,[rbx+rdx*8+524288]
	paddsw xmm1,xmm0
	movzx rax,word [rsi+4]
	movzx rdx,word [rsi+12]
	movq xmm0, [rbx+rax*8]
	movhpd xmm0, [rbx+rdx*8]
	paddsw xmm1,xmm0
	movdqu [rdi],xmm1
	lea rdi,[rdi+16]
	lea rsi,[rsi+16]
	dec rcx
	jnz cbgra16_2nalop2
	add rsi,qword [rsp+64] ;srcRGBBpl
	add rdi,qword [rsp+72] ;destRGBBpl
	dec r9
	jnz cbgra16_2nalop
	jmp cbgra16exit

	align 16
cbgra16_1lop:
	mov rcx,r8 ;width
	align 16
cbgra16_1lop2:
	movzx rax,word [rsi]
	movq xmm1,[rbx+rax*8+1048576]
	movzx rax,word [rsi+2]
	movq xmm0,[rbx+rax*8+524288]
	paddsw xmm1,xmm0
	movzx rax,word [rsi+4]
	movq xmm0, [rbx+rax*8]
	paddsw xmm1,xmm0
	movq [rdi],xmm1
	lea rdi,[rdi+8]
	lea rsi,[rsi+8]
	dec rcx
	jnz cbgra16_1lop2
	add rsi,qword [rsp+64] ;srcRGBBpl
	add rdi,qword [rsp+72] ;destRGBBpl
	dec r9
	jnz cbgra16_1lop

	align 16
cbgra16exit:
	pop rdi
	pop rsi
	pop rbx
	ret

;void CSRGB16_LRGBC_ConvertR16G16B16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, UInt8 *rgbTable);
;0 rdi
;8 rsi
;16 rbx
;24 retAddr
;rcx srcPtr
;rdx destPtr
;r8 width
;r9 height
;64 srcRGBBpl
;72 destRGBBpl
;80 rgbTable
	align 16
CSRGB16_LRGBC_ConvertR16G16B16A16:
	push rbx
	push rsi
	push rdi
	mov rsi,rcx ;srcPtr
	mov rdi,rdx ;destPtr
	mov rbx,qword [rsp+80] ;rgbTable
	mov rdx,r8 ;width

	lea rcx,[rdx*8]
	sub qword [rsp+64],rcx ;srcRGBBpl
	sub qword [rsp+72],rcx ;destRGBBpl
	test r8,1
	jnz crgba16_1lop
	shr r8,1
	test rdi,15
	jnz crgba16_2nalop
	test qword [rsp+64],15
	jnz crgba16_2nalop
	
	align 16
crgba16_2lop:
	mov rcx,r8 ;width
	align 16
crgba16_2lop2:
	movzx rax,word [rsi+4]
	movzx rdx,word [rsi+12]
	movq xmm1,[rbx+rax*8+1048576]
	movhpd xmm1,[rbx+rdx*8+1048576]
	movzx rax,word [rsi+2]
	movzx rdx,word [rsi+10]
	movq xmm0,[rbx+rax*8+524288]
	movhpd xmm0,[rbx+rdx*8+524288]
	paddsw xmm1,xmm0
	movzx rax,word [rsi]
	movzx rdx,word [rsi+8]
	movq xmm0, [rbx+rax*8]
	movhpd xmm0, [rbx+rdx*8]
	paddsw xmm1,xmm0
	movntdq [rdi],xmm1
	lea rdi,[rdi+16]
	lea rsi,[rsi+16]
	dec rcx
	jnz crgba16_2lop2
	add rsi,qword [rsp+64] ;srcRGBBpl
	add rdi,qword [rsp+72] ;destRGBBpl
	dec r9
	jnz crgba16_2lop
	jmp crgba16exit
	
	align 16
crgba16_2nalop:
	mov rcx,r8 ;width
	align 16
crgba16_2nalop2:
	movzx rax,word [rsi+4]
	movzx rdx,word [rsi+12]
	movq xmm1,[rbx+rax*8+1048576]
	movhpd xmm1,[rbx+rdx*8+1048576]
	movzx rax,word [rsi+2]
	movzx rdx,word [rsi+10]
	movq xmm0,[rbx+rax*8+524288]
	movhpd xmm0,[rbx+rdx*8+524288]
	paddsw xmm1,xmm0
	movzx rax,word [rsi]
	movzx rdx,word [rsi+8]
	movq xmm0, [rbx+rax*8]
	movhpd xmm0, [rbx+rdx*8]
	paddsw xmm1,xmm0
	movdqu [rdi],xmm1
	lea rdi,[rdi+16]
	lea rsi,[rsi+16]
	dec rcx
	jnz crgba16_2nalop2
	add rsi,qword [rsp+64] ;srcRGBBpl
	add rdi,qword [rsp+72] ;destRGBBpl
	dec r9
	jnz crgba16_2nalop
	jmp crgba16exit

	align 16
crgba16_1lop:
	mov rcx,r8 ;width
	align 16
crgba16_1lop2:
	movzx rax,word [rsi+4]
	movq xmm1,[rbx+rax*8+1048576]
	movzx rax,word [rsi+2]
	movq xmm0,[rbx+rax*8+524288]
	paddsw xmm1,xmm0
	movzx rax,word [rsi]
	movq xmm0, [rbx+rax*8]
	paddsw xmm1,xmm0
	movq [rdi],xmm1
	lea rdi,[rdi+8]
	lea rsi,[rsi+8]
	dec rcx
	jnz crgba16_1lop2
	add rsi,qword [rsp+64] ;srcRGBBpl
	add rdi,qword [rsp+72] ;destRGBBpl
	dec r9
	jnz crgba16_1lop

	align 16
crgba16exit:
	pop rdi
	pop rsi
	pop rbx
	ret

;void CSRGB16_LRGBC_ConvertB16G16R16(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, UInt8 *rgbTable);
;0 rdi
;8 rsi
;16 rbx
;24 retAddr
;rcx srcPtr
;rdx destPtr
;r8 width
;r9 height
;64 srcRGBBpl
;72 destRGBBpl
;80 rgbTable
	align 16
CSRGB16_LRGBC_ConvertB16G16R16:
	push rbx
	push rsi
	push rdi
	mov rsi,rcx ;srcPtr
	mov rdi,rdx ;destPtr
	mov rbx,qword [rsp+80] ;rgbTable
	mov rdx,r8 ;width

	lea rcx,[rdx*8]
	mov rax,6
	mul rdx
	sub qword [rsp+64],rax ;srcRGBBpl
	sub qword [rsp+72],rcx ;destRGBBpl
	align 16
cbgr16lop:
	mov rcx,r8 ;width
	align 16
cbgr16lop2:
	movzx rax,word [rsi]
	movq xmm1,[rbx+rax*8+1048576]
	movzx rax,word [rsi+2]
	movq xmm0,[rbx+rax*8+524288]
	paddsw xmm1,xmm0
	movzx rax,word [rsi+4]
	movq xmm0,[rbx+rax*8]
	paddsw xmm1,xmm0
	movq [rdi],xmm1
	lea rdi,[rdi+8]
	lea rsi,[rsi+6]
	dec rcx
	jnz cbgr16lop2
	add rsi,qword [rsp+64] ;srcRGBBpl
	add rdi,qword [rsp+72] ;destRGBBpl
	dec r9
	jnz cbgr16lop
	
	pop rdi
	pop rsi
	pop rbx
	ret

;void CSRGB16_LRGBC_ConvertR16G16B16(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, UInt8 *rgbTable);
;0 rdi
;8 rsi
;16 rbx
;24 retAddr
;rcx srcPtr
;rdx destPtr
;r8 width
;r9 height
;64 srcRGBBpl
;72 destRGBBpl
;80 rgbTable
	align 16
CSRGB16_LRGBC_ConvertR16G16B16:
	push rbx
	push rsi
	push rdi
	mov rsi,rcx ;srcPtr
	mov rdi,rdx ;destPtr
	mov rbx,qword [rsp+80] ;rgbTable
	mov rdx,r8 ;width

	lea rcx,[rdx*8]
	mov rax,6
	mul rdx
	sub qword [rsp+64],rax ;srcRGBBpl
	sub qword [rsp+72],rcx ;destRGBBpl
	align 16
crgb16lop:
	mov rcx,r8 ;width
	align 16
crgb16lop2:
	movzx rax,word [rsi]
	movq xmm1,[rbx+rax*8+1048576]
	movzx rax,word [rsi+2]
	movq xmm0,[rbx+rax*8+524288]
	paddsw xmm1,xmm0
	movzx rax,word [rsi+4]
	movq xmm0,[rbx+rax*8]
	paddsw xmm1,xmm0
	movq [rdi],xmm1
	lea rdi,[rdi+8]
	lea rsi,[rsi+6]
	dec rcx
	jnz crgb16lop2
	add rsi,qword [rsp+64] ;srcRGBBpl
	add rdi,qword [rsp+72] ;destRGBBpl
	dec r9
	jnz crgb16lop
	
	pop rdi
	pop rsi
	pop rbx
	ret

;void CSRGB16_LRGBC_ConvertW16A16(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, UInt8 *rgbTable);
;0 rdi
;8 rsi
;16 rbx
;24 retAddr
;rcx srcPtr
;rdx destPtr
;r8 width
;r9 height
;64 srcRGBBpl
;72 destRGBBpl
;80 rgbTable
	align 16
CSRGB16_LRGBC_ConvertW16A16:
	push rbx
	push rsi
	push rdi
	mov rsi,rcx ;srcPtr
	mov rdi,rdx ;destPtr
	mov rbx,qword [rsp+80] ;rgbTable
	mov rdx,r8 ;width

	lea rax,[rdx*4]
	lea rcx,[rdx*8]
	sub qword [rsp+64],rax ;srcRGBBpl
	sub qword [rsp+72],rcx ;destRGBBpl
	
	align 16
cwa16lop:
	mov rcx,r8 ;width
	align 16
cwa16lop2:
	movzx rax,word [rsi]
	movq xmm1,[rbx+rax*8+1048576]
	movq xmm0,[rbx+rax*8+524288]
	paddsw xmm1,xmm0
	movq xmm0, [rbx+rax*8]
	paddsw xmm1,xmm0
	movq [rdi],xmm1
	lea rsi,[rsi+4]
	lea rdi,[rdi+8]
	dec rcx
	jnz cwa16lop2
	add rsi,qword [rsp+64] ;srcRGBBpl
	add rdi,qword [rsp+72] ;destRGBBpl
	dec r9
	jnz cwa16lop

	pop rdi
	pop rsi
	pop rbx
	ret

;void CSRGB16_LRGBC_ConvertW16(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, UInt8 *rgbTable);
;0 rdi
;8 rsi
;16 rbx
;24 retAddr
;rcx srcPtr
;rdx destPtr
;r8 width
;r9 height
;64 srcRGBBpl
;72 destRGBBpl
;80 rgbTable
	align 16
CSRGB16_LRGBC_ConvertW16:
	push rbx
	push rsi
	push rdi
	mov rsi,rcx ;srcPtr
	mov rdi,rdx ;destPtr
	mov rbx,qword [rsp+80] ;rgbTable
	mov rdx,r8 ;width

	lea rax,[rdx*2]
	lea rcx,[rdx*8]
	sub qword [rsp+64],rax ;srcRGBBpl
	sub qword [rsp+72],rcx ;destRGBBpl
	
	align 16
cw16lop:
	mov rcx,r8 ;width
	align 16
cw16lop2:
	movzx rax,word [rsi]
	movq xmm1,[rbx+rax*8+1048576]
	movq xmm0,[rbx+rax*8+524288]
	paddsw xmm1,xmm0
	movq xmm2, [rbx+rax*8]
	paddsw xmm1,xmm2
	movq [rdi],xmm1
	lea rdi,[rdi+8]
	lea rsi,[rsi+2]
	dec rcx
	jnz cw16lop2
	add rsi,qword [rsp+64] ;srcRGBBpl
	add rdi,qword [rsp+72] ;destRGBBpl
	dec r9
	jnz cw16lop

	pop rdi
	pop rsi
	pop rbx
	ret

;void CSRGB16_LRGBC_ConvertA2B10G10R10(UInt8 *srcPtr, UInt8 *destPtr, OSInt width, OSInt height, OSInt srcRGBBpl, OSInt destRGBBpl, UInt8 *rgbTable);
;0 rdi
;8 rsi
;16 rbx
;24 retAddr
;rcx srcPtr
;rdx destPtr
;r8 width
;r9 height
;64 srcRGBBpl
;72 destRGBBpl
;89 rgbTable
	align 16
CSRGB16_LRGBC_ConvertA2B10G10R10:
	push rbx
	push rsi
	push rdi
	mov rsi,rcx ;srcPtr
	mov rdi,rdx ;destPtr
	mov rbx,qword [rsp+80] ;rgbTable
	mov rdx,r8 ;width
	
	lea rax,[rdx*4]
	lea rcx,[rdx*8]
	sub qword [rsp+64],rax ;srcRGBBpl
	sub qword [rsp+72],rcx ;destRGBBpl
	
	align 16
ca2b10g10r10lop:
	mov rcx,r8 ;width
	align 16
ca2b10g10r10lop2:
	mov edx,dword [rsi]
	mov rax,rdx
	shr rdx,14
	shl rax,6
	shr dx,6
	shr ax,6
	movd xmm1,edx
	movd xmm2,eax
	punpckldq xmm2,xmm1
	psllw xmm2,6
	movdqa xmm1,xmm2
	psrlw xmm2,10
	por xmm2,xmm1
	pextrw rax,xmm2,0
	movq xmm1, [rbx+rax*8]
	pextrw rax,xmm2,1
	movq xmm0,[rbx+rax*8+524288]
	paddsw xmm1,xmm0
	pextrw rax,xmm2,2
	movq xmm0,[rbx+rax*8+1048576]
	paddsw xmm1,xmm0
	movq [rdi],xmm1
	lea rsi,[rsi+4]
	lea rdi,[rdi+8]
	dec rcx
	jnz ca2b10g10r10lop2
	add rsi,qword [rsp+64] ;srcRGBBpl
	add rdi,qword [rsp+72] ;destRGBBpl
	dec r9
	jnz ca2b10g10r10lop

	pop rdi
	pop rsi
	pop rbx
	ret
