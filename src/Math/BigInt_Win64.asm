section .text

global BigInt_ByteSwap
global BigInt_Neg
global BigInt_Add
global BigInt_And
global BigInt_Or
global BigInt_Xor
global BigInt_AssignI32
global BigInt_AssignStrW16
global BigInt_MulUI32
global BigInt_DivUI32
global BigInt_ToStringW16

;void BigInt_ByteSwap(const UInt8 *srcBuff, UInt8 *destBuff, OSInt valSize)
;0 retAddr
;rcx srcBuff
;rdx destBuff
;r8 valSize
	align 16
BigInt_ByteSwap:
	add rdx,r8
	shr r8,3
	
	align 16
bibslop:
	lea rdx,[rdx-8]
	mov rax,qword [rcx]
	bswap rax
	lea rcx,[rcx+8]
	mov qword [rdx],rax
	dec r8
	jnz bibslop
	ret

;void BigInt_Neg(UInt8 *valBuff, OSInt valSize);
;0 retAddr
;rcx valBuff
;rdx valSize
	align 16
BigInt_Neg:
	shr rdx,3
	mov r8,rcx
	mov r9,rdx
	align 16
bineglop:
	not qword [rcx]
	lea rcx,[rcx+4]
	dec rdx
	jnz bineglop

	mov rcx,r8
	mov rdx,r9
	align 16
bineglop2:
	inc qword [rcx]
	jnb bineglop3
	lea rcx,[rcx+8]
	dec rdx
	jnz bineglop2
	align 16
bineglop3:
	ret

;void BigInt_Add(UInt8 *destBuff, UInt8 *srcBuff, OSInt destSize, OSInt srcSize)
;0 rbx
;8 retAddr
;rcx destBuff
;rdx srcBuff
;r8 destSize
;r9 srcSize
	align 16
BigInt_Add:
	push rbx
	cmp r8,r9
	jnb biaddlop
	xor rax,rax
	xor rbx,rbx
	shr r8,3
	align 16
biaddlop2:
	add rax,qword [rdx]
	setb bl
	add qword [rcx],rax
	adc rbx,0
	lea rdx,[rdx+4]
	lea rcx,[rcx+4]
	mov rax,rbx
	dec r8
	jnz biaddlop2
	jmp biaddexit
	
	align 16
biaddlop:
	sub r8,r9
	xor eax,eax
	xor ebx,ebx
	shr r9,3
	
	align 16
biaddlop3:
	add rax,qword [rdx]
	setb bl
	add qword [rcx],rax
	adc rbx,0
	lea rdx,[rdx+8]
	lea rcx,[rcx+8]
	mov rax,rbx
	dec r9
	jnz biaddlop3
	
	test byte [rdx-1],0x80
	jnz biaddlop4
	test rax,rax
	jz biaddexit
	shr r8,3
	
	align 16
biaddlop5:
	inc qword [rcx]
	jnb biaddexit
	lea rcx,[rcx+8]
	dec r8
	jnz biaddlop5
	jmp biaddexit
	
	align 16
biaddlop4:
	test rax,rax
	jnz biaddexit
	shr r8,3
	
	align 16
biaddlop6:
	sub qword [rcx],1
	jb biaddexit
	lea rcx,[rcx+8]
	dec r8
	jnz biaddlop6
	
	align 16
biaddexit:
	pop rbx
	ret

;void BigInt_And(UInt8 *destBuff, UInt8 *srcBuff, OSInt destSize, OSInt srcSize)
;0 retAddr
;rcx destBuff
;rdx srcBuff
;r8 destSize
;r9 srcSize
	align 16
BigInt_And:
	cmp r8,r9
	jnb biandlop
	shr r8,3
	
	align 16
biandlop1:
	mov rax,qword [rdx]
	and qword [rcx],rax
	lea rdx,[rdx+8]
	lea rcx,[rcx+8]
	dec r8
	jnz biandlop1
	jmp biandexit
	
	align 16
