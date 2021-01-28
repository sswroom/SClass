section .text

global CRC32R_InitTable
global CRC32R_Reverse
global CRC32R_Calc

extern UseAVX

;void CRC32R_InitTable(UInt32 *tab, UInt32 rpn);
;0 rdi
;8 rbx
;16 retAddr
;rcx tab
;rdx rpn
	align 16
CRC32R_InitTable:
	push rbx
	push rdi
	mov rbx,rdx ;rpn
	mov rdi,rcx ;tab
	mov rcx,256
	xor rdx,rdx
	align 16
crc32ritlop:
	lea eax,[rcx-1]
	mov edx,8
	align 16
crc32ritlop2:
	shr eax,1
	jnb crc32ritlop3
	xor eax,ebx
	align 16
crc32ritlop3:
	dec edx
	jnz crc32ritlop2
	mov dword [rdi+rcx*4-4],eax
	dec rcx
	jnz crc32ritlop
	
	
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
	
	pop rdi
	pop rbx
	ret

;UInt32 CRC32R_Reverse(UInt32 polynomial)
;0 retAddr
;rcx polynomial
	align 16
CRC32R_Reverse:
	mov edx,32
	xor rax,rax
	align 16
crc32rrlop:
	shl ecx,1
	rcr eax,1
	dec edx
	jnz crc32rrlop
	ret
	
;UInt32 CRC32R_Calc(const UInt8 *buff, UOSInt buffSize, UInt32 *tab, UInt32 currVal)
;0 retAddr
;rcx buff
;rdx buffSize
;r8 tab
;r9 currVal
	align 16
CRC32R_Calc:
	xchg r10,rbx
	mov rax,r9
	mov r9,rdx
	mov r11,r9
	shr r9,4
	jz crc32rcalclop1
	
	align 16
crc32rcalclop:
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
	jnz crc32rcalclop
	
	align 16
crc32rcalclop1:
	mov r9,r11
	and r9,15
	shr r9,2
	jz crc32rcalclop2
	align 16
crc32rcalclop1a:
	xor eax,dword [rcx]
	movzx rdx,al
	mov ebx,dword [r8+rdx*4+3072]
	movzx edx,ah
	shr eax,16
	xor ebx,dword [r8+rdx*4+2048]
	movzx rdx,al
	xor ebx,dword [r8+rdx*4+1024]
	movzx edx,ah
	xor ebx,dword [r8+rdx*4]

	lea rcx,[rcx+4]
	dec r9
	mov eax,ebx
	jnz crc32rcalclop1a
	
	align 16
crc32rcalclop2:
	and r11,3
	jz crc32rcalcexit
	
	align 16
crc32rcalclop3:
	movzx rdx,al
	shr eax,8
	xor dl,byte [rcx]
	lea rcx,[rcx+1]
	xor eax,dword [r8+rdx*4]
	dec r11
	jnz crc32rcalclop3
	
	align 16
crc32rcalcexit:
	mov rbx,r10
	ret
