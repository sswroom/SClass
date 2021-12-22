section .text

global MemFillB
global _MemFillB
global MemFillW
global _MemFillW
global MemClearAC_SSE
global _MemClearAC_SSE
global MemClearANC_SSE
global _MemClearANC_SSE
global MemCopyAC_SSE
global _MemCopyAC_SSE
global MemCopyANC_SSE
global _MemCopyANC_SSE
global MemCopyNAC_SSE
global _MemCopyNAC_SSE
global MemCopyNANC_SSE
global _MemCopyNANC_SSE
global MemClearAC_AVX
global _MemClearAC_AVX
global MemClearANC_AVX
global _MemClearANC_AVX
global MemCopyAC_AVX
global _MemCopyAC_AVX
global MemCopyANC_AVX
global _MemCopyANC_AVX
global MemCopyNAC_AVX
global _MemCopyNAC_AVX
global MemCopyNANC_AVX
global _MemCopyNANC_AVX
global MemClearAC_AMDSSE
global _MemClearAC_AMDSSE
global MemClearANC_AMDSSE
global _MemClearANC_AMDSSE
global MemCopyAC_AMDSSE
global _MemCopyAC_AMDSSE
global MemCopyANC_AMDSSE
global _MemCopyANC_AMDSSE
global MemCopyNAC_AMDSSE
global _MemCopyNAC_AMDSSE
global MemCopyNANC_AMDSSE
global _MemCopyNANC_AMDSSE
global MemClearAC_AMDAVX
global _MemClearAC_AMDAVX
global MemClearANC_AMDAVX
global _MemClearANC_AMDAVX
global MemCopyAC_AMDAVX
global _MemCopyAC_AMDAVX
global MemCopyANC_AMDAVX
global _MemCopyANC_AMDAVX
global MemCopyNAC_AMDAVX
global _MemCopyNAC_AMDAVX
global MemCopyNANC_AMDAVX
global _MemCopyNANC_AMDAVX

;void MemFillB(UInt8 *buff, OSInt byteCnt, UInt8 val)
;0 retAddr
;rdi buff
;rsi byteCnt
;rdx val
	align 16
MemFillB:
_MemFillB:
	mov rax,rdx
	mov rcx,rsi
	rep stosb
	ret

;void MemFillW(UInt8 *buff, OSInt wordCnt, UInt16 val);
;0 retAddr
;rdi buff
;rsi wordCnt
;rdx val
	align 16
MemFillW:
_MemFillW:
	mov rax,rdx
	mov rcx,rsi
	rep stosw
	ret

;void MemClearAC_SSE(void *buff, OSInt buffSize); //buff 16-byte align, buffSize 16 bytes
;0 retAddr
;rdi buff
;rsi buffSize
	align 16
MemClearAC_SSE:
_MemClearAC_SSE:
MemClearAC_AVX:
_MemClearAC_AVX:
MemClearAC_AMDSSE:
_MemClearAC_AMDSSE:
MemClearAC_AMDAVX:
_MemClearAC_AMDAVX:
	shr rsi,4
	pxor xmm0,xmm0
	align 16
memclearaclop:
	movaps [rdi],xmm0
	lea rdi,[rdi+16]
	dec rsi
	jnz memclearaclop
	ret
	
;void MemClearANC_SSE(void *buff, OSInt buffSize); //buff 16-byte align, buffSize 16 bytes
;0 retAddr
;rdi buff
;rsi buffSize
	align 16
MemClearANC_SSE:
_MemClearANC_SSE:
MemClearANC_AVX:
_MemClearANC_AVX:
MemClearANC_AMDSSE:
_MemClearANC_AMDSSE:
MemClearANC_AMDAVX:
_MemClearANC_AMDAVX:
	shr rsi,4
	pxor xmm0,xmm0
	align 16
memclearanclop:
	movntps [rdi],xmm0
	lea rdi,[rdi+16]
	dec rsi
	jnz memclearanclop
	ret
	
;void MemCopyAC_SSE(void *destPtr, const void *srcPtr, OSInt leng)
;0 retAddr
;rdi destPtr
;rsi srcPtr
;rdx leng
	align 16
