section .text

global CRC32_InitTable
global CRC32_Calc

;void CRC32_InitTable(UInt32 *tab, UInt32 polynormial);
;8 rbx
;8 retAddr
;rcx tab
;rdx polynormial
	align 16
CRC32_InitTable:
	push rbx
	xor rbx,rbx
	align 16
crc32itlop:
	mov rax,rbx
	shl rax,24
	mov r8,8
	align 16
crc32itlop2:
	shl eax,1
	jnc crc32itlop3
	xor eax,edx
	align 16
crc32itlop3:
	dec r8
	jnz crc32itlop2
	mov dword [rcx+rbx*4],eax
	inc rbx
	cmp rbx,256
	jb crc32itlop

	mov r8,256
	xor rdx,rdx
	align 16
crc32itlop4:
	lea rax,[r8-1]
	mov ebx,dword [rcx+r8*4-4]
	mov edx,ebx
	shr edx,24
	shl ebx,8
	mov edx,dword [rcx+rdx*4]
	xor edx,ebx
	mov dword [rcx+r8*4+1024-4],edx
	mov ebx,edx
	mov edx,ebx
	shr edx,24
	shl ebx,8
	mov edx,dword [rcx+rdx*4]
	xor edx,ebx
	mov dword [rcx+r8*4+2048-4],edx
	mov ebx,edx
	mov edx,ebx
	shr edx,24
	shl ebx,8
	mov edx,dword [rcx+rdx*4]
	xor edx,ebx
	mov dword [rcx+r8*4+3072-4],edx
	mov ebx,edx
	mov edx,ebx
	shr edx,24
	shl ebx,8
	mov edx,dword [rcx+rdx*4]
	xor edx,ebx
	mov dword [rcx+r8*4+4096-4],edx
	mov ebx,edx
	mov edx,ebx
	shr edx,24
	shl ebx,8
	mov edx,dword [rcx+rdx*4]
	xor edx,ebx
	mov dword [rcx+r8*4+5120-4],edx
	mov ebx,edx
	mov edx,ebx
	shr edx,24
	shl ebx,8
	mov edx,dword [rcx+rdx*4]
	xor edx,ebx
	mov dword [rcx+r8*4+6144-4],edx
	mov ebx,edx
	mov edx,ebx
	shr edx,24
	shl ebx,8
	mov edx,dword [rcx+rdx*4]
	xor edx,ebx
	mov dword [rcx+r8*4+7168-4],edx
	mov ebx,edx
	mov edx,ebx
	shr edx,24
	shl ebx,8
	mov edx,dword [rcx+rdx*4]
	xor edx,ebx
	mov dword [rcx+r8*4+8192-4],edx
	mov ebx,edx
	mov edx,ebx
	shr edx,24
	shl ebx,8
	mov edx,dword [rcx+rdx*4]
	xor edx,ebx
	mov dword [rcx+r8*4+9216-4],edx
	mov ebx,edx
	mov edx,ebx
	shr edx,24
	shl ebx,8
	mov edx,dword [rcx+rdx*4]
	xor edx,ebx
	mov dword [rcx+r8*4+10240-4],edx
	mov ebx,edx
	mov edx,ebx
	shr edx,24
	shl ebx,8
	mov edx,dword [rcx+rdx*4]
	xor edx,ebx
	mov dword [rcx+r8*4+11264-4],edx
	mov ebx,edx
	mov edx,ebx
	shr edx,24
	shl ebx,8
	mov edx,dword [rcx+rdx*4]
	xor edx,ebx
	mov dword [rcx+r8*4+12288-4],edx
	mov ebx,edx
	mov edx,ebx
	shr edx,24
	shl ebx,8
	mov edx,dword [rcx+rdx*4]
	xor edx,ebx
	mov dword [rcx+r8*4+13312-4],edx
	mov ebx,edx
	mov edx,ebx
	shr edx,24
	shl ebx,8
	mov edx,dword [rcx+rdx*4]
	xor edx,ebx
	mov dword [rcx+r8*4+14336-4],edx
	mov ebx,edx
	mov edx,ebx
	shr edx,24
	shl ebx,8
	mov edx,dword [rcx+rdx*4]
	xor edx,ebx
	mov dword [rcx+r8*4+15360-4],edx
	dec r8
	jnz crc32itlop4
	
	pop rbx
	ret

;UInt32 CRC32_Calc(const UInt8 *buff, UOSInt buffSize, UInt32 *tab, UInt32 currVal)
;0 retAddr
;rcx buff
;rdx buffSize
;r8 tab
;r9 currVal
	align 16
CRC32_Calc:
	xchg r10,rbx
	mov rax,r9
	mov r9,rdx
	mov r11,r9
	shr r9,4
	jz crc32calclop1
	
	align 16
crc32calclop:
	bswap eax
	mov ebx,dword [rcx]
	xor ebx,eax
	movzx rdx,byte [rcx+15]
	mov eax,dword [r8+rdx*4+0]
	movzx rdx,byte [rcx+14]
	xor eax,dword [r8+rdx*4+1024]
	movzx rdx,byte [rcx+13]
	xor eax,dword [r8+rdx*4+2048]
	movzx rdx,byte [rcx+12]
	xor eax,dword [r8+rdx*4+3072]
	movzx rdx,byte [rcx+11]
	xor eax,dword [r8+rdx*4+4096]
	movzx rdx,byte [rcx+10]
	xor eax,dword [r8+rdx*4+5120]
	movzx rdx,byte [rcx+9]
	xor eax,dword [r8+rdx*4+6144]
	movzx rdx,byte [rcx+8]
	xor eax,dword [r8+rdx*4+7168]
	movzx rdx,byte [rcx+7]
	xor eax,dword [r8+rdx*4+8192]
	movzx rdx,byte [rcx+6]
	xor eax,dword [r8+rdx*4+9216]
	movzx rdx,byte [rcx+5]
	xor eax,dword [r8+rdx*4+10240]
	movzx rdx,byte [rcx+4]
	xor eax,dword [r8+rdx*4+11264]
	movzx edx,bh
	xor eax,dword [r8+rdx*4+14336]
	movzx rdx,bl 
	shr ebx,16
	xor eax,dword [r8+rdx*4+15360]
	movzx edx,bh
	xor eax,dword [r8+rdx*4+12288]
	movzx rdx,bl 
	xor eax,dword [r8+rdx*4+13312]

	lea rcx,[rcx+16]
	dec r9
	jnz crc32calclop
	
crc32calclop1:
	mov r9,r11
	and r9,15
	shr r9,2
	jz crc32calclop2
	align 16
crc32calclop1a:
	bswap eax
	mov ebx,dword [rcx]
	xor ebx,eax
	movzx rdx,bl 
	mov eax,dword [r8+rdx*4+3072]
	movzx edx,bh
	shr ebx,16
	xor eax,dword [r8+rdx*4+2048]
	movzx rdx,bl 
	xor eax,dword [r8+rdx*4+1024]
	movzx edx,bh
	xor eax,dword [r8+rdx*4+0]

	lea rcx,[rcx+4]
	dec r9
	jnz crc32calclop1a
	
	align 16
crc32calclop2:
	and r11,3
	jz crc32calcexit
	
	xor rbx,rbx
	align 16
crc32calclop3:
	mov ebx,eax
	shr ebx,24
	shl eax,8
	xor bl,byte [rcx]
	xor eax,dword [r8+rbx*4]
	lea rcx,[rcx+1]
	dec r11
	jnz crc32calclop3

	align 16
crc32calcexit:
	mov rbx,r10
	ret