biandlop:
	sub r8,r9
	shr r9,3
	align 16
biandlop2:
	mov rax,qword [rdx]
	and qword [rcx],rax
	lea rdx,[rdx+8]
	lea rcx,[rcx+8]
	dec r9
	jnz biandlop2
	cmp rax,0
	jge biandexit
	mov rax,-1
	shr r8,3
	align 16
biandlop3:
	and qword [rcx],rax
	lea rcx,[rcx+8]
	dec ecx
	jnz biandlop3
	align 16
biandexit:
	ret

;void BigInt_Or(UInt8 *destBuff, UInt8 *srcBuff, OSInt destSize, OSInt srcSize)
;0 retAddr
;rcx destBuff
;rdx srcBuff
;r8 destSize
;r9 srcSize
	align 16
BigInt_Or:
	cmp r8,r9
	jnb biorlop
	shr r8,3
	
	align 16
biorlop1:
	mov rax,qword [rdx]
	or qword [rcx],rax
	lea rdx,[rdx+8]
	lea rcx,[rcx+8]
	dec r8
	jnz biorlop1
	jmp biorexit
	
	align 16
biorlop:
	sub r8,r9
	shr r9,3
	align 16
biorlop2:
	mov rax,qword [rdx]
	or qword [rcx],rax
	lea rdx,[rdx+8]
	lea rcx,[rcx+8]
	dec r9
	jnz biorlop2
	cmp rax,0
	jge biorexit
	mov rax,-1
	shr r8,3
	align 16
biorlop3:
	or qword [rcx],rax
	lea rcx,[rcx+8]
	dec ecx
	jnz biorlop3
	align 16
biorexit:
	ret

;void BigInt_Xor(UInt8 *destBuff, UInt8 *srcBuff, OSInt destSize, OSInt srcSize)
;0 retAddr
;rcx destBuff
;rdx srcBuff
;r8 destSize
;r9 srcSize
	align 16
BigInt_Xor:
	cmp r8,r9
	jnb bixorlop
	shr r8,3
	
	align 16
bixorlop1:
	mov rax,qword [rdx]
	xor qword [rcx],rax
	lea rdx,[rdx+8]
	lea rcx,[rcx+8]
	dec r8
	jnz bixorlop1
	jmp bixorexit
	
	align 16
bixorlop:
	sub r8,r9
	shr r9,3
	align 16
bixorlop2:
	mov rax,qword [rdx]
	xor qword [rcx],rax
	lea rdx,[rdx+8]
	lea rcx,[rcx+8]
	dec r9
	jnz bixorlop2
	cmp rax,0
	jge bixorexit
	mov rax,-1
	shr r8,3
	align 16
bixorlop3:
	xor qword [rcx],rax
	lea rcx,[rcx+8]
	dec ecx
	jnz bixorlop3
	align 16
bixorexit:
	ret

;void BigInt_AssignI32(UInt8 *valBuff, OSInt valSize, Int32 val);
;0 retAddr
;rcx valBuff
;rdx valSize
;r8 val
	align 16
BigInt_AssignI32:
	shr rdx,4
	movd xmm0,r8d
	sar r8d,31
	movd xmm1,r8d
	punpckldq xmm0,xmm1
	punpckldq xmm1,xmm1
	punpcklqdq xmm0,xmm1
	movdqa [rcx],xmm0
	lea rcx,[rcx+16]
	dec rdx
	jz biei32exit
	punpckldq xmm1,xmm1
	align 16
biei32lop:
	movdqa [rcx],xmm1
	lea rcx,[rcx+16]
	dec rdx
	jnz biei32lop
		
	align 16
biei32exit:
	ret

;void BigInt_AssignStrW16(UInt8 *valBuff, OSInt valSize, const WChar *val);
;0 rbx
;8 rdi
;16 rsi
;24 retAddr
;rcx valBuff
;rdx valSize
;r8 val
	align 16
