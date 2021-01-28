section .text

global MemFillB
global MemFillW
global MemClearAC_SSE
global MemClearANC_SSE
global MemCopyAC_SSE
global MemCopyANC_SSE
global MemCopyNAC_SSE
global MemCopyNANC_SSE
global MemClearAC_AVX
global MemClearANC_AVX
global MemCopyAC_AVX
global MemCopyANC_AVX
global MemCopyNAC_AVX
global MemCopyNANC_AVX
global MemClearAC_AMDSSE
global MemClearANC_AMDSSE
global MemCopyAC_AMDSSE
global MemCopyANC_AMDSSE
global MemCopyNAC_AMDSSE
global MemCopyNANC_AMDSSE
global MemClearAC_AMDAVX
global MemClearANC_AMDAVX
global MemCopyAC_AMDAVX
global MemCopyANC_AMDAVX
global MemCopyNAC_AMDAVX
global MemCopyNANC_AMDAVX

;void MemFillB(UInt8 *buff, OSInt byteCnt, UInt8 val)
;0 retAddr
;rcx buff
;rdx byteCnt
;r8 val
	align 16
MemFillB:
	push rdi
	mov rdi,rcx
	mov rax,r8
	mov rcx,rdx
	rep stosb
	pop rdi
	ret

;void MemFillW(UInt8 *buff, OSInt wordCnt, UInt16 val);
;0 retAddr
;rcx buff
;rdx wordCnt
;r8 val
	align 16
MemFillW:
	push rdi
	mov rdi,rcx
	mov rax,r8
	mov rcx,rdx
	rep stosw
	pop rdi
	ret

;void MemClearAC_SSE(void *buff, OSInt buffSize); //buff 16-byte align, buffSize 16 bytes
;0 retAddr
;rcx buff
;rdx buffSize
	align 16
MemClearAC_SSE:
	shr rdx,4
	pxor xmm0,xmm0
	align 16
memclearaclop:
	movaps [rcx],xmm0
	lea rcx,[rcx+16]
	dec rdx
	jnz memclearaclop
	ret
	
;void MemClearANC_SSE(void *buff, OSInt buffSize); //buff 16-byte align, buffSize 16 bytes
;0 retAddr
;rcx buff
;rdx buffSize
	align 16
MemClearANC_SSE:
	shr rdx,4
	pxor xmm0,xmm0
	align 16
memclearanclop:
	movntps [rcx],xmm0
	lea rcx,[rcx+16]
	dec rdx
	jnz memclearanclop
	ret
	
;void MemCopyAC_SSE(void *destPtr, const void *srcPtr, OSInt leng)
;0 retAddr
;rcx destPtr
;rdx srcPtr
;r8 leng
	align 16
MemCopyAC_SSE:
	cmp rcx,rdx
	jz memcopyacexit
	cmp r8,64
	jb memcopyaclop

	mov r9,r8
	shr r9,6
	ALIGN 16
memcopyaclop5f:
	movups xmm0,[rdx]
	movups xmm1,[rdx+16]
	movups xmm2,[rdx+32]
	movups xmm3,[rdx+48]
	movaps [rcx],xmm0
	movaps [rcx+16],xmm1
	movaps [rcx+32],xmm2
	movaps [rcx+48],xmm3
	lea rcx,[rcx+64]
	lea rdx,[rdx+64]
	dec r9
	jnz memcopyaclop5f

	and r8,63
	jz memcopyacexit
	mov r9,r8
	shr r9,4
	jz memcopyaclop2
	align 16
memcopyaclop5g:
	movups xmm0,[rdx]
	movaps [rcx],xmm0
	lea rcx,[rcx+16]
	lea rdx,[rdx+16]
	dec r9
	jnz memcopyaclop5g
	and r8,15
	jz memcopyacexit
	align 16
memcopyaclop5h
	mov al,byte [rdx]
	mov byte [rcx],al
	lea rcx,[rcx+1]
	lea rdx,[rdx+1]
	dec r8
	jnz memcopyaclop5h
	jmp memcopyacexit

	align 16
memcopyaclop2:
memcopyaclop:
	mov r9,r8
	shr r9,3
	jz memcopyaclop4
	align 16
memcopyaclop3:
	mov rax,[rdx]
	mov [rcx],rax
	lea rcx,[rcx+8]
	lea rdx,[rdx+8]
	dec r9
	jnz memcopyaclop3
	align 16
memcopyaclop4:
	and r8,7
	jz memcopyacexit
	align 16
memcopyaclop4a:
	mov al,byte [rdx]
	mov byte [rcx],al
	lea rcx,[rcx+1]
	lea rdx,[rdx+1]
	dec r8
	jnz memcopyaclop4a
	
	align 16
memcopyacexit:
	ret

;void MemCopyANC_SSE(void *destPtr, const void *srcPtr, OSInt leng)
;0 retAddr
;rcx destPtr
;rdx srcPtr
;r8 leng
	align 16
MemCopyANC_SSE:
	cmp rcx,rdx
	jz memcopyancexit
	cmp r8,64
	jb memcopyanclop

	mov r9,r8
	shr r9,6
	ALIGN 16
