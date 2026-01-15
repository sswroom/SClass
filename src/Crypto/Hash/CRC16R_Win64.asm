section .text

global CRC16R_InitTable
global CRC16R_Calc

extern UseAVX

;void CRC16R_InitTable(UInt16 *tab, UInt16 rpn);
;0 rdi
;8 rbx
;16 retAddr
;rcx tab
;rdx rpn
	align 16
CRC16R_InitTable:
	push rbx
	push rdi
	mov rbx,rdx ;rpn
	mov rdi,rcx ;tab
	mov rcx,256
	xor rdx,rdx
	align 16
crc16ritlop:
	lea rax,[rcx-1]
	mov edx,8
	align 16
crc16ritlop2:
	shr ax,1
	jnb crc16ritlop3
	xor ax,bx
	align 16
crc16ritlop3:
	dec edx
	jnz crc16ritlop2
	mov word [rdi+rcx*2-2],ax
	dec rcx
	jnz crc16ritlop
	
	
	mov rcx,256
	align 16
crc16ritlop4:
	lea eax,[rcx-1]
	mov bx,word [rdi+rcx*2-2]
	movzx rdx,bl
	shr bx,8
	mov dx,word [rdi+rdx*2]
	xor dx,bx
	mov word [rdi+rcx*2+512-2],dx
	mov bx,dx
	movzx rdx,bl
	shr bx,8
	mov dx,word [rdi+rdx*2]
	xor dx,bx
	mov word [rdi+rcx*2+1024-2],dx
	mov bx,dx
	movzx rdx,bl
	shr bx,8
	mov dx,word [rdi+rdx*2]
	xor dx,bx
	mov word [rdi+rcx*2+1536-2],dx
	mov bx,dx
	movzx rdx,bl
	shr bx,8
	mov dx,word [rdi+rdx*2]
	xor dx,bx
	mov word [rdi+rcx*2+2048-2],dx
	mov bx,dx
	movzx rdx,bl
	shr bx,8
	mov dx,word [rdi+rdx*2]
	xor dx,bx
	mov word [rdi+rcx*2+2560-2],dx
	mov bx,dx
	movzx rdx,bl
	shr bx,8
	mov dx,word [rdi+rdx*2]
	xor dx,bx
	mov word [rdi+rcx*2+3072-2],dx
	mov bx,dx
	movzx rdx,bl
	shr bx,8
	mov dx,word [rdi+rdx*2]
	xor dx,bx
	mov word [rdi+rcx*2+3584-2],dx
	mov bx,dx
	movzx rdx,bl
	shr bx,8
	mov dx,word [rdi+rdx*2]
	xor dx,bx
	mov word [rdi+rcx*2+4096-2],dx
	mov bx,dx
	movzx rdx,bl
	shr bx,8
	mov dx,word [rdi+rdx*2]
	xor dx,bx
	mov word [rdi+rcx*2+4608-2],dx
	mov bx,dx
	movzx rdx,bl
	shr bx,8
	mov dx,word [rdi+rdx*2]
	xor dx,bx
	mov word [rdi+rcx*2+5120-2],dx
	mov bx,dx
	movzx rdx,bl
	shr bx,8
	mov dx,word [rdi+rdx*2]
	xor dx,bx
	mov word [rdi+rcx*2+5632-2],dx
	mov bx,dx
	movzx rdx,bl
	shr bx,8
	mov dx,word [rdi+rdx*2]
	xor dx,bx
	mov word [rdi+rcx*2+6144-2],dx
	mov bx,dx
	movzx rdx,bl
	shr bx,8
	mov dx,word [rdi+rdx*2]
	xor dx,bx
	mov word [rdi+rcx*2+6656-2],dx
	mov bx,dx
	movzx rdx,bl
	shr bx,8
	mov dx,word [rdi+rdx*2]
	xor dx,bx
	mov word [rdi+rcx*2+7168-2],dx
	mov bx,dx
	movzx rdx,bl
	shr bx,8
	mov dx,word [rdi+rdx*2]
	xor dx,bx
	mov word [rdi+rcx*2+7680-2],dx
	dec rcx
	jnz crc16ritlop4
	
	pop rdi
	pop rbx
	ret

;UInt16 CRC16R_Calc(const UInt8 *buff, UIntOS buffSize, UInt16 *tab, UInt16 currVal)
;0 retAddr
;rcx buff
;rdx buffSize
;r8 tab
;r9 currVal
	align 16
CRC16R_Calc:
	xchg r10,rbx
	mov rax,r9
	mov r9,rdx
	mov r11,r9
	shr r9,4
	jz crc16rcalclop1
	
	align 16
crc16rcalclop:
	mov bx,word [rcx]
	xor bx,ax
	movzx rdx,byte [rcx+15]
	mov ax,word [r8+rdx*2+0]
	movzx rdx,byte [rcx+14]
	xor ax,word [r8+rdx*2+512]
	movzx rdx,byte [rcx+13]
	xor ax,word [r8+rdx*2+1024]
	movzx rdx,byte [rcx+12]
	xor ax,word [r8+rdx*2+1536]
	movzx rdx,byte [rcx+11]
	xor ax,word [r8+rdx*2+2048]
	movzx rdx,byte [rcx+10]
	xor ax,word [r8+rdx*2+2560]
	movzx rdx,byte [rcx+9]
	xor ax,word [r8+rdx*2+3072]
	movzx rdx,byte [rcx+8]
	xor ax,word [r8+rdx*2+3584]
	movzx rdx,byte [rcx+7]
	xor ax,word [r8+rdx*2+4096]
	movzx rdx,byte [rcx+6]
	xor ax,word [r8+rdx*2+4608]
	movzx rdx,byte [rcx+5]
	xor ax,word [r8+rdx*2+5120]
	movzx rdx,byte [rcx+4]
	xor ax,word [r8+rdx*2+5632]
	movzx rdx,byte [rcx+3]
	xor ax,word [r8+rdx*2+6144]
	movzx rdx,byte [rcx+2]
	xor ax,word [r8+rdx*2+6656]
	movzx edx,bh
	xor ax,word [r8+rdx*2+7168]
	movzx rdx,bl
	xor ax,word [r8+rdx*2+7680]

	lea rcx,[rcx+16]
	dec r9
	jnz crc16rcalclop
	
	align 16
crc16rcalclop1:
	mov r9,r11
	and r9,15
	shr r9,1
	jz crc16rcalclop2
	align 16
crc16rcalclop1a:
	xor ax,word [rcx]
	movzx edx,ah
	mov bx,word [r8+rdx*2]
	movzx rdx,al
	xor bx,word [r8+rdx*2+512]

	lea rcx,[rcx+2]
	dec r9
	mov ax,bx
	jnz crc16rcalclop1a
	
	align 16
crc16rcalclop2:
	and r11,1
	jz crc16rcalcexit
	
	align 16
crc16rcalclop3:
	movzx rdx,al
	shr ax,8
	xor dl,byte [rcx]
	lea rcx,[rcx+1]
	xor ax,word [r8+rdx*2]
	dec r11
	jnz crc16rcalclop3
	
	align 16
crc16rcalcexit:
	mov rbx,r10
	ret
