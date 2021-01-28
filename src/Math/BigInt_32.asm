section .text

global _BigInt_ByteSwap
global _BigInt_Neg
global _BigInt_Add
global _BigInt_And
global _BigInt_Or
global _BigInt_Xor
global _BigInt_AssignI32
global _BigInt_AssignStrW16
global _BigInt_MulUI32
global _BigInt_DivUI32
global _BigInt_ToStringW16

;void BigInt_ByteSwap(const UInt8 *srcBuff, UInt8 *destBuff, OSInt valSize)
;0 ebx
;4 retAddr
;8 srcBuff
;12 destBuff
;16 valSize
	align 16
_BigInt_ByteSwap:
	push ebx
	mov edx,dword [esp+8] ;srcBuff
	mov ebx,dword [esp+12] ;destBuff
	mov ecx,dword [esp+16] ;valSize
	add ebx,ecx
	shr ecx,2
	
	align 16
bibslop:
	lea ebx,[ebx-4]
	mov eax,dword [edx]
	bswap eax
	lea edx,[edx+4]
	mov dword [ebx],eax
	dec ecx
	jnz bibslop
	pop ebx
	ret

;void BigInt_Neg(UInt8 *valBuff, OSInt valSize);
;0 retAddr
;4 valBuff
;8 valSize
	align 16
_BigInt_Neg:
	mov edx,dword [esp+4] ;valBuff
	mov ecx,dword [esp+8] ;valSize
	shr ecx,2
	align 16
bineglop:
	not dword [edx]
	lea edx,[edx+4]
	dec ecx
	jnz bineglop
	mov edx,dword [esp+4] ;valBuff
	mov ecx,dword [esp+8] ;valSize
	shr ecx,2
	align 16
bineglop2:
	inc dword [edx]
	jnc bineglop3
	lea edx,[edx+4]
	dec ecx
	jnz bineglop2
	align 16
bineglop3:
	ret

;void BigInt_Add(UInt8 *destBuff, UInt8 *srcBuff, OSInt destSize, OSInt srcSize)
;0 edi
;4 esi
;8 ebx
;12 retAddr
;16 destBuff
;20 srcBuff
;24 destSize
;28 srcSize
	align 16
_BigInt_Add:
	push ebx
	push esi
	push edi
	mov edi,dword [esp+12] ;destBuff
	mov esi,dword [esp+16] ;srcBuff
	mov ecx,dword [esp+20] ;destSize
	mov edx,dword [esp+24] ;srcSize
	cmp ecx,edx
	jnb biaddlop
	xor eax,eax
	xor ebx,ebx
	shr ecx,2
	align 16
biaddlop2:
	add eax,dword [esi]
	setb bl
	add dword [edi],eax
	adc ebx,0
	lea esi,[esi+4]
	lea edi,[edi+4]
	mov eax,ebx
	dec ecx
	jnz biaddlop2
	jmp biaddexit
	
	align 16
biaddlop:
	sub ecx,edx
	xor eax,eax
	xor ebx,ebx
	shr edx,2
	
	align 16
biaddlop3:
	add eax,dword [esi]
	setb bl
	add dword [edi],eax
	adc ebx,0
	lea esi,[esi+4]
	lea edi,[edi+4]
	mov eax,ebx
	dec edx
	jnz biaddlop3
	
	test byte [esi-1],0x80
	jnz biaddlop4
	test eax,eax
	jz biaddexit
	shr ecx,2
	
	align 16
biaddlop5:
	inc dword [edi]
	jnb biaddexit
	lea edi,[edi+4]
	dec ecx
	jnz biaddlop5
	jmp biaddexit
	
	align 16
biaddlop4:
	test eax,eax
	jnz biaddexit
	shr ecx,2
	
	align 16
biaddlop6:
	add dword [edi],0xffffffff
	jb biaddexit
	lea edi,[edi+4]
	dec ecx
	jnz biaddlop6
	
	align 16
biaddexit:
	pop edi
	pop esi
	pop ebx
	ret