memcopyanclop5f:
	movups xmm0,[rdx]
	movups xmm1,[rdx+16]
	movups xmm2,[rdx+32]
	movups xmm3,[rdx+48]
	movntps [rcx],xmm0
	movntps [rcx+16],xmm1
	movntps [rcx+32],xmm2
	movntps [rcx+48],xmm3
	lea rcx,[rcx+64]
	lea rdx,[rdx+64]
	dec r9
	jnz memcopyanclop5f

	and r8,63
	jz memcopyancexit
	mov r9,r8
	shr r9,4
	jz memcopyanclop2
	align 16
memcopyanclop5g:
	movups xmm0,[rdx]
	movntps [rcx],xmm0
	lea rcx,[rcx+16]
	lea rdx,[rdx+16]
	dec r9
	jnz memcopyanclop5g
	and r8,15
	jz memcopyancexit
	align 16
memcopyanclop5h
	mov al,byte [rdx]
	mov byte [rcx],al
	lea rcx,[rcx+1]
	lea rdx,[rdx+1]
	dec r8
	jnz memcopyanclop5h
	jmp memcopyancexit

	align 16
memcopyanclop2:
memcopyanclop:
	mov r9,r8
	shr r9,3
	jz memcopyanclop4
	align 16
memcopyanclop3:
	mov rax,[rdx]
	mov [rcx],rax
	lea rcx,[rcx+8]
	lea rdx,[rdx+8]
	dec r9
	jnz memcopyanclop3
	align 16
memcopyanclop4:
	and r8,7
	jz memcopyancexit
	align 16
memcopyanclop4a:
	mov al,byte [rdx]
	mov byte [rcx],al
	lea rcx,[rcx+1]
	lea rdx,[rdx+1]
	dec r8
	jnz memcopyanclop4a
	
	align 16
memcopyancexit:
	ret

;void MemCopyNAC_SSE(void *destPtr, const void *srcPtr, OSInt leng)
;0 retAddr
;rcx destPtr
;rdx srcPtr
;r8 leng
	align 16
MemCopyNAC_SSE:
	mov qword [rsp+8],rsi
	mov qword [rsp+16],rdi
	cmp rcx,rdx
	jz memcopynacexit
	mov rsi,rdx
	mov rdi,rcx
	cmp r8,80
	jb memcopynaclop

	mov rax,0xf
	and rax,rdi
	jz memcopynaclop5a
	mov rcx,16
	sub rcx,rax
	sub r8,rcx
	rep movsb
	align 16
memcopynaclop5a:
	mov rcx,r8
	shr rcx,6
	ALIGN 16
memcopynaclop5f:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	movups xmm2,[rsi+32]
	movups xmm3,[rsi+48]
	movaps [rdi],xmm0
	movaps [rdi+16],xmm1
	movaps [rdi+32],xmm2
	movaps [rdi+48],xmm3
	lea rsi,[rsi+64]
	lea rdi,[rdi+64]
	dec rcx
	jnz memcopynaclop5f

	and r8,63
	jz memcopynacexit
	mov rcx,r8
	shr rcx,4
	jz memcopynaclop2
	align 16
memcopynaclop5g:
	movups xmm0,[rsi]
	movaps [rdi],xmm0
	lea rsi,[rsi+16]
	lea rdi,[rdi+16]
	dec rcx
	jnz memcopynaclop5g
	and r8,15
	jz memcopynacexit
	mov rcx,r8
	rep movsb
	jmp memcopynacexit

	align 16
memcopynaclop2:
memcopynaclop:
	mov rcx,r8
	shr rcx,3
	jz memcopynaclop4
	rep movsq
	align 16
memcopynaclop4:
	mov rcx,7
	and rcx,r8
	jz memcopynacexit
	rep movsb
	
	align 16
memcopynacexit:
	mov rsi,qword [rsp+8]
	mov rdi,qword [rsp+16]
	ret

;void MemCopyNANC_SSE(void *destPtr, const void *srcPtr, OSInt leng)
;0 retAddr
;rcx destPtr
;rdx srcPtr
;r8 leng
	align 16
MemCopyNANC_SSE:
	mov qword [rsp+8],rsi
	mov qword [rsp+16],rdi
	cmp rcx,rdx
	jz memcopynancexit
	mov rsi,rdx
	mov rdi,rcx
	cmp r8,80
	jb memcopynanclop

	mov rax,0xf
	and rax,rdi
	jz memcopynanclop5a
	mov rcx,16
	sub rcx,rax
	sub r8,rcx
	rep movsb
	align 16
memcopynanclop5a:
	mov rcx,r8
	shr rcx,6
	ALIGN 16
memcopynanclop5f:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	movups xmm2,[rsi+32]
	movups xmm3,[rsi+48]
	movntps [rdi],xmm0
	movntps [rdi+16],xmm1
	movntps [rdi+32],xmm2
	movntps [rdi+48],xmm3
	lea rsi,[rsi+64]
	lea rdi,[rdi+64]
	dec rcx
	jnz memcopynanclop5f

	and r8,63
	jz memcopynancexit
	mov rcx,r8
	shr rcx,4
	jz memcopynanclop2
	align 16
