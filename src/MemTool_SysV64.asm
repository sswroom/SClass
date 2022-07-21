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

global MemCopyNACAddr

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
memcopyaclop5h:
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
memcopyanclop5h:
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
	cmp rdx,64
	jnb memcopynaclop0
	lea rax,[rel MemCopyNACAddr]
	jmp [rdx*8+rax]

	align 16
memcopynaclop0:
	cmp rdx,256
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
	add rsi,128
	add rdi,128
	dec rcx
	jnz memcopynaclop5f

	test rdx,64
	jz memcopynaclop2
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	movups xmm2,[rsi+32]
	movups xmm3,[rsi+48]
	movaps [rdi],xmm0
	movaps [rdi+16],xmm1
	movaps [rdi+32],xmm2
	movaps [rdi+48],xmm3
	add rsi,64
	add rdi,64

	align 16
memcopynaclop2:
	and rdx,63
	lea rax,[rel MemCopyNACAddr]
	jmp [rax+rdx*8]

	align 16
memcopynaclop:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	movups xmm2,[rsi+32]
	movups xmm3,[rsi+48]
	movups [rdi],xmm0
	movups [rdi+16],xmm1
	movups [rdi+32],xmm2
	movups [rdi+48],xmm3
	cmp rdx,128
	jb memcopynaclop64
	movups xmm0,[rsi+64]
	movups xmm1,[rsi+80]
	movups xmm2,[rsi+96]
	movups xmm3,[rsi+112]
	movups [rdi+64],xmm0
	movups [rdi+80],xmm1
	movups [rdi+96],xmm2
	movups [rdi+112],xmm3
	cmp rdx,192
	jb memcopynaclop128
	movups xmm0,[rsi+128]
	movups xmm1,[rsi+144]
	movups xmm2,[rsi+160]
	movups xmm3,[rsi+176]
	movups [rdi+128],xmm0
	movups [rdi+144],xmm1
	movups [rdi+160],xmm2
	movups [rdi+176],xmm3
	add rsi,192
	add rdi,192
	lea rax,[rel MemCopyNACAddr - 192 * 8]
	jmp [rax+rdx*8]

	align 16
memcopynaclop64:
	add rsi,64
	add rdi,64
	lea rax,[rel MemCopyNACAddr - 64 * 8]
	jmp [rax+rdx*8]

	align 16
memcopynaclop128:
	add rsi,128
	add rdi,128
	lea rax,[rel MemCopyNACAddr - 128 * 8]
	jmp [rax+rdx*8]
	
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
	cmp rdx,128
	jb memcopyacavxlop

	mov r9,rdx
	shr r9,7
	ALIGN 16
memcopyacavxlop5f:
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
	mov rcx,rdx
	rep movsb
;memcopyacavxlop5h
;	mov al,byte [rsi]
;	mov byte [rdi],al
;	lea rsi,[rsi+1]
;	lea rdi,[rdi+1]
;	dec rdx
;	jnz memcopyacavxlop5h
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
memcopyancavxlop5h:
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
	cmp rdx,64
	jnb memcopynacavxlop0
	lea rax,[rel MemCopyNACAddr]
	jmp [rax+rdx*8]

	align 16
memcopynacavxlop0:
	cmp rdx,256
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
	add rsi,128
	add rdi,128
	dec rcx
	jnz memcopynacavxlop5f
	vzeroupper

	test rdx,64
	jz memcopynacavxlop2
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	movups xmm2,[rsi+32]
	movups xmm3,[rsi+48]
	movaps [rdi],xmm0
	movaps [rdi+16],xmm1
	movaps [rdi+32],xmm2
	movaps [rdi+48],xmm3
	add rsi,64
	add rdi,64

	align 16
memcopynacavxlop2:
	and rdx,63
	lea rax,[rel MemCopyNACAddr]
	jmp [rax+rdx*8]

	align 16