;void BigInt_And(UInt8 *destBuff, UInt8 *srcBuff, OSInt destSize, OSInt srcSize)
;0 edi
;4 esi
;8 retAddr
;12 destBuff
;16 srcBuff
;20 destSize
;24 srcSize
	align 16
_BigInt_And:
	push esi
	push edi
	mov edi,dword [esp+12] ;destBuff
	mov esi,dword [esp+16] ;srcBuff
	mov ecx,dword [esp+20] ;destSize
	mov edx,dword [esp+24] ;srcSize
	cmp ecx,edx
	jnb biandlop
	shr ecx,2
	
	align 16
biandlop1:
	lodsd
	and dword [edi],eax
	lea edi,[edi+4]
	dec ecx
	jnz biandlop1
	jmp biandexit
	
	align 16
biandlop:
	sub ecx,edx
	shr edx,2
	align 16
biandlop2:
	lodsd
	and dword [edi],eax
	lea edi,[edi+4]
	dec edx
	jnz biandlop2
	test eax,0x80000000
	jz biandexit
	mov eax,0xffffffff
	shr ecx,2
	align 16
biandlop3:
	and dword [edi],eax
	lea edi,[edi+4]
	dec ecx
	jnz biandlop3
	align 16
biandexit:
	pop edi
	pop esi
	ret

;void BigInt_Or(UInt8 *destBuff, UInt8 *srcBuff, OSInt destSize, OSInt srcSize)
;0 edi
;4 esi
;8 retAddr
;12 destBuff
;16 srcBuff
;20 destSize
;24 srcSize
	align 16
_BigInt_Or:
	push esi
	push edi
	mov edi,dword [esp+12] ;destBuff
	mov esi,dword [esp+16] ;srcBuff
	mov ecx,dword [esp+20] ;destSize
	mov edx,dword [esp+24] ;srcSize
	cmp ecx,edx
	jnb biorlop
	shr ecx,2
	
	align 16
biorlop1:
	lodsd
	or dword [edi],eax
	lea edi,[edi+4]
	dec ecx
	jnz biorlop1
	jmp biorexit
	
	align 16
biorlop:
	sub ecx,edx
	shr edx,2
	align 16
biorlop2:
	lodsd
	or dword [edi],eax
	lea edi,[edi+4]
	dec edx
	jnz biorlop2
	test eax,0x80000000
	jz biorexit
	mov eax,0xffffffff
	shr ecx,2
	align 16
biorlop3:
	or dword [edi],eax
	lea edi,[edi+4]
	dec ecx
	jnz biorlop3
	align 16
biorexit:
	pop edi
	pop esi
	ret

;void BigInt_Xor(UInt8 *destBuff, UInt8 *srcBuff, OSInt destSize, OSInt srcSize)
;0 edi
;4 esi
;8 retAddr
;12 destBuff
;16 srcBuff
;20 destSize
;24 srcSize
	align 16
_BigInt_Xor:
	push esi
	push edi
	mov edi,dword [esp+12] ;destBuff
	mov esi,dword [esp+16] ;srcBuff
	mov ecx,dword [esp+20] ;destSize
	mov edx,dword [esp+24] ;srcSize
	cmp ecx,edx
	jnb bixorlop
	shr ecx,2
	
	align 16
bixorlop1:
	lodsd
	xor dword [edi],eax
	lea edi,[edi+4]
	dec ecx
	jnz bixorlop1
	jmp bixorexit
	
	align 16
bixorlop:
	sub ecx,edx
	shr edx,2
	align 16
bixorlop2:
	lodsd
	xor dword [edi],eax
	lea edi,[edi+4]
	dec edx
	jnz bixorlop2
	test eax,0x80000000
	jz bixorexit
	mov eax,0xffffffff
	shr ecx,2
	align 16
bixorlop3:
	xor dword [edi],eax
	lea edi,[edi+4]
	dec ecx
	jnz bixorlop3
	align 16
bixorexit:
	pop edi
	pop esi
	ret

;void BigInt_AssignI32(UInt8 *valBuff, OSInt valSize, Int32 val);
;0 edi
;4 retAddr
;8 valBuff
;12 valSize
;16 val
	align 16