BigInt_AssignStrW16:
	push rsi
	push rdi
	push rbx
	push rbp
	mov rsi,r8
	mov rdi,rcx
	mov rcx,rdx
	cmp word [rsi],'0'
	jnz biesw16lop2
	cmp word [rsi+2],'x'
	jnz biesw16lop2
	
	align 16
biesw16lop1a:
	cmp word [rsi],0
	jz biesw16lop1b
	lea esi,[rsi+2]
	jmp biesw16lop1a
	
	align 16
biesw16lop1b:
	lea rsi,[rsi-2]
	movzx rax,word [rsi]
	cmp rax,0x30
	jb biesw16lop1c
	cmp rax,0x3a
	jb biesw16lop1d
	cmp rax,0x41
	jb biesw16lop1c
	cmp rax,0x47
	jb biesw16lop1e
	cmp rax,0x61
	jb biesw16lop1c
	cmp rax,0x67
	jnb biesw16lop1c
	lea rdx,[rax-0x57]
	jmp biesw16lop1f
	
	align 16
biesw16lop1c:
	xor rax,rax
	rep stosb
	jmp biesw16exit

	align 16
biesw16lop1d:
	lea rdx,[rax-0x30]
	jmp biesw16lop1f
	
	align 16
biesw16lop1e:
	lea rdx,[rax-0x37]
	align 16
biesw16lop1f:
	lea rsi,[rsi-2]
	movzx rax,word [rsi]
	cmp rax,0x30
	jb biesw16lop1g
	cmp rax,0x3a
	jb biesw16lop1h
	cmp rax,0x41
	jb biesw16lop1g
	cmp rax,0x47
	jb biesw16lop1i
	cmp rax,0x61
	jb biesw16lop1g
	cmp rax,0x67
	jnb biesw16lop1g
	lea r9,[rax-0x57]
	jmp biesw16lop1j
	
	align 16
biesw16lop1g:
	mov rax,rdx
	stosb
	dec rcx
	jz biesw16exit
	xor rax,rax
	rep stosb
	jmp biesw16exit

	align 16
biesw16lop1h:
	lea r9,[rax-0x30]
	jmp biesw16lop1j
	
	align 16
biesw16lop1i:
	lea r9,[rax-0x37]
	align 16
biesw16lop1j:
	shl r9,4
	or rdx,r9
	mov rax,rdx
	stosb
	dec rcx
	jnz biesw16lop1b
	jmp biesw16exit

	align 16
biesw16lop2:
	mov r9,rdi
	mov r10,rcx
	shr rcx,3
	xor rax,rax
	rep stosq
	cmp word [rsi],'-'
	jnz biesw16lop2a
	lea rsi,[rsi+2]
	mov rax,1
	
	align 16
biesw16lop2a:
	mov r11,rax
	mov rcx,10
	mov rdi,r9 ;valBuff
	
	align 16
biesw16lop2b:
	movzx rax,word [rsi]
	lea rsi,[rsi+2]
	cmp rax,0x30
	jb biesw16lop2e
	cmp rax,0x39
	jnbe biesw16lop2e
	
	mov rbx,r9 ;valBuff
	lea rdx,[rax-0x30]
	align 16
biesw16lop2c:
	cmp rdi,rbx
	jbe biesw16lop2d
	mov rax,qword [rbx]
	mov rbp,rdx
	mul rcx
	add rax,rbp
	adc rdx,0
	mov qword [rbx],rax

	lea rbx,[rbx+8]
	jmp biesw16lop2c
	
	align 16
biesw16lop2d:
	cmp rdx,0
	jz biesw16lop2b
	mov rax,rdi
	sub rax,r9 ;valBuff
	cmp rax,r10 ;valSize
	jnb biesw16exit
	mov qword [rdi],rdx
	lea rdi,[rdi+8]
	jmp biesw16lop2b
	
	align 16