MemCopyAC_SSE:
_MemCopyAC_SSE:
MemCopyAC_AMDSSE:
_MemCopyAC_AMDSSE:
	cmp rdi,rsi
	jz memcopyacexit
	cmp rdx,128
	jb memcopyaclop

	mov r9,rdx
	shr r9,7
	ALIGN 16
memcopyaclop5f:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	movups xmm2,[rsi+32]
	movups xmm3,[rsi+48]
	movups xmm4,[rsi+64]
	movups xmm5,[rsi+80]
	movups xmm6,[rsi+96]
	movups xmm7,[rsi+112]
	movaps [rdi],xmm0
	movaps [rdi+16],xmm1
	movaps [rdi+32],xmm2
	movaps [rdi+48],xmm3
	movaps [rdi+64],xmm4
	movaps [rdi+80],xmm5
	movaps [rdi+96],xmm6
	movaps [rdi+112],xmm7
	lea rsi,[rsi+128]
	lea rdi,[rdi+128]
	dec r9
	jnz memcopyaclop5f

	and rdx,127
	jz memcopyacexit
	mov r9,rdx
	shr r9,4
	jz memcopyaclop2
	align 16
memcopyaclop5g:
	movups xmm0,[rsi]
	movaps [rdi],xmm0
	lea rsi,[rsi+16]
	lea rdi,[rdi+16]
	dec r9
	jnz memcopyaclop5g
	and rdx,15
	jz memcopyacexit
	align 16
memcopyaclop5h
	mov al,byte [rsi]
	mov byte [rdi],al
	lea rsi,[rsi+1]
	lea rdi,[rdi+1]
	dec rdx
	jnz memcopyaclop5h
	jmp memcopyacexit

	align 16
memcopyaclop2:
memcopyaclop:
	mov r9,rdx
	shr r9,3
	jz memcopyaclop4
	align 16
memcopyaclop3:
	mov rax,[rsi]
	mov [rdi],rax
	lea rsi,[rsi+8]
	lea rdi,[rdi+8]
	dec r9
	jnz memcopyaclop3
	align 16
memcopyaclop4:
	and rdx,7
	jz memcopyacexit
	align 16
memcopyaclop4a:
	mov al,byte [rsi]
	mov byte [rdi],al
	lea rsi,[rsi+1]
	lea rdi,[rdi+1]
	dec rdx
	jnz memcopyaclop4a
	
	align 16
memcopyacexit:
	ret

;void MemCopyANC_SSE(void *destPtr, const void *srcPtr, OSInt leng)
;0 retAddr
;rdi destPtr
;rsi srcPtr
;rdx leng
	align 16
MemCopyANC_SSE:
_MemCopyANC_SSE:
MemCopyANC_AMDSSE:
_MemCopyANC_AMDSSE:
	cmp rsi,rdi
	jz memcopyancexit
	cmp rdx,128
	jb memcopyanclop

	mov r9,rdx
	shr r9,7
	ALIGN 16
memcopyanclop5f:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	movups xmm2,[rsi+32]
	movups xmm3,[rsi+48]
	movups xmm4,[rsi+64]
	movups xmm5,[rsi+80]
	movups xmm6,[rsi+96]
	movups xmm7,[rsi+112]
	movntps [rdi],xmm0
	movntps [rdi+16],xmm1
	movntps [rdi+32],xmm2
	movntps [rdi+48],xmm3
	movntps [rdi+64],xmm4
	movntps [rdi+80],xmm5
	movntps [rdi+96],xmm6
	movntps [rdi+112],xmm7
	lea rsi,[rsi+128]
	lea rdi,[rdi+128]
	dec r9
	jnz memcopyanclop5f

	and rdx,127
	jz memcopyancexit
	mov r9,rdx
	shr r9,4
	jz memcopyanclop2
	align 16
memcopyanclop5g:
	movups xmm0,[rsi]
	movntps [rdi],xmm0
	lea rsi,[rsi+16]
	lea rdi,[rdi+16]
	dec r9
	jnz memcopyanclop5g
	and rdx,15
	jz memcopyancexit
	align 16