memcopynanclop5g:
	movups xmm0,[rsi]
	movntps [rdi],xmm0
	lea rsi,[rsi+16]
	lea rdi,[rdi+16]
	dec rcx
	jnz memcopynanclop5g
	and r8,15
	jz memcopynancexit
	mov rcx,r8
	rep movsb
	jmp memcopynancexit

	align 16
memcopynanclop2:
memcopynanclop:
	mov rcx,r8
	shr rcx,3
	jz memcopynanclop4
	rep movsq
	align 16
memcopynanclop4:
	mov rcx,7
	and rcx,r8
	jz memcopynancexit
	rep movsb
	
	align 16
memcopynancexit:
	mov rsi,qword [rsp+8]
	mov rdi,qword [rsp+16]
	ret

;void MemClearAC_AVX(void *buff, OSInt buffSize); //buff 16-byte align, buffSize 16 bytes
;void MemClearANC_AVX(void *buff, OSInt buffSize); //buff 16-byte align, buffSize 16 bytes
;0 retAddr
;rcx buff
;rdx buffSize
	align 16
MemClearAC_AVX:
MemClearANC_AVX:
	mov r8,rdi
	mov rdi,rcx
	mov rcx,rdx
	xor rax,rax
	shr rcx,3
	rep stosq
	mov rcx,rdx
	and rcx,7
	rep stosb
	mov rdi,r8
	ret
	
;void MemCopyAC_AVX(void *destPtr, const void *srcPtr, OSInt leng)
;0 retAddr
;rcx destPtr
;rdx srcPtr
;r8 leng
	align 16
MemCopyAC_AVX:
	cmp rcx,rdx
	jz memcopyacavxexit
	cmp r8,128
	jb memcopyacavxlop

	mov r9,r8
	shr r9,7
	ALIGN 16
memcopyacavxlop5f:
	vmovdqu ymm0,[rdx]
	vmovdqu ymm1,[rdx+32]
	vmovdqu ymm2,[rdx+64]
	vmovdqu ymm3,[rdx+96]
	vmovdqa [rcx],ymm0
	vmovdqa [rcx+32],ymm1
	vmovdqa [rcx+64],ymm2
	vmovdqa [rcx+96],ymm3
	lea rcx,[rcx+128]
	lea rdx,[rdx+128]
	dec r9
	jnz memcopyacavxlop5f
	vzeroupper

	and r8,127
	jz memcopyacavxexit
	mov r9,r8
	shr r9,4
	jz memcopyacavxlop2
	align 16
memcopyacavxlop5g:
	movups xmm0,[rdx]
	movaps [rcx],xmm0
	lea rcx,[rcx+16]
	lea rdx,[rdx+16]
	dec r9
	jnz memcopyacavxlop5g
	and r8,15
	jz memcopyacavxexit
	align 16
memcopyacavxlop5h
	mov al,byte [rdx]
	mov byte [rcx],al
	lea rcx,[rcx+1]
	lea rdx,[rdx+1]
	dec r8
	jnz memcopyacavxlop5h
	jmp memcopyacavxexit

	align 16
memcopyacavxlop2:
memcopyacavxlop:
	mov r9,r8
	shr r9,3
	jz memcopyacavxlop4
	align 16
memcopyacavxlop3:
	mov rax,[rdx]
	mov [rcx],rax
	lea rcx,[rcx+8]
	lea rdx,[rdx+8]
	dec r9
	jnz memcopyacavxlop3
	align 16
memcopyacavxlop4:
	and r8,7
	jz memcopyacavxexit
	align 16
memcopyacavxlop4a:
	mov al,byte [rdx]
	mov byte [rcx],al
	lea rcx,[rcx+1]
	lea rdx,[rdx+1]
	dec r8
	jnz memcopyacavxlop4a
	
	align 16
memcopyacavxexit:
	ret

;void MemCopyANC_AVX(void *destPtr, const void *srcPtr, OSInt leng)
;0 retAddr
;rcx destPtr
;rdx srcPtr
;r8 leng
	align 16
MemCopyANC_AVX:
	cmp rcx,rdx
	jz memcopyancavxexit
	cmp r8,128
	jb memcopyancavxlop

	mov r9,r8
	shr r9,7
	ALIGN 16
memcopyancavxlop5f:
	vmovntdqa ymm0,[rdx]
	vmovntdqa ymm1,[rdx+32]
	vmovntdqa ymm2,[rdx+64]
	vmovntdqa ymm3,[rdx+96]
	vmovntps [rcx],ymm0
	vmovntps [rcx+32],ymm1
	vmovntps [rcx+64],ymm2
	vmovntps [rcx+96],ymm3
	lea rcx,[rcx+128]
	lea rdx,[rdx+128]
	dec r9
	jnz memcopyancavxlop5f
	vzeroupper

	and r8,127
	jz memcopyancavxexit
	mov r9,r8
	shr r9,4
	jz memcopyancavxlop2
	align 16
memcopyancavxlop5g:
	movups xmm0,[rdx]
	movntps [rcx],xmm0
	lea rcx,[rcx+16]
	lea rdx,[rdx+16]
	dec r9
	jnz memcopyancavxlop5g
	and r8,15
	jz memcopyancavxexit
	align 16