memcopynacavxlop:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	movups xmm2,[rsi+32]
	movups xmm3,[rsi+48]
	movups [rdi],xmm0
	movups [rdi+16],xmm1
	movups [rdi+32],xmm2
	movups [rdi+48],xmm3
	cmp rdx,128
	jb memcopynacavxlop64
	movups xmm0,[rsi+64]
	movups xmm1,[rsi+80]
	movups xmm2,[rsi+96]
	movups xmm3,[rsi+112]
	movups [rdi+64],xmm0
	movups [rdi+80],xmm1
	movups [rdi+96],xmm2
	movups [rdi+112],xmm3
	cmp rdx,192
	jb memcopynacavxlop128
	movups xmm0,[rsi+128]
	movups xmm1,[rsi+144]
	movups xmm2,[rsi+160]
	movups xmm3,[rsi+176]
	movups [rdi+128],xmm0
	movups [rdi+144],xmm1
	movups [rdi+160],xmm2
	movups [rdi+176],xmm3
	add rsi,192
	add rdi,192
	lea rax,[rel MemCopyNACAddr - 192 * 8]
	jmp [rax+rdx*8]

	align 16
memcopynacavxlop64:
	add rsi,64
	add rdi,64
	lea rax,[rel MemCopyNACAddr - 64 * 8]
	jmp [rax+rdx*8]

	align 16
memcopynacavxlop128:
	add rsi,128
	add rdi,128
	lea rax,[rel MemCopyNACAddr - 128 * 8]
	jmp [rax+rdx*8]

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
	cmp rdx,32
	jnb memcopynancavxlop0
	lea rax,[rel MemCopyNACAddr]
	jmp [rax+rdx*8]

	align 16
memcopynancavxlop0:
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

	align 16
MemCopyB:
	rep movsb
	ret

	align 16
MemCopyNAC0:
	ret

	align 16
MemCopyNAC1:
	mov al,[rsi]
	mov [rdi],al
	ret

	align 16
MemCopyNAC2:
	mov ax,[rsi]
	mov [rdi],ax
	ret

	align 16
MemCopyNAC3:
	mov ax,[rsi]
	mov dl,[rsi+2]
	mov [rdi],ax
	mov [rdi+2],dl
	ret

	align 16
MemCopyNAC4:
	mov eax,[rsi]
	mov [rdi],eax
	ret

	align 16
MemCopyNAC5:
	mov eax,[rsi]
	mov dl,[rsi+4]
	mov [rdi],eax
	mov [rdi+4],dl
	ret

	align 16
MemCopyNAC6:
	mov eax,[rsi]
	mov dx,[rsi+4]
	mov [rdi],eax
	mov [rdi+4],dx
	ret

	align 16
MemCopyNAC7:
	mov eax,[rsi]
	mov dx,[rsi+4]
	mov cl,[rsi+6]
	mov [rdi],eax
	mov [rdi+4],dx
	mov [rdi+6],cl
	ret

	align 16
MemCopyNAC8:
	mov rax,[rsi]
	mov [rdi],rax
	ret

	align 16
MemCopyNAC9:
	mov rax,[rsi]
	mov dl,[rsi+8]
	mov [rdi],rax
	mov [rdi+8],dl
	ret

	align 16
MemCopyNAC10:
	mov rax,[rsi]
	mov dx,[rsi+8]
	mov [rdi],rax
	mov [rdi+8],dx
	ret

	align 16
MemCopyNAC11:
	mov rax,[rsi]
	mov dx,[rsi+8]
	mov cl,[rsi+10]
	mov [rdi],rax
	mov [rdi+8],dx
	mov [rdi+10],cl
	ret

	align 16
MemCopyNAC12:
	mov rax,[rsi]
	mov edx,[rsi+8]
	mov [rdi],rax
	mov [rdi+8],edx
	ret

	align 16
MemCopyNAC13:
	mov rax,[rsi]
	mov edx,[rsi+8]
	mov cl,[rsi+12]
	mov [rdi],rax
	mov [rdi+8],edx
	mov [rdi+12],cl
	ret

	align 16
MemCopyNAC14:
	mov rax,[rsi]
	mov edx,[rsi+8]
	mov cx,[rsi+12]
	mov [rdi],rax
	mov [rdi+8],edx
	mov [rdi+12],cx
	ret


	align 16
