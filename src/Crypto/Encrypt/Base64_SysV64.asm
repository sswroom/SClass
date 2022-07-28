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

extern _Base64_encArr
extern _Base64_decArr

global Base64_Encrypt
global _Base64_Encrypt
global Base64_Decrypt
global _Base64_Decrypt

;OSInt Base64_Encrypt(const UInt8 *inBuff, OSInt inSize, UInt8 *outBuff, void *encParam);
;0 rbx
;8 rbp
;16 retAddr
;rdi inBuff
;rsi inSize
;rdx outbuff
;rcx encParam
	align 16
Base64_Encrypt:
_Base64_Encrypt:
	push rbp
	push rbx
	
	mov r8,rdx ;outBuff
	mov rax,rsi ;inSize
	xor rdx,rdx
	mov rcx,3
	div rcx

	mov r9,rax
	test rdx,rdx
	jz b64elop
	inc rax
	align 16
b64elop:
	mov rsi,rdx
	shl rax,2
	mov r10,rax
	jz b64eexit

	mov rbp,qword [rel _Base64_encArr]
	test r8,r8 ;outBuff
	jz b64eexit
	test r9,r9
	jz b64elop2b

	align 16
b64elop2:
	movzx rax,word [rdi]
	movzx rdx,al
	shr rdx,2
	mov cl,byte [rbp+rdx]
	
	mov dh,ah
	and dh,0xf

	rol ax,4
	and rax,0x3f
	mov ch,byte [rbp+rax]
	mov word [r8],cx

	mov al,byte [rdi+2]
	mov dl,al
	shr rdx,6
	mov cl,byte [rbp+rdx]
	
	and rax,0x3f
	mov ch,byte [rbp+rax]
	mov word [r8+2],cx

	lea rdi,[rdi+3]
	lea r8,[r8+4]
	dec r9
	jnz b64elop2
	align 16
b64elop2b:
	cmp rsi,1
	jb b64eexit
	jz b64elop4

	movzx rax,byte [rdi]
	mov dh,al
	shr rax,2
	mov al,byte [rbp+rax]
	mov byte [r8],al

	mov dl,byte [rdi+1]
	mov ah,dl
	shr rdx,4
	and rdx,0x3f
	mov al,byte [rbp+rdx]
	mov byte [r8+1],al

	mov ah,byte [rdi+1]
	mov al,0
	shr rax,6
	and rax,0x3f
	mov al,byte [rbp+rax]
	mov ah,'='
	mov word [r8+2],ax

	jmp b64eexit
		
	align 16
b64elop4:
	movzx rax,byte [rdi]
	mov dh,al
	shr rax,2
	mov al,byte [rbp+rax]

	mov dl,0
	shr rdx,4
	and rdx,0x3f
	mov ah,byte [rbp+rdx]
	mov word [r8],ax

	mov ax,'=='
	mov word [r8+2],ax

	align 16
b64eexit:
	mov rax,r10
	pop rbx
	pop rbp
	ret

;OSInt Base64_Decrypt(const UInt8 *inBuff, OSInt inSize, UInt8 *outBuff, void *decParam);
;16 rbx
;24 rbp
;32 retAddr
;rdi inBuff
;rsi inSize
;rdx outbuff
;rcx encParam
	align 16
Base64_Decrypt:
_Base64_Decrypt:
	mov r10,rsi ;inSize
	xor r8,r8 ;ret
	mov rsi,rdx ;outBuff
	test r10,3 ;inSize
	jnz b64dexit
	test r10,r10 ;inSize
	jz b64dexit
	lea r11,[rel _Base64_decArr]
	xor rcx,rcx
	xor rdx,rdx
	test rsi,rsi
	jz b64dlop11
	align 16
b64dlop:
	movzx rax,byte [rdi]
	mov dl,byte [rax+r11]
	lea rdi,[rdi+1]
	cmp dl,0xff
	jz b64dlop2
	cmp ecx,1
	jb b64dlop8
	je b64dlop9
	cmp ecx,3
	jb b64dlop10
	mov ecx,0
	shl dl,2
	shr dx,2
	inc r8
	mov byte [rsi],dl
	lea rsi,[rsi+1]
	jmp b64dlop2
	align 16
b64dlop8:
	mov dh,dl
	mov ecx,1
	jmp b64dlop2
	align 16
b64dlop9:
	mov ecx,2
	shl dl,2
	shl edx,2
	inc r8
	mov byte [rsi],dh
	and edx,0xff
	inc rsi
	shl edx,4
	jmp b64dlop2
	align 16
b64dlop10:
	mov ecx,3
	shl dl,2
	shl edx,4
	inc r8
	mov byte [rsi],dh
	and edx,0xff
	inc rsi
	shl edx,2

	align 16
b64dlop2:
	dec r10
	jnz b64dlop
	jmp b64dexit

	align 16
b64dlop11:
	movzx rax,byte [rdi]
	mov dl,byte [rax+r11]
	inc rdi
	cmp dl,0xff
	jz b64dlop13
	inc r8
	align 16
b64dlop13:
	dec r10
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
	ret