memcopyancavxlop5h
	mov al,byte [rdx]
	mov byte [rcx],al
	lea rcx,[rcx+1]
	lea rdx,[rdx+1]
	dec r8
	jnz memcopyancavxlop5h
	jmp memcopyancavxexit

	align 16
memcopyancavxlop2:
memcopyancavxlop:
	mov r9,r8
	shr r9,3
	jz memcopyancavxlop4
	align 16
memcopyancavxlop3:
	mov rax,[rdx]
	mov [rcx],rax
	lea rcx,[rcx+8]
	lea rdx,[rdx+8]
	dec r9
	jnz memcopyancavxlop3
	align 16
memcopyancavxlop4:
	and r8,7
	jz memcopyancavxexit
	align 16
memcopyancavxlop4a:
	mov al,byte [rdx]
	mov byte [rcx],al
	lea rcx,[rcx+1]
	lea rdx,[rdx+1]
	dec r8
	jnz memcopyancavxlop4a
	
	align 16
memcopyancavxexit:
	ret

;void MemCopyNAC_AVX(void *destPtr, const void *srcPtr, OSInt leng)
;0 retAddr
;rcx destPtr
;rdx srcPtr
;r8 leng
	align 16
MemCopyNAC_AVX:
	mov qword [rsp+8],rsi
	mov qword [rsp+16],rdi
	cmp rcx,rdx
	jz memcopynacavxexit
	mov rsi,rdx
	mov rdi,rcx
	cmp r8,160
	jb memcopynacavxlop

	mov rax,0x1f
	and rax,rdi
	jz memcopynacavxlop5a
	mov rcx,32
	sub rcx,rax
	sub r8,rcx
	rep movsb
	align 16
memcopynacavxlop5a:
	mov rcx,r8
	shr rcx,7
	ALIGN 16
memcopynacavxlop5f:
	vmovups ymm0,[rsi]
	vmovups ymm1,[rsi+32]
	vmovups ymm2,[rsi+64]
	vmovups ymm3,[rsi+96]
	vmovaps [rdi],ymm0
	vmovaps [rdi+32],ymm1
	vmovaps [rdi+64],ymm2
	vmovaps [rdi+96],ymm3
	lea rsi,[rsi+128]
	lea rdi,[rdi+128]
	dec rcx
	jnz memcopynacavxlop5f
	vzeroupper

	and r8,127
	jz memcopynacavxexit
	mov rcx,r8
	shr rcx,4
	jz memcopynacavxlop2
	align 16
memcopynacavxlop5g:
	movups xmm0,[rsi]
	movaps [rdi],xmm0
	lea rsi,[rsi+16]
	lea rdi,[rdi+16]
	dec rcx
	jnz memcopynacavxlop5g
	and r8,15
	jz memcopynacavxexit
	mov rcx,r8
	rep movsb
	jmp memcopynacavxexit

	align 16
memcopynacavxlop2:
memcopynacavxlop:
	mov rcx,r8
	shr rcx,3
	jz memcopynacavxlop4
	rep movsq
	align 16
memcopynacavxlop4:
	mov rcx,7
	and rcx,r8
	jz memcopynacavxexit
	rep movsb
	
	align 16
memcopynacavxexit:
	mov rsi,qword [rsp+8]
	mov rdi,qword [rsp+16]
	ret

;void MemCopyNANC_AVX(void *destPtr, const void *srcPtr, OSInt leng)
;0 retAddr
;rcx destPtr
;rdx srcPtr
;r8 leng
	align 16
MemCopyNANC_AVX:
	mov r9,rsi
	mov r10,rdi
	cmp rcx,rdx
	jz memcopynancavxexit
	mov rsi,rdx
	mov rdi,rcx
	cmp r8,160
	jb memcopynancavxlop

	mov rax,0x1f
	and rax,rdi
	jz memcopynancavxlop5a
	mov rcx,32
	sub rcx,rax
	sub r8,rcx
	rep movsb
	align 16
memcopynancavxlop5a:
	mov rcx,r8
	shr rcx,7
	ALIGN 16
memcopynancavxlop5f:
	vmovups ymm0,[rsi]
	vmovups ymm1,[rsi+32]
	vmovups ymm2,[rsi+64]
	vmovups ymm3,[rsi+96]
	vmovntps [rdi],ymm0
	vmovntps [rdi+32],ymm1
	vmovntps [rdi+64],ymm2
	vmovntps [rdi+96],ymm3
	lea rsi,[rsi+128]
	lea rdi,[rdi+128]
	dec rcx
	jnz memcopynancavxlop5f
	vzeroupper

	and r8,127
	jz memcopynancavxexit
	mov rcx,r8
	shr rcx,4
	jz memcopynancavxlop2
	align 16
memcopynancavxlop5g:
	movups xmm0,[rsi]
	movntps [rdi],xmm0
	lea rsi,[rsi+16]
	lea rdi,[rdi+16]
	dec rcx
	jnz memcopynancavxlop5g
	and r8,15
	jz memcopynancavxexit
	mov rcx,r8
	rep movsb
	jmp memcopynancavxexit

	align 16
