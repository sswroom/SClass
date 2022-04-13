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
	xor rcx,rcx ;i
	align 16
crc16clop:
	mov rax,rcx
	shl rax,8
	mov rdx,8
	align 16
crc16clop2:
	shl ax,1
	jnb crc16clop3
	xor ax,si
	align 16
crc16clop3:
	dec rdx
	jnz crc16clop2
	mov word [rdi+rcx*2],ax
	inc cl
	jnz crc16clop
	
	xor r8,r8
	xor rdx,rdx
	align 16
crc16itlop4:
	mov cx,word [rdi+r8*2]
	movzx edx,ch
	shl cx,8
	xor cx,word [rdi+rdx*2]
	mov word [rdi+r8*2+512],cx
	movzx edx,ch
	shl cx,8
	xor cx,word [rdi+rdx*2]
	mov word [rdi+r8*2+1024],cx
	movzx edx,ch
	shl cx,8
	xor cx,word [rdi+rdx*2]
	mov word [rdi+r8*2+1536],cx
	movzx edx,ch
	shl cx,8
	xor cx,word [rdi+rdx*2]
	mov word [rdi+r8*2+2048],cx
	movzx edx,ch
	shl cx,8
	xor cx,word [rdi+rdx*2]
	mov word [rdi+r8*2+2560],cx
	movzx edx,ch
	shl cx,8
	xor cx,word [rdi+rdx*2]
	mov word [rdi+r8*2+3072],cx
	movzx edx,ch
	shl cx,8
	xor cx,word [rdi+rdx*2]
	mov word [rdi+r8*2+3584],cx
	movzx edx,ch
	shl cx,8
	xor cx,word [rdi+rdx*2]
	mov word [rdi+r8*2+4096],cx
	movzx edx,ch
	shl cx,8
	xor cx,word [rdi+rdx*2]
	mov word [rdi+r8*2+4608],cx
	movzx edx,ch
	shl cx,8
	xor cx,word [rdi+rdx*2]
	mov word [rdi+r8*2+5120],cx
	movzx edx,ch
	shl cx,8
	xor cx,word [rdi+rdx*2]
	mov word [rdi+r8*2+5632],cx
	movzx edx,ch
	shl cx,8
	xor cx,word [rdi+rdx*2]
	mov word [rdi+r8*2+6144],cx
	movzx edx,ch
	shl cx,8
	xor cx,word [rdi+rdx*2]
	mov word [rdi+r8*2+6656],cx
	movzx edx,ch
	shl cx,8
	xor cx,word [rdi+rdx*2]
	mov word [rdi+r8*2+7168],cx
	movzx edx,ch
	shl cx,8
	xor cx,word [rdi+rdx*2]
	mov word [rdi+r8*2+7680],cx
	inc r8b
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
	mov rax,rcx
	xor rcx,rcx
	test rsi,rsi
	jz crc16calcexit
;	jmp crc16calclop3
	mov r8,rsi
	shr r8,4
	jz crc16calclop1
	
	align 16
crc16calclop:
	mov cx,word [rdi]
	xor cl,ah
	xor ch,al
	movzx r9,byte [rdi+15]
	mov ax,word [rdx+r9*2+0]
	movzx r9,byte [rdi+14]
	xor ax,word [rdx+r9*2+512]
	movzx r9,byte [rdi+13]
	xor ax,word [rdx+r9*2+1024]
	movzx r9,byte [rdi+12]
	xor ax,word [rdx+r9*2+1536]
	movzx r9,byte [rdi+11]
	xor ax,word [rdx+r9*2+2048]
	movzx r9,byte [rdi+10]
	xor ax,word [rdx+r9*2+2560]
	movzx r9,byte [rdi+9]
	xor ax,word [rdx+r9*2+3072]
	movzx r9,byte [rdi+8]
	xor ax,word [rdx+r9*2+3584]
	movzx r9,byte [rdi+7]
	xor ax,word [rdx+r9*2+4096]
	movzx r9,byte [rdi+6]
	xor ax,word [rdx+r9*2+4608]
	movzx r9,byte [rdi+5]
	xor ax,word [rdx+r9*2+5120]
	movzx r9,byte [rdi+4]
	xor ax,word [rdx+r9*2+5632]
	movzx r9,byte [rdi+3]
	xor ax,word [rdx+r9*2+6144]
	movzx r9,byte [rdi+2]
	xor ax,word [rdx+r9*2+6656]
	movzx r9,cl 
	xor ax,word [rdx+r9*2+7680]
	shr rcx,8
	xor ax,word [rdx+rcx*2+7168]
	lea rdi,[rdi+16]
	dec r8
	jnz crc16calclop
	
	and rsi,15
	jz crc16calcexit

crc16calclop1:
	mov r8,rsi
	shr r8,2
	jz crc16calclop3

	align 16
crc16calclop1a:
	mov cx,word [rdi]
	xor cl,ah
	xor ch,al
	movzx r9,byte [rdi+3]
	mov ax,word [rdx+r9*2+0]
	movzx r9,byte [rdi+2] 
	xor ax,word [rdx+r9*2+512]
	movzx r9,cl 
	xor ax,word [rdx+r9*2+1536]
	shr rcx,8
	xor ax,word [rdx+rcx*2+1024]

	lea rdi,[rdi+4]
	dec r8
	jnz crc16calclop1a
	
	and rsi,3
	jz crc16calcexit
	
	align 16
crc16calclop3:
	movzx ecx,ah
	shl ax,8
	xor cl,byte [rdi]
	xor ax,word [rdx+rcx*2]
	inc rdi
	dec rsi
	jnz crc16calclop3

crc16calcexit:
	ret

