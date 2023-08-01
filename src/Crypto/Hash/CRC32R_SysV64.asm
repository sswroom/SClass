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

global CRC32R_InitTable
global _CRC32R_InitTable
global CRC32R_Reverse
global _CRC32R_Reverse
global CRC32R_Calc
global _CRC32R_Calc

extern _UseSSE42

;void CRC32R_InitTable(UInt32 *tab, UInt32 rpn);
;16 retAddr
;rdi tab
;rsi rpn
	align 16
CRC32R_InitTable:
_CRC32R_InitTable:
	xor rcx,rcx
	align 16
crc32clop:
	mov rax,rcx

	xor edx,edx
	shr eax,1
	cmovb edx,esi
	xor eax,edx
	xor edx,edx
	shr eax,1
	cmovb edx,esi
	xor eax,edx
	xor edx,edx
	shr eax,1
	cmovb edx,esi
	xor eax,edx
	xor edx,edx
	shr eax,1
	cmovb edx,esi
	xor eax,edx
	xor edx,edx
	shr eax,1
	cmovb edx,esi
	xor eax,edx
	xor edx,edx
	shr eax,1
	cmovb edx,esi
	xor eax,edx
	xor edx,edx
	shr eax,1
	cmovb edx,esi
	xor eax,edx
	xor edx,edx
	shr eax,1
	cmovb edx,esi
	xor eax,edx

	mov dword [rdi+rcx*4],eax
	inc cl
	jnz crc32clop

	xor rcx,rcx
	align 16
crc32ritlop4:
	mov eax,dword [rdi+rcx*4]
	movzx edx,al
	shr eax,8
	xor eax,dword [rdi+rdx*4]
	mov dword [rdi+rcx*4+1024],eax
	movzx edx,al
	shr eax,8
	xor eax,dword [rdi+rdx*4]
	mov dword [rdi+rcx*4+2048],eax
	movzx edx,al
	shr eax,8
	xor eax,dword [rdi+rdx*4]
	mov dword [rdi+rcx*4+3072],eax
	movzx edx,al
	shr eax,8
	xor eax,dword [rdi+rdx*4]
	mov dword [rdi+rcx*4+4096],eax
	movzx edx,al
	shr eax,8
	xor eax,dword [rdi+rdx*4]
	mov dword [rdi+rcx*4+5120],eax
	movzx edx,al
	shr eax,8
	xor eax,dword [rdi+rdx*4]
	mov dword [rdi+rcx*4+6144],eax
	movzx edx,al
	shr eax,8
	xor eax,dword [rdi+rdx*4]
	mov dword [rdi+rcx*4+7168],eax
	movzx edx,al
	shr eax,8
	xor eax,dword [rdi+rdx*4]
	mov dword [rdi+rcx*4+8192],eax
	movzx edx,al
	shr eax,8
	xor eax,dword [rdi+rdx*4]
	mov dword [rdi+rcx*4+9216],eax
	movzx edx,al
	shr eax,8
	xor eax,dword [rdi+rdx*4]
	mov dword [rdi+rcx*4+10240],eax
	movzx edx,al
	shr eax,8
	xor eax,dword [rdi+rdx*4]
	mov dword [rdi+rcx*4+11264],eax
	movzx edx,al
	shr eax,8
	xor eax,dword [rdi+rdx*4]
	mov dword [rdi+rcx*4+12288],eax
	movzx edx,al
	shr eax,8
	xor eax,dword [rdi+rdx*4]
	mov dword [rdi+rcx*4+13312],eax
	movzx edx,al
	shr eax,8
	xor eax,dword [rdi+rdx*4]
	mov dword [rdi+rcx*4+14336],eax
	movzx edx,al
	shr eax,8
	xor eax,dword [rdi+rdx*4]
	mov dword [rdi+rcx*4+15360],eax
	inc cl
	jnz crc32ritlop4
	ret

;UInt32 CRC32R_Reverse(UInt32 polynomial)
;0 retAddr
;rdi polynomial
	align 16
CRC32R_Reverse:
_CRC32R_Reverse:
	xor rax,rax

	shl edi,1
	rcr eax,1
	shl edi,1
	rcr eax,1
	shl edi,1
	rcr eax,1
	shl edi,1
	rcr eax,1
	shl edi,1
	rcr eax,1
	shl edi,1
	rcr eax,1
	shl edi,1
	rcr eax,1
	shl edi,1
	rcr eax,1

	shl edi,1
	rcr eax,1
	shl edi,1
	rcr eax,1
	shl edi,1
	rcr eax,1
	shl edi,1
	rcr eax,1
	shl edi,1
	rcr eax,1
	shl edi,1
	rcr eax,1
	shl edi,1
	rcr eax,1
	shl edi,1
	rcr eax,1

	shl edi,1
	rcr eax,1
	shl edi,1
	rcr eax,1
	shl edi,1
	rcr eax,1
	shl edi,1
	rcr eax,1
	shl edi,1
	rcr eax,1
	shl edi,1
	rcr eax,1
	shl edi,1
	rcr eax,1
	shl edi,1
	rcr eax,1

	shl edi,1
	rcr eax,1
	shl edi,1
	rcr eax,1
	shl edi,1
	rcr eax,1
	shl edi,1
	rcr eax,1
	shl edi,1
	rcr eax,1
	shl edi,1
	rcr eax,1
	shl edi,1
	rcr eax,1
	shl edi,1
	rcr eax,1

	ret