memcopynancavxlop2:
memcopynancavxlop:
	mov rcx,r8
	shr rcx,3
	jz memcopynancavxlop4
	rep movsq
	align 16
memcopynancavxlop4:
	mov rcx,7
	and rcx,r8
	jz memcopynancavxexit
	rep movsb
	
	align 16
memcopynancavxexit:
	mov rsi,r9
	mov rdi,r10
	ret

;void MemClearAC_AMDSSE(void *buff, OSInt buffSize); //buff 16-byte align, buffSize 16 bytes
;0 retAddr
;rcx buff
;rdx buffSize
	align 16
MemClearAC_AMDSSE:
	shr rdx,4
	pxor xmm0,xmm0
	align 16
memclearacamdlop:
	movdqa [rcx],xmm0
	lea rcx,[rcx+16]
	dec rdx
	jnz memclearacamdlop
	ret
	
;void MemClearANC_AMDSSE(void *buff, OSInt buffSize); //buff 16-byte align, buffSize 16 bytes
;0 retAddr
;rcx buff
;rdx buffSize
	align 16
MemClearANC_AMDSSE:
	shr rdx,4
	pxor xmm0,xmm0
	align 16
memclearancamdlop:
	movntdq [rcx],xmm0
	lea rcx,[rcx+16]
	dec rdx
	jnz memclearancamdlop
	ret
	
;void MemCopyAC_SSE(void *destPtr, const void *srcPtr, OSInt leng)
;0 retAddr
;rcx destPtr
;rdx srcPtr
;r8 leng
	align 16
MemCopyAC_AMDSSE:
	cmp rcx,rdx
	jz memcopyacamdexit
	cmp r8,64
	jb memcopyacamdlop

	mov r9,r8
	shr r9,6
	ALIGN 16
memcopyacamdlop5f:
	movdqu xmm0,[rdx]
	movdqu xmm1,[rdx+16]
	movdqu xmm2,[rdx+32]
	movdqu xmm3,[rdx+48]
	movdqa [rcx],xmm0
	movdqa [rcx+16],xmm1
	movdqa [rcx+32],xmm2
	movdqa [rcx+48],xmm3
	lea rcx,[rcx+64]
	lea rdx,[rdx+64]
	dec r9
	jnz memcopyacamdlop5f

	and r8,63
	jz memcopyacamdexit
	mov r9,r8
	shr r9,4
	jz memcopyacamdlop2
	align 16
memcopyacamdlop5g:
	movdqu xmm0,[rdx]
	movdqa [rcx],xmm0
	lea rcx,[rcx+16]
	lea rdx,[rdx+16]
	dec r9
	jnz memcopyacamdlop5g
	and r8,15
	jz memcopyacamdexit
	align 16
memcopyacamdlop5h
	mov al,byte [rdx]
	mov byte [rcx],al
	lea rcx,[rcx+1]
	lea rdx,[rdx+1]
	dec r8
	jnz memcopyacamdlop5h
	jmp memcopyacamdexit

	align 16
memcopyacamdlop2:
memcopyacamdlop:
	mov r9,r8
	shr r9,3
	jz memcopyacamdlop4
	align 16
memcopyacamdlop3:
	mov rax,[rdx]
	mov [rcx],rax
	lea rcx,[rcx+8]
	lea rdx,[rdx+8]
	dec r9
	jnz memcopyacamdlop3
	align 16
memcopyacamdlop4:
	and r8,7
	jz memcopyacamdexit
	align 16
memcopyacamdlop4a:
	mov al,byte [rdx]
	mov byte [rcx],al
	lea rcx,[rcx+1]
	lea rdx,[rdx+1]
	dec r8
	jnz memcopyacamdlop4a
	
	align 16
memcopyacamdexit:
	ret

;void MemCopyANC_AMDSSE(void *destPtr, const void *srcPtr, OSInt leng)
;0 retAddr
;rcx destPtr
;rdx srcPtr
;r8 leng
	align 16
MemCopyANC_AMDSSE:
	cmp rcx,rdx
	jz memcopyancamdexit
	cmp r8,64
	jb memcopyancamdlop

	mov r9,r8
	shr r9,6
	ALIGN 16
memcopyancamdlop5f:
	movdqu xmm0,[rdx]
	movdqu xmm1,[rdx+16]
	movdqu xmm2,[rdx+32]
	movdqu xmm3,[rdx+48]
	movntdq [rcx],xmm0
	movntdq [rcx+16],xmm1
	movntdq [rcx+32],xmm2
	movntdq [rcx+48],xmm3
	lea rcx,[rcx+64]
	lea rdx,[rdx+64]
	dec r9
	jnz memcopyancamdlop5f

	and r8,63
	jz memcopyancamdexit
	mov r9,r8
	shr r9,4
	jz memcopyancamdlop2
	align 16
memcopyancamdlop5g:
	movdqu xmm0,[rdx]
	movntdq [rcx],xmm0
	lea rcx,[rcx+16]
	lea rdx,[rdx+16]
	dec r9
	jnz memcopyancamdlop5g
	and r8,15
	jz memcopyancamdexit
	align 16
memcopyancamdlop5h
	mov al,byte [rdx]
	mov byte [rcx],al
	lea rcx,[rcx+1]
	lea rdx,[rdx+1]
	dec r8
	jnz memcopyancamdlop5h
	jmp memcopyancamdexit

	align 16