_BigInt_AssignI32:
	push edi
	mov edi,dword [esp+8] ;valBuff
	mov ecx,dword [esp+12] ;valSize
	shr ecx,2
	mov eax,dword [esp+16] ;val
	stosd
	dec ecx
	jz biei32exit
	sar eax,31
	rep stosd
	align 16
biei32exit:
	pop edi
	ret

;void BigInt_AssignStrW16(UInt8 *valBuff, OSInt valSize, const WChar *val);
;0 edi
;4 esi
;8 ebx
;12 ebp
;16 retAddr
;20 valBuff
;24 valSize
;28 val
	align 16
_BigInt_AssignStrW16:
	push ebp
	push ebx
	push esi
	push edi
	mov edi,dword [esp+20] ;valBuff
	mov ecx,dword [esp+24] ;valSize
	mov esi,dword [esp+28] ;val
	cmp word [esi],'0'
	jnz biesw16lop2
	cmp word [esi+2],'x'
	jnz biesw16lop2
	
	align 16
biesw16lop1a:
	cmp word [esi],0
	jz biesw16lop1b
	lea esi,[esi+2]
	jmp biesw16lop1a
	
	align 16
biesw16lop1b:
	lea esi,[esi-2]
	movzx eax,word [esi]
	cmp eax,0x30
	jb biesw16lop1c
	cmp eax,0x3a
	jb biesw16lop1d
	cmp eax,0x41
	jb biesw16lop1c
	cmp eax,0x47
	jb biesw16lop1e
	cmp eax,0x61
	jb biesw16lop1c
	cmp eax,0x67
	jnb biesw16lop1c
	lea edx,[eax-0x57]
	jmp biesw16lop1f
	
	align 16
biesw16lop1c:
	xor eax,eax
	rep stosb
	jmp biesw16exit

	align 16
biesw16lop1d:
	lea edx,[eax-0x30]
	jmp biesw16lop1f
	
	align 16
biesw16lop1e:
	lea edx,[eax-0x37]
	align 16
biesw16lop1f:
	lea esi,[esi-2]
	movzx eax,word [esi]
	cmp eax,0x30
	jb biesw16lop1g
	cmp eax,0x3a
	jb biesw16lop1h
	cmp eax,0x41
	jb biesw16lop1g
	cmp eax,0x47
	jb biesw16lop1i
	cmp eax,0x61
	jb biesw16lop1g
	cmp eax,0x67
	jnb biesw16lop1g
	lea ebx,[eax-0x57]
	jmp biesw16lop1j
	
	align 16
biesw16lop1g:
	mov eax,edx
	stosb
	dec ecx
	jz biesw16exit
	xor eax,eax
	rep stosb
	jmp biesw16exit

	align 16
biesw16lop1h:
	lea ebx,[eax-0x30]
	jmp biesw16lop1j
	
	align 16
biesw16lop1i:
	lea ebx,[eax-0x37]
	align 16
biesw16lop1j:
	shl bl,4
	or dl,bl
	mov eax,edx
	stosb
	dec ecx
	jnz biesw16lop1b
	jmp biesw16exit

	align 16
biesw16lop2:
	shr ecx,2
	xor eax,eax
	rep stosd
	cmp word [esi],'-'
	jnz biesw16lop2a
	lea esi,[esi+2]
	mov eax,1
	
	align 16
biesw16lop2a:
	mov dword [esp+28],eax
	mov ecx,10
	mov edi,dword [esp+20] ;valBuff
	
	align 16
biesw16lop2b:
	movzx eax,word [esi]
	lea esi,[esi+2]
	cmp eax,0x30
	jb biesw16lop2e
	cmp eax,0x39
	jnbe biesw16lop2e
	
	mov ebx,dword [esp+20] ;valBuff
	lea edx,[eax-0x30]
	align 16
biesw16lop2c:
	cmp edi,ebx
	jbe biesw16lop2d
	mov eax,dword [ebx]
	mov ebp,edx
	mul ecx
	add eax,ebp
	adc edx,0
	mov dword [ebx],eax

	lea ebx,[ebx+4]
	jmp biesw16lop2c
	
	align 16