MemCopyNAC15:
	mov r8,[rsi]
	mov edx,[rsi+8]
	mov cx,[rsi+12]
	mov al,[rsi+14]
	mov [rdi],r8
	mov [rdi+8],edx
	mov [rdi+12],cx
	mov [rdi+14],al
	ret


	align 16
MemCopyNAC16:
	movups xmm0,[rsi]
	movups [rdi],xmm0
	ret

	align 16
MemCopyNAC17:
	movups xmm0,[rsi]
	mov al,[rsi+16]
	movups [rdi],xmm0
	mov [rdi+16],al
	ret

	align 16
MemCopyNAC18:
	movups xmm0,[rsi]
	mov ax,[rsi+16]
	movups [rdi],xmm0
	mov [rdi+16],ax
	ret

	align 16
MemCopyNAC19:
	movups xmm0,[rsi]
	mov ax,[rsi+16]
	mov dl,[rsi+18]
	movups [rdi],xmm0
	mov [rdi+16],ax
	mov [rdi+18],dl
	ret

	align 16
MemCopyNAC20:
	movups xmm0,[rsi]
	mov eax,[rsi+16]
	movups [rdi],xmm0
	mov [rdi+16],eax
	ret

	align 16
MemCopyNAC21:
	movups xmm0,[rsi]
	mov eax,[rsi+16]
	mov dl,[rsi+20]
	movups [rdi],xmm0
	mov [rdi+16],eax
	mov [rdi+20],dl
	ret

	align 16
MemCopyNAC22:
	movups xmm0,[rsi]
	mov eax,[rsi+16]
	mov dx,[rsi+20]
	movups [rdi],xmm0
	mov [rdi+16],eax
	mov [rdi+20],dx
	ret

	align 16
MemCopyNAC23:
	movups xmm0,[rsi]
	mov eax,[rsi+16]
	mov dx,[rsi+20]
	mov cl,[rsi+22]
	movups [rdi],xmm0
	mov [rdi+16],eax
	mov [rdi+20],dx
	mov [rdi+22],cl
	ret

	align 16
MemCopyNAC24:
	movups xmm0,[rsi]
	mov rax,[rsi+16]
	movups [rdi],xmm0
	mov [rdi+16],rax
	ret

	align 16
MemCopyNAC25:
	movups xmm0,[rsi]
	mov rax,[rsi+16]
	mov dl,[rsi+24]
	movups [rdi],xmm0
	mov [rdi+16],rax
	mov [rdi+24],dl
	ret

	align 16
MemCopyNAC26:
	movups xmm0,[rsi]
	mov rax,[rsi+16]
	mov dx,[rsi+24]
	movups [rdi],xmm0
	mov [rdi+16],rax
	mov [rdi+24],dx
	ret

	align 16
MemCopyNAC27:
	movups xmm0,[rsi]
	mov rax,[rsi+16]
	mov dx,[rsi+24]
	mov cl,[rsi+26]
	movups [rdi],xmm0
	mov [rdi+16],rax
	mov [rdi+24],dx
	mov [rdi+26],cl
	ret

	align 16
MemCopyNAC28:
	movups xmm0,[rsi]
	mov rax,[rsi+16]
	mov edx,[rsi+24]
	movups [rdi],xmm0
	mov [rdi+16],rax
	mov [rdi+24],edx
	ret

	align 16
MemCopyNAC29:
	movups xmm0,[rsi]
	mov rax,[rsi+16]
	mov edx,[rsi+24]
	mov cl,[rsi+28]
	movups [rdi],xmm0
	mov [rdi+16],rax
	mov [rdi+24],edx
	mov [rdi+28],cl
	ret

	align 16
MemCopyNAC30:
	movups xmm0,[rsi]
	mov rax,[rsi+16]
	mov edx,[rsi+24]
	mov cx,[rsi+28]
	movups [rdi],xmm0
	mov [rdi+16],rax
	mov [rdi+24],edx
	mov [rdi+28],cx
	ret


	align 16