;UInt32 CRC32R_Calc(const UInt8 *buff, UOSInt buffSize, UInt32 *tab, UInt32 currVal)
;0 retAddr
;rdi buff
;rsi buffSize
;rdx tab
;rcx currVal
	align 16
CRC32R_Calc:
_CRC32R_Calc:
	mov rax,rcx ;currVal
	test rsi,rsi ;buffSize
	jz calcexit
	
	cmp dword [rdx+512],0x82F63B78
	jnz calclop0
	cmp dword [rel _UseSSE42],0
	jnz calcsse42
	align 16
calclop0:
	mov r8,rsi ;buffSize
	shr r8,4 ;buffSize 
	jz calclop1
	mov r9,rax
calclop:
	mov eax,dword [rdi] ;buff
	xor eax,r9d
	movzx rcx,byte [rdi+15]
	mov r9d,dword [rdx+rcx*4+0]
	movzx rcx,byte [rdi+14]
	xor r9d,dword [rdx+rcx*4+1024]
	movzx rcx,byte [rdi+13]
	xor r9d,dword [rdx+rcx*4+2048]
	movzx rcx,byte [rdi+12]
	xor r9d,dword [rdx+rcx*4+3072]
	movzx rcx,byte [rdi+11]
	xor r9d,dword [rdx+rcx*4+4096]
	movzx rcx,byte [rdi+10]
	xor r9d,dword [rdx+rcx*4+5120]
	movzx rcx,byte [rdi+9]
	xor r9d,dword [rdx+rcx*4+6144]
	movzx rcx,byte [rdi+8]
	xor r9d,dword [rdx+rcx*4+7168]
	movzx rcx,byte [rdi+7]
	xor r9d,dword [rdx+rcx*4+8192]
	movzx rcx,byte [rdi+6]
	xor r9d,dword [rdx+rcx*4+9216]
	movzx rcx,byte [rdi+5]
	xor r9d,dword [rdx+rcx*4+10240]
	movzx rcx,byte [rdi+4]
	xor r9d,dword [rdx+rcx*4+11264]
	movzx rcx,al
	xor r9d,dword [rdx+rcx*4+15360]
	movzx ecx,ah
	shr eax,16
	xor r9d,dword [rdx+rcx*4+14336]
	movzx rcx,al
	xor r9d,dword [rdx+rcx*4+13312]
	movzx ecx,ah
	xor r9d,dword [rdx+rcx*4+12288]

	lea rdi,[rdi+16]
	dec r8
	jnz calclop
	mov rax,r9
	and rsi,15
	jz calcexit	

	align 16
calclop1:
	mov r8,rsi
	shr r8,2
	jz calclop2
	mov r9,rax
	align 16
calclop1a:
	xor eax,dword [rdi]
	lea rdi,[rdi+4]
	movzx rcx,al
	mov r9d,dword [rdx+rcx*4+3072]
	movzx ecx,ah
	shr eax,16
	xor r9d,dword [rdx+rcx*4+2048]
	movzx rcx,al
	xor r9d,dword [rdx+rcx*4+1024]
	movzx ecx,ah
	xor r9d,dword [rdx+rcx*4]

	dec r8
	mov rax,r9
	jnz calclop1a
	
	and rsi,3
	jz calcexit

	align 16
calclop2:
	movzx rcx,al
	shr rax,8
	xor cl,byte [rdi]
	lea rdi,[rdi+1]
	xor eax,dword [rdx+rcx*4]
	dec rsi
	jnz calclop2
	
	align 16
calcexit:
	ret

calcsse42:
	mov r8,rsi
	shr r8,6
	jz calcsse42_1a

	align 16
calcsse42_0:
	crc32 rax,qword [rdi]
	crc32 rax,qword [rdi+8]
	crc32 rax,qword [rdi+16]
	crc32 rax,qword [rdi+24]
	crc32 rax,qword [rdi+32]
	crc32 rax,qword [rdi+40]
	crc32 rax,qword [rdi+48]
	crc32 rax,qword [rdi+56]
	lea rdi,[rdi+64]
	dec r8
	jnz calcsse42_0
	and rsi,63
	jz calcexit

calcsse42_1a:
	mov r8,rsi
	shr r8,3
	jz calcsse42_2
	align 16
calcsse42_1:
	crc32 rax,qword [rdi]
	lea rdi,[rdi+8]
	dec r8
	jnz calcsse42_1

	and rsi,7
	jz calcexit

	align 16
calcsse42_2:
	crc32 eax,byte [rdi]
	lea rdi,[rdi+1]
	dec rsi
	jnz calcsse42_2
	ret