biesw16lop2d:
	cmp edx,0
	jz biesw16lop2b
	mov eax,edi
	sub eax,dword [esp+20] ;valBuff
	cmp eax,dword [esp+24] ;valSize
	jnb biesw16exit
	mov dword [edi],edx
	lea edi,[edi+4]
	jmp biesw16lop2b
	
	align 16
biesw16lop2e:
	cmp dword [esp+28],1
	jnz biesw16exit
	mov eax,dword [esp+20] ;valBuff
	mov edx,dword [esp+24] ;valSize
	push edx
	push eax
	call _BigInt_Neg
	add esp,8
	
	align 16
biesw16exit:	
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret
	
;UInt32 BigInt_MulUI32(UInt8 *valBuff, OSInt valSize, UInt32 val);
;0 edi
;4 esi
;8 ebx
;12 retAddr
;16 valBuff
;20 valSize
;24 val
	align 16
_BigInt_MulUI32:
	push ebx
	push esi
	push edi
	mov edi,dword [esp+16] ;valBuff
	mov esi,dword [esp+20] ;valSize
	mov ebx,dword [esp+24] ;val
	shr esi,2
	xor edx,edx
	align 16
bimui32lop:
	mov ecx,edx
	mov eax,dword [edi]
	mul ebx
	add eax,ecx
	adc edx,0
	stosd
	dec esi
	jnz bimui32lop
	mov eax,edx

	pop edi
	pop esi
	pop ebx
	ret

;UInt32 BigInt_DivUI32(UInt8 *valBuff, OSInt valSize, UInt32 val);
;0 valBuff
;4 valSize
;8 ebp
;12 edi
;16 esi
;20 ebx
;24 retAddr
;28 valBuff
;32 valSize
;36 val
	align 16
_BigInt_DivUI32:
	push ebx
	push esi
	push edi
	push ebp
	push eax
	push eax
	
	mov edi,dword [esp+28] ;valBuff
	mov esi,dword [esp+32] ;valSize
	mov ebx,dword [esp+36] ;val
	mov dword [esp],edi
	mov dword [esp+4],esi
	add edi,esi
	shr esi,2
	test byte [edi-1],0x80
	jz bidui32lop2a
	mov ebp,1
	call _BigInt_Neg
	jmp bidui32lop2
	
	align 16
bidui32lop2a:
	xor ebp,ebp
	align 16
bidui32lop2:
	xor edx,edx
bidui32lop:
	lea edi,[edi-4]
	mov eax,dword [edi]
	div ebx
	mov dword [edi],eax
	dec esi
	jnz bidui32lop

	mov eax,ebp
	test ebp,ebp
	jz bidui32lop3
	mov ebp,edx
	call _BigInt_Neg
	mov eax,ebp

	align 16
bidui32lop3:
	add esp,8
	pop ebp
	pop edi
	pop esi
	pop ebx
	ret

;WChar *BigInt_ToStringW16(WChar *buff, UInt8 *valArr, UInt8 *tmpArr, OSInt valSize)
;0 val2
;4 val3
;8 ebp
;12 ebx
;16 edi
;20 esi
;24 retAddr
;28 buff
;32 valArr
;36 tmpArr
;40 valSize

	align 16
_BigInt_ToStringW16:
	push esi
	push edi
	push ebx
	push ebp
	push eax
	push eax
	mov esi,dword [esp+32] ;valArr
	mov edi,dword [esp+36] ;tmpArr
	mov ecx,dword [esp+40] ;valSize
	shr ecx,4
	align 16
bitsw16lop11:
	movaps xmm0,[esi]
	movaps [edi],xmm0
	lea esi,[esi+16]
	lea edi,[edi+16]
	dec ecx
	jnz bitsw16lop11
	
	test byte [edi-1],0x80
	jz bitsw16lop
	mov ebx,dword [esp+28] ;buff
	mov word [ebx],'-'
	mov ecx,dword [esp+40] ;valSize
	add dword [esp+28],2 ;buff
	lea ebx,[ebx+2]
	shr ecx,2
	align 16
bitsw16lop2:
	lea edi,[edi-4]
	not dword [edi]
	dec ecx
	jnz bitsw16lop2
	mov ecx,dword [esp+40] ;valSize
	shr ecx,2
	align 16
