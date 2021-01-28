section .text

global CRC16_InitTable
global _CRC16_InitTable
global CRC16_Calc
global _CRC16_Calc

;void CRC16_InitTable(UInt16 *tab, UInt16 polynomial);
;0 retAddr
;rdi tab
;rsi polynomial
	align 16
CRC16_InitTable:
_CRC16_InitTable:
	mov rcx,256
	align 16
crc16clop:
	lea rax,[rcx-1]
	mov edx,8
	shl rax,8
	align 16
crc16clop2:
	shl ax,1
	jnb crc16clop3
	xor ax,si
	align 16
crc16clop3:
	dec edx
	jnz crc16clop2
	mov word [rdi+rcx*2-2],ax
	dec rcx
	jnz crc16clop
	
	mov r8,256
	xor rdx,rdx
	align 16
crc16itlop4:
	lea rax,[r8-1]
	mov cx,word [rdi+r8*2-2]
	movzx edx,ch
	shl cx,8
	mov dx,word [rdi+rdx*2]
	xor dx,cx
	mov word [rdi+r8*2+512-2],dx
	mov ecx,edx
	movzx edx,ch
	shl cx,8
	mov dx,word [rdi+rdx*2]
	xor dx,cx
	mov word [rdi+r8*2+1024-2],dx
	mov ecx,edx
	movzx edx,ch
	shl cx,8
	mov dx,word [rdi+rdx*2]
	xor dx,cx
	mov word [rdi+r8*2+1536-2],dx
	mov ecx,edx
	movzx edx,ch
	shl cx,8
	mov dx,word [rdi+rdx*2]
	xor dx,cx
	mov word [rdi+r8*2+2048-2],dx
	mov ecx,edx
	movzx edx,ch
	shl cx,8
	mov dx,word [rdi+rdx*2]
	xor dx,cx
	mov word [rdi+r8*2+2560-2],dx
	mov ecx,edx
	movzx edx,ch
	shl cx,8
	mov dx,word [rdi+rdx*2]
	xor dx,cx
	mov word [rdi+r8*2+3072-2],dx
	mov ecx,edx
	movzx edx,ch
	shl cx,8
	mov dx,word [rdi+rdx*2]
	xor dx,cx
	mov word [rdi+r8*2+3584-2],dx
	mov ecx,edx
	movzx edx,ch
	shl cx,8
	mov dx,word [rdi+rdx*2]
	xor dx,cx
	mov word [rdi+r8*2+4096-2],dx
	mov ecx,edx
	movzx edx,ch
	shl cx,8
	mov dx,word [rdi+rdx*2]
	xor dx,cx
	mov word [rdi+r8*2+4608-2],dx
	mov ecx,edx
	movzx edx,ch
	shl cx,8
	mov dx,word [rdi+rdx*2]
	xor dx,cx
	mov word [rdi+r8*2+5120-2],dx
	mov ecx,edx
	movzx edx,ch
	shl cx,8
	mov dx,word [rdi+rdx*2]
	xor dx,cx
	mov word [rdi+r8*2+5632-2],dx
	mov ecx,edx
	movzx edx,ch
	shl cx,8
	mov dx,word [rdi+rdx*2]
	xor dx,cx
	mov word [rdi+r8*2+6144-2],dx
	mov ecx,edx
	movzx edx,ch
	shl cx,8
	mov dx,word [rdi+rdx*2]
	xor dx,cx
	mov word [rdi+r8*2+6656-2],dx
	mov ecx,edx
	movzx edx,ch
	shl cx,8
	mov dx,word [rdi+rdx*2]
	xor dx,cx
	mov word [rdi+r8*2+7168-2],dx
	mov ecx,edx
	movzx edx,ch
	shl cx,8
	mov dx,word [rdi+rdx*2]
	xor dx,cx
	mov word [rdi+r8*2+7680-2],dx
	dec r8
	jnz crc16itlop4
	
	ret


;UInt16 CRC16_Calc(const UInt8 *buff, UOSInt buffSize, UInt16 *tab, UInt16 currVal)
;0 retAddr
;rdi buff
;rsi buffSize
;rdx tab
;rcx currVal
	align 16
CRC16_Calc:
_CRC16_Calc:
	xchg r10,rbx
	mov rax,rcx
	mov r9,rsi
	mov r11,r9
	mov r8,rdx
	xor rbx,rbx
	jmp crc16calclop3
	shr r9,4
	jz crc16calclop1
	
	align 16
crc16calclop:
	xchg ah,al
	mov bx,word [rdi]
	xor bx,ax
	movzx rdx,bl 
	mov ax,word [r8+rdx*2+7680]
	movzx edx,bh
	xor ax,word [r8+rdx*2+7168]
	movzx rdx,byte [rdi+2]
	xor ax,word [r8+rdx*2+6656]
	movzx rdx,byte [rdi+3]
	xor ax,word [r8+rdx*2+6144]
	movzx rdx,byte [rdi+4]
	xor ax,word [r8+rdx*2+5632]
	movzx rdx,byte [rdi+5]
	xor ax,word [r8+rdx*2+5120]
	movzx rdx,byte [rdi+6]
	xor ax,word [r8+rdx*2+4608]
	movzx rdx,byte [rdi+7]
	xor ax,word [r8+rdx*2+4096]
	movzx rdx,byte [rdi+8]
	xor ax,word [r8+rdx*2+3584]
	movzx rdx,byte [rdi+9]
	xor ax,word [r8+rdx*2+3072]
	movzx rdx,byte [rdi+10]
	xor ax,word [r8+rdx*2+2560]
	movzx rdx,byte [rdi+11]
	xor ax,word [r8+rdx*2+2048]
	movzx rdx,byte [rdi+12]
	xor ax,word [r8+rdx*2+1536]
	movzx rdx,byte [rdi+13]
	xor ax,word [r8+rdx*2+1024]
	movzx rdx,byte [rdi+14]
	xor ax,word [r8+rdx*2+512]
	movzx rdx,byte [rdi+15]
	xor ax,word [r8+rdx*2+0]

	lea rdi,[rdi+16]
	dec r9
	jnz crc16calclop
	
crc16calclop1:
	mov r9,r11
	and r9,15
	shr r9,2
	jz crc16calclop2
	align 16
crc16calclop1a:
	xchg ah,al
	mov bx,word [rdi]
	xor bx,ax
	movzx rdx,bl 
	mov ax,word [r8+rdx*2+1536]
	movzx edx,bh
	xor ax,word [r8+rdx*2+1024]
	movzx rdx,byte [rdi+2] 
	xor ax,word [r8+rdx*2+512]
	movzx rdx,byte [rdi+3]
	xor ax,word [r8+rdx*2+0]

	lea rdi,[rdi+4]
	dec r9
	jnz crc16calclop1a
	
	align 16
crc16calclop2:
	and r11,3
	jz crc16calcexit
	
	xor rbx,rbx
	align 16
crc16calclop3:
	movzx ebx,ah
	shl ax,8
	xor bl,byte [rdi]
	xor ax,word [r8+rbx*2]
	lea rdi,[rdi+1]
	dec r11
	jnz crc16calclop3

	align 16
crc16calcexit:
	mov rbx,r10
	ret

