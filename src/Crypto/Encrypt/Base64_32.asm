section .text

extern _Base64_encArr
extern _Base64_decArr

global _Base64_Encrypt
global Base64_Encrypt
global _Base64_Decrypt
global Base64_Decrypt

;IntOS Base64_Encrypt(const UInt8 *inBuff, IntOS inSize, UInt8 *outBuff);
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 inBuff
;24 inSize
;28 outbuff
	align 16
_Base64_Encrypt:
Base64_Encrypt:
	push ebp
	push ebx
	push esi
	push edi
	
	mov eax,dword [esp+24] ;inSize
	mov edx,0
	mov ecx,3
	div ecx

	mov ecx,eax
	test edx,edx
	jz b64elop
	inc eax
b64elop:
	shl eax,2
	jz b64eexit

	mov ebx,dword [esp+28] ;outBuff
	mov esi,dword [esp+20] ;inBuff
	mov edi,dword [_Base64_encArr]
	cmp ebx,0
	jz b64eexit
	push eax
	push edx
	test edx,edx
	jecxz b64elop2b
	mov ebp,ecx
b64elop2:
	movzx eax,word [esi]
	movzx edx,al
	shr edx,2
	mov cl,byte [edi+edx]
	
	mov dh,ah
	and dh,0xf

	rol ax,4
	and eax,0x3f
	mov ch,byte [edi+eax]
	mov word [ebx],cx

	mov al,byte [esi+2]
	mov dl,al
	shr edx,6
	mov cl,byte [edi+edx]
	
	and eax,0x3f
	mov ch,byte [edi+eax]
	mov word [ebx+2],cx

	add esi,3
	add ebx,4
	dec ebp
	jnz b64elop2
b64elop2b:

	pop edx
	cmp edx,1
	jb b64elop3
	jz b64elop4

	movzx eax,byte [esi]
	mov dh,al
	shr eax,2
	mov al,byte [edi+eax]
	mov byte [ebx],al

	mov dl,byte [esi+1]
	mov ah,dl
	shr edx,4
	and edx,0x3f
	mov al,byte [edi+edx]
	mov byte [ebx+1],al

	mov ah,byte [esi+1]
	mov al,0
	shr eax,6
	and eax,0x3f
	mov al,byte [edi+eax]
	mov ah,'='
	mov word [ebx+2],ax

	pop eax
	jmp b64eexit
b64elop3:
	pop eax
	jmp b64eexit
b64elop4:
	movzx eax,byte [esi]
	mov dh,al
	shr eax,2
	mov al,byte [edi+eax]

	mov dl,0
	shr edx,4
	and edx,0x3f
	mov ah,byte [edi+edx]
	mov word [ebx],ax

	mov ax,'=='
	mov word [ebx+2],ax

	pop eax
	jmp b64eexit
b64eexit:
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

;IntOS Base64_Decrypt(const UInt8 *inBuff, IntOS inSize, UInt8 *outBuff);
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 inBuff
;24 inSize
;28 outbuff
	align 16
_Base64_Decrypt:
Base64_Decrypt:
	push ebp
	push ebx
	push esi
	push edi
	xor eax,eax
	mov ecx,dword [esp+24] ;inSize
	test ecx,3
	jnz b64dexit
	mov esi,dword [esp+20] ;inBuff
	mov edi,dword [esp+28] ;outBuff
	mov ebx,0
	mov edx,0
	cmp ecx,0
	jz b64dexit
	cmp edi,0
	jz b64dlop11
b64dlop:
	push eax
	movzx eax,byte [esi]
	inc esi
	mov dl,byte [eax+_Base64_decArr]
	pop eax
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
	inc eax
	mov byte [edi],dl
	inc edi
	jmp b64dlop2
b64dlop8:
	mov dh,dl
	mov ebx,1
	jmp b64dlop2
b64dlop9:
	mov ebx,2
	shl dl,2
	shl edx,2
	inc eax
	mov byte [edi],dh
	and edx,0xff
	inc edi
	shl edx,4
	jmp b64dlop2
b64dlop10:
	mov ebx,3
	shl dl,2
	shl edx,4
	inc eax
	mov byte [edi],dh
	and edx,0xff
	inc edi
	shl edx,2

b64dlop2:
	dec ecx
	jnz b64dlop
	jmp b64dexit

b64dlop11:
	movzx eax,byte [esi]
	mov dl,byte [eax+_Base64_decArr]
	inc esi
	cmp dl,0xff
	jz b64dlop13
	inc edx
b64dlop13:
	dec ecx
	jnz b64dlop
	mov eax,edx
	and edx,3
	shr eax,2
	dec edx
	lea eax,[eax+eax*2]
	add eax,edx
b64dexit:
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