MemCopyNAC31:
	movups xmm0,[rsi]
	mov r8,[rsi+16]
	mov edx,[rsi+24]
	mov cx,[rsi+28]
	mov al,[rsi+30]
	movups [rdi],xmm0
	mov [rdi+16],r8
	mov [rdi+24],edx
	mov [rdi+28],cx
	mov [rdi+30],al
	ret

	align 16
MemCopyNAC32:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	movups [rdi],xmm0
	movups [rdi+16],xmm1
	ret

	align 16
MemCopyNAC33:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	mov al,[rsi+32]
	movups [rdi],xmm0
	movups [rdi+16],xmm1
	mov [rdi+32],al
	ret

	align 16
MemCopyNAC34:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	mov ax,[rsi+32]
	movups [rdi],xmm0
	movups [rdi+16],xmm1
	mov [rdi+32],ax
	ret

	align 16
MemCopyNAC35:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	mov ax,[rsi+32]
	mov dl,[rsi+34]
	movups [rdi],xmm0
	movups [rdi+16],xmm1
	mov [rdi+32],ax
	mov [rdi+34],dl
	ret

	align 16
MemCopyNAC36:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	mov eax,[rsi+32]
	movups [rdi],xmm0
	movups [rdi+16],xmm1
	mov [rdi+32],eax
	ret

	align 16
MemCopyNAC37:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	mov eax,[rsi+32]
	mov dl,[rsi+36]
	movups [rdi],xmm0
	movups [rdi+16],xmm1
	mov [rdi+32],eax
	mov [rdi+36],dl
	ret

	align 16
MemCopyNAC38:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	mov eax,[rsi+32]
	mov dx,[rsi+36]
	movups [rdi],xmm0
	movups [rdi+16],xmm1
	mov [rdi+32],eax
	mov [rdi+36],dx
	ret

	align 16
MemCopyNAC39:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	mov eax,[rsi+32]
	mov dx,[rsi+36]
	mov cl,[rsi+38]
	movups [rdi],xmm0
	movups [rdi+16],xmm1
	mov [rdi+32],eax
	mov [rdi+36],dx
	mov [rdi+38],cl
	ret

	align 16
MemCopyNAC40:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	mov rax,[rsi+32]
	movups [rdi],xmm0
	movups [rdi+16],xmm1
	mov [rdi+32],rax
	ret

	align 16
MemCopyNAC41:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	mov rax,[rsi+32]
	mov dl,[rsi+40]
	movups [rdi],xmm0
	movups [rdi+16],xmm1
	mov [rdi+32],rax
	mov [rdi+40],dl
	ret

	align 16
MemCopyNAC42:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	mov rax,[rsi+32]
	mov dx,[rsi+40]
	movups [rdi],xmm0
	movups [rdi+16],xmm1
	mov [rdi+32],rax
	mov [rdi+40],dx
	ret

	align 16
MemCopyNAC43:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	mov rax,[rsi+32]
	mov dx,[rsi+40]
	mov cl,[rsi+42]
	movups [rdi],xmm0
	movups [rdi+16],xmm1
	mov [rdi+32],rax
	mov [rdi+40],dx
	mov [rdi+42],cl
	ret

	align 16
MemCopyNAC44:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	mov rax,[rsi+32]
	mov edx,[rsi+40]
	movups [rdi],xmm0
	movups [rdi+16],xmm1
	mov [rdi+32],rax
	mov [rdi+40],edx
	ret

	align 16
MemCopyNAC45:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	mov rax,[rsi+32]
	mov edx,[rsi+40]
	mov cl,[rsi+44]
	movups [rdi],xmm0
	movups [rdi+16],xmm1
	mov [rdi+32],rax
	mov [rdi+40],edx
	mov [rdi+44],cl
	ret

	align 16
MemCopyNAC46:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	mov rax,[rsi+32]
	mov edx,[rsi+40]
	mov cx,[rsi+44]
	movups [rdi],xmm0
	movups [rdi+16],xmm1
	mov [rdi+32],rax
	mov [rdi+40],edx
	mov [rdi+44],cx
	ret


	align 16
