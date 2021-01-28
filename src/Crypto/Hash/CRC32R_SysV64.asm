section .text

global CRC32R_InitTable
global _CRC32R_InitTable
global CRC32R_Reverse
global _CRC32R_Reverse
global CRC32R_Calc
global _CRC32R_Calc

;void CRC32R_InitTable(UInt32 *tab, UInt32 rpn);
;16 retAddr
;rdi tab
;rsi rpn
	align 16
CRC32R_InitTable:
_CRC32R_InitTable:
	mov rcx,256
	align 16
crc32clop:
	lea rax,[rcx-1]
	mov edx,8
	align 16
crc32clop2:
	shr eax,1
	jnb crc32clop3
	xor eax,esi
	align 16
crc32clop3:
	dec edx
	jnz crc32clop2
	mov dword [rdi+rcx*4-4],eax
	dec rcx
	jnz crc32clop

	mov r8,rbx
	mov rcx,256
	align 16
crc32ritlop4:
	lea eax,[rcx-1]
	mov ebx,dword [rdi+rcx*4-4]
	movzx edx,bl
	shr ebx,8
	mov edx,dword [rdi+rdx*4]
	xor edx,ebx
	mov dword [rdi+rcx*4+1024-4],edx
	mov ebx,edx
	movzx edx,bl
	shr ebx,8
	mov edx,dword [rdi+rdx*4]
	xor edx,ebx
	mov dword [rdi+rcx*4+2048-4],edx
	mov ebx,edx
	movzx edx,bl
	shr ebx,8
	mov edx,dword [rdi+rdx*4]
	xor edx,ebx
	mov dword [rdi+rcx*4+3072-4],edx
	mov ebx,edx
	movzx edx,bl
	shr ebx,8
	mov edx,dword [rdi+rdx*4]
	xor edx,ebx
	mov dword [rdi+rcx*4+4096-4],edx
	mov ebx,edx
	movzx edx,bl
	shr ebx,8
	mov edx,dword [rdi+rdx*4]
	xor edx,ebx
	mov dword [rdi+rcx*4+5120-4],edx
	mov ebx,edx
	movzx edx,bl
	shr ebx,8
	mov edx,dword [rdi+rdx*4]
	xor edx,ebx
	mov dword [rdi+rcx*4+6144-4],edx
	mov ebx,edx
	movzx edx,bl
	shr ebx,8
	mov edx,dword [rdi+rdx*4]
	xor edx,ebx
	mov dword [rdi+rcx*4+7168-4],edx
	mov ebx,edx
	movzx edx,bl
	shr ebx,8
	mov edx,dword [rdi+rdx*4]
	xor edx,ebx
	mov dword [rdi+rcx*4+8192-4],edx
	mov ebx,edx
	movzx edx,bl
	shr ebx,8
	mov edx,dword [rdi+rdx*4]
	xor edx,ebx
	mov dword [rdi+rcx*4+9216-4],edx
	mov ebx,edx
	movzx edx,bl
	shr ebx,8
	mov edx,dword [rdi+rdx*4]
	xor edx,ebx
	mov dword [rdi+rcx*4+10240-4],edx
	mov ebx,edx
	movzx edx,bl
	shr ebx,8
	mov edx,dword [rdi+rdx*4]
	xor edx,ebx
	mov dword [rdi+rcx*4+11264-4],edx
	mov ebx,edx
	movzx edx,bl
	shr ebx,8
	mov edx,dword [rdi+rdx*4]
	xor edx,ebx
	mov dword [rdi+rcx*4+12288-4],edx
	mov ebx,edx
	movzx edx,bl
	shr ebx,8
	mov edx,dword [rdi+rdx*4]
	xor edx,ebx
	mov dword [rdi+rcx*4+13312-4],edx
	mov ebx,edx
	movzx edx,bl
	shr ebx,8
	mov edx,dword [rdi+rdx*4]
	xor edx,ebx
	mov dword [rdi+rcx*4+14336-4],edx
	mov ebx,edx
	movzx edx,bl
	shr ebx,8
	mov edx,dword [rdi+rdx*4]
	xor edx,ebx
	mov dword [rdi+rcx*4+15360-4],edx
	dec rcx
	jnz crc32ritlop4
	mov rbx,r8
	ret

