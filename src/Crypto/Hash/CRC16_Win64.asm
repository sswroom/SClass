section .text

global CRC16_InitTable
global CRC16_Calc

;void CRC16_InitTable(UInt32 *tab, UInt16 polynormial);
;8 rbx
;8 retAddr
;rcx tab
;rdx polynormial
	align 16
CRC16_InitTable:
	push rbx
	xor rbx,rbx
	align 16
crc16itlop:
	mov rax,rbx
	shl rax,8
	mov r8,8
	align 16
crc16itlop2:
	shl ax,1
	jnc crc16itlop3
	xor ax,dx
	align 16
crc16itlop3:
	dec r8
	jnz crc16itlop2
	mov word [rcx+rbx*2],ax
	inc rbx
	cmp rbx,256
	jb crc16itlop

	mov r8,256
	xor rdx,rdx
	align 16
crc16itlop4:
	lea rax,[r8-1]
	mov bx,word [rcx+r8*2-2]
	movzx edx,bh
	shl bx,8
	mov dx,word [rcx+rdx*2]
	xor dx,bx
	mov word [rcx+r8*2+512-2],dx
	mov ebx,edx
	movzx edx,bh
	shl bx,8
	mov dx,word [rcx+rdx*2]
	xor dx,bx
	mov word [rcx+r8*2+1024-2],dx
	mov ebx,edx
	movzx edx,bh
	shl bx,8
	mov dx,word [rcx+rdx*2]
	xor dx,bx
	mov word [rcx+r8*2+1536-2],dx
	mov ebx,edx
	movzx edx,bh
	shl bx,8
	mov dx,word [rcx+rdx*2]
	xor dx,bx
	mov word [rcx+r8*2+2048-2],dx
	mov ebx,edx
	movzx edx,bh
	shl bx,8
	mov dx,word [rcx+rdx*2]
	xor dx,bx
	mov word [rcx+r8*2+2560-2],dx
	mov ebx,edx
	movzx edx,bh
	shl bx,8
	mov dx,word [rcx+rdx*2]
	xor dx,bx
	mov word [rcx+r8*2+3072-2],dx
	mov ebx,edx
	movzx edx,bh
	shl bx,8
	mov dx,word [rcx+rdx*2]
	xor dx,bx
	mov word [rcx+r8*2+3584-2],dx
	mov ebx,edx
	movzx edx,bh
	shl bx,8
	mov dx,word [rcx+rdx*2]
	xor dx,bx
	mov word [rcx+r8*2+4096-2],dx
	mov ebx,edx
	movzx edx,bh
	shl bx,8
	mov dx,word [rcx+rdx*2]
	xor dx,bx
	mov word [rcx+r8*2+4608-2],dx
	mov ebx,edx
	movzx edx,bh
	shl bx,8
	mov dx,word [rcx+rdx*2]
	xor dx,bx
	mov word [rcx+r8*2+5120-2],dx
	mov ebx,edx
	movzx edx,bh
	shl bx,8
	mov dx,word [rcx+rdx*2]
	xor dx,bx
	mov word [rcx+r8*2+5632-2],dx
	mov ebx,edx
	movzx edx,bh
	shl bx,8
	mov dx,word [rcx+rdx*2]
	xor dx,bx
	mov word [rcx+r8*2+6144-2],dx
	mov ebx,edx
	movzx edx,bh
	shl bx,8
	mov dx,word [rcx+rdx*2]
	xor dx,bx
	mov word [rcx+r8*2+6656-2],dx
	mov ebx,edx
	movzx edx,bh
	shl bx,8
	mov dx,word [rcx+rdx*2]
	xor dx,bx
	mov word [rcx+r8*2+7168-2],dx
	mov ebx,edx
	movzx edx,bh
	shl bx,8
	mov dx,word [rcx+rdx*2]
	xor dx,bx
	mov word [rcx+r8*2+7680-2],dx
	dec r8
	jnz crc16itlop4
	
	pop rbx
	ret

;UInt16 CRC16_Calc(const UInt8 *buff, UOSInt buffSize, UInt16 *tab, UInt16 currVal)
;0 retAddr
;rcx buff
;rdx buffSize
;r8 tab
;r9 currVal
	align 16
CRC16_Calc:
	xchg r10,rbx
	mov rax,r9
	mov r9,rdx
	mov r11,r9
;	xor rbx,rbx
;	jmp crc16calclop3
	shr r9,4
	jz crc16calclop1
	
	align 16
crc16calclop:
	mov bx,word [rcx]
	xor bl,ah
	xor bh,al
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
	jnz crc16calclop
	
crc16calclop1:
	mov r9,r11
	and r9,15
	shr r9,2
	jz crc16calclop2
	align 16
crc16calclop1a:
	xchg ah,al
	mov bx,word [rcx]
	xor bx,ax
	movzx rdx,bl 
	mov ax,word [r8+rdx*2+1536]
	movzx edx,bh
	xor ax,word [r8+rdx*2+1024]
	movzx rdx,byte [rcx+2] 
	xor ax,word [r8+rdx*2+512]
	movzx rdx,byte [rcx+3]
	xor ax,word [r8+rdx*2+0]

	lea rcx,[rcx+4]
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
	xor bl,byte [rcx]
	xor ax,word [r8+rbx*2]
	lea rcx,[rcx+1]
	dec r11
	jnz crc16calclop3

	align 16
crc16calcexit:
	mov rbx,r10
	ret