MemCopyNAC47:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	mov r8,[rsi+32]
	mov edx,[rsi+40]
	mov cx,[rsi+44]
	mov al,[rsi+46]
	movups [rdi],xmm0
	movups [rdi+16],xmm1
	mov [rdi+32],r8
	mov [rdi+40],edx
	mov [rdi+44],cx
	mov [rdi+46],al
	ret

	align 16
MemCopyNAC48:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	movups xmm2,[rsi+32]
	movups [rdi],xmm0
	movups [rdi+16],xmm1
	movups [rdi+32],xmm2
	ret

	align 16
MemCopyNAC49:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	movups xmm2,[rsi+32]
	mov al,[rsi+48]
	movups [rdi],xmm0
	movups [rdi+16],xmm1
	movups [rdi+32],xmm2
	mov [rdi+48],al
	ret

	align 16
MemCopyNAC50:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	movups xmm2,[rsi+32]
	mov ax,[rsi+48]
	movups [rdi],xmm0
	movups [rdi+16],xmm1
	movups [rdi+32],xmm2
	mov [rdi+48],ax
	ret

	align 16
MemCopyNAC51:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	movups xmm2,[rsi+32]
	mov ax,[rsi+48]
	mov dl,[rsi+50]
	movups [rdi],xmm0
	movups [rdi+16],xmm1
	movups [rdi+32],xmm2
	mov [rdi+48],ax
	mov [rdi+50],dl
	ret

	align 16
MemCopyNAC52:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	movups xmm2,[rsi+32]
	mov eax,[rsi+48]
	movups [rdi],xmm0
	movups [rdi+16],xmm1
	movups [rdi+32],xmm2
	mov [rdi+48],eax
	ret

	align 16
MemCopyNAC53:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	movups xmm2,[rsi+32]
	mov eax,[rsi+48]
	mov dl,[rsi+52]
	movups [rdi],xmm0
	movups [rdi+16],xmm1
	movups [rdi+32],xmm2
	mov [rdi+48],eax
	mov [rdi+52],dl
	ret

	align 16
MemCopyNAC54:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	movups xmm2,[rsi+32]
	mov eax,[rsi+48]
	mov dx,[rsi+52]
	movups [rdi],xmm0
	movups [rdi+16],xmm1
	movups [rdi+32],xmm2
	mov [rdi+48],eax
	mov [rdi+52],dx
	ret

	align 16
MemCopyNAC55:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	movups xmm2,[rsi+32]
	mov eax,[rsi+48]
	mov dx,[rsi+52]
	mov cl,[rsi+54]
	movups [rdi],xmm0
	movups [rdi+16],xmm1
	movups [rdi+32],xmm2
	mov [rdi+48],eax
	mov [rdi+52],dx
	mov [rdi+54],cl
	ret

	align 16
MemCopyNAC56:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	movups xmm2,[rsi+32]
	mov rax,[rsi+48]
	movups [rdi],xmm0
	movups [rdi+16],xmm1
	movups [rdi+32],xmm2
	mov [rdi+48],rax
	ret

	align 16
MemCopyNAC57:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	movups xmm2,[rsi+32]
	mov rax,[rsi+48]
	mov dl,[rsi+56]
	movups [rdi],xmm0
	movups [rdi+16],xmm1
	movups [rdi+32],xmm2
	mov [rdi+48],rax
	mov [rdi+56],dl
	ret

	align 16
MemCopyNAC58:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	movups xmm2,[rsi+32]
	mov rax,[rsi+48]
	mov dx,[rsi+56]
	movups [rdi],xmm0
	movups [rdi+16],xmm1
	movups [rdi+32],xmm2
	mov [rdi+48],rax
	mov [rdi+56],dx
	ret

	align 16