biesw16lop2e:
	cmp r11,1
	jnz biesw16exit
	mov rcx,r9 ;valBuff
	mov rdx,r10 ;valSize
	call BigInt_Neg
	
	align 16
biesw16exit:	
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret
	
;UInt32 BigInt_MulUI32(UInt8 *valBuff, OSInt valSize, UInt32 val);
;8 retAddr
;rcx valBuff
;rdx valSize
;r8 val
	align 16
BigInt_MulUI32:
	mov r9,rdx
	xor rdx,rdx
	shr r9,3
	align 16
bimui32lop:
	mov r10,rdx
	mov rax,qword [rcx]
	mul r8
	add rax,r10
	adc rdx,0
	mov qword [rcx],rax
	lea rcx,[rcx+8]
	dec r9
	jnz bimui32lop
	mov rax,rdx
	ret

;UInt32 BigInt_DivUI32(UInt8 *valBuff, OSInt valSize, UInt32 val);
;0 rbp
;8 rdi
;16 rsi
;24 rbx
;32 retAddr
;rcx valBuff
;rdx valSize
;r8 val
	align 16
BigInt_DivUI32:
	push rbx
	push rsi
	push rdi
	push rbp
	
	mov rdi,rcx ;valBuff
	mov rsi,r8 ;val
	mov r9,rdx ;valSize
	mov r10,rcx
	mov r11,rdx
	add rdi,r9
	shr r9,3
	test byte [rdi-1],0x80
	jz bidui32lop2a
	mov rbp,1
	push r9
	push r10
	push r11
	mov rcx,r10
	mov rdx,r11
	call BigInt_Neg
	pop r11
	pop r10
	pop r9
	jmp bidui32lop2
	
	align 16
bidui32lop2a:
	xor rbp,rbp
	align 16
bidui32lop2:
	xor rdx,rdx
	align 16
bidui32lop:
	lea rdi,[rdi-8]
	mov rax,qword [rdi]
	div rsi
	mov qword [rdi],rax
	dec r9
	jnz bidui32lop

	mov rax,rdx
	test rbp,rbp
	jz bidui32lop3
	mov rbp,rdx
	mov rcx,r10
	mov rdx,r11
	call BigInt_Neg
	mov rax,rbp

	align 16
bidui32lop3:
	pop rbp
	pop rdi
	pop rsi
	pop rbx
	ret

;WChar *BigInt_ToStringW16(WChar *buff, UInt8 *valArr, UInt8 *tmpArr, OSInt valSize)
;0 val2
;8 val3
;16 ebp
;24 ebx
;32 edi
;40 esi
;48 retAddr
;rcx buff r10
;rdx valArr r11
;r8 tmpArr
;r9 valSize

	align 16
BigInt_ToStringW16:
	push rsi
	push rdi
	push rbx
	push rbp
	push rax
	push rax
	mov r10,rcx
	mov r11,rdx
	mov rsi,rdx
	mov rdi,r8
	mov rcx,r9
	shr rcx,4
	align 16
bitsw16lop11:
	movaps xmm0,[rsi]
	movaps [rdi],xmm0
	lea rsi,[rsi+16]
	lea rdi,[rdi+16]
	dec rcx
	jnz bitsw16lop11
	
	test byte [rdi-1],0x80
	jz bitsw16lop
	mov rbx,r10 ;buff
	mov word [rbx],'-'
	mov rcx,r9 ;valSize
	add r10,2 ;buff
	lea rbx,[rbx+2]
	shr rcx,3
	align 16
bitsw16lop2:
	lea rdi,[rdi-8]
	not qword [rdi]
	dec rcx
	jnz bitsw16lop2
	mov rcx,r9 ;valSize
	shr rcx,3
	align 16
bitsw16lop3:
	inc qword [rdi]
	jnb bitsw16lop
	lea rdi,[rdi+8]
	dec rcx
	jnz bitsw16lop3

	align 16
