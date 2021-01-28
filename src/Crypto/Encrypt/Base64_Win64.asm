section .text

extern Base64_encArr
extern Base64_decArr

global Base64_Encrypt
global Base64_Decrypt

;OSInt Base64_Encrypt(const UInt8 *inBuff, OSInt inSize, UInt8 *outBuff, void *encParam);
;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx inBuff
;rdx inSize
;r8 outbuff
;r9 encParam
	align 16
Base64_Encrypt:
	push rbp
	push rbx
	push rsi
	push rdi
	
	mov rsi,rcx ;inBuff
	mov rax,rdx ;inSize
	mov rdx,0
	mov rcx,3
	div rcx

	mov rcx,rax
	test rdx,rdx
	jz b64elop
	inc eax
	align 16
b64elop:
	shl rax,2
	jz b64eexit

	mov rbx,r8 ;outBuff
	mov rdi,qword [rel Base64_encArr]
	cmp rbx,0
	jz b64eexit
	push rax
	push rdx
	xor rdx,rdx
	jecxz b64elop2b
	mov rbp,rcx
	align 16
b64elop2:
	movzx rax,word [rsi]
	movzx rdx,al
	shr rdx,2
	mov cl,byte [rdi+rdx]
	
	mov dh,ah
	and dh,0xf

	rol ax,4
	and rax,0x3f
	mov ch,byte [rdi+rax]
	mov word [rbx],cx

	mov al,byte [rsi+2]
	mov dl,al
	shr rdx,6
	mov cl,byte [rdi+rdx]
	
	and rax,0x3f
	mov ch,byte [rdi+rax]
	mov word [rbx+2],cx

	add rsi,3
	add rbx,4
	dec rbp
	jnz b64elop2
	align 16
b64elop2b:
	pop rdx
	cmp rdx,1
	jb b64elop3
	jz b64elop4

	movzx rax,byte [rsi]
	mov dh,al
	shr rax,2
	mov al,byte [rdi+rax]
	mov byte [rbx],al

	mov dl,byte [rsi+1]
	mov ah,dl
	shr rdx,4
	and rdx,0x3f
	mov al,byte [rdi+rdx]
	mov byte [rbx+1],al

	mov ah,byte [rsi+1]
	mov al,0
	shr rax,6
	and rax,0x3f
	mov al,byte [rdi+rax]
	mov ah,'='
	mov word [rbx+2],ax

	pop rax
	jmp b64eexit
	
	align 16
b64elop3:
	pop rax
	jmp b64eexit
	
	align 16
b64elop4:
	movzx rax,byte [rsi]
	mov dh,al
	shr rax,2
	mov al,byte [rdi+rax]

	mov dl,0
	shr rdx,4
	and rdx,0x3f
	mov ah,byte [rdi+rdx]
	mov word [rbx],ax

	mov ax,'=='
	mov word [rbx+2],ax

	pop rax

	align 16
b64eexit:
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

;OSInt Base64_Decrypt(const UInt8 *inBuff, OSInt inSize, UInt8 *outBuff, void *decParam);
;0 rdi
;8 rsi
;16 rbx
;24 rbp
;32 retAddr
;rcx inBuff
;rdx inSize
;r8 outbuff
;r9 encParam
	align 16
Base64_Decrypt:
	push rbp
	push rbx
	push rsi
	push rdi
	mov rsi,rcx ;inBuff
	mov rdi,r8 ;outBuff
	mov rcx,rdx ;inSize
	xor r8,r8
	test rcx,3
	jnz b64dexit
	xor rbx,rbx
	xor rdx,rdx
	test rcx,rcx
	jz b64dexit
	lea rbp,[rel Base64_decArr]
	test rdi,rdi
	jz b64dlop11
	align 16
b64dlop:
	movzx rax,byte [rsi]
	inc rsi
	mov dl,byte [rax+rbp]
	cmp dl,0xff
	jz b64dlop2
	cmp ebx,1
	jb b64dlop8
	je b64dlop9
	cmp ebx,3
	jb b64dlop10
	mov ebx,0
	shl dl,2
	shr dx,2
	inc r8
	mov byte [rdi],dl
	inc rdi
	jmp b64dlop2
	align 16
b64dlop8:
	mov dh,dl
	mov ebx,1
	jmp b64dlop2
	align 16
b64dlop9:
	mov ebx,2
	shl dl,2
	shl edx,2
	inc r8
	mov byte [rdi],dh
	and edx,0xff
	inc rdi
	shl edx,4
	jmp b64dlop2
	align 16
b64dlop10:
	mov ebx,3
	shl dl,2
	shl edx,4
	inc r8
	mov byte [rdi],dh
	and edx,0xff
	inc rdi
	shl edx,2

	align 16
b64dlop2:
	dec rcx
	jnz b64dlop
	jmp b64dexit

	align 16
b64dlop11:
	movzx rax,byte [rsi]
	mov dl,byte [rax+rbp]
	inc rsi
	cmp dl,0xff
	jz b64dlop13
	inc r8
	align 16
b64dlop13:
	dec ecx
	jnz b64dlop
	mov rax,r8
	and r8,3
	shr rax,2
	dec r8
	lea rax,[rax+rax*2]
	add rax,r8
	align 16
b64dexit:
	mov rax,r8
	pop rdi
	pop rsi
	pop rbx
	pop rbp
	ret