memcopyancamdlop2:
memcopyancamdlop:
	mov r9,r8
	shr r9,3
	jz memcopyancamdlop4
	align 16
memcopyancamdlop3:
	mov rax,[rdx]
	mov [rcx],rax
	lea rcx,[rcx+8]
	lea rdx,[rdx+8]
	dec r9
	jnz memcopyancamdlop3
	align 16
memcopyancamdlop4:
	and r8,7
	jz memcopyancamdexit
	align 16
memcopyancamdlop4a:
	mov al,byte [rdx]
	mov byte [rcx],al
	lea rcx,[rcx+1]
	lea rdx,[rdx+1]
	dec r8
	jnz memcopyancamdlop4a
	
	align 16
memcopyancamdexit:
	ret

;void MemCopyNAC_AMDSSE(void *destPtr, const void *srcPtr, OSInt leng)
;0 retAddr
;rcx destPtr
;rdx srcPtr
;r8 leng
	align 16
MemCopyNAC_AMDSSE:
	mov qword [rsp+8],rsi
	mov qword [rsp+16],rdi
	cmp rcx,rdx
	jz memcopynacamdexit
	mov rsi,rdx
	mov rdi,rcx
	cmp r8,80
	jb memcopynacamdlop

	mov rax,0xf
	and rax,rdi
	jz memcopynacamdlop5a
	mov rcx,16
	sub rcx,rax
	sub r8,rcx
	rep movsb
	align 16
memcopynacamdlop5a:
	mov rcx,r8
	shr rcx,6
	ALIGN 16
memcopynacamdlop5f:
	movdqu xmm0,[rsi]
	movdqu xmm1,[rsi+16]
	movdqu xmm2,[rsi+32]
	movdqu xmm3,[rsi+48]
	movdqa [rdi],xmm0
	movdqa [rdi+16],xmm1
	movdqa [rdi+32],xmm2
	movdqa [rdi+48],xmm3
	lea rsi,[rsi+64]
	lea rdi,[rdi+64]
	dec rcx
	jnz memcopynacamdlop5f

	and r8,63
	jz memcopynacamdexit
	mov rcx,r8
	shr rcx,4
	jz memcopynacamdlop2
	align 16
memcopynacamdlop5g:
	movdqu xmm0,[rsi]
	movdqa [rdi],xmm0
	lea rsi,[rsi+16]
	lea rdi,[rdi+16]
	dec rcx
	jnz memcopynacamdlop5g
	and r8,15
	jz memcopynacamdexit
	mov rcx,r8
	rep movsb
	jmp memcopynacamdexit

	align 16
memcopynacamdlop2:
memcopynacamdlop:
	mov rcx,r8
	shr rcx,3
	jz memcopynacamdlop4
	rep movsq
	align 16
memcopynacamdlop4:
	mov rcx,7
	and rcx,r8
	jz memcopynacamdexit
	rep movsb
	
	align 16
memcopynacamdexit:
	mov rsi,qword [rsp+8]
	mov rdi,qword [rsp+16]
	ret

;void MemCopyNANC_AMDSSE(void *destPtr, const void *srcPtr, OSInt leng)
;0 retAddr
;rcx destPtr
;rdx srcPtr
;r8 leng
	align 16
MemCopyNANC_AMDSSE:
	mov qword [rsp+8],rsi
	mov qword [rsp+16],rdi
	cmp rcx,rdx
	jz memcopynancamdexit
	mov rsi,rdx
	mov rdi,rcx
	cmp r8,80
	jb memcopynancamdlop

	mov rax,0xf
	and rax,rdi
	jz memcopynancamdlop5a
	mov rcx,16
	sub rcx,rax
	sub r8,rcx
	rep movsb
	align 16
memcopynancamdlop5a:
	mov rcx,r8
	shr rcx,6
	ALIGN 16
memcopynancamdlop5f:
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
	jnz memcopynancamdlop5f

	and r8,63
	jz memcopynancamdexit
	mov rcx,r8
	shr rcx,4
	jz memcopynancamdlop2
	align 16
memcopynancamdlop5g:
	movdqu xmm0,[rsi]
	movntdq [rdi],xmm0
	lea rsi,[rsi+16]
	lea rdi,[rdi+16]
	dec rcx
	jnz memcopynancamdlop5g
	and r8,15
	jz memcopynancamdexit
	mov rcx,r8
	rep movsb
	jmp memcopynancamdexit

	align 16
memcopynancamdlop2:
memcopynancamdlop:
	mov rcx,r8
	shr rcx,3
	jz memcopynancamdlop4
	rep movsq
	align 16
memcopynancamdlop4:
	mov rcx,7
	and rcx,r8
	jz memcopynancamdexit
	rep movsb
	
	align 16
memcopynancamdexit:
	mov rsi,qword [rsp+8]
	mov rdi,qword [rsp+16]
	ret

;void MemClearAC_AMDAVX(void *buff, OSInt buffSize); //buff 16-byte align, buffSize 16 bytes
;0 retAddr
;rcx buff
;rdx buffSize
	align 16