memcopyanclop5h
	mov al,byte [rsi]
	mov byte [rdi],al
	lea rsi,[rsi+1]
	lea rdi,[rdi+1]
	dec rdx
	jnz memcopyanclop5h
	jmp memcopyancexit

	align 16
memcopyanclop2:
memcopyanclop:
	mov r9,rdx
	shr r9,3
	jz memcopyanclop4
	align 16
memcopyanclop3:
	mov rax,[rsi]
	mov [rdi],rax
	lea rsi,[rsi+8]
	lea rdi,[rdi+8]
	dec r9
	jnz memcopyanclop3
	align 16
memcopyanclop4:
	and rdx,7
	jz memcopyancexit
	align 16
memcopyanclop4a:
	mov al,byte [rsi]
	mov byte [rdi],al
	lea rsi,[rsi+1]
	lea rdi,[rdi+1]
	dec rdx
	jnz memcopyanclop4a
	
	align 16
memcopyancexit:
	ret

;void MemCopyNAC_SSE(void *destPtr, const void *srcPtr, OSInt leng)
;0 retAddr
;rdi destPtr
;rsi srcPtr
;rdx leng
	align 16
MemCopyNAC_SSE:
_MemCopyNAC_SSE:
MemCopyNAC_AMDSSE:
_MemCopyNAC_AMDSSE:
	cmp rsi,rdi
	jz memcopynacexit
	cmp rdx,144
	jb memcopynaclop

	mov rax,0xf
	and rax,rdi
	jz memcopynaclop5a
	mov rcx,16
	sub rcx,rax
	sub rdx,rcx
	rep movsb
	align 16
memcopynaclop5a:
	mov rcx,rdx
	shr rcx,7
	ALIGN 16
memcopynaclop5f:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	movups xmm2,[rsi+32]
	movups xmm3,[rsi+48]
	movups xmm4,[rsi+64]
	movups xmm5,[rsi+80]
	movups xmm6,[rsi+96]
	movups xmm7,[rsi+112]
	movaps [rdi],xmm0
	movaps [rdi+16],xmm1
	movaps [rdi+32],xmm2
	movaps [rdi+48],xmm3
	movaps [rdi+64],xmm4
	movaps [rdi+80],xmm5
	movaps [rdi+96],xmm6
	movaps [rdi+112],xmm7
	lea rsi,[rsi+128]
	lea rdi,[rdi+128]
	dec rcx
	jnz memcopynaclop5f

	and rdx,127
	jz memcopynacexit
	mov rcx,rdx
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
	and rdx,15
	jz memcopynacexit
	mov rcx,rdx
	rep movsb
	jmp memcopynacexit

	align 16
memcopynaclop2:
memcopynaclop:
	mov rcx,rdx
	shr rcx,3
	jz memcopynaclop4
	rep movsq
	align 16
memcopynaclop4:
	mov rcx,7
	and rcx,rdx
	jz memcopynacexit
	rep movsb
	
	align 16
memcopynacexit:
	ret

;void MemCopyNANC_SSE(void *destPtr, const void *srcPtr, OSInt leng)
;0 retAddr
;rdi destPtr
;rsi srcPtr
;rdx leng
	align 16
MemCopyNANC_SSE:
_MemCopyNANC_SSE:
MemCopyNANC_AMDSSE:
_MemCopyNANC_AMDSSE:
	cmp rsi,rdi
	jz memcopynancexit
	cmp rdx,144
	jb memcopynanclop

	mov rax,0xf
	and rax,rdi
	jz memcopynanclop5a
	mov rcx,16
	sub rcx,rax
	sub rdx,rcx
	rep movsb
	align 16
memcopynanclop5a:
	mov rcx,rdx
	shr rcx,7
	ALIGN 16