bitsw16lop:
	xor rbp,rbp ;val1
	mov rcx,r9 ;valSize
	mov rdi,r8 ;tmpArr
	add rdi,rcx
	shr rcx,3
	mov qword [rsp+0],rcx ;val2
	mov qword [rsp+8],rdi ;val3
	align 16
bitsw16lop4:
	mov rcx,qword [rsp+0] ;val2
	mov rdi,qword [rsp+8] ;val3
	align 16
bitsw16lop5:
	cmp qword [rdi-8],0
	jnz bitsw16lop6
	lea rdi,[rdi-8]
	dec rcx
	jnz bitsw16lop5
	jmp bitsw16lop7
	
	align 16
bitsw16lop6:
	inc rbp ;val1
	mov qword [rsp+0],rcx ;val2
	mov qword [rsp+8],rdi ;val3
	mov rdx,0
	mov rbx,10
	align 16
bitsw16lop6b:
	sub rdi,8
	mov rax,qword [rdi]
	div rbx
	mov qword [rdi],rax
	dec rcx
	jnz bitsw16lop6b
	jmp bitsw16lop4

	align 16
bitsw16lop7:
	test rbp,rbp ;val1
	jnz bitsw16lop8
	mov rbx,r10 ;buff
	mov qword [rbx],'0'
	lea rax,[rbx+2]
	jmp bitsw16exit

	align 16
bitsw16lop8:
	mov rsi,r11 ;valArr
	mov rdi,r8 ;tmpArr
	mov rcx,r9 ;valSize
	shr rcx,4
	align 16
bitsw16lop8a:
	movaps xmm0,[rsi]
	movaps [rdi],xmm0
	lea rsi,[rsi+16]
	lea rdi,[rdi+16]
	dec rcx
	jnz bitsw16lop8a
	test byte [rdi-1],0x80
	jz bitsw16lop9
	mov rcx,r9 ;valSize
	shr rcx,3
	align 16
bitsw16lop8b:
	lea rdi,[rdi-8]
	not qword [rdi]
	dec rcx
	jnz bitsw16lop8b
	mov rcx,r9 ;valSize
	shr rcx,3
bitsw16lop8c:
	inc qword [rdi]
	jnb bitsw16lop9
	lea rdi,[rdi+8]
	dec rcx
	jnz bitsw16lop8c

;rcx buff r10
;rdx valArr r11
;r8 tmpArr
;r9 valSize

	align 16
bitsw16lop9:
	mov rsi,r10 ;buff
	mov rdi,r8 ;tmpArr
	mov rcx,r9 ;valSize
	lea rsi,[rsi+rbp*2] ;val1
	add rdi,rcx
	shr rcx,3
	mov qword [rsp+0],rcx ;val2
	mov qword [rsp+8],rdi ;val3
	mov rbp,rsi ;val1
	mov word [rsi],0
	align 16
bitsw16lop9b:
	mov rcx,qword [rsp+0] ;val2
	mov rdi,qword [rsp+8] ;val3
	align 16
bitsw16lop9c:
	cmp qword [rdi-8],0
	jnz bitsw16lop9d
	lea rdi,[rdi-8]
	dec rcx
	jnz bitsw16lop9c
	jmp bitsw16lop10
	
	align 16
bitsw16lop9d:
	mov qword [rsp+8],rdi ;val3
	mov qword [rsp+0],rcx ;val2
	mov rdx,0
	mov rbx,10
	align 16
bitsw16lop9e:
	lea rdi,[rdi-8]
	mov rax,qword [rdi]
	div rbx
	mov qword [rdi],rax
	dec rcx
	jnz bitsw16lop9e
	add rdx,'0'
	mov word [rsi-2],dx
	lea rsi,[rsi-2]
	jmp bitsw16lop9b
	
	align 16
bitsw16lop10:
	mov rax,rbp ;val1
	align 16
bitsw16exit:
	add rsp,16
	pop rbp
	pop rbx
	pop rdi
	pop rsi
	ret