MemClearAC_AMDAVX:
	shr rdx,5
	vpxor ymm0,ymm0,ymm0
	align 16
memclearacamdavxlop:
	vmovdqa [rcx],ymm0
	lea rcx,[rcx+32]
	dec rdx
	jnz memclearacamdavxlop
	vzeroupper
	ret
	
;void MemClearANC_AMDAVX(void *buff, OSInt buffSize); //buff 16-byte align, buffSize 16 bytes
;0 retAddr
;rcx buff
;rdx buffSize
	align 16
MemClearANC_AMDAVX:
	shr rdx,5
	vpxor ymm0,ymm0,ymm0
	align 16
memclearancamdavxlop:
	vmovntdq [rcx],ymm0
	lea rcx,[rcx+32]
	dec rdx
	jnz memclearancamdavxlop
	vzeroupper
	ret
	
;void MemCopyAC_AMDAVX(void *destPtr, const void *srcPtr, OSInt leng)
;0 retAddr
;rcx destPtr
;rdx srcPtr
;r8 leng
	align 16
MemCopyAC_AMDAVX:
	cmp rcx,rdx
	jz memcopyacamdavxexit
	cmp r8,128
	jb memcopyacamdavxlop

	mov r9,r8
	shr r9,7
	ALIGN 16
memcopyacamdavxlop5f:
	vmovdqu ymm0,[rdx]
	vmovdqu ymm1,[rdx+32]
	vmovdqu ymm2,[rdx+64]
	vmovdqu ymm3,[rdx+96]
	vmovdqa [rcx],ymm0
	vmovdqa [rcx+32],ymm1
	vmovdqa [rcx+64],ymm2
	vmovdqa [rcx+96],ymm3
	lea rcx,[rcx+128]
	lea rdx,[rdx+128]
	dec r9
	jnz memcopyacamdavxlop5f
	vzeroupper

	and r8,127
	jz memcopyacamdavxexit
	mov r9,r8
	shr r9,4
	jz memcopyacamdavxlop2
	align 16
memcopyacamdavxlop5g:
	movdqu xmm0,[rdx]
	movdqa [rcx],xmm0
	lea rcx,[rcx+16]
	lea rdx,[rdx+16]
	dec r9
	jnz memcopyacamdavxlop5g
	and r8,15
	jz memcopyacamdavxexit
	align 16
memcopyacamdavxlop5h
	mov al,byte [rdx]
	mov byte [rcx],al
	lea rcx,[rcx+1]
	lea rdx,[rdx+1]
	dec r8
	jnz memcopyacamdavxlop5h
	jmp memcopyacamdavxexit

	align 16
memcopyacamdavxlop2:
memcopyacamdavxlop:
	mov r9,r8
	shr r9,3
	jz memcopyacamdavxlop4
	align 16
memcopyacamdavxlop3:
	mov rax,[rdx]
	mov [rcx],rax
	lea rcx,[rcx+8]
	lea rdx,[rdx+8]
	dec r9
	jnz memcopyacamdavxlop3
	align 16
memcopyacamdavxlop4:
	and r8,7
	jz memcopyacamdavxexit
	align 16
memcopyacamdavxlop4a:
	mov al,byte [rdx]
	mov byte [rcx],al
	lea rcx,[rcx+1]
	lea rdx,[rdx+1]
	dec r8
	jnz memcopyacamdavxlop4a
	
	align 16
memcopyacamdavxexit:
	ret

;void MemCopyANC_AMDAVX(void *destPtr, const void *srcPtr, OSInt leng)
;0 retAddr
;rcx destPtr
;rdx srcPtr
;r8 leng
	align 16
MemCopyANC_AMDAVX:
	cmp rcx,rdx
	jz memcopyancamdavxexit
	cmp r8,128
	jb memcopyancamdavxlop

	mov r9,r8
	shr r9,7
	ALIGN 16
memcopyancamdavxlop5f:
	vmovntdqa ymm0,[rdx]
	vmovntdqa ymm1,[rdx+32]
	vmovntdqa ymm2,[rdx+64]
	vmovntdqa ymm3,[rdx+96]
	vmovntdq [rcx],ymm0
	vmovntdq [rcx+32],ymm1
	vmovntdq [rcx+64],ymm2
	vmovntdq [rcx+96],ymm3
	lea rcx,[rcx+128]
	lea rdx,[rdx+128]
	dec r9
	jnz memcopyancamdavxlop5f
	vzeroupper

	and r8,127
	jz memcopyancamdavxexit
	mov r9,r8
	shr r9,4
	jz memcopyancamdavxlop2
	align 16
memcopyancamdavxlop5g:
	movdqu xmm0,[rdx]
	movntdq [rcx],xmm0
	lea rcx,[rcx+16]
	lea rdx,[rdx+16]
	dec r9
	jnz memcopyancamdavxlop5g
	and r8,15
	jz memcopyancamdavxexit
	align 16
memcopyancamdavxlop5h
	mov al,byte [rdx]
	mov byte [rcx],al
	lea rcx,[rcx+1]
	lea rdx,[rdx+1]
	dec r8
	jnz memcopyancamdavxlop5h
	jmp memcopyancamdavxexit

	align 16