MemCopyNAC59:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	movups xmm2,[rsi+32]
	mov rax,[rsi+48]
	mov dx,[rsi+56]
	mov cl,[rsi+58]
	movups [rdi],xmm0
	movups [rdi+16],xmm1
	movups [rdi+32],xmm2
	mov [rdi+48],rax
	mov [rdi+56],dx
	mov [rdi+58],cl
	ret

	align 16
MemCopyNAC60:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	movups xmm2,[rsi+32]
	mov rax,[rsi+48]
	mov edx,[rsi+56]
	movups [rdi],xmm0
	movups [rdi+16],xmm1
	movups [rdi+32],xmm2
	mov [rdi+48],rax
	mov [rdi+56],edx
	ret

	align 16
MemCopyNAC61:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	movups xmm2,[rsi+32]
	mov rax,[rsi+48]
	mov edx,[rsi+56]
	mov cl,[rsi+60]
	movups [rdi],xmm0
	movups [rdi+16],xmm1
	movups [rdi+32],xmm2
	mov [rdi+48],rax
	mov [rdi+56],edx
	mov [rdi+60],cl
	ret

	align 16
MemCopyNAC62:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	movups xmm2,[rsi+32]
	mov rax,[rsi+48]
	mov edx,[rsi+56]
	mov cx,[rsi+60]
	movups [rdi],xmm0
	movups [rdi+16],xmm1
	movups [rdi+32],xmm2
	mov [rdi+48],rax
	mov [rdi+56],edx
	mov [rdi+60],cx
	ret


	align 16
MemCopyNAC63:
	movups xmm0,[rsi]
	movups xmm1,[rsi+16]
	movups xmm2,[rsi+32]
	mov r8,[rsi+48]
	mov edx,[rsi+56]
	mov cx,[rsi+60]
	mov al,[rsi+62]
	movups [rdi],xmm0
	movups [rdi+16],xmm1
	movups [rdi+32],xmm2
	mov [rdi+48],r8
	mov [rdi+56],edx
	mov [rdi+60],cx
	mov [rdi+62],al
	ret

section .data
	align 8
MemCopyNACAddr:
	dq MemCopyNAC0
	dq MemCopyNAC1
	dq MemCopyNAC2
	dq MemCopyNAC3
	dq MemCopyNAC4
	dq MemCopyNAC5
	dq MemCopyNAC6
	dq MemCopyNAC7
	dq MemCopyNAC8
	dq MemCopyNAC9
	dq MemCopyNAC10
	dq MemCopyNAC11
	dq MemCopyNAC12
	dq MemCopyNAC13
	dq MemCopyNAC14
	dq MemCopyNAC15
	dq MemCopyNAC16
	dq MemCopyNAC17
	dq MemCopyNAC18
	dq MemCopyNAC19
	dq MemCopyNAC20
	dq MemCopyNAC21
	dq MemCopyNAC22
	dq MemCopyNAC23
	dq MemCopyNAC24
	dq MemCopyNAC25
	dq MemCopyNAC26
	dq MemCopyNAC27
	dq MemCopyNAC28
	dq MemCopyNAC29
	dq MemCopyNAC30
	dq MemCopyNAC31
	dq MemCopyNAC32
	dq MemCopyNAC33
	dq MemCopyNAC34
	dq MemCopyNAC35
	dq MemCopyNAC36
	dq MemCopyNAC37
	dq MemCopyNAC38
	dq MemCopyNAC39
	dq MemCopyNAC40
	dq MemCopyNAC41
	dq MemCopyNAC42
	dq MemCopyNAC43
	dq MemCopyNAC44
	dq MemCopyNAC45
	dq MemCopyNAC46
	dq MemCopyNAC47
	dq MemCopyNAC48
	dq MemCopyNAC49
	dq MemCopyNAC50
	dq MemCopyNAC51
	dq MemCopyNAC52
	dq MemCopyNAC53
	dq MemCopyNAC54
	dq MemCopyNAC55
	dq MemCopyNAC56
	dq MemCopyNAC57
	dq MemCopyNAC58
	dq MemCopyNAC59
	dq MemCopyNAC60
	dq MemCopyNAC61
	dq MemCopyNAC62
	dq MemCopyNAC63