bitsw16lop3:
	inc dword [edi]
	jnb bitsw16lop
	lea edi,[edi+4]
	dec ecx
	jnz bitsw16lop3

	align 16
bitsw16lop:
	xor ebp,ebp ;val1
	mov ecx,dword [esp+40] ;valSize
	mov edi,dword [esp+36] ;tmpArr
	add edi,ecx
	shr ecx,2
	mov dword [esp+0],ecx ;val2
	mov dword [esp+4],edi ;val3
	align 16
bitsw16lop4:
	mov ecx,dword [esp+0] ;val2
	mov edi,dword [esp+4] ;val3
	align 16
bitsw16lop5:
	cmp dword [edi-4],0
	jnz bitsw16lop6
	lea edi,[edi-4]
	dec ecx
	jnz bitsw16lop5
	jmp bitsw16lop7
	
	align 16
bitsw16lop6:
	inc ebp ;val1
	mov dword [esp+0],ecx ;val2
	mov dword [esp+4],edi ;val3
	mov edx,0
	mov ebx,10
	align 16
bitsw16lop6b:
	sub edi,4
	mov eax,dword [edi]
	div ebx
	mov dword [edi],eax
	dec ecx
	jnz bitsw16lop6b
	jmp bitsw16lop4

	align 16
bitsw16lop7:
	test ebp,ebp ;val1
	jnz bitsw16lop8
	mov ebx,dword [esp+28] ;buff
	mov dword [ebx],'0'
	lea eax,[ebx+2]
	jmp bitsw16exit

	align 16
bitsw16lop8:
	mov esi,dword [esp+32] ;valArr
	mov edi,dword [esp+36] ;tmpArr
	mov ecx,dword [esp+40] ;valSize
	shr ecx,4
	align 16
bitsw16lop8a:
	movaps xmm0,[esi]
	movaps [edi],xmm0
	lea esi,[esi+16]
	lea edi,[edi+16]
	dec ecx
	jnz bitsw16lop8a
	test byte [edi-1],0x80
	jz bitsw16lop9
	mov ecx,dword [esp+40] ;valSize
	shr ecx,2
	align 16
bitsw16lop8b:
	lea edi,[edi-4]
	not dword [edi]
	dec ecx
	jnz bitsw16lop8b
	mov ecx,dword [esp+40] ;valSize
	shr ecx,2
bitsw16lop8c:
	inc dword [edi]
	jnb bitsw16lop9
	lea edi,[edi+4]
	dec ecx
	jnz bitsw16lop8c

	align 16
bitsw16lop9:
	mov esi,dword [esp+28] ;buff
	mov edi,dword [esp+36] ;tmpArr
	mov ecx,dword [esp+40] ;valSize
	lea esi,[esi+ebp*2] ;val1
	add edi,ecx
	shr ecx,2
	mov dword [esp+0],ecx ;val2
	mov dword [esp+4],edi ;val3
	mov ebp,esi ;val1
	mov word [esi],0
	align 16
bitsw16lop9b:
	mov ecx,dword [esp+0] ;val2
	mov edi,dword [esp+4] ;val3
	align 16
bitsw16lop9c:
	cmp dword [edi-4],0
	jnz bitsw16lop9d
	lea edi,[edi-4]
	dec ecx
	jnz bitsw16lop9c
	jmp bitsw16lop10
	
	align 16
bitsw16lop9d:
	mov dword [esp+4],edi ;val3
	mov dword [esp+0],ecx ;val2
	mov edx,0
	mov ebx,10
	align 16
bitsw16lop9e:
	lea edi,[edi-4]
	mov eax,dword [edi]
	div ebx
	mov dword [edi],eax
	dec ecx
	jnz bitsw16lop9e
	add edx,'0'
	mov word [esi-2],dx
	lea esi,[esi-2]
	jmp bitsw16lop9b
	
	align 16
bitsw16lop10:
	mov eax,ebp ;val1
	align 16
bitsw16exit:
	add esp,8
	pop ebp
	pop ebx
	pop edi
	pop esi
	ret