memcopynanclop5f:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	movups xmm2,[rsi+32]
	movups xmm3,[rsi+48]
	movups xmm4,[rsi+64]
	movups xmm5,[rsi+80]
	movups xmm6,[rsi+96]
	movups xmm7,[rsi+112]
	movntps [rdi],xmm0
	movntps [rdi+16],xmm1
	movntps [rdi+32],xmm2
	movntps [rdi+48],xmm3
	movntps [rdi+64],xmm4
	movntps [rdi+80],xmm5
	movntps [rdi+96],xmm6
	movntps [rdi+112],xmm7
	lea rsi,[rsi+128]
	lea rdi,[rdi+128]
	dec rcx
	jnz memcopynanclop5f

	and rdx,127
	jz memcopynancexit
	mov rcx,rdx
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
	and rdx,15
	jz memcopynancexit
	mov rcx,rdx
	rep movsb
	jmp memcopynancexit

	align 16
memcopynanclop2:
memcopynanclop:
	mov rcx,rdx
	shr rcx,3
	jz memcopynanclop4
	rep movsq
	align 16
memcopynanclop4:
	mov rcx,7
	and rcx,rdx
	jz memcopynancexit
	rep movsb
	
	align 16
memcopynancexit:
	ret

;void MemCopyAC_AVX(void *destPtr, const void *srcPtr, OSInt leng)
;0 retAddr
;rdi destPtr
;rsi srcPtr
;rdx leng
	align 16
MemCopyAC_AVX:
_MemCopyAC_AVX:
MemCopyAC_AMDAVX:
_MemCopyAC_AMDAVX:
	cmp rdi,rsi
	jz memcopyacavxexit
	cmp rdx,128
	jb memcopyacavxlop

	mov r9,rdx
	shr r9,7
	ALIGN 16
memcopyacavxlop5f:
	vmovups xmm0,[rsi]
	vmovups xmm1,[rsi+32]
	vmovups xmm2,[rsi+64]
	vmovups xmm3,[rsi+96]
	vmovaps [rdi],ymm0
	vmovaps [rdi+32],ymm1
	vmovaps [rdi+64],ymm2
	vmovaps [rdi+96],ymm3
	lea rsi,[rsi+128]
	lea rdi,[rdi+128]
	dec r9
	jnz memcopyacavxlop5f
	vzeroupper

	and rdx,127
	jz memcopyacavxexit
	mov r9,rdx
	shr r9,4
	jz memcopyacavxlop2
	align 16
memcopyacavxlop5g:
	movups xmm0,[rsi]
	movaps [rdi],xmm0
	lea rsi,[rsi+16]
	lea rdi,[rdi+16]
	dec r9
	jnz memcopyacavxlop5g
	and rdx,15
	jz memcopyacavxexit
	align 16
memcopyacavxlop5h
	mov al,byte [rsi]
	mov byte [rdi],al
	lea rsi,[rsi+1]
	lea rdi,[rdi+1]
	dec rdx
	jnz memcopyacavxlop5h
	jmp memcopyacavxexit

	align 16
memcopyacavxlop2:
memcopyacavxlop:
	mov r9,rdx
	shr r9,3
	jz memcopyacavxlop4
	align 16
memcopyacavxlop3:
	mov rax,[rsi]
	mov [rdi],rax
	lea rsi,[rsi+8]
	lea rdi,[rdi+8]
	dec r9
	jnz memcopyacavxlop3
	align 16
memcopyacavxlop4:
	and rdx,7
	jz memcopyacavxexit
	align 16
memcopyacavxlop4a:
	mov al,byte [rsi]
	mov byte [rdi],al
	lea rsi,[rsi+1]
	lea rdi,[rdi+1]
	dec rdx
	jnz memcopyacavxlop4a
	
	align 16
memcopyacavxexit:
	ret

;void MemCopyANC_AVX(void *destPtr, const void *srcPtr, OSInt leng)
;0 retAddr
;rdi destPtr
;rsi srcPtr
;rdx leng
	align 16
MemCopyANC_AVX:
_MemCopyANC_AVX:
MemCopyANC_AMDAVX:
_MemCopyANC_AMDAVX:
	cmp rsi,rdi
	jz memcopyancavxexit
	cmp rdx,128
	jb memcopyancavxlop

	mov r9,rdx
	shr r9,7
	ALIGN 16
memcopyancavxlop5f:
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
	dec r9
	jnz memcopyancavxlop5f
	vzeroupper

	and rdx,127
	jz memcopyancavxexit
	mov r9,rdx
	shr r9,4
	jz memcopyancavxlop2
	align 16