;UInt32 CRC32R_Reverse(UInt32 polynomial)
;0 retAddr
;rdi polynomial
	align 16
CRC32R_Reverse:
_CRC32R_Reverse:
	mov edx,32
	xor rax,rax
	align 16
crc32rlop:
	shl edi,1
	rcr eax,1
	dec edx
	jnz crc32rlop
	ret

;UInt32 CRC32R_Calc(const UInt8 *buff, UOSInt buffSize, UInt32 *tab, UInt32 currVal)
;0 retAddr
;rdi rcx buff
;rsi rdx buffSize
;rdx r8 tab
;rcx r9 currVal
	align 16
CRC32R_Calc:
_CRC32R_Calc:
	xchg r10,rbx
	mov rax,rcx ;currVal
	mov r8,rdx ;tab
	mov r9,rsi ;buffSize
	mov r11,r9 ;buffSize
	shr r9,4 ;buffSize 
	jz calclop1
calclop:
	mov ebx,dword [rdi] ;buff
	xor ebx,eax
	movzx rdx,bl
	mov eax,dword [r8+rdx*4+15360]
	movzx edx,bh
	shr ebx,16
	xor eax,dword [r8+rdx*4+14336]
	movzx edx,bl
	xor eax,dword [r8+rdx*4+13312]
	movzx edx,bh
	xor eax,dword [r8+rdx*4+12288]
	movzx rdx,byte [rdi+4]
	xor eax,dword [r8+rdx*4+11264]
	movzx rdx,byte [rdi+5]
	xor eax,dword [r8+rdx*4+10240]
	movzx rdx,byte [rdi+6]
	xor eax,dword [r8+rdx*4+9216]
	movzx rdx,byte [rdi+7]
	xor eax,dword [r8+rdx*4+8192]
	movzx rdx,byte [rdi+8]
	xor eax,dword [r8+rdx*4+7168]
	movzx rdx,byte [rdi+9]
	xor eax,dword [r8+rdx*4+6144]
	movzx rdx,byte [rdi+10]
	xor eax,dword [r8+rdx*4+5120]
	movzx rdx,byte [rdi+11]
	xor eax,dword [r8+rdx*4+4096]
	movzx rdx,byte [rdi+12]
	xor eax,dword [r8+rdx*4+3072]
	movzx rdx,byte [rdi+13]
	xor eax,dword [r8+rdx*4+2048]
	movzx rdx,byte [rdi+14]
	xor eax,dword [r8+rdx*4+1024]
	movzx rdx,byte [rdi+15]
	xor eax,dword [r8+rdx*4+0]

	lea rdi,[rdi+16]
	dec r9
	jnz calclop
	
	align 16
calclop1:
	mov r9,r11
	and r9,15
	shr r9,2
	jz calclop2
	align 16
calclop1a:
	xor eax,dword [rdi]
	lea rdi,[rdi+4]
	movzx rdx,al
	mov ebx,dword [r8+rdx*4+3072]
	movzx edx,ah
	shr eax,16
	xor ebx,dword [r8+rdx*4+2048]
	movzx rdx,al
	xor ebx,dword [r8+rdx*4+1024]
	movzx edx,ah
	xor ebx,dword [r8+rdx*4]

	dec r9
	mov eax,ebx
	jnz calclop1a
	
	align 16
calclop2:
	and r11,3
	jz calcexit
	
	align 16
calclop3:
	movzx rdx,al
	shr rax,8
	xor dl,byte [rdi]
	lea rdi,[rdi+1]
	xor eax,dword [r8+rdx*4]
	dec r11
	jnz calclop3
	
	align 16
calcexit:
	mov rbx,r10
	ret