memcopyancamdavxlop2:
memcopyancamdavxlop:
	mov r9,r8
	shr r9,3
	jz memcopyancamdavxlop4
	align 16
memcopyancamdavxlop3:
	mov rax,[rdx]
	mov [rcx],rax
	lea rcx,[rcx+8]
	lea rdx,[rdx+8]
	dec r9
	jnz memcopyancamdavxlop3
	align 16
memcopyancamdavxlop4:
	and r8,7
	jz memcopyancamdavxexit
	align 16
memcopyancamdavxlop4a:
	mov al,byte [rdx]
	mov byte [rcx],al
	lea rcx,[rcx+1]
	lea rdx,[rdx+1]
	dec r8
	jnz memcopyancamdavxlop4a
	
	align 16
memcopyancamdavxexit:
	ret

;void MemCopyNAC_AMDAVX(void *destPtr, const void *srcPtr, OSInt leng)
;0 retAddr
;rcx destPtr
;rdx srcPtr
;r8 leng
	align 16
MemCopyNAC_AMDAVX:
	mov qword [rsp+8],rsi
	mov qword [rsp+16],rdi
	cmp rcx,rdx
	jz memcopynacamdavxexit
	mov rsi,rdx
	mov rdi,rcx
	cmp r8,160
	jb memcopynacamdavxlop

	mov rax,0x1f
	and rax,rdi
	jz memcopynacamdavxlop5a
	mov rcx,32
	sub rcx,rax
	sub r8,rcx
	rep movsb
	align 16
memcopynacamdavxlop5a:
	mov rcx,r8
	shr rcx,7
	ALIGN 16
memcopynacamdavxlop5f:
	vmovdqu ymm0,[rsi]
	vmovdqu ymm1,[rsi+32]
	vmovdqu ymm2,[rsi+64]
	vmovdqu ymm3,[rsi+96]
	vmovdqa [rdi],ymm0
	vmovdqa [rdi+32],ymm1
	vmovdqa [rdi+64],ymm2
	vmovdqa [rdi+96],ymm3
	lea rsi,[rsi+128]
	lea rdi,[rdi+128]
	dec rcx
	jnz memcopynacamdavxlop5f
	vzeroupper

	and r8,127
	jz memcopynacamdavxexit
	mov rcx,r8
	shr rcx,4
	jz memcopynacamdavxlop2
	align 16
memcopynacamdavxlop5g:
	movups xmm0,[rsi]
	movaps [rdi],xmm0
	lea rsi,[rsi+16]
	lea rdi,[rdi+16]
	dec rcx
	jnz memcopynacamdavxlop5g
	and r8,15
	jz memcopynacamdavxexit
	mov rcx,r8
	rep movsb
	jmp memcopynacamdavxexit

	align 16
memcopynacamdavxlop2:
memcopynacamdavxlop:
	mov rcx,r8
	shr rcx,3
	jz memcopynacamdavxlop4
	rep movsq
	align 16
memcopynacamdavxlop4:
	mov rcx,7
	and rcx,r8
	jz memcopynacamdavxexit
	rep movsb
	
	align 16
memcopynacamdavxexit:
	mov rsi,qword [rsp+8]
	mov rdi,qword [rsp+16]
	ret

;void MemCopyNANC_AMDAVX(void *destPtr, const void *srcPtr, OSInt leng)
;0 retAddr
;rcx destPtr
;rdx srcPtr
;r8 leng
	align 16
MemCopyNANC_AMDAVX:
	mov qword [rsp+8],rsi
	mov qword [rsp+16],rdi
	cmp rcx,rdx
	jz memcopynancamdavxexit
	mov rsi,rdx
	mov rdi,rcx
	cmp r8,160
	jb memcopynancamdavxlop

	mov rax,0x1f
	and rax,rdi
	jz memcopynancamdavxlop5a
	mov rcx,32
	sub rcx,rax
	sub r8,rcx
	rep movsb
	align 16
memcopynancamdavxlop5a:
	mov rcx,r8
	shr rcx,7
	ALIGN 16
memcopynancamdavxlop5f:
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
	jnz memcopynancamdavxlop5f
	vzeroupper

	and r8,127
	jz memcopynancamdavxexit
	mov rcx,r8
	shr rcx,4
	jz memcopynancamdavxlop2
	align 16
memcopynancamdavxlop5g:
	movdqu xmm0,[rsi]
	movntdq [rdi],xmm0
	lea rsi,[rsi+16]
	lea rdi,[rdi+16]
	dec rcx
	jnz memcopynancamdavxlop5g
	and r8,15
	jz memcopynancamdavxexit
	mov rcx,r8
	rep movsb
	jmp memcopynancamdavxexit

	align 16
memcopynancamdavxlop2:
memcopynancamdavxlop:
	mov rcx,r8
	shr rcx,3
	jz memcopynancamdavxlop4
	rep movsq
	align 16
memcopynancamdavxlop4:
	mov rcx,7
	and rcx,r8
	jz memcopynancamdavxexit
	rep movsb
	
	align 16
memcopynancamdavxexit:
	mov rsi,qword [rsp+8]
	mov rdi,qword [rsp+16]
	ret