memcopyancavxlop5g:
	movups xmm0,[rsi]
	movntps [rdi],xmm0
	lea rsi,[rsi+16]
	lea rdi,[rdi+16]
	dec r9
	jnz memcopyancavxlop5g
	and rdx,15
	jz memcopyancavxexit
	align 16
memcopyancavxlop5h
	mov al,byte [rsi]
	mov byte [rdi],al
	lea rsi,[rsi+1]
	lea rdi,[rdi+1]
	dec rdx
	jnz memcopyancavxlop5h
	jmp memcopyancavxexit

	align 16
memcopyancavxlop2:
memcopyancavxlop:
	mov r9,rdx
	shr r9,3
	jz memcopyancavxlop4
	align 16
memcopyancavxlop3:
	mov rax,[rsi]
	mov [rdi],rax
	lea rsi,[rsi+8]
	lea rdi,[rdi+8]
	dec r9
	jnz memcopyancavxlop3
	align 16
memcopyancavxlop4:
	and rdx,7
	jz memcopyancavxexit
	align 16
memcopyancavxlop4a:
	mov al,byte [rsi]
	mov byte [rdi],al
	lea rsi,[rsi+1]
	lea rdi,[rdi+1]
	dec rdx
	jnz memcopyancavxlop4a
	
	align 16
memcopyancavxexit:
	ret

;void MemCopyNAC_AVX(void *destPtr, const void *srcPtr, OSInt leng)
;0 retAddr
;rdi destPtr
;rsi srcPtr
;rdx leng
	align 16
MemCopyNAC_AVX:
_MemCopyNAC_AVX:
MemCopyNAC_AMDAVX:
_MemCopyNAC_AMDAVX:
	cmp rsi,rdi
	jz memcopynacavxexit
	cmp rdx,160
	jb memcopynacavxlop

	mov rax,0x1f
	and rax,rdi
	jz memcopynacavxlop5a
	mov rcx,32
	sub rcx,rax
	sub rdx,rcx
	rep movsb
	align 16
memcopynacavxlop5a:
	mov rcx,rdx
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

	and rdx,127
	jz memcopynacavxexit
	mov rcx,rdx
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
	and rdx,15
	jz memcopynacavxexit
	mov rcx,rdx
	rep movsb
	jmp memcopynacavxexit

	align 16
memcopynacavxlop2:
memcopynacavxlop:
	mov rcx,rdx
	shr rcx,3
	jz memcopynacavxlop4
	rep movsq
	align 16
memcopynacavxlop4:
	mov rcx,7
	and rcx,rdx
	jz memcopynacavxexit
	rep movsb
	
	align 16
memcopynacavxexit:
	ret

;void MemCopyNANC_AVX(void *destPtr, const void *srcPtr, OSInt leng)
;0 retAddr
;rdi destPtr
;rsi srcPtr
;rdx leng
	align 16
MemCopyNANC_AVX:
_MemCopyNANC_AVX:
MemCopyNANC_AMDAVX:
_MemCopyNANC_AMDAVX:
	cmp rsi,rdi
	jz memcopynancavxexit
	cmp rdx,160
	jb memcopynancavxlop

	mov rax,0x1f
	and rax,rdi
	jz memcopynancavxlop5a
	mov rcx,32
	sub rcx,rax
	sub rdx,rcx
	rep movsb
	align 16
memcopynancavxlop5a:
	mov rcx,rdx
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

	and rdx,127
	jz memcopynancavxexit
	mov rcx,rdx
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
	and rdx,15
	jz memcopynancavxexit
	mov rcx,rdx
	rep movsb
	jmp memcopynancavxexit

	align 16
memcopynancavxlop2:
memcopynancavxlop:
	mov rcx,rdx
	shr rcx,3
	jz memcopynancavxlop4
	rep movsq
	align 16
memcopynancavxlop4:
	mov rcx,7
	and rcx,rdx
	jz memcopynancavxexit
	rep movsb
	
	align 16
memcopynancavxexit:
	ret

