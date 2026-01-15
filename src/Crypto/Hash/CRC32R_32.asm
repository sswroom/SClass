section .text

global _CRC32R_InitTable
global CRC32R_InitTable
global _CRC32R_Reverse
global CRC32R_Reverse
global _CRC32R_Calc
global CRC32R_Calc

;void CRC32R_InitTable(UInt32 *tab, UInt32 rpn);
;0 edi
;4 ebx
;8 retAddr
;12 tab
;16 rpn
	align 16
_CRC32R_InitTable:
CRC32R_InitTable:
	push ebx
	push edi
	mov ebx,dword [esp+16] ;rpn
	mov edi,dword [esp+12] ;tab
	mov ecx,256
	align 16
crc32clop:
	lea eax,[ecx-1]
	mov edx,8
	align 16
crc32clop2:
	shr eax,1
	jnb crc32clop3
	xor eax,ebx
	align 16
crc32clop3:
	dec edx
	jnz crc32clop2
	mov dword [edi+ecx*4-4],eax
	dec ecx
	jnz crc32clop
	pop edi
	pop ebx
	ret

;UInt32 CRC32R_Reverse(UInt32 polynomial)
;0 retAddr
;4 polynomial
	align 16
_CRC32R_Reverse:
CRC32R_Reverse:
	mov ecx,dword [esp+4]
	mov edx,32
	xor eax,eax
	align 16
crc32rlop:
	shl ecx,1
	rcr eax,1
	dec edx
	jnz crc32rlop
	ret
	
;UInt32 CRC32R_Calc(const UInt8 *buff, UIntOS buffSize, UInt32 *tab, UInt32 currVal)
;0 edi
;4 esi
;8 retAddr
;12 buff
;16 buffSize
;20 tab
;24 currVal
	align 16
_CRC32R_Calc:
CRC32R_Calc:
	push esi
	push edi
	
	mov ecx,dword [esp+12] ;buff
	mov edi,dword [esp+16] ;buffSize
	mov esi,dword [esp+20] ;tab
	mov eax,dword [esp+24] ;currVal
	align 16
calclop3:
	movzx edx,al
	shr eax,8
	xor dl,byte [ecx]
	lea ecx,[ecx+1]
	xor eax,dword [esi+edx*4]
	dec edi
	jnz calclop3

	pop edi
	pop esi
	ret
	